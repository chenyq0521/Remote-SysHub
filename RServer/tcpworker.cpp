#include "tcpworker.h"
#include "serverdlg.h"
extern ServerDlg* __ServerDlg__;

TcpWorker::TcpWorker(qintptr descriptor,QObject *parent)
    : QObject(parent) , m_socketDescriptor(descriptor)
{}

void TcpWorker::Start()
{
    m_ctxobj = new CONTEXT_OBJECT;
    m_ctxobj->Initialize();
    m_ctxobj->socket = new QTcpSocket;
    m_ctxobj -> socket -> moveToThread(QThread::currentThread());
    if (!m_ctxobj -> socket -> setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "Failed to set socket descriptor!";
        emit finished();
        return;
    }
    connect(m_ctxobj->socket, &QTcpSocket::readyRead, this, &TcpWorker::OnReadyRead);
    connect(m_ctxobj->socket, &QTcpSocket::disconnected, this, [this](){
        emit disconnected(m_ctxobj);
        m_ctxobj->socket->deleteLater();
        emit finished();
    });
}

void TcpWorker::SendData()
{
    QByteArray fullPacket;
    QDataStream out(&fullPacket, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    m_ctxobj->dataLength = m_ctxobj->payload.size();
    m_ctxobj->checkNum = 0;
    out << m_ctxobj->isToken;
    out << m_ctxobj->dataLength;
    out << m_ctxobj->checkNum;
    if (!m_ctxobj->payload.isEmpty()) {
        out.writeRawData(m_ctxobj->payload.constData(), m_ctxobj->payload.size());
    }

    qDebug() << "[SendData] build full packet, total size =" << fullPacket.size();

    if(fullPacket.size() <= PACKET_LENGTH)
    {
        int padding = PACKET_LENGTH - fullPacket.size();
        if (padding > 0) fullPacket.append(QByteArray(padding, 0));
        m_ctxobj->socket->write(fullPacket);
        m_ctxobj->socket->waitForBytesWritten();
        qDebug() << "[SendData] send one full packet, size =" << fullPacket.size();
        return;
    }

    int offset = 0;
    while (offset < fullPacket.size()) {
        int chunkSize = qMin(PACKET_LENGTH, fullPacket.size() - offset);
        QByteArray chunk = fullPacket.mid(offset, chunkSize);
        m_ctxobj->socket->write(chunk);
        m_ctxobj->socket->waitForBytesWritten();

        if (offset == 0)
            qDebug() << "[SendData] send first chunk (with header), size =" << chunk.size();
        else
            qDebug() << "[SendData] send body chunk, size =" << chunk.size() << " offset =" << offset;

        offset += chunkSize;
    }
}

void TcpWorker::OnReadyRead()
{
    Qt::HANDLE currentThreadId = QThread::currentThreadId();
    qDebug() << "current Thread ID: in OnreadyRead" << currentThreadId;

    if (!m_ctxobj->socket) return;

    m_ctxobj->receiveBuffer.append(m_ctxobj->socket->readAll());
    qDebug() << "服务器 收到字节数:" << m_ctxobj->receiveBuffer.size();


    while (true) {
        //正在接受多个包
        if (m_ctxobj->receivingLarge) {
            if (m_ctxobj->receiveBuffer.isEmpty()) return;

            qint64 take = qMin<qint64>(m_ctxobj->payloadRemaining, m_ctxobj->receiveBuffer.size());
            m_receiveBuffer.append(m_ctxobj->receiveBuffer.left(take));
            m_ctxobj->receiveBuffer.remove(0, take);
            m_ctxobj->payloadRemaining -= take;

            qDebug() << "[OnReadyRead] receiving large payload, append"
                     << take << "bytes, remaining =" << m_ctxobj->payloadRemaining;

            // 如果已经收齐整个 payload
            if (m_ctxobj->payloadRemaining == 0) {
                m_ctxobj->payload = m_receiveBuffer;
                qDebug() << "[OnReadyRead] 多包数据接收完成, payload size =" << m_ctxobj->payload.size()
                         << " identity =" << m_ctxobj->dialogIdentity
                         << " handle ptr =" << m_ctxobj->dialogHandle;

                emit dataPrased(m_ctxobj);

                m_ctxobj->receivingLarge = false;
                m_ctxobj->payloadRemaining = 0;
                m_receiveBuffer.clear();
                continue;
            } else {
                return;
            }
        }

        if (m_ctxobj->receiveBuffer.size() < 9) return;

        QDataStream headerStream(m_ctxobj->receiveBuffer);
        headerStream.setByteOrder(QDataStream::BigEndian);
        headerStream
            >> m_ctxobj->isToken
            >> m_ctxobj->dataLength
            >> m_ctxobj->checkNum;

        //单包
        if (m_ctxobj->dataLength <= PACKET_LENGTH - 9) {
            if (m_ctxobj->receiveBuffer.size() < PACKET_LENGTH) {
                // 还没有收到一个完整的后的单包
                qDebug()<<"[OnReadyRead] 未收到完整单包。";
                return;
            }

            QByteArray pkt = m_ctxobj->receiveBuffer.left(PACKET_LENGTH);
            m_ctxobj->receiveBuffer.remove(0, PACKET_LENGTH);

            //记录数据
            m_ctxobj->payload = pkt.mid(9, m_ctxobj->dataLength);

            qDebug() << "[OnReadyRead] 完整数据接收完成, payload size =" << m_ctxobj->payload.size()
                     << " identity =" << m_ctxobj->dialogIdentity
                     << " handle ptr =" << m_ctxobj->dialogHandle;

            emit dataPrased(m_ctxobj);
            continue;
        }
        //多包
        else {
            m_ctxobj->receiveBuffer.remove(0, 9);

            m_ctxobj->receivingLarge = true;
            m_ctxobj->payloadRemaining = m_ctxobj->dataLength;
            m_receiveBuffer.clear();

            qDebug() << "[OnReadyRead] 进入多包接收模式, totalPayload =" << m_ctxobj->dataLength
                     << " firstChunk(can be 0) =" << qMin(m_ctxobj->receiveBuffer.size(), (int)m_ctxobj->payloadRemaining)
                     << " remaining(before drain) =" << m_ctxobj->payloadRemaining;

            continue;

        }
    }
}

