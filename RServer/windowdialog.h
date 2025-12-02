#ifndef WINDOWDIALOG_H
#define WINDOWDIALOG_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QDebug>
#include <QMessageBox>
#include <QVector>
#include "common.h"
#include "tcpworker.h"

namespace Ui {
class WindowDialog;
}

class WindowDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowDialog(TcpWorker* worker = nullptr,CONTEXT_OBJECT *ctx = nullptr, QWidget *parent = nullptr);
    ~WindowDialog();

    void HandlePacket(unsigned char isToken , QByteArray data);

private slots:

    void on_refreshButton_clicked();
    void on_hideButton_clicked();
    void on_restoreButton_clicked();
    void on_maximizeButton_clicked();
    void on_minimizeButton_clicked();
    void on_closeButton_clicked();

private:
    CONTEXT_OBJECT *m_ctxobj;
    TcpWorker *m_worker;

    QStandardItemModel *m_model;

    Ui::WindowDialog *ui;

    void SetWindowTableView();
    void ShowClientWindowList(QByteArray payload);

    void SendControlRequest(unsigned char Token,DWORD subkey);

};

#endif // WINDOWDIALOG_H
