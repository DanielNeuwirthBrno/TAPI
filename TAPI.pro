#-------------------------------------------------
#
# Project created by QtCreator 2019-03-04T21:03:45
#
#-------------------------------------------------

QT += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TAPI
TEMPLATE = app

HEADERS += buildrequestwindow.h \
           connection.h \
           credentials.h \
           database.h \
           endpoint.h \
           endpointswindow.h \
           error.h \
           logwindow.h \
           mainwindow.h \
           methods.h \
           pathwindow.h \
           random.h \
           request.h \
           requestwindow.h \
           responsewindow.h \
           session.h \
           tables.h \
           tablewidget.h \
           tokenwindow.h \
           types.h \
           ui/ui_buildrequestwindow.h \
           ui/ui_endpointswindow.h \
           ui/ui_logwindow.h \
           ui/ui_mainwindow.h \
           ui/ui_pathwindow.h \
           ui/ui_requestwindow.h \
           ui/ui_responsewindow.h \
           ui/ui_tokenwindow.h

SOURCES += buildrequestwindow.cpp \
           database.cpp \
           endpoint.cpp \
           endpointswindow.cpp \
           logwindow.cpp \
           main.cpp \
           mainwindow.cpp \
           random.cpp \
           request.cpp \
           responsewindow.cpp \
           session.cpp

DISTFILES += notes.txt

RESOURCES += resource.qrc

win32:RC_ICONS += app.ico
win32:VERSION = 0.0.1.51
win32:RC_LANG = 0x0405
win32:QMAKE_TARGET_COPYRIGHT = "Daniel Neuwirth"
