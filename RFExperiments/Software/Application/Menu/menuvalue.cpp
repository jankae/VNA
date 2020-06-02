#include "menuvalue.h"

#include <QVBoxLayout>
#include <math.h>
#include <sstream>
#include <iomanip>
#include "valueinput.h"
#include <QMouseEvent>
#include "unit.h"
#include <QDoubleValidator>

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
    lvalue = new QLineEdit(this);
    lvalue->setAlignment(Qt::AlignCenter);
    lvalue->installEventFilter(this);
    lvalue->setValidator(new QDoubleValidator);
    connect(lvalue, &QLineEdit::editingFinished, [this]() {
       parseNewValue(1.0);
    });
    layout->addWidget(lvalue);
    setValueQuiet(defaultValue);
    setIncrement(1);
    setLayout(layout);
}

void MenuValue::setValueQuiet(double value)
{
    this->value = value;
    lvalue->clear();
    lvalue->setPlaceholderText(Unit::ToString(value, unit, prefixes, precision));
    lvalue->clearFocus();
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
    lvalue->setFocus();
    //startInputDialog();
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

bool MenuValue::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        int key = static_cast<QKeyEvent *>(event)->key();
        if(key == Qt::Key_Escape) {
            setValueQuiet(value);
            lvalue->clearFocus();
            return true;
        }
        auto mod = static_cast<QKeyEvent *>(event)->modifiers();
        if (!(mod & Qt::ShiftModifier)) {
            key = tolower(key);
        }
        if(key <= 255 && prefixes.indexOf(key) >= 0) {
            // a valid prefix key was pressed
            parseNewValue(Unit::SIPrefixToFactor(key));
            return true;
        }
    } else if(event->type() == QEvent::FocusOut) {
        setValueQuiet(value);
    }
    return false;
}

void MenuValue::parseNewValue(double factor)
{
    double v = lvalue->text().toDouble() * factor;
    setValue(v);
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
