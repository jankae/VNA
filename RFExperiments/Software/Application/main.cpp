#include <iostream>

#include <unistd.h>
#include <QtWidgets/QApplication>
#include "vna.h"
#include "valueinput.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    VNA vna;
    vna.resize(1000, 700);
    vna.show();
//    std::vector<ValueInput::Unit> u;
//    u.push_back(ValueInput::Unit("Hz", 1.0));
//    u.push_back(ValueInput::Unit("kHz", 1000.0));
//    u.push_back(ValueInput::Unit("MHz", 1000000.0));
//    u.push_back(ValueInput::Unit("GHz", 1000000000.0));
//    ValueInput v(u, "Test");
    a.exec();
}
