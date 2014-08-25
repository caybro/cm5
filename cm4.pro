HEADERS     = mainwindow.h catalogDirModel.h
FORMS       = mainwindow.ui
SOURCES     = mainwindow.cpp catalogDirModel.cpp main.cpp
RESOURCES   = cm4.qrc
TRANSLATIONS= cm4_cs.ts
#QT          += network xml
DISTFILES   += ./icons/*
CONFIG      += qt debug
VERSION     = 1.1
DEFINES     += VERSION_NUMBER=\\\"$${VERSION}\\\" 
