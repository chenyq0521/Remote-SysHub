/********************************************************************************
** Form generated from reading UI file 'registrydatadialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTRYDATADIALOG_H
#define UI_REGISTRYDATADIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegistryDataDialog
{
public:
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QStackedWidget *stackedWidget;
    QWidget *page_1;
    QTextEdit *SZTextEdit;
    QWidget *page_4;
    QTextEdit *binaryTextEdit;
    QWidget *page_3;
    QLineEdit *valueLineEdit;
    QWidget *page_2;
    QLineEdit *SZLineEdit;
    QLabel *dataLabel;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *confirmButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *RegistryDataDialog)
    {
        if (RegistryDataDialog->objectName().isEmpty())
            RegistryDataDialog->setObjectName("RegistryDataDialog");
        RegistryDataDialog->resize(800, 600);
        nameLabel = new QLabel(RegistryDataDialog);
        nameLabel->setObjectName("nameLabel");
        nameLabel->setGeometry(QRect(30, 30, 191, 31));
        QFont font;
        font.setPointSize(11);
        nameLabel->setFont(font);
        nameEdit = new QLineEdit(RegistryDataDialog);
        nameEdit->setObjectName("nameEdit");
        nameEdit->setGeometry(QRect(30, 80, 741, 31));
        stackedWidget = new QStackedWidget(RegistryDataDialog);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(10, 180, 781, 361));
        page_1 = new QWidget();
        page_1->setObjectName("page_1");
        SZTextEdit = new QTextEdit(page_1);
        SZTextEdit->setObjectName("SZTextEdit");
        SZTextEdit->setGeometry(QRect(20, 0, 741, 351));
        stackedWidget->addWidget(page_1);
        page_4 = new QWidget();
        page_4->setObjectName("page_4");
        binaryTextEdit = new QTextEdit(page_4);
        binaryTextEdit->setObjectName("binaryTextEdit");
        binaryTextEdit->setGeometry(QRect(20, 0, 741, 351));
        stackedWidget->addWidget(page_4);
        page_3 = new QWidget();
        page_3->setObjectName("page_3");
        valueLineEdit = new QLineEdit(page_3);
        valueLineEdit->setObjectName("valueLineEdit");
        valueLineEdit->setGeometry(QRect(20, 40, 741, 31));
        stackedWidget->addWidget(page_3);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        SZLineEdit = new QLineEdit(page_2);
        SZLineEdit->setObjectName("SZLineEdit");
        SZLineEdit->setGeometry(QRect(20, 40, 741, 31));
        stackedWidget->addWidget(page_2);
        dataLabel = new QLabel(RegistryDataDialog);
        dataLabel->setObjectName("dataLabel");
        dataLabel->setGeometry(QRect(30, 130, 190, 31));
        dataLabel->setFont(font);
        layoutWidget = new QWidget(RegistryDataDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(600, 550, 169, 31));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        confirmButton = new QPushButton(layoutWidget);
        confirmButton->setObjectName("confirmButton");

        horizontalLayout->addWidget(confirmButton);

        cancelButton = new QPushButton(layoutWidget);
        cancelButton->setObjectName("cancelButton");

        horizontalLayout->addWidget(cancelButton);


        retranslateUi(RegistryDataDialog);

        stackedWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(RegistryDataDialog);
    } // setupUi

    void retranslateUi(QDialog *RegistryDataDialog)
    {
        RegistryDataDialog->setWindowTitle(QCoreApplication::translate("RegistryDataDialog", "Dialog", nullptr));
        nameLabel->setText(QCoreApplication::translate("RegistryDataDialog", "\346\225\260\345\200\274\345\220\215\347\247\260\357\274\232", nullptr));
        dataLabel->setText(QCoreApplication::translate("RegistryDataDialog", "\346\225\260\345\200\274\346\225\260\346\215\256\357\274\232", nullptr));
        confirmButton->setText(QCoreApplication::translate("RegistryDataDialog", "\347\241\256\345\256\232", nullptr));
        cancelButton->setText(QCoreApplication::translate("RegistryDataDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegistryDataDialog: public Ui_RegistryDataDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTRYDATADIALOG_H
