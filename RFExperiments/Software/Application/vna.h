#ifndef VNA_H
#define VNA_H

#include <QWidget>
#include <QMainWindow>
#include <QGridLayout>
#include "device.h"
#include "plot.h"
#include "sparamtable.h"
#include "calibration.h"

class VNA : public QMainWindow
{
    Q_OBJECT
public:
    VNA(QWidget *parent = nullptr);
private:
    static constexpr Protocol::SweepSettings defaultSweep = {
        .f_start = 25000000,
        .f_stop = 3000000000,
        .points = 101,
        .if_bandwidth = 50,
        .mdbm_excitation = 0,
    };
    void NewDatapoint(Protocol::Datapoint d);
    Device device;
    Protocol::SweepSettings settings;
    QList<Plot*> plots;
    QGridLayout plotLayout;
    bool fsPlot;
    int fsRow, fsColumn, fsRowSpan, fsColumnSpan;
private:
    void SettingsChanged();
    SParamTable dataTable;
    Calibration cal;
    bool calValid;
    Calibration::Measurement calMeasurement;
    bool calMeasuring;
signals:
    void dataChanged();
};

#endif // VNA_H
