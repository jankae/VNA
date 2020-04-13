#include "smithchart.h"
#include <QPainter>
#include <array>

SmithChart::SmithChart(QWidget *parent)
    : Plot(parent)
{
    // We define pens
    chartLinesPen = QPen(palette().windowText(), 0.75);
    thinPen = QPen(palette().windowText(), 0.25);
    textPen = QPen(palette().windowText(), 0.25);
    pointDataPen = QPen(QColor("red"), 4.0, Qt::SolidLine, Qt::RoundCap);
    lineDataPen = QPen(QColor("blue"), 1.0);

}

void SmithChart::setTitle(const QString &title)
{

}

void SmithChart::setXAxis(Protocol::SweepSettings s)
{

}

void SmithChart::addPoint(int index, double frequency, SParam point)
{
    Plot::addPoint(index, frequency, point);
    update();
}

void SmithChart::clearPoints()
{
    Plot::clearPoints();
    update();
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
    for(int i=1;i<m_db->count();i++) {
        auto last = SParam(m_db->at(i-1).y(), m_phase->at(i-1).y()).ReflectionToImpedance();
        auto now = SParam(m_db->at(i).y(), m_phase->at(i).y()).ReflectionToImpedance();
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
        painter->drawLine(real(last), -imag(last), real(now), -imag(now));
    }
}

void SmithChart::paintEvent(QPaintEvent * /* the event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBackground(QBrush(QColor(255, 255, 255)));
    painter.fillRect(0, 0, width(), height(), QBrush(QColor(255, 255, 255)));

    int side = qMin(width(), height());

    painter.setViewport((width()-side)/2, (height()-side)/2, side, side);
    painter.setWindow(-512, -512, 1024, 1024);

//    if(!arcsCalculated)
//    {
//        calculateInsideArcs();
//        arcsCalculated = true;
//    }

    draw(&painter);
}
