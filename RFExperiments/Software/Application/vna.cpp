#include "vna.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <math.h>
#include "bodeplot.h"
#include "smithchart.h"
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include "valueinput.h"
#include <QSpinBox>
#include <algorithm>
#include "Menu/menu.h"
#include "Menu/menuaction.h"
#include "Menu/menuvalue.h"
#include <QMessageBox>


constexpr Protocol::SweepSettings VNA::defaultSweep;

VNA::VNA(QWidget *parent)
    : QMainWindow(parent),
      dataTable(10001)
{
    settings = defaultSweep;
    calValid = false;
    calMeasuring = false;
    device.Configure(settings);
    std::function<void(Protocol::Datapoint)> callback = [=](Protocol::Datapoint d) {
        this->NewDatapoint(d);
    };
    plots.append(new SmithChart(dataTable, "S11"));
    plots.append(new BodePlot(dataTable, "S21"));
    plots.append(new BodePlot(dataTable, "S12"));
    plots.append(new SmithChart(dataTable, "S22"));

    // no plot is in fullscreen mode
    fsPlot = false;

    auto button = new QPushButton(QIcon(":/icons/plus.svg"), "Add plot");
    plotLayout.addWidget(button, 0, 0, Qt::AlignCenter);
    plotLayout.addWidget(new QPushButton("Dummy"), 0, 1, Qt::AlignCenter);
    plotLayout.addWidget(new QPushButton("Dummy"), 1, 0, Qt::AlignCenter);
    plotLayout.addWidget(new QPushButton("Dummy"), 1, 1, Qt::AlignCenter);
    for(int i=0;i<plotLayout.columnCount();i++) {
        plotLayout.setColumnStretch(i, 1);
    }
    for(int i=0;i<plotLayout.rowCount();i++) {
        plotLayout.setRowStretch(i, 1);
    }

    plotLayout.addWidget(plots[0], 0, 0);
    plotLayout.addWidget(plots[1], 0, 1);
    plotLayout.addWidget(plots[2], 1, 0);
    plotLayout.addWidget(plots[3], 1, 1);
    plotLayout.setSpacing(0);
    plotLayout.setContentsMargins(0,0,0,0);

    for(auto p : plots) {
        connect(p, &Plot::doubleClicked, [=](QWidget *w) {
            if(!fsPlot) {
                // this plot is becoming the new fullscreen plot, save old position in layout
                plotLayout.getItemPosition(plotLayout.indexOf(w), &fsRow, &fsColumn, &fsRowSpan, &fsColumnSpan);
                fsPlot = true;
                plotLayout.addWidget(w, 0, 0, 2, 2);
                w->raise();
            } else {
                // restore old widget position in layout
                plotLayout.addWidget(w, fsRow, fsColumn, fsRowSpan, fsColumnSpan);
                fsPlot = false;
            }
        });
        connect(this, &VNA::dataChanged, p, &Plot::dataChanged, Qt::QueuedConnection);
        connect(p, &Plot::deleteRequest, [=](Plot *p) {
            auto it = std::find(plots.begin(), plots.end(), p);
            if(it != plots.end()) {
                plots.erase(it);
                delete p;
            }
        });
    }

    auto menuLayout = new QStackedLayout;
    auto mMain = new Menu(*menuLayout);

    auto mFrequency = new Menu(*menuLayout);
    auto mCenter = new MenuValue("Center Frequency", (settings.f_start + settings.f_stop)/2, "Hz", " kMG", 6);
    mFrequency->addItem(mCenter);
    auto mStart = new MenuValue("Start Frequency", settings.f_start, "Hz", " kMG", 6);
    mFrequency->addItem(mStart);
    auto mStop = new MenuValue("Stop Frequency", settings.f_stop, "Hz", " kMG", 6);
    mFrequency->addItem(mStop);
    mFrequency->finalize();
    mMain->addMenu(mFrequency, "Frequency");

    auto mSpan = new Menu(*menuLayout);
    auto mSpanWidth = new MenuValue("Span", settings.f_stop - settings.f_start, "Hz", " kMG", 6);
    mSpan->addItem(mSpanWidth);
    auto mSpanZoomIn = new MenuAction("Zoom in");
    mSpan->addItem(mSpanZoomIn);
    auto mSpanZoomOut = new MenuAction("Zoom out");
    mSpan->addItem(mSpanZoomOut);
    auto mSpanFull = new MenuAction("Full span");
    mSpan->addItem(mSpanFull);
    mSpan->finalize();
    mMain->addMenu(mSpan, "Span");

    auto mAcquisition = new Menu(*menuLayout);
    auto mPoints = new MenuValue("Points", settings.points, "", " ");
    mAcquisition->addItem(mPoints);
    auto mBandwidth = new MenuValue("IF Bandwidth", settings.if_bandwidth, "Hz", " k", 2);
    mAcquisition->addItem(mBandwidth);
    mAcquisition->finalize();
    mMain->addMenu(mAcquisition, "Acquisition");

    auto mCalibration = new Menu(*menuLayout);
    auto mCalPort1 = new Menu(*menuLayout);
    auto mCalPort1Open = new MenuAction("Port 1 Open");
    auto mCalPort1Short = new MenuAction("Port 1 Short");
    auto mCalPort1Load = new MenuAction("Port 1 Load");
    mCalPort1->addItem(mCalPort1Open);
    mCalPort1->addItem(mCalPort1Short);
    mCalPort1->addItem(mCalPort1Load);
    mCalPort1->finalize();
    mCalibration->addMenu(mCalPort1, "Port 1");
    auto mCalPort2 = new Menu(*menuLayout);
    auto mCalPort2Open = new MenuAction("Port 2 Open");
    auto mCalPort2Short = new MenuAction("Port 2 Short");
    auto mCalPort2Load = new MenuAction("Port 2 Load");
    mCalPort2->addItem(mCalPort2Open);
    mCalPort2->addItem(mCalPort2Short);
    mCalPort2->addItem(mCalPort2Load);
    mCalPort2->finalize();
    mCalibration->addMenu(mCalPort2, "Port 2");
    auto mCalThrough = new MenuAction("Through");
    auto mCalIsolation = new MenuAction("Isolation");
    mCalibration->addItem(mCalThrough);
    mCalibration->addItem(mCalIsolation);

    auto mCalOSL1 = new MenuAction("Apply Port 1 OSL");
    mCalibration->addItem(mCalOSL1);

    mCalibration->finalize();
    mMain->addMenu(mCalibration, "Calibration");



    mMain->finalize();

    auto updateMenuValues = [=]() {
        mStart->setValueQuiet(settings.f_start);
        mStop->setValueQuiet(settings.f_stop);
        mSpanWidth->setValueQuiet(settings.f_stop - settings.f_start);
        mCenter->setValueQuiet((settings.f_stop + settings.f_start)/2);
    };

    // Frequency and span connections
    connect(mCenter, &MenuValue::valueChanged, [=](double newval){
        auto old_span = settings.f_stop - settings.f_start;
        settings.f_start = newval - old_span / 2;
        settings.f_stop = newval + old_span / 2;
        updateMenuValues();
        SettingsChanged();
    });
    connect(mStart, &MenuValue::valueChanged, [=](double newval){
        settings.f_start = newval;
        if(settings.f_stop < newval) {
            settings.f_stop = newval;
        }
        updateMenuValues();
        SettingsChanged();
    });
    connect(mStop, &MenuValue::valueChanged, [=](double newval){
        settings.f_stop = newval;
        if(settings.f_start > newval) {
            settings.f_start = newval;
        }
        updateMenuValues();
        SettingsChanged();
    });
    connect(mSpanWidth, &MenuValue::valueChanged, [=](double newval){
        auto old_center = (settings.f_start + settings.f_stop) / 2;
        settings.f_start = old_center - newval / 2;
        settings.f_stop = old_center + newval / 2;
        updateMenuValues();
        SettingsChanged();
    });
    connect(mSpanZoomIn, &MenuAction::triggered, [=](){
        auto center = (settings.f_start + settings.f_stop) / 2;
        auto old_span = settings.f_stop - settings.f_start;
        settings.f_start = center - old_span / 4;
        settings.f_stop = center + old_span / 4;
        updateMenuValues();
        SettingsChanged();
    });
    connect(mSpanZoomOut, &MenuAction::triggered, [=](){
        auto center = (settings.f_start + settings.f_stop) / 2;
        auto old_span = settings.f_stop - settings.f_start;
        settings.f_start = center - old_span;
        settings.f_stop = center + old_span;
        updateMenuValues();
        SettingsChanged();
    });
    connect(mSpanFull, &MenuAction::triggered, [=](){
        settings.f_start = 0;
        settings.f_stop = 6000000000;
        updateMenuValues();
        SettingsChanged();
    });
    connect(mPoints, &MenuValue::valueChanged, [=](double newval){
       settings.points = newval;
       SettingsChanged();
    });
    connect(mBandwidth, &MenuValue::valueChanged, [=](double newval){
       settings.if_bandwidth = newval;
       SettingsChanged();
    });

    // Calibration connections
    auto startCalibration = [=](Calibration::Measurement m) {
        // Trigger sweep to start from beginning
        SettingsChanged();
        calMeasurement = m;
        // Delete any already captured data of this measurement
        cal.clearMeasurement(m);
        calWaitFirst = true;
        calMeasuring = true;
        calDialog = new QProgressDialog("Performing calibration measurement...", "Abort", 0, settings.points);
        calDialog->setValue(0);
        calDialog->setWindowModality(Qt::WindowModal);
        // always show the dialog
        calDialog->setMinimumDuration(0);
        connect(calDialog, &QProgressDialog::canceled, [=]() {
            // the user aborted the calibration measurement
            calMeasuring = false;
            cal.clearMeasurement(calMeasurement);
            delete calDialog;
        });
    };
    connect(mCalPort1Open, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port1Open);
    });
    connect(mCalPort1Short, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port1Short);
    });
    connect(mCalPort1Load, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port1Load);
    });
    connect(mCalPort2Open, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port2Open);
    });
    connect(mCalPort2Short, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port2Short);
    });
    connect(mCalPort2Load, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port2Load);
    });
    connect(mCalThrough, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Through);
    });
    connect(mCalIsolation, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Isolation);
    });
    connect(mCalOSL1, &MenuAction::triggered, [=](){
        cal.constructPort1OSL();
        calValid = true;
    });

    auto mainWidget = new QWidget;
    auto plotWidget = new QWidget;
    auto mainLayout = new QHBoxLayout;
    mainWidget->setLayout(mainLayout);
    plotWidget->setLayout(&plotLayout);
    mainLayout->addWidget(plotWidget);
    auto menuWidget = new QWidget;
    menuWidget->setLayout(menuLayout);
    menuWidget->setFixedWidth(180);
    mainLayout->addWidget(menuWidget);
    setCentralWidget(mainWidget);
    //setLayout(mainLayout);
    qRegisterMetaType<Protocol::Datapoint>("Datapoint");
    auto success = connect(&device, &Device::DatapointReceived, this, &VNA::NewDatapoint);
    Q_ASSERT(success);
    SettingsChanged();
}

void VNA::NewDatapoint(Protocol::Datapoint d)
{
    if(calMeasuring) {
        if(!calWaitFirst || d.pointNum == 0) {
            calWaitFirst = false;
            cal.addMeasurement(calMeasurement, d);
            calDialog->setValue(d.pointNum);
            if(d.pointNum == settings.points - 1) {
                calMeasuring = false;
                delete calDialog;
            }
        }
    }
    if(calValid) {
        cal.correctMeasurement(d);
    }
    dataTable.addVNAResult(d);
    emit dataChanged();
}

void VNA::SettingsChanged()
{
    device.Configure(settings);
    dataTable.clearResults();
    for(auto p : plots) {
        p->setXAxis(settings);
    }
}
