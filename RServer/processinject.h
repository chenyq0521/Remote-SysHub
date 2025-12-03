#ifndef PROCESSINJECT_H
#define PROCESSINJECT_H

#include <QDialog>
#include "common.h"

namespace Ui {
class processinject;
}

class processinject : public QDialog
{
    Q_OBJECT

public:
    explicit processinject(QWidget *parent = nullptr, quint32 pid = 0);
    ~processinject();

private slots:
    void on_InstallBtn_clicked();

    void on_UninstallBtn_clicked();
signals:
    void InstallHook(quint8 choice);
    void UninstallHook(quint8 choice);
private:
    Ui::processinject *ui;
    quint32 m_pid;
};

#endif // PROCESSINJECT_H
