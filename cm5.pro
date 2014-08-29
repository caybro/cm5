HEADERS     = mainwindow.h catalogDirModel.h \
    configdlg.h
FORMS       = mainwindow.ui \
    configdlg.ui
SOURCES     = mainwindow.cpp catalogDirModel.cpp main.cpp \
    configdlg.cpp
RESOURCES   = cm5.qrc
TRANSLATIONS = cm5_cs.ts
QT          += widgets
DISTFILES   += ./icons/*
CONFIG      += debug
VERSION     = 1.0
DEFINES     += VERSION_NUMBER=\\\"$${VERSION}\\\" 
