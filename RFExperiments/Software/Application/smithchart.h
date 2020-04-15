#ifndef SMITHCHART_H
#define SMITHCHART_H

#include "plot.h"
#include <QPen>
#include <QVXYModelMapper>

class SmithChart : public Plot
{
    Q_OBJECT
public:
    SmithChart(SParamTable &datatable, QString parameter = "S11", QWidget *parent = 0);

    virtual void setParameter(QString p);
    virtual QList<QString> allowedParameters();
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

    QLineSeries *m_real, *m_imag;
    QVXYModelMapper *m_mapperReal, *m_mapperImag;
};

#endif // SMITHCHART_H
