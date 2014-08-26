//////////////////////////////////////////////////////////////////////////
// catalogDirModel.cpp                                                  //
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


#include "catalogDirModel.h"

#include <QTextStream>
#include <QDateTime>
#include <QDebug>

#include <stdio.h>
#include <QTemporaryFile>

CatalogDirModel::CatalogDirModel( const QString & rootDir, QObject * parent )
    : QDirModel( parent ), m_rootDir( rootDir )
{
    m_watcher = new QFileSystemWatcher( QStringList( m_rootDir ), this );

    setNameFilters( QStringList( "*.po" ) );
    setFilter( QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot );

    connect( m_watcher, SIGNAL( fileChanged( const QString & ) ),
             this, SLOT( slotFileChanged( const QString & ) ) );
}

int CatalogDirModel::columnCount( const QModelIndex & parent ) const
{
    if ( parent.column() > 0 )
        return 0;

    return 7;
}

QVariant CatalogDirModel::data( const QModelIndex & index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    const QString filename = filePath( index );
    const bool isdir = isDir( index );
    const uint key = qHash( filename );
    bool finished = false;

    if ( role == Qt::DisplayRole || role == FileIconRole || role == FilePathRole || role == FileNameRole )
    {
        int fuzzy = 0;
        int untranslated = 0;
        int translated = 0;
        int total = 0;

        if ( !isdir && !m_catalogHash.contains( key ) ) // perform msgfmt stats and insert into cache
        {
            QRegExp rx( "((\\d+) translated message[s]?)?(, )?((\\d+) fuzzy translation[s]?)?(, )?((\\d+) untranslated message[s]?)?" );
            if ( rx.indexIn( msgStats( filename ) ) != -1 )
            {
                CatalogEntry entry;
                entry.parentDir = fileInfo( index ).path();
                entry.translated = rx.cap( 2 ).toInt();
                entry.fuzzy = rx.cap( 5 ).toInt();
                entry.untranslated = rx.cap( 8 ).toInt();
                entry.total = entry.translated + entry.untranslated + entry.fuzzy;
                entry.lastTranslator = lastTranslator( filename );
                entry.status = svnStatus( filename );

                //qDebug() << "Inserting entry " << filename << " with parent dir " << entry.parentDir;
                m_catalogHash.insert( key, entry );
                m_watcher->addPath( filename );
            }
        }
        else if ( isdir ) // calculate totals
        {
            foreach( CatalogEntry entry, m_catalogHash.values() )
            {
                if ( entry.parentDir == filename )
                {
                    fuzzy += entry.fuzzy;
                    untranslated += entry.untranslated;
                    translated += entry.translated;
                    total += entry.total;
                }
            }

            CatalogEntry entry;
            entry.fuzzy = fuzzy;
            entry.translated = translated;
            entry.untranslated = untranslated;
            entry.total = total;
            m_catalogHash.insert( key, entry );
        }

        CatalogEntry cachedEntry = m_catalogHash.value( key );

        if ( cachedEntry.fuzzy == 0 && cachedEntry.untranslated == 0 )
            finished = true;

        if ( index.column() == 0 ) // icons
        {
            if ( role == FileIconRole )
            {
                if ( isdir ) {
                    return QIcon::fromTheme("folder"); // TODO expanded folder icon too
                } else {
                    return finished ? QIcon::fromTheme("flag-green") : QIcon::fromTheme("flag-red");
                }
            }
            if ( role == FilePathRole )
                return filePath( index );
            if ( role == FileNameRole )
                return fileName( index );
        }

        switch ( index.column() ) // data
        {
        case 0:
            return fileName( index );
        case 1:
            return isdir ? fuzzy : cachedEntry.fuzzy;
        case 2:
            return isdir ? untranslated : cachedEntry.untranslated;
        case 3:
            return isdir ? total : cachedEntry.total;
        case 4:
            return cachedEntry.status;
        case 5:
            return fileInfo( index ).lastModified().toString( Qt::SystemLocaleDate );
        case 6:
            return cachedEntry.lastTranslator;
        default:
            qWarning( "data: invalid display value column %d", index.column() );
            return QVariant();
        }
    }

    if ( ( index.column() == 1 || index.column() == 2 || index.column() == 3 )
         && Qt::TextAlignmentRole == role ) // reimp part from QDirModel, align numbers to center
    {
        return Qt::AlignCenter;
    }

    return QDirModel::data( index, role );
}

