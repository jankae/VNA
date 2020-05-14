#include "bodeplot.h"
#include <QGridLayout>
#include <QMenu>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include "qwtplotpiecewisecurve.h"

static const QList<QString> displayableParameters = QList<QString>()
    << "S11"
    << "S21"
    << "S12"
    << "S22";

using DisplaySetting = struct {
    QString name;
    int Column1;
    int Column2;
    double Y1_min, Y1_max;
    double Y2_min, Y2_max;
};

std::vector<DisplaySetting> CreateDisplaySettings() {
    std::vector<DisplaySetting> v;
    v.push_back({.name = "S11",
                .Column1 = SParamTable::S11_db,
                .Column2 = SParamTable::S11_phase,
                .Y1_min = -60,
                .Y1_max = 10,
                .Y2_min = -180,
                .Y2_max = 180});
    v.push_back({.name = "S21",
                .Column1 = SParamTable::S21_db,
                .Column2 = SParamTable::S21_phase,
                .Y1_min = -120,
                .Y1_max = 10,
                .Y2_min = -180,
                .Y2_max = 180});
    v.push_back({.name = "S12",
                .Column1 = SParamTable::S12_db,
                .Column2 = SParamTable::S12_phase,
                .Y1_min = -120,
                .Y1_max = 10,
                .Y2_min = -180,
                .Y2_max = 180});
    v.push_back({.name = "S22",
                .Column1 = SParamTable::S22_db,
                .Column2 = SParamTable::S22_phase,
                .Y1_min = -60,
                .Y1_max = 10,
                .Y2_min = -180,
                .Y2_max = 180});
    return v;
}

std::vector<DisplaySetting> displaySettings = CreateDisplaySettings();

BodePlot::BodePlot(SParamTable &datatable, QString parameter, QWidget *parent):
    Plot(datatable, parent),
    table(datatable)
{
    createContextMenu(parameter);

    nPoints = 0;
    plot = new QwtPlot(this);
    plot->enableAxis(QwtPlot::yRight);
    plot->setCanvasBackground(Qt::white);
    auto pal = plot->palette();
    pal.setColor(QPalette::Window, Qt::white);
    plot->setPalette(pal);
    plot->setAutoFillBackground(true);
    curve1 = new QwtPlotPiecewiseCurve();
    curve2 = new QwtPlotPiecewiseCurve();

    curve1->setPen(QPen(Qt::red));
    curve2->setPen(QPen(Qt::darkGreen));

    setParameter(parameter);

    curve1->attach(plot);
    curve2->attach(plot);
    curve2->setYAxis(QwtPlot::yRight);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Qt::DotLine));
    grid->attach(plot);

    auto layout = new QGridLayout;
    layout->addWidget(plot);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    plot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}


void BodePlot::setXAxis(Protocol::SweepSettings s)
{
    nPoints = s.points;
    curve1->setRawSamples(xparam, param1, nPoints);
    curve2->setRawSamples(xparam, param2, nPoints);
    plot->setAxisScale(QwtPlot::xBottom, s.f_start, s.f_stop);
    // TODO adjust axis
}

void BodePlot::setParameter(QString p) {
    for(auto s : displaySettings) {
        if(!s.name.compare(p)) {
            // found correct setting
            xparam = table.ParamArray(SParamTable::Frequency);
            param1 = table.ParamArray(s.Column1);
            param2 = table.ParamArray(s.Column2);
            curve1->setRawSamples(xparam, param1, nPoints);
            curve2->setRawSamples(xparam, param2, nPoints);
            plot->setAxisScale(QwtPlot::yLeft, s.Y1_min, s.Y1_max, 10);
            plot->setAxisScale(QwtPlot::yRight, s.Y2_min, s.Y2_max, 30);
            plot->setTitle(p);
            break;
        }
    }
}

QList<QString> BodePlot::allowedParameters() {
    return displayableParameters;
}

void BodePlot::dataChanged()
{
    plot->replot();
}
