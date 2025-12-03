#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "absmanager.h"
#include "USNFileFinder.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QMutex>
#include <QFileSystemWatcher>
#include <QUrl>
#include <QDesktopServices>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

// 文件信息结构
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

class FileManager : public AbsManager
{
    Q_OBJECT

public:
    explicit FileManager(TcpClient *TClient, QObject *parent = nullptr);
    ~FileManager();

    virtual void HandlePacket(const PACKET& packet) override;

signals:
    void searchProgress(int current, int total, const QString &currentFile);

private slots:
    void cancelSearch();

private:
    // 文件操作函数
    void handleFileListRequest(const QByteArray &payload);
    void handleFileSearchRequest(const QByteArray &payload);
    void handleFileInfoRequest(const QByteArray &payload);
    void handleFileDeleteRequest(const QByteArray &payload);
    void handleFileExecuteRequest(const QByteArray &payload);
    void handleNewFolderRequest(const QByteArray &payload);
    void handleRenameRequest(const QByteArray &payload);
    void handleDiskRequest(const QByteArray &payload);

    // 工具函数
    QList<FileInfo> getDirectoryContents(const QString &path);
    QList<FileInfo> searchFiles(const QString &params);
    FileInfo getDetailedFileInfo(const QString &filePath);
    QString getFileType(const QFileInfo &fileInfo);
    QString getFileTypeByExtension(const QString &extension);
    QString getFilePermissions(const QFileInfo &fileInfo);
    QString getFileOwner(const QString &filePath);
    qint64 getFileDiskSize(const QString &filePath);
    qint64 getDirectorySize(const QString &path);
    QString formatFileSize(qint64 bytes);
    // 搜索辅助函数
    QList<FileInfo> searchFilesWithUSN(const QString &driveLetter, const QString &fileNamePattern);
    QList<FileInfo> searchFilesWithTraditional(const QString &path, const QString &fileNamePattern);
    FileInfo convertUSNToFileInfo(const USNFileInfo &usnInfo);
    bool matchPattern(const QString &fileName, const QString &pattern);
    USNFileFinder* getOrCreateUSNFinder(const QString &driveLetter);

    // 文件系统监控
    void startWatching(const QString &path);
    void stopWatching();
    void onDirectoryChanged(const QString &path);

    // 搜索控制
    QMutex m_searchMutex;
    bool m_searchCancelled;
    QFileSystemWatcher *m_watcher;
    QFuture<void> m_searchFuture;
     QMap<QString, USNFileFinder*> m_usnFinders; // 按盘符存储查找器

    // 发送回复的辅助函数
    void sendFileListReply(const QString &path, const QList<FileInfo> &fileList);
    void sendSearchReply(const QList<FileInfo> &searchResults, bool completed);
    void sendErrorReply(quint8 token, const QString &error);

    // 辅助方法 - 封装发送数据包
    void sendPacket(quint8 token, const QByteArray &data);
};

#endif // FILEMANAGER_H
