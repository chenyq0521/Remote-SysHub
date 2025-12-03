#include "USNFileFinder.h"
#include <QDebug>
#include <QRegularExpression>

USNFileFinder::USNFileFinder(QObject *parent)
    : QObject(parent)
    , m_hVolume(INVALID_HANDLE_VALUE)
{
}

USNFileFinder::~USNFileFinder()
{
    clearIndex();
    if (m_hVolume != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hVolume);
    }
}

bool USNFileFinder::isNTFSVolume(const QString &driveLetter)
{
    DWORD fsFlags = 0;
    WCHAR fsName[MAX_PATH] = {0};
    if (!GetVolumeInformationW((LPCWSTR)(driveLetter + "\\").utf16(),
                               nullptr, 0, nullptr, nullptr, &fsFlags,
                               fsName, MAX_PATH)) {
        return false;
    }
    return QString::fromWCharArray(fsName).compare("NTFS", Qt::CaseInsensitive) == 0;
}

bool USNFileFinder::initialize(const QString &driveLetter)
{
    m_driveLetter = driveLetter.toUpper();
    QString volumePath = QString("\\\\.\\%1:").arg(m_driveLetter[0]);

    m_hVolume = CreateFileW(
        (LPCWSTR)volumePath.utf16(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        nullptr
        );

    if (m_hVolume == INVALID_HANDLE_VALUE) {
        qWarning() << "[USNFileFinder] 打开卷失败:" << volumePath;
        return false;
    }

    return buildIndex();
}

void USNFileFinder::clearIndex()
{
    m_usnIndex.clear();
}

bool USNFileFinder::buildIndex()
{
    if (m_hVolume == INVALID_HANDLE_VALUE) return false;

    DWORD bytesReturned = 0;
    MFT_ENUM_DATA med = {0};

    med.StartFileReferenceNumber = 0;
    med.LowUsn = 0;
    med.HighUsn = 0x7FFFFFFFFFFFFFFF;

    const DWORD bufferSize = 1024 * 1024; // 1MB
    QByteArray buffer;
    buffer.resize(bufferSize);

    while (true) {
        if (!DeviceIoControl(
                m_hVolume,
                FSCTL_ENUM_USN_DATA,
                &med,
                sizeof(med),
                buffer.data(),
                bufferSize,
                &bytesReturned,
                nullptr)) {
            DWORD err = GetLastError();
            if (err == ERROR_HANDLE_EOF) break;
            qWarning() << "[USNFileFinder] 枚举 USN 失败, Error:" << err;
            return false;
        }

        // 第一个8字节是 USN 记录起始 USN
        ULONGLONG* pNextUSN = (ULONGLONG*)buffer.data();
        char* ptr = buffer.data() + sizeof(ULONGLONG);
        char* end = buffer.data() + bytesReturned;

        while (ptr < end) {
            PUSN_RECORD record = (PUSN_RECORD)ptr;
            processUSNRecord(record);
            ptr += record->RecordLength;
        }

        med.StartFileReferenceNumber = *pNextUSN;
    }

    qDebug() << "[USNFileFinder] 索引构建完成，文件数量:" << m_usnIndex.size();
    return true;
}

void USNFileFinder::processUSNRecord(PUSN_RECORD record)
{
    if (record->FileNameLength == 0) return;

    USNFileInfo info;
    info.fileReferenceNumber = record->FileReferenceNumber;
    info.parentReferenceNumber = record->ParentFileReferenceNumber;
    info.fileAttributes = record->FileAttributes;

    // 文件名
    info.fileName = QString::fromWCharArray(
        (WCHAR*)((BYTE*)record + record->FileNameOffset),
        record->FileNameLength / sizeof(WCHAR)
        );

    // 构建完整路径（参考父引用号索引）
    QString fullPath = info.fileName;
    ULONGLONG parent = record->ParentFileReferenceNumber;
    while (parent != 0 && m_usnIndex.contains(parent)) {
        fullPath = m_usnIndex[parent].fileName + "\\" + fullPath;
        parent = m_usnIndex[parent].parentReferenceNumber;
    }
    info.fullPath = m_driveLetter + ":\\" + fullPath;

    // 文件大小和时间只能用额外 API 获取
    if (!(info.fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        WIN32_FILE_ATTRIBUTE_DATA attr;
        if (GetFileAttributesExW(info.fullPath.toStdWString().c_str(),
                                 GetFileExInfoStandard, &attr)) {
            LARGE_INTEGER li;
            li.HighPart = attr.nFileSizeHigh;
            li.LowPart = attr.nFileSizeLow;
            info.fileSize = li.QuadPart;

            FILETIME ft = attr.ftLastWriteTime;
            SYSTEMTIME st;
            FileTimeToSystemTime(&ft, &st);
            info.lastWriteTime = QDateTime(QDate(st.wYear, st.wMonth, st.wDay),
                                           QTime(st.wHour, st.wMinute, st.wSecond));
        }
    } else {
        info.fileSize = 0;
    }

    m_usnIndex[record->FileReferenceNumber] = info;
}

bool USNFileFinder::searchFiles(const QString &filePattern, QVector<USNFileInfo> &results)
{
    results.clear();

    if (m_usnIndex.isEmpty()) return false;

    QRegularExpression regex;
    QString pattern = filePattern;
    if (pattern.contains('*') || pattern.contains('?')) {
        pattern = QRegularExpression::wildcardToRegularExpression(pattern);
    } else {
        pattern = QRegularExpression::escape(pattern);
    }
    regex.setPattern(pattern);
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    for (const USNFileInfo &info : m_usnIndex) {
        if (regex.match(info.fileName).hasMatch()) {
            results.append(info);
            if (results.size() >= 1000) break; // 限制最多1000条
        }
    }

    return true;
}
