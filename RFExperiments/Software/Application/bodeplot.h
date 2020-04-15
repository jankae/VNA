#ifndef BODEPLOT_H
#define BODEPLOT_H

#include "plot.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QVXYModelMapper>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class BodePlot : public Plot
{
    Q_OBJECT
public:
    BodePlot(SParamTable &datatable, QString parameter = "S21", QWidget *parent = 0);

    void setXAxis(Protocol::SweepSettings s);
    virtual void setParameter(QString p);
    virtual QList<QString> allowedParameters();
private:
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY, *m_axisY2;
    QLineSeries *m_series1, *m_series2;
    QVXYModelMapper *m_mapper1, *m_mapper2;
};

#endif // BODEPLOT_H
