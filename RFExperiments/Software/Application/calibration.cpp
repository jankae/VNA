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

void Calibration::clearMeasurement(Calibration::Measurement type)
{
    measurements[type].clear();
}

void Calibration::addMeasurement(Calibration::Measurement type, Protocol::Datapoint &d)
{
    measurements[type].push_back(d);
}

bool Calibration::calculationPossible(Calibration::Type type)
{
    std::vector<Measurement> requiredMeasurements;
    switch(type) {
    case Type::Port1SOL:
        requiredMeasurements.push_back(Measurement::Port1Open);
        requiredMeasurements.push_back(Measurement::Port1Short);
        requiredMeasurements.push_back(Measurement::Port1Load);
        break;
    case Type::Port2SOL:
        requiredMeasurements.push_back(Measurement::Port2Open);
        requiredMeasurements.push_back(Measurement::Port2Short);
        requiredMeasurements.push_back(Measurement::Port2Load);
        break;
    case Type::FullSOLT:
        requiredMeasurements.push_back(Measurement::Port1Open);
        requiredMeasurements.push_back(Measurement::Port1Short);
        requiredMeasurements.push_back(Measurement::Port1Load);
        requiredMeasurements.push_back(Measurement::Port2Open);
        requiredMeasurements.push_back(Measurement::Port2Short);
        requiredMeasurements.push_back(Measurement::Port2Load);
        requiredMeasurements.push_back(Measurement::Through);
        break;
    }
    return SanityCheckSamples(requiredMeasurements);
}

bool Calibration::constructErrorTerms(Calibration::Type type, Calkit c)
{
    if(!calculationPossible(type)) {
        return false;
    }
    switch(type) {
    case Type::Port1SOL:
        constructPort1SOL(c);
        break;
    case Type::Port2SOL:
        constructPort2SOL(c);
        break;
    case Type::FullSOLT:
        construct12TermPoints(c);
        break;
    }
    return true;
}

void Calibration::construct12TermPoints(Calkit c)
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

        auto actual = c.toReflection(p.frequency);
        // Forward calibration
        computeSOL(S11_short, S11_open, S11_load, p.fe00, p.fe11, p.fe10e01, actual.Open, actual.Short, actual.Load);
        p.fe30 = S21_isolation;
        // See page 17 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
        // Formulas for S11M and S21M solved for e22 and e10e32
        auto deltaS = actual.ThroughS11*actual.ThroughS22 - actual.ThroughS21 * actual.ThroughS12;
        p.fe22 = ((S11_through - p.fe00)*(1.0 - p.fe11 * actual.ThroughS11)-actual.ThroughS11*p.fe10e01)
                / ((S11_through - p.fe00)*(actual.ThroughS22-p.fe11*deltaS)-deltaS*p.fe10e01);
        p.fe10e32 = (S21_through - p.fe30)*(1.0 - p.fe11*actual.ThroughS11 - p.fe22*actual.ThroughS22 + p.fe11*p.fe22*deltaS) / actual.ThroughS21;
        // Reverse calibration
        computeSOL(S22_short, S22_open, S22_load, p.re33, p.re22, p.re23e32, actual.Open, actual.Short, actual.Load);
        p.re03 = S12_isolation;
        p.re11 = ((S22_through - p.re33)*(1.0 - p.re22 * actual.ThroughS22)-actual.ThroughS22*p.re23e32)
                / ((S22_through - p.re33)*(actual.ThroughS11-p.re22*deltaS)-deltaS*p.re23e32);
        p.re23e01 = (S12_through - p.re03)*(1.0 - p.re11*actual.ThroughS11 - p.re22*actual.ThroughS22 + p.re11*p.re22*deltaS) / actual.ThroughS12;
        points.push_back(p);
    }
}

void Calibration::constructPort1SOL(Calkit c)
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
        auto actual = c.toReflection(p.frequency);
        // See page 19 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
        computeSOL(S11_short, S11_open, S11_load, p.fe00, p.fe11, p.fe10e01, actual.Open, actual.Short, actual.Load);
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

