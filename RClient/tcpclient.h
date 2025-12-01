
#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include "common.h"
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QVector>
#include <QWidget>
#include <QMessageBox>
#include <zlib.h>


class AbsManager;
class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    void SetManagerObject(AbsManager* Manager);
    void ConnectToServer(const QString &ip, int port);
    QHostAddress GetAddress();
    /*PACKET * GetPacket() const
    {
        return m_pkt;
    }
    */

    //数据处理
    void SendData(PACKET &pkt);
public slots:
    void SendLogin();
    void SendReply(unsigned char IsToken);

private slots:
    void OnReadyRead();
    void OnErrorOccurred(QAbstractSocket::SocketError socketError);

signals:
    void socketErrorOccurred(QAbstractSocket::SocketError socketError);
    void connected();
    void disconnected();

private:

    //PACKET* m_pkt;
    QTcpSocket *m_socket;
    AbsManager *m_manager;
    QByteArray m_sendBuffer;
    QByteArray m_receiveBuffer;

    bool m_receivingLarge = false;
    quint32 m_payloadRemaining;
    quint8 m_currentIsToken;
    quint32 m_currentDatalength;
    quint32 m_currentCheckNum;
    QByteArray m_partialPayload;
};

#endif // TCPCLIENT_H
