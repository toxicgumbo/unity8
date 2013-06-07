/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Renato Araujo Oliveira Filho <renato@canonical.com>
 */

extern "C" {
#include <gio/gio.h>
}

#include "flatmenuproxymodel.h"
#include <QString>
#include <QStringList>

FlatMenuProxyModel::FlatMenuProxyModel(QAbstractItemModel *source)
    : QAbstractProxyModel(source),
      m_rowCount(0)
{
    m_model = new QDBusMenuModel;
    setSourceModel(m_model);
}

/*! \internal */
FlatMenuProxyModel::~FlatMenuProxyModel()
{
    delete m_model;
}

void FlatMenuProxyModel::setSourceModel(QAbstractItemModel *source)
{
    if (sourceModel()) {
        QAbstractItemModel *oldSource = sourceModel();
        if (oldSource == source) {
            return;
        }

        oldSource->disconnect(this);
    }

    QAbstractProxyModel::setSourceModel(source);

    if (source) {
        connect(source,
                SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
                SLOT(onRowsAboutToBeInserted(QModelIndex,int,int)));
        connect(source,
                SIGNAL(rowsInserted(QModelIndex,int,int)),
                SLOT(onRowsInserted(QModelIndex,int,int)));

        connect(source,
                SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                SLOT(onRowsAboutToBeRemoved(QModelIndex,int,int)));
        connect(source,
                SIGNAL(rowsRemoved(QModelIndex,int,int)),
                SLOT(onRowsRemoved(QModelIndex,int,int)));

        connect(source,
                SIGNAL(modelAboutToBeReset()),
                SLOT(onModelAboutToBeReset()));
        connect(source,
                SIGNAL(modelReset()),
                SLOT(onModelReset()));

        connect(source,
                SIGNAL(rowsInserted(QModelIndex,int,int)),
                SIGNAL(countChanged()));
        connect(source,
                SIGNAL(rowsRemoved(QModelIndex,int,int)),
                SIGNAL(countChanged()));
        connect(source,
                SIGNAL(modelReset()),
                SIGNAL(countChanged()));

        // initiliaze rowCount
        QModelIndex lastItem = source->index(source->rowCount() - 1, 0);
        m_rowCount = recursiveRowCount(lastItem);
    } else {
        m_rowCount = 0;
    }

    Q_EMIT countChanged();
}

void FlatMenuProxyModel::setBusName(const QString &busName)
{
    m_model->setBusName(busName);
}

QString FlatMenuProxyModel::busName() const
{
    return m_model->busName();
}

void FlatMenuProxyModel::setObjectPath(const QString &busPath)
{
    m_model->setObjectPath(busPath);
}

QString FlatMenuProxyModel::objectPath() const
{
    return m_model->objectPath();
}

int FlatMenuProxyModel::status() const
{
    return m_model->status();
}

DBusEnums::BusType FlatMenuProxyModel::busType() const
{
    return m_model->busType();
}

void FlatMenuProxyModel::setIntBusType(int type)
{
    m_model->setBusType((DBusEnums::BusType) type);
}

void FlatMenuProxyModel::start()
{
    m_model->start();
}

void FlatMenuProxyModel::stop()
{
    m_model->stop();
}

QModelIndex FlatMenuProxyModel::mapFromSource(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    QModelIndex result = createIndex(rowOffsetOf(index, index.row()), 0);
    return result;
}

QModelIndex FlatMenuProxyModel::mapToSource(const QModelIndex &index) const
{
    if (sourceModel() && m_indexCache.contains(index.row())) {
        QString key = m_indexCache.value(index.row());
        QStringList sections = key.split('.', QString::SkipEmptyParts);
        QModelIndex sourceIndex = QModelIndex();
        for(int i=0; i < sections.size(); i++) {
            sourceIndex = sourceModel()->index(sections.at(i).toInt(), 9, sourceIndex);
        }
        return sourceIndex;
    } else {
        return QModelIndex();
    }
}

QModelIndex FlatMenuProxyModel::index(int row, int sectionRow, const QModelIndex &parent, const QString &key) const
{
    for (int i = 0, count = 0; i < sourceModel()->rowCount(parent); i++) {
        QModelIndex sourceIndex = sourceModel()->index(i, 0, parent);
        int indexCount = recursiveRowCount(sourceIndex);

        if (count == sectionRow) {
            QString sectionKey = key + "." + QString::number(i);
            m_indexCache.insert(row, sectionKey);
            return createIndex(row, 0);
        } else if ((count + indexCount) >= sectionRow) {
            count++;
            return index(row, sectionRow - count, sourceIndex, key + "." + QString::number(i));
        }

        count += indexCount + 1;
    }
    return QModelIndex();
}

