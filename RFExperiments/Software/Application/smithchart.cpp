#include "smithchart.h"
#include <QPainter>
#include <array>
#include <math.h>

SmithChart::SmithChart(SParamTable &datatable, QString parameter, QWidget *parent)
    : Plot(datatable, parent),
      table(datatable)
{
    chartLinesPen = QPen(palette().windowText(), 0.75);
    thinPen = QPen(palette().windowText(), 0.25);
    textPen = QPen(palette().windowText(), 0.25);
    pointDataPen = QPen(QColor("red"), 4.0, Qt::SolidLine, Qt::RoundCap);
    lineDataPen = QPen(QColor("blue"), 1.0);

    nPoints = 0;
    createContextMenu(parameter);
    setParameter(parameter);
}

void SmithChart::draw(QPainter * painter) {
    painter->setPen(QPen(0.75));
    painter->setBrush(palette().windowText());

    // Outer circle
    QRectF rectangle(-512, -512, 1024, 1024);
    painter->drawArc(rectangle, 0, 5760);
    painter->drawLine(-512, 0, 512, 0);

    constexpr int Circles = 6;
    painter->setPen(QPen(palette().windowText(), 0.75, Qt::DashLine));
    for(int i=1;i<Circles;i++) {
        rectangle.adjust(1024/Circles, 512/Circles, 0, -512/Circles);
        painter->drawArc(rectangle, 0, 5760);
    }

    // TODO
//    constexpr std::array<double, 5> impedanceLines = {10, 25, 50, 100, 250};
//    for(auto z : impedanceLines) {
//        z /= 50;
//    }

    painter->setPen(QPen(Qt::red));
    for(int i=1;i<nPoints;i++) {
        auto last = std::complex<double>(real[i-1], imag[i-1]);
        auto now = std::complex<double>(real[i], imag[i]);
        if(isnan(now.real())) {
            break;
        }
        // transform into smith diagramm
        last /= ReferenceImpedance;
        now /= ReferenceImpedance;
        constexpr auto cpl_one = std::complex<double>(1.0, 0.0);
        last = (last - cpl_one) / (last + cpl_one);
        now = (now - cpl_one) / (now + cpl_one);
        // scale to size of smith diagram
        last *= 512;
        now *= 512;
        // draw line
        painter->drawLine(std::real(last), -std::imag(last), std::real(now), -std::imag(now));
    }
}

void SmithChart::setParameter(QString p)
{
    if(p == "S11") {
        real = table.ParamArray(SParamTable::S11_ImpedanceReal);
        imag = table.ParamArray(SParamTable::S11_ImpedanceImag);
    } else {
        real = table.ParamArray(SParamTable::S22_ImpedanceReal);
        imag = table.ParamArray(SParamTable::S22_ImpedanceImag);
    }
}

QList<QString> SmithChart::allowedParameters()
{
    return QList<QString>() << "S11" << "S22";
}

void SmithChart::setXAxis(Protocol::SweepSettings s)
{
    nPoints = s.points;
}

void SmithChart::paintEvent(QPaintEvent * /* the event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBackground(QBrush(QColor(255, 255, 255)));
    painter.fillRect(0, 0, width(), height(), QBrush(QColor(255, 255, 255)));

    int side = qMin(width(), height()) * 0.8;

    painter.setViewport((width()-side)/2, (height()-side)/2, side, side);
    painter.setWindow(-512, -512, 1024, 1024);

//    if(!arcsCalculated)
//    {
//        calculateInsideArcs();
//        arcsCalculated = true;
//    }

    draw(&painter);
}
