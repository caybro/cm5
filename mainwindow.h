//////////////////////////////////////////////////////////////////////////
// mainwindow.h                                                         //
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

#include "ui_mainwindow.h"
#include "catalogDirModel.h"

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

/**
 * Mainwindow of the Catalog Manager
 *
 * \author Lukas Tinkl <lukas@kde.org>
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0 );
    ~MainWindow();
private slots:
    void loadSettings();
    void saveSettings();

    void slotAbout();

    void slotCatalogContextMenu( const QPoint & pos );
    void slotCatalogDoubleClicked( const QModelIndex & index );
    void slotCatalogChanged( const QModelIndex & index );
    void slotOpenCatalog();
    void slotSpellcheck();
    void slotStatistics();
    void slotConfigure();
private:
    QString formatNumber( double num, int prec = 2 ) const;
    void updateCaption();
    CatalogDirModel * m_model;
    Ui_MainWindow ui;
    QString m_editor;           // editor program to launch with Open action
    QString m_rootdir;
};

#endif
