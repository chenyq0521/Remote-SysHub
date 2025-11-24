#ifndef REGISTRYDATADIALOG_H
#define REGISTRYDATADIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include "registrydialog.h"
#include "common.h"

namespace Ui {
class RegistryDataDialog;
}

class RegistryDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistryDataDialog(QString name,MREG_VALUE type,QString data,QWidget *parent);
    ~RegistryDataDialog();

private slots:
    void on_cancelButton_clicked();
    void on_confirmButton_clicked();

signals:
    void DataConfirmed(const QString &name,MREG_VALUE type,const QByteArray &data);

private:
    Ui::RegistryDataDialog *ui;
    QString m_name;
    MREG_VALUE m_type;

};

#endif // REGISTRYDATADIALOG_H
