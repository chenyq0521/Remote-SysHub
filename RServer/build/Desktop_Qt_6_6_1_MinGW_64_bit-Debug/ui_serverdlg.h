/********************************************************************************
** Form generated from reading UI file 'serverdlg.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERDLG_H
#define UI_SERVERDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerDlg
{
public:
    QWidget *centralwidget;
    QTableView *ClientTableView;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *ListenButton;
    QPushButton *CmdButton;
    QPushButton *WindowButton;
    QPushButton *ProcessButton;
    QPushButton *RegistryButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ServerDlg)
    {
        if (ServerDlg->objectName().isEmpty())
            ServerDlg->setObjectName("ServerDlg");
        ServerDlg->resize(1366, 768);
        ServerDlg->setMinimumSize(QSize(1366, 768));
        ServerDlg->setMaximumSize(QSize(1366, 768));
        centralwidget = new QWidget(ServerDlg);
        centralwidget->setObjectName("centralwidget");
        ClientTableView = new QTableView(centralwidget);
        ClientTableView->setObjectName("ClientTableView");
        ClientTableView->setGeometry(QRect(8, 60, 1350, 220));
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(20, 10, 461, 41));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        ListenButton = new QPushButton(widget);
        ListenButton->setObjectName("ListenButton");

        horizontalLayout->addWidget(ListenButton);

        CmdButton = new QPushButton(widget);
        CmdButton->setObjectName("CmdButton");

        horizontalLayout->addWidget(CmdButton);

        WindowButton = new QPushButton(widget);
        WindowButton->setObjectName("WindowButton");

        horizontalLayout->addWidget(WindowButton);

        ProcessButton = new QPushButton(widget);
        ProcessButton->setObjectName("ProcessButton");

        horizontalLayout->addWidget(ProcessButton);

        RegistryButton = new QPushButton(widget);
        RegistryButton->setObjectName("RegistryButton");

        horizontalLayout->addWidget(RegistryButton);

        ServerDlg->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ServerDlg);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1366, 24));
        ServerDlg->setMenuBar(menubar);
        statusbar = new QStatusBar(ServerDlg);
        statusbar->setObjectName("statusbar");
        ServerDlg->setStatusBar(statusbar);

        retranslateUi(ServerDlg);

        QMetaObject::connectSlotsByName(ServerDlg);
    } // setupUi

    void retranslateUi(QMainWindow *ServerDlg)
    {
        ServerDlg->setWindowTitle(QCoreApplication::translate("ServerDlg", "MainWindow", nullptr));
        ListenButton->setText(QCoreApplication::translate("ServerDlg", "\345\220\257\345\212\250\346\234\215\345\212\241\345\231\250", nullptr));
        CmdButton->setText(QCoreApplication::translate("ServerDlg", "\347\273\210\347\253\257", nullptr));
        WindowButton->setText(QCoreApplication::translate("ServerDlg", "\347\252\227\345\217\243", nullptr));
        ProcessButton->setText(QCoreApplication::translate("ServerDlg", "\350\277\233\347\250\213", nullptr));
        RegistryButton->setText(QCoreApplication::translate("ServerDlg", "\346\263\250\345\206\214\350\241\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ServerDlg: public Ui_ServerDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERDLG_H
