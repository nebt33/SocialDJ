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
#define ID3LIB_LINKOPTION 1
#include <id3/tag.h>

FolderList::FolderList(Database& thedb)
{
	db = &thedb;
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
		QFile file("C:/Users/Jacob/Documents/rolla/cs397/SocialDJ/folders.txt");
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream out(&file);
		
		QStringList dirs = dialog.selectedFiles();
		QStringList::const_iterator it;
		for (it = dirs.begin(); it != dirs.end(); it++)
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
			//TODO: move file writing to some onQuit() method when the dialog quitting is fixed
			for(int i = 0; i < results.size(); i++)
			{
				out<<results[i]<<"\n";
			}
		}
		file.close();
		QMessageBox::information(0, QString("Social DJ"), QString("Done Adding Folders"));
	}
}

void FolderList::initFolderList()
{
	QFile file("C:/Users/Jacob/Documents/rolla/cs397/SocialDJ/folders.txt");
	if( file.open(QIODevice::ReadOnly) )
	{
		QTextStream in(&file);

		while(!in.atEnd()) {
			QString line = in.readLine();    
			fileWatcher.addPath(line);
			
			//check folder for .mp3 files and add them to the database
			QDir *dir = new QDir(line);
			if(dir)
			{
				QStringList files = dir->entryList(QDir::Files);
				for(int i = 0; i < files.size(); i++) 
				{
					if(files[i].endsWith(".mp3"))
					{
						const char* song = NULL;
						const char* album = NULL;
						const char* artist = NULL;
						unsigned int index = 0;
						unsigned int duration = 0;
						//get id3 info and add song to database
						ID3_Tag tag((dir->absolutePath() + "/" + files[i]).toUtf8().constData());
						if(tag.NumFrames() > 0)
						{
							//get song title
							ID3_Frame* frame = tag.Find(ID3FID_TITLE);
							if( NULL != frame)
							{
								ID3_Field* field = frame->GetField(ID3FN_TEXT);
								song = field->GetRawText();
							}
							qDebug() << "title: "<<song;
							//get album name
							frame = tag.Find(ID3FID_ALBUM);
							if( NULL != frame)
							{
								ID3_Field* field = frame->GetField(ID3FN_TEXT);
								album = field->GetRawText();
							}
							qDebug() << "album: "<<album;
							//get artist name
							frame = tag.Find(ID3FID_LEADARTIST);
							if( NULL != frame)
							{
								ID3_Field* field = frame->GetField(ID3FN_TEXT);
								artist = field->GetRawText();
							}
							qDebug() << "artist: "<<artist;
							//get song index on album
							frame = tag.Find(ID3FID_TRACKNUM);
							if( NULL != frame)
							{
								ID3_Field* field = frame->GetField(ID3FN_TEXT);
								const char* temp = field->GetRawText();
								sscanf(temp, "%d/",&index);
							}
							qDebug() << "index: "<<index;
							qDebug() << "\n";
						}
						if(song == NULL)
						{
							song = files[i].toUtf8().constData();
						}
						
						//id artistId = db->add_artist(artist);
						//id albumId = db->add_album(album);
						//id newId = db->add_song();
						//db->update_song(newId, song, artistId, albumId, index, duration);
					}
				}
			}
		}

		file.close();
	}
}