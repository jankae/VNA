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

    if(t.isTouchstone()) {
        ui->bFile->click();
    }

    auto updateFileStatus = [=]() {
        if (ui->bFile->isChecked() && !ui->touchstoneImport->getStatus())  {
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        } else {
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        }
    };

    switch(t.liveType()) {
    case Trace::LivedataType::Overwrite: ui->CLiveType->setCurrentIndex(0); break;
    case Trace::LivedataType::MaxHold: ui->CLiveType->setCurrentIndex(1); break;
    case Trace::LivedataType::MinHold: ui->CLiveType->setCurrentIndex(2); break;
    }

    switch(t.liveParameter()) {
    case Trace::LiveParameter::S11: ui->CLiveParam->setCurrentIndex(0); break;
    case Trace::LiveParameter::S12: ui->CLiveParam->setCurrentIndex(1); break;
    case Trace::LiveParameter::S21: ui->CLiveParam->setCurrentIndex(2); break;
    case Trace::LiveParameter::S22: ui->CLiveParam->setCurrentIndex(3); break;
    }

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
        Trace::LivedataType type;
        Trace::LiveParameter param;
        switch(ui->CLiveType->currentIndex()) {
        case 0: type = Trace::LivedataType::Overwrite; break;
        case 1: type = Trace::LivedataType::MaxHold; break;
        case 2: type = Trace::LivedataType::MinHold; break;
        }
        switch(ui->CLiveParam->currentIndex()) {
        case 0: param = Trace::LiveParameter::S11; break;
        case 1: param = Trace::LiveParameter::S12; break;
        case 2: param = Trace::LiveParameter::S21; break;
        case 3: param = Trace::LiveParameter::S22; break;
        }
        trace.fromLivedata(type, param);
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
