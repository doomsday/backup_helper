TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    config.cpp \
    performer.cpp \
    configparser.cpp

HEADERS += \
    config.hpp \
    performer.hpp \
    configparser.hpp

OTHER_FILES += \
    README.txt

