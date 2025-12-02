#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "absmanager.h"
#include <winuser.h>
class WindowManager : public AbsManager
{
    Q_OBJECT
public:
    explicit WindowManager(TcpClient* TClient,QObject *parent = nullptr);
    ~WindowManager();


    void HandlePacket(const PACKET &pkt) override;
signals:
    void onConnected();

private:
    //QString GetWindowState(HWND hwnd);
    void SendClientWindowList();
    //static bool CALLBACK EnumWindowProcedure(HWND Hwnd, LPARAM ParameterData);
    void ControlWindow(const QByteArray &payload); //实现原来testwindow的功能
};

#endif // WINDOWMANAGER_H
