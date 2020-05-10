#ifndef UNIT_H
#define UNIT_H

#include <QString>

class Unit
{
public:
    static QString ToString(double value, QString unit = QString(), QString prefixes = " ", int precision = 6);
};

#endif // UNIT_H
