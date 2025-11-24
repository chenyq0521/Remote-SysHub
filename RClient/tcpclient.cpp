#include "tcpclient.h"
#include "absmanager.h"
#include "loginhelper.h"



TcpClient::TcpClient(QObject *parent)
    : QObject(parent),m_socket(new QTcpSocket(this))
{
    connect(m_socket,&QTcpSocket::connected,this,[=](){
        qDebug() << "[TcpClient]socket connected";
        emit connected();
    });
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::OnReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, [=](){
        qDebug() << "[TcpClient]Disconnected from server!";
        emit disconnected();
    });
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &TcpClient::OnErrorOccurred);
}

void TcpClient::SetManagerObject(AbsManager *Manager)
{
    m_manager = Manager;

}

void TcpClient::ConnectToServer(const QString &ip, int port)
{
    qDebug() << "socket thread ID:"<<QThread::currentThreadId();
    qDebug() << "Connecting to server at" << ip << ":" << port;
    m_socket->connectToHost(ip, port);
}

QHostAddress TcpClient::GetAddress()
{
    return m_socket->localAddress();
}

void TcpClient::SendData(PACKET &pkt)
{
    pkt.dataLength = pkt.payload.size();
    pkt.checkNum = 0;

    QByteArray fullPacket;
    QDataStream out(&fullPacket, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << pkt.isToken;
    out << pkt.dataLength;
    out << pkt.checkNum;
    if (!pkt.payload.isEmpty()) {
        out.writeRawData(pkt.payload.constData(), pkt.payload.size());
    }

    qDebug() << "[SendData] build full packet, total size =" << fullPacket.size();

    if (fullPacket.size() <= PACKET_LENGTH) {
        int padding = PACKET_LENGTH - fullPacket.size();
        if (padding > 0) fullPacket.append(QByteArray(padding, 0));
        m_socket->write(fullPacket);
        m_socket->waitForBytesWritten();
        qDebug() << "[SendData] send one full packet, size =" << fullPacket.size();
        return;
    }

    int offset = 0;
    while (offset < fullPacket.size()) {
        int chunkSize = qMin(PACKET_LENGTH, fullPacket.size() - offset);
        QByteArray chunk = fullPacket.mid(offset, chunkSize);
        m_socket->write(chunk);
        m_socket->waitForBytesWritten();
        if (offset == 0)
            qDebug() << "[SendData] send first chunk (with header), size =" << chunk.size();
        else
            qDebug() << "[SendData] send body chunk, size =" << chunk.size() << " offset =" << offset;
        offset += chunkSize;
    }
}

void TcpClient::OnReadyRead()
{
    m_receiveBuffer.append(m_socket->readAll());
    qDebug() << "客户端 收到字节数:" << m_receiveBuffer.size();

    while (true) {
        //正在接受多个包
        if (m_receivingLarge) {
            if (m_receiveBuffer.isEmpty()) return;

            qint64 take = qMin<qint64>(m_receiveBuffer.size(), m_payloadRemaining);
            m_partialPayload.append(m_receiveBuffer.left((int)take));
            m_receiveBuffer.remove(0, (int)take);
            m_payloadRemaining -= take;

            qDebug() << "[OnReadyRead] receiving large payload, append" << take
                     << "remaining =" << m_payloadRemaining;

            if (m_payloadRemaining == 0) {
                //完整payload到齐，构造PACKET并交由Manager处理
                PACKET pkt;
                pkt.isToken = m_currentIsToken;
                pkt.dataLength = m_partialPayload.size();
                pkt.checkNum = m_currentCheckNum;
                pkt.payload = m_partialPayload;

                qDebug() << "[OnReadyRead] 大数据接收完成, payload size =" << pkt.payload.size();
                if (m_manager) m_manager->HandlePacket(pkt);

                //重置状态
                m_receivingLarge = false;
                m_payloadRemaining = 0;
                m_partialPayload.clear();
                continue;
            } else {
                return;
            }
        }

        if (m_receiveBuffer.size() < 9) return;

        QDataStream headerStream(m_receiveBuffer);
        headerStream.setByteOrder(QDataStream::BigEndian);

        headerStream
            >> m_currentIsToken
            >> m_currentDatalength
            >> m_currentCheckNum;

        //单包
        if (m_currentDatalength <= PACKET_LENGTH - 9) {
            if (m_receiveBuffer.size() < PACKET_LENGTH) {
                // 还没有收到一个完整的后的单包
                qDebug()<<"[OnReadyRead] 未收到完整单包。";
                return;
            }

            QByteArray pktData = m_receiveBuffer.left(PACKET_LENGTH);
            m_receiveBuffer.remove(0, PACKET_LENGTH);
            //构造PACKET并交由Manager处理
            PACKET pkt;

            pkt.isToken = m_currentIsToken;
            pkt.checkNum = m_currentCheckNum;
            pkt.dataLength = m_currentDatalength;
            pkt.payload = pktData.mid(9, pkt.dataLength);

            qDebug() << "[OnReadyRead] 完整数据接收完成, payload size =" << pkt.payload.size();
            if (m_manager) m_manager->HandlePacket(pkt);
            continue;
        }
        //多包
        else  {
            m_receiveBuffer.remove(0, 9);

            m_partialPayload.clear();
            m_receivingLarge = true;
            m_payloadRemaining = m_currentDatalength;


            qDebug() << "[OnReadyRead] 进入大数据接收模式, totalPayload =" << m_currentDatalength
                     << " remaining =" << m_payloadRemaining;

            continue;
        }
    }
}

void TcpClient::OnErrorOccurred(QAbstractSocket::SocketError socketError)
{
    qDebug() << "[TcpClient]Socket error:" << socketError;
    emit socketErrorOccurred(socketError);
}

void TcpClient::SendLogin()
{
    SendLoginInformation(this);
}

void TcpClient::SendReply(unsigned char IsToken)
{
    PACKET pkt;
    pkt.isToken = IsToken;
    SendData(pkt);
}


