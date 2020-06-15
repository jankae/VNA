#include "traceplot.h"

constexpr QColor TracePlot::Background;
constexpr QColor TracePlot::Border;
constexpr QColor TracePlot::Divisions;

TracePlot::TracePlot(QWidget *parent) : QWidget(parent)
{
    contextmenu = nullptr;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lastUpdate = QTime::currentTime();
}

void TracePlot::enableTrace(Trace *t, bool enabled)
{
    if(traces[t] != enabled) {
        traces[t] = enabled;
        if(enabled) {
            // connect signals
            connect(t, &Trace::dataChanged, this, &TracePlot::triggerReplot);
            connect(t, &Trace::visibilityChanged, this, &TracePlot::triggerReplot);
        } else {
            // disconnect from notifications
            disconnect(t, &Trace::dataChanged, this, &TracePlot::triggerReplot);
            disconnect(t, &Trace::visibilityChanged, this, &TracePlot::triggerReplot);
        }
        updateContextMenu();
        triggerReplot();
    }
}

void TracePlot::mouseDoubleClickEvent(QMouseEvent *event) {
    emit doubleClicked(this);
}

void TracePlot::initializeTraceInfo(TraceModel &model)
{
    // Populate already present traces
    auto tvect = model.getTraces();
    for(auto t : tvect) {
        newTraceAvailable(t);
    }

    // connect notification of traces added at later point
    connect(&model, &TraceModel::traceAdded, this, &TracePlot::newTraceAvailable);
}

void TracePlot::contextMenuEvent(QContextMenuEvent *event)
{
    contextmenu->exec(event->globalPos());
}

void TracePlot::updateContextMenu()
{
    if(contextmenu) {
        delete contextmenu;
        contextmenu = nullptr;
    }
    contextmenu = new QMenu();
    contextmenu->addSection("Traces");
    // Populate context menu
    for(auto t : traces) {
        auto action = new QAction(t.first->name());
        action->setCheckable(true);
        if(t.second) {
            action->setChecked(true);
        }
        connect(action, &QAction::toggled, [=](bool active) {
            enableTrace(t.first, active);
        });
        contextmenu->addAction(action);
    }
    contextmenu->addSeparator();
    auto close = new QAction("Close");
    contextmenu->addAction(close);
    connect(close, &QAction::triggered, [=]() {
        emit deleted(this);
        delete this;
    });
}

void TracePlot::newTraceAvailable(Trace *t)
{
    if(supported(t)) {
        traces[t] = false;
        connect(t, &Trace::deleted, this, &TracePlot::traceDeleted);
        connect(t, &Trace::nameChanged, this, &TracePlot::updateContextMenu);
    }
    updateContextMenu();
}

void TracePlot::traceDeleted(Trace *t)
{
    enableTrace(t, false);
    traces.erase(t);
    updateContextMenu();
    triggerReplot();
}

void TracePlot::triggerReplot()
{
    auto now = QTime::currentTime();
    if (lastUpdate.msecsTo(now) >= MinUpdateInterval) {
        replot();
        lastUpdate = now;
    }
}
