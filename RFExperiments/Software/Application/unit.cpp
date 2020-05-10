#include "unit.h"
#include <math.h>
#include <sstream>
#include <iomanip>

using namespace std;

QString Unit::ToString(double value, QString unit, QString prefixes, int precision)
{
    // change label text
    QString sValue;
    if(value < 0) {
        sValue.append('-');
        value = -value;
    } else if(value == 0.0) {
        sValue.append('0');
    } else {
        int preDotDigits = log10(value) + 1;
        int prefixIndex = prefixes.indexOf(' ');
        while(preDotDigits > 3 && prefixIndex < prefixes.length() - 1) {
            value /= 1000.0;
            preDotDigits -= 3;
            prefixIndex++;
        }
        while(preDotDigits<=0 && prefixIndex > 0) {
            value *= 1000.0;
            preDotDigits += 3;
            prefixIndex--;
        }
        stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        sValue.append(QString::fromStdString(ss.str()));
        sValue.append(prefixes[prefixIndex]);
    }
    sValue.append(unit);
    return sValue;
}
