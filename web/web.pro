TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../common/include/utils.cpp \
        httpparser.cpp \
        httpserver.cpp \
        main.cpp

DISTFILES += \
    disk/root.html \
    first_scan.txt \
    open_browser.sh

HEADERS += \
    ../common/include/utils.h \
    httpparser.h \
    httpserver.h
