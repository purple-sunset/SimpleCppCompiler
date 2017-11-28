#-------------------------------------------------
#
# Project created by QtCreator 2017-11-28T19:10:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleCppCompiler
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
    codeeditor.cpp \
    tokentype.cpp \
    valuetype.cpp \
    declaredvariable.cpp \
    error.cpp \
    token.cpp \
    parser.cpp \
    declaredfunction.cpp \
    enummath.cpp

HEADERS += \
        mainwindow.h \
    codeeditor.h \
    tokentype.h \
    valuetype.h \
    declaredvariable.h \
    error.h \
    token.h \
    parser.h \
    declaredfunction.h \
    enummath.h

FORMS += \
        mainwindow.ui
