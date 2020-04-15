#include "sparamtable.h"

SParamTable::SParamTable(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_rowcount = 0;
}

int SParamTable::rowCount(const QModelIndex &parent) const
{
    return m_rowcount;
}

int SParamTable::columnCount(const QModelIndex &parent) const
{
    return column_count;
}

QVariant SParamTable::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_data[index.row()][index.column()];
    }
    return QVariant();
}

void SParamTable::addVNAResult(Protocol::Datapoint d)
{
    Entry p;
    p.frequency = d.frequency;
    p.S11 = SParam(10.0 * log10(d.S11Mag), d.S11Phase * 180.0 / M_PI);
    p.S12 = SParam(10.0 * log10(d.S12Mag), d.S12Phase * 180.0 / M_PI);
    p.S21 = SParam(10.0 * log10(d.S21Mag), d.S21Phase * 180.0 / M_PI);
    p.S22 = SParam(10.0 * log10(d.S22Mag), d.S22Phase * 180.0 / M_PI);
    p.ImpedanceS11 = p.S11.ReflectionToImpedance();
    p.ImpedanceS22 = p.S22.ReflectionToImpedance();
    if(d.pointNum == m_rowcount) {
        // this point needs a new row
        beginInsertRows(QModelIndex(), d.pointNum, d.pointNum);
        m_data.append(p);
        m_rowcount++;
        endInsertRows();
    } else if(d.pointNum < m_rowcount) {
        // can simply replace the point
        m_data[d.pointNum] = p;
        emit dataChanged(createIndex(d.pointNum, 0), createIndex(d.pointNum, column_count - 1));
    }
}

void SParamTable::clearResults()
{
    beginRemoveRows(QModelIndex(), 0, m_rowcount - 1);
    m_data.clear();
    m_rowcount = 0;
    endRemoveRows();
}
