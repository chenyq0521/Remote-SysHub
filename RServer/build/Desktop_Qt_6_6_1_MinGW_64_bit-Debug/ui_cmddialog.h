/********************************************************************************
** Form generated from reading UI file 'cmddialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CMDDIALOG_H
#define UI_CMDDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CmdDialog
{
public:
    QAction *settings;
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *terminal;
    QTextEdit *textEdit;
    QWidget *config;
    QPushButton *confirmButton;
    QPushButton *cancelButton;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QComboBox *themecomboBox;
    QLineEdit *sizeEdit;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QComboBox *fontcomboBox;
    QSpacerItem *horizontalSpacer;
    QStatusBar *statusbar;
    QMenuBar *menubar;
    QMenu *menu;

    void setupUi(QMainWindow *CmdDialog)
    {
        if (CmdDialog->objectName().isEmpty())
            CmdDialog->setObjectName("CmdDialog");
        CmdDialog->resize(1120, 630);
        CmdDialog->setMinimumSize(QSize(1120, 630));
        CmdDialog->setMaximumSize(QSize(1120, 630));
        settings = new QAction(CmdDialog);
        settings->setObjectName("settings");
        centralwidget = new QWidget(CmdDialog);
        centralwidget->setObjectName("centralwidget");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(0, 0, 1115, 625));
        stackedWidget->setMinimumSize(QSize(1115, 625));
        terminal = new QWidget();
        terminal->setObjectName("terminal");
        textEdit = new QTextEdit(terminal);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(10, 6, 1100, 580));
        textEdit->setMinimumSize(QSize(1100, 0));
        stackedWidget->addWidget(terminal);
        config = new QWidget();
        config->setObjectName("config");
        confirmButton = new QPushButton(config);
        confirmButton->setObjectName("confirmButton");
        confirmButton->setGeometry(QRect(880, 550, 80, 35));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        confirmButton->setFont(font);
        cancelButton = new QPushButton(config);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setGeometry(QRect(970, 550, 80, 35));
        cancelButton->setFont(font);
        layoutWidget = new QWidget(config);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(60, 50, 461, 271));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        themecomboBox = new QComboBox(layoutWidget);
        themecomboBox->addItem(QString());
        themecomboBox->addItem(QString());
        themecomboBox->setObjectName("themecomboBox");
        QFont font1;
        font1.setPointSize(12);
        themecomboBox->setFont(font1);

        gridLayout->addWidget(themecomboBox, 0, 2, 1, 1);

        sizeEdit = new QLineEdit(layoutWidget);
        sizeEdit->setObjectName("sizeEdit");

        gridLayout->addWidget(sizeEdit, 2, 2, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font2.setPointSize(16);
        label_3->setFont(font2);

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");
        label_2->setFont(font2);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName("label");
        label->setFont(font2);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        fontcomboBox = new QComboBox(layoutWidget);
        fontcomboBox->setObjectName("fontcomboBox");

        gridLayout->addWidget(fontcomboBox, 1, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 1, 3, 1);

        stackedWidget->addWidget(config);
        CmdDialog->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(CmdDialog);
        statusbar->setObjectName("statusbar");
        CmdDialog->setStatusBar(statusbar);
        menubar = new QMenuBar(CmdDialog);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1120, 20));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        CmdDialog->setMenuBar(menubar);

        menubar->addAction(menu->menuAction());
        menu->addAction(settings);

        retranslateUi(CmdDialog);

        stackedWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(CmdDialog);
    } // setupUi

    void retranslateUi(QMainWindow *CmdDialog)
    {
        CmdDialog->setWindowTitle(QCoreApplication::translate("CmdDialog", "MainWindow", nullptr));
        settings->setText(QCoreApplication::translate("CmdDialog", "\350\256\276\347\275\256", nullptr));
        confirmButton->setText(QCoreApplication::translate("CmdDialog", "\344\277\235\345\255\230", nullptr));
        cancelButton->setText(QCoreApplication::translate("CmdDialog", "\345\217\226\346\266\210", nullptr));
        themecomboBox->setItemText(0, QCoreApplication::translate("CmdDialog", "\346\267\261\350\211\262", nullptr));
        themecomboBox->setItemText(1, QCoreApplication::translate("CmdDialog", "\346\265\205\350\211\262", nullptr));

        label_3->setText(QCoreApplication::translate("CmdDialog", "\345\255\227\345\217\267", nullptr));
        label_2->setText(QCoreApplication::translate("CmdDialog", "\345\255\227\344\275\223", nullptr));
        label->setText(QCoreApplication::translate("CmdDialog", "\344\270\273\351\242\230", nullptr));
        menu->setTitle(QCoreApplication::translate("CmdDialog", "\350\217\234\345\215\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CmdDialog: public Ui_CmdDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CMDDIALOG_H
