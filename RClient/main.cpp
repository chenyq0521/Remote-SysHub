
#include "mainwindow.h"
#include "kernelmanager.h"
#include "tcpclient.h"
#include "common.h"
#include "loginhelper.h"
#include <QApplication>
#include <QObject>
#include <windows.h>
#include <winternl.h>     // 如果需要 NT API
#include <tlhelp32.h>
#include <psapi.h>
#include "detours.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QThread* thread = new QThread;
    TcpClient* client = new TcpClient;
    KernelManager* kmanager = new KernelManager(client);

    kmanager->moveToThread(thread);
    client->moveToThread(thread);

    QObject::connect(thread, &QThread::started, kmanager, &KernelManager::Start);
    QObject::connect(kmanager, &KernelManager::finished, thread, &QThread::quit);
    QObject::connect(kmanager, &KernelManager::finished, kmanager, &KernelManager::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    //qDebug()<<GetCpuName();
    //qDebug()<<GetWinVersion();
    //qDebug()<<GetGpuName();
    thread->start();
    QObject::connect(client,&TcpClient::connected,client,&TcpClient::SendLogin);

    return a.exec();
}
