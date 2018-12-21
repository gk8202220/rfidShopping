#-------------------------------------------------
#
# Project created by QtCreator 2018-12-11T15:39:22
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = showList
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


LIBS+= E:\qt-learing\showList\qrcodelib.lib
LIBS+= E:\qt-learing\showList\qrcodelib.dll
LIBS+= E:\qt-learing\showList\KL4003_32.dll

SOURCES += \
        main.cpp \
    goodsdatabase.cpp \
    mainwindow.cpp \
    qrcodegenerate.cpp \
    mytableview.cpp \
    kl4003.cpp

HEADERS += \
    goodsdatabase.h \
    mainwindow.h \
    qrcodegenerate.h \
    mytableview.h \
    KL4003.h

FORMS += \
        mainwindow.ui \
    qrcodegenerate.ui

RESOURCES += \
    src.qrc
