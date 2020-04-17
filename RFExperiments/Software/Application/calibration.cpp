#include "calibration.h"
#include <algorithm>

using namespace std;

Calibration::Calibration()
{
    // Creator vectors for measurements
    measurements[Measurement::Port1Open] = vector<Protocol::Datapoint>();
    measurements[Measurement::Port1Short] = vector<Protocol::Datapoint>();
    measurements[Measurement::Port1Load] = vector<Protocol::Datapoint>();
    measurements[Measurement::Port2Open] = vector<Protocol::Datapoint>();
    measurements[Measurement::Port2Short] = vector<Protocol::Datapoint>();
    measurements[Measurement::Port2Load] = vector<Protocol::Datapoint>();
    measurements[Measurement::Isolation] = vector<Protocol::Datapoint>();
    measurements[Measurement::Through] = vector<Protocol::Datapoint>();
}

void Calibration::addMeasurement(Calibration::Measurement type, Protocol::Datapoint d)
{
    measurements[type].push_back(d);
}

void Calibration::constructErrorTerms()
{
    bool isolation_measured = true;
    // sanity check measurements, all need to be of the same size with the same frequencies (except for isolation which may be empty)
    vector<uint64_t> freqs;
    for(auto m : measurements) {
        if(freqs.size() == 0) {
            // this is the first measurement, create frequency vector
            for(auto p : m.second) {
                freqs.push_back(p.frequency);
            }
        } else {
            // no isolation measurement is ok
            if(m.first == Measurement::Isolation && m.second.size() == 0) {
                isolation_measured = false;
                continue;
            }
            // compare with already assembled frequency vector
            if(m.second.size() != freqs.size()) {
                throw runtime_error("Calibration measurements of unequal size");
            }
            for(unsigned int i=0;i<freqs.size();i++) {
                if(m.second[i].frequency != freqs[i]) {
                    throw runtime_error("Calibration frequencies are not consistent");
                }
            }
        }
    }
    // If we get here the calibration measurements are all okay
    points.clear();
    for(unsigned int i = 0;i<freqs.size();i++) {
        Point p;
        p.frequency = freqs[i];
        // extract required complex reflection/transmission factors from datapoints
        auto S11_open = polar<double>(measurements[Measurement::Port1Open][i].S11Mag, measurements[Measurement::Port1Open][i].S11Phase);
        auto S11_short = polar<double>(measurements[Measurement::Port1Short][i].S11Mag, measurements[Measurement::Port1Short][i].S11Phase);
        auto S11_load = polar<double>(measurements[Measurement::Port1Load][i].S11Mag, measurements[Measurement::Port1Load][i].S11Phase);
        auto S22_open = polar<double>(measurements[Measurement::Port2Open][i].S22Mag, measurements[Measurement::Port2Open][i].S22Phase);
        auto S22_short = polar<double>(measurements[Measurement::Port2Short][i].S22Mag, measurements[Measurement::Port2Short][i].S22Phase);
        auto S22_load = polar<double>(measurements[Measurement::Port2Load][i].S22Mag, measurements[Measurement::Port2Load][i].S22Phase);
        auto S21_isolation = complex<double>(0,0);
        auto S12_isolation = complex<double>(0,0);
        if(isolation_measured) {
            S21_isolation = polar<double>(measurements[Measurement::Isolation][i].S21Mag, measurements[Measurement::Isolation][i].S21Phase);
            S12_isolation = polar<double>(measurements[Measurement::Isolation][i].S12Mag, measurements[Measurement::Isolation][i].S12Phase);
        }
        auto S11_through = polar<double>(measurements[Measurement::Through][i].S11Mag, measurements[Measurement::Through][i].S11Phase);
        auto S21_through = polar<double>(measurements[Measurement::Through][i].S21Mag, measurements[Measurement::Through][i].S21Phase);
        auto S22_through = polar<double>(measurements[Measurement::Through][i].S22Mag, measurements[Measurement::Through][i].S22Phase);
        auto S12_through = polar<double>(measurements[Measurement::Through][i].S12Mag, measurements[Measurement::Through][i].S12Phase);
        // Forward calibration
        // See page 19 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
        computeOSL(S11_open, S11_short, S11_load, p.fe00, p.fe11, p.fe10e01);
        p.fe30 = S21_isolation;
        p.fe22 = correctOSL(S11_through, p.fe00, p.fe11, p.fe10e01);
        p.fe10e32 = (S21_through - p.fe30)*(complex<double>(1,0) - p.fe11*p.fe22);
        // Reverse calibration
        computeOSL(S22_open, S22_short, S22_load, p.re33, p.re22, p.re23e32);
        p.re03 = S12_isolation;
        p.re11 = correctOSL(S22_through, p.re33, p.re22, p.re23e32);
        p.re23e01 = (S12_through - p.re03)*(complex<double>(1,0) - p.re22*p.re11);
        points.push_back(p);
    }
}

