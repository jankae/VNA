#include "sparamtable.h"
#include <math.h>
#include <sparam.h>
#include <complex>

using namespace std;

SParamTable::SParamTable(int maxPoints)
    : maxPoints(maxPoints)
{
    // allocate arrays
    for(int i=0;i<NumParams;i++) {
        params.push_back(new double[maxPoints]);
    }
}

void SParamTable::addVNAResult(Protocol::Datapoint d)
{
    if(d.pointNum >= maxPoints) {
        return;
    }
    auto S11 = complex<double>(d.real_S11, d.imag_S11);
    auto S12 = complex<double>(d.real_S12, d.imag_S12);
    auto S21 = complex<double>(d.real_S21, d.imag_S21);
    auto S22 = complex<double>(d.real_S22, d.imag_S22);
    params[Frequency][d.pointNum] = d.frequency;
    params[S11_db][d.pointNum] =  20 * log10(abs(S11));
    params[S11_phase][d.pointNum] = arg(S11) * 180 / M_PI;
    params[S12_db][d.pointNum] = 20 * log10(abs(S12));
    params[S12_phase][d.pointNum] = arg(S12) * 180 / M_PI;
    params[S21_db][d.pointNum] = 20 * log10(abs(S21));
    params[S21_phase][d.pointNum] = arg(S21) * 180 / M_PI;
    params[S22_db][d.pointNum] = 20 * log10(abs(S22));
    params[S22_phase][d.pointNum] = arg(S22) * 180 / M_PI;
    auto ImpedanceS11 = (1.0 + S11) / (1.0 - S11) * 50.0;
    auto ImpedanceS22 = (1.0 + S22) / (1.0 - S22) * 50.0;
    params[S11_ImpedanceReal][d.pointNum] = ImpedanceS11.real();
    params[S11_ImpedanceImag][d.pointNum] = ImpedanceS11.imag();
    params[S22_ImpedanceReal][d.pointNum] = ImpedanceS22.real();
    params[S22_ImpedanceImag][d.pointNum] = ImpedanceS22.imag();
}

void SParamTable::clearResults()
{
    for(auto p : params) {
        for(int i=0;i<maxPoints;i++) {
            p[i] = nan("");
        }
    }
}

double *SParamTable::ParamArray(int parameter)
{
    if(parameter < NumParams) {
        return params[parameter];
    } else {
        throw std::runtime_error("Invalid parameter requested");
    }
}
