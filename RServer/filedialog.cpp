#include "filedialog.h"
#include "ui_filedialog.h"

#include <QDataStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QHeaderView>
#include <QInputDialog>
#include <QDir>
#include <QTimer>
#include <QBrush>

FileDialog::FileDialog(TcpWorker *worker, CONTEXT_OBJECT *ctx, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FileDialog)
    , m_worker(worker)
    , m_context(ctx)
    , m_currentPath("/")
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("文件管理");

    // 设置表格属性
    ui->treeWidget->setColumnCount(5);
    ui->treeWidget->setHeaderLabels({"名称", "大小", "类型", "修改日期", "权限"});
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->setIconSize(QSize(16, 16));

    // 设置列宽
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    // 初始化UI
    ui->diskCombo->addItem("正在获取盘符...");
    ui->diskCombo->setEnabled(false);
    ui->pathLabel->setText("准备连接...");
    ui->searchEdit->setText("");

    // 搜索框回车 = 点击搜索按钮
    connect(ui->searchEdit, &QLineEdit::returnPressed,
            this, &FileDialog::on_searchButton_clicked);
    // 请求盘符信息
    RequestDiskInfo();
}

FileDialog::~FileDialog()
{
    delete ui;
}

void FileDialog::RequestDiskInfo()
{
    if (!m_worker || !m_context) return;

    // 发送FILE_REQUIRE请求盘符
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    stream << (unsigned char)FILE_REQUIRE;

    m_context->payload = data;
    m_context->isToken = FILE_REQUIRE;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

    ui->pathLabel->setText("正在获取盘符信息...");
}

void FileDialog::HandlePacket(unsigned char isToken, const QByteArray &payload)
{
    QDataStream stream(payload);
    stream.setVersion(QDataStream::Qt_5_15);

    switch (isToken) {
    case FILE_REPLY:
        ParseDiskInfo(payload);
        break;
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

void FileDialog::ParseDiskInfo(const QByteArray &data)
{
    QDataStream stream(data);
    stream.setVersion(QDataStream::Qt_5_15);

    QList<FileInfo> driveList;
    stream >> driveList;

    qDebug() << "[FileDialog] Received drive list, count:" << driveList.size();

    // 清空并更新盘符组合框
    ui->diskCombo->clear();
    ui->diskCombo->setEnabled(true);

    if (driveList.isEmpty()) {
        ui->diskCombo->addItem("未找到盘符");
        ui->pathLabel->setText("无可用盘符");
        return;
    }

    // 添加盘符到组合框
    for (const FileInfo &drive : driveList) {
        QString displayName = drive.name;
        // 如果有大小信息，可以显示容量
        if (drive.size > 0) {
            QString sizeStr = formatSize(drive.size);
            displayName += QString(" (%1)").arg(sizeStr);
        }
        ui->diskCombo->addItem(displayName, drive.name); // 使用用户数据存储原始名称
    }

    // 如果有盘符，自动选择第一个并请求其文件列表
    if (!driveList.isEmpty()) {
        QString firstDrive = driveList.first().name;
        // 确保盘符格式正确，添加反斜杠
        if (!firstDrive.endsWith('\\') && !firstDrive.endsWith('/')) {
            firstDrive += '\\';
        }
        m_currentPath = firstDrive;

        // 更新路径显示
        ui->searchEdit->setText(firstDrive);
        ui->pathLabel->setText("正在加载文件列表...");

        // 请求该盘符的文件列表
        qDebug()<<"请求文件路径:"<<m_currentPath;
        SendFileRequest(firstDrive, FILE_LIST_REQUEST);
    }

    // 更新状态显示
    ui->pathLabel->setText(QString("已连接，共 %1 个驱动器").arg(driveList.size()));
}

void FileDialog::on_diskCombo_currentIndexChanged(int index)
{
    if (index < 0 || ui->diskCombo->count() == 0) return;

    QString drive = ui->diskCombo->itemData(index).toString();
    if (drive.isEmpty() || drive == "未找到盘符") {
        return;
    }

    // 确保盘符格式正确
    if (!drive.endsWith('\\') && !drive.endsWith('/')) {
        drive += '\\';
    }

    m_currentPath = drive;
    ui->searchEdit->setText(drive);
    ui->pathLabel->setText("正在加载文件列表...");

    // 请求该盘符的文件列表
    SendFileRequest(m_currentPath, FILE_LIST_REQUEST);
}
// 解析文件列表
void FileDialog::ParseFileList(const QByteArray &data)
{
    qDebug()<<"解析文件列表";
    QDataStream stream(data);
    stream.setVersion(QDataStream::Qt_5_15);

    unsigned char packetType;
    QString currentPath;
    QList<FileInfo> fileList;

    try {
        // 读取数据包类型和路径
        stream >> currentPath;

        // 检查是否为特殊情况（如"MyComputer"）
        if (currentPath == "MyComputer") {
            // 处理驱动器列表
            HandleDriveList(data);
            return;
        }

        // 读取文件列表
        stream >> fileList;

        qDebug() << "[FileDialog] Received file list for path:" << currentPath
                 << "file count:" << fileList.size();

        // 更新当前路径
        m_currentPath = currentPath;

        // 更新UI显示
        UpdateCurrentPath(currentPath);

        // 清空当前显示
        ClearFileList();

        // 如果不是根目录，添加".."上级目录项
        QDir dir(currentPath);
        if (dir.cdUp()) {
            // 创建上级目录项
            FileInfo parentDir;
            parentDir.name = "..";
            parentDir.isDirectory = true;
            parentDir.type = "上级目录";
            AddFileItem(parentDir);
        }

        // 添加文件项到表格
        for (const FileInfo &fileInfo : fileList) {
            AddFileItem(fileInfo);
        }

    } catch (const std::exception &e) {
        qWarning() << "[FileDialog] Error parsing file list:" << e.what();
        ui->treeWidget->clear();
        ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList() << "解析错误"));
    }
}

