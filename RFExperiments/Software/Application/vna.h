#ifndef VNA_H
#define VNA_H

#include <QWidget>
#include "device.h"
#include "plot.h"

class VNA : public QWidget
{
    Q_OBJECT
public:
    VNA(QWidget *parent = nullptr);
private:
    static constexpr Protocol::SweepSettings defaultSweep = {
        .f_start = 1000000,
        .f_stop = 6000000000,
        .points = 101,
        .averaging = 1024,
        .mdbm_excitation = 0,
    };
    void NewDatapoint(Protocol::Datapoint d);
    Device device;
    Protocol::SweepSettings settings;
    Plot *plotS12, *plotS21;
    Plot *plotS11, *plotS22;
};

#endif // VNA_H
