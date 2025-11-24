/********************************************************************************
** Form generated from reading UI file 'clientdlg.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTDLG_H
#define UI_CLIENTDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientDlg
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ClientDlg)
    {
        if (ClientDlg->objectName().isEmpty())
            ClientDlg->setObjectName("ClientDlg");
        ClientDlg->resize(800, 600);
        centralwidget = new QWidget(ClientDlg);
        centralwidget->setObjectName("centralwidget");
        ClientDlg->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ClientDlg);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 24));
        ClientDlg->setMenuBar(menubar);
        statusbar = new QStatusBar(ClientDlg);
        statusbar->setObjectName("statusbar");
        ClientDlg->setStatusBar(statusbar);

        retranslateUi(ClientDlg);

        QMetaObject::connectSlotsByName(ClientDlg);
    } // setupUi

    void retranslateUi(QMainWindow *ClientDlg)
    {
        ClientDlg->setWindowTitle(QCoreApplication::translate("ClientDlg", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientDlg: public Ui_ClientDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTDLG_H
