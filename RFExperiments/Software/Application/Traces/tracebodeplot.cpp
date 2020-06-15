#include "tracebodeplot.h"
#include <QGridLayout>
#include <qwt_plot_grid.h>
#include "qwtplotpiecewisecurve.h"
#include "qwt_series_data.h"
#include "trace.h"
#include <cmath>
#include <QFrame>
#include <qwt_plot_canvas.h>
#include <qwt_scale_div.h>
#include <qwt_plot_layout.h>

using namespace std;

template<TraceBodePlot::YAxisType E> class QwtTraceSeries : public QwtSeriesData<QPointF> {
public:
    QwtTraceSeries(Trace &t)
        : QwtSeriesData<QPointF>(),
          t(t){};
    size_t size() const override {
        return t.size();
    }
    QPointF sample(size_t i) const override {
        Trace::Data d = t.sample(i);
        QPointF p;
        p.setX(d.frequency);
        switch(E) {
        case TraceBodePlot::YAxisType::Magnitude:
            p.setY(20*log10(abs(d.S)));
            break;
        case TraceBodePlot::YAxisType::Phase:
            p.setY(arg(d.S) * 180.0 / M_PI);
            break;
        case TraceBodePlot::YAxisType::VSWR:
            if(abs(d.S) >= 1.0) {
                p.setY(numeric_limits<double>::quiet_NaN());
            } else {
                p.setY((1+abs(d.S)) / (1-abs(d.S)));
            }
            break;
        }
        return p;
    }
    QRectF boundingRect() const override {
        return qwtBoundingRect(*this);
    }

private:
    Trace &t;
};

TraceBodePlot::TraceBodePlot(TraceModel &model, QWidget *parent)
    : TracePlot(parent)
{
    plot = new QwtPlot(this);
    plot->setCanvasBackground(Background);
    auto pal = plot->palette();
    pal.setColor(QPalette::Window, Background);
    pal.setColor(QPalette::WindowText, Border);
    pal.setColor(QPalette::Text, Border);
    auto canvas = new QwtPlotCanvas(plot);
    canvas->setFrameStyle(QFrame::Plain);
    plot->setCanvas(canvas);
    plot->setPalette(pal);
    plot->setAutoFillBackground(true);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Divisions, 1.0, Qt::DotLine));
    grid->attach(plot);
    auto layout = new QGridLayout;
    layout->addWidget(plot);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    plot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    plot->plotLayout()->setAlignCanvasToScales(true);
    initializeTraceInfo(model);
    setAutoFillBackground(true);

    setYAxisType(0, YAxisType::Disabled);
    setYAxisType(1, YAxisType::Disabled);
}

TraceBodePlot::~TraceBodePlot()
{
    for(int axis = 0;axis < 2;axis++) {
        for(auto pd : curves[axis]) {
            delete pd.second.curve;
        }
    }
}

void TraceBodePlot::setXAxis(double min, double max)
{
    //plot->setAxisScale(QwtPlot::xBottom, min, max);
    QList<double> tickList;
    for(double tick = min;tick <= max;tick+= (max-min)/10) {
        tickList.append(tick);
    }
//    auto scalediv = plot->axisScaleDiv(QwtPlot::xBottom);
//    scalediv.setTicks(QwtScaleDiv::MajorTick, tickList);
    QwtScaleDiv scalediv(min, max, QList<double>(), QList<double>(), tickList);
    plot->setAxisScaleDiv(QwtPlot::xBottom, scalediv);
}

void TraceBodePlot::setYAxisType(int axis, TraceBodePlot::YAxisType type)
{
    if(AxisType[axis] != type) {
        AxisType[axis] = type;
        // remove traces that are active but not supported with the new axis type
        bool erased = false;
        do {
            erased = false;
            for(auto t : tracesAxis[axis]) {
                if(!supported(t, type)) {
                    enableTraceAxis(t, axis, false);
                    erased = true;
                    break;
                }
            }
        }while(erased);

        updateContextMenu();
        triggerReplot();
    }
    // enable/disable y axis
    auto qwtaxis = axis == 0 ? QwtPlot::yLeft : QwtPlot::yRight;
    plot->enableAxis(qwtaxis, type != YAxisType::Disabled);
    // TODO add autoscaling
    switch(type) {
    case YAxisType::Magnitude:
        plot->setAxisScale(qwtaxis, -120, 20, 10);
        break;
    case YAxisType::Phase:
        plot->setAxisScale(qwtaxis, -180, 180, 30);
        break;
    case YAxisType::VSWR:
        plot->setAxisScale(qwtaxis, 1, 10, 1);
        break;
    }
    triggerReplot();
}

void TraceBodePlot::enableTrace(Trace *t, bool enabled)
{
    for(int axis = 0;axis < 2;axis++) {
        if(supported(t, AxisType[axis])) {
            enableTraceAxis(t, axis, enabled);
        }
    }
}

