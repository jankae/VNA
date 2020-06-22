#include "vna.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <math.h>
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include "valueinput.h"
#include <QSpinBox>
#include <algorithm>
#include "Menu/menu.h"
#include "Menu/menuaction.h"
#include "Menu/menuvalue.h"
#include "Menu/menubool.h"
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <QDateTime>
#include "unit.h"
#include "CustomWidgets/toggleswitch.h"
#include "manualcontroldialog.h"
#include "Traces/tracemodel.h"
#include "Traces/tracewidget.h"
#include "Traces/tracesmithchart.h"
#include "Traces/tracebodeplot.h"
#include "Traces/traceimportdialog.h"
#include "CustomWidgets/tilewidget.h"
#include <QDockWidget>
#include "Traces/markerwidget.h"
#include "Tools/impedancematchdialog.h"

using namespace std;

constexpr Protocol::SweepSettings VNA::defaultSweep;

VNA::VNA(QWidget *parent)
    : QMainWindow(parent),
      dataTable(10001)
{
    settings = defaultSweep;
    averages = 1;
    calValid = false;
    calMeasuring = false;
    calDialog.reset();
    device.Configure(settings);
    setWindowTitle("VNA");

    markerModel = new TraceMarkerModel(traceModel);

    // Create status panel
    auto statusLayout = new QVBoxLayout();
    statusLayout->setSpacing(0);
    QFont statusFont( "Arial", 8);
    {
        auto l = new QLabel("Start Frequency:");
        l->setAlignment(Qt::AlignLeft);
        l->setFont(statusFont);
        statusLayout->addWidget(l);
        lStart.setAlignment(Qt::AlignRight);
        lStart.setFont(statusFont);
        statusLayout->addWidget(&lStart);

        l = new QLabel("Center Frequency:");
        l->setAlignment(Qt::AlignLeft);
        l->setFont(statusFont);
        statusLayout->addWidget(l);
        lCenter.setAlignment(Qt::AlignRight);
        lCenter.setFont(statusFont);
        statusLayout->addWidget(&lCenter);

        l = new QLabel("Stop Frequency:");
        l->setAlignment(Qt::AlignLeft);
        l->setFont(statusFont);
        statusLayout->addWidget(l);
        lStop.setAlignment(Qt::AlignRight);
        lStop.setFont(statusFont);
        statusLayout->addWidget(&lStop);

        l = new QLabel("Span:");
        l->setAlignment(Qt::AlignLeft);
        l->setFont(statusFont);
        statusLayout->addWidget(l);
        lSpan.setAlignment(Qt::AlignRight);
        lSpan.setFont(statusFont);
        statusLayout->addWidget(&lSpan);

        statusLayout->addStretch();

        l = new QLabel("Points:");
        l->setAlignment(Qt::AlignLeft);
        l->setFont(statusFont);
        statusLayout->addWidget(l);
        lPoints.setAlignment(Qt::AlignRight);
        lPoints.setFont(statusFont);
        statusLayout->addWidget(&lPoints);

        l = new QLabel("IF Bandwidth:");
        l->setAlignment(Qt::AlignLeft);
        l->setFont(statusFont);
        statusLayout->addWidget(l);
        lBandwidth.setAlignment(Qt::AlignRight);
        lBandwidth.setFont(statusFont);
        statusLayout->addWidget(&lBandwidth);

        l = new QLabel("Averages:");
        l->setAlignment(Qt::AlignLeft);
        l->setFont(statusFont);
        statusLayout->addWidget(l);
        lAverages.setAlignment(Qt::AlignRight);
        lAverages.setFont(statusFont);
        statusLayout->addWidget(&lAverages);

        statusLayout->addStretch();

        l = new QLabel("Calibration:");
        l->setAlignment(Qt::AlignLeft);
        l->setFont(statusFont);
        statusLayout->addWidget(l);
        lCalibration.setAlignment(Qt::AlignRight);
        lCalibration.setFont(statusFont);
        statusLayout->addWidget(&lCalibration);
    }
    statusLayout->addStretch();

    auto tw = new TraceWidget(traceModel);
    // Create default traces
    auto tS11 = new Trace("S11", Qt::yellow);
    tS11->fromLivedata(Trace::LivedataType::Overwrite, Trace::LiveParameter::S11);
    traceModel.addTrace(tS11);
    auto tS12 = new Trace("S12", Qt::blue);
    tS12->fromLivedata(Trace::LivedataType::Overwrite, Trace::LiveParameter::S12);
    traceModel.addTrace(tS12);
    auto tS21 = new Trace("S21", Qt::green);
    tS21->fromLivedata(Trace::LivedataType::Overwrite, Trace::LiveParameter::S21);
    traceModel.addTrace(tS21);
    auto tS22 = new Trace("S22", Qt::red);
    tS22->fromLivedata(Trace::LivedataType::Overwrite, Trace::LiveParameter::S22);
    traceModel.addTrace(tS22);

    auto tracesmith1 = new TraceSmithChart(traceModel);
    tracesmith1->enableTrace(tS11, true);
    auto tracesmith2 = new TraceSmithChart(traceModel);
    tracesmith2->enableTrace(tS22, true);

    auto tracebode1 = new TraceBodePlot(traceModel);
    tracebode1->setYAxisType(0, TraceBodePlot::YAxisType::Magnitude);
    tracebode1->setYAxisType(1, TraceBodePlot::YAxisType::Phase);
    tracebode1->enableTrace(tS12, true);
    auto tracebode2 = new TraceBodePlot(traceModel);
    tracebode2->setYAxisType(0, TraceBodePlot::YAxisType::Magnitude);
    tracebode2->setYAxisType(1, TraceBodePlot::YAxisType::Phase);
    tracebode2->enableTrace(tS21, true);

    auto tiles = new TileWidget(traceModel);
    tiles->splitVertically();
    tiles->Child1()->splitHorizontally();
    tiles->Child2()->splitHorizontally();
    tiles->Child1()->Child1()->setPlot(tracesmith1);
    tiles->Child1()->Child2()->setPlot(tracebode1);
    tiles->Child2()->Child1()->setPlot(tracebode2);
    tiles->Child2()->Child2()->setPlot(tracesmith2);

    auto menuLayout = new QStackedLayout;
    auto mMain = new Menu(*menuLayout);

    auto mFrequency = new Menu(*menuLayout, "Frequency");
    auto mCenter = new MenuValue("Center Frequency", (settings.f_start + settings.f_stop)/2, "Hz", " kMG", 6);
    mFrequency->addItem(mCenter);
    auto mStart = new MenuValue("Start Frequency", settings.f_start, "Hz", " kMG", 6);
    mFrequency->addItem(mStart);
    auto mStop = new MenuValue("Stop Frequency", settings.f_stop, "Hz", " kMG", 6);
    mFrequency->addItem(mStop);
    mFrequency->finalize();
    mMain->addMenu(mFrequency);

    auto mSpan = new Menu(*menuLayout, "Span");
    auto mSpanWidth = new MenuValue("Span", settings.f_stop - settings.f_start, "Hz", " kMG", 6);
    mSpan->addItem(mSpanWidth);
    auto mSpanZoomIn = new MenuAction("Zoom in");
    mSpan->addItem(mSpanZoomIn);
    auto mSpanZoomOut = new MenuAction("Zoom out");
    mSpan->addItem(mSpanZoomOut);
    auto mSpanFull = new MenuAction("Full span");
    mSpan->addItem(mSpanFull);
    mSpan->finalize();
    mMain->addMenu(mSpan);

    auto mAcquisition = new Menu(*menuLayout, "Acquisition");
    auto mdbm = new MenuValue("Source Level", -10, "dbm", " ");
    mAcquisition->addItem(mdbm);
    auto mPoints = new MenuValue("Points", settings.points, "", " ");
    mAcquisition->addItem(mPoints);
    auto mBandwidth = new MenuValue("IF Bandwidth", settings.if_bandwidth, "Hz", " k", 2);
    mAcquisition->addItem(mBandwidth);
    auto mAverages = new MenuValue("Averages", averages);
    mAcquisition->addItem(mAverages);
    mAcquisition->finalize();
    mMain->addMenu(mAcquisition);

    auto mCalibration = new Menu(*menuLayout, "Calibration");
    auto mCalPort1 = new Menu(*menuLayout, "Port 1");
    auto mCalPort1Open = new MenuAction("Port 1 Open");
    auto mCalPort1Short = new MenuAction("Port 1 Short");
    auto mCalPort1Load = new MenuAction("Port 1 Load");
    mCalPort1->addItem(mCalPort1Short);
    mCalPort1->addItem(mCalPort1Open);
    mCalPort1->addItem(mCalPort1Load);
    mCalPort1->finalize();
    mCalibration->addMenu(mCalPort1);
    auto mCalPort2 = new Menu(*menuLayout, "Port 2");
    auto mCalPort2Open = new MenuAction("Port 2 Open");
    auto mCalPort2Short = new MenuAction("Port 2 Short");
    auto mCalPort2Load = new MenuAction("Port 2 Load");
    mCalPort2->addItem(mCalPort2Short);
    mCalPort2->addItem(mCalPort2Open);
    mCalPort2->addItem(mCalPort2Load);
    mCalPort2->finalize();
    mCalibration->addMenu(mCalPort2);
    auto mCalThrough = new MenuAction("Through");
    auto mCalIsolation = new MenuAction("Isolation");
    mCalibration->addItem(mCalThrough);
    mCalibration->addItem(mCalIsolation);

    mCalSOL1 = new MenuAction("Apply Port 1 SOL");
    mCalibration->addItem(mCalSOL1);
    mCalSOL1->setDisabled(true);

    mCalSOL2 = new MenuAction("Apply Port 2 SOL");
    mCalibration->addItem(mCalSOL2);
    mCalSOL2->setDisabled(true);

    mCalFullSOLT = new MenuAction("Apply full SOLT");
    mCalibration->addItem(mCalFullSOLT);
    mCalFullSOLT->setDisabled(true);

    auto mCalSave = new MenuAction("Save to file");
    mCalibration->addItem(mCalSave);

    auto mCalLoad = new MenuAction("Load from file");
    mCalibration->addItem(mCalLoad);

    auto mEditKit = new MenuAction("Edit CalKit");
    mCalibration->addItem(mEditKit);

    mCalibration->finalize();
    mMain->addMenu(mCalibration);

    auto mSystem = new Menu(*menuLayout, "System");
    auto aManual = new MenuAction("Manual Control");
    auto aMatchDialog = new MenuAction("Impedance Matching");
    mSystem->addItem(aManual);
    mSystem->addItem(aMatchDialog);
    mSystem->finalize();
    mMain->addMenu(mSystem);

    mMain->finalize();

    auto updateMenuValues = [=]() {
        if(settings.f_stop > 6000000000) {
            settings.f_stop = 6000000000;
        }
        if(settings.f_start > settings.f_stop) {
            settings.f_start = settings.f_stop;
        }
        mStart->setValueQuiet(settings.f_start);
        mStop->setValueQuiet(settings.f_stop);
        mSpanWidth->setValueQuiet(settings.f_stop - settings.f_start);
        mCenter->setValueQuiet((settings.f_stop + settings.f_start)/2);
    };

    // Frequency and span connections
    connect(mCenter, &MenuValue::valueChanged, [=](double newval){
        auto old_span = settings.f_stop - settings.f_start;
        if (newval > old_span / 2) {
            settings.f_start = newval - old_span / 2;
            settings.f_stop = newval + old_span / 2;
        } else {
            settings.f_start = 0;
            settings.f_stop = 2 * newval;
        }
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
        if(old_center > newval / 2) {
            settings.f_start = old_center - newval / 2;
        } else {
            settings.f_start = 0;
        }
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
        if(center > old_span) {
            settings.f_start = center - old_span;
        } else {
            settings.f_start = 0;
        }
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
    connect(mdbm, &MenuValue::valueChanged, [=](double newval){
        if(newval > -10.0) {
            newval = -10.0;
        } else if(newval < -42.0) {
            newval = -42.0;
        }
        mdbm->setValueQuiet(newval);
        settings.mdbm_excitation = newval * 100;
        SettingsChanged();
    });
    connect(mBandwidth, &MenuValue::valueChanged, [=](double newval){
       settings.if_bandwidth = newval;
       SettingsChanged();
    });
    connect(mAverages, &MenuValue::valueChanged, [=](double newval){
       averages = newval;
       dataTable.setAverages(averages);
       SettingsChanged();
    });

    // Calibration connections
    auto startCalibration = [=](Calibration::Measurement m, MenuAction *menu) {
        // Trigger sweep to start from beginning
        SettingsChanged();
        calMeasurement = m;
        // Delete any already captured data of this measurement
        cal.clearMeasurement(m);
        calWaitFirst = true;
        calMeasuring = true;
        QString text = "Measuring \"";
        text.append(Calibration::MeasurementToString(m));
        text.append("\" parameters.");
//        calDialog = new QProgressDialog(text, "Abort", 0, settings.points);
        calDialog.setRange(0, settings.points);
        calDialog.setLabelText(text);
        calDialog.setCancelButtonText("Abort");
        calDialog.setWindowTitle("Taking calibration measurement...");
        calDialog.setValue(0);
        calDialog.setWindowModality(Qt::ApplicationModal);
        // always show the dialog
        calDialog.setMinimumDuration(0);
        menu->AddSubline("Measured at " + QDateTime::currentDateTime().toString("hh:mm:ss"));
        connect(&calDialog, &QProgressDialog::canceled, [=]() {
            // the user aborted the calibration measurement
            calMeasuring = false;
            cal.clearMeasurement(calMeasurement);
            menu->RemoveSubline();
        });
    };
    connect(mCalPort1Open, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port1Open, mCalPort1Open);
    });
    connect(mCalPort1Short, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port1Short, mCalPort1Short);
    });
    connect(mCalPort1Load, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port1Load, mCalPort1Load);
    });
    connect(mCalPort2Open, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port2Open, mCalPort2Open);
    });
    connect(mCalPort2Short, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port2Short, mCalPort2Short);
    });
    connect(mCalPort2Load, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Port2Load, mCalPort2Load);
    });
    connect(mCalThrough, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Through, mCalThrough);
    });
    connect(mCalIsolation, &MenuAction::triggered, [=](){
       startCalibration(Calibration::Measurement::Isolation, mCalIsolation);
    });
    connect(mCalSOL1, &MenuAction::triggered, [=](){
        cal.constructErrorTerms(Calibration::Type::Port1SOL, calkit);
        calValid = true;
    });
    connect(mCalSOL2, &MenuAction::triggered, [=](){
        cal.constructErrorTerms(Calibration::Type::Port2SOL, calkit);
        calValid = true;
    });
    connect(mCalFullSOLT, &MenuAction::triggered, [=](){
        cal.constructErrorTerms(Calibration::Type::FullSOLT, calkit);
        calValid = true;
    });

    connect(mCalSave, &MenuAction::triggered, [=](){
        auto filename = QFileDialog::getSaveFileName(this, "Save calibration data", "", "Calibration files (*.cal)", nullptr, QFileDialog::DontUseNativeDialog);
        if(filename.length() > 0) {
            ofstream file;
            file.open(filename.toStdString());
            file << cal;
            file.close();
        }
    });

    connect(mCalLoad, &MenuAction::triggered, [=](){
        auto filename = QFileDialog::getOpenFileName(this, "Save calibration data", "", "Calibration files (*.cal)", nullptr, QFileDialog::DontUseNativeDialog);
        if(filename.length() > 0) {
            ifstream file;
            file.open(filename.toStdString());
            file >> cal;
            file.close();
            auto msg = new QMessageBox();
            msg->setText("Calibration loaded");
            QString calInfo = "The calibration contains ";
            calInfo.append(QString::number(cal.nPoints()));
            calInfo.append(" points.");
            msg->setInformativeText(calInfo);
            msg->exec();
            calValid = true;
        }
    });

    connect(mEditKit, &MenuAction::triggered, [=](){
        calkit.edit();
    });

    // Manual control trigger
    connect(aManual, &MenuAction::triggered, [=]() {
        auto control = new ManualControlDialog(device, this);
        control->show();
    });

    connect(aMatchDialog, &MenuAction::triggered, [=]() {
        auto dialog = new ImpedanceMatchDialog(*markerModel);
        dialog->show();
    });

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    auto mainWidget = new QWidget;
    auto mainLayout = new QHBoxLayout;
    mainWidget->setLayout(mainLayout);
    auto statusWidget = new QWidget;
    statusWidget->setLayout(statusLayout);
