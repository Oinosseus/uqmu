#-------------------------------------------------
#
# Project created by QtCreator 2015-09-03T19:45:33
#
#-------------------------------------------------

# application version
UQMU_VERSION = "0.1"
DEFINES += UQMU_VERSION=\\\"$$UQMU_VERSION\\\"

QT       += core gui widgets network

message("Qt version: $${QT_VERSION}")

TARGET = aqmu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    runlog.cpp \
    dialogsettings.cpp \
    widgetpathselector.cpp \
    dialogabout.cpp \
    widgettargetprocessor.cpp \
    servermanager.cpp \
    networkaccessmanager.cpp \
    serverfile.cpp \
    widgettextedit.cpp \
    servermanager_file.cpp \
    dialoguserauth.cpp \
    servermanager_uqmu_http.cpp

HEADERS  += mainwindow.h \
    runlog.h \
    dialogsettings.h \
    widgetpathselector.h \
    dialogabout.h \
    widgettargetprocessor.h \
    servermanager.h \
    networkaccessmanager.h \
    serverfile.h \
    widgettextedit.h \
    servermanager_file.h \
    dialoguserauth.h \
    servermanager_uqmu_http.h

FORMS    += mainwindow.ui
