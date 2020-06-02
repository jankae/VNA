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

double Unit::SIPrefixToFactor(char prefix)
{
    switch(prefix) {
    case 'f': return 1e-15; break;
    case 'p': return 1e-12; break;
    case 'n': return 1e-9; break;
    case 'u': return 1e-6; break;
    case 'm': return 1e-3; break;
    case ' ': return 1e0; break;
    case 'k': return 1e3; break;
    case 'M': return 1e6; break;
    case 'G': return 1e9; break;
    case 'T': return 1e12; break;
    case 'P': return 1e15; break;
    default: return 0; break;
    }
}