void FileDialog::HandleDriveList(const QByteArray &data)
{
    QDataStream stream(data);
    stream.setVersion(QDataStream::Qt_5_15);

    unsigned char packetType;
    QString path;
    QList<FileInfo> driveList;

    stream >> packetType >> path >> driveList;

    qDebug() << "[FileDialog] Received drive list, count:" << driveList.size();

    // 更新当前路径
    m_currentPath = "MyComputer";
    UpdateCurrentPath("我的电脑");

    // 清空当前显示
    ClearFileList();

    // 添加驱动器项到表格
    for (const FileInfo &driveInfo : driveList) {
        AddDriveItem(driveInfo);
    }
}

void FileDialog::UpdateCurrentPath(const QString &path)
{
    ui->pathLabel->setText(path);

}

void FileDialog::SendFileRequest(const QString &path, unsigned char requestType)
{
    if (!m_worker || !m_context) return;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);

    // 确保路径不为空
    QString requestPath = path;
    if (requestPath.isEmpty()) {
        requestPath = "C:\\";  // 默认路径
    }

    stream << path;

    m_context->payload = data;
    m_context->isToken = requestType;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

    // 显示加载提示
    ui->treeWidget->clear();
    QTreeWidgetItem *loadingItem = new QTreeWidgetItem(ui->treeWidget);
    loadingItem->setText(0, "加载中...");
    loadingItem->setIcon(0, QIcon(":/icons/loading.png"));
}

