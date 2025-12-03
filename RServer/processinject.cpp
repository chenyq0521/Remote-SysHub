#include "processinject.h"
#include "ui_processinject.h"

processinject::processinject(QWidget *parent, quint32 pid)
    : QDialog(parent)
    , ui(new Ui::processinject)
{
    ui->setupUi(this);
    m_pid = pid;
}

processinject::~processinject()
{
    delete ui;
}

void processinject::on_InstallBtn_clicked()
{
    //获得选框第几个
    quint8 choice = ui->comboBox->currentIndex();
    qDebug()<<choice;

    //发消息()
    emit InstallHook(choice);
    //TerminateProcess（0）   OpenProcess（1）  GetModuleHandleW（2）

}


void processinject::on_UninstallBtn_clicked()
{
    //获得选框第几个
    quint8 choice = ui->comboBox->currentIndex();
    qDebug()<<choice;
    emit UninstallHook(choice);
}

