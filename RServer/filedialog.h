#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QSplitter>
#include <QFileIconProvider>
#include <QInputDialog>
#include "common.h"
#include "tcpworker.h"

// 前向声明需要放在正确的命名空间中
namespace Ui {
class FileDialog;  // Ui 命名空间中的类
}

// 注意：这个 FileDialog 是全局命名空间中的类
class FileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileDialog(TcpWorker *worker, CONTEXT_OBJECT *ctx, QWidget *parent = nullptr);
    ~FileDialog();

    void HandlePacket(unsigned char isToken, const QByteArray &payload);

private slots:
    void on_refreshButton_clicked();
    void on_deleteButton_clicked();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_pathLineEdit_returnPressed();
    void on_backButton_clicked();
    void on_newFolderButton_clicked();

private:
    void ParseFileList(const QByteArray &data);
    void UpdateCurrentPath(const QString &path);
    void SendFileRequest(const QString &path, unsigned char requestType);
    void AddFileItem(const QString &name, const QString &size, const QString &type,
                     const QString &modified, bool isDir);
    void ClearFileList();
    QString FormatSize(qint64 bytes);

    // 使用正确的命名空间
    Ui::FileDialog *ui;  // 这是 Ui::FileDialog 类型

    TcpWorker *m_worker;
    CONTEXT_OBJECT *m_context;
    QString m_currentPath;
    QHash<QString, QTreeWidgetItem*> m_pathItems;
    QFileIconProvider m_iconProvider;
};

#endif // FILEDIALOG_H
