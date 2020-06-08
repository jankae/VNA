#include "traceeditdialog.h"
#include "ui_traceeditdialog.h"
#include <QColorDialog>
#include <QFileDialog>

TraceEditDialog::TraceEditDialog(Trace &t, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TraceEditDialog),
    trace(t)
{
    ui->setupUi(this);
    ui->name->setText(t.name());
    setColor(trace.color());

    ui->GSource->setId(ui->bLive, 0);
    ui->GSource->setId(ui->bFile, 1);

    auto updateFileStatus = [=]() {
        if (ui->bFile->isChecked() && !ui->touchstoneImport->getStatus())  {
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        } else {
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        }
    };

    connect(ui->GSource, qOverload<int>(&QButtonGroup::buttonClicked), updateFileStatus);
    connect(ui->touchstoneImport, &TouchstoneImport::statusChanged, updateFileStatus);
}

TraceEditDialog::~TraceEditDialog()
{
    delete ui;
}

void TraceEditDialog::on_color_clicked()
{
    auto color = QColorDialog::getColor(trace.color(), this, "Select color", QColorDialog::DontUseNativeDialog);
    setColor(color);
}


void TraceEditDialog::on_buttonBox_accepted()
{
    trace.setName(ui->name->text());
    if (ui->bFile->isChecked()) {
        auto t = ui->touchstoneImport->getTouchstone();
        trace.fillFromTouchstone(t);
    } else {
        switch(ui->CLiveType->currentIndex()) {
        case 0: trace.fromLivedata(Trace::LivedataType::Overwrite); break;
        case 1: trace.fromLivedata(Trace::LivedataType::MaxHold); break;
        case 2: trace.fromLivedata(Trace::LivedataType::MinHold); break;
        }
    }
    delete this;
}

void TraceEditDialog::setColor(QColor c)
{
    QPalette pal = ui->color->palette();
    pal.setColor(QPalette::Button, c);
    ui->color->setAutoFillBackground(true);
    ui->color->setPalette(pal);
    ui->color->update();
    trace.setColor(c);
}
