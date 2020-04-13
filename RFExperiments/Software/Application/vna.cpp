#include "vna.h"
#include <QGridLayout>
#include <QPushButton>
#include <math.h>
#include "bodeplot.h"
#include "smithchart.h"

constexpr Protocol::SweepSettings VNA::defaultSweep;

VNA::VNA(QWidget *parent)
    : QWidget(parent)
{
    settings = defaultSweep;
    device.Configure(settings);
    std::function<void(Protocol::Datapoint)> callback = [=](Protocol::Datapoint d) {
        this->NewDatapoint(d);
    };
    plotS12 = new BodePlot;
    plotS12->setTitle("S12");
    plotS12->setXAxis(settings);

    plotS21 = new BodePlot;
    plotS21->setTitle("S21");
    plotS21->setXAxis(settings);

    plotS11 = new SmithChart;
    plotS11->setTitle("S11");
    plotS11->setXAxis(settings);

    plotS22 = new SmithChart;
    plotS22->setTitle("S22");
    plotS22->setXAxis(settings);

    auto layout = new QGridLayout;
    layout->addWidget(plotS12, 0, 1);
    layout->addWidget(plotS21, 1, 0);
    layout->addWidget(plotS11, 0, 0);
    layout->addWidget(plotS22, 1, 1);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

    setLayout(layout);
    device.SetCallback(callback);
}

void VNA::NewDatapoint(Protocol::Datapoint d)
{
    // Convert from factor to db and from rad to deg
    auto S11 = SParam(10.0 * log10(d.S11Mag), d.S11Phase * 180.0 / M_PI);
    auto S12 = SParam(10.0 * log10(d.S12Mag), d.S12Phase * 180.0 / M_PI);
    auto S21 = SParam(10.0 * log10(d.S21Mag), d.S21Phase * 180.0 / M_PI);
    auto S22 = SParam(10.0 * log10(d.S22Mag), d.S22Phase * 180.0 / M_PI);

    plotS12->addPoint(d.pointNum, d.frequency, S12);
    plotS21->addPoint(d.pointNum, d.frequency, S21);
    plotS11->addPoint(d.pointNum, d.frequency, S11);
    plotS22->addPoint(d.pointNum, d.frequency, S22);
}
