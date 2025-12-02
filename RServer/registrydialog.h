#ifndef REGISTRYDIALOG_H
#define REGISTRYDIALOG_H

#include <QMainWindow>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include "common.h"
#include "serverdlg.h"
#include "tcpworker.h"

namespace Ui {
class RegistryDialog;
}

typedef struct _PACKET_HEADER_
{
    int count;
    DWORD nameSize;
    DWORD dataSize;
}PACKET_HEADER;

enum MREG_KEY
{
    MHKEY_CLASSES_ROOT,
    MHKEY_CURRENT_USER,
    MHKEY_LOCAL_MACHINE,
    MHKEY_USERS,
    MHKEY_CURRENT_CONFIG
};

enum MREG_VALUE
{
    MREG_SZ,
    MREG_DWORD,
    MREG_QWORD,
    MREG_BINARY,
    MREG_MULTI_SZ,
    MREG_EXPAND_SZ,
    MREG_UNKNOWN_TYPE
};



class RegistryDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegistryDialog(TcpWorker* worker = nullptr,CONTEXT_OBJECT *ctx = nullptr, QWidget *parent = nullptr);
    ~RegistryDialog();
    void HandlePacket(unsigned char istoken,QByteArray data);
private slots:


    void on_valueTableView_customContextMenuRequested(const QPoint &pos);

    void on_pathTreeView_customContextMenuRequested(const QPoint &pos);

    void on_valueTableView_doubleClicked(const QModelIndex &index);

    void RenameValue(const QModelIndex &index);
    void DeleteValue(const QModelIndex &index);
    void CreateNewValue(MREG_VALUE type);
    void CreateNewKey(const QModelIndex &index);
    void HandleNewKey(quint8 rootId, const QString &subPath, const QString &newKey);
    void RenameKey(const QModelIndex &index);
    void DeleteKey(const QModelIndex &index);
    void HandleRenameKey(quint8 rootId, const QString &subPath,const QString &oldName, const QString &newName);
    void HandleDeleteKey(quint8 rootId, const QString &subPath,const QString KeyName);


private:
    void showSubKeys(const QByteArray &data);
    void showValues(const QByteArray &data);
    void SendFindRequest(quint8 rootKey,const QString&subPath);
    void SendRefreshValueRequest(quint8 rootKey, const QString &subPath);
    void SetPathTreeView();
    void SetValueTableView();

    CONTEXT_OBJECT *m_ctxobj;
    TcpWorker *m_worker;

    QStandardItemModel *m_keyModel;
    QStandardItemModel *m_valueModel;
    QSet<QString> m_existingNames;
    Ui::RegistryDialog *ui;
};

#endif // REGISTRYDIALOG_H
