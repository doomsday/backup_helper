TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG += static
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -static-libstdc++

SOURCES += main.cpp \
    config.cpp \
    performer.cpp \
    configparser.cpp \
    logger.cpp

HEADERS += \
    config.hpp \
    performer.hpp \
    configparser.hpp \
    logger.hpp

OTHER_FILES += \
    README.txt \
    bh.conf

