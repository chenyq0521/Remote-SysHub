#ifndef SERVERDLG_H
#define SERVERDLG_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QComboBox>
#include <QMap>
#include "tcpserver.h"
#include "common.h"

class test;

QT_BEGIN_NAMESPACE
namespace Ui {
class ServerDlg;
}
QT_END_NAMESPACE

class ServerDlg : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerDlg(QWidget *parent = nullptr);
    TcpServer* GetTcpServer() const{
        return m_tcpServer;
    }
    ~ServerDlg();

private slots:

    //控件响应
    void on_ListenButton_clicked();
    void on_CmdButton_clicked();
    void on_ProcessButton_clicked();
    void on_WindowButton_clicked();
    //数据处理
    void HandlePacket(CONTEXT_OBJECT *ctx);

    //处理客户端登录信息
    void HandleClientLogin(CONTEXT_OBJECT *ctx);
    void AddLoginToClientTable(const LOGIN_INFORMAITON& info);
    void RemoveClient(qintptr socketDescriptor);

    void CreateWorker(unsigned char istoken);


    void on_RegistryButton_clicked();

    void on_FileButton_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:

private:

    //处理功能业务建立逻辑
    template<typename FuncClass,unsigned char isToken,unsigned char DialogID>
    void HandleReply(CONTEXT_OBJECT *ctx);

    Ui::ServerDlg *ui;
    TcpServer *m_tcpServer;
    QStandardItemModel *m_model;
    int m_selectedRow;
    qintptr m_selectedDescriptor;
    QMap<qintptr, QMap<unsigned char,CONTEXT_OBJECT*>> m_functionConnections;

    void SetClientTableView();
    void ClearClientTableView();

};

#endif // SERVERDLG_H
