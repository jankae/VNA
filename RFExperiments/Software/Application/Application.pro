HEADERS += \
    ../RFExperiments/Application/Communication/Protocol.hpp \
    device.h

SOURCES += \
    ../RFExperiments/Application/Communication/Protocol.cpp \
    device.cpp \
    main.cpp

LIBS += -lboost_log -lusb-1.0
DEFINES += BOOST_ALL_DYN_LINK
