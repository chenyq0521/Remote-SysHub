#include "processmemorydialog.h"
#include "ui_processmemorydialog.h"


ProcessMemoryDialog::ProcessMemoryDialog(QWidget *parent, const QByteArray &data)
    : QDialog(parent), ui(new Ui::ProcessMemoryDialog)

{

    ui->setupUi(this);
    setWindowTitle("进程内存信息");
    m_model = new QStandardItemModel(this);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(ui->MemoryTableView);
    m_model->setHorizontalHeaderLabels(
        {"地址", "大小", "状态", "类型", "初始保护", "访问保护"}
        );

    ui->MemoryTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->MemoryTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->MemoryTableView->setFocusPolicy(Qt::NoFocus);
    ui->MemoryTableView->setModel(m_model);

    ParseMemoryInfo(data);
}

ProcessMemoryDialog::~ProcessMemoryDialog()
{
    delete ui;
}

void ProcessMemoryDialog::ParseMemoryInfo(const QByteArray &data)
{
    QDataStream in(data);
    while (!in.atEnd())
    {
        quint64 base, size;
        quint32 state, protect, allocProt, type;

        in >> base >> size >> state >> protect >> allocProt >> type;

        QList<QStandardItem*> row;

        row << new QStandardItem(QString("0x%1").arg(base, 0, 16).toUpper())
            << new QStandardItem(QString("0x%1").arg(size, 0, 16).toUpper())
            << new QStandardItem(MapState(state))
            << new QStandardItem(MapType(type))
            << new QStandardItem(ProtectToString(allocProt))
            << new QStandardItem(ProtectToString(protect));

        m_model->appendRow(row);
    }
}

QString ProcessMemoryDialog::MapState(DWORD s)
{
    switch(s)
    {
    case MEM_COMMIT: return "提交";
    case MEM_RESERVE: return "保留";
    case MEM_FREE: return "空闲";
    default: return "Unknown";
    }
}

QString ProcessMemoryDialog::MapType(DWORD t)
{
    switch(t)
    {
    case MEM_IMAGE: return "Image";
    case MEM_MAPPED: return "Mapping";
    case MEM_PRIVATE: return "私有";
    default: return "Unknown";
    }
}
QString ProcessMemoryDialog::ProtectToString(DWORD p)
{
    switch(p)
    {
    case PAGE_READONLY: return "-R--";
    case PAGE_READWRITE: return "-RW-";
    case PAGE_WRITECOPY: return "-RWC";
    case PAGE_EXECUTE: return "E---";
    case PAGE_EXECUTE_READ: return "ER--";
    case PAGE_EXECUTE_READWRITE: return "ERW-";
    case PAGE_EXECUTE_WRITECOPY: return "ERWC";
    case PAGE_NOACCESS: return "----";
    default: return "----";
    }
}
