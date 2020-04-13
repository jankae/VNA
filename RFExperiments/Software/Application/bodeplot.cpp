#include "bodeplot.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QGridLayout>

BodePlot::BodePlot(QWidget *parent):
    Plot(parent),
    m_axisX(new QValueAxis()),
    m_axisY(new QValueAxis()),
    m_axisY2(new QValueAxis())
{
    m_db->setPen(QPen(Qt::red));
    m_phase->setPen(QPen(Qt::darkGreen));

    m_chart = new QChart();
    m_chart->legend()->hide();
    m_chart->setMargins({0,0,0,0});
    m_chart->setBackgroundRoundness(0);

    m_chart->addSeries(m_db);
    m_chart->addSeries(m_phase);

    m_chart->addAxis(m_axisX,Qt::AlignBottom);
    m_chart->addAxis(m_axisY,Qt::AlignLeft);
    m_chart->addAxis(m_axisY2, Qt::AlignRight);
    m_db->attachAxis(m_axisX);
    m_db->attachAxis(m_axisY);
    m_phase->attachAxis(m_axisX);
    m_phase->attachAxis(m_axisY2);

    m_axisX->setTickCount(5);
    m_axisX->setRange(0, 6);
    m_axisY->setRange(-80, 80);
    m_axisX->setTickCount(6);
    m_axisY->setTickCount(9);
    m_axisY->setLabelsColor(Qt::red);

    m_axisY2->setRange(-180, 180);
    m_axisY2->setTickCount(9);
    m_axisY2->setLabelsColor(Qt::darkGreen);

    auto view = new QChartView(m_chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setContentsMargins(0,0,0,0);

    auto layout = new QGridLayout;
    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void BodePlot::setTitle(const QString &title)
{
    m_chart->setTitle(title);
}

void BodePlot::setXAxis(Protocol::SweepSettings s)
{
    m_axisX->setRange(s.f_start, s.f_stop);
}
