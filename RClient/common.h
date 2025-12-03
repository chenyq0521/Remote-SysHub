
#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <winsock2.h>
#include <cstdlib>
#include <minwindef.h>
#include <synchapi.h>
#include <QString>
#include <QDateTime>

#define PACKET_LENGTH 0x4000

typedef struct _PACKET_
{
    QByteArray receiveBuffer;
    QByteArray sendBuffer;

    unsigned char isToken;
    quint32 dataLength;
    quint32 checkNum;
    QByteArray payload;

    void clear()
    {
        receiveBuffer.clear();
        sendBuffer.clear();
        payload.clear();
    }

}PACKET;


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
    FILE_REQUIRE,
    FILE_REPLY,

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
    REGISTRY_KEY_DELETE,
    // 文件操作标头
    FILE_LIST_REQUEST,       // 文件列表请求
    FILE_LIST_REPLY,         // 文件列表回复
    FILE_SEARCH_REQUEST,     // 文件搜索请求
    FILE_SEARCH_REPLY,       // 文件搜索回复
    FILE_DELETE_REQUEST,     // 文件删除请求
    FILE_DELETE_REPLY,       // 文件删除回复
    FILE_NEWFOLDER_REQUEST,  // 新建文件夹请求
    FILE_NEWFOLDER_REPLY    // 新建文件夹回复

};


#endif // COMMON_H
