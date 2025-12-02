#ifndef ABSMANAGER_H
#define ABSMANAGER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include <zlib.h>
#include <QDebug>
#include <QtEndian>
#include <QMap>
#include <thread>
#include "tcpclient.h"
#include "common.h"

class TcpClient;
class AbsManager : public QObject
{
    Q_OBJECT
public:
    explicit AbsManager(TcpClient* TClient,QObject *parent = nullptr);
    virtual void HandlePacket(const PACKET &pkt) = 0;
    bool EnableSeDebugPrivilege(HANDLE ProcessHandle, BOOL IsEnable, LPCTSTR RequireLevel);

public slots:
    void Start();
    void OnSocketError(QAbstractSocket::SocketError socketError);
    void OnDisconnected();

signals:
    void finished();
    void logMessage(const QString &message);
protected:
    TcpClient* m_client;
    QString m_ip = "127.0.0.1";
    quint16 m_port = 8756;
};

#endif // ABSMANAGER_H
