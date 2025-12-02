#include "registrydialog.h"
#include "ui_registrydialog.h"
#include "registrydatadialog.h"

RegistryDialog::RegistryDialog(TcpWorker *worker, CONTEXT_OBJECT *ctx, QWidget *parent)
    : QMainWindow(parent), m_ctxobj(ctx), m_worker(worker), ui(new Ui::RegistryDialog)
{
    ui->setupUi(this);
    m_keyModel = new QStandardItemModel(this);
    m_valueModel = new QStandardItemModel(this);
    SetPathTreeView();
    SetValueTableView();
}

RegistryDialog::~RegistryDialog()
{
    delete ui;
}

void RegistryDialog::HandlePacket(unsigned char istoken, QByteArray data)
{
    qDebug()<<"registry dialog is handeling…";
    QDataStream in(data);

    switch(istoken)
    {
    case REGISTRY_SEARCH_PATH_REPLY:
        showSubKeys(data);
        break;
    case REGISTRY_SEARCH_KEY_REPLY:
        showValues(data);
        break;
    case REGISTRY_KEY_NEW:
    {
        quint8 rootId; QString subPath, newKey;
        in >> rootId >> subPath >> newKey;
        HandleNewKey(rootId, subPath, newKey);
        break;
    }
    case REGISTRY_KEY_RENAME:
    {
        quint8 rootId;
        QString subPath, oldName, newName;
        in >> rootId >> subPath >> oldName >> newName;

        HandleRenameKey(rootId, subPath, oldName, newName);
        break;
    }
    case REGISTRY_KEY_DELETE:
    {
        quint8 rootId;
        QString parentPath, keyName;
        in >> rootId >> parentPath >> keyName;
        HandleDeleteKey(rootId, parentPath, keyName);
        break;
    }
    default:
        qDebug()<<"unknown token"<<istoken;
        break;
    }
}

void RegistryDialog::showSubKeys(const QByteArray &data)
{
    QDataStream in(data);
    int count = 0;
    quint32 nameSize, dataSize;
    in >> count >> nameSize >> dataSize;

    QModelIndex curIndex = ui->pathTreeView->currentIndex();
    if (!curIndex.isValid()) return;


    QStandardItem *parentItem = m_keyModel->itemFromIndex(curIndex);
    if (!parentItem) return;

    parentItem->removeRows(0, parentItem->rowCount());

    int rootId = parentItem->data(Qt::UserRole+1).toInt();
    QString parentPath = parentItem->data(Qt::UserRole+2).toString();

    for (int i = 0; i < count; i++) {
        QString subKey;
        in >> subKey;

        if (subKey.isEmpty()) continue;

        // 生成完整路径
        QString fullPath = parentPath.isEmpty() ? subKey : parentPath + "\\" + subKey;

        QStandardItem *item = new QStandardItem(QIcon(":/asserts/asserts/folder.ico"), subKey);
        item->setData(rootId, Qt::UserRole+1);       // 根键 ID
        item->setData(fullPath, Qt::UserRole+2);     // 完整子路径

        parentItem->appendRow(item);
    }

    ui->pathTreeView->expand(curIndex);

}

