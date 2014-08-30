//////////////////////////////////////////////////////////////////////////
// catalogDirModel.h                                                    //
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


#ifndef _CATALOGDIRMODEL_H_
#define _CATALOGDIRMODEL_H_

#include <QDirModel>
#include <QHash>
#include <QFileSystemWatcher>

/**
 * Entry representing a catalog (PO file)
 */
struct CatalogEntry
{
    QString parentDir;
    int translated, fuzzy, untranslated, total;
    QString lastTranslator;
    QString status;
};

/**
 * Model representing a PO files directory structure
 *
 * \sa QDirModel
 */
class CatalogDirModel: public QDirModel
{
    Q_OBJECT
public:
    /**
     * CTOR
     *
     * \param rootDir catalog tree root directory
     */
    CatalogDirModel( const QString & rootDir = QDir::homePath(), QObject * parent = 0 );
    /**
     * \return Returns the catalog tree root directory
     */
    QString rootDir() const;
    /**
     * Sets the catalog tree root directory to @p rootDir
     */
    void setRootDir( const QString & rootDir = QDir::homePath() );
    /**
     * \return Index of the root directory as a QModelIndex
     */
    QModelIndex rootIndex() const;
    /**
     * \return a CatalogEntry for the given @p index
     */
    CatalogEntry entry( const QModelIndex & index ) const;

protected:
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    //virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

public slots:
    void reload();

private slots:
    void slotFileChanged( const QString & path );

private:
    /// root directory
    QString m_rootDir;
    /// output from msgfmt --statistics for @p file
    QString msgStats( const QString & file ) const;
    /// last translator of @p filename
    QString lastTranslator( const QString & filename ) const;
    /// subversion status of @p filename
    QString svnStatus( const QString & filename ) const;
    /// cache structure containing catalog entries as values and a qHash(filename) as keys
    mutable QHash<qint64, CatalogEntry> m_catalogHash;
    /// file system watcher
    QFileSystemWatcher * m_watcher;
};

#endif
