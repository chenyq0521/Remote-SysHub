/********************************************************************************
** Form generated from reading UI file 'processdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSDIALOG_H
#define UI_PROCESSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProcessDialog
{
public:
    QTableView *ProcessTableView;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *refreshButton;
    QLineEdit *PIDlineEdit;

    void setupUi(QWidget *ProcessDialog)
    {
        if (ProcessDialog->objectName().isEmpty())
            ProcessDialog->setObjectName("ProcessDialog");
        ProcessDialog->resize(1600, 900);
        ProcessDialog->setMinimumSize(QSize(1600, 900));
        ProcessDialog->setMaximumSize(QSize(1600, 900));
        ProcessTableView = new QTableView(ProcessDialog);
        ProcessTableView->setObjectName("ProcessTableView");
        ProcessTableView->setGeometry(QRect(5, 50, 1590, 845));
        ProcessTableView->setMinimumSize(QSize(1590, 20));
        ProcessTableView->setMaximumSize(QSize(1590, 900));
        widget = new QWidget(ProcessDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(20, 10, 851, 31));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        refreshButton = new QPushButton(widget);
        refreshButton->setObjectName("refreshButton");

        horizontalLayout->addWidget(refreshButton);

        PIDlineEdit = new QLineEdit(widget);
        PIDlineEdit->setObjectName("PIDlineEdit");

        horizontalLayout->addWidget(PIDlineEdit);


        retranslateUi(ProcessDialog);

        QMetaObject::connectSlotsByName(ProcessDialog);
    } // setupUi

    void retranslateUi(QWidget *ProcessDialog)
    {
        ProcessDialog->setWindowTitle(QCoreApplication::translate("ProcessDialog", "Form", nullptr));
        refreshButton->setText(QCoreApplication::translate("ProcessDialog", "\345\210\267\346\226\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProcessDialog: public Ui_ProcessDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROCESSDIALOG_H
