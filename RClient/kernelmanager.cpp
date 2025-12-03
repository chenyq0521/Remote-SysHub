#include "kernelmanager.h"
#include "cmdmanager.h"
#include "windowmanager.h"
#include "processmanager.h"
#include "registrymanager.h"
#include  "filemanager.h"
#include "loginhelper.h"
#include <QThread>


KernelManager::KernelManager(TcpClient *TClient, QObject *parent) :AbsManager(TClient)
{
    qDebug()<<"A Kernel Manager is Created.";
    connect(m_client,&TcpClient::connected,this,[=](){
        emit logMessage("[KernelManager] 客户端已与服务器连接");
    });
    connect(m_client,&TcpClient::disconnected,this,[=](){
        emit logMessage("服务器已断开连接!");
    });
}

void KernelManager::HandlePacket(const PACKET &pkt)
{
    qDebug()<<"Kernel Manager Handling...";
    auto Token = pkt.isToken;
    switch(Token)
    {
    case CMD_REQUIRE:
        CreateManager<CmdManager>();
        break;
    case WINDOW_REQUIRE:
        CreateManager<WindowManager>();
        break;
    case PROCESS_REQUIRE:
        CreateManager<ProcessManager>();
        break;
    case REGISTRY_REQUIRE:
        CreateManager<RegistryManager>();
        break;
    case FILE_REQUIRE:
        CreateManager<FileManager>();
        break;
    default:
        qDebug()<<"unknown istoken:"<<Token;
        break;
    }
}


template<typename ManagerType>
void KernelManager::CreateManager()
{
    QThread* thread = new QThread;
    TcpClient* client = new TcpClient;
    ManagerType* manager = new ManagerType(client);

    manager->moveToThread(thread);
    client->moveToThread(thread);

    connect(thread, &QThread::started, manager, &ManagerType::Start);
    connect(manager, &ManagerType::finished, thread, &QThread::quit);
    connect(manager, &ManagerType::finished, manager, &ManagerType::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}
