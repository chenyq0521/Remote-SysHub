#ifndef CMDMANAGER_H
#define CMDMANAGER_H

#include "absmanager.h"
#include <atomic>

class CmdManager : public AbsManager
{
    Q_OBJECT
public:
    explicit CmdManager(TcpClient* TClient, QObject *parent = nullptr);
    ~CmdManager();
    void HandlePacket(const PACKET &pkt) override;

signals:
    void onConnected();

private:
    void StartCmdProcess();
    void StopCmdProcess();
    void ReaderLoop();
    void ForwardMessage(const QByteArray &command);
    static DWORD WINAPI ReceiveProcedure(LPVOID ParameterData);

    PROCESS_INFORMATION   m_processInfo = {0};
    STARTUPINFOA          m_startupInfo = {0};

    bool m_isLoop;

    HANDLE m_inReadHandle;  //读cmd输出的读端
    HANDLE m_inWriteHandle; //cmd输出的写端
    HANDLE m_outReadHandle; //写给cmd的读端
    HANDLE m_outWriteHandle;    //写给cmd的写端
    HANDLE m_threadHandle;

    QByteArray m_outputBuffer; // 缓存当前命令的输出
};

#endif // CMDMANAGER_H
