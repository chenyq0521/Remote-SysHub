#ifndef LOGINHELPER_H
#define LOGINHELPER_H

#include "common.h"
#include "tcpclient.h"
#include <versionhelpers.h>
#include <vfw.h>
#include <ntstatus.h>
#include <winternl.h>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QDebug>
#include <QDataStream>
#include <QBuffer>
#include <QSettings>
#include <QFile>

typedef LONG (WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

typedef struct _LOGIN_IMFORMATION_
{
    QString OSversion;
    QString CPU;
    QHostAddress Address;
    QString HostName;
    QString GPU;
    QString WebSpeed;

}LOGIN_INFORMAITON, * PLOGIN_INFORMAITON;

void SendLoginInformation(TcpClient *client);
QString GetWinVersion();
QString GetCpuName();
QString GetGpuName();

#endif // LOGINHELPER_H
