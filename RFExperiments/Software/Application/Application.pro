HEADERS += \
    ../RFExperiments/Application/Communication/Protocol.hpp \
    CustomWidgets/siunitedit.h \
    CustomWidgets/toggleswitch.h \
    CustomWidgets/touchstoneimport.h \
    Menu/menu.h \
    Menu/menuaction.h \
    Menu/menubool.h \
    Menu/menuitem.h \
    Menu/menuvalue.h \
    Traces/trace.h \
    Traces/traceeditdialog.h \
    Traces/tracemodel.h \
    Traces/traceplot.h \
    Traces/tracesmithchart.h \
    Traces/tracewidget.h \
    bodeplot.h \
    calibration.h \
    calkit.h \
    calkitdialog.h \
    device.h \
    manualcontroldialog.h \
    plot.h \
    qwtplotpiecewisecurve.h \
    smithchart.h \
    sparam.h \
    sparamtable.h \
    touchstone.h \
    unit.h \
    valueinput.h \
    vna.h

SOURCES += \
    ../RFExperiments/Application/Communication/Protocol.cpp \
    CustomWidgets/siunitedit.cpp \
    CustomWidgets/toggleswitch.cpp \
    CustomWidgets/touchstoneimport.cpp \
    Menu/menu.cpp \
    Menu/menuaction.cpp \
    Menu/menubool.cpp \
    Menu/menuitem.cpp \
    Menu/menuvalue.cpp \
    Traces/trace.cpp \
    Traces/traceeditdialog.cpp \
    Traces/tracemodel.cpp \
    Traces/traceplot.cpp \
    Traces/tracesmithchart.cpp \
    Traces/tracewidget.cpp \
    bodeplot.cpp \
    calibration.cpp \
    calkit.cpp \
    calkitdialog.cpp \
    device.cpp \
    main.cpp \
    manualcontroldialog.cpp \
    qwtplotpiecewisecurve.cpp \
    smithchart.cpp \
    sparamtable.cpp \
    touchstone.cpp \
    unit.cpp \
    valueinput.cpp \
    vna.cpp

LIBS += -lboost_log -lusb-1.0
DEFINES += BOOST_ALL_DYN_LINK
INCLUDEPATH += /usr/include/qwt
LIBS += -L/usr/lib/ -lqwt-qt5

QT += widgets charts

FORMS += \
    CustomWidgets/touchstoneimport.ui \
    Traces/traceeditdialog.ui \
    Traces/tracewidget.ui \
    calkitdialog.ui \
    manualcontroldialog.ui

DISTFILES +=

RESOURCES += \
    icons.qrc
