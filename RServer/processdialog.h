#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QWidget>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include "common.h"
#include "serverdlg.h"
#include "tcpworker.h"

namespace Ui {
class ProcessDialog;
}

class ProcessDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessDialog(TcpWorker* worker = nullptr,CONTEXT_OBJECT *ctx = nullptr, QWidget *parent = nullptr);
    ~ProcessDialog();

    void HandlePacket(unsigned char istoken,QByteArray data);
    void CreateInjectDialog(quint32 pid);
private slots:
    void on_refreshButton_clicked();

    void on_ProcessTableView_customContextMenuRequested(const QPoint &pos);

    void SendRefreshRequest();
    void SendOperateRequest(unsigned char OperToken);


    void on_PIDlineEdit_textChanged(const QString &pid);

private:

    void SetProcessTableView();
    void ShowClientProcessList(QByteArray data);

    CONTEXT_OBJECT* m_ctxobj;
    TcpWorker *m_worker;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    Ui::ProcessDialog *ui;
};




#endif // PROCESSDIALOG_H
