//player.h
/* http://qt-project.org/doc/qt-5/audiooverview.html */

#include <QMediaPlaylist>
#include <QtMultimedia/QMediaPlayer>
#include <iostream>
#include "item.h"

struct Player : public QObject
{	
	private:
		Q_OBJECT
		
	public:
		QMediaPlayer *player;
		QMediaPlaylist *playlist;
		
		Player()
		{
			player = new QMediaPlayer(0);
			playlist = new QMediaPlaylist(0);
			
			playlist->setPlaybackMode(QMediaPlaylist::Sequential);
			player->setPlaylist(playlist);

			player->play();
		}	  
		
		//Called by the queue when a new song is moved to the top of the queue
		void newSong(const Song *song);
		void next();
		void Play();
		void pause();
};

