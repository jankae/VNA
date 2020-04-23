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


constexpr Protocol::SweepSettings VNA::defaultSweep;

VNA::VNA(QWidget *parent)
    : QMainWindow(parent),
      dataTable(10001)
{
    settings = defaultSweep;
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
    device.SetCallback(callback);
    SettingsChanged();
}

void VNA::NewDatapoint(Protocol::Datapoint d)
{
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
