#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "absmanager.h"

MainWindow * __MainWindow__ = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    __MainWindow__ = this;
    ui->recordText->setReadOnly(true);
    //ui->recordText->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LogMessage(const QString &message)
{
    ui->recordText->append(message);
}
