#include "tcpserver.h"
#include "tcpworker.h"
#include <QNetworkProxy>
TcpServer::TcpServer(QObject *parent)
    : QTcpServer{parent}
{}

void TcpServer::IocpListen()
{
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    if (!listen(QHostAddress::Any, m_listenPort))
    {
        qDebug() << "Server could not start!"<< m_ip << ":" << m_listenPort;
        qDebug() << this->errorString();
    }
    else
    {
        qDebug() << "Server started!"<< m_ip << ":" << m_listenPort;;
    }
}

void TcpServer::SetAddress(const QString addr, unsigned short port)
{
    m_ip = addr;
    m_listenPort = port;
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "New client descriptor:" << socketDescriptor;
    // 获取当前线程的 ID
    Qt::HANDLE currentThreadId = QThread::currentThreadId();
    // 打印当前线程的 ID
    qDebug() << "TCP Thread ID:" << currentThreadId;
    //创建子线程对象
    QThread* thread = new QThread;
    //创建主线程中的工作对象
    TcpWorker* worker = new TcpWorker(socketDescriptor);
    //将工作对象移入子线程对象中
    worker->moveToThread(thread);
    m_clients.insert(socketDescriptor,worker);

    connect(thread, &QThread::started, worker, &TcpWorker::Start);  // 启动工作逻辑
    connect(worker, &TcpWorker::finished,[=](){        // 任务结束退出线程
        qDebug() << "socket disconnected "<<socketDescriptor;
        worker->GetContext()->socket->disconnectFromHost();
        thread->quit();
        worker->deleteLater();
        m_clients.remove(socketDescriptor);
        //删除对应的功能类连接
        emit RemoveClient(socketDescriptor);

    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    //连接业务子线程到服务器逻辑
    connect(worker, &TcpWorker::dataPrased, this, &TcpServer::DataPrased);

    thread->start();
}
