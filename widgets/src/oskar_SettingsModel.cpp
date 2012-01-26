#include "widgets/oskar_SettingsModel.h"
#include "widgets/oskar_SettingsItem.h"
#include <QtCore/QVector>
#include <cstdio>

oskar_SettingsModel::oskar_SettingsModel(QObject* parent)
: QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    rootData.append("Setting");
    rootData.append("Value");
    rootItem_ = new oskar_SettingsItem(QString(), QString(), 0, rootData);
}

oskar_SettingsModel::~oskar_SettingsModel()
{
    delete rootItem_;
}

void oskar_SettingsModel::append(const QString& key,
        const QString& keyShort, int type, const QVector<QVariant>& data,
        const QModelIndex& parent)
{
    oskar_SettingsItem *parentItem = getItem(parent);

    beginInsertRows(parent, rowCount(), rowCount());
    parentItem->appendChild(new oskar_SettingsItem(key, keyShort, type, data,
            parentItem));
    endInsertRows();
}

int oskar_SettingsModel::columnCount(const QModelIndex& parent) const
{
    return getItem(parent)->columnCount();
}

QVariant oskar_SettingsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    return getItem(index)->data(index.column());
}

Qt::ItemFlags oskar_SettingsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    oskar_SettingsItem* item = getItem(index);
    if (index.column() == 0 ||
            item->type() == oskar_SettingsItem::OSKAR_SETTINGS_CAPTION_ONLY)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant oskar_SettingsModel::headerData(int section,
        Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem_->data(section);

    return QVariant();
}

QModelIndex oskar_SettingsModel::index(int row, int column,
        const QModelIndex& parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    oskar_SettingsItem* parentItem = getItem(parent);
    oskar_SettingsItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

/*
bool oskar_SettingsModel::insertColumns(int position, int columns,
        const QModelIndex& parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem_->insertColumns(position, columns);
    endInsertColumns();

    return success;
}
*/

QModelIndex oskar_SettingsModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    oskar_SettingsItem* childItem = getItem(index);
    oskar_SettingsItem* parentItem = childItem->parent();

    if (parentItem == rootItem_)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

void oskar_SettingsModel::registerSetting(const QString& key,
        const QString& caption, int type, const QStringList& options)
{
    QStringList keys = key.split('/');
    QVector<QVariant> data(columnCount() <= 2 ? 2 : columnCount());

    // Find the parent, creating groups as necessary.
    QModelIndex parent, child;
    for (int k = 0; k < keys.size() - 1; ++k)
    {
        QString keyShort = keys[k];
        child = getChild(keys[k], parent);

        if (child.isValid())
            parent = child;
        else
        {
            // Append the group and set it as the new parent.
            data[0] = keys[k];
            append(key, keys[k],
                    oskar_SettingsItem::OSKAR_SETTINGS_CAPTION_ONLY,
                    data, parent);
            parent = index(rowCount(parent) - 1, 0, parent);
        }
    }

    // Append the actual setting.
    data[0] = caption;
    append(key, keys.last(), type, data, parent);
}

int oskar_SettingsModel::rowCount(const QModelIndex& parent) const
{
    return getItem(parent)->childCount();
}

void oskar_SettingsModel::setCaption(const QString& key,
        const QString& caption)
{
    QStringList keys = key.split('/');
    QVector<QVariant> data(columnCount() <= 2 ? 2 : columnCount());

    // Find the parent, creating groups as necessary.
    QModelIndex parent, child;
    for (int k = 0; k < keys.size() - 1; ++k)
    {
        QString keyShort = keys[k];
        child = getChild(keys[k], parent);

        if (child.isValid())
            parent = child;
        else
        {
            // Append the group and set it as the new parent.
            data[0] = keys[k];
            append(key, keys[k],
                    oskar_SettingsItem::OSKAR_SETTINGS_CAPTION_ONLY,
                    data, parent);
            parent = index(rowCount(parent) - 1, 0, parent);
        }
    }

    // Set the new caption.
    child = getChild(keys.last(), parent);
    if (child.isValid())
        setData(index(child.row(), 0, parent), caption);
    else
    {
        data[0] = caption;
        append(key, keys.last(),
                oskar_SettingsItem::OSKAR_SETTINGS_CAPTION_ONLY, data, parent);
    }
}

bool oskar_SettingsModel::setData(const QModelIndex& index,
        const QVariant& value, int role)
{
    if (role != Qt::EditRole)
        return false;

    oskar_SettingsItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

/*
bool oskar_SettingsModel::setHeaderData(int section,
        Qt::Orientation orientation, const QVariant& value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem_->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}
*/

// Private methods.

oskar_SettingsItem* oskar_SettingsModel::getItem(const QModelIndex& index) const
{
    if (index.isValid())
    {
        oskar_SettingsItem* item =
                static_cast<oskar_SettingsItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem_;
}

QModelIndex oskar_SettingsModel::getChild(const QString& keyShort,
        const QModelIndex& parent) const
{
    // Search this parent's children.
    oskar_SettingsItem* item = getItem(parent);
    for (int i = 0; i < item->childCount(); ++i)
    {
        if (item->child(i)->keyShort() == keyShort)
            return index(i, 0, parent);
    }
    return QModelIndex();
}
