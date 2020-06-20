#ifndef TRACEMARKER_H
#define TRACEMARKER_H

#include <QPixmap>
#include <QObject>
#include "trace.h"

class TraceMarker : public QObject
{
    friend class TraceMarkerModel;
    Q_OBJECT;
public:
    TraceMarker();
    ~TraceMarker();
    void assignTrace(Trace *t);
    Trace* trace();
    QString readableData();
public slots:
    void setFrequency(double freq);
signals:
    void deleted(TraceMarker *m);
    void dataChanged();

private slots:
    void parentTraceDeleted(Trace *t);
    void traceDataChanged();
private:
    void constrainFrequency();
    Trace *parentTrace;
    double frequency;
    int number;
    std::complex<double> data;
};

#endif // TRACEMARKER_H
