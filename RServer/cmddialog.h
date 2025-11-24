#ifndef CMDDIALOG_H
#define CMDDIALOG_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QFontDatabase>
#include <QPalette>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "common.h"
#include "serverdlg.h"
#include "tcpworker.h"

namespace Ui {
class CmdDialog;
}

class CmdDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit CmdDialog(TcpWorker* worker = nullptr,CONTEXT_OBJECT *ctx = nullptr, QWidget *parent = nullptr);
    ~CmdDialog();

    void SendCommand(const QString &cmd);
    void ShowOutput(const QString &text);
    void HandlePacket(unsigned char istoken,QByteArray data);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
private slots:

    void on_cancelButton_clicked();

    void on_confirmButton_clicked();

private:
    void LoadFonts();

    CONTEXT_OBJECT* m_ctxobj;
    TcpWorker *m_worker;
    Ui::CmdDialog *ui;
    int m_preInputLength = 0;
    int m_promptPosition = 0; //当前输入所在位置

    QString m_darkTheme = "QTextEdit { background-color: black; color: white; selection-background-color: #A9A9A9; }";
    QString m_lightTheme = "QTextEdit { background-color: white; color: black; selection-background-color: #383A42; }";
};

#endif // CMDDIALOG_H
