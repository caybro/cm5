//////////////////////////////////////////////////////////////////////////
// mainwindow.cpp                                                       //
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

#include <QtGui>

#include "mainwindow.h"


MainWindow::MainWindow( QWidget * parent )
    : QMainWindow( parent )
{
    m_model = new CatalogDirModel( "/home/lukas/svn/KDE/3.5/l10n/cs/messages/", this );

    // GUI
    ui.setupUi( this );
    ui.actionSpellcheck->setVisible( false );

    connect( ui.actionQuit, SIGNAL( triggered() ),
             qApp, SLOT( quit() ) );
    connect( ui.actionReload, SIGNAL( triggered() ),
             m_model, SLOT( reload() ) );
    connect( ui.actionAboutQt, SIGNAL( triggered() ),
             qApp, SLOT( aboutQt() ) );
    connect( ui.actionAbout, SIGNAL( triggered() ),
             this, SLOT( slotAbout() ) );
    connect( ui.actionOpenInEditor, SIGNAL( triggered() ),
             this, SLOT( slotOpenCatalog() ) );
    connect( ui.actionSpellcheck, SIGNAL( triggered() ),
             this, SLOT( slotSpellcheck() ) );
    connect( ui.actionStatistics, SIGNAL( triggered() ),
             this, SLOT( slotStatistics() ) );
    connect( ui.actionConfigure, SIGNAL( triggered() ),
             this, SLOT( slotConfigure() ) );

    // view
    connect( ui.catalogView, SIGNAL( pressed( const QModelIndex & ) ),
             this, SLOT( slotCatalogChanged( const QModelIndex & ) ) );

    connect( ui.catalogView, SIGNAL( activated( const QModelIndex & ) ),
             this, SLOT( slotCatalogDoubleClicked( const QModelIndex & ) ) );

    connect( ui.catalogView, SIGNAL( customContextMenuRequested ( const QPoint & ) ),
             this, SLOT( slotCatalogContextMenu( const QPoint & ) ) );

    loadSettings();

    // model
    ui.catalogView->setModel( m_model );
    ui.catalogView->setRootIndex( m_model->rootIndex() );
    //ui.catalogView->resizeColumnToContents( 0 );

    updateCaption();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete m_model;
}

void MainWindow::loadSettings()
{
    QSettings settings;

    settings.beginGroup( "MainWindow" );
    restoreState( settings.value( "state" ).toByteArray() );
    resize( settings.value( "size", QSize( 400, 400 ) ).toSize() );
    move( settings.value( "pos", QPoint( 200, 200) ).toPoint() );
    settings.endGroup();

    settings.beginGroup( "CatalogManager" );
    ui.catalogView->header()->restoreState( settings.value( "catalog_headers" ).toByteArray() );
    m_rootdir = settings.value( "root_dir", QDir::homePath() ).toString();
    m_model->setRootDir( m_rootdir  );
    m_editor = settings.value( "editor", "linguist-qt5" ).toString();
    settings.endGroup();
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.beginGroup( "MainWindow" );
    settings.setValue( "state", saveState() );
    settings.setValue( "size", size() );
    settings.setValue( "pos", pos() );
    settings.endGroup();

    settings.beginGroup( "CatalogManager" );
    settings.setValue( "catalog_headers", ui.catalogView->header()->saveState() );
    settings.setValue( "root_dir", m_rootdir );
    settings.setValue( "editor", m_editor );
    settings.endGroup();
}

void MainWindow::slotAbout()
{
    QMessageBox::about( this, tr( "Catalog Manager" ),
                        tr( "<b>Catalog Manager %1</b><br>"
                            "Proof-of-concept port of original KBabel's catalog manager to Qt 4.<br>"
                            "Written during SUSE/Novell Hack Week, 25-30th June 2007<br>"
                            "Author: %2" ).arg( qApp->applicationVersion() )
                        .arg( "Lukas Tinkl <a href=\"mailto:lukas@kde.org\">lukas@kde.org</a>" ) );
}

void MainWindow::slotCatalogDoubleClicked( const QModelIndex & index )
{
    if ( m_model->isDir( index ) )
    {
        ui.catalogView->setExpanded( index, !ui.catalogView->isExpanded( index ) );

        return;
    }

    QString path = m_model->filePath( index );

    QProcess::startDetached( m_editor, QStringList( path ) );
}

void MainWindow::slotCatalogChanged( const QModelIndex & index )
{
    ui.actionOpenInEditor->setEnabled( !m_model->isDir( index ) );
}

void MainWindow::slotCatalogContextMenu( const QPoint & pos )
{
    QModelIndex currentIndex = ui.catalogView->indexAt( pos );
    const bool isdir = m_model->isDir( currentIndex );
    if ( isdir )
        ui.catalogView->expand( currentIndex ); // fetch totals

    QMenu menu( this );
    if ( !isdir )
        menu.addAction( ui.actionOpenInEditor );
    menu.addAction( ui.actionStatistics );
    menu.addAction( ui.actionSpellcheck );

    menu.exec( ui.catalogView->viewport()->mapToGlobal( pos ) );
}

void MainWindow::slotOpenCatalog()
{
    QModelIndex currentIndex = ui.catalogView->currentIndex();
    if ( !currentIndex.isValid() )
        return;

    QProcess::startDetached( m_editor, QStringList( m_model->filePath( currentIndex ) ) );
}

void MainWindow::slotSpellcheck()
{
    QModelIndex currentIndex = ui.catalogView->currentIndex();
    if ( !currentIndex.isValid() )
        return;

    //system( "kbabel" );
    //system( "dcop kbabel KBabelIFace spellcheck '" + QFile::encodeName( m_model->filePath( currentIndex ) ) + "'" ); // FIXME stringlist?
}

void MainWindow::slotStatistics()
{
    QModelIndex currIndex = ui.catalogView->currentIndex();

    CatalogEntry entry = m_model->entry( currIndex );

    int untranslated = entry.untranslated;
    int translated = entry.translated;
    int fuzzy = entry.fuzzy;
    int total = entry.total;

    QString message = tr( "Translation statistics for: <em>%1</em><br><br>"
                          "Untranslated: %2 (%3%)<br>"
                          "Translated: %4 (%5%)<br>"
                          "Fuzzy: %6 (%7%)<br>"
                          "Total: %8" ).arg( m_model->fileName( currIndex ) )
                      .arg( untranslated ).arg( formatNumber( ( float )untranslated / total * 100.0 ) )
                      .arg( translated ).arg( formatNumber(  ( float )translated / total * 100.0 ) )
                      .arg( fuzzy ).arg( formatNumber( ( float )fuzzy / total * 100.0 ) )
                      .arg( total );

    QMessageBox::information( this, tr( "Translation Statistics" ), message );
}

QString MainWindow::formatNumber( double num, int prec ) const
{
    return QLocale::system().toString( num, 'g', prec );
}

void MainWindow::slotConfigure()
{
    QString rootdir = QFileDialog::getExistingDirectory( this, tr( "Root Directory" ), QDir::homePath() );

    if ( !rootdir.isEmpty() )
    {
        m_rootdir = rootdir;
        m_model->setRootDir( m_rootdir );
        ui.catalogView->setRootIndex( m_model->rootIndex() );
        updateCaption();
    }
}

void MainWindow::updateCaption()
{
    setWindowTitle( tr( "Catalog Manager - %1" ).arg( m_rootdir ) );
}
