#ifndef TRACESMITHCHART_H
#define TRACESMITHCHART_H

#include "traceplot.h"
#include <QPen>
#include "sparamtable.h"

class TraceSmithChart : public TracePlot
{
    Q_OBJECT
public:
    TraceSmithChart(TraceModel &model, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event) override;
    static constexpr double ReferenceImpedance = 50.0;
    bool supported(Trace *t) override;
    void draw(QPainter * painter, double width_factor);
    void replot() override;
    QPen textPen;
    QPen chartLinesPen;
    QPen thinPen;
    QPen pointDataPen;
    QPen lineDataPen;

    /// Path for the thin arcs
    QPainterPath thinArcsPath;
    /// Path for the thick arcs
    QPainterPath thickArcsPath;
};

#endif // TRACESMITHCHART_H
