#ifndef TRACE_H
#define TRACE_H

#include <QObject>
#include <complex>
#include <map>

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

    Trace();
    void clear() {
        data.clear();
        emit cleared();
    }
    void addData(double frequency, Data d) {
        data[frequency] = d;
        emit dataAdded(frequency, d);
    }
signals:
    void cleared();
    void dataAdded(double frequency, Data d);

private:
    std::map<double, Data> data;
};

#endif // TRACE_H
