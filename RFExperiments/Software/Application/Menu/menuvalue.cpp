#include "menuvalue.h"

#include <QVBoxLayout>
#include <math.h>
#include <sstream>
#include <iomanip>
#include "valueinput.h"
#include <QMouseEvent>

using namespace std;

MenuValue::MenuValue(QString name, double defaultValue, QString unit)
    : unit(unit), name(name)
{
    auto layout = new QVBoxLayout;
    auto label = new QLabel(name, this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    lvalue = new QLabel(this);
    lvalue->setAlignment(Qt::AlignCenter);
    layout->addWidget(lvalue);
    setValueQuiet(defaultValue);
    setIncrement(1);
    setLayout(layout);
}

void MenuValue::setValueQuiet(double value)
{
    this->value = value;
    // change label text
    QString sValue;
    if(value < 0) {
        sValue.append('-');
        value = -value;
    } else if(value == 0.0) {
        sValue.append('0');
    } else {
        constexpr char prefixes[] = {
            'f', 'p', 'n', 'u', 'm', ' ', 'k', 'M', 'G', 'T', 'P'
        };
        int preDotDigits = log10(value) + 1;
        int prefixIndex = 5;
        while(preDotDigits > 3) {
            value /= 1000.0;
            preDotDigits -= 3;
            prefixIndex++;
        }
        while(preDotDigits<=0) {
            value *= 1000.0;
            preDotDigits += 3;
            prefixIndex--;
        }
        stringstream ss;
        ss << std::fixed << std::setprecision(6) << value;
        sValue.append(QString::fromStdString(ss.str()));
        sValue.append(prefixes[prefixIndex]);
    }
    sValue.append(unit);
    lvalue->setText(sValue);
}

void MenuValue::setValue(double value)
{
    setValueQuiet(value);
    emit valueChanged(this->value);
}

void MenuValue::setIncrement(double inc)
{
    increment = inc;
}

void MenuValue::userSelected()
{
    startInputDialog();
}

void MenuValue::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if((key >= '0' && key <= '9') || key == '.' || key == ',' || key == '-') {
        startInputDialog(QString(key));
        event->accept();
    } else {
        event->ignore();
    }
}

void MenuValue::startInputDialog(QString initialInput)
{
    vector<ValueInput::Unit> v;
    v.push_back(ValueInput::Unit("GHz", 1000000000.0));
    auto input = new ValueInput(v, name, initialInput);
    connect(input, &ValueInput::ValueChanged, this, &MenuValue::setValue);
}
