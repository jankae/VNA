#include "calkit.h"

#include <fstream>
#include <iomanip>
#include "calkitdialog.h"
#include <math.h>

using namespace std;

Calkit::Calkit()
{
    open_Z0 = 50.0;
    open_delay = 0.0;
    open_loss = 0.0;
    open_C0 = 0.0;
    open_C1 = 0.0;
    open_C2 = 0.0;
    open_C3 = 0.0;

    short_Z0 = 50.0;
    short_delay = 0.0;
    short_loss = 0.0;
    short_L0 = 0.0;
    short_L1 = 0.0;
    short_L2 = 0.0;
    short_L3 = 0.0;

    load_Z0 = 50.0;

    through_Z0 = 50.0;
    through_delay = 0.0;
    through_loss = 0.0;
}

void Calkit::toFile(std::string filename)
{
    ofstream file;
    file.open(filename);
    file << std::fixed << std::setprecision(12);
    file << open_Z0 << "\n" << open_delay << "\n" << open_loss << "\n" << open_C0 << "\n" << open_C1 << "\n" << open_C2 << "\n" << open_C3 << "\n";
    file << short_Z0 << "\n" << short_delay << "\n" << short_loss << "\n" << short_L0 << "\n" << short_L1 << "\n" << short_L2 << "\n" << short_L3 << "\n";
    file << load_Z0 << "\n";
    file << through_Z0 << "\n" << through_delay << "\n" << through_loss << "\n";
    file.close();
}

Calkit Calkit::fromFile(std::string filename)
{
    Calkit c;
    ifstream file;
    file.open(filename);
    file >> c.open_Z0;
    file >> c.open_delay;
    file >> c.open_loss;
    file >> c.open_C0;
    file >> c.open_C1;
    file >> c.open_C2;
    file >> c.open_C3;
    file >> c.short_Z0;
    file >> c.short_delay;
    file >> c.short_loss;
    file >> c.short_L0;
    file >> c.short_L1;
    file >> c.short_L2;
    file >> c.short_L3;
    file >> c.load_Z0;
    file >> c.through_Z0;
    file >> c.through_delay;
    file >> c.through_loss;
    file.close();
    return c;
}

void Calkit::edit()
{
    auto dialog = new CalkitDialog(*this);
    dialog->show();
}

Calkit::Reflection Calkit::toReflection(double frequency)
{
    Reflection ref;
    auto imp_load = complex<double>(load_Z0, 0);
    ref.Load = (imp_load - complex<double>(50.0)) / (imp_load + complex<double>(50.0));

    // calculate fringing capacitance for open
    double Cfringing = open_C0 * 1e-15 + open_C1 * 1e-27 * frequency + open_C2 * 1e-36 * pow(frequency, 2) + open_C3 * 1e-45 * pow(frequency, 3);
    // convert to impedance
    if (Cfringing == 0) {
        // special case to avoid issues with infinity
        ref.Open = complex<double>(1.0, 0);
    } else {
        auto imp_open = complex<double>(0, -1.0 / (frequency * 2 * M_PI * Cfringing));
        ref.Open = (imp_open - complex<double>(50.0)) / (imp_open + complex<double>(50.0));
    }
    // transform the delay into a phase shift for the given frequency
    double open_phaseshift = 2 * M_PI * frequency * open_delay * 1e-12;
    double open_att_db = open_loss * 1e9 * 4.3429 * open_delay * 1e-12 / open_Z0 * sqrt(frequency / 1e9);
    double open_att = pow(10.0, -open_att_db / 10.0);
    auto open_correction = polar<double>(open_att, open_phaseshift);
    ref.Open *= open_correction;

    // calculate inductance for short
    double Lseries = short_L0 * 1e-12 + short_L1 * 1e-24 * frequency + short_L2 * 1e-33 * pow(frequency, 2) + short_L3 * 1e-42 * pow(frequency, 3);
    // convert to impedance
    auto imp_short = complex<double>(0, frequency * 2 * M_PI * Lseries);
    ref.Short =  (imp_short - complex<double>(50.0)) / (imp_short + complex<double>(50.0));
    // transform the delay into a phase shift for the given frequency
    double short_phaseshift = 2 * M_PI * frequency * short_delay * 1e-12;
    double short_att_db = short_loss * 1e9 * 4.3429 * short_delay * 1e-12 / short_Z0 * sqrt(frequency / 1e9);;
    double short_att = pow(10.0, -short_att_db / 10.0);
    auto short_correction = polar<double>(short_att, short_phaseshift);
    ref.Short *= short_correction;

    // calculate effect of through
    double through_phaseshift = 2 * M_PI * frequency * through_delay * 1e-12;
    double through_att_db = through_loss * 1e9 * 4.3429 * through_delay * 1e-12 / through_Z0 * sqrt(frequency / 1e9);;
    double through_att = pow(10.0, -through_att_db / 10.0);
    ref.Through = polar<double>(through_att, through_phaseshift);

    return ref;
}
