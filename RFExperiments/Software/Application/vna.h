#ifndef VNA_H
#define VNA_H

#include <QWidget>
#include <QMainWindow>
#include <QGridLayout>
#include "device.h"
#include "plot.h"
#include "sparamtable.h"
#include "calibration.h"
#include <QProgressDialog>

class VNA : public QMainWindow
{
    Q_OBJECT
public:
    VNA(QWidget *parent = nullptr);
private:
    static constexpr Protocol::SweepSettings defaultSweep = {
        .f_start = 999999950,
        .f_stop = 1000000050,
        .points = 101,
        .if_bandwidth = 50,
        .mdbm_excitation = 0,
    };
private slots:
    void NewDatapoint(Protocol::Datapoint d);
private:
    Device device;
    Protocol::SweepSettings settings;
    QList<Plot*> plots;
    QGridLayout plotLayout;
    bool fsPlot;
    int fsRow, fsColumn, fsRowSpan, fsColumnSpan;
    void SettingsChanged();
    SParamTable dataTable;

    // Calibration
    Calibration cal;
    bool calValid;
    Calibration::Measurement calMeasurement;
    bool calMeasuring;
    bool calWaitFirst;
    QProgressDialog *calDialog;
signals:
    void dataChanged();
};

#endif // VNA_H
