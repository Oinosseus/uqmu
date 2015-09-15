#-------------------------------------------------
#
# Project created by QtCreator 2015-09-03T19:45:33
#
#-------------------------------------------------

# application version
UQMU_VERSION = "0.1"
DEFINES += UQMU_VERSION=\\\"$$UQMU_VERSION\\\"

QT       += core gui widgets network

TARGET = uqmu
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
    servermanager_uqmu_http.cpp \
    dialoglicense.cpp

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
    servermanager_uqmu_http.h \
    dialoglicense.h

FORMS    += mainwindow.ui

# extra work
linux {
    extramake.commands = $(COPY_DIR) \"$$PWD/../../LICENSE\" \"$$OUT_PWD\"
}
win32 {
    extramake.commands = ..\extramake.bat \"$$PWD\" \"$$OUT_PWD\"
}
first.depends = $(first) extramake
export(first.depends)
export(extramake.commands)
QMAKE_EXTRA_TARGETS += first extramake
