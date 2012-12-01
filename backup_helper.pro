TEMPLATE = app
CONFIG += console
CONFIG -= qt

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
    README.txt

