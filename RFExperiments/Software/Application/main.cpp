#include <iostream>

#include <unistd.h>
#include <QtWidgets/QApplication>
#include "vna.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    VNA vna;
    vna.resize(1000, 700);
    vna.show();
    a.exec();
}
