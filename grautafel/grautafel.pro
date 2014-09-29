#-------------------------------------------------
#
# Project created by QtCreator 2013-08-20T10:36:28
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -pg
QMAKE_LFLAGS += -pg


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = grautafel
TEMPLATE = app


SOURCES +=\
    gthoughtransform.C \
    gtconvolutionkernel.C \
    gtimageview.C \
    gtimagelistwidget.C \
    gtmainwidget.C \
    gtimage.C \
    main.C \
    mainwindow.C \
    viewzoomwidget.C \
    pagedialog.C \
    colorwidget.C

HEADERS  += mainwindow.h \
    gtimage.h \
    gthoughtransform.h \
    gtimageview.h \
    gtimagelistwidget.h \
    gtmainwidget.h \
    viewzoomwidget.h \
    pagedialog.h \
    gtconvolutionkernel.h \
    colorwidget.h

FORMS    += mainwindow.ui

TRANSLATIONS += grautafel_cs.ts
