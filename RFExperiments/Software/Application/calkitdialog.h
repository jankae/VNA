#ifndef CALKITDIALOG_H
#define CALKITDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <iostream>
#include <iomanip>
#include "calkit.h"

namespace Ui {
class CalkitDialog;
}

class CalkitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalkitDialog(Calkit &c, QWidget *parent = nullptr);
    ~CalkitDialog();

private:
    void parseEntries();
    void updateEntries();
    Ui::CalkitDialog *ui;

    Calkit ownCoefficients;
    Calkit &editCoefficients;
};

#endif // CALKITDIALOG_H
