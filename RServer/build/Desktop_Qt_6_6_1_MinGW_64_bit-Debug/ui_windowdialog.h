/********************************************************************************
** Form generated from reading UI file 'windowdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOWDIALOG_H
#define UI_WINDOWDIALOG_H

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

class Ui_WindowDialog
{
public:
    QWidget *centralwidget;
    QTableView *windowTableView;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *refreshButton;
    QPushButton *hideButton;
    QPushButton *restoreButton;
    QPushButton *maximizeButton;
    QPushButton *minimizeButton;
    QPushButton *closeButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *WindowDialog)
    {
        if (WindowDialog->objectName().isEmpty())
            WindowDialog->setObjectName("WindowDialog");
        WindowDialog->resize(1280, 720);
        WindowDialog->setMinimumSize(QSize(1280, 720));
        WindowDialog->setMaximumSize(QSize(1280, 720));
        centralwidget = new QWidget(WindowDialog);
        centralwidget->setObjectName("centralwidget");
        windowTableView = new QTableView(centralwidget);
        windowTableView->setObjectName("windowTableView");
        windowTableView->setGeometry(QRect(5, 50, 1270, 620));
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(0, 0, 471, 41));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        refreshButton = new QPushButton(widget);
        refreshButton->setObjectName("refreshButton");

        horizontalLayout->addWidget(refreshButton);

        hideButton = new QPushButton(widget);
        hideButton->setObjectName("hideButton");

        horizontalLayout->addWidget(hideButton);

        restoreButton = new QPushButton(widget);
        restoreButton->setObjectName("restoreButton");

        horizontalLayout->addWidget(restoreButton);

        maximizeButton = new QPushButton(widget);
        maximizeButton->setObjectName("maximizeButton");

        horizontalLayout->addWidget(maximizeButton);

        minimizeButton = new QPushButton(widget);
        minimizeButton->setObjectName("minimizeButton");

        horizontalLayout->addWidget(minimizeButton);

        closeButton = new QPushButton(widget);
        closeButton->setObjectName("closeButton");

        horizontalLayout->addWidget(closeButton);

        WindowDialog->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WindowDialog);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1280, 20));
        WindowDialog->setMenuBar(menubar);
        statusbar = new QStatusBar(WindowDialog);
        statusbar->setObjectName("statusbar");
        WindowDialog->setStatusBar(statusbar);

        retranslateUi(WindowDialog);

        QMetaObject::connectSlotsByName(WindowDialog);
    } // setupUi

    void retranslateUi(QMainWindow *WindowDialog)
    {
        WindowDialog->setWindowTitle(QCoreApplication::translate("WindowDialog", "MainWindow", nullptr));
        refreshButton->setText(QCoreApplication::translate("WindowDialog", "\345\210\267\346\226\260", nullptr));
        hideButton->setText(QCoreApplication::translate("WindowDialog", "\351\232\220\350\227\217", nullptr));
        restoreButton->setText(QCoreApplication::translate("WindowDialog", "\346\201\242\345\244\215", nullptr));
        maximizeButton->setText(QCoreApplication::translate("WindowDialog", "\346\234\200\345\244\247\345\214\226", nullptr));
        minimizeButton->setText(QCoreApplication::translate("WindowDialog", "\346\234\200\345\260\217\345\214\226", nullptr));
        closeButton->setText(QCoreApplication::translate("WindowDialog", "\345\205\263\351\227\255", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WindowDialog: public Ui_WindowDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOWDIALOG_H
