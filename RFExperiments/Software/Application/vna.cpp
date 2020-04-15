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

constexpr Protocol::SweepSettings VNA::defaultSweep;

VNA::VNA(QWidget *parent)
    : QMainWindow(parent)
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
    }

    auto tbSweep = addToolBar("Sweep");
    addToolBar(Qt::RightToolBarArea, tbSweep);
    tbSweep->setIconSize(QSize(64, 64));
    tbSweep->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    auto aStart = new QAction(QPixmap(":/icons/start.png"), "Start", this);
    auto aCenter = new QAction(QPixmap(":/icons/center.png"), "Center", this);
    auto aStop = new QAction(QPixmap(":/icons/stop.png"), "Stop", this);
    auto aSweep = new QAction(QPixmap(":/icons/span.png"), "Span", this);
    tbSweep->addSeparator();
    tbSweep->addAction(aCenter);
    tbSweep->addAction(aStart);
    tbSweep->addAction(aStop);
    tbSweep->addAction(aSweep);
    tbSweep->addSeparator();

    connect(tbSweep, &QToolBar::actionTriggered, this, &VNA::ChangeValue);

    auto central = new QWidget;
    central->setLayout(&plotLayout);
    setCentralWidget(central);
    //setLayout(mainLayout);
    device.SetCallback(callback);
    SettingsChanged();
}

void VNA::NewDatapoint(Protocol::Datapoint d)
{
    dataTable.addVNAResult(d);
}

void VNA::ChangeValue(QAction *action)
{
    std::vector<ValueInput::Unit> u;
    u.push_back(ValueInput::Unit("Hz", 1.0));
    u.push_back(ValueInput::Unit("kHz", 1000.0));
    u.push_back(ValueInput::Unit("MHz", 1000000.0));
    u.push_back(ValueInput::Unit("GHz", 1000000000.0));
    auto dialog = new ValueInput(u, action->text());
    if(action->text() == "Start") {
        connect(dialog, &ValueInput::ValueChanged, [=](double newval) {
            settings.f_start = newval;
            if(settings.f_stop < newval) {
                settings.f_stop = newval;
            }
            SettingsChanged();
        });
    } else if(action->text() == "Stop") {
        connect(dialog, &ValueInput::ValueChanged, [=](double newval) {
            settings.f_stop = newval;
            if(settings.f_start > newval) {
                settings.f_start = newval;
            }
            SettingsChanged();
        });
    } else if(action->text() == "Center") {
        connect(dialog, &ValueInput::ValueChanged, [=](double newval) {
            auto old_span = settings.f_stop - settings.f_start;
            settings.f_start = newval - old_span / 2;
            settings.f_stop = newval + old_span / 2;
            SettingsChanged();
        });
    } else if(action->text() == "Span") {
        connect(dialog, &ValueInput::ValueChanged, [=](double newval) {
            auto old_center = (settings.f_start + settings.f_stop) / 2;
            settings.f_start = old_center - newval / 2;
            settings.f_stop = old_center + newval / 2;
            SettingsChanged();
        });
    }
}

void VNA::SettingsChanged()
{
    device.Configure(settings);
    dataTable.clearResults();
    for(auto p : plots) {
        p->setXAxis(settings);
    }
}
