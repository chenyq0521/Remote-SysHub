#include "registrydatadialog.h"
#include "ui_registrydatadialog.h"
RegistryDataDialog::RegistryDataDialog(QString name,MREG_VALUE type,QString data,QWidget *parent)
    : QDialog(parent), ui(new Ui::RegistryDataDialog)
{
    m_name = name; m_type = type;
    ui->setupUi(this);
    ui->nameEdit->setEnabled(false);
    ui->nameEdit->setText(name);

    QRegularExpression exp3("\\d*"),exp4("[^0-9A-F]");
    ui->valueLineEdit->setValidator((new QRegularExpressionValidator(exp3, this)));


    connect(ui->binaryTextEdit, &QTextEdit::textChanged, this, [=]() {
        QString text = ui->binaryTextEdit->toPlainText();
        int oldCursor = ui->binaryTextEdit->textCursor().position();

        text.remove(' ');
        text = text.toUpper();
        text.remove((exp4));

        QString formatted;
        int addedCursor = 0;
        for (int i = 0; i < text.length(); i++) {
            formatted.append(text[i]);
            if ((i + 1) % 2 == 0 && i != text.length() - 1) {
                formatted.append(' ');
                if(i < oldCursor) addedCursor++;
            }
        }

        // 阻止递归信号
        if (formatted != ui->binaryTextEdit->toPlainText()) {
            ui->binaryTextEdit->blockSignals(true);
            ui->binaryTextEdit->setPlainText(formatted);

            int cursorPos = oldCursor + addedCursor;
            QTextCursor cursor = ui->binaryTextEdit->textCursor();
            cursor.setPosition(qMin(cursorPos, formatted.length()));
            ui->binaryTextEdit->setTextCursor(cursor);
            ui->binaryTextEdit->blockSignals(false);
        }
    });


    switch(type)
    {
    case MREG_MULTI_SZ:
        ui->stackedWidget->setCurrentWidget(ui->page_1);
        ui->SZTextEdit->setText(data);
        break;
    case MREG_SZ:
    case MREG_EXPAND_SZ:
        ui->stackedWidget->setCurrentWidget(ui->page_2);
        ui->SZLineEdit->setText(data);
        break;
    case MREG_DWORD:
    case MREG_QWORD:
        ui->stackedWidget->setCurrentWidget(ui->page_3);
        ui->valueLineEdit->setText(data);
        break;
    case MREG_BINARY:
    case MREG_UNKNOWN_TYPE:
        ui->stackedWidget->setCurrentWidget(ui->page_4);
        ui->binaryTextEdit->setText(data);
        break;


    }
}

RegistryDataDialog::~RegistryDataDialog()
{
    delete ui;
}


void RegistryDataDialog::on_cancelButton_clicked()
{
    this->close();
}


void RegistryDataDialog::on_confirmButton_clicked()
{
    QString name = ui->nameEdit->text();
    QByteArray rawData;

    auto currentPage = ui->stackedWidget->currentWidget();
    qDebug()<<currentPage;

    if (currentPage == ui->page_1) { // MULTI_SZ
        QStringList lines = ui->SZTextEdit->toPlainText().split("\n");
        QString joined = lines.join(QChar(L'\0'));
        const ushort *utf16 = joined.utf16();
        rawData = QByteArray(reinterpret_cast<const char*>(utf16),
                             (joined.size() + 2) * sizeof(ushort));
    }
    else if (currentPage == ui->page_2) { // SZ, Expand_SZ
        QString text = ui->SZLineEdit->text();
        const ushort *utf16 = text.utf16();
        rawData = QByteArray(reinterpret_cast<const char*>(utf16),
                             (text.size() + 1) * sizeof(ushort));
    }
    else if (currentPage == ui->page_3) { // DWORD, QWORD
        QString text = ui->valueLineEdit->text();
        quint64 num = text.toULongLong();
        if (m_type == MREG_DWORD) {
            rawData.resize(4);
            memcpy(rawData.data(), &num, 4);
        } else {
            rawData.resize(8);
            memcpy(rawData.data(), &num, 8);
        }
    }
    else if (currentPage == ui->page_4) { // Binary
        QString HexStr = ui->binaryTextEdit->toPlainText();
        HexStr.remove(' ');
        rawData = QByteArray::fromHex(HexStr.toUtf8());
    }
    emit DataConfirmed(name,m_type,rawData);

    this->accept();
}

