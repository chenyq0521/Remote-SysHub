#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QSplitter>
#include <QFileIconProvider>
#include <QInputDialog>
#include "common.h"
#include "tcpworker.h"

// 前向声明需要放在正确的命名空间中
namespace Ui {
class FileDialog;  // Ui 命名空间中的类
}
struct FileInfo {
    QString name;
    qint64 size;
    QString type;
    QString suffix;
    QDateTime modified;
    QDateTime created;
    QDateTime accessed;
    bool isDirectory;
    bool isHidden;
    bool isSystem;
    bool isReadOnly;
    bool isSymbolicLink;
    bool isExecutable;
    QString permissions;
    QString owner;
    qint64 diskSize;

    friend QDataStream &operator<<(QDataStream &out, const FileInfo &info) {
        out << info.name << info.size << info.type << info.suffix
            << info.modified << info.created << info.accessed
            << info.isDirectory << info.isHidden << info.isSystem
            << info.isReadOnly << info.isSymbolicLink << info.isExecutable
            << info.permissions << info.owner << info.diskSize;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, FileInfo &info) {
        in >> info.name >> info.size >> info.type >> info.suffix
            >> info.modified >> info.created >> info.accessed
            >> info.isDirectory >> info.isHidden >> info.isSystem
            >> info.isReadOnly >> info.isSymbolicLink >> info.isExecutable
            >> info.permissions >> info.owner >> info.diskSize;
        return in;
    }
};
// 注意：这个 FileDialog 是全局命名空间中的类
class FileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileDialog(TcpWorker *worker, CONTEXT_OBJECT *ctx, QWidget *parent = nullptr);
    ~FileDialog();

    void HandlePacket(unsigned char isToken, const QByteArray &payload);

//自动连接事件槽函数！
private slots:
    void on_refreshButton_clicked();
    void on_deleteButton_clicked();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_pathLineEdit_returnPressed();
    void on_backButton_clicked();
    void on_newFolderButton_clicked();
    void on_diskCombo_currentIndexChanged(int index);

private:
    void RequestDiskInfo();
    void ParseDiskInfo(const QByteArray &data);
    void ParseFileList(const QByteArray &data);
    void UpdateCurrentPath(const QString &path);
    void SendFileRequest(const QString &path, unsigned char requestType);
    void AddFileItem(const QString &name, const QString &size, const QString &type,
                     const QString &modified, bool isDir);
    void ClearFileList();
    QString formatSize(qint64 bytes);

    // 使用正确的命名空间
    Ui::FileDialog *ui;  // 这是 Ui::FileDialog 类型

    TcpWorker *m_worker;
    CONTEXT_OBJECT *m_context;
    QString m_currentPath;
    QHash<QString, QTreeWidgetItem*> m_pathItems;
    QFileIconProvider m_iconProvider;

    // UI更新函数
    void AddFileItem(const FileInfo &fileInfo);
    void AddDriveItem(const FileInfo &driveInfo);
    void HandleDriveList(const QByteArray &data);
};

#endif // FILEDIALOG_H
