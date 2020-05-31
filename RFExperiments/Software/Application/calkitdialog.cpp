#include "calkitdialog.h"
#include "ui_calkitdialog.h"
#include <QPushButton>

#include <QDebug>
#include <QFileDialog>
#include <fstream>
#include <touchstone.h>

using namespace std;

CalkitDialog::CalkitDialog(Calkit &c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalkitDialog),
    open_ok(true),
    short_ok(true),
    load_ok(true),
    through_ok(true),
    editKit(c)
{
    ui->setupUi(this);

    auto FileChanged = [this](QString filename, QRadioButton *S11, QRadioButton *S22, QRadioButton *S33,
            QRadioButton *S44, QLineEdit *points,
            QLineEdit *lower, QLineEdit *upper, QLabel *status, bool *ok) {
        S11->setEnabled(false);
        S22->setEnabled(false);
        S33->setEnabled(false);
        S44->setEnabled(false);
        points->setText("");
        lower->setText("");
        upper->setText("");
        status->clear();
        try {
            auto t = Touchstone::fromFile(filename.toStdString());
            S11->setEnabled(t.ports() >= 1);
            S22->setEnabled(t.ports() >= 2);
            S33->setEnabled(t.ports() >= 3);
            S44->setEnabled(t.ports() >= 4);
            points->setText(QString::number(t.points()));
            lower->setText(QString::number(t.minFreq()));
            upper->setText(QString::number(t.maxFreq()));
            if(S11->group()->checkedId() == -1 || !S11->group()->checkedButton()->isEnabled()) {
                // no or invalid S parameter selected
                S11->setChecked(true);
            }
            *ok = true;
            if(open_ok && short_ok && load_ok && through_ok) {
                ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
                ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
            }
        } catch (const exception &e) {
            status->setText(e.what());
            *ok = false;
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
        }
    };

    // switch to correct stacked entries
    auto SetupStandardConnections = [this, FileChanged](QRadioButton *coefficients, QRadioButton *measurements,
            QStackedWidget *stacked, QRadioButton *S11, QRadioButton *S22, QRadioButton *S33,
            QRadioButton *S44, QLineEdit *file, QPushButton *browse, QLineEdit *points,
            QLineEdit *lower, QLineEdit *upper, QLabel *status, bool *ok) {
        auto g = S11->group();
        g->setId(S11, 0);
        g->setId(S22, 1);
        g->setId(S33, 2);
        g->setId(S44, 3);

        connect(coefficients, &QRadioButton::clicked, [=]() {
           stacked->setCurrentIndex(0);
           *ok = true;
           if(open_ok && short_ok && load_ok && through_ok) {
               ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
               ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
           }
        });
        connect(measurements, &QRadioButton::clicked, [=]() {
           stacked->setCurrentIndex(1);
           FileChanged(file->text(), S11, S22, S33, S44, points, lower, upper, status, ok);
        });
        connect(browse, &QPushButton::clicked, [=]() {
            auto filename = QFileDialog::getOpenFileName(nullptr, "Open measurement file", "", "Touchstone files (*.s1p *.s2p *.s3p *.s4p)", nullptr, QFileDialog::DontUseNativeDialog);
            if (filename.length() > 0) {
                file->setText(filename);
                FileChanged(filename, S11, S22, S33, S44, points, lower, upper, status, ok);
            }
        });
        connect(file, &QLineEdit::editingFinished, [=]() {
            FileChanged(file->text(), S11, S22, S33, S44, points, lower, upper, status, ok);
        });
    };

    SetupStandardConnections(ui->open_coefficients, ui->open_measurement, ui->open_stack, ui->open_S11,
                             ui->open_S22, ui->open_S33, ui->open_S44, ui->open_file, ui->open_browse,
                             ui->open_points, ui->open_lower, ui->open_upper, ui->open_status, &open_ok);

    SetupStandardConnections(ui->short_coefficients, ui->short_measurement, ui->short_stack, ui->short_S11,
                             ui->short_S22, ui->short_S33, ui->short_S44, ui->short_file, ui->short_browse,
                             ui->short_points, ui->short_lower, ui->short_upper, ui->short_status, &short_ok);

    SetupStandardConnections(ui->load_coefficients, ui->load_measurement, ui->load_stack, ui->load_S11,
                             ui->load_S22, ui->load_S33, ui->load_S44, ui->load_file, ui->load_browse,
                             ui->load_points, ui->load_lower, ui->load_upper, ui->load_status, &load_ok);

    SetupStandardConnections(ui->through_coefficients, ui->through_measurement, ui->through_stack, ui->through_S11,
                             ui->through_S22, ui->through_S33, ui->through_S44, ui->through_file, ui->through_browse,
                             ui->through_points, ui->through_lower, ui->through_upper, ui->through_status, &through_ok);

    ownKit = editKit;
    updateEntries();

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, [this]() {
        parseEntries();
        editKit = ownKit;
        delete this;
    });
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, [this]() {
        delete this;
    });
    connect(ui->buttonBox->button(QDialogButtonBox::Open), &QPushButton::clicked, [=](){
        auto filename = QFileDialog::getOpenFileName(this, "Open calibration kit coefficients", "", "Calibration kit files (*.calkit)", nullptr, QFileDialog::DontUseNativeDialog);
        if(filename.length() > 0) {
            ownKit = Calkit::fromFile(filename.toStdString());
            updateEntries();
        }
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked, [=](){
        auto filename = QFileDialog::getSaveFileName(this, "Save calibration kit coefficients", "", "Calibration kit files (*.calkit)", nullptr, QFileDialog::DontUseNativeDialog);
        if(filename.length() > 0) {
            parseEntries();
            ownKit.toFile(filename.toStdString());
        }
    });
}

