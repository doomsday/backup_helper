TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    services.cpp \
    performs.cpp \
    checks.cpp \
    auxiliary.cpp \
    config.cpp \
    globalexceptions.cpp

HEADERS += \
    services.hpp \
    performs.hpp \
    checks.hpp \
    auxiliary.hpp \
    config.hpp \
    globalexceptions.hpp

