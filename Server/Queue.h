//Queue.h
#include "item.h"
#include <string.h>
#include <unordered_set>
#include <list>
#include <cctype>
#include <QtGui>
#include <QtNetwork/QtNetwork>
#include <QMessageBox>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QString>
#include <QTranslator>
#include <QWidget>
#include <iostream>
#include <functional>
#include <assert.h>
#include <QtMultimedia/QMediaPlayer>
#include "Player.h"


struct Queue : public QObject
{
	private:
		Q_OBJECT
    public:
		struct QueueObject
		{
			int numVotes;
			int submitterID;
			const Song* song;
			std::unordered_set<int> clientsVoted;
			
			QueueObject(int id, const Song *s)
			{
				song = s;
				numVotes = 0;
				submitterID = id;
				clientsVoted.insert(id);
			}
		};
		std::list<QueueObject> queue;
		
		Queue(Player *player)
		{
			player->playlist->connect(player->player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
			player->playlist->addMedia(QUrl::fromLocalFile("C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/12 Elevator.mp3"));
			player->playlist->addMedia(QUrl::fromLocalFile("C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/07 Head On A Plate.mp3"));
			player->playlist->setPlaybackMode(QMediaPlaylist::Sequential);
			player->player->setPlaylist(player->playlist);

			player->player->play();
		}
		
		const Song* currentlyPlaying;
		
		void insertSong(const Song *s, int submitterID);
		int evaluateVote(int increase, const Song *s, int submitterID);
		
	public slots:
		void mediaStatusChanged(QMediaPlayer::MediaStatus status)
		{
			std::cout << "THE SIG WORKED " << status <<  std::endl;
			//Song playing has ended, pop top song of queue and set currentlyPlaying
			if(status == 8)
			{
				if(queue.size() > 0)
				{
					currentlyPlaying = queue.front().song;
					queue.pop_front();

				}
			}
		}	
};


