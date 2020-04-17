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
    void addMeasurement(Measurement type, Protocol::Datapoint d);
    void constructErrorTerms();
    void correctMeasurement(Protocol::Datapoint &d);

private:
    class Point
    {
    public:
        double frequency;
        // Forward error terms
        std::complex<double> fe00, fe11, fe10e01, fe10e32, fe22, fe30;
        // Reverse error terms
        std::complex<double> re33, re11, re23e32, re23e01, re22, re03;
    };
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
