#ifndef VNA_H
#define VNA_H

#include <QWidget>
#include <QMainWindow>
#include <QGridLayout>
#include "device.h"
#include "plot.h"
#include "sparamtable.h"

class VNA : public QMainWindow
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
    QList<Plot*> plots;
    QGridLayout plotLayout;
    bool fsPlot;
    int fsRow, fsColumn, fsRowSpan, fsColumnSpan;
private slots:
    void ChangeValue(QAction *action);
private:
    void SettingsChanged();
    SParamTable dataTable;
signals:
    void dataChanged();
};

#endif // VNA_H
