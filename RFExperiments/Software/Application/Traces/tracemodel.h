#ifndef TRACEMODEL_H
#define TRACEMODEL_H

#include <QAbstractTableModel>
#include "trace.h"
#include <vector>

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

signals:
    void traceAdded(Trace *t);

private:
    std::vector<Trace*> traces;
};

#endif // TRACEMODEL_H
