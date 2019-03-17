#-------------------------------------------------
#
# Project created by QtCreator 2019-02-20T17:22:49
#
#-------------------------------------------------

QT       += core gui
QT       +=serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = usartprotocal
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
RC_ICONS = logo.ico

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    serial_handle.cpp \
    mytransmission.cpp \
    qt_ui_style.cpp

HEADERS += \
        mainwindow.h \
    serial_handle.h \
    mytransmission.h \
    qt_ui_style.h

FORMS += \
        mainwindow.ui

RESOURCES +=

DISTFILES +=
