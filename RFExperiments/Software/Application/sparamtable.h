#ifndef SPARAMTABLE_H
#define SPARAMTABLE_H

#include <QtCore/QAbstractTableModel>
#include "device.h"
#include "sparam.h"

class SParamTable : public QAbstractTableModel
{
public:  
    SParamTable(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void addVNAResult(Protocol::Datapoint d);
    void clearResults();

    enum Parameter {
        Frequency = 0,
        S11_db = 1,
        S11_phase = 2,
        S12_db = 3,
        S12_phase = 4,
        S21_db = 5,
        S21_phase = 6,
        S22_db = 7,
        S22_phase = 8,
        S11_ImpedanceReal = 9,
        S11_ImpedanceImag = 10,
        S22_ImpedanceReal = 11,
        S22_ImpedanceImag = 12,
    };

private:
    static constexpr int column_count = 13;
    class Entry {
    public:
        Entry() :
            S11(0,0),
            S12(0,0),
            S21(0,0),
            S22(0,0),
            ImpedanceS11(0),
            ImpedanceS22(0){};
        double operator[] (size_t i) const {
            switch(i) {
            case Frequency: return frequency;
            case S11_db: return S11.db;
            case S11_phase: return S11.phase;
            case S12_db: return S12.db;
            case S12_phase: return S12.phase;
            case S21_db: return S21.db;
            case S21_phase: return S21.phase;
            case S22_db: return S22.db;
            case S22_phase: return S22.phase;
            case S11_ImpedanceReal: return ImpedanceS11.real();
            case S11_ImpedanceImag: return ImpedanceS11.imag();
            case S22_ImpedanceReal: return ImpedanceS22.real();
            case S22_ImpedanceImag: return ImpedanceS22.imag();
            default: throw std::out_of_range("Index does not exist in SParamTable::Entry");
            }
        }
        double frequency;
        SParam S11, S12, S21, S22;
        std::complex<double> ImpedanceS11, ImpedanceS22;
    };
    QList<Entry> m_data;
    int m_rowcount;
};

#endif // SPARAMTABLE_H
