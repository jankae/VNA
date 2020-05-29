#include "calkitdialog.h"
#include "ui_calkitdialog.h"
#include <QPushButton>

#include <QDebug>
#include <QFileDialog>
#include <fstream>

using namespace std;

CalkitDialog::CalkitDialog(Calkit &coefficients, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalkitDialog),
    editCoefficients(coefficients)
{
    ui->setupUi(this);

    ownCoefficients = editCoefficients;
    updateEntries();

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, [this]() {
        parseEntries();
        editCoefficients = ownCoefficients;
        delete this;
    });
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, [this]() {
        delete this;
    });
    connect(ui->buttonBox->button(QDialogButtonBox::Open), &QPushButton::clicked, [=](){
        auto filename = QFileDialog::getOpenFileName(this, "Open calibration kit coefficients", "", "Calibration kit files (*.calkit)", nullptr, QFileDialog::DontUseNativeDialog);
        if(filename.length() > 0) {
            ownCoefficients = Calkit::fromFile(filename.toStdString());
            updateEntries();
        }
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked, [=](){
        auto filename = QFileDialog::getSaveFileName(this, "Save calibration kit coefficients", "", "Calibration kit files (*.calkit)", nullptr, QFileDialog::DontUseNativeDialog);
        if(filename.length() > 0) {
            parseEntries();
            ownCoefficients.toFile(filename.toStdString());
        }
    });
}

CalkitDialog::~CalkitDialog()
{
    delete ui;
}

void CalkitDialog::parseEntries()
{
    ownCoefficients.open_Z0 = ui->open_Z0->text().toDouble();
    ownCoefficients.open_delay = ui->open_delay->text().toDouble();
    ownCoefficients.open_loss = ui->open_loss->text().toDouble();
    ownCoefficients.open_C0 = ui->open_C0->text().toDouble();
    ownCoefficients.open_C1 = ui->open_C1->text().toDouble();
    ownCoefficients.open_C2 = ui->open_C2->text().toDouble();
    ownCoefficients.open_C3 = ui->open_C3->text().toDouble();

    ownCoefficients.short_Z0 = ui->short_Z0->text().toDouble();
    ownCoefficients.short_delay = ui->short_delay->text().toDouble();
    ownCoefficients.short_loss = ui->short_loss->text().toDouble();
    ownCoefficients.short_L0 = ui->short_L0->text().toDouble();
    ownCoefficients.short_L1 = ui->short_L1->text().toDouble();
    ownCoefficients.short_L2 = ui->short_L2->text().toDouble();
    ownCoefficients.short_L3 = ui->short_L3->text().toDouble();

    ownCoefficients.load_Z0 = ui->load_Z0->text().toDouble();

    ownCoefficients.through_Z0 = ui->through_Z0->text().toDouble();
    ownCoefficients.through_delay = ui->through_delay->text().toDouble();
    ownCoefficients.through_loss = ui->through_loss->text().toDouble();
}

void CalkitDialog::updateEntries()
{
    ui->open_Z0->setText(QString::number(ownCoefficients.open_Z0));
    ui->open_delay->setText(QString::number(ownCoefficients.open_delay));
    ui->open_loss->setText(QString::number(ownCoefficients.open_loss));
    ui->open_C0->setText(QString::number(ownCoefficients.open_C0));
    ui->open_C1->setText(QString::number(ownCoefficients.open_C1));
    ui->open_C2->setText(QString::number(ownCoefficients.open_C2));
    ui->open_C3->setText(QString::number(ownCoefficients.open_C3));

    ui->short_Z0->setText(QString::number(ownCoefficients.short_Z0));
    ui->short_delay->setText(QString::number(ownCoefficients.short_delay));
    ui->short_loss->setText(QString::number(ownCoefficients.short_loss));
    ui->short_L0->setText(QString::number(ownCoefficients.short_L0));
    ui->short_L1->setText(QString::number(ownCoefficients.short_L1));
    ui->short_L2->setText(QString::number(ownCoefficients.short_L2));
    ui->short_L3->setText(QString::number(ownCoefficients.short_L3));

    ui->load_Z0->setText(QString::number(ownCoefficients.load_Z0));

    ui->through_Z0->setText(QString::number(ownCoefficients.through_Z0));
    ui->through_delay->setText(QString::number(ownCoefficients.through_delay));
    ui->through_loss->setText(QString::number(ownCoefficients.through_loss));
}
