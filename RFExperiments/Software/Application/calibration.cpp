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

void Calibration::clearMeasurements()
{
    for(auto m : measurements) {
        m.second.clear();
    }
}

void Calibration::addMeasurement(Calibration::Measurement type, Protocol::Datapoint d)
{
    measurements[type].push_back(d);
}

void Calibration::construct12TermPoints()
{
    std::vector<Measurement> requiredMeasurements;
    requiredMeasurements.push_back(Measurement::Port1Open);
    requiredMeasurements.push_back(Measurement::Port1Short);
    requiredMeasurements.push_back(Measurement::Port1Load);
    requiredMeasurements.push_back(Measurement::Port2Open);
    requiredMeasurements.push_back(Measurement::Port2Short);
    requiredMeasurements.push_back(Measurement::Port2Load);
    requiredMeasurements.push_back(Measurement::Through);
    if(!SanityCheckSamples(requiredMeasurements)) {
        throw runtime_error("Missing/wrong calibration measurement");
    }
    requiredMeasurements.push_back(Measurement::Isolation);
    bool isolation_measured = SanityCheckSamples(requiredMeasurements);

    // If we get here the calibration measurements are all okay
    points.clear();
    for(unsigned int i = 0;i<measurements[Measurement::Port1Open].size();i++) {
        Point p;
        p.frequency = measurements[Measurement::Port1Open][i].frequency;
        // extract required complex reflection/transmission factors from datapoints
        auto S11_open = complex<double>(measurements[Measurement::Port1Open][i].real_S11, measurements[Measurement::Port1Open][i].imag_S11);
        auto S11_short = complex<double>(measurements[Measurement::Port1Short][i].real_S11, measurements[Measurement::Port1Short][i].imag_S11);
        auto S11_load = complex<double>(measurements[Measurement::Port1Load][i].real_S11, measurements[Measurement::Port1Load][i].imag_S11);
        auto S22_open = complex<double>(measurements[Measurement::Port2Open][i].real_S22, measurements[Measurement::Port2Open][i].imag_S22);
        auto S22_short = complex<double>(measurements[Measurement::Port2Short][i].real_S22, measurements[Measurement::Port2Short][i].imag_S22);
        auto S22_load = complex<double>(measurements[Measurement::Port2Load][i].real_S22, measurements[Measurement::Port2Load][i].imag_S22);
        auto S21_isolation = complex<double>(0,0);
        auto S12_isolation = complex<double>(0,0);
        if(isolation_measured) {
            S21_isolation = complex<double>(measurements[Measurement::Isolation][i].real_S21, measurements[Measurement::Isolation][i].imag_S21);
            S12_isolation = complex<double>(measurements[Measurement::Isolation][i].real_S12, measurements[Measurement::Isolation][i].imag_S12);
        }
        auto S11_through = complex<double>(measurements[Measurement::Through][i].real_S11, measurements[Measurement::Through][i].imag_S11);
        auto S21_through = complex<double>(measurements[Measurement::Through][i].real_S21, measurements[Measurement::Through][i].imag_S21);
        auto S22_through = complex<double>(measurements[Measurement::Through][i].real_S22, measurements[Measurement::Through][i].imag_S22);
        auto S12_through = complex<double>(measurements[Measurement::Through][i].real_S12, measurements[Measurement::Through][i].imag_S12);
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

void Calibration::constructPort1OSL()
{
    std::vector<Measurement> requiredMeasurements;
    requiredMeasurements.push_back(Measurement::Port1Open);
    requiredMeasurements.push_back(Measurement::Port1Short);
    requiredMeasurements.push_back(Measurement::Port1Load);
    if(!SanityCheckSamples(requiredMeasurements)) {
        throw runtime_error("Missing/wrong calibration measurement");
    }

    // If we get here the calibration measurements are all okay
    points.clear();
    for(unsigned int i = 0;i<measurements[Measurement::Port1Open].size();i++) {
        Point p;
        p.frequency = measurements[Measurement::Port1Open][i].frequency;
        // extract required complex reflection/transmission factors from datapoints
        auto S11_open = complex<double>(measurements[Measurement::Port1Open][i].real_S11, measurements[Measurement::Port1Open][i].imag_S11);
        auto S11_short = complex<double>(measurements[Measurement::Port1Short][i].real_S11, measurements[Measurement::Port1Short][i].imag_S11);
        auto S11_load = complex<double>(measurements[Measurement::Port1Load][i].real_S11, measurements[Measurement::Port1Load][i].imag_S11);
        // OSL port1
        // See page 19 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
        computeOSL(S11_open, S11_short, S11_load, p.fe00, p.fe11, p.fe10e01);
        // All other calibration coefficients to ideal values
        p.fe30 = 0.0;
        p.fe22 = 0.0;
        p.fe10e32 = 1.0;
        p.re33 = 0.0;
        p.re22 = 0.0;
        p.re23e32 = 1.0;
        p.re03 = 0.0;
        p.re11 = 0.0;
        p.re23e01 = 1.0;
        points.push_back(p);
    }
}

void Calibration::constructPort2OSL()
{
    std::vector<Measurement> requiredMeasurements;
    requiredMeasurements.push_back(Measurement::Port2Open);
    requiredMeasurements.push_back(Measurement::Port2Short);
    requiredMeasurements.push_back(Measurement::Port2Load);
    if(!SanityCheckSamples(requiredMeasurements)) {
        throw runtime_error("Missing/wrong calibration measurement");
    }

    // If we get here the calibration measurements are all okay
    points.clear();
    for(unsigned int i = 0;i<measurements[Measurement::Port2Open].size();i++) {
        Point p;
        p.frequency = measurements[Measurement::Port2Open][i].frequency;
        // extract required complex reflection/transmission factors from datapoints
        auto S22_open = complex<double>(measurements[Measurement::Port2Open][i].real_S11, measurements[Measurement::Port2Open][i].imag_S11);
        auto S22_short = complex<double>(measurements[Measurement::Port2Short][i].real_S11, measurements[Measurement::Port2Short][i].imag_S11);
        auto S22_load = complex<double>(measurements[Measurement::Port2Load][i].real_S11, measurements[Measurement::Port2Load][i].imag_S11);
        // OSL port2
        // See page 19 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
        computeOSL(S22_open, S22_short, S22_load, p.re33, p.re22, p.re23e32);
        // All other calibration coefficients to ideal values
        p.fe30 = 0.0;
        p.fe22 = 0.0;
        p.fe10e32 = 1.0;
        p.fe00 = 0.0;
        p.fe11 = 0.0;
        p.fe10e01 = 1.0;
        p.re03 = 0.0;
        p.re11 = 0.0;
        p.re23e01 = 1.0;
        points.push_back(p);
    }
}

void Calibration::correctMeasurement(Protocol::Datapoint &d)
{
    // Convert measurements to complex variables
    auto S11m = complex<double>(d.real_S11, d.imag_S11);
    auto S21m = complex<double>(d.real_S21, d.imag_S21);
    auto S22m = complex<double>(d.real_S22, d.imag_S22);
    auto S12m = complex<double>(d.real_S12, d.imag_S12);

    // find correct entry
    auto p = lower_bound(points.begin(), points.end(), d.frequency, [](Point p, uint64_t freq) -> bool {
        return p.frequency < freq;
    });
    if(p->frequency != d.frequency) {
        throw runtime_error("No calibration point for current frequency");
    }
    // equations from page 20 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
    auto denom = (1.0 + (S11m - p->fe00) / p->fe10e01 * p->fe11) * (1.0 + (S22m - p->re33) / p->re23e32 * p->re22)
            - (S21m - p->fe30) / p->fe10e32 * (S12m - p->re03) / p->re23e01 * p->fe22 * p->re11;
    auto S11 = ((S11m - p->fe00) / p->fe10e01 * (1.0 + (S22m - p->re33) / p->re23e32 * p->re22)
            - p->fe22 * (S21m - p->fe30) / p->fe10e32 * (S12m - p->re03) / p->re23e01) / denom;
    auto S21 = ((S21m - p->fe30) / p->fe10e32 * (1.0 + (S22m - p->re33) / p->re23e32 * (p->re22 - p->fe22))) / denom;
    auto S22 = ((S22m - p->re33) / p->re23e32 * (1.0 + (S11m - p->fe00) / p->fe10e01 * p->fe11)
            - p->re11 * (S21m - p->fe30) / p->fe10e32 * (S12m - p->re03) / p->re23e01) / denom;
    auto S12 = ((S12m - p->re03) / p->re23e01 * (1.0 + (S11m - p->fe00) / p->fe10e01 * (p->fe11 - p->re11))) / denom;

    S22 = (S11m - p->fe00) / (S11m * p->fe11 - (p->fe00 * p->fe11 - p->fe10e01));

    d.real_S11 = S11.real();
    d.imag_S11 = S11.imag();
    d.real_S12 = S12.real();
    d.imag_S12 = S12.imag();
    d.real_S21 = S21.real();
    d.imag_S21 = S21.imag();
    d.real_S22 = S22.real();
    d.imag_S22 = S22.imag();
}

bool Calibration::SanityCheckSamples(std::vector<Calibration::Measurement> &requiredMeasurements)
{
    // sanity check measurements, all need to be of the same size with the same frequencies (except for isolation which may be empty)
    vector<uint64_t> freqs;
    for(auto type : requiredMeasurements) {
        auto m = measurements[type];
        if(freqs.size() == 0) {
            // this is the first measurement, create frequency vector
            for(auto p : m) {
                freqs.push_back(p.frequency);
            }
        } else {
            // compare with already assembled frequency vector
            if(m.size() != freqs.size()) {
                return false;
            }
            for(unsigned int i=0;i<freqs.size();i++) {
                if(m[i].frequency != freqs[i]) {
                    return false;
                }
            }
        }
    }
    return true;
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


