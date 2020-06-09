#ifndef TRACEPLOT_H
#define TRACEPLOT_H

#include <QWidget>
#include "tracemodel.h"
#include <QMenu>
#include <QContextMenuEvent>

class TracePlot : public QWidget
{
    Q_OBJECT
public:
    TracePlot( QWidget *parent = nullptr);

    void enableTrace(Trace *t, bool enabled);
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void setXAxis(double min, double max){};

signals:
    void doubleClicked(QWidget *w);
    void deleted(TracePlot*);

protected:
    // need to be called in derived class constructor
    void initializeTraceInfo(TraceModel &model);
    void contextMenuEvent(QContextMenuEvent *event) override;
    void updateContextMenu();
    virtual bool supported(Trace *t) = 0;
    std::map<Trace*, bool> traces;
    QMenu *contextmenu;

protected slots:
    void newTraceAvailable(Trace *t);
    void traceDeleted(Trace *t);

};

#endif // TRACEPLOT_H
