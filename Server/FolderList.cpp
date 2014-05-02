#include "FolderList.h"
#include <QFileDialog>
#include <QWidget>
#include <QString>
#include <QTranslator>
#include <QListView>
#include <QTreeView>
#include <QAbstractItemView>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QMessageBox>
#include <queue>

FolderList::FolderList(Database& thedb)
{
	db = &thedb;
	connect(&fileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(rescanDir(QString)));
}

//TODO: remove parent parameter if this is the way we do it
void FolderList::add_folders_by_choosing(QWidget* parent)
{
	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setOption(QFileDialog::DontUseNativeDialog,true);
	
	QListView *l = dialog.findChild<QListView*>("listView");
    if (l)
		l->setSelectionMode(QAbstractItemView::MultiSelection);
    QTreeView *t = dialog.findChild<QTreeView*>();
    if (t)
		t->setSelectionMode(QAbstractItemView::MultiSelection);
	
	if( dialog.exec() )
	{
		QStringList dirs = dialog.selectedFiles();
		add_folders(dirs);
	}
}

void FolderList::add_folders(QStringList paths)
{
	QStringList::const_iterator it;
	for (it = paths.begin(); it != paths.end(); it++)
	{
		QDir *dir = new QDir(QString(*it));
		std::queue<QString> queue;
		queue.push(dir->absolutePath());
		QStringList results;
		while (!queue.empty()) {
			auto const subdir_name = queue.front();
			queue.pop();
			results.push_back(subdir_name);
			QDir subdir(subdir_name);
	 
			auto const &directories = subdir.entryInfoList({}, QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
			for (auto const &data : directories) {
				queue.push(data.absoluteFilePath());
			}
		}
		for(int i = 0; i < results.size(); i++)
		{
			fileWatcher.addPath(results[i]);
		}
		scanDirs(results);
	}
	QMessageBox::information(0, QString("Social DJ"), QString("Done Adding Folders"));
}

void FolderList::initFolderList()
{
	QFile file("../../folders.txt");
	if( file.open(QIODevice::ReadOnly) )
	{
		QTextStream in(&file);

		while(!in.atEnd()) {
			QString line = in.readLine();    
			fileWatcher.addPath(line);
			
			//scan the directory for files if it is an existing directory
			QDir *dir = new QDir(line);
			if(dir)
			{
				scanDirs(QStringList(dir->absolutePath()));
			}
		}

		file.close();
	}
}

void FolderList::writeFolders()
{
	QFile file("../../folders.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&file);
	
	QStringList dirs = fileWatcher.directories();
	for(int i = 0; i < dirs.size(); i++)
	{
		out<<dirs[i]<<"\n";
	}
	file.close();
}

void FolderList::scanDirs(QStringList dirs)
{
	for( int i = 0; i < dirs.size(); i ++ )
	{
		QDir *dir = new QDir(dirs[i]);
		QStringList files = dir->entryList(QDir::Files);
		for(int i = 0; i < files.size(); i++) 
		{
			Database::addSongFromPath((dir->absolutePath() + "/" + files[i]).toUtf8().constData(), *db);
		}
	}
}

void FolderList::rescanDir(const QString &path)
{
	qDebug()<<"rescanning dir";
	add_folders(QStringList(path));
}