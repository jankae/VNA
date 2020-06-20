#ifndef TRACEMODEL_H
#define TRACEMODEL_H

#include <QAbstractTableModel>
#include "trace.h"
#include <vector>
#include "device.h"

class TraceModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TraceModel(QObject *parent = 0);

    void addTrace(Trace *t);
    void removeTrace(int index);
    Trace *trace(int index);
    void toggleVisibility(int index);
    void togglePause(int index);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    std::vector<Trace*> getTraces();
signals:
    void traceAdded(Trace *t);
    void traceRemoved(Trace *t);

public slots:
    void clearVNAData();
    void addVNAData(Protocol::Datapoint d);

private:
    std::vector<Trace*> traces;
};

#endif // TRACEMODEL_H
