#ifndef SMITHCHART_H
#define SMITHCHART_H

#include "plot.h"
#include <QPen>

class SmithChart : public Plot
{
    Q_OBJECT
public:
    SmithChart(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setXAxis(Protocol::SweepSettings s);
    void addPoint(int index, double frequency, SParam point);
    void clearPoints();

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
};

#endif // SMITHCHART_H
