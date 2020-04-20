#ifndef BODEPLOT_H
#define BODEPLOT_H

#include "plot.h"
#include <qwt_plot_curve.h>
#include <qwt_plot.h>

class BodePlot : public Plot
{
    Q_OBJECT
public:
    BodePlot(SParamTable &datatable, QString parameter = "S21", QWidget *parent = 0);
    ~BodePlot() {
        delete curve1;
        delete curve2;
        delete plot;
    }
    void setXAxis(Protocol::SweepSettings s) override;
    virtual void setParameter(QString p) override;
    virtual QList<QString> allowedParameters() override;
public slots:
    void dataChanged() override;
private:
    int nPoints;
    double *xparam, *param1, *param2;
    SParamTable &table;
    QwtPlotCurve *curve1;
    QwtPlotCurve *curve2;
    QwtPlot *plot;
};

#endif // BODEPLOT_H
