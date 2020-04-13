#ifndef SPARAM_H
#define SPARAM_H

#include <complex>

class SParam
{
public:
    SParam(double db, double phase)
        : db(db), phase(phase){};

    std::complex<double> toComplex() {
        return std::complex<double>(db * cos(phase*M_PI/180.0), db * sin(phase*M_PI/180.0));
    }

    std::complex<double> ReflectionToImpedance() {
        constexpr auto cpl_one = std::complex<double>(1.0, 0.0);
        auto cpl = toComplex();
        // convert from db to factor
        cpl *= pow(10.0, db/10.0);
        // convert from ratio to load impedance
        return (cpl_one + cpl) / (cpl_one - cpl) * 50.0;
    }

    double db;
    double phase;
};

#endif // SPARAM_H
