#include "tracemarker.h"

TraceMarker::TraceMarker()
    : parentTrace(nullptr),
      frequency(1000000000),
      number(1),
      data(0)
{

}

TraceMarker::~TraceMarker()
{
    emit deleted(this);
}

void TraceMarker::assignTrace(Trace *t)
{
    if(parentTrace) {
        // remove connection from previous parent trace
        disconnect(parentTrace, &Trace::deleted, this, &TraceMarker::parentTraceDeleted);
        disconnect(parentTrace, &Trace::dataChanged, this, &TraceMarker::traceDataChanged);
    }
    parentTrace = t;
    connect(parentTrace, &Trace::deleted, this, &TraceMarker::parentTraceDeleted);
    connect(parentTrace, &Trace::dataChanged, this, &TraceMarker::traceDataChanged);
    constrainFrequency();
}

Trace *TraceMarker::trace()
{
    return parentTrace;
}

QString TraceMarker::readableData()
{
    auto db = 20*log10(abs(data));
    auto phase = arg(data);
    return QString::number(db, 'g', 4) + "db@" + QString::number(phase*180/M_PI, 'g', 4);
}

void TraceMarker::setFrequency(double freq)
{
    frequency = freq;
    constrainFrequency();
}

void TraceMarker::parentTraceDeleted(Trace *t)
{
    if(t == parentTrace) {
        delete this;
    }
}

void TraceMarker::traceDataChanged()
{
    // some data of the parent trace changed, check if marker data also changed
    auto tracedata = parentTrace->getData(frequency);
    if(tracedata != data) {
        data = tracedata;
        emit dataChanged();
    }
}

void TraceMarker::constrainFrequency()
{
    if(parentTrace && parentTrace->size() > 0)  {
        if(frequency > parentTrace->maxFreq()) {
            frequency = parentTrace->maxFreq();
        } else if(frequency < parentTrace->minFreq()) {
            frequency = parentTrace->minFreq();
        }
    }
}