void RegistryDialog::showValues(const QByteArray &data)
{
    m_valueModel->removeRows(0, m_valueModel->rowCount());

    QDataStream in(data);
    int count = 0;
    quint32 nameSize, dataSize;
    in >> count >> nameSize >> dataSize;

    QList<QStandardItem*> defaultRow;
    defaultRow << new QStandardItem(QIcon(":/asserts/asserts/String.ico"), "(默认)")
               << new QStandardItem("REG_SZ")
               << new QStandardItem("(值未设置)");
    m_valueModel->appendRow(defaultRow);
    m_existingNames.clear();
    for (int i = 0; i < count; i++) {
        quint32 type;
        QString name;
        QByteArray valueData;

        in >> type >> name >> valueData;

        QString typeStr;
        switch (type) {
        case MREG_SZ:        typeStr = "REG_SZ"; break;
        case MREG_DWORD:     typeStr = "REG_DWORD"; break;
        case MREG_QWORD:     typeStr = "REG_QWORD"; break;
        case MREG_BINARY:    typeStr = "REG_BINARY"; break;
        case MREG_MULTI_SZ:  typeStr = "REG_MULTI_SZ"; break;
        case MREG_EXPAND_SZ: typeStr = "REG_EXPAND_SZ"; break;
        default:             typeStr = "UNKNOWN"; break;
        }

        QIcon icon;
        if (type == MREG_SZ || type == MREG_EXPAND_SZ || type == MREG_MULTI_SZ)
            icon = QIcon(":/asserts/asserts/String.ico");
        else if (type == MREG_DWORD || type == MREG_QWORD || type == MREG_BINARY)
            icon = QIcon(":/asserts/asserts/Dword.ico");


        QString valueStr;
        if (type == MREG_SZ || type == MREG_EXPAND_SZ || type == MREG_MULTI_SZ) {
            const wchar_t *wstr = reinterpret_cast<const wchar_t*>(valueData.constData());
            int wcharLen = valueData.size() / sizeof(wchar_t);
            valueStr = QString::fromWCharArray(wstr, wcharLen).trimmed();
        } else if (type == MREG_DWORD && valueData.size() >= 4) {
            quint32 num;
            memcpy(&num, valueData.data(), sizeof(num));
            valueStr = QString::number(num);
        } else if (type == MREG_QWORD && valueData.size() >= 8) {
            quint64 num;
            memcpy(&num, valueData.data(), sizeof(num));
            valueStr = QString::number(num);
        } else {
            valueStr = valueData.toHex(' ').toUpper();
        }

        if (name.isEmpty())
        {
            m_valueModel->item(0, 1)->setText(typeStr);
            m_valueModel->item(0, 1)->setData(type,Qt::UserRole);
            m_valueModel->item(0, 2)->setText(valueData.isEmpty() ? "(值未设置)" : valueStr.trimmed());
            m_valueModel->item(0, 0)->setIcon(icon);
        } else
        {
            auto nameItem = new QStandardItem(icon, name);
            auto typeItem = new QStandardItem(typeStr);
            auto dataItem = new QStandardItem(valueStr);

            typeItem->setData(type, Qt::UserRole);
            m_valueModel->appendRow({nameItem, typeItem, dataItem});
        }
        m_existingNames.insert(name);
    }


}


void RegistryDialog::SendFindRequest(quint8 rootKey, const QString &subPath)
{
    m_ctxobj->payload.clear();
    QDataStream stream(&m_ctxobj->payload,QIODevice::WriteOnly);

    stream << rootKey << subPath;
    m_ctxobj->isToken = REGISTRY_SEARCH_REQUIRE;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}

void RegistryDialog::SendRefreshValueRequest(quint8 rootKey, const QString &subPath)
{
    m_ctxobj->payload.clear();
    QDataStream stream(&m_ctxobj->payload,QIODevice::WriteOnly);

    stream << rootKey << subPath;
    m_ctxobj->isToken = REGISTRY_REFRESH_VALUE_REQUIRE;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}

void RegistryDialog::HandleNewKey(quint8 rootId, const QString &subPath, const QString &newKey)
{
    QList<QStandardItem*> items = m_keyModel->findItems(QFileInfo(subPath).fileName(),
                                                         Qt::MatchExactly | Qt::MatchRecursive);
    if (items.isEmpty()) return;
    QStandardItem *parent = items.first();

    QString fullPath = subPath.isEmpty() ? newKey : subPath + "\\" + newKey;
    QStandardItem *item = new QStandardItem(QIcon(":/asserts/asserts/folder.ico"), newKey);
    item->setData(rootId, Qt::UserRole + 1);
    item->setData(fullPath, Qt::UserRole + 2);
    parent->appendRow(item);
}