//    statusWidget->setFixedWidth(150);
    auto statusDock = new QDockWidget("Status");
    statusDock->setWidget(statusWidget);
//    statusDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, statusDock);

    auto tracesDock = new QDockWidget("Traces");
    tracesDock->setWidget(tw);
//    tracesDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, tracesDock);

//    mainLayout->addWidget(statusWidget);
    mainLayout->addWidget(tiles);
    auto menuWidget = new QWidget;
    menuWidget->setLayout(menuLayout);
//    menuWidget->setFixedWidth(180);
    auto menuDock = new QDockWidget("Menu");
    menuDock->setWidget(menuWidget);
//    menuDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, menuDock);
//    mainLayout->addWidget(menuWidget);

    auto markerWidget = new MarkerWidget(*markerModel);

    auto markerDock = new QDockWidget("Marker");
    markerDock->setWidget(markerWidget);
//    markerDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::BottomDockWidgetArea, markerDock);

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
            if(d.pointNum == settings.points - 1) {
                calMeasuring = false;
                // Check if applying calibration is available
                if(cal.calculationPossible(Calibration::Type::Port1SOL)) {
                    mCalSOL1->setEnabled(true);
                }
                if(cal.calculationPossible(Calibration::Type::Port2SOL)) {
                    mCalSOL2->setEnabled(true);
                }
                if(cal.calculationPossible(Calibration::Type::FullSOLT)) {
                    mCalFullSOLT->setEnabled(true);
                }
            }
            calDialog.setValue(d.pointNum + 1);
        }
    }
    if(calValid) {
        cal.correctMeasurement(d);
    }
    dataTable.addVNAResult(d);
    traceModel.addVNAData(d);
    emit dataChanged();
    if(d.pointNum == settings.points - 1) {
        UpdateStatusPanel();
    }
}

