#ifndef TCPWORKER_H
#define TCPWORKER_H

#include "common.h"
#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include <QFile>
#include <QWidget>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QIODevice>
//只做通信的读取，发信号等功能
class TcpWorker : public QObject
{
    Q_OBJECT
public:
    explicit TcpWorker(qintptr descriptor,QObject *parent = nullptr);
    CONTEXT_OBJECT* GetContext() const
    {
        return m_ctxobj;
    }
signals:
    void dataPrased(CONTEXT_OBJECT *ctx);
    void connected(CONTEXT_OBJECT *ctx);
    void disconnected(CONTEXT_OBJECT* ctx);
    void finished();

public slots:
    void Start();           // 线程启动后执行
    void OnReadyRead();     // 解包
    void SendData();        // 拆包
private:
    CONTEXT_OBJECT* m_ctxobj;
    qintptr m_socketDescriptor;
    QByteArray m_receiveBuffer;
    QByteArray m_sendBuffer;
};

#endif // TCPWORKER_H
