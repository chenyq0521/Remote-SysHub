#ifndef REGISTRYMANAGER_H
#define REGISTRYMANAGER_H

#include "absmanager.h"
#include <QObject>
typedef struct _PACKET_HEADER_
{
    int count;
    DWORD nameSize;
    DWORD dataSize;
}PACKET_HEADER;

enum MREG_KEY
{
    MHKEY_CLASSES_ROOT,
    MHKEY_CURRENT_USER,
    MHKEY_LOCAL_MACHINE,
    MHKEY_USERS,
    MHKEY_CURRENT_CONFIG
};

enum MREG_VALUE
{
    MREG_SZ,
    MREG_DWORD,
    MREG_QWORD,
    MREG_BINARY,
    MREG_MULTI_SZ,
    MREG_EXPAND_SZ,
    MREG_UNKNOWN_TYPE
};

class RegistryManager : public AbsManager
{
    Q_OBJECT
public:
    explicit RegistryManager(TcpClient *TClient,QObject *parent = nullptr);
    ~RegistryManager();

    void HandlePacket(const PACKET &pkt) override;

signals:
    void onConnected();

private:
    QByteArray FindPath(HKEY rootKey, const QString &subPath);
    QByteArray FindValue(HKEY rootKey, const QString &subPath);
    void FindRegistryData(quint8 rootId, const QString &subPath, bool isFindPath);
    void ModifyValue(quint8 rootId, const QString &subPath, const QString &valueName, MREG_VALUE type, const QByteArray &data);
    void DeleteValue(quint8 rootId, const QString &subPath, const QString &valueName);
    void DeleteKey(quint8 rootId, const QString &subPath, const QString &KeyName);
    void RenameValue(quint8 rootId, const QString &subPath, const QString &oldName, const QString &newName);
    void RenameKey(quint8 rootId, const QString &subPath, const QString &oldName, const QString &newName);
    void CreateKey(quint8 rootId, const QString &subPath, const QString &KeyName);
    void CreateValue(quint8 rootId, const QString &subPath,
                     const QString &valueName, MREG_VALUE type, const QByteArray &data);
    HKEY MapKey(unsigned char key);
    MREG_VALUE MapRegType(DWORD type);
};

#endif // REGISTRYMANAGER_H
