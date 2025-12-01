#ifndef PROCESSMEMORYDIALOG_H
#define PROCESSMEMORYDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QByteArray>
#include <QVBoxLayout>

#include "common.h"
namespace Ui {
class ProcessMemoryDialog;
}

class ProcessMemoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessMemoryDialog(QWidget *parent, const QByteArray &data);
    ~ProcessMemoryDialog();

private:
    QStandardItemModel *m_model;
    void ParseMemoryInfo(const QByteArray &data);
    Ui::ProcessMemoryDialog *ui;

    QString MapState(DWORD s);
    QString MapType(DWORD t);
    QString ProtectToString(DWORD p);
};

#endif // PROCESSMEMORYDIALOG_H
