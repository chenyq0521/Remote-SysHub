#pragma once
#include <QObject>
#include <QMap>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <windows.h>
#include <winioctl.h>

// USN 文件信息结构
struct USNFileInfo {
    QString fileName;
    QString fullPath;
    quint64 fileSize;
    DWORD fileAttributes;
    QDateTime lastWriteTime;
    ULONGLONG fileReferenceNumber; // 文件引用号，用于索引
    ULONGLONG parentReferenceNumber; // 父目录引用号
};

// USN 文件查找器
class USNFileFinder : public QObject
{
    Q_OBJECT
public:
    explicit USNFileFinder(QObject *parent = nullptr);
    ~USNFileFinder();

    // 初始化 USN 查找器
    bool initialize(const QString &driveLetter);

    // 是否是 NTFS 卷
    static bool isNTFSVolume(const QString &driveLetter);

    // 搜索文件
    bool searchFiles(const QString &filePattern, QVector<USNFileInfo> &results);

private:
    bool buildIndex();
    void clearIndex();
    void processUSNRecord(PUSN_RECORD record);

private:
    QString m_driveLetter;
    HANDLE m_hVolume;
    QMap<ULONGLONG, USNFileInfo> m_usnIndex; // key: 文件引用号
};
