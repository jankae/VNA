#ifndef MENUVALUE_H
#define MENUVALUE_H

#include "menuitem.h"
#include <QLabel>

class MenuValue : public MenuItem
{
    Q_OBJECT
public:
    MenuValue(QString name, double defaultValue = 0.0, QString unit = QString(), QString prefixes = " ", int precision = 0);

signals:
    void valueChanged(double value);
public slots:
    void setValue(double value);
    // same as setValue, except that no valueChanged signal is emitted
    void setValueQuiet(double value);
    void setIncrement(double inc);
    void userSelected() override;
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    void startInputDialog(QString initialInput = QString());
    double value;
    double increment;
    QLabel *lvalue;
    const QString unit, name, prefixes;
    const int precision;
};

#endif // MENUVALUE_H