void Calibration::constructPort2SOL(Calkit c)
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
        auto S22_open = complex<double>(measurements[Measurement::Port2Open][i].real_S22, measurements[Measurement::Port2Open][i].imag_S22);
        auto S22_short = complex<double>(measurements[Measurement::Port2Short][i].real_S22, measurements[Measurement::Port2Short][i].imag_S22);
        auto S22_load = complex<double>(measurements[Measurement::Port2Load][i].real_S22, measurements[Measurement::Port2Load][i].imag_S22);
        // OSL port2
        auto actual = c.toReflection(p.frequency);
        // See page 19 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
        computeSOL(S22_short, S22_open, S22_load, p.re33, p.re22, p.re23e32, actual.Open, actual.Short, actual.Load);
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
    auto p = getCalibrationPoint(d);

    // equations from page 20 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
    auto denom = (1.0 + (S11m - p.fe00) / p.fe10e01 * p.fe11) * (1.0 + (S22m - p.re33) / p.re23e32 * p.re22)
            - (S21m - p.fe30) / p.fe10e32 * (S12m - p.re03) / p.re23e01 * p.fe22 * p.re11;
    auto S11 = ((S11m - p.fe00) / p.fe10e01 * (1.0 + (S22m - p.re33) / p.re23e32 * p.re22)
            - p.fe22 * (S21m - p.fe30) / p.fe10e32 * (S12m - p.re03) / p.re23e01) / denom;
    auto S21 = ((S21m - p.fe30) / p.fe10e32 * (1.0 + (S22m - p.re33) / p.re23e32 * (p.re22 - p.fe22))) / denom;
    auto S22 = ((S22m - p.re33) / p.re23e32 * (1.0 + (S11m - p.fe00) / p.fe10e01 * p.fe11)
            - p.re11 * (S21m - p.fe30) / p.fe10e32 * (S12m - p.re03) / p.re23e01) / denom;
    auto S12 = ((S12m - p.re03) / p.re23e01 * (1.0 + (S11m - p.fe00) / p.fe10e01 * (p.fe11 - p.re11))) / denom;

    d.real_S11 = S11.real();
    d.imag_S11 = S11.imag();
    d.real_S12 = S12.real();
    d.imag_S12 = S12.imag();
    d.real_S21 = S21.real();
    d.imag_S21 = S21.imag();
    d.real_S22 = S22.real();
    d.imag_S22 = S22.imag();
}

Calibration::InterpolationType Calibration::getInterpolation(Protocol::SweepSettings settings)
{
    if(!points.size()) {
        return InterpolationType::NoCalibration;
    }
    if(settings.f_start < points.front().frequency || settings.f_stop > points.back().frequency) {
        return InterpolationType::Extrapolate;
    }
    // Either exact or interpolation, check individual frequencies
    uint32_t f_step = (settings.f_stop - settings.f_start) / (settings.points - 1);
    for(uint64_t f = settings.f_start; f <= settings.f_stop; f += f_step) {
        if(find_if(points.begin(), points.end(), [&f](const Point& p){
            return abs(f - p.frequency) < 100;
        }) == points.end()) {
            return InterpolationType::Interpolate;
        }
    }
    // if we get here all frequency points were matched
    if(points.front().frequency == settings.f_start && points.back().frequency == settings.f_stop) {
        return InterpolationType::Unchanged;
    } else {
        return InterpolationType::Exact;
    }
}

QString Calibration::MeasurementToString(Calibration::Measurement m)
{
    switch(m) {
    case Measurement::Port1Open:
        return "Port 1 Open";
    case Measurement::Port1Short:
        return "Port 1 Short";
    case Measurement::Port1Load:
        return "Port 1 Load";
    case Measurement::Port2Open:
        return "Port 2 Open";
    case Measurement::Port2Short:
        return "Port 2 Short";
    case Measurement::Port2Load:
        return "Port 2 Load";
    case Measurement::Through:
        return "Through";
    case Measurement::Isolation:
        return "Isolation";
    }
}

ostream& operator<<(ostream &os, const Calibration &c)
{
    os << c.points.size() << "\n";
    for(auto p : c.points) {
        os << p;
    }
    os << endl;
    return os;
}