void TraceBodePlot::updateContextMenu()
{
    if(contextmenu) {
        delete contextmenu;
        contextmenu = nullptr;
    }
    contextmenu = new QMenu();
    for(int axis = 0;axis < 2;axis++) {
        QMenu *axisMenu;
        if(axis == 0) {
            axisMenu = contextmenu->addMenu("Primary Axis");
        } else {
            axisMenu = contextmenu->addMenu("Secondary Axis");
        }
        auto group = new QActionGroup(this);
        for(int i=0;i<(int) YAxisType::Last;i++) {
            auto action = new QAction(AxisTypeToName((YAxisType) i));
            action->setCheckable(true);
            group->addAction(action);
            if(AxisType[axis] == (YAxisType) i) {
                action->setChecked(true);
            }
            connect(action, &QAction::triggered, [=](bool active) {
                if(active) {
                    setYAxisType(axis, (YAxisType) i);
                }
            });
        }
        axisMenu->addActions(group->actions());
    }
    for(int axis = 0;axis < 2;axis++) {
        if(AxisType[axis] == YAxisType::Disabled) {
            continue;
        }
        if(axis == 0) {
            contextmenu->addSection("Primary Traces");
        } else {
            contextmenu->addSection("Secondary Traces");
        }
        for(auto t : traces) {
            // Skip traces that are not applicable for the selected axis type
            if(!supported(t.first, AxisType[axis])) {
                continue;
            }

            auto action = new QAction(t.first->name());
            action->setCheckable(true);
            if(tracesAxis[axis].find(t.first) != tracesAxis[axis].end()) {
                action->setChecked(true);
            }
            connect(action, &QAction::toggled, [=](bool active) {
                enableTraceAxis(t.first, axis, active);
            });
            contextmenu->addAction(action);
        }
    }
    contextmenu->addSeparator();
    auto close = new QAction("Close");
    contextmenu->addAction(close);
    connect(close, &QAction::triggered, [=]() {
        emit deleted(this);
        delete this;
    });
}

bool TraceBodePlot::supported(Trace *t)
{
    // potentially possible to add every kind of trace (depends on axis)
    return true;
}

void TraceBodePlot::replot()
{
    plot->replot();
}

QString TraceBodePlot::AxisTypeToName(TraceBodePlot::YAxisType type)
{
    switch(type) {
    case YAxisType::Disabled: return "Disabled"; break;
    case YAxisType::Magnitude: return "Magnitude"; break;
    case YAxisType::Phase: return "Phase"; break;
    case YAxisType::VSWR: return "VSWR"; break;
    default: return "Unknown"; break;
    }
}

void TraceBodePlot::enableTraceAxis(Trace *t, int axis, bool enabled)
{
    bool alreadyEnabled = tracesAxis[axis].find(t) != tracesAxis[axis].end();
    if(alreadyEnabled != enabled) {
        if(enabled) {
            tracesAxis[axis].insert(t);
            CurveData cd;
            cd.data = createQwtSeriesData(*t, axis);
            cd.curve = new QwtPlotPiecewiseCurve();
            cd.curve->attach(plot);
            cd.curve->setYAxis(axis == 0 ? QwtPlot::yLeft : QwtPlot::yRight);
            cd.curve->setSamples(cd.data);
            curves[axis][t] = cd;
            // connect signals
            connect(t, &Trace::dataChanged, this, &TraceBodePlot::triggerReplot);
            connect(t, &Trace::colorChanged, this, &TraceBodePlot::traceColorChanged);
            connect(t, &Trace::visibilityChanged, this, &TraceBodePlot::traceColorChanged);
            connect(t, &Trace::visibilityChanged, this, &TraceBodePlot::triggerReplot);
            traceColorChanged(t);
        } else {
            tracesAxis[axis].erase(t);
            // clean up and delete
            if(curves[axis].find(t) != curves[axis].end()) {
                if(curves[axis][t].curve) {
                    qDebug() << "Deleting curve: " << curves[axis][t].curve;
                    delete curves[axis][t].curve;
                }
                curves[axis].erase(t);
            }
            int otherAxis = axis == 0 ? 1 : 0;
            if(curves[otherAxis].find(t) == curves[otherAxis].end()) {
                // this trace is not used anymore, disconnect from notifications
                disconnect(t, &Trace::dataChanged, this, &TraceBodePlot::triggerReplot);
                disconnect(t, &Trace::colorChanged, this, &TraceBodePlot::traceColorChanged);
                disconnect(t, &Trace::visibilityChanged, this, &TraceBodePlot::traceColorChanged);
                disconnect(t, &Trace::visibilityChanged, this, &TraceBodePlot::triggerReplot);
            }
        }

        updateContextMenu();
        triggerReplot();
    }
}

bool TraceBodePlot::supported(Trace *t, TraceBodePlot::YAxisType type)
{
    switch(type) {
    case YAxisType::Disabled:
        return false;
    case YAxisType::VSWR:
        if(!t->isReflection()) {
            return false;
        }
        break;
    default:
        break;
    }
    return true;
}

QwtSeriesData<QPointF> *TraceBodePlot::createQwtSeriesData(Trace &t, int axis)
{
    switch(AxisType[axis]) {
    case YAxisType::Magnitude:
        return new QwtTraceSeries<YAxisType::Magnitude>(t);
    case YAxisType::Phase:
        return new QwtTraceSeries<YAxisType::Phase>(t);
    case YAxisType::VSWR:
        return new QwtTraceSeries<YAxisType::VSWR>(t);
    default:
        return nullptr;
    }
}

void TraceBodePlot::traceColorChanged(Trace *t)
{
    for(int axis = 0;axis < 2;axis++) {
        if(curves[axis].find(t) != curves[axis].end()) {
            // trace active, change the pen color
            if(t->isVisible()) {
                if(axis == 0) {
                    curves[axis][t].curve->setPen(t->color());
                } else {
                    curves[axis][t].curve->setPen(t->color(), 1.0, Qt::DashLine);
                }
            } else {
                curves[axis][t].curve->setPen(t->color(), 0.0, Qt::NoPen);
            }
        }
    }
}