void Calibration::correctMeasurement(Protocol::Datapoint &d)
{
    // Convert measurements to complex variables
    auto S11m = polar<double>(d.S11Mag, d.S11Phase);
    auto S21m = polar<double>(d.S21Mag, d.S21Phase);
    auto S22m = polar<double>(d.S22Mag, d.S22Phase);
    auto S12m = polar<double>(d.S12Mag, d.S12Phase);

    // find correct entry
    auto p = lower_bound(points.begin(), points.end(), d.frequency, [](Point p, uint64_t freq) -> bool {
        return p.frequency < freq;
    });
    if(p->frequency != d.frequency) {
        throw runtime_error("No calibration point for current frequency");
    }
    // equations from page 20 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
    auto denom = (1.0 + (S11m - p->fe00) / p->fe10e01)+(1.0 + (S22m - p->re33) / p->re23e32)
            - (S21m - p->fe30) / p->fe10e32 * (S12m - p->re03) / p->re23e01 * p->fe22 * p->re11;
    auto S11 = ((S11m - p->fe00) / p->fe10e01 * (1.0 + (S22m - p->re33) / p->re23e32 * p->re22)
            - p->fe22 * (S21m - p->fe30) / p->fe10e32 * (S12m - p->re03) / p->re23e01) / denom;
    auto S21 = ((S21m - p->fe30) / p->fe10e32 * (1.0 + (S22m - p->re33) / p->re23e32 * (p->re22 - p->fe22))) / denom;
    auto S22 = ((S22m - p->re33) / p->re23e32 * (1.0 + (S11m - p->fe00) / p->fe10e01 * p->fe11)
            - p->re11 * (S21m - p->fe30) / p->fe10e32 * (S12m - p->re03) / p->re23e01) / denom;
    auto S12 = ((S12m - p->re03) / p->re23e01 * (1.0 + (S11m - p->fe00) / p->fe10e01 * (p->fe11 - p->re11))) / denom;

    // Convert back into magnitude and phase
    d.S11Mag = abs(S11);
    d.S11Phase = arg(S11);
    d.S21Mag = abs(S21);
    d.S21Phase = arg(S21);
    d.S22Mag = abs(S22);
    d.S22Phase = arg(S22);
    d.S12Mag = abs(S12);
    d.S12Phase = arg(S12);
}

void Calibration::computeOSL(std::complex<double> o_m, std::complex<double> s_m, std::complex<double> l_m, std::complex<double> &directivity, std::complex<double> &match, std::complex<double> &tracking)
{
    // equations from page 13 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
    // solved while assuming ideal o/s/l standards (Gamma_o=1.0, Gamma_s=-1.0, Gamma_l=0.0)
    directivity = l_m;
    match = (s_m - l_m * 2.0 + o_m) / (o_m - s_m);
    auto delta = (o_m * s_m * 2.0 - l_m * s_m - l_m * o_m) / (o_m - s_m);
    tracking = directivity * match - delta;
}

std::complex<double> Calibration::correctOSL(std::complex<double> measured, std::complex<double> directivity, std::complex<double> match, std::complex<double> tracking)
{
    return (measured - directivity) / (measured * match - directivity * match + tracking);
}


