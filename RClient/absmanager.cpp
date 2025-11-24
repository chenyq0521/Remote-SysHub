#include "absmanager.h"
#include "mainwindow.h"
extern MainWindow * __MainWindow__;

AbsManager::AbsManager(TcpClient *TClient, QObject *parent)
    : QObject(parent)
{
    m_client = TClient;
    TClient->SetManagerObject(this);
    connect(m_client,&TcpClient::socketErrorOccurred,this,&AbsManager::OnSocketError);
    connect(m_client,&TcpClient::disconnected,this,&AbsManager::OnDisconnected);
    connect(this,&AbsManager::logMessage, __MainWindow__, &MainWindow::LogMessage);

}

bool AbsManager::EnableSeDebugPrivilege(HANDLE ProcessHandle, WINBOOL IsEnable, LPCTSTR RequireLevel)
{
    DWORD  LastError;
    HANDLE TokenHandle = 0;

    if (!OpenProcessToken(ProcessHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
    {
        LastError = GetLastError();
        if (TokenHandle)
            CloseHandle(TokenHandle);
        qDebug()<<"[Absmanager]aaa";
        return LastError;
    }
    TOKEN_PRIVILEGES TokenPrivileges;
    memset(&TokenPrivileges, 0, sizeof(TOKEN_PRIVILEGES));
    LUID v1;
    if (!LookupPrivilegeValue(NULL, RequireLevel, &v1))
    {
        LastError = GetLastError();
        CloseHandle(TokenHandle);
        qDebug()<<"[Absmanager]bbb";
        return LastError;
    }
    TokenPrivileges.PrivilegeCount = 1;
    TokenPrivileges.Privileges[0].Luid = v1;
    if (IsEnable)
        TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        TokenPrivileges.Privileges[0].Attributes = 0;
    AdjustTokenPrivileges(TokenHandle, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    LastError = GetLastError();
    CloseHandle(TokenHandle);
    qDebug()<<"[Absmanager]ccc";
    return LastError;
}

void AbsManager::Start()
{
    qDebug()<<"[AbsManager]manager thread id:"<<QThread::currentThreadId();
    m_client->ConnectToServer(m_ip,m_port);

}

void AbsManager::OnSocketError(QAbstractSocket::SocketError socketError)
{
    //qDebug() << "[AbsManager]Failed to set socket descriptor!";
    emit finished();
}

void AbsManager::OnDisconnected()
{
    qDebug() << "[AbsManager]Client disconnected";
    m_client->deleteLater();
    emit finished();
}