void RegistryDialog::SetPathTreeView()
{
    QTreeView *pathTree = ui->pathTreeView;
    pathTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pathTree->setSelectionMode(QAbstractItemView::SingleSelection);
    pathTree->setContextMenuPolicy(Qt::CustomContextMenu);
    pathTree->setExpandsOnDoubleClick(false);
    pathTree->setContextMenuPolicy(Qt::CustomContextMenu);

    m_keyModel->clear();
    pathTree->setModel(m_keyModel);

    QStandardItem *rootItem = new QStandardItem(QIcon(":/asserts/asserts/root.ico"), "计算机");
    rootItem->setData(-1, Qt::UserRole + 1);
    rootItem->setData(QString(""), Qt::UserRole + 2); // 空路径
    rootItem->setData(true, Qt::UserRole + 3); // 根节点不需要加载
    m_keyModel->appendRow(rootItem);


    QStringList roots = {
        "HKEY_CLASSES_ROOT",
        "HKEY_CURRENT_USER",
        "HKEY_LOCAL_MACHINE",
        "HKEY_USERS",
        "HKEY_CURRENT_CONFIG"
    };
    for (int i = 0; i < roots.size(); ++i) {
        QStandardItem *item = new QStandardItem(QIcon(":/asserts/asserts/folder.ico"), roots[i]);
        item->setData(i, Qt::UserRole + 1);
        item->setData(QString(""), Qt::UserRole + 2);

        rootItem->appendRow(item);
    }

    pathTree->expand(rootItem->index());

    connect(pathTree, &QTreeView::doubleClicked, this, [=](const QModelIndex &index){
        QStandardItem *item = m_keyModel->itemFromIndex(index);
        if (!item) return;

        int rootId = item->data(Qt::UserRole+1).toInt();
        QString subPath = item->data(Qt::UserRole+2).toString();

        if (rootId < 0) return; // 跳过“计算机”

        qDebug()<<"index:"<<index;
        if (pathTree->isExpanded(index))
        {
            qDebug()<<1;
            SendRefreshValueRequest(rootId,subPath);
            pathTree->collapse(index);
            return;
        }
        if (item->rowCount() > 0)
        {
            qDebug()<<2;
            SendRefreshValueRequest(rootId,subPath);
            pathTree->expand(index);
        } else
        {
            qDebug()<<3;
            SendFindRequest(rootId, subPath);
        }
    });

}

void RegistryDialog::SetValueTableView()
{
    QTableView *valueView = ui->valueTableView;

    valueView->setShowGrid(false);
    valueView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    valueView->setSelectionBehavior(QAbstractItemView::SelectRows);
    valueView->setSelectionMode(QAbstractItemView::SingleSelection);
    valueView->verticalHeader()->setDefaultSectionSize(20);
    valueView->verticalHeader()->setVisible(false);


    m_valueModel->setColumnCount(3);
    m_valueModel->setHorizontalHeaderLabels({"名称","类型","数据"});

    valueView->setFocusPolicy(Qt::NoFocus);
    valueView->setModel(m_valueModel);

    valueView->setColumnWidth(0,500);
    valueView->setColumnWidth(1,100);
    valueView->setColumnWidth(2,600);

    valueView->setContextMenuPolicy(Qt::CustomContextMenu);
}



void RegistryDialog::on_valueTableView_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui->valueTableView->indexAt(pos);
    QMenu valueMenu;
    QAction *ModifyData = new QAction("修改数据");
    QAction *Delete = new QAction("删除");
    QAction *Rename = new QAction("重命名");
    QAction *New = new QAction("新建");
    QAction *StringValue = new QAction("字符串值");
    QAction *BinaryValue = new QAction("二进制值");
    QAction *DwordValue = new QAction("DWORD值");
    QAction *QwordValue = new QAction("QWORD值");
    QAction *MultiStringValue = new QAction("多字符串值");
    QAction *ExpandedStringValue = new QAction("可扩充字符串值");

    QMenu *SubNewMenu = new QMenu(this);

    SubNewMenu->addAction(StringValue);
    SubNewMenu->addAction(BinaryValue);
    SubNewMenu->addAction(DwordValue);
    SubNewMenu->addAction(QwordValue);
    SubNewMenu->addAction(MultiStringValue);
    SubNewMenu->addAction(ExpandedStringValue);

    New->setMenu(SubNewMenu);

    if(index.isValid())
    {
        index = m_valueModel->index(index.row(), 0);

        valueMenu.addAction(ModifyData);
        valueMenu.addSeparator();
        valueMenu.addAction(Delete);
        valueMenu.addAction(Rename);
        QString name = m_valueModel->item(index.row(), 0)->text();
        if (name == "(默认)") {
            Delete->setEnabled(false);
            Rename->setEnabled(false);
        }
    }
    else
    {
        valueMenu.addAction(New);
    }
    connect(ModifyData,&QAction::triggered,this,[=](){on_valueTableView_doubleClicked(index);});
    connect(Rename,&QAction::triggered,this,[=](){RenameValue(index);});
    connect(Delete,&QAction::triggered,this,[=](){DeleteValue(index);});
    connect(StringValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_SZ);});
    connect(BinaryValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_BINARY);});
    connect(DwordValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_DWORD);});
    connect(QwordValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_QWORD);});
    connect(MultiStringValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_MULTI_SZ);});
    connect(ExpandedStringValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_EXPAND_SZ);});
    valueMenu.exec(ui->valueTableView->viewport()->mapToGlobal(pos));
}

