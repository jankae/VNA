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
    averages = 1;
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

    if (d.pointNum == avg.size()) {
        // add moving average entry
        deque<array<complex<double>, 4>> deque;
        avg.push_back(deque);
    }

    if (d.pointNum < avg.size()) {
        // can compute average
        // get correct queue
        auto deque = &avg[d.pointNum];
        // add newest sample to queue
        array<complex<double>, 4> sample = {S11, S12, S21, S22};
        deque->push_back(sample);
        if(deque->size() > averages) {
            deque->pop_front();
        }

        // calculate average
        complex<double> sum[4];
        for(auto s : *deque) {
            sum[0] += s[0];
            sum[1] += s[1];
            sum[2] += s[2];
            sum[3] += s[3];
        }
        S11 = sum[0] / (double) (deque->size());
        S12 = sum[1] / (double) (deque->size());
        S21 = sum[2] / (double) (deque->size());
        S22 = sum[3] / (double) (deque->size());
    }

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
    avg.clear();
}

void SParamTable::setAverages(unsigned int avg)
{
    averages = avg;
    this->avg.clear();
}

unsigned int SParamTable::getAcquiredAverages()
{
    if (avg.size() > 0) {
        return avg.back().size();
    } else {
        return 0;
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
