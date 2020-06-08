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
        std::complex<double> S11;
        std::complex<double> S12;
        std::complex<double> S21;
        std::complex<double> S22;
    };

    Trace(QString name = QString());
    ~Trace();

    enum class LivedataType {
        Overwrite,
        MaxHold,
        MinHold,
    };

    void clear();
    void addData(double frequency, Data d);
    void setName(QString name);
    void fillFromTouchstone(Touchstone &t);
    void fromLivedata(LivedataType type);
    QString name() { return _name; };
    void setColor(QColor color);
    QColor color() { return _color; };
    void setVisible(bool visible);
    bool isVisible();
    bool pause();
    bool resume();
    bool isPaused();
    bool isTouchstone();
signals:
    void cleared();
    void dataAdded(double frequency, Data d);
    void deleted();
    void visibilityChanged();

private:
    std::map<double, Data> _data;
    QString _name;
    QColor _color;
    LivedataType liveType;
    bool visible;
    bool paused;
    bool touchstone;
};

#endif // TRACE_H
