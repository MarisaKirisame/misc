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
    misc.hpp \
    value_less.hpp \
    expansion.hpp \
    can_call.hpp \
    restriction.hpp \
    combinator.hpp

LIBS += -lpthread
