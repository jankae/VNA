#include "tracewidget.h"
#include "ui_tracewidget.h"
#include "trace.h"
#include <QKeyEvent>
#include "traceeditdialog.h"
#include "traceimportdialog.h"
#include "traceexportdialog.h"

TraceWidget::TraceWidget(TraceModel &model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceWidget),
    model(model)
{
    ui->setupUi(this);
    ui->view->setModel(&model);
    ui->view->setAutoScroll(false);
    installEventFilter(this);
    createCount = 0;
}

TraceWidget::~TraceWidget()
{
    delete ui;
}

void TraceWidget::on_add_clicked()
{
    createCount++;
    auto t = new Trace("Trace #"+QString::number(createCount));
    t->setColor(QColor::fromHsl((createCount * 50) % 360, 250, 128));
    model.addTrace(t);
}

void TraceWidget::on_remove_clicked()
{
    model.removeTrace(ui->view->currentIndex().row());
}

bool TraceWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        int key = static_cast<QKeyEvent *>(event)->key();
        if(key == Qt::Key_Escape) {
            ui->view->clearSelection();
            return true;
        } else if(key == Qt::Key_Delete) {
            model.removeTrace(ui->view->currentIndex().row());
            return true;
        }
    }
    return false;
}

void TraceWidget::on_edit_clicked()
{
    if(ui->view->currentIndex().isValid()) {
        auto edit = new TraceEditDialog(*model.trace(ui->view->currentIndex().row()));
        edit->show();
    }
}

void TraceWidget::on_view_doubleClicked(const QModelIndex &index)
{
    if(index.column() == 2) {
        auto edit = new TraceEditDialog(*model.trace(index.row()));
        edit->show();
    }
}

void TraceWidget::on_view_clicked(const QModelIndex &index)
{
    if(index.column()==0) {
        model.toggleVisibility(index.row());
    } else if(index.column()==1) {
        model.togglePause(index.row());
    }
}

void TraceWidget::on_bImport_clicked()
{
    auto i = new TraceImportDialog(model);
    i->show();
}

void TraceWidget::on_bExport_clicked()
{
    auto e = new TraceExportDialog(model);
    // Attempt to set default traces (this will result in correctly populated
    // 2 port export if the initial 4 traces have not been modified)
    e->setPortNum(2);
    auto traces = model.getTraces();
    for(int i=0;i<4;i++) {
        if(i >= traces.size()) {
            break;
        }
        e->setTrace(i%2, i/2, traces[i]);
    }
    e->show();
}
