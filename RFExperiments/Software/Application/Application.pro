HEADERS += \
    ../RFExperiments/Application/Communication/Protocol.hpp \
    Menu/menu.h \
    Menu/menuaction.h \
    Menu/menuitem.h \
    Menu/menuvalue.h \
    bodeplot.h \
    calibration.h \
    device.h \
    plot.h \
    qwtplotpiecewisecurve.h \
    smithchart.h \
    sparam.h \
    sparamtable.h \
    unit.h \
    valueinput.h \
    vna.h

SOURCES += \
    ../RFExperiments/Application/Communication/Protocol.cpp \
    Menu/menu.cpp \
    Menu/menuitem.cpp \
    Menu/menulabel.cpp \
    Menu/menuvalue.cpp \
    bodeplot.cpp \
    calibration.cpp \
    device.cpp \
    main.cpp \
    qwtplotpiecewisecurve.cpp \
    smithchart.cpp \
    sparamtable.cpp \
    unit.cpp \
    valueinput.cpp \
    vna.cpp

LIBS += -lboost_log -lusb-1.0
DEFINES += BOOST_ALL_DYN_LINK
INCLUDEPATH += /usr/include/qwt
LIBS += -L/usr/lib/ -lqwt-qt5

QT += widgets charts

FORMS +=

DISTFILES +=

RESOURCES += \
    icons.qrc
