#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QMenu>
#include "device.h"
#include "sparam.h"
#include "sparamtable.h"

QT_CHARTS_USE_NAMESPACE

class Plot : public QWidget
{
    Q_OBJECT
public:
    Plot(SParamTable &datatable, QWidget *parent = nullptr) : QWidget(parent) {
        data = &datatable;
    };

    virtual void setXAxis(Protocol::SweepSettings s){};
    virtual void setParameter(QString p) = 0;
    virtual QList<QString> allowedParameters() = 0;

    void contextMenuEvent(QContextMenuEvent *event)
    {
        contextmenu.exec(event->globalPos());
    }

    void mouseDoubleClickEvent(QMouseEvent *event) {
        emit doubleClicked(this);
    }

signals:
    void doubleClicked(QWidget *w);
    void deleteRequest(Plot *p);

public slots:
    virtual void dataChanged(){};
protected:
    void createContextMenu(QString &selectedParameter) {
        if(selectedParameter.isEmpty()) {
            selectedParameter = allowedParameters()[0];
        }
        contextmenu.addSection("Parameter");
        // Populate context menu
        auto group = new QActionGroup(this);
        for(auto p : allowedParameters()) {
            auto action = new QAction(p);
            action->setCheckable(true);
            group->addAction(action);
            if(!p.compare(selectedParameter)) {
                action->setChecked(true);
            }
            connect(group, &QActionGroup::triggered, [=](QAction *a) {
                setParameter(a->text());
            });
        }
        contextmenu.addActions(group->actions());
        contextmenu.addSeparator();
        auto close = new QAction("Close");
        contextmenu.addAction(close);
        connect(close, &QAction::triggered, [=]() {
            emit deleteRequest(this);
        });
    }
    SParamTable *data;
    QMenu contextmenu;
};

#endif // PLOT_H
