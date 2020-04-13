HEADERS += \
    ../RFExperiments/Application/Communication/Protocol.hpp \
    bodeplot.h \
    device.h \
    plot.h \
    smithchart.h \
    sparam.h \
    valueinput.h \
    vna.h

SOURCES += \
    ../RFExperiments/Application/Communication/Protocol.cpp \
    bodeplot.cpp \
    device.cpp \
    main.cpp \
    smithchart.cpp \
    valueinput.cpp \
    vna.cpp

LIBS += -lboost_log -lusb-1.0
DEFINES += BOOST_ALL_DYN_LINK

QT += widgets charts
