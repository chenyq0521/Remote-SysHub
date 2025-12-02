#include "cmddialog.h"
#include "ui_cmddialog.h"
#include <QChar>

CmdDialog::CmdDialog(TcpWorker *worker, CONTEXT_OBJECT* ctx,QWidget *parent)
    : QMainWindow(parent), m_ctxobj(ctx), m_worker(worker), ui(new Ui::CmdDialog)
{
    ui->setupUi(this);
    ui->textEdit->installEventFilter(this);
    ui->stackedWidget->setCurrentWidget(ui->terminal);
    connect(ui->settings,&QAction::triggered,this,[=]{
        QPalette p = ui->textEdit->palette();
        QColor bg = p.color(QPalette::Base);
        if (bg == QColor(Qt::black)) {
            ui->themecomboBox->setCurrentText("深色");
        } else {
            ui->themecomboBox->setCurrentText("浅色");
        }
        ui->fontcomboBox->setCurrentText(ui->textEdit->font().family());
        ui->sizeEdit->setText(QString::number(ui->textEdit->font().pointSize()));
        ui->stackedWidget->setCurrentWidget(ui->config);
    });

    QFont defaultfont("Cascadia Code", 12);
    ui->textEdit->setFont(defaultfont);
    ui->textEdit->setStyleSheet(m_lightTheme);
    QRegularExpression re("^([6-9]|[1-6][0-9]|7[0-2])$");
    ui->sizeEdit->setValidator(new QRegularExpressionValidator(re, this));
    LoadFonts();

}

CmdDialog::~CmdDialog()
{
    delete ui;
}

void CmdDialog::ShowOutput(const QString &text)
{
    QTextCursor cursor(ui->textEdit->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text);
    m_promptPosition = ui->textEdit->document()->characterCount() - 1;
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
}

void CmdDialog::HandlePacket(unsigned char istoken, QByteArray data)
{
    qDebug() <<"cmd dialog is handeling…";
    switch(istoken)
    {
    case CMD_OUTPUT:
    {
        QString info = QString::fromUtf8(data);
        if (m_preInputLength > 0 && info.length() >= m_preInputLength) {
            info = info.mid(m_preInputLength);
            m_preInputLength = 0;
        }
        ShowOutput(info);
        break;
    }
    default:
        qDebug()<<"unknown token"<<istoken;
        break;
    }
}

bool CmdDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->textEdit) {
        //处理 Ctrl+滚轮字体缩放
        if (event->type() == QEvent::Wheel) {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
                int numDegrees = wheelEvent->angleDelta().y() / 8;
                int numSteps = numDegrees / 15;
                QFont font = ui->textEdit->font();
                int newSize = font.pointSize() + numSteps;
                if (newSize < 6) newSize = 6;
                if (newSize > 72) newSize = 72;
                font.setPointSize(newSize);
                ui->textEdit->setFont(font);

                ui->sizeEdit->setText(QString::number(newSize));

                return true;
            }
        }
    }
    if (obj == ui->textEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QTextCursor cursor = ui->textEdit->textCursor();

        // 限制光标
        if (cursor.position() < m_promptPosition) {
            cursor.setPosition(m_promptPosition);
            ui->textEdit->setTextCursor(cursor);
        }

        // 回车
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            QString cmd = ui->textEdit->toPlainText().mid(m_promptPosition);
            m_preInputLength = cmd.length();

            m_ctxobj->payload = cmd.toUtf8();
            m_ctxobj->isToken = CMD_INPUT;
            QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
            return true;
        }


        if ((keyEvent->key() == Qt::Key_Backspace
             || keyEvent->key() == Qt::Key_Delete
             || keyEvent->key() == Qt::Key_Up
             || keyEvent->key() == Qt::Key_Left)
            && cursor.position() <= m_promptPosition) {
            return true;
        }


    }
    return QMainWindow::eventFilter(obj, event);
}


void CmdDialog::on_cancelButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->terminal);
}


void CmdDialog::on_confirmButton_clicked()
{
    //设置主题
    if(ui->themecomboBox->currentText() == "深色")
    {
        ui->textEdit->setStyleSheet(m_darkTheme);
    }
    else if(ui->themecomboBox->currentText() == "浅色")
    {
        ui->textEdit->setStyleSheet(m_lightTheme);
    }
    //设置字体与字号
    int size = ui->sizeEdit->text().toInt();
    QString style = ui->fontcomboBox->currentText();
    QFont font(style,size);
    ui->textEdit->setFont(font);

    ui->stackedWidget->setCurrentWidget(ui->terminal);
}

void CmdDialog::LoadFonts()
{
    QFontDatabase fontDB;
    ui->fontcomboBox->clear();
    ui->fontcomboBox->addItems(fontDB.families(QFontDatabase::Any)); // 加载系统所有字体
    int index = ui->fontcomboBox->findText("Cascadia Code", Qt::MatchExactly);
    if (index >= 0) {
        ui->fontcomboBox->setCurrentIndex(index);
    }
}

