#include "filedialog.h"
#include "ui_filedialog.h"

#include <QDataStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QHeaderView>
#include <QInputDialog>
#include <QDir>


// 文件信息结构
struct FileInfo {
    QString name;
    qint64 size;
    QString type;
    QDateTime modified;
    bool isDirectory;
    QString permissions;

    friend QDataStream &operator<<(QDataStream &out, const FileInfo &info) {
        out << info.name << info.size << info.type << info.modified
            << info.isDirectory << info.permissions;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, FileInfo &info) {
        in >> info.name >> info.size >> info.type >> info.modified
            >> info.isDirectory >> info.permissions;
        return in;
    }
};

FileDialog::FileDialog(TcpWorker *worker, CONTEXT_OBJECT *ctx, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FileDialog)
    , m_worker(worker)
    , m_context(ctx)
    , m_currentPath("/")
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("远程文件管理");

    // 设置表格属性
    ui->treeWidget->setColumnCount(5);
    ui->treeWidget->setHeaderLabels({"名称", "大小", "类型", "修改日期", "权限"});
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->setIconSize(QSize(16, 16));

    // 连接信号槽
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked,
            this, &FileDialog::on_treeWidget_itemDoubleClicked);
    connect(ui->treeWidget, &QTreeWidget::currentItemChanged,
            this, &FileDialog::on_treeWidget_currentItemChanged);

    // 请求根目录文件列表
    SendFileRequest(m_currentPath, FILE_LIST_REQUEST);
}

FileDialog::~FileDialog()
{
    delete ui;
}

void FileDialog::HandlePacket(unsigned char isToken, const QByteArray &payload)
{
    QDataStream stream(payload);
    switch (isToken) {
    case FILE_LIST_REPLY:
        ParseFileList(payload);
        break;
    case FILE_DELETE_REPLY: {
        bool success;
        QString message;
        stream >> success >> message;

        QMessageBox::information(this, success ? "成功" : "错误", message);
        if (success) {
            SendFileRequest(m_currentPath, FILE_LIST_REQUEST);
        }
        break;
    }

    case FILE_NEWFOLDER_REPLY: {
        bool success;
        QString message;
        stream >> success >> message;

        QMessageBox::information(this, success ? "成功" : "错误", message);
        if (success) {
            SendFileRequest(m_currentPath, FILE_LIST_REQUEST);
        }
        break;
    }

    default:
        qDebug() << "[FileDialog] Unknown packet type:" << isToken;
        break;
    }
}

void FileDialog::ParseFileList(const QByteArray &data)
{
    QDataStream stream(data);
    unsigned char packetType;
    QString currentPath;
    QList<FileInfo> fileList;

    stream >> packetType >> currentPath;
    //TODO 需要正确序列化FileInfo列表

    m_currentPath = currentPath;
    UpdateCurrentPath(currentPath);

    ClearFileList();

    // 添加".."上级目录项（如果不是根目录）
    if (m_currentPath != "/" && m_currentPath != "C:\\" && m_currentPath != "C:/") {
        AddFileItem("..", "", "上级目录", "", true);
    }

    // 临时添加示例数据（实际应从流中读取）
    // 这里需要根据实际的序列化方式修改
    // TODO: 从 stream 中正确读取 fileList

    // 示例：添加一些测试项目
    AddFileItem("Documents", "", "文件夹", "2024-01-15", true);
    AddFileItem("test.txt", "1.2 KB", "文本文档", "2024-01-14", false);
}

void FileDialog::UpdateCurrentPath(const QString &path)
{
    ui->pathLineEdit->setText(path);

    // 更新按钮状态
    ui->backButton->setEnabled(path != "/" && path != "C:\\" && path != "C:/");
}

void FileDialog::SendFileRequest(const QString &path, unsigned char requestType)
{
    if (!m_worker || !m_context) return;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << requestType << path;

    m_context->payload = data;
    m_context->isToken = requestType;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

    // 显示加载提示
    ui->treeWidget->clear();
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList() << "加载中..."));
}

