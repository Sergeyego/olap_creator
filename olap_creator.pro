#-------------------------------------------------
#
# Project created by QtCreator 2019-12-05T11:08:44
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = olap_creator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    db/dblogin.cpp \
    olap/axiswidget.cpp \
    olap/cubewidget.cpp \
    olap/cubic.cpp \
    olap/olapmodel.cpp \
    olap/tableview.cpp \
    formedtolap.cpp \
    dialogopen.cpp \
    sqlhighlighter.cpp \
    codeeditor.cpp

HEADERS += \
        mainwindow.h \
    db/dblogin.h \
    olap/axiswidget.h \
    olap/cubewidget.h \
    olap/cubic.h \
    olap/olapmodel.h \
    olap/tableview.h \
    formedtolap.h \
    dialogopen.h \
    sqlhighlighter.h \
    codeeditor.h

FORMS += \
        mainwindow.ui \
    db/dblogin.ui \
    olap/axiswidget.ui \
    olap/cubewidget.ui \
    formedtolap.ui \
    dialogopen.ui

INCLUDEPATH+=/usr/include/xlslib
LIBS += -lxls

RESOURCES += \
    res.qrc
