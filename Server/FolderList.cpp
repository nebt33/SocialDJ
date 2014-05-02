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
		qDebug() <<"opening file";
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
						//qDebug() << files[i];
						//get id3 info and add song to database
						ID3_Tag tag(files[i].toUtf8().constData());
						ID3_Frame* frame = tag.Find(ID3FID_ALBUM);
						if( NULL != frame )
						{
							ID3_Field* field = frame->GetField(ID3FN_TEXT);
							qDebug() << "album: "<<field->GetRawText();
						}
						else
						{
							qDebug() << "null frame";
						}
					}
				}
			}
		}

		file.close();
	}
}