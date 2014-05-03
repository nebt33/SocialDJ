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
#include <QStandardPaths>
#include <queue>


FolderList::FolderList(Database& thedb)
{
	db = &thedb;
	connect(&fileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(rescanDir(QString)));
}

void FolderList::add_folders_by_choosing()
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
	
	//pop up the dialog and add the selected directories to the file watcher
	if( dialog.exec() )
	{
		QStringList dirs = dialog.selectedFiles();
		add_folders(dirs);
	}
}

void FolderList::add_folders(QStringList paths)
{
	QStringList::const_iterator it;
	//for every path given, add it to the file watcher and scan it for files
	for (it = paths.begin(); it != paths.end(); it++)
	{
		QDir *dir = new QDir(QString(*it));
		std::queue<QString> queue;
		queue.push(dir->absolutePath());
		QStringList results;
		//add subdirectories
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
		//for the given path and all its subdirectories, add the path the file watcher
		for(int i = 0; i < results.size(); i++)
		{
			if(-1 == fileWatcher.directories().indexOf(results[i]))
			{
				qDebug()<<"adding new dir: "<<results[i];
				fileWatcher.addPath(results[i]);
				scanDirs(results);
			}
			else
				qDebug()<<"dir already being watched: "<<results[i];
		}
	}
	//let the user know we added the directories in case it took a while
	QMessageBox::information(0, QString("Social DJ"), QString("Done Adding Folders"));
}

static void addSongFromPath(QString dirPath, QString fileName, Database& db)
{
	if(fileName.endsWith(".mp3"))
	{
		const char* song = NULL;
		const char* album = NULL;
		const char* artist = NULL;
		unsigned int index = 0;
		unsigned int duration = 0;
		QString fullPath = dirPath + "/" + fileName;
		//get id3 info and add song to database
		auto pathUtf8 = fullPath.toUtf8();
		ID3_Tag tag(pathUtf8.constData());
		if(tag.NumFrames() > 0)
		{
			//get song title
			ID3_Frame* frame = tag.Find(ID3FID_TITLE);
			if( NULL != frame)
			{
				ID3_Field* field = frame->GetField(ID3FN_TEXT);
				song = field->GetRawText();
			}
			//qDebug()<<"id3 song name: "<<song;
			
			//get album name
			frame = tag.Find(ID3FID_ALBUM);
			if( NULL != frame)
			{
				ID3_Field* field = frame->GetField(ID3FN_TEXT);
				album = field->GetRawText();
			}
			//qDebug()<<"id3 album name: "<<album;
			
			//get artist name
			frame = tag.Find(ID3FID_LEADARTIST);
			if( NULL != frame)
			{
				ID3_Field* field = frame->GetField(ID3FN_TEXT);
				artist = field->GetRawText();
			}
			//qDebug()<<"id3 artist name: "<<artist;
			
			//get song index on album
			frame = tag.Find(ID3FID_TRACKNUM);
			if( NULL != frame)
			{
				ID3_Field* field = frame->GetField(ID3FN_TEXT);
				const char* temp = field->GetRawText();
				if(temp) sscanf(temp, "%d/",&index);
			}
			//qDebug()<<"id3 track number: "<<index;
			
		}
		//if we didn't get a song name from the id3 tag, use the file name
		if(song == NULL)
		{
			song = fileName.toUtf8().constData();
		}
		
		//add the artist to the database
		id artistId;
		if(artist != NULL )
			artistId = db.add_artist(artist);
		else
			artistId = 0;
			
		//add the album to the database
		id albumId;
		if(album != NULL )
			albumId = db.add_album(artistId, album);
		else
			albumId = 0;
		
		printf("song path=%s\n", pathUtf8.constData());
		id theId = db.add_song(pathUtf8.constData());
		
		printf("update_song(%u, %s, %u, %u, %u, %u);\n", theId, song, artistId, albumId, index, duration);
		db.update_song(theId, song, artistId, albumId, index, duration);
	}
}

void FolderList::initFolderList()
{
	QStringList configs = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
	if(configs.size() > 0)
	{
		foldersPath = configs[0];
		QDir dir(foldersPath);
		if(!dir.exists())
		{
			dir.mkpath(foldersPath);
			qDebug()<<"making folders dir";
		}
		foldersPath += "/folders.txt";
	}
	else
		foldersPath = "../../";
	
	
	
	qDebug()<<foldersPath;
	
	QFile file(foldersPath);
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
	QFile file(foldersPath);
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
			addSongFromPath(dir->absolutePath(), files[i], *db);
		}
	}
}

void FolderList::rescanDir(const QString &path)
{
	//add_folders(QStringList(path));
}
