#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "common.h"
#include "tcpworker.h"
#include <QObject>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QThread>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:

    explicit TcpServer(QObject *parent = nullptr);
    void IocpListen();
    void SetAddress(const QString addr, unsigned short port);

    QMap<qintptr, TcpWorker*> m_clients;


protected:
    void incomingConnection(qintptr socketDescriptor) override;

signals:

    //客户端连接相关
    void ConnectedtoClient(QString address,qint16 port);
    void DisconnectedtoClient(QString address,qint16 port,QString username);

    //处理客户端登录信息
    //void AddLoginToClientTable(LOGIN_INFORMAITON info);
    void RemoveClient(qintptr socketDescriptor);

    //处理通信
    void DataPrased(CONTEXT_OBJECT *ctx);
private:
    QString m_ip;   //ip地址
    unsigned short m_listenPort;  //监听端口

};

#endif // TCPSERVER_H