void FileDialog::AddFileItem(const FileInfo &fileInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

    // 第0列：名称
    QString displayName = fileInfo.name;
    if (displayName == "..") {
        // 上级目录特殊处理
        item->setText(0, displayName);
    } else {
        // 如果是隐藏文件，添加标记
        if (fileInfo.isHidden) {
            displayName = "[隐藏] " + displayName;
        }
        // 如果是系统文件，添加标记
        if (fileInfo.isSystem) {
            displayName = "[系统] " + displayName;
        }
        item->setText(0, displayName);
    }

    // 第1列：大小
    if (fileInfo.isDirectory) {
        if (fileInfo.name == "..") {
            item->setText(1, "");
        } else {
            // 格式化目录大小
            QString sizeStr = formatSize(fileInfo.size);
            if (fileInfo.size == 0) {
                sizeStr = "<文件夹>";
            }
            item->setText(1, sizeStr);
        }
    } else {
        item->setText(1, formatSize(fileInfo.size));
    }

    // 第2列：类型
    if (fileInfo.name == "..") {
        item->setText(2, "上级目录");
    } else {
        item->setText(2, fileInfo.type);
    }

    // 第3列：修改日期
    if (fileInfo.name == "..") {
        item->setText(3, "");
    } else if (fileInfo.modified.isValid()) {
        item->setText(3, fileInfo.modified.toString("yyyy-MM-dd hh:mm:ss"));
    } else {
        item->setText(3, "");
    }

    // 第4列：权限
    QString permissionStr;
    if (!fileInfo.isDirectory || fileInfo.name == "..") {
        permissionStr = "";
    } else {
        permissionStr = fileInfo.permissions;
        if (fileInfo.isReadOnly) permissionStr += " R/O";
        if (fileInfo.isHidden) permissionStr += " 隐藏";
        if (fileInfo.isSystem) permissionStr += " 系统";
    }
    item->setText(4, permissionStr);

    // 设置图标
    QIcon icon;
    if (fileInfo.name == "..") {
        // 上级目录图标
        icon = QApplication::style()->standardIcon(QStyle::SP_FileDialogToParent);
    } else if (fileInfo.isDirectory) {
        icon = m_iconProvider.icon(QFileIconProvider::Folder);
    } else {
        // 根据文件后缀设置图标
        QString suffix = fileInfo.suffix.toLower();
        if (suffix == "exe" || suffix == "bat" || suffix == "cmd") {
            icon = m_iconProvider.icon(QFileIconProvider::Desktop);
        } else if (suffix == "txt" || suffix == "log" || suffix == "ini") {
            icon = m_iconProvider.icon(QFileIconProvider::File);
        } else if (suffix == "jpg" || suffix == "png" || suffix == "bmp" ||
                   suffix == "gif" || suffix == "ico") {
            icon = m_iconProvider.icon(QFileIconProvider::File);
        } else {
            icon = m_iconProvider.icon(QFileIconProvider::File);
        }
    }
    item->setIcon(0, icon);

    // 存储完整路径
    QString fullPath;
    if (fileInfo.name == "..") {
        QDir dir(m_currentPath);
        if (dir.cdUp()) {
            fullPath = dir.absolutePath();
        } else {
            fullPath = "";
        }
    } else {
        fullPath = m_currentPath;
        if (!fullPath.endsWith('/') && !fullPath.endsWith('\\')) {
            fullPath += '\\';
        }
        fullPath += fileInfo.name;
    }

    item->setData(0, Qt::UserRole, fullPath);
    item->setData(0, Qt::UserRole + 1, fileInfo.isDirectory);
    item->setData(0, Qt::UserRole + 2, fileInfo.isHidden);
    item->setData(0, Qt::UserRole + 3, fileInfo.isReadOnly);

    // 设置文本颜色（隐藏文件用灰色）
    if (fileInfo.isHidden && fileInfo.name != "..") {
        QColor grayColor(128, 128, 128);
        item->setForeground(0, QBrush(grayColor));
        item->setForeground(1, QBrush(grayColor));
        item->setForeground(2, QBrush(grayColor));
        item->setForeground(3, QBrush(grayColor));
    }

    // 只读文件用斜体
    if (fileInfo.isReadOnly && !fileInfo.isDirectory) {
        QFont font = item->font(0);
        font.setItalic(true);
        item->setFont(0, font);
    }
}

