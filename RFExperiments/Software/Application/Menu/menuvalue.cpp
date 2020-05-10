#include "menuvalue.h"

#include <QVBoxLayout>
#include <math.h>
#include <sstream>
#include <iomanip>
#include "valueinput.h"
#include <QMouseEvent>
#include "unit.h"

using namespace std;

MenuValue::MenuValue(QString name, double defaultValue, QString unit, QString prefixes, int precision)
    : unit(unit), name(name), prefixes(prefixes), precision(precision)
{
    if(prefixes.indexOf(' ') < 0) {
        throw runtime_error("Prefix string must contain space");
    }
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
    lvalue->setText(Unit::ToString(value, unit, prefixes, precision));
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
    int unityIndex = prefixes.indexOf(' ');
    for(int i=0;i<prefixes.length();i++) {
        QString unitname = prefixes[i];
        unitname.append(unit);
        double factor = pow(1000.0, i - unityIndex);
        v.push_back(ValueInput::Unit(unitname, factor));
    }
    auto input = new ValueInput(v, name, initialInput);
    connect(input, &ValueInput::ValueChanged, this, &MenuValue::setValue);
}
