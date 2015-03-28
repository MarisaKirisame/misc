TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++1y
SOURCES += main.cpp

OTHER_FILES += \
    LICENSE \
    README.md

HEADERS += \
    misc.hpp

LIBS += -lpthread
