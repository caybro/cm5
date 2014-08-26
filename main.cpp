//////////////////////////////////////////////////////////////////////////
// main.cpp                                                             //
//                                                                      //
// Copyright (C)  2007  Lukas Tinkl <lukas@kde.org>                     //
//                                                                      //
// This program is free software; you can redistribute it and/or        //
// modify it under the terms of the GNU General Public License          //
// as published by the Free Software Foundation; either version 2       //
// of the License, or (at your option) any later version.               //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program; if not, write to the Free Software          //
// Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA         //
// 02110-1301, USA.                                                     //
//////////////////////////////////////////////////////////////////////////


#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include "mainwindow.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    app.setOrganizationName( "KDE" );
    app.setOrganizationDomain( "kde.org" );
    app.setApplicationName( "cm5" );
    app.setApplicationVersion( VERSION_NUMBER );

    QTranslator translator;
    translator.load( ":/translations/cm5_" + QLocale::system().name() );
    app.installTranslator( &translator );

    MainWindow mw;
    mw.show();
    QObject::connect( &app, SIGNAL( aboutToQuit() ),
                      &mw, SLOT( saveSettings() ) );
    return app.exec();
}
