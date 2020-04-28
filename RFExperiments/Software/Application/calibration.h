#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "device.h"
#include <complex>
#include <vector>
#include <map>

class Calibration
{
public:
    Calibration();

    enum class Measurement {
        Port1Open,
        Port1Short,
        Port1Load,
        Port2Open,
        Port2Short,
        Port2Load,
        Isolation,
        Through,
    };
    void clearMeasurements();
    void clearMeasurement(Measurement type);
    void addMeasurement(Measurement type, Protocol::Datapoint &d);
    void construct12TermPoints();
    void constructPort1OSL();
    void constructPort2OSL();
    void correctMeasurement(Protocol::Datapoint &d);

    enum class InterpolationType {
        Exact,
        Interpolate,
        Extrapolate,
        NoCalibration,
    };

    InterpolationType getInterpolation(Protocol::SweepSettings settings);

private:
    bool SanityCheckSamples(std::vector<Measurement> &requiredMeasurements);
    class Point
    {
    public:
        double frequency;
        // Forward error terms
        std::complex<double> fe00, fe11, fe10e01, fe10e32, fe22, fe30;
        // Reverse error terms
        std::complex<double> re33, re11, re23e32, re23e01, re22, re03;
    };
    Point getCalibrationPoint(Protocol::Datapoint &d);
    void computeOSL(std::complex<double> o_m,
                    std::complex<double> s_m,
                    std::complex<double> l_m,
                    std::complex<double> &directivity,
                    std::complex<double> &match,
                    std::complex<double> &tracking);
    std::complex<double> correctOSL(std::complex<double> measured,
                                    std::complex<double> directivity,
                                    std::complex<double> match,
                                    std::complex<double> tracking);
    std::map<Measurement, std::vector<Protocol::Datapoint>> measurements;
    std::vector<Point> points;
};

#endif // CALIBRATION_H
