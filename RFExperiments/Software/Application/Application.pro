HEADERS += \
    ../RFExperiments/Application/Communication/Protocol.hpp \
    bodeplot.h \
    device.h \
    plot.h \
    smithchart.h \
    sparam.h \
    sparamtable.h \
    valueinput.h \
    vna.h

SOURCES += \
    ../RFExperiments/Application/Communication/Protocol.cpp \
    bodeplot.cpp \
    device.cpp \
    main.cpp \
    smithchart.cpp \
    sparamtable.cpp \
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
