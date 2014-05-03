///Queue.h
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
		Player *connectedPlayer;
		const Song* currentlyPlaying;				
					
		Queue(Player *player)
		{
			//Connect the queue to the player so that the queue knows when a song ends
			connectedPlayer = player;
			connectedPlayer->playlist->connect(player->player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

		}
	
		
		void insertSong(const Song *s, int submitterID);
		int evaluateVote(int increase, const Song *s, int submitterID);
		
	public slots:
		void mediaStatusChanged(QMediaPlayer::MediaStatus status)
		{
			//Song playing has ended or song has been skipped, pop top song of queue and set currentlyPlaying
			if(status == 7 || status == 1)
			{
				if(queue.size() > 0)
				{
					currentlyPlaying = queue.front().song;
					queue.pop_front();
					connectedPlayer->newSong(currentlyPlaying);
				}
			}
		}	
};


