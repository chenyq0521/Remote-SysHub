/********************************************************************************
** Form generated from reading UI file 'processmemorydialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSMEMORYDIALOG_H
#define UI_PROCESSMEMORYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_ProcessMemoryDialog
{
public:
    QTableView *MemoryTableView;

    void setupUi(QDialog *ProcessMemoryDialog)
    {
        if (ProcessMemoryDialog->objectName().isEmpty())
            ProcessMemoryDialog->setObjectName("ProcessMemoryDialog");
        ProcessMemoryDialog->resize(960, 800);
        MemoryTableView = new QTableView(ProcessMemoryDialog);
        MemoryTableView->setObjectName("MemoryTableView");
        MemoryTableView->setGeometry(QRect(20, 10, 920, 780));

        retranslateUi(ProcessMemoryDialog);

        QMetaObject::connectSlotsByName(ProcessMemoryDialog);
    } // setupUi

    void retranslateUi(QDialog *ProcessMemoryDialog)
    {
        ProcessMemoryDialog->setWindowTitle(QCoreApplication::translate("ProcessMemoryDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProcessMemoryDialog: public Ui_ProcessMemoryDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROCESSMEMORYDIALOG_H
