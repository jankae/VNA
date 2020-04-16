#include "sparamtable.h"
#include <math.h>
#include <sparam.h>

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
    auto S11 = SParam(10.0 * log10(d.S11Mag), d.S11Phase * 180.0 / M_PI);
    auto S12 = SParam(10.0 * log10(d.S12Mag), d.S12Phase * 180.0 / M_PI);
    auto S21 = SParam(10.0 * log10(d.S21Mag), d.S21Phase * 180.0 / M_PI);
    auto S22 = SParam(10.0 * log10(d.S22Mag), d.S22Phase * 180.0 / M_PI);
    params[Frequency][d.pointNum] = d.frequency;
    params[S11_db][d.pointNum] = S11.db;
    params[S11_phase][d.pointNum] = S11.phase;
    params[S12_db][d.pointNum] = S12.db;
    params[S12_phase][d.pointNum] = S12.phase;
    params[S21_db][d.pointNum] = S21.db;
    params[S21_phase][d.pointNum] = S21.phase;
    params[S22_db][d.pointNum] = S22.db;
    params[S22_phase][d.pointNum] = S22.phase;
    auto ImpedanceS11 = S11.ReflectionToImpedance();
    auto ImpedanceS22 = S22.ReflectionToImpedance();
    params[S11_ImpedanceReal][d.pointNum] = ImpedanceS11.real();
    params[S11_ImpedanceImag][d.pointNum] = ImpedanceS11.imag();
    params[S22_ImpedanceReal][d.pointNum] = ImpedanceS22.real();
    params[S22_ImpedanceImag][d.pointNum] = ImpedanceS22.imag();
}

void SParamTable::clearResults()
{
    // TODO set to Nan?
}

double *SParamTable::ParamArray(int parameter)
{
    if(parameter < NumParams) {
        return params[parameter];
    } else {
        throw std::runtime_error("Invalid parameter requested");
    }
}
