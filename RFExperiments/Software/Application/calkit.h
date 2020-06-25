#ifndef CALKIT_H
#define CALKIT_H

#include <string>
#include <complex>

class Calkit
{
public:
    Calkit();

    class Reflection {
    public:
        std::complex<double> Open;
        std::complex<double> Short;
        std::complex<double> Load;
        std::complex<double> ThroughS11, ThroughS12, ThroughS21, ThroughS22;
    };

    double open_Z0, open_delay, open_loss, open_C0, open_C1, open_C2, open_C3;
    double short_Z0, short_delay, short_loss, short_L0, short_L1, short_L2, short_L3;
    double load_Z0;
    double through_Z0, through_delay, through_loss;

    // coefficients/measurement file switch
    bool open_measurements;
    bool short_measurements;
    bool load_measurements;
    bool through_measurements;

    std::string open_file, short_file, load_file, through_file;
    int open_Sparam, short_Sparam, load_Sparam, through_Sparam1, through_Sparam2;

    void toFile(std::string filename);
    static Calkit fromFile(std::string filename);
    void edit();
    Reflection toReflection(double frequency);
private:


};

#endif // CALKIT_H