QVariant CatalogDirModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal )
    {
        if ( role != Qt::DisplayRole )
            return QVariant();
        switch ( section )
        {
        case 0: return tr( "Name" );
        case 1: return tr( "Fuzzy" );
        case 2: return tr( "Untranslated" );
        case 3: return tr( "Total" );
        case 4: return tr( "Status" );
        case 5: return tr( "Last modified" );
        case 6: return tr( "Last translator" );
        default: return QVariant();
        }
    }
    return QDirModel::headerData( section, orientation, role );
}

#if 0
Qt::ItemFlags CatalogDirModel::flags( const QModelIndex & index ) const
{
    Qt::ItemFlags flags = QDirModel::flags( index );
    if ( !isDir( index ) && index.column() == 0 )
        flags |= Qt::ItemIsUserCheckable;
    return flags;
}
#endif

QString CatalogDirModel::rootDir() const
{
    return m_rootDir;
}

void CatalogDirModel::setRootDir( const QString & rootDir )
{
    m_watcher->removePath( m_rootDir );
    m_rootDir = rootDir;
    m_watcher->addPath( m_rootDir );
    m_catalogHash.clear();
    refresh();
}

QModelIndex CatalogDirModel::rootIndex() const
{
    return QDirModel::index( m_rootDir );
}

QString CatalogDirModel::msgStats( const QString & file ) const
{
    QTemporaryFile tmp;
    if ( tmp.open() )
    {
        system( "LC_ALL=C msgfmt --statistics '" + QFile::encodeName( file ) + "' 2> "
                + QFile::encodeName( tmp.fileName() ) );

        QTextStream stream( &tmp );
        const QString result = stream.readAll();
        //qDebug() << "Result for " << file << " is: " << result;
        return result;
    }

    return QString();
}

QString CatalogDirModel::lastTranslator( const QString & filename ) const
{
    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return QString();

    QTextStream in( &file );
    while ( !in.atEnd() )
    {
        const QString line = in.readLine();
        if ( line.contains( "Last-Translator:" ) )
        {
            file.close();
            return line.section( ':', 1 ).remove( "\\n\"" ).simplified();
        }
    }

    file.close();
    return QString();
}

QString CatalogDirModel::svnStatus( const QString & filename ) const
{
    QTemporaryFile tmp;
    if ( tmp.open() )
    {
        system( "LC_ALL=en_US svn status '" + QFile::encodeName( filename ) + "' > "
                + QFile::encodeName( tmp.fileName() ) );

        QTextStream stream( &tmp );
        const QString line = stream.readLine();
        //qDebug() << "SVN status for " << filename << " is: " << line;

        if ( line.startsWith( "M" ) )
            return tr( "Modified" );
        else if ( line.startsWith( "C" ) )
            return tr( "Conflict" );
        else if ( line.startsWith( "A" ) )
            return tr( "Added" );
        else if ( line.startsWith( "D" ) )
            return tr( "Deleted" );
        else if ( line.startsWith( "I" ) )
            return tr( "Ignored" );
        else if ( line.startsWith( "R" ) )
            return tr( "Replaced" );
    }

    return tr( "Up-to-date" );
}

void CatalogDirModel::slotFileChanged( const QString & path )
{
    //qDebug() << "File changed: " << path;
    m_catalogHash.remove( qHash( path ) );
}

CatalogEntry CatalogDirModel::entry( const QModelIndex & index ) const
{
    return m_catalogHash.value( qHash( filePath( index ) ) );
}

void CatalogDirModel::reload()
{
    m_catalogHash.clear();
    refresh();
}
