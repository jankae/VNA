#include "tracesmithchart.h"
#include <QPainter>
#include <array>
#include <math.h>
#include "tracemarker.h"

TraceSmithChart::TraceSmithChart(TraceModel &model, QWidget *parent)
    : TracePlot(parent)
{
    chartLinesPen = QPen(palette().windowText(), 0.75);
    thinPen = QPen(palette().windowText(), 0.25);
    textPen = QPen(palette().windowText(), 0.25);
    pointDataPen = QPen(QColor("red"), 4.0, Qt::SolidLine, Qt::RoundCap);
    lineDataPen = QPen(QColor("blue"), 1.0);
    initializeTraceInfo(model);
}

void TraceSmithChart::draw(QPainter * painter, double width_factor) {
    painter->setPen(QPen(1.0 * width_factor));
    painter->setBrush(palette().windowText());
    painter->setRenderHint(QPainter::Antialiasing);

//    // Display parameter name
//    QFont font = painter->font();
//    font.setPixelSize(48);
//    font.setBold(true);
//    painter->setFont(font);
//    painter->drawText(-512, -512, title);

    // Outer circle
    painter->setPen(QPen(Border, 1.5 * width_factor));
    QRectF rectangle(-512, -512, 1024, 1024);
    painter->drawArc(rectangle, 0, 5760);

    constexpr int Circles = 6;
    painter->setPen(QPen(Divisions, 0.5 * width_factor, Qt::DashLine));
    for(int i=1;i<Circles;i++) {
        rectangle.adjust(1024/Circles, 512/Circles, 0, -512/Circles);
        painter->drawArc(rectangle, 0, 5760);
    }

    painter->drawLine(-512, 0, 512, 0);
    constexpr std::array<double, 5> impedanceLines = {10, 25, 50, 100, 250};
    for(auto z : impedanceLines) {
        z /= ReferenceImpedance;
        auto radius = 512.0 * 1/z;
        double span = M_PI - 2 * atan(radius/512);
        span *= 5760 / (2 * M_PI);
        QRectF rectangle(512 - radius, -2*radius, 2 * radius, 2 * radius);
        painter->drawArc(rectangle, 4320 - span, span);
        rectangle = QRectF(512 - radius, 0, 2 * radius, 2 * radius);
        painter->drawArc(rectangle, 1440, span);
    }

    for(auto t : traces) {
        if(!t.second) {
            // trace not enabled in plot
            continue;
        }
        auto trace = t.first;
        if(!trace->isVisible()) {
            // trace marked invisible
            continue;
        }
        painter->setPen(QPen(trace->color(), 1.5 * width_factor));
        int nPoints = trace->size();
        for(int i=1;i<nPoints;i++) {
            auto last = trace->sample(i-1).S;
            auto now = trace->sample(i).S;
            if(isnan(now.real())) {
                break;
            }
            // scale to size of smith diagram
            last *= 512;
            now *= 512;
            // draw line
            painter->drawLine(std::real(last), -std::imag(last), std::real(now), -std::imag(now));
        }
        auto markers = t.first->getMarkers();
        for(auto m : markers) {
            auto coords = m->getData();
            coords *= 512;
            auto symbol = m->getSymbol();
            symbol = symbol.scaled(symbol.width()*width_factor, symbol.height()*width_factor);
            painter->drawPixmap(coords.real() - symbol.width()/2, -coords.imag() - symbol.height(), symbol);
        }
    }
}

void TraceSmithChart::replot()
{
    update();
}

void TraceSmithChart::paintEvent(QPaintEvent * /* the event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBackground(QBrush(Background));
    painter.fillRect(0, 0, width(), height(), QBrush(Background));

    double side = qMin(width(), height()) * 0.9;

    painter.setViewport((width()-side)/2, (height()-side)/2, side, side);
    painter.setWindow(-512, -512, 1024, 1024);

    draw(&painter, 1024.0/side);
}

bool TraceSmithChart::supported(Trace *t)
{
    if(t->isReflection()) {
        return true;
    } else {
        return false;
    }
}