CalkitDialog::~CalkitDialog()
{
    delete ui;
}

void CalkitDialog::parseEntries()
{

    // type
    ownKit.open_measurements = ui->open_measurement->isChecked();
    ownKit.short_measurements = ui->short_measurement->isChecked();
    ownKit.load_measurements = ui->load_measurement->isChecked();
    ownKit.through_measurements = ui->through_measurement->isChecked();

    // coefficients
    ownKit.open_Z0 = ui->open_Z0->text().toDouble();
    ownKit.open_delay = ui->open_delay->text().toDouble();
    ownKit.open_loss = ui->open_loss->text().toDouble();
    ownKit.open_C0 = ui->open_C0->text().toDouble();
    ownKit.open_C1 = ui->open_C1->text().toDouble();
    ownKit.open_C2 = ui->open_C2->text().toDouble();
    ownKit.open_C3 = ui->open_C3->text().toDouble();

    ownKit.short_Z0 = ui->short_Z0->text().toDouble();
    ownKit.short_delay = ui->short_delay->text().toDouble();
    ownKit.short_loss = ui->short_loss->text().toDouble();
    ownKit.short_L0 = ui->short_L0->text().toDouble();
    ownKit.short_L1 = ui->short_L1->text().toDouble();
    ownKit.short_L2 = ui->short_L2->text().toDouble();
    ownKit.short_L3 = ui->short_L3->text().toDouble();

    ownKit.load_Z0 = ui->load_Z0->text().toDouble();

    ownKit.through_Z0 = ui->through_Z0->text().toDouble();
    ownKit.through_delay = ui->through_delay->text().toDouble();
    ownKit.through_loss = ui->through_loss->text().toDouble();

    // file
    ownKit.open_file = ui->open_file->text().toStdString();
    ownKit.short_file = ui->short_file->text().toStdString();
    ownKit.load_file = ui->load_file->text().toStdString();
    ownKit.through_file = ui->through_file->text().toStdString();

    ownKit.open_Sparam = ui->openSparam->checkedId();
    ownKit.short_Sparam = ui->shortSparam->checkedId();
    ownKit.load_Sparam = ui->loadSparam->checkedId();
    ownKit.through_Sparam = ui->throughSparam->checkedId();
}

void CalkitDialog::updateEntries()
{
    // Coefficients
    ui->open_Z0->setText(QString::number(ownKit.open_Z0));
    ui->open_delay->setText(QString::number(ownKit.open_delay));
    ui->open_loss->setText(QString::number(ownKit.open_loss));
    ui->open_C0->setText(QString::number(ownKit.open_C0));
    ui->open_C1->setText(QString::number(ownKit.open_C1));
    ui->open_C2->setText(QString::number(ownKit.open_C2));
    ui->open_C3->setText(QString::number(ownKit.open_C3));

    ui->short_Z0->setText(QString::number(ownKit.short_Z0));
    ui->short_delay->setText(QString::number(ownKit.short_delay));
    ui->short_loss->setText(QString::number(ownKit.short_loss));
    ui->short_L0->setText(QString::number(ownKit.short_L0));
    ui->short_L1->setText(QString::number(ownKit.short_L1));
    ui->short_L2->setText(QString::number(ownKit.short_L2));
    ui->short_L3->setText(QString::number(ownKit.short_L3));

    ui->load_Z0->setText(QString::number(ownKit.load_Z0));

    ui->through_Z0->setText(QString::number(ownKit.through_Z0));
    ui->through_delay->setText(QString::number(ownKit.through_delay));
    ui->through_loss->setText(QString::number(ownKit.through_loss));

    // Measurements
    ui->open_file->setText(QString::fromStdString(ownKit.open_file));
    switch(ownKit.open_Sparam) {
        case 0: ui->open_S11->click(); break;
        case 1: ui->open_S22->click(); break;
        case 2: ui->open_S33->click(); break;
        case 3: ui->open_S44->click(); break;
    }

    ui->short_file->setText(QString::fromStdString(ownKit.short_file));
    switch(ownKit.short_Sparam) {
        case 0: ui->short_S11->click(); break;
        case 1: ui->short_S22->click(); break;
        case 2: ui->short_S33->click(); break;
        case 3: ui->short_S44->click(); break;
    }

    ui->load_file->setText(QString::fromStdString(ownKit.load_file));
    switch(ownKit.load_Sparam) {
        case 0: ui->load_S11->click(); break;
        case 1: ui->load_S22->click(); break;
        case 2: ui->load_S33->click(); break;
        case 3: ui->load_S44->click(); break;
    }

    ui->through_file->setText(QString::fromStdString(ownKit.through_file));
    switch(ownKit.through_Sparam) {
        case 0: ui->through_S11->click(); break;
        case 1: ui->through_S22->click(); break;
        case 2: ui->through_S33->click(); break;
        case 3: ui->through_S44->click(); break;
    }

    // Type
    if (ownKit.open_measurements) {
        ui->open_measurement->click();
    } else {
        ui->open_coefficients->click();
    }

    if (ownKit.short_measurements) {
        ui->short_measurement->click();
    } else {
        ui->short_coefficients->click();
    }

    if (ownKit.load_measurements) {
        ui->load_measurement->click();
    } else {
        ui->load_coefficients->click();
    }

    if (ownKit.through_measurements) {
        ui->through_measurement->click();
    } else {
        ui->through_coefficients->click();
    }
}