void VNA::UpdateStatusPanel()
{
    lStart.setText(Unit::ToString(settings.f_start, "Hz", " kMG", 4));
    lCenter.setText(Unit::ToString((settings.f_start + settings.f_stop)/2, "Hz", " kMG", 4));
    lStop.setText(Unit::ToString(settings.f_stop, "Hz", " kMG", 4));
    lSpan.setText(Unit::ToString(settings.f_stop - settings.f_start, "Hz", " kMG", 4));
    lPoints.setText(QString::number(settings.points));
    lBandwidth.setText(Unit::ToString(settings.if_bandwidth, "Hz", " k", 2));
    lAverages.setText(QString::number(dataTable.getAcquiredAverages()) + "/" + QString::number(averages));
    switch(cal.getInterpolation(settings)) {
    case Calibration::InterpolationType::NoCalibration:
        lCalibration.setText("Off");
        break;
    case Calibration::InterpolationType::Extrapolate:
        lCalibration.setText("Enabled/Extrapolating");
        break;
    case Calibration::InterpolationType::Interpolate:
        lCalibration.setText("Enabled/Interpolating");
        break;
    case Calibration::InterpolationType::Exact:
    case Calibration::InterpolationType::Unchanged:
        lCalibration.setText("Enabled");
        break;
    }
}

void VNA::SettingsChanged()
{
    device.Configure(settings);
    dataTable.clearResults();
    traceModel.clearVNAData();
    UpdateStatusPanel();
}
