#ifndef KERNELMANAGER_H
#define KERNELMANAGER_H

#include "absmanager.h"

class KernelManager:public AbsManager
{
    Q_OBJECT
public:
    explicit KernelManager(TcpClient* TClient, QObject *parent = nullptr);
    void HandlePacket(const PACKET &pkt) override;

    template <typename ManagerType>
    void CreateManager();

signals:
    void onConnected();
private:

};

#endif // KERNELMANAGER_H