QModelIndex FlatMenuProxyModel::index(int row, int, const QModelIndex &) const
{
    return index(row, row, QModelIndex(), "");
}

QModelIndex FlatMenuProxyModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int FlatMenuProxyModel::columnCount(const QModelIndex &) const
{
    return 1;
}

int FlatMenuProxyModel::rowCount(const QModelIndex &) const
{
    return m_rowCount;
}

int FlatMenuProxyModel::count() const
{
    return rowCount(QModelIndex());
}

void FlatMenuProxyModel::onRowsAboutToBeInserted(const QModelIndex &index, int first, int last)
{

    if (index.isValid()) {
        int firstIndex = rowOffsetOf(index, first);
        int lastIndex = rowOffsetOf(index, last, true);

        beginInsertRows(QModelIndex(), firstIndex, lastIndex);
        m_rowCount += (lastIndex - firstIndex + 1);
    }
}

void FlatMenuProxyModel::onRowsAboutToBeRemoved(const QModelIndex &index, int first, int last)
{
    if (index.isValid()) {
        int firstIndex = rowOffsetOf(index, first);
        int lastIndex = rowOffsetOf(index, last, true);

        m_indexCache.clear();
        beginRemoveRows(QModelIndex(), firstIndex, lastIndex);
        m_rowCount -= (lastIndex - firstIndex + 1);
    }
}

void FlatMenuProxyModel::onModelAboutToBeReset()
{
   beginResetModel();
}

void FlatMenuProxyModel::onRowsInserted(const QModelIndex &index, int first, int last)
{
    if (index.isValid()) {
        m_indexCache.clear();
        endInsertRows();
        for (int i=first; i <= last; i++) {
            QModelIndex subIndex = sourceModel()->index(i, 0, index.parent());
            if (subIndex.isValid()) {
                int subSectionSize = recursiveRowCount(subIndex);
                if (subSectionSize > 0) {
                    int firstIndex = rowOffsetOf(index, i) + 1;
                    int lastIndex = rowOffsetOf(index, i, true);

                    beginInsertRows(QModelIndex(), firstIndex, lastIndex);
                    m_rowCount += lastIndex - firstIndex + 1;
                    endInsertRows();
                }
            }
        }
    }
    Q_EMIT countChanged();
}

void FlatMenuProxyModel::onRowsRemoved(const QModelIndex &index, int, int)
{
    if (index.isValid()) {
        endRemoveRows();
        Q_EMIT countChanged();
    }
}

void FlatMenuProxyModel::onModelReset()
{
    // initiliaze rowCount
    m_rowCount = 0;
    for (int i = 0, iMax = sourceModel()->rowCount(); i < iMax; i++) {
        QModelIndex index = sourceModel()->index(i, 0);
        m_rowCount += recursiveRowCount(index) + 1;
    }
    m_indexCache.clear();
    endResetModel();
    Q_EMIT countChanged();
}

int FlatMenuProxyModel::recursiveRowCount(const QModelIndex &index) const
{
    int size = sourceModel()->rowCount(index);
    for (int i=0, iMax=size; i < iMax; i++) {
        QModelIndex currentIndex = sourceModel()->index(i, 0, index);
        size += recursiveRowCount(currentIndex);
    }

    return size;
}

int FlatMenuProxyModel::rowOffsetOf(const QModelIndex &index, int row, bool inclusive) const
{
    int offset = 0;

    QModelIndex parent = index.parent();

    // Check row 0 offset
    while(parent.isValid()) {
        offset += 1;
        for (int i = 0; i < parent.row(); i++) {
            QModelIndex currentIndex = sourceModel()->index(i, 0, parent);
            offset += recursiveRowCount(currentIndex) + 1;
        }
        parent = parent.parent();
    }

    // Check index.row() offset
    for(int i = 0; i < row; i++) {
        QModelIndex currentIndex = sourceModel()->index(i, 0, index.parent());
        offset += recursiveRowCount(currentIndex) + 1;
    }

    if (inclusive) {
        // itself
        QModelIndex currentIndex = sourceModel()->index(row, 0, index.parent());
        offset += recursiveRowCount(currentIndex);
    }

    return offset;
}

/*! \for use in qml */
QVariantMap FlatMenuProxyModel::get(int row) const
{
    QVariantMap result;

    QModelIndex index = this->index(row);
    if (index.isValid()) {
        const QHash<int, QByteArray> roleNames = this->roleNames();
        Q_FOREACH(int i, roleNames.keys()) {
            result.insert(roleNames[i], data(index, i));
        }
    }
    return result;
}
