# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = FTP_client

HEADERS = \
    ../../common/include/filepipe.h \
    ../../common/include/ftpstatics.h \
    ../../common/include/nlohmann/json.hpp \
    ../../common/include/nlohmann/json_fwd.hpp \
    ../../common/include/utils.h \
    ftpclient.h \
    ftpclienttest.h

SOURCES = \
   $$PWD/client_main.cpp \
   ../../common/include/filepipe.cpp \
   ../../common/include/utils.cpp \
   ftpclient.cpp \
   ftpclienttest.cpp

INCLUDEPATH =

#DEFINES = 

DISTFILES += \
    ../../common/ftp_config.json \
    input.txt