void FileDialog::AddDriveItem(const FileInfo &driveInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

    // 第0列：驱动器名称
    QString displayName = driveInfo.name;
    if (driveInfo.isHidden) {
        displayName = "[隐藏] " + displayName;
    }
    item->setText(0, displayName);

    // 第1列：容量信息
    QString sizeInfo;
    if (driveInfo.size > 0) {
        qint64 freeSpace = driveInfo.size - driveInfo.diskSize;
        QString totalSize = formatSize(driveInfo.size);
        QString usedSize = formatSize(driveInfo.diskSize);
        QString freeSize = formatSize(freeSpace);

        double usedPercent = (double)driveInfo.diskSize / driveInfo.size * 100;
        sizeInfo = QString("已用: %1 / %2 (%.1f%%)").arg(usedSize).arg(totalSize).arg(usedPercent);
    } else {
        sizeInfo = "未知容量";
    }
    item->setText(1, sizeInfo);

    // 第2列：类型
    item->setText(2, driveInfo.type);

    // 第3列：修改日期（驱动器没有修改日期）
    item->setText(3, "");

    // 第4列：权限
    QString permissionStr = driveInfo.permissions;
    if (driveInfo.isReadOnly) permissionStr += " R/O";
    item->setText(4, permissionStr);

    // 设置图标
    QIcon icon = m_iconProvider.icon(QFileIconProvider::Drive);
    item->setIcon(0, icon);

    // 存储完整路径
    QString fullPath = driveInfo.name;
    if (!fullPath.endsWith('\\') && !fullPath.endsWith('/')) {
        fullPath += '\\';
    }

    item->setData(0, Qt::UserRole, fullPath);
    item->setData(0, Qt::UserRole + 1, true); // 驱动器也是目录
    item->setData(0, Qt::UserRole + 2, driveInfo.isHidden);
    item->setData(0, Qt::UserRole + 3, driveInfo.isReadOnly);
}

void FileDialog::ClearFileList()
{
    ui->treeWidget->clear();
    m_pathItems.clear();
}

QString FileDialog::formatSize(qint64 bytes)
{
    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    const double GB = MB * 1024.0;
    const double TB = GB * 1024.0;

    if (bytes == 0) return "0 字节";

    if (bytes < KB) {
        return QString("%1 字节").arg(bytes);
    } else if (bytes < MB) {
        return QString("%1 KB").arg(bytes / KB, 0, 'f', 1);
    } else if (bytes < GB) {
        return QString("%1 MB").arg(bytes / MB, 0, 'f', 1);
    } else if (bytes < TB) {
        return QString("%1 GB").arg(bytes / GB, 0, 'f', 2);
    } else {
        return QString("%1 TB").arg(bytes / TB, 0, 'f', 2);
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
    stream.setVersion(QDataStream::Qt_5_15);
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
            qDebug()<<"请求返回目录"<<dir.absolutePath();
            if (dir.cdUp()) {
                SendFileRequest(dir.absolutePath(), FILE_LIST_REQUEST);
            }
        } else {
            // 进入子目录
            qDebug()<<"请求进入目录："<<path;
            SendFileRequest(path, FILE_LIST_REQUEST);
        }
    } else {
        // 如果是文件，可以在这里处理文件打开等操作
        qDebug() << "文件被双击:" << path;
        // TODO: 可以添加文件预览、下载等功能的调用
    }
}

void FileDialog::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (!current) {
        ui->deleteButton->setEnabled(false);
        return;
    }

    QString fileName = current->text(0);
    ui->deleteButton->setEnabled(fileName != ".." && fileName != "加载中...");
}

//搜索按钮点击事件
void FileDialog::on_searchButton_clicked()
{
    QString curtext = ui->searchEdit->text();
    //回到当前目录
    if(curtext.isEmpty()){
        SendFileRequest(m_currentPath, FILE_LIST_REQUEST);
        return;
    }
    SearchFile(curtext);
}
void FileDialog::SearchFile(QString fileName){
    if(fileName.isEmpty()){
        return ;
    }
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);


    stream << fileName;

    m_context->payload = data;
    m_context->isToken = FILE_SEARCH_REQUEST;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

    // 显示加载提示
    ui->treeWidget->clear();
    QTreeWidgetItem *loadingItem = new QTreeWidgetItem(ui->treeWidget);
    loadingItem->setText(0, "加载中...");
    loadingItem->setIcon(0, QIcon(":/icons/loading.png"));

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
        fullPath += '\\';
    }
    fullPath += folderName;

    // 发送创建文件夹请求
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    stream << FILE_NEWFOLDER_REQUEST << fullPath;

    m_context->payload = data;
    m_context->isToken = FILE_NEWFOLDER_REQUEST;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}
