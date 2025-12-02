#include "filemanager.h"
#include <QDirIterator>
#include <QDateTime>
#include <QProcess>
#include <QRegularExpression>
#include <windows.h>
#include <aclapi.h>
#include <lmcons.h>

// 启用备份权限函数（保持与项目其他部分一致）
bool EnableSeDebugPrivilege(HANDLE processHandle, BOOL enable, const wchar_t* privilegeName)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(processHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;

    LookupPrivilegeValueW(NULL, privilegeName, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

    bool result = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
    CloseHandle(hToken);

    return (result && GetLastError() == ERROR_SUCCESS);
}

FileManager::FileManager(TcpClient *TClient, QObject *parent)
    : AbsManager(TClient, parent)
    , m_searchCancelled(false)
    , m_watcher(nullptr)
{
    qDebug() << "[FileManager] Created";
    // 连接成功后立即发送 FILE_REPLY
    connect(m_client, &TcpClient::connected, this, [=]() {
        qDebug() << "[FileManager] Connected to server, sending FILE_REPLY";
        // 发送回复
        m_client->SendReply(FILE_REPLY);
        qDebug() << "[FileManager] FILE_REPLY sent with token:" << FILE_REPLY;
    });
}
FileManager::~FileManager()
{
    cancelSearch(); // 取消正在进行的搜索
    stopWatching();
    delete m_watcher;
    EnableSeDebugPrivilege(GetCurrentProcess(), FALSE, SE_BACKUP_NAME);
}

void FileManager::HandlePacket(const PACKET& packet)
{
    qDebug() << "[FileManager] Handling packet, token:" << packet.isToken;

    switch (packet.isToken) {
    case FILE_REQUIRE:
        handleDiskRequest(packet.payload);
        break;
    case FILE_LIST_REQUEST:
        handleFileListRequest(packet.payload);
        break;
    case FILE_SEARCH_REQUEST:
        handleFileSearchRequest(packet.payload);
        break;
    case FILE_DELETE_REQUEST:
        handleFileDeleteRequest(packet.payload);
        break;
    case FILE_NEWFOLDER_REQUEST:
        handleNewFolderRequest(packet.payload);
        break;
    default:
        qDebug() << "[FileManager] Unknown token:" << packet.isToken;
        sendErrorReply(packet.isToken, QString("未知操作类型: %1").arg(packet.isToken));
        break;
    }
}

void FileManager::handleDiskRequest(const QByteArray &payload)
{
    qDebug() << "[FileManager] Handling disk request";

    // 获取Windows盘符列表
    QList<FileInfo> drives;
    QFileInfoList driveList = QDir::drives();

    for (const QFileInfo &drive : driveList) {
        FileInfo info;
        info.name = drive.absoluteFilePath();
        // 移除末尾的斜杠，如 "C:/" -> "C:"
        if (info.name.endsWith('/') || info.name.endsWith('\\')) {
            info.name = info.name.left(info.name.length() - 1);
        }
        info.type = "驱动器";
        info.isDirectory = true;

        // 获取驱动器信息
        ULARGE_INTEGER freeBytes, totalBytes;
        if (GetDiskFreeSpaceExW(drive.absoluteFilePath().toStdWString().c_str(),
                                &freeBytes, &totalBytes, NULL)) {
            info.size = totalBytes.QuadPart;
            info.diskSize = totalBytes.QuadPart - freeBytes.QuadPart;
        }

        drives.append(info);
    }

    qDebug() << "[FileManager] Available drives:" << drives.size();

    // 发送驱动器列表回复
    QByteArray replyData;
    QDataStream replyStream(&replyData, QIODevice::WriteOnly);
    replyStream << drives;  // 只发送驱动器列表

    sendPacket(FILE_REPLY, replyData);
}
//发送目录下的文件List
void FileManager::handleFileListRequest(const QByteArray &payload)
{
    QDataStream stream(payload);
    QString path;
    stream >> path;

    qDebug() << "[FileManager] File list request for path:" << path;
    qDebug() << "[FileManager] 获得目录 time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 检查是否是特殊路径
    if (path == "MyComputer" || path.isEmpty()) {
        // 不应该在这里处理，应该由handleDiskRequest处理
        sendErrorReply(FILE_LIST_REPLY, "请使用FILE_REQUIRE请求驱动器列表");
        return;
    }

    QDir dir(path);
    if (!dir.exists()) {
        sendErrorReply(FILE_LIST_REPLY, QString("目录不存在: %1").arg(path));
        return;
    }


    // 获取目录内容
    QList<FileInfo> fileList = getDirectoryContents(path);

    // 发送文件列表回复
    QByteArray replyData;
    QDataStream replyStream(&replyData, QIODevice::WriteOnly);
    replyStream << path << fileList;

    qDebug() << "[FileManager] 发送目录前 time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    sendPacket(FILE_LIST_REPLY, replyData);
}

void FileManager::handleFileSearchRequest(const QByteArray &payload)
{
    // 取消之前的搜索
    cancelSearch();

    QMutexLocker locker(&m_searchMutex);
    m_searchCancelled = false;

    QDataStream stream(payload);
    SearchParams params;
    stream >> params;

    qDebug() << "[FileManager] File search request:"
             << "path:" << params.path
             << "keyword:" << params.keyword
             << "subfolders:" << params.searchSubfolders;

    if (!QDir(params.path).exists()) {
        sendErrorReply(FILE_SEARCH_REPLY, QString("搜索路径不存在: %1").arg(params.path));
        return;
    }

    // 异步搜索
    m_searchFuture = QtConcurrent::run([this, params]() {
        QList<FileInfo> results = searchFiles(params);

        if (!m_searchCancelled) {
            sendSearchReply(results, true);
        }
    });
}

void FileManager::handleFileDeleteRequest(const QByteArray &payload)
{
    QDataStream stream(payload);
    QString filePath;
    stream >> filePath;

    qDebug() << "[FileManager] File delete request:" << filePath;

    QFileInfo fileInfo(filePath);
    bool success = false;
    QString message;

    if (!fileInfo.exists()) {
        message = "文件或文件夹不存在";
    } else if (fileInfo.isDir()) {
        QDir dir(filePath);
        success = dir.removeRecursively();
        message = success ? "文件夹删除成功" : "文件夹删除失败";
    } else {
        // 检查文件是否只读
        QFile file(filePath);
        QFile::Permissions perms = file.permissions();
        if (!(perms & QFile::WriteUser)) {
            // 如果是只读文件，修改权限
            file.setPermissions(perms | QFile::WriteUser);
        }

        success = file.remove();
        message = success ? "文件删除成功" : "文件删除失败";
    }

    qDebug() << "[FileManager] Delete result:" << message;

    QByteArray replyData;
    QDataStream replyStream(&replyData, QIODevice::WriteOnly);
    replyStream << success << message;

    sendPacket(FILE_DELETE_REPLY, replyData);
}

void FileManager::handleNewFolderRequest(const QByteArray &payload)
{
    QDataStream stream(payload);
    QString folderPath;
    stream >> folderPath;

    qDebug() << "[FileManager] New folder request:" << folderPath;

    QDir dir;
    bool success = dir.mkpath(folderPath);
    QString message = success ? "文件夹创建成功" : "文件夹创建失败";

    qDebug() << "[FileManager] New folder result:" << message;

    QByteArray replyData;
    QDataStream replyStream(&replyData, QIODevice::WriteOnly);
    replyStream << success << message;

    sendPacket(FILE_NEWFOLDER_REPLY, replyData);
}

/*
QList<FileInfo> FileManager::getDirectoryContents(const QString &path)
{
    QList<FileInfo> fileList;
    QDir dir(path);

    // 添加上级目录（如果不是根目录）
    if (dir.cdUp()) {
        FileInfo parent;
        parent.name = "..";
        parent.type = "上级目录";
        parent.isDirectory = true;
        fileList.append(parent);
    }

    // 获取所有条目
    qDebug() << "[FileManager] 目录信息搜索前 time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QFileInfoList entries = dir.entryInfoList(
        QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System,
        QDir::DirsFirst | QDir::Name | QDir::IgnoreCase);

    for (const QFileInfo &entry : entries) {
        FileInfo info;
        info.name = entry.fileName();
        info.suffix = entry.suffix().toLower();
        info.size = entry.isDir() ? getDirectorySize(entry.absoluteFilePath()) : entry.size();
        info.type = getFileType(entry);
        info.modified = entry.lastModified();
        info.created = entry.birthTime();
        info.accessed = entry.lastRead();
        info.isDirectory = entry.isDir();
        info.isHidden = entry.isHidden();
        info.isSystem = false; // QFileInfo没有直接的系统文件检测
        info.isReadOnly = !(entry.permissions() & QFile::WriteUser);
        info.isSymbolicLink = entry.isSymLink();
        info.isExecutable = entry.isExecutable();
        info.permissions = getFilePermissions(entry);
        info.owner = getFileOwner(entry.absoluteFilePath());
        info.diskSize = getFileDiskSize(entry.absoluteFilePath());

        fileList.append(info);
    }
    qDebug() << "[FileManager] 目录信息搜索后 time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    return fileList;
}*/

QList<FileInfo> FileManager::getDirectoryContents(const QString &path)
{
    QList<FileInfo> fileList;

    // 添加上级目录
    if (path.length() > 3) { // 不是根目录
        FileInfo parent;
        parent.name = "..";
        parent.type = "上级目录";
        parent.isDirectory = true;
        fileList.append(parent);
    }

    qDebug() << "[FileManager] 目录信息搜索前 time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 使用Windows API直接枚举文件
    WIN32_FIND_DATAW findData;
    QString searchPath = QDir::toNativeSeparators(path);
    if (!searchPath.endsWith("\\")) searchPath += "\\";
    searchPath += "*.*";

    HANDLE hFind = FindFirstFileW((LPCWSTR)searchPath.utf16(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return fileList;
    }

    do {
        QString fileName = QString::fromWCharArray(findData.cFileName);

        // 跳过 "." 和 ".."
        if (fileName == "." || fileName == "..") continue;

        FileInfo info;
        info.name = fileName;

        // 检查是否为目录
        info.isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        // 检查是否为隐藏文件
        info.isHidden = (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;

        // 文件大小
        if (!info.isDirectory) {
            LARGE_INTEGER size;
            size.LowPart = findData.nFileSizeLow;
            size.HighPart = findData.nFileSizeHigh;
            info.size = size.QuadPart;

            // 获取文件后缀
            int dotIndex = fileName.lastIndexOf('.');
            if (dotIndex > 0) {
                info.suffix = fileName.mid(dotIndex + 1).toLower();
            }
        } else {
            info.size = 0; // 目录大小设为0，需要时再计算
        }

        // 修改时间
        FILETIME ft = findData.ftLastWriteTime;
        SYSTEMTIME st;
        FileTimeToSystemTime(&ft, &st);
        info.modified = QDateTime(QDate(st.wYear, st.wMonth, st.wDay),
                                  QTime(st.wHour, st.wMinute, st.wSecond));

        // 使用Shell API快速获取文件类型描述
        SHFILEINFOW shfi = {0};
        QString filePath = path + "/" + fileName;

        if (SHGetFileInfoW((LPCWSTR)filePath.utf16(),
                           0,
                           &shfi,
                           sizeof(shfi),
                           SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES)) {
            info.type = QString::fromWCharArray(shfi.szTypeName);
        } else {
            info.type = info.isDirectory ? "文件夹" : "文件";
        }

        fileList.append(info);

    } while (FindNextFileW(hFind, &findData) != 0);

    FindClose(hFind);

    // 排序：目录在前，按名称排序
    std::sort(fileList.begin(), fileList.end(), [](const FileInfo &a, const FileInfo &b) {
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory > b.isDirectory; // 目录在前
        }
        return QString::compare(a.name, b.name, Qt::CaseInsensitive) < 0;
    });

    qDebug() << "[FileManager] 目录信息搜索后 time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    return fileList;
}

QList<FileInfo> FileManager::searchFiles(const SearchParams &params)
{
    QList<FileInfo> results;
    int fileCount = 0;

    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
    if (params.searchSubfolders) {
        flags |= QDirIterator::Subdirectories;
    }

    QDirIterator it(params.path,
                    params.filters.isEmpty() ? QStringList("*") : params.filters,
                    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System,
                    flags);

    QRegularExpression regex;
    if (!params.keyword.isEmpty()) {
        QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
        if (!params.caseSensitive) {
            options |= QRegularExpression::CaseInsensitiveOption;
        }
        regex.setPattern(params.keyword);
        regex.setPatternOptions(options);
    }

    QStringList visitedDirs;

    while (it.hasNext() && !m_searchCancelled) {
        QString filePath = it.next();
        QFileInfo info = it.fileInfo();

        // 跳过重复目录
        if (info.isDir()) {
            if (visitedDirs.contains(filePath)) continue;
            visitedDirs.append(filePath);
        }

        fileCount++;

        // 发送进度信号
        if (fileCount % 100 == 0) {
            emit searchProgress(fileCount, 0, info.fileName());
        }

        // 关键词匹配
        bool match = false;
        if (params.keyword.isEmpty()) {
            match = true; // 空关键词匹配所有
        } else {
            // 匹配文件名
            if (regex.match(info.fileName()).hasMatch()) {
                match = true;
            }
            // 匹配路径
            else if (regex.match(info.absoluteFilePath()).hasMatch()) {
                match = true;
            }
            // 匹配后缀
            else if (regex.match(info.suffix()).hasMatch()) {
                match = true;
            }
        }

        if (match) {
            FileInfo fileInfo;
            fileInfo.name = info.fileName();
            fileInfo.suffix = info.suffix().toLower();
            fileInfo.size = info.isDir() ? getDirectorySize(filePath) : info.size();
            fileInfo.type = getFileType(info);
            fileInfo.modified = info.lastModified();
            fileInfo.created = info.birthTime();
            fileInfo.accessed = info.lastRead();
            fileInfo.isDirectory = info.isDir();
            fileInfo.isHidden = info.isHidden();
            fileInfo.isReadOnly = !(info.permissions() & QFile::WriteUser);
            fileInfo.isSymbolicLink = info.isSymLink();
            fileInfo.isExecutable = info.isExecutable();
            fileInfo.permissions = getFilePermissions(info);
            fileInfo.owner = getFileOwner(filePath);
            fileInfo.diskSize = getFileDiskSize(filePath);

            results.append(fileInfo);

            // 限制结果数量
            if (results.size() >= 1000) {
                emit searchProgress(fileCount, fileCount, QString("搜索结果过多，已限制为1000条"));
                break;
            }
        }
    }

    return results;
}

FileInfo FileManager::getDetailedFileInfo(const QString &filePath)
{
    FileInfo info;
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists()) return info;

    info.name = fileInfo.fileName();
    info.suffix = fileInfo.suffix().toLower();
    info.size = fileInfo.isDir() ? getDirectorySize(filePath) : fileInfo.size();
    info.type = getFileType(fileInfo);
    info.modified = fileInfo.lastModified();
    info.created = fileInfo.birthTime();
    info.accessed = fileInfo.lastRead();
    info.isDirectory = fileInfo.isDir();
    info.isHidden = fileInfo.isHidden();
    info.isReadOnly = !(fileInfo.permissions() & QFile::WriteUser);
    info.isSymbolicLink = fileInfo.isSymLink();
    info.isExecutable = fileInfo.isExecutable();
    info.permissions = getFilePermissions(fileInfo);
    info.owner = getFileOwner(filePath);
    info.diskSize = getFileDiskSize(filePath);

    // 获取额外Windows信息
    WIN32_FILE_ATTRIBUTE_DATA attrData;
    if (GetFileAttributesExW(filePath.toStdWString().c_str(),
                             GetFileExInfoStandard, &attrData)) {
        info.isSystem = (attrData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;
    }

    return info;
}

QString FileManager::getFileType(const QFileInfo &fileInfo)
{
    if (fileInfo.isDir()) {
        return "文件夹";
    }

    QString suffix = fileInfo.suffix().toLower();

    // 常见文件类型
    static QHash<QString, QString> typeMap = {
        {"exe", "应用程序"}, {"dll", "动态链接库"}, {"sys", "系统文件"},
        {"txt", "文本文档"}, {"log", "日志文件"}, {"ini", "配置文件"},
        {"jpg", "JPEG图像"}, {"jpeg", "JPEG图像"}, {"png", "PNG图像"}, {"bmp", "位图图像"}, {"gif", "GIF图像"},
        {"mp3", "MP3音频"}, {"wav", "WAV音频"}, {"flac", "FLAC音频"},
        {"mp4", "MP4视频"}, {"avi", "AVI视频"}, {"mkv", "MKV视频"},
        {"zip", "ZIP压缩包"}, {"rar", "RAR压缩包"}, {"7z", "7-Zip压缩包"},
        {"pdf", "PDF文档"},
        {"doc", "Word文档"}, {"docx", "Word文档"},
        {"xls", "Excel文档"}, {"xlsx", "Excel文档"},
        {"ppt", "PowerPoint文档"}, {"pptx", "PowerPoint文档"},
        {"html", "网页文件"}, {"htm", "网页文件"},
        {"cpp", "C++源代码"}, {"h", "C/C++头文件"}, {"java", "Java源代码"}
    };

    if (typeMap.contains(suffix)) {
        return typeMap[suffix];
    }

    return suffix.isEmpty() ? "文件" : QString("%1文件").arg(suffix.toUpper());
}

QString FileManager::getFilePermissions(const QFileInfo &fileInfo)
{
    QFile::Permissions perms = fileInfo.permissions();
    QString permissions;

    if (perms & QFile::ReadUser) permissions += "R";
    if (perms & QFile::WriteUser) permissions += "W";
    if (perms & QFile::ExeUser) permissions += "X";
    if (fileInfo.isHidden()) permissions += "H";
    if (fileInfo.isSymLink()) permissions += "L";

    return permissions.isEmpty() ? "---" : permissions;
}

QString FileManager::getFileOwner(const QString &filePath)
{
    PSID pSidOwner = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;
    QString owner;

    if (GetNamedSecurityInfoW(filePath.toStdWString().c_str(),
                              SE_FILE_OBJECT,
                              OWNER_SECURITY_INFORMATION,
                              &pSidOwner, NULL, NULL, NULL, &pSD) == ERROR_SUCCESS) {

        LPWSTR name = NULL;
        LPWSTR domain = NULL;
        DWORD nameSize = 0, domainSize = 0;
        SID_NAME_USE sidType;

        // 获取用户名
        if (LookupAccountSidW(NULL, pSidOwner, name, &nameSize,
                              domain, &domainSize, &sidType) ||
            GetLastError() == ERROR_INSUFFICIENT_BUFFER) {

            name = new WCHAR[nameSize];
            domain = new WCHAR[domainSize];

            if (LookupAccountSidW(NULL, pSidOwner, name, &nameSize,
                                  domain, &domainSize, &sidType)) {
                owner = QString::fromWCharArray(domain) + "\\" +
                        QString::fromWCharArray(name);
            }

            delete[] name;
            delete[] domain;
        }

        LocalFree(pSD);
    }

    if (owner.isEmpty()) {
        // 回退方案：获取当前用户名
        wchar_t username[UNLEN + 1];
        DWORD size = UNLEN + 1;
        if (GetUserNameW(username, &size)) {
            owner = QString::fromWCharArray(username);
        } else {
            owner = "Unknown";
        }
    }

    return owner;
}

qint64 FileManager::getFileDiskSize(const QString &filePath)
{
    QFileInfo info(filePath);
    if (!info.exists()) return 0;

    if (info.isDir()) {
        return getDirectorySize(filePath);
    }

    // 获取文件在磁盘上的实际大小（考虑簇大小）
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(filePath.toStdWString().c_str(), &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        LARGE_INTEGER size;
        size.HighPart = findData.nFileSizeHigh;
        size.LowPart = findData.nFileSizeLow;
        return size.QuadPart;
    }

    return info.size();
}

qint64 FileManager::getDirectorySize(const QString &path)
{
    qint64 totalSize = 0;
    QDirIterator it(path, QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        totalSize += it.fileInfo().size();
    }

    return totalSize;
}

QString FileManager::formatFileSize(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (bytes >= GB) {
        return QString("%1 GB").arg(qreal(bytes) / GB, 0, 'f', 2);
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(qreal(bytes) / MB, 0, 'f', 2);
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(qreal(bytes) / KB, 0, 'f', 2);
    }

    return QString("%1 bytes").arg(bytes);
}

void FileManager::startWatching(const QString &path)
{
    if (!m_watcher) {
        m_watcher = new QFileSystemWatcher(this);
        connect(m_watcher, &QFileSystemWatcher::directoryChanged,
                this, &FileManager::onDirectoryChanged);
    }

    // 停止之前的监控
    if (!m_watcher->directories().isEmpty()) {
        m_watcher->removePaths(m_watcher->directories());
    }

    // 添加新监控
    if (QDir(path).exists()) {
        m_watcher->addPath(path);
    }
}

void FileManager::stopWatching()
{
    if (m_watcher && !m_watcher->directories().isEmpty()) {
        m_watcher->removePaths(m_watcher->directories());
    }
}

void FileManager::onDirectoryChanged(const QString &path)
{
    qDebug() << "[FileManager] Directory changed:" << path;
}

void FileManager::cancelSearch()
{
    QMutexLocker locker(&m_searchMutex);
    m_searchCancelled = true;

    // 等待搜索完成
    if (m_searchFuture.isRunning()) {
        m_searchFuture.waitForFinished();
    }
}

void FileManager::sendFileListReply(const QString &path, const QList<FileInfo> &fileList)
{
    QByteArray replyData;
    QDataStream replyStream(&replyData, QIODevice::WriteOnly);
    replyStream << path << fileList;

    sendPacket(FILE_LIST_REPLY, replyData);
}

void FileManager::sendSearchReply(const QList<FileInfo> &searchResults, bool completed)
{
    QByteArray replyData;
    QDataStream replyStream(&replyData, QIODevice::WriteOnly);
    replyStream << searchResults << completed;

    sendPacket(FILE_SEARCH_REPLY, replyData);
}

void FileManager::sendErrorReply(quint8 token, const QString &error)
{
    QByteArray replyData;
    QDataStream replyStream(&replyData, QIODevice::WriteOnly);

    // 根据token类型发送不同的错误回复
    switch (token) {
    case FILE_LIST_REQUEST:
        replyStream << QString("") << QList<FileInfo>();
        break;
    case FILE_SEARCH_REQUEST:
        replyStream << QList<FileInfo>() << false;
        break;
    default:
        replyStream << false << error;
        break;
    }

    sendPacket(token + 1, replyData);

    qDebug() << "[FileManager] Error:" << error;
}

void FileManager::sendPacket(quint8 token, const QByteArray &data)
{
    PACKET pkt;
    pkt.isToken = token;
    pkt.payload = data;

    // 使用基类的m_client发送数据
    if (m_client) {
        m_client->SendData(pkt);
    } else {
        qWarning() << "[FileManager] m_client is null, cannot send packet";
    }
}
