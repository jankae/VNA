#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "device.h"
#include <complex>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include "calkit.h"

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

    enum class Type {
        Port1SOL,
        Port2SOL,
        FullSOLT,
    };

    bool calculationPossible(Type type);
    bool constructErrorTerms(Type type, Calkit c);

    void correctMeasurement(Protocol::Datapoint &d);

    enum class InterpolationType {
        Unchanged, // Nothing has changed, settings and calibration points match
        Exact, // Every frequency point in settings has an exact calibration point (but there are more calibration points outside of the sweep)
        Interpolate, // Every point in the sweep can be interpolated between two calibration points
        Extrapolate, // At least one point in sweep is outside of the calibration and has to be extrapolated
        NoCalibration, // No calibration available
    };

    InterpolationType getInterpolation(Protocol::SweepSettings settings);

    static QString MeasurementToString(Measurement m);

    friend std::ostream& operator<<(std::ostream& os, const Calibration& c);
    friend std::istream& operator >> (std::istream &in, Calibration& c);
    int nPoints() {
        return points.size();
    }
private:
    void construct12TermPoints(Calkit c);
    void constructPort1SOL(Calkit c);
    void constructPort2SOL(Calkit c);
    bool SanityCheckSamples(std::vector<Measurement> &requiredMeasurements);
    class Point
    {
    public:
        double frequency;
        // Forward error terms
        std::complex<double> fe00, fe11, fe10e01, fe10e32, fe22, fe30;
        // Reverse error terms
        std::complex<double> re33, re11, re23e32, re23e01, re22, re03;
        friend std::ostream& operator<<(std::ostream& os, const Point& p) {
            os << std::fixed << std::setprecision(12);
            os << p.frequency << "\n";
            os << p.fe00 << "\n" << p.fe11 << "\n" << p.fe10e01 << "\n" << p.fe10e32 << "\n" << p.fe22 << "\n" << p.fe30 << "\n";
            os << p.re33 << "\n" << p.re11 << "\n" << p.re23e32 << "\n" << p.re23e01 << "\n" << p.re22 << "\n" << p.re03 << std::endl;
            return os;
        }
        friend std::istream & operator >> (std::istream &in, Point& p)
        {
            in >> p.frequency;
            in >> p.fe00;
            in >> p.fe11;
            in >> p.fe10e01;
            in >> p.fe10e32;
            in >> p.fe22;
            in >> p.fe30;
            in >> p.re33;
            in >> p.re11;
            in >> p.re23e32;
            in >> p.re23e01;
            in >> p.re22;
            in >> p.re03;
            return in;
        }
    };
    Point getCalibrationPoint(Protocol::Datapoint &d);
    /*
     * Constructs directivity, match and tracking correction factors from measurements of three distinct impedances
     * Normally, an open, short and load are used (with ideal reflection coefficients of 1, -1 and 0 respectively).
     * The actual reflection coefficients can be passed on as optional arguments to take into account the non-ideal
     * calibration kit.
     */
    void computeSOL(std::complex<double> s_m,
                    std::complex<double> o_m,
                    std::complex<double> l_m,
                    std::complex<double> &directivity,
                    std::complex<double> &match,
                    std::complex<double> &tracking,
                    std::complex<double> o_c = std::complex<double>(1.0, 0),
                    std::complex<double> s_c = std::complex<double>(-1.0, 0),
                    std::complex<double> l_c = std::complex<double>(0, 0));
    std::complex<double> correctSOL(std::complex<double> measured,
                                    std::complex<double> directivity,
                                    std::complex<double> match,
                                    std::complex<double> tracking);
    std::map<Measurement, std::vector<Protocol::Datapoint>> measurements;
    std::vector<Point> points;
};

#endif // CALIBRATION_H
