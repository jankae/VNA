#include <iostream>

#include <unistd.h>
#include <QtWidgets/QApplication>
#include "vna.h"
#include "valueinput.h"

#include "Menu/menu.h"
#include "Menu/menuaction.h"
#include "Menu/menuvalue.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    VNA vna;
    vna.resize(1280, 800);
    vna.show();

//    QStackedLayout menuLayout;
//    Menu mMain(menuLayout);
//    Menu mFrequency(menuLayout);
//    mFrequency.addItem(new MenuValue("Center Frequency", 1000000000, "Hz"));
//    mFrequency.addItem(new MenuValue("Start Frequency", 1000000000, "Hz"));
//    mFrequency.addItem(new MenuValue("Stop Frequency", 1000000000, "Hz"));
//    mFrequency.finalize();
//    mMain.addMenu(&mFrequency, "Frequency");
//    Menu mSpan(menuLayout);
//    mSpan.addItem(new MenuValue("Span", 30000000, "Hz"));
//    mSpan.addItem(new MenuAction("Zoom in"));
//    mSpan.addItem(new MenuAction("Zoom out"));
//    mSpan.addItem(new MenuAction("Full span"));
//    mSpan.finalize();
//    mMain.addMenu(&mSpan, "Span");
//    mMain.finalize();
//    QWidget main;
//    main.setLayout(&menuLayout);
//    main.show();
    a.exec();
}
