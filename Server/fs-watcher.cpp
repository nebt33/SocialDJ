#include <QFileSystemWatcher>

/*
from <http://qt-project.org/doc/qt-4.8/qfilesystemwatcher.html>:

Public Functions:
QFileSystemWatcher ( QObject * parent = 0 )
QFileSystemWatcher ( const QStringList & paths, QObject * parent = 0 )
~QFileSystemWatcher ()
void	addPath ( const QString & path )
void	addPaths ( const QStringList & paths )
QStringList	directories () const
QStringList	files () const
void	removePath ( const QString & path )
void	removePaths ( const QStringList & paths )
29 public functions inherited from QObject

Signals:
void	directoryChanged ( const QString & path )
void	fileChanged ( const QString & path )
*/

static void watch()
{
	QFileSystemWatcher watcher;
}
