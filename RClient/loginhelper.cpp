#include "loginhelper.h"



void SendLoginInformation(TcpClient *client)
{
    if(!client) return;
    PACKET pkt;
    LOGIN_INFORMAITON info;
    info.OSversion = GetWinVersion();
    qDebug()<<info.OSversion;

    info.CPU = GetCpuName();
    info.GPU = GetGpuName();
    info.HostName = QHostInfo::localHostName();
    info.Address = client->GetAddress();
    info.WebSpeed = 0;

    QDataStream stream(&pkt.payload,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << info.OSversion << info.CPU << info.Address << info.HostName << info.GPU << info.WebSpeed;

    pkt.isToken = CLIENT_LOGIN;

    client->SendData(pkt);

    qDebug() << "[LoginHelper] Login info sent. OS:" << info.OSversion
             << "CPU:" << info.CPU
             << "GPU:" << info.GPU
             << "Host:" << info.HostName
             << "IP:" << info.Address;

}

QString GetWinVersion()
{
    DWORD major = 0,minor = 0,build = 0;

    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
    if (hMod) {
        RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
        if (fxPtr != nullptr) {
            RTL_OSVERSIONINFOW rovi = { 0 };
            rovi.dwOSVersionInfoSize = sizeof(rovi);
            if (fxPtr(&rovi) == 0) {
                major = rovi.dwMajorVersion;
                minor = rovi.dwMinorVersion;
                build = rovi.dwBuildNumber;
            }
        }
    }
    if(!(major || minor || build)) return "Unknown Windows Version";
    if(major == 10 && build >= 22000)
        return QString("Windows %1.%2 (Build %3)").arg("11").arg(minor).arg(build);
    else return QString("Windows %1.%2 (Build %3)").arg(major).arg(minor).arg(build);
}

QString GetCpuName()
{

    QSettings reg("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                  QSettings::NativeFormat);
    return reg.value("ProcessorNameString", "Unknown CPU").toString();
}

/*QString GetGpuName()  //获取gpu0
{
    QSettings reg("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000",
                  QSettings::NativeFormat);
    return reg.value("DriverDesc", "Unknown GPU").toString();
}*/

QString GetGpuName()    //获取所有的gpu
{
    QStringList gpuNames;
    QString baseKey = "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}";

    int index = 0;
    while (true)
    {
        QString subKey = QString("%1\\%2").arg(baseKey).arg(QString("%1").arg(index, 4, 10, QChar('0')));
        QSettings reg(subKey, QSettings::NativeFormat);

        // 检查是否存在该子项
        if (reg.allKeys().isEmpty())
            break;  // 不存在则跳出循环

        QString desc = reg.value("DriverDesc").toString();
        if (!desc.isEmpty())
            gpuNames << desc;

        index++;
    }

    // 合并结果
    return gpuNames.join(",");
}

