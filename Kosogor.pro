#-------------------------------------------------
#
# Project created by QtCreator 2011-11-28T10:01:25
#
#-------------------------------------------------

QT       += core gui sql

TARGET = Kosogor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    authdialog.cpp

HEADERS  += mainwindow.h \
    authdialog.h

FORMS    += mainwindow.ui \
    authdialog.ui

OTHER_FILES += \
    create.sql