istream& operator >>(istream &in, Calibration &c)
{
    c.clearMeasurements();
    c.points.clear();
    int npoints;
    in >> npoints;
    for(int i=0;i<npoints;i++) {
        Calibration::Point p;
        in >> p;
        c.points.push_back(p);
    }
    return in;
}

bool Calibration::SanityCheckSamples(std::vector<Calibration::Measurement> &requiredMeasurements)
{
    // sanity check measurements, all need to be of the same size with the same frequencies (except for isolation which may be empty)
    vector<uint64_t> freqs;
    for(auto type : requiredMeasurements) {
        auto m = measurements[type];
        if(m.size() == 0) {
            // empty required measurement
            return false;
        }
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

Calibration::Point Calibration::getCalibrationPoint(Protocol::Datapoint &d)
{
    if(!points.size()) {
        throw runtime_error("No calibration points available");
    }
    if(d.frequency <= points.front().frequency) {
        // use first point even for lower frequencies
        return points.front();
    }
    if(d.frequency >= points.back().frequency) {
        // use last point even for higher frequencies
        return points.back();
    }
    auto p = lower_bound(points.begin(), points.end(), d.frequency, [](Point p, uint64_t freq) -> bool {
        return p.frequency < freq;
    });
    if(p->frequency == d.frequency) {
        // Exact match, return point
        return *p;
    }
    // need to interpolate
    auto high = p;
    p--;
    auto low = p;
    double alpha = (d.frequency - low->frequency) / (high->frequency - low->frequency);
    Point ret;
    ret.frequency = d.frequency;
    ret.fe00 = low->fe00 * (1 - alpha) + high->fe00 * alpha;
    ret.fe11 = low->fe11 * (1 - alpha) + high->fe11 * alpha;
    ret.fe22 = low->fe22 * (1 - alpha) + high->fe22 * alpha;
    ret.fe30 = low->fe30 * (1 - alpha) + high->fe30 * alpha;
    ret.re03 = low->re03 * (1 - alpha) + high->re03 * alpha;
    ret.re11 = low->re11 * (1 - alpha) + high->re11 * alpha;
    ret.re22 = low->re22 * (1 - alpha) + high->re22 * alpha;
    ret.re33 = low->re33 * (1 - alpha) + high->re33 * alpha;
    ret.fe10e01 = low->fe10e01 * (1 - alpha) + high->fe10e01 * alpha;
    ret.fe10e32 = low->fe10e32 * (1 - alpha) + high->fe10e32 * alpha;
    ret.re23e01 = low->re23e01 * (1 - alpha) + high->re23e01 * alpha;
    ret.re23e32 = low->re23e32 * (1 - alpha) + high->re23e32 * alpha;
    return ret;
}

void Calibration::computeSOL(std::complex<double> s_m, std::complex<double> o_m, std::complex<double> l_m,
                             std::complex<double> &directivity, std::complex<double> &match, std::complex<double> &tracking,
                             std::complex<double> o_c, std::complex<double> s_c, std::complex<double> l_c)
{
    // equations from page 13 of http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
    // solved while taking non ideal o/s/l standards into account
    auto denom = l_c * o_c * (o_m - l_m) + l_c * s_c * (l_m - s_m) + o_c * s_c * (s_m - o_m);
    directivity = (l_c * o_m * (s_m * (o_c - s_c) + l_m * s_c) - l_c * o_c * l_m * s_m + o_c * l_m * s_c * (s_m - o_m)) / denom;
    match = (l_c * (o_m - s_m) + o_c * (s_m - l_m) + s_c * (l_m - o_m)) / denom;
    auto delta = (l_c * l_m * (o_m - s_m) + o_c * o_m * (s_m - l_m) + s_c * s_m * (l_m - o_m)) / denom;
    tracking = directivity * match - delta;
}

std::complex<double> Calibration::correctSOL(std::complex<double> measured, std::complex<double> directivity, std::complex<double> match, std::complex<double> tracking)
{
    return (measured - directivity) / (measured * match - directivity * match + tracking);
}


