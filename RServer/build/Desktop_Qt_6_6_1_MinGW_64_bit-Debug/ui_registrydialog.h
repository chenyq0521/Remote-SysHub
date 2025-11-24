/********************************************************************************
** Form generated from reading UI file 'registrydialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTRYDIALOG_H
#define UI_REGISTRYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegistryDialog
{
public:
    QWidget *centralwidget;
    QWidget *widget;
    QGridLayout *gridLayout;
    QTreeView *pathTreeView;
    QLineEdit *pathLineEdit;
    QTableView *valueTableView;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *RegistryDialog)
    {
        if (RegistryDialog->objectName().isEmpty())
            RegistryDialog->setObjectName("RegistryDialog");
        RegistryDialog->resize(1600, 900);
        centralwidget = new QWidget(RegistryDialog);
        centralwidget->setObjectName("centralwidget");
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(10, 10, 1581, 841));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        pathTreeView = new QTreeView(widget);
        pathTreeView->setObjectName("pathTreeView");
        pathTreeView->setMinimumSize(QSize(200, 0));
        pathTreeView->setMaximumSize(QSize(372, 16777215));

        gridLayout->addWidget(pathTreeView, 1, 0, 1, 1);

        pathLineEdit = new QLineEdit(widget);
        pathLineEdit->setObjectName("pathLineEdit");

        gridLayout->addWidget(pathLineEdit, 0, 0, 1, 2);

        valueTableView = new QTableView(widget);
        valueTableView->setObjectName("valueTableView");

        gridLayout->addWidget(valueTableView, 1, 1, 1, 1);

        RegistryDialog->setCentralWidget(centralwidget);
        menubar = new QMenuBar(RegistryDialog);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1600, 24));
        RegistryDialog->setMenuBar(menubar);
        statusbar = new QStatusBar(RegistryDialog);
        statusbar->setObjectName("statusbar");
        RegistryDialog->setStatusBar(statusbar);

        retranslateUi(RegistryDialog);

        QMetaObject::connectSlotsByName(RegistryDialog);
    } // setupUi

    void retranslateUi(QMainWindow *RegistryDialog)
    {
        RegistryDialog->setWindowTitle(QCoreApplication::translate("RegistryDialog", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegistryDialog: public Ui_RegistryDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTRYDIALOG_H