void FileDialog::AddFileItem(const QString &name, const QString &size,
                             const QString &type, const QString &modified, bool isDir)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, name);
    item->setText(1, size);
    item->setText(2, type);
    item->setText(3, modified);

    // 设置图标
    QIcon icon = isDir ? m_iconProvider.icon(QFileIconProvider::Folder)
                       : m_iconProvider.icon(QFileIconProvider::File);
    item->setIcon(0, icon);

    // 存储完整路径
    QString fullPath = m_currentPath;
    if (!fullPath.endsWith('/') && !fullPath.endsWith('\\')) {
        fullPath += '/';
    }
    fullPath += name;
    item->setData(0, Qt::UserRole, fullPath);
    item->setData(0, Qt::UserRole + 1, isDir);
}

void FileDialog::ClearFileList()
{
    ui->treeWidget->clear();
    m_pathItems.clear();
}

QString FileDialog::FormatSize(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (bytes >= GB) {
        return QString("%1 GB").arg(bytes / (double)GB, 0, 'f', 1);
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(bytes / (double)MB, 0, 'f', 1);
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(bytes / (double)KB, 0, 'f', 1);
    } else {
        return QString("%1 字节").arg(bytes);
    }
}

// ============== 槽函数实现 ==============

void FileDialog::on_refreshButton_clicked()
{
    SendFileRequest(m_currentPath, FILE_LIST_REQUEST);
}

void FileDialog::on_deleteButton_clicked()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "提示", "请选择要删除的文件或文件夹");
        return;
    }

    QString fileName = item->text(0);
    if (fileName == "..") {
        QMessageBox::warning(this, "提示", "无法删除上级目录");
        return;
    }

    int ret = QMessageBox::question(this, "确认删除",
                                    QString("确定要删除 '%1' 吗？").arg(fileName));
    if (ret != QMessageBox::Yes) return;

    QString filePath = item->data(0, Qt::UserRole).toString();

    // 发送删除请求
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << FILE_DELETE_REQUEST << filePath;

    m_context->payload = data;
    m_context->isToken = FILE_DELETE_REQUEST;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}

void FileDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if (!item) return;

    bool isDir = item->data(0, Qt::UserRole + 1).toBool();
    QString path = item->data(0, Qt::UserRole).toString();

    if (isDir) {
        if (item->text(0) == "..") {
            // 返回上级目录
            QDir dir(m_currentPath);
            dir.cdUp();
            SendFileRequest(dir.absolutePath(), FILE_LIST_REQUEST);
        } else {
            // 进入子目录
            SendFileRequest(path, FILE_LIST_REQUEST);
        }
    } else {
        // 如果是文件，选中文件
    }
}

void FileDialog::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (!current) {
        ui->deleteButton->setEnabled(false);
        return;
    }

    bool isDir = current->data(0, Qt::UserRole + 1).toBool();
    QString fileName = current->text(0);

    ui->deleteButton->setEnabled(fileName != "..");
}

void FileDialog::on_pathLineEdit_returnPressed()
{
    QString newPath = ui->pathLineEdit->text().trimmed();
    if (!newPath.isEmpty() && newPath != m_currentPath) {
        SendFileRequest(newPath, FILE_LIST_REQUEST);
    }
}

void FileDialog::on_backButton_clicked()
{
    if (m_currentPath == "/" || m_currentPath == "C:\\" || m_currentPath == "C:/") {
        return;
    }

    QDir dir(m_currentPath);
    dir.cdUp();
    SendFileRequest(dir.absolutePath(), FILE_LIST_REQUEST);
}

void FileDialog::on_newFolderButton_clicked()
{
    bool ok;
    QString folderName = QInputDialog::getText(this, "新建文件夹",
                                               "请输入文件夹名称:",
                                               QLineEdit::Normal,
                                               "", &ok);
    if (!ok || folderName.isEmpty()) return;

    // 构建完整路径
    QString fullPath = m_currentPath;
    if (!fullPath.endsWith('/') && !fullPath.endsWith('\\')) {
        fullPath += '/';
    }
    fullPath += folderName;

    // 发送创建文件夹请求
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << FILE_NEWFOLDER_REQUEST << fullPath;

    m_context->payload = data;
    m_context->isToken = FILE_NEWFOLDER_REQUEST;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}
