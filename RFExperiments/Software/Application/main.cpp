#include <iostream>

#include <unistd.h>
#include <QtWidgets/QApplication>
#include "vna.h"
#include "valueinput.h"

#include "Menu/menu.h"
#include "Menu/menuaction.h"
#include "Menu/menuvalue.h"

#include "calkit.h"
#include "touchstone.h"

#include <complex>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    VNA vna;
    vna.resize(1280, 800);
    vna.show();
    a.exec();

//    QApplication a(argc, argv);
//    auto c = new Calkit;
//    c->edit();
//    a.exec();

//    int ports = 2;
//    auto t = Touchstone(ports);
//    for(int i=0;i<10;i++) {
//        Touchstone::Datapoint p;
//        p.frequency = 100000000 * i;
//        for(int j=0;j<ports*ports;j++) {
//            auto s = std::complex<double>(i + (double) j/10, i + (double) j/10);
//            p.S.push_back(s);
//        }
//        t.AddDatapoint(p);
//    }
//    t.toFile("test.s2p", Touchstone::Unit::MHz, Touchstone::Format::RealImaginary);

//    auto t2 = Touchstone::fromFile("test.s2p");
//    t2.reduceTo1Port(1);

//    t2.toFile("copy.s2p");
}
