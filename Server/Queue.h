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

			
			//test songs
			Song *song1 = new Song(0,0,0,"1");
			song1->path = "C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/i can break these cuffs.mp3";
			Song *song2 = new Song(0,0,0,"2");
			song2->path = "C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/12 Elevator.mp3";
			Song *song3 = new Song(0,0,0,"3");
			song3->path = "C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/07 Head On A Plate.mp3";
			
			QueueObject q1 = QueueObject(1, song1);
			QueueObject q2 = QueueObject(2, song2);
			QueueObject q3 = QueueObject(3, song3);
			
			queue.push_back(q1);
			queue.push_back(q2);
			queue.push_back(q3);
			
			connectedPlayer->next();
			connectedPlayer->next();
		}
	
		
		void insertSong(const Song *s, int submitterID);
		int evaluateVote(int increase, const Song *s, int submitterID);
		
	public slots:
		void mediaStatusChanged(QMediaPlayer::MediaStatus status)
		{
			std::cout << "THE SIG WORKED " << status <<  std::endl;
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


