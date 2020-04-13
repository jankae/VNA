#ifndef BODEPLOT_H
#define BODEPLOT_H

#include "plot.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class BodePlot : public Plot
{
    Q_OBJECT
public:
    BodePlot(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setXAxis(Protocol::SweepSettings s);
private:
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY, *m_axisY2;
};

#endif // BODEPLOT_H
