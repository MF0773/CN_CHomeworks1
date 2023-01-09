# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = FTP_server

HEADERS =\
    ../../common/include/filepipe.h \
    ../../common/include/nlohmann/json.hpp \
    ../../common/include/nlohmann/json_fwd.hpp \
    ../../common/include/utils.h \
    accountinfo.h \
    ftpserver.h \
    user.h

INCLUDEPATH =

#DEFINES =

SOURCES += \
    ../../common/include/filepipe.cpp \
    ../../common/include/utils.cpp \
    server_main.cpp \
    ftpserver.cpp \
    user.cpp

DISTFILES += \
    config.json

