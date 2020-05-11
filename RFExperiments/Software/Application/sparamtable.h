#ifndef SPARAMTABLE_H
#define SPARAMTABLE_H

#include <complex>
#include <vector>
#include <deque>
#include "device.h"

class SParamTable
{
public:  
    SParamTable(int maxPoints);

    void addVNAResult(Protocol::Datapoint d);
    void clearResults();
    void setAverages(unsigned int avg);
    unsigned int getAcquiredAverages();

    enum Parameter {
        Frequency = 0,
        S11_db = 1,
        S11_phase = 2,
        S12_db = 3,
        S12_phase = 4,
        S21_db = 5,
        S21_phase = 6,
        S22_db = 7,
        S22_phase = 8,
        S11_ImpedanceReal = 9,
        S11_ImpedanceImag = 10,
        S22_ImpedanceReal = 11,
        S22_ImpedanceImag = 12,
        // always has to be last
        NumParams
    };
    double *ParamArray(int parameter);

private:
    std::vector<double*> params;
    std::vector<std::deque<std::array<std::complex<double>, 4>>> avg;
    int maxPoints;
    unsigned int averages;
};

#endif // SPARAMTABLE_H
