TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    config.cpp \
    globalexceptions.cpp \
    performer.cpp \
    configparser.cpp

HEADERS += \
    config.hpp \
    globalexceptions.hpp \
    performer.hpp \
    configparser.hpp

OTHER_FILES += \
    README.txt