void RegistryDialog::on_valueTableView_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;

    int row = index.row();
    QString name = m_valueModel->item(row, 0)->text();
    auto type = m_valueModel->item(row, 1)->data(Qt::UserRole).toUInt();
    QString data = m_valueModel->item(row, 2)->text();

    QModelIndex treeIndex = ui->pathTreeView->currentIndex();
    if (!treeIndex.isValid()) return;

    QStandardItem *pathItem = m_keyModel->itemFromIndex(treeIndex);
    if (!pathItem) return;

    int rootId = pathItem->data(Qt::UserRole+1).toInt();
    QString subPath = pathItem->data(Qt::UserRole+2).toString();

    RegistryDataDialog dlg(name, (MREG_VALUE)type, data, this);


    connect(&dlg, &RegistryDataDialog::DataConfirmed, this, [=](const QString &mdfname, MREG_VALUE mdftype, const QByteArray mdfData){
        qDebug()<<"[Registry] will modify data of"<<mdfname<<name<<"from"<<data<<"to"<<mdfData.toStdString();

        QString realName = (mdfname == "(默认)") ? QString("") : mdfname;
        m_ctxobj->payload.clear();
        QDataStream stream(&m_ctxobj->payload, QIODevice::WriteOnly);

        stream << (quint8)rootId << subPath << realName << (quint32)mdftype << mdfData;

        m_ctxobj->isToken = REGISTRY_VALUE_MODIFY;
        QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

    });

    dlg.exec();
}

void RegistryDialog::RenameValue(const QModelIndex &index)
{
    if (!index.isValid()) return;

    int row = index.row();
    QStandardItem *nameItem = m_valueModel->item(row, 0);
    if (!nameItem) return;
    QString oldName = nameItem->text();

    ui->valueTableView->edit(index);

    connect(m_valueModel,&QStandardItemModel::itemChanged,this,[=](QStandardItem *changedItem)
    {
        if(changedItem != nameItem) return;
        QString newName = changedItem->text().trimmed();
        if(newName.isEmpty())
        {
            QMessageBox::warning(this,"重命名时产生错误","指定的值名为空，请重试一次。");
            nameItem->setText(oldName);
            return;
        }
        for(auto name:m_existingNames)
        {
            if(name == newName)
            {
                QMessageBox::warning(this,"重命名时产生错误","指定的值名已经存在，请重试一次。");
                nameItem->setText(oldName);
                return;
            }
        }
        if(newName == oldName) return;

        QModelIndex treeIndex = ui->pathTreeView->currentIndex();
        if (!treeIndex.isValid()) return;

        QStandardItem *pathItem = m_keyModel->itemFromIndex(treeIndex);
        if (!pathItem) return;

        int rootId = pathItem->data(Qt::UserRole+1).toInt();
        QString subPath = pathItem->data(Qt::UserRole+2).toString();

        m_ctxobj->payload.clear();
        QDataStream stream(&m_ctxobj->payload,QIODevice::WriteOnly);
        stream << (quint8)rootId << subPath << oldName << newName;
        m_ctxobj->isToken = REGISTRY_VALUE_RENAME;
        QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

        disconnect(m_valueModel, &QStandardItemModel::itemChanged, this, nullptr);
    });
}

void RegistryDialog::DeleteValue(const QModelIndex &index)
{
    if (!index.isValid()) return;

    int row = index.row();
    QString name = m_valueModel->item(row, 0)->text();

    QString data = m_valueModel->item(row, 2)->text();
    QModelIndex treeIndex = ui->pathTreeView->currentIndex();
    if (!treeIndex.isValid()) return;

    QStandardItem *pathItem = m_keyModel->itemFromIndex(treeIndex);
    if (!pathItem) return;

    int rootId = pathItem->data(Qt::UserRole+1).toInt();
    QString subPath = pathItem->data(Qt::UserRole+2).toString();

    auto reply = QMessageBox::warning(this,"确认数值删除",
                         "删除某些注册表值会引起系统不稳定，确实要永久删除此值吗？",
                         QMessageBox::Yes|QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
        m_ctxobj->payload.clear();
        QDataStream stream(&m_ctxobj->payload, QIODevice::WriteOnly);
        stream << (quint8)rootId << subPath << name;


        m_ctxobj->isToken = REGISTRY_VALUE_DELETE;
        QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

    }
}

