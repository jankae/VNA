#ifndef TRACE_H
#define TRACE_H

#include <QObject>
#include <complex>
#include <map>
#include <QColor>
#include "touchstone.h"

class Trace : public QObject
{
    Q_OBJECT
public:

    class Data {
    public:
        double frequency;
        std::complex<double> S;
    };

    Trace(QString name = QString(), QColor color = Qt::darkYellow);
    ~Trace();

    enum class LivedataType {
        Overwrite,
        MaxHold,
        MinHold,
    };
    enum class LiveParameter {
        S11,
        S12,
        S21,
        S22,
    };

    void clear();
    void addData(Data d);
    void setName(QString name);
    void fillFromTouchstone(Touchstone &t);
    void fromLivedata(LivedataType type, LiveParameter param);
    QString name() { return _name; };
    void setColor(QColor color);
    QColor color() { return _color; };
    void setVisible(bool visible);
    bool isVisible();
    bool pause();
    bool resume();
    bool isPaused();
    bool isTouchstone();
    bool isReflection();
    LiveParameter liveParameter() { return _liveParam; }
    LivedataType liveType() { return _liveType; }
    unsigned int size() { return _data.size(); }
    Data sample(unsigned int index) { return _data.at(index); }
signals:
    void cleared(Trace *t);
    void dataAdded(Trace *t, Data d);
    void deleted(Trace *t);
    void visibilityChanged(Trace *t);
    void dataChanged();
    void nameChanged();
    void colorChanged(Trace *t);

private:
    std::vector<Data> _data;
    QString _name;
    QColor _color;
    LivedataType _liveType;
    LiveParameter _liveParam;
    bool reflection;
    bool visible;
    bool paused;
    bool touchstone;
};

#endif // TRACE_H
