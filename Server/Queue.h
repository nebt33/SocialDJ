///Queue.h
#include "item.h"
#include <string.h>
#include <map>
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
			client_id submitterID;
			const Song* song;
			std::map<client_id, int> clientsVoted;
			
			QueueObject(client_id id, const Song *s)
			{
				song = s;
				submitterID = id;
				//When a user inserts a new song into the queue, it enters with one vote.
				numVotes = 0;
			}
		};		
		
		std::list<QueueObject> queue;
		Player *connectedPlayer;
		const Song* currentlyPlaying=nullptr;
		std::function<void(const Song*)> top_removed;
		
		void insertSong(const Song *s, client_id submitterID);
		void removeSong(const Song *s);
		int evaluateVote(int increase, const Song *s, client_id submitterID);
				
		Queue(Player *player, std::function<void(const Song*)> top_removed_cb)
		{
			top_removed=top_removed_cb;
			//Connect the queue to the player so that the queue knows when a song ends
			connectedPlayer = player;
			connectedPlayer->playlist->connect(player->player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
		}

		
	public slots:
		void mediaStatusChanged(QMediaPlayer::MediaStatus status)
		{
			//std::cout << "THE SIG STATUS " << status <<  std::endl;
			//Song playing has ended or song has been skipped, pop top song of queue and set currentlyPlaying
			if(status == 7 || status == 1)
			{
				if(currentlyPlaying)
					top_removed(currentlyPlaying);
				if(queue.size() > 0)
				{
					currentlyPlaying = queue.front().song;
					queue.pop_front();
					connectedPlayer->newSong(currentlyPlaying);
				}
				else
				{
					currentlyPlaying = NULL;
					connectedPlayer->playlist->clear();
				}
			}
		}	
};


