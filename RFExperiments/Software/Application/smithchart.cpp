#include "smithchart.h"
#include <QPainter>
#include <array>

SmithChart::SmithChart(SParamTable &datatable, QString parameter, QWidget *parent)
    : Plot(datatable, parent)
{
    chartLinesPen = QPen(palette().windowText(), 0.75);
    thinPen = QPen(palette().windowText(), 0.25);
    textPen = QPen(palette().windowText(), 0.25);
    pointDataPen = QPen(QColor("red"), 4.0, Qt::SolidLine, Qt::RoundCap);
    lineDataPen = QPen(QColor("blue"), 1.0);

    m_real = new QLineSeries(this);
    m_imag = new QLineSeries(this);

    m_mapperReal = new QVXYModelMapper(this);
    m_mapperReal->setXColumn(SParamTable::Frequency);
    m_mapperReal->setSeries(m_real);
    m_mapperReal->setModel(&datatable);

    m_mapperImag = new QVXYModelMapper(this);
    m_mapperImag->setXColumn(SParamTable::Frequency);
    m_mapperImag->setSeries(m_imag);
    m_mapperImag->setModel(&datatable);

    createContextMenu(parameter);
    setParameter(parameter);

    connect(m_real, &QLineSeries::pointReplaced, [=](int) {
        update();
    });
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
    for(int i=1;i<m_real->count();i++) {
        auto last = std::complex<double>(m_real->at(i-1).y(), m_imag->at(i-1).y());
        auto now = std::complex<double>(m_real->at(i).y(), m_imag->at(i).y());
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

void SmithChart::setParameter(QString p)
{
    if(p == "S11") {
        m_mapperReal->setYColumn(SParamTable::S11_ImpedanceReal);
        m_mapperImag->setYColumn(SParamTable::S11_ImpedanceImag);
    } else {
        m_mapperReal->setYColumn(SParamTable::S22_ImpedanceReal);
        m_mapperImag->setYColumn(SParamTable::S22_ImpedanceImag);
    }
}

QList<QString> SmithChart::allowedParameters()
{
    return QList<QString>() << "S11" << "S22";
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
