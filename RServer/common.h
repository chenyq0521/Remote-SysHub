#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <QString>
#include <QTcpSocket>
#include <QHostAddress>

#define PACKET_LENGTH 0x4000
#define PACKET_HEADER_LENGTH 9
typedef struct _CONTEXT_OBJECT_
{
    QTcpSocket* socket;

    QByteArray receiveBuffer;
    QByteArray sendBuffer;

    unsigned char dialogIdentity;   //窗口id
    QObject* dialogHandle;        //窗口句柄

    // 通信数据流
    unsigned char isToken;
    quint32 dataLength;
    quint32 checkNum;
    QByteArray payload;

    bool receivingLarge = false;
    quint32 payloadRemaining;

    void Initialize()
    {
        socket = nullptr;
        receiveBuffer.clear();
        sendBuffer.clear();
        dialogHandle = nullptr;
        dialogIdentity = 0;
    }

}CONTEXT_OBJECT;

typedef struct _LOGIN_IMFORMATION_
{
    qintptr socketDescriptor;
    QString OSversion;
    QString CPU;
    QHostAddress Address;
    QString HostName;
    QString GPU;
    QString WebSpeed;

}LOGIN_INFORMAITON, * PLOGIN_INFORMAITON;


//定义通信包头
enum CONNECTION
{
    CLIENT_LOGIN,
    CLIENT_GO_ON,
    CMD_REQUIRE,
    CMD_REPLY,
    WINDOW_REQUIRE,
    WINDOW_REPLY,
    PROCESS_REQUIRE,
    PROCESS_REPLY,
    REGISTRY_REQUIRE,
    REGISTRY_REPLY,

    CMD_INPUT,
    CMD_OUTPUT,

    WINDOW_CLOSE_REQUIRE,
    WINDOW_HIDE_REQUIRE,
    WINDOW_MAX_REQUIRE,
    WINDOW_MIN_REQUIRE,
    WINDOW_RESTORE_REQUIRE,
    WINDOW_REFRESH_REQUIRE,
    WINDOW_OPERATION_REPLY,

    PROCESS_REFRESH_RQRUIRE,
    PROCESS_KILL_REQUIURE,
    PROCESS_SUSPEND_REQUIRE,
    PROCESS_RESUME_REQUIRE,
    PROCESS_MEMORY_REQUIRE,
    PROCESS_OPERATION_REPLY,

    REGISTRY_SEARCH_REQUIRE,
    REGISTRY_SEARCH_PATH_REPLY,
    REGISTRY_SEARCH_KEY_REPLY,
    REGISTRY_REFRESH_VALUE_REQUIRE,
    REGISTRY_VALUE_MODIFY,
    REGISTRY_VALUE_DELETE,
    REGISTRY_KEY_RENAME,
    REGISTRY_VALUE_RENAME,
    REGISTRY_VALUE_NEW,
    REGISTRY_KEY_NEW,
    REGISTRY_KEY_DELETE

};

//定义窗口id
enum DIALOG
{
    PLACE_HOLDER,
    CMD_DIALOG,
    WINDOW_DIALOG,
    PROCESS_DIALOG,
    REGISTRY_DIALOG
};

#endif // COMMON_H