void RegistryDialog::CreateNewValue(MREG_VALUE type)
{
    QModelIndex treeIndex = ui->pathTreeView->currentIndex();
    if (!treeIndex.isValid()) return;

    QStandardItem *pathItem = m_keyModel->itemFromIndex(treeIndex);
    if (!pathItem) return;

    int rootId = pathItem->data(Qt::UserRole + 1).toInt();
    QString subPath = pathItem->data(Qt::UserRole + 2).toString();

    QString newName;
    int counter = 1;
    do  newName = QString("新值 #%1").arg(counter++);
    while (m_existingNames.contains(newName));

    QByteArray Data;
    switch (type) {
    case MREG_DWORD: {
        quint32 zero = 0;
        Data.resize(4);
        memcpy(Data.data(), &zero, 4);
        break;
    }
    case MREG_QWORD: {
        quint64 zero = 0;
        Data.resize(8);
        memcpy(Data.data(), &zero, 8);
        break;
    }
    case MREG_BINARY:
        Data.append('\0');
        break;
    default: //字符串类
        Data.clear();
        break;
    }

    m_ctxobj->payload.clear();
    QDataStream stream(&m_ctxobj->payload, QIODevice::WriteOnly);
    stream << (quint8)rootId << subPath << newName << (quint32)type << Data;

    m_ctxobj->isToken = REGISTRY_VALUE_NEW;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}

void RegistryDialog::CreateNewKey(const QModelIndex &index)
{
    if(!index.isValid()) return;

    QStandardItem *parentItem = m_keyModel->itemFromIndex(index);
    if (!parentItem) return;

    int rootId = parentItem->data(Qt::UserRole + 1).toInt();
    QString parentPath = parentItem->data(Qt::UserRole + 2).toString();

    QSet<QString> existingKeys;
    for (int i = 0; i < parentItem->rowCount(); ++i)
        existingKeys.insert(parentItem->child(i)->text());

    QString newKeyName;
    int counter = 1;
    do  newKeyName = QString("新项 #%1").arg(counter++);
    while (existingKeys.contains(newKeyName));

    qDebug() << "[Registry] Will create subkey:" << newKeyName;

    m_ctxobj->payload.clear();
    QDataStream stream(&m_ctxobj->payload, QIODevice::WriteOnly);
    stream << (quint8)rootId << parentPath << newKeyName;

    m_ctxobj->isToken = REGISTRY_KEY_NEW;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

}

void RegistryDialog::RenameKey(const QModelIndex &index)
{
    if (!index.isValid()) return;
    QStandardItem *item = m_keyModel->itemFromIndex(index);
    if (!item) return;
    QString oldName = item->text();
    ui->pathTreeView->edit(index);

    connect(m_keyModel,&QStandardItemModel::itemChanged,this,[=](QStandardItem *changedItem)
    {
        if(changedItem != item) return;
        QString newName = changedItem->text().trimmed();
        if(newName.isEmpty())
        {
            QMessageBox::warning(this,"重命名时产生错误","指定的值名为空，请重试一次。");
            item->setText(oldName);
            return;
        }
        if(oldName == newName) return;
        QStandardItem *parent = changedItem->parent();
        if (!parent) return;

        item->setText(oldName);

        int rootId = parent->data(Qt::UserRole + 1).toInt();
        QString subPath = parent->data(Qt::UserRole + 2).toString();

        m_ctxobj->payload.clear();
        QDataStream stream(&m_ctxobj->payload,QIODevice::WriteOnly);
        stream << (quint8)rootId << subPath << oldName << newName;
        m_ctxobj->isToken = REGISTRY_KEY_RENAME;
        QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);

        disconnect(m_keyModel,&QStandardItemModel::itemChanged, this, nullptr);
    });

}

void RegistryDialog::DeleteKey(const QModelIndex &index)
{
    if(!index.isValid()) return;
    QString name = index.data().toString();         // 要删除的项名
    QStandardItem *item = m_keyModel->itemFromIndex(index);
    if (!item) return;

    QStandardItem *parentItem = item->parent();
    if (!parentItem) return;

    int rootId = parentItem->data(Qt::UserRole+1).toInt();
    QString parentPath = parentItem->data(Qt::UserRole+2).toString();

    auto reply = QMessageBox::warning(this,"确认删除项",
        QString("删除注册表项 '%1' 将删除所有子项和值。\n确定继续？").arg(name),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply != QMessageBox::Yes)
        return;

    m_ctxobj->payload.clear();
    QDataStream out(&m_ctxobj->payload, QIODevice::WriteOnly);
    out << (quint8)rootId << parentPath << name;

    m_ctxobj->isToken = REGISTRY_KEY_DELETE;
    QMetaObject::invokeMethod(m_worker, "SendData", Qt::QueuedConnection);
}

