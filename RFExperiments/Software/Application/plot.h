#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include "device.h"
#include "sparam.h"

QT_CHARTS_USE_NAMESPACE

class Plot : public QWidget
{
    Q_OBJECT
public:
    Plot(QWidget *parent = nullptr) : QWidget(parent) {
        m_db = new QLineSeries(this);
        m_phase = new QLineSeries(this);
    };
    virtual void setXAxis(Protocol::SweepSettings s) = 0;
    virtual void addPoint(int index, double frequency, SParam point) {
        if(index >= m_db->count()) {
            m_db->append(frequency, point.db);
            m_phase->append(frequency, point.phase);
        } else {
            m_db->replace(index, frequency, point.db);
            m_phase->replace(index, frequency, point.phase);
        }
    }
    virtual void clearPoints() {
        m_db->clear();
        m_phase->clear();
    }
    virtual void setTitle(const QString &title) = 0;

protected:
    QLineSeries *m_db, *m_phase;
};

#endif // PLOT_H
