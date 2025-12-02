#include "windowdialog.h"
#include "ui_windowdialog.h"

WindowDialog::WindowDialog(TcpWorker *worker, CONTEXT_OBJECT* ctx,QWidget *parent)
    : QMainWindow(parent), m_ctxobj(ctx), m_worker(worker), ui(new Ui::WindowDialog)
{
    ui->setupUi(this);
    m_model = new QStandardItemModel(this);
    SetWindowTableView();
}

WindowDialog::~WindowDialog()
{
    delete ui;
}

void WindowDialog::HandlePacket(unsigned char isToken, QByteArray data)
{
    qDebug()<<"window dialog is handeling…";
    switch(isToken)
    {
    case WINDOW_OPERATION_REPLY:
        ShowClientWindowList(data);
        break;
    default:
        qDebug()<<"unknown token"<<isToken;
        break;
    }
}

void WindowDialog::SetWindowTableView()
{

    QTableView *windowView = ui->windowTableView;

    windowView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    windowView->setSelectionBehavior(QAbstractItemView::SelectRows);
    windowView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_model->setColumnCount(3);
    m_model->setHorizontalHeaderLabels({"窗口句柄","窗口名称","窗口状态"});

    windowView -> setFocusPolicy(Qt::NoFocus);
    windowView -> setModel(m_model);

}

void WindowDialog::ShowClientWindowList(QByteArray payload)
{
    QSet<QString> updatedHwnds;
    //m_model->removeRows(0, m_model->rowCount());
    QDataStream stream(&payload,QIODevice::ReadOnly);
    //stream.setByteOrder(QDataStream::BigEndian);
    while (!stream.atEnd()) {
        quint64 hwndVal;
        QString title, state;

        stream >> hwndVal >> title >> state;

        QString hwndStr = QString::number(hwndVal);
        updatedHwnds.insert(hwndStr);

        //处理隐藏逻辑
        bool found = false;
        for (int r = 0; r < m_model->rowCount(); r++) {
            if (m_model->item(r,0)->text() == hwndStr) {
                m_model->item(r,1)->setText(title);
                m_model->item(r,2)->setText(state);
                found = true;
                break;
            }
        }

        if (!found) {
            QList<QStandardItem*> row;
            row << new QStandardItem(hwndStr);
            row << new QStandardItem(title);
            row << new QStandardItem(state);
            m_model->appendRow(row);
        }
    }

    for (int r = m_model->rowCount() - 1; r >= 0; r--) {
        QString hwndStr = m_model->item(r,0)->text();
        QString state = m_model->item(r,2)->text();

        if (!updatedHwnds.contains(hwndStr)) {
            if (state == "隐藏") continue;
             else   m_model->removeRow(r);
        }
    }

}

void WindowDialog::SendControlRequest(unsigned char Token, DWORD subkey)
{
    auto selectedRow = ui->windowTableView->selectionModel()->currentIndex();
    if(!selectedRow.isValid()) return;

    int Row = selectedRow.row();
    HWND hwnd = (HWND)m_model->item(Row,0)->text().toULongLong();

    QDataStream stream(&m_ctxobj->payload,QIODevice::WriteOnly);

    stream << (quint64)(uintptr_t)hwnd;
    stream << (quint32)subkey;

    m_ctxobj->isToken = Token;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

}

void WindowDialog::on_refreshButton_clicked()
{
    m_ctxobj->payload.clear();
    m_ctxobj->isToken = WINDOW_REFRESH_REQUIRE;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}


void WindowDialog::on_hideButton_clicked()
{
    SendControlRequest(WINDOW_HIDE_REQUIRE, SW_HIDE);
    auto selectedRow = ui->windowTableView->selectionModel()->currentIndex();
    if (selectedRow.isValid()) {
        int row = selectedRow.row();
        m_model->item(row, 2)->setText("隐藏");
    }
}


void WindowDialog::on_restoreButton_clicked()
{
    SendControlRequest(WINDOW_RESTORE_REQUIRE, SW_RESTORE);
}


void WindowDialog::on_maximizeButton_clicked()
{
    SendControlRequest(WINDOW_MAX_REQUIRE, SW_SHOWMAXIMIZED);
}


void WindowDialog::on_minimizeButton_clicked()
{
    SendControlRequest(WINDOW_MIN_REQUIRE, SW_SHOWMINIMIZED);
}


void WindowDialog::on_closeButton_clicked()
{
    SendControlRequest(WINDOW_CLOSE_REQUIRE, WM_CLOSE);
}