void RegistryDialog::HandleRenameKey(quint8 rootId, const QString &subPath, const QString &oldName, const QString &newName)
{
    qDebug()<<"[rename] subpath"<<subPath;
    QList<QStandardItem*> items =
        m_keyModel->findItems(oldName, Qt::MatchRecursive);

    for (QStandardItem *item : items)
    {
        QString path = item->parent()->data(Qt::UserRole+2).toString();
        if (path == subPath)
        {
            QString fullPath = subPath + "\\" + newName;

            item->setText(newName);
            item->setData(fullPath, Qt::UserRole+2);
            return;
        }
    }
}

void RegistryDialog::HandleDeleteKey(quint8 rootId, const QString &subPath, const QString KeyName)
{
    // 找到父节点
    QList<QStandardItem*> items =
        m_keyModel->findItems(KeyName, Qt::MatchRecursive);
    for (QStandardItem *item : items)
    {
        QString path = item->parent()->data(Qt::UserRole+2).toString();
        if (path == subPath)
        {
            QStandardItem *parent = item->parent();
            parent->removeRow(item->row());
            m_valueModel->removeRows(1,m_valueModel->rowCount() - 1);
            return;
        }
    }
}

void RegistryDialog::on_pathTreeView_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui->pathTreeView->indexAt(pos);
    if(!index.isValid()) return;
    QString nodeText = index.data().toString();


    QMenu pathMenu;
    QAction *Delete = new QAction("删除");
    QAction *Rename = new QAction("重命名");
    QAction *New = new QAction("新建");
    QAction *item = new QAction("项");
                                                                               QAction *StringValue = new QAction("字符串值");
    QAction *BinaryValue = new QAction("二进制值");
    QAction *DwordValue = new QAction("DWORD值");
    QAction *QwordValue = new QAction("QWORD值");
    QAction *MultiStringValue = new QAction("多字符串值");
    QAction *ExpandedStringValue = new QAction("可扩充字符串值");

    QMenu *SubNewMenu = new QMenu(this);

    SubNewMenu->addAction(item);
    SubNewMenu->addSeparator();
    SubNewMenu->addAction(StringValue);
    SubNewMenu->addAction(BinaryValue);
    SubNewMenu->addAction(DwordValue);
    SubNewMenu->addAction(QwordValue);
    SubNewMenu->addAction(MultiStringValue);
    SubNewMenu->addAction(ExpandedStringValue);

    New->setMenu(SubNewMenu);

    if (nodeText == "计算机") return;
    else if (nodeText == "HKEY_CLASSES_ROOT" ||
               nodeText == "HKEY_CURRENT_USER" ||
               nodeText == "HKEY_LOCAL_MACHINE" ||
               nodeText == "HKEY_USERS" ||
               nodeText == "HKEY_CURRENT_CONFIG")
    {
        pathMenu.addAction(New);
        pathMenu.addSeparator();
        Delete->setEnabled(false);
        Rename->setEnabled(false);
        pathMenu.addAction(Delete);
        pathMenu.addAction(Rename);
    } else
    {
        pathMenu.addAction(New);
        pathMenu.addSeparator();
        pathMenu.addAction(Delete);
        pathMenu.addAction(Rename);
    }

    connect(item,&QAction::triggered,this,[=](){CreateNewKey(ui->pathTreeView->currentIndex());});
    connect(Rename,&QAction::triggered,this,[=](){RenameKey(ui->pathTreeView->currentIndex());});
    connect(Delete,&QAction::triggered,this,[=](){DeleteKey(ui->pathTreeView->currentIndex());});
    connect(StringValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_SZ);});
    connect(BinaryValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_BINARY);});
    connect(DwordValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_DWORD);});
    connect(QwordValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_QWORD);});
    connect(MultiStringValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_MULTI_SZ);});
    connect(ExpandedStringValue, &QAction::triggered, this, [=](){CreateNewValue(MREG_EXPAND_SZ);});
    pathMenu.exec(ui->pathTreeView->viewport()->mapToGlobal(pos));
}


