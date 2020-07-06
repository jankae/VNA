#ifndef TRACEBODEPLOT_H
#define TRACEBODEPLOT_H

#include "traceplot.h"
#include <set>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_plot_marker.h>

class TraceBodePlot : public TracePlot
{
    Q_OBJECT
public:
    TraceBodePlot(TraceModel &model, QWidget *parent = nullptr);
    ~TraceBodePlot();

    enum class YAxisType {
        Disabled = 0,
        Magnitude = 1,
        Phase = 2,
        VSWR = 3,
        Last,
    };

    virtual void setXAxis(double min, double max) override;
    void setYAxisType(int axis, YAxisType type);
    void enableTrace(Trace *t, bool enabled) override;

protected:
    virtual void updateContextMenu();
    virtual bool supported(Trace *t);
    void replot() override;

private slots:
    void traceColorChanged(Trace *t);
    void markerAdded(TraceMarker *m) override;
    void markerRemoved(TraceMarker *m) override;
    void markerDataChanged(TraceMarker *m);
private:
    QString AxisTypeToName(YAxisType type);
    void enableTraceAxis(Trace *t, int axis, bool enabled);
    bool supported(Trace *t, YAxisType type);
    QwtSeriesData<QPointF> *createQwtSeriesData(Trace &t, int axis);

    std::set<Trace*> tracesAxis[2];
    YAxisType AxisType[2];

    using CurveData = struct {
        QwtPlotCurve *curve;
        QwtSeriesData<QPointF> *data;
    };

    std::map<Trace*, CurveData> curves[2];
    std::map<TraceMarker*, QwtPlotMarker*> markers;
    QwtPlot *plot;
    TraceMarker *selectedMarker;
    QwtPlotCurve *selectedCurve;
};

#endif // TRACEBODEPLOT_H
