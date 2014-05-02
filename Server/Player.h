//player.h
/* http://qt-project.org/doc/qt-5/audiooverview.html */

#include <QMediaPlaylist>
#include "Queue.h"


struct Player : public QObject
{	
	private:
		Q_OBJECT
		
	public:
		QMediaPlayer *player;
		QMediaPlaylist *playlist;
		
		Player(Queue *queue)
		{
			player = new QMediaPlayer(0);
			
			playlist = new QMediaPlaylist(0);
			playlist->connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), queue, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
			playlist->addMedia(QUrl::fromLocalFile("C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/12 Elevator.mp3"));
			playlist->addMedia(QUrl::fromLocalFile("C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/07 Head On A Plate.mp3"));
			playlist->setPlaybackMode(QMediaPlaylist::Sequential);
			player->setPlaylist(playlist);

			player->play();
		}	  
		
	public slots:
		void mediaStatusChanged(QMediaPlayer::MediaStatus status)
		{
			//Song playing has ended, pop top song of queue and set currentlyPlaying
			if(status == 8)
			{

			}
		}
};
