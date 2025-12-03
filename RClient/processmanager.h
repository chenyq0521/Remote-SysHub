#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H


#include "absmanager.h"
#include "processhelper.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <QVector>
#include <QList>
#include "detours.h"

typedef LONG (NTAPI *PFN_NT_QUERY_SYSTEM_INFORMATION)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

typedef struct _PROCESS_DATA_
{
    DWORD ProcessID;
    QString ImageName;
    QString ProcessPath;
    QString ProcessType;
    QString ThreadStatus;
    ULONG HandleNum;
    ULONG ThreadNum;
    QString ProcessBit;
}PROCESS_DATA;

class ProcessManager : public AbsManager
{
    Q_OBJECT
public:
    explicit ProcessManager(TcpClient *TClient, QObject *parent = nullptr);
    ~ProcessManager();
    void HandlePacket(const PACKET &pkt) override;

signals:
    void onConnected();
private:
    QVector <PROCESS_DATA> m_allProcessData;
    // 钩子状态
    bool m_termProcessHooked;
    bool m_openProcessHooked;
    bool m_getModuleHooked;

    void KillProcess(DWORD pid);
    void SuspendProcess(DWORD pid);
    void ResumeProcess(DWORD pid);
    void SendProcessMemoryInfo(DWORD pid);
    bool EnumProcessData();

    bool GetThreadIdentity(IN HANDLE ProcessIdentity, OUT HANDLE* ThreadIdentity);
    bool GetProcessState(IN HANDLE ProcessIdentity, OUT ULONG* ThreadState, OUT ULONG* WaitReason);
    QString GetProcessPath(HANDLE ProcessHandle);
    QString CheckProcessBit(HANDLE ProcessHandle); //"返回X86，X64或未知"
    QString GetProcessType(DWORD Pid,const QString& path,DWORD sessionId);
    QString MapThreadState(ULONG Status,ULONG WaitReason);
    void SendClientProcessList();


    void InstallHook(DWORD pid,quint8 choice);
    void UninstallHook(DWORD pid, quint8 choice);
};

extern BOOL (WINAPI *OldTerminateProcess)(HANDLE, UINT);
extern HANDLE(WINAPI *OldOpenProcess)(DWORD dwDesiredAccess,BOOL bInheritHandle,
                               DWORD dwProcessId);
extern HMODULE(WINAPI *OldGetModuleHandleW)(LPCWSTR lpModuleName);

BOOL WINAPI NewTerminateProcess(
    HANDLE hProcess,
    UINT uExitCode);
HANDLE WINAPI NewOpenProcess(DWORD dwDesiredAccess,
                             BOOL  bInheritHandle,
                             DWORD dwProcessId);
HMODULE WINAPI NewGetModuleHandleW(LPCWSTR lpModuleName);

#endif // PROCESSMANAGER_H
