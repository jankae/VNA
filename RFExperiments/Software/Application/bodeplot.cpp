#include "bodeplot.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QGridLayout>
#include <QVXYModelMapper>
#include <QMenu>

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
                .Y1_min = -80,
                .Y1_max = 0,
                .Y2_min = -180,
                .Y2_max = 180});
    v.push_back({.name = "S21",
                .Column1 = SParamTable::S21_db,
                .Column2 = SParamTable::S21_phase,
                .Y1_min = -80,
                .Y1_max = 0,
                .Y2_min = -180,
                .Y2_max = 180});
    v.push_back({.name = "S12",
                .Column1 = SParamTable::S12_db,
                .Column2 = SParamTable::S12_phase,
                .Y1_min = -80,
                .Y1_max = 0,
                .Y2_min = -180,
                .Y2_max = 180});
    v.push_back({.name = "S22",
                .Column1 = SParamTable::S22_db,
                .Column2 = SParamTable::S22_phase,
                .Y1_min = -80,
                .Y1_max = 0,
                .Y2_min = -180,
                .Y2_max = 180});
    return v;
}

std::vector<DisplaySetting> displaySettings = CreateDisplaySettings();

BodePlot::BodePlot(SParamTable &datatable, QString parameter, QWidget *parent):
    Plot(datatable, parent),
    m_axisX(new QValueAxis()),
    m_axisY(new QValueAxis()),
    m_axisY2(new QValueAxis())
{
    createContextMenu(parameter);

    m_series1 = new QLineSeries();
    m_series2 = new QLineSeries();

    m_mapper1 = new QVXYModelMapper(this);
    m_mapper1->setXColumn(SParamTable::Frequency);
    m_mapper1->setSeries(m_series1);
    m_mapper1->setModel(&datatable);

    m_mapper2 = new QVXYModelMapper(this);
    m_mapper2->setXColumn(SParamTable::Frequency);
    m_mapper2->setSeries(m_series2);
    m_mapper2->setModel(&datatable);


    m_series1->setPen(QPen(Qt::red));
    m_series2->setPen(QPen(Qt::darkGreen));

    m_chart = new QChart();
    m_chart->legend()->hide();
    m_chart->setMargins({0,0,0,0});
    m_chart->setBackgroundRoundness(0);

    m_chart->addSeries(m_series1);
    m_chart->addSeries(m_series2);

    m_chart->addAxis(m_axisX,Qt::AlignBottom);
    m_chart->addAxis(m_axisY,Qt::AlignLeft);
    m_chart->addAxis(m_axisY2, Qt::AlignRight);
    m_series1->attachAxis(m_axisX);
    m_series1->attachAxis(m_axisY);
    m_series2->attachAxis(m_axisX);
    m_series2->attachAxis(m_axisY2);

    m_axisX->setTickCount(5);
    m_axisX->setTickCount(6);
    m_axisY->setTickCount(9);
    m_axisY->setLabelsColor(Qt::red);

    m_axisY2->setTickCount(9);
    m_axisY2->setLabelsColor(Qt::darkGreen);

    setParameter(parameter);

    auto view = new QChartView(m_chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setContentsMargins(0,0,0,0);

    auto layout = new QGridLayout;
    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}


void BodePlot::setXAxis(Protocol::SweepSettings s)
{
    m_axisX->setRange(s.f_start, s.f_stop);
}

void BodePlot::setParameter(QString p) {
    for(auto s : displaySettings) {
        if(!s.name.compare(p)) {
            // found correct setting
            m_axisY->setRange(s.Y1_min, s.Y1_max);
            m_axisY2->setRange(s.Y2_min, s.Y2_max);
            m_mapper1->setYColumn(s.Column1);
            m_mapper2->setYColumn(s.Column2);
            m_chart->setTitle(p);
        }
    }
}

QList<QString> BodePlot::allowedParameters() {
    return displayableParameters;
}
