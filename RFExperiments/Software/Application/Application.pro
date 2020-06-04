HEADERS += \
    ../RFExperiments/Application/Communication/Protocol.hpp \
    CustomWidgets/toggleswitch.h \
    Menu/menu.h \
    Menu/menuaction.h \
    Menu/menubool.h \
    Menu/menuitem.h \
    Menu/menuvalue.h \
    bodeplot.h \
    calibration.h \
    calkit.h \
    calkitdialog.h \
    device.h \
    plot.h \
    qwtplotpiecewisecurve.h \
    smithchart.h \
    sparam.h \
    sparamtable.h \
    touchstone.h \
    trace.h \
    unit.h \
    valueinput.h \
    vna.h

SOURCES += \
    ../RFExperiments/Application/Communication/Protocol.cpp \
    CustomWidgets/toggleswitch.cpp \
    Menu/menu.cpp \
    Menu/menuaction.cpp \
    Menu/menubool.cpp \
    Menu/menuitem.cpp \
    Menu/menuvalue.cpp \
    bodeplot.cpp \
    calibration.cpp \
    calkit.cpp \
    calkitdialog.cpp \
    device.cpp \
    main.cpp \
    qwtplotpiecewisecurve.cpp \
    smithchart.cpp \
    sparamtable.cpp \
    touchstone.cpp \
    trace.cpp \
    unit.cpp \
    valueinput.cpp \
    vna.cpp

LIBS += -lboost_log -lusb-1.0
DEFINES += BOOST_ALL_DYN_LINK
INCLUDEPATH += /usr/include/qwt
LIBS += -L/usr/lib/ -lqwt-qt5

QT += widgets charts

FORMS += \
    calkitdialog.ui

DISTFILES +=

RESOURCES += \
    icons.qrc
