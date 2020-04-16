#ifndef SMITHCHART_H
#define SMITHCHART_H

#include "plot.h"
#include <QPen>
#include "sparamtable.h"

class SmithChart : public Plot
{
    Q_OBJECT
public:
    SmithChart(SParamTable &datatable, QString parameter = "S11", QWidget *parent = 0);

    virtual void setParameter(QString p);
    virtual QList<QString> allowedParameters();
    virtual void setXAxis(Protocol::SweepSettings s);
public slots:
    void dataChanged() override {
        update();
    }
protected:
    void paintEvent(QPaintEvent *event);

private:
    static constexpr double ReferenceImpedance = 50.0;
    void draw(QPainter * painter);
    QPen textPen;
    QPen chartLinesPen;
    QPen thinPen;
    QPen pointDataPen;
    QPen lineDataPen;

    /// Path for the thin arcs
    QPainterPath thinArcsPath;
    /// Path for the thick arcs
    QPainterPath thickArcsPath;

    SParamTable &table;
    int nPoints;
    double *real;
    double *imag;
};

#endif // SMITHCHART_H
