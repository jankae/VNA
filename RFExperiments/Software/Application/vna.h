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
#include "Menu/menuaction.h"

class VNA : public QMainWindow
{
    Q_OBJECT
public:
    VNA(QWidget *parent = nullptr);
private:
    static constexpr Protocol::SweepSettings defaultSweep = {
        .f_start = 100000000,
        .f_stop = 2000000000,
        .points = 501,
        .if_bandwidth = 10000,
        .mdbm_excitation = 0,
    };
private slots:
    void NewDatapoint(Protocol::Datapoint d);
private:
    void UpdateStatusPanel();
    Device device;
    Protocol::SweepSettings settings;
    unsigned int averages;
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
    QProgressDialog calDialog;

    // Calibration menu
    MenuAction *mCalOSL1, *mCalOSL2, *mCalFullOSLT;

    // Status Labels
    QLabel lStart, lCenter, lStop, lSpan, lPoints, lBandwidth;
    QLabel lCalibration;
    QLabel lAverages;
signals:
    void dataChanged();
};

#endif // VNA_H
