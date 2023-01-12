TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        httpparser.cpp \
        httpserver.cpp \
        main.cpp

DISTFILES += \
    first_scan.txt \
    open_browser.sh

HEADERS += \
    httpparser.h \
    httpserver.h \
    utils.hpp
