#include "processdialog.h"
#include "ui_processdialog.h"
#include "processmemorydialog.h"
#include "processinject.h"

ProcessDialog::ProcessDialog(TcpWorker *worker, CONTEXT_OBJECT* ctx,QWidget *parent)
    : QWidget(parent), m_ctxobj(ctx), m_worker(worker), ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);

    ui->PIDlineEdit->setPlaceholderText("输入PID进行搜索");
    QRegularExpression exp("\\d*");
    ui->PIDlineEdit->setValidator(new QRegularExpressionValidator(exp, this));

    m_model = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive); // 不区分大小写
    m_proxyModel->setFilterKeyColumn(0);

    SetProcessTableView();
}

ProcessDialog::~ProcessDialog()
{
    delete ui;
}

void ProcessDialog::HandlePacket(unsigned char istoken, QByteArray data)
{
    qDebug() <<"process dialog is handeling…";
    switch(istoken)
    {
    case PROCESS_OPERATION_REPLY:
        ShowClientProcessList(data);
        break;
    case PROCESS_MEMORY_REPLY:
    {
        ProcessMemoryDialog *dlg = new ProcessMemoryDialog(this,data);
        dlg->show();
        break;
    }
    default:
        qDebug()<<"unknown token"<<istoken;
        break;
    }
}

void ProcessDialog::CreateInjectDialog(quint32 pid)
{
    //创建对话框
    processinject *dlg = new processinject(this,pid);
    dlg->show();

    connect(dlg,&processinject::InstallHook,this,[=](quint8 choice){
        qDebug()<<"[Process] send hook install for"<<pid;
        m_ctxobj->payload.clear();
        QDataStream stream(&m_ctxobj->payload,QIODevice::WriteOnly);
        stream << pid << choice;

        m_ctxobj->isToken = PROCESS_HOOK_INSTALL;
        QMetaObject::invokeMethod(m_worker,"SendData",Qt::QueuedConnection);

    });//install


    connect(dlg,&processinject::UninstallHook,this,[=](quint8 choice){
        qDebug()<<"[Process] send hook uninstall for"<<pid;
        m_ctxobj->payload.clear();
        QDataStream stream(&m_ctxobj->payload,QIODevice::WriteOnly);
        stream << pid << choice;

        m_ctxobj->isToken = PROCESS_HOOK_UNINSTALL;
        QMetaObject::invokeMethod(m_worker,"SendData",Qt::QueuedConnection);
    });//uninstall




}

void ProcessDialog::SetProcessTableView()
{
    QTableView *processView = ui->ProcessTableView;

    processView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    processView->setSelectionBehavior(QAbstractItemView::SelectRows);
    processView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_model->setColumnCount(8);
    m_model->setHorizontalHeaderLabels({"PID","名称","程序路径","类型","状态","句柄","线程","体系结构"});

    processView->setFocusPolicy(Qt::NoFocus);
    processView->setModel(m_proxyModel);

    processView->setSortingEnabled(true);
    processView->setContextMenuPolicy(Qt::CustomContextMenu);

}

void ProcessDialog::ShowClientProcessList(QByteArray data)
{
    m_model->removeRows(0, m_model->rowCount()); // 清空旧数据

    QDataStream in(&data, QIODevice::ReadOnly);

    while (!in.atEnd()) {
        quint32 pid;
        QString imageName, processPath, processType, threadStatus, processBit;
        quint32 handleNum, threadNum;

        //按客户端的发送顺序读取
        in  >> pid
            >> imageName
            >> processPath
            >> processType
            >> threadStatus
            >> handleNum
            >> threadNum
            >> processBit;

        QList<QStandardItem*> row;

        //给表格填一行
        row << new QStandardItem(QString::number(pid));
        row << new QStandardItem(imageName);
        row << new QStandardItem(processPath);
        row << new QStandardItem(processType);
        row << new QStandardItem(threadStatus);
        row << new QStandardItem(QString::number(handleNum));
        row << new QStandardItem(QString::number(threadNum));
        row << new QStandardItem(processBit);

        m_model->appendRow(row);
    }
}

void ProcessDialog::on_refreshButton_clicked()
{
    SendRefreshRequest();
}


void ProcessDialog::on_ProcessTableView_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui->ProcessTableView->indexAt(pos);
    QMenu ProcessMenu;
    QAction *RefreshProcess = new QAction("刷新");
    QAction *KillProcess = new QAction("结束进程");
    QAction *SuspendProcess = new QAction("挂起");
    QAction *ResumeProcess = new QAction("恢复");
    QAction *ProcessInject = new QAction("进程劫持");
    QAction *ProcessMemory = new QAction("进程内存");
    if(index.isValid())
    {
        ProcessMenu.addAction(RefreshProcess);
        ProcessMenu.addAction(KillProcess);
        ProcessMenu.addAction(SuspendProcess);
        ProcessMenu.addAction(ResumeProcess);
        ProcessMenu.addSeparator();
        ProcessMenu.addAction(ProcessInject);
        ProcessMenu.addAction(ProcessMemory);
    }
    else
    {
        ProcessMenu.addAction(RefreshProcess);
    }

    connect(RefreshProcess , &QAction::triggered ,this,[=](){SendRefreshRequest();});
    connect(KillProcess,&QAction::triggered,this, [=](){SendOperateRequest(PROCESS_KILL_REQUIURE);});
    connect(SuspendProcess,&QAction::triggered,this, [=](){SendOperateRequest(PROCESS_SUSPEND_REQUIRE);});
    connect(ResumeProcess,&QAction::triggered,this, [=](){SendOperateRequest(PROCESS_RESUME_REQUIRE);});
    connect(ProcessMemory,&QAction::triggered,this, [=](){SendOperateRequest(PROCESS_MEMORY_REQUIRE);});
    connect(ProcessInject,&QAction::triggered,this, [=](){
        //创建窗口
        auto selectedRow = ui->ProcessTableView->selectionModel()->currentIndex();
        if(!selectedRow.isValid()) return;

        QModelIndex sourceIndex = m_proxyModel->mapToSource(selectedRow);
        int row = sourceIndex.row();
        quint32 processID = m_model->item(row, 0)->text().toUInt();
        CreateInjectDialog(processID);
    });

    ProcessMenu.exec(ui->ProcessTableView->viewport()->mapToGlobal(pos));
}

void ProcessDialog::SendRefreshRequest()
{
    m_ctxobj->payload.clear();
    m_ctxobj->isToken = PROCESS_REFRESH_RQRUIRE;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}

void ProcessDialog::SendOperateRequest(unsigned char OperToken)
{
    auto selectedRow = ui->ProcessTableView->selectionModel()->currentIndex();
    if(!selectedRow.isValid()) return;

    QModelIndex sourceIndex = m_proxyModel->mapToSource(selectedRow);
    int row = sourceIndex.row();

    quint32 processID = m_model->item(row, 0)->text().toUInt();
    qDebug()<<"[ProcessDialog] send opreation request"<<processID;

    m_ctxobj->payload.clear();
    QDataStream stream(&m_ctxobj->payload,QIODevice::WriteOnly);
    stream << processID;

    m_ctxobj->isToken = OperToken;
    QMetaObject::invokeMethod(m_worker,"SendData",Qt::QueuedConnection);

}


void ProcessDialog::on_PIDlineEdit_textChanged(const QString &pid)
{
    if (pid.isEmpty()) {
        // 清空搜索，恢复显示所有
        m_proxyModel->setFilterRegularExpression(QString());
    } else {
        QRegularExpression regex(QString(".*%1.*").arg(pid));
        m_proxyModel->setFilterRegularExpression(regex);
    }
}

