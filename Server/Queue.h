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
#include <unordered_set>
#include <iostream>
#include <functional>
#include <assert.h>
#include <QtMultimedia/QMediaPlayer>

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
		
		const Song* currentlyPlaying;
		
		void insertSong(const Song *s, int submitterID);
		int evaluateVote(int increase, const Song *s, int submitterID);
		
	public slots:
		void mediaStatusChanged(QMediaPlayer::MediaStatus status)
		{
			//Song playing has ended, pop top song of queue and set currentlyPlaying
			if(status == 8)
			{
				if(queue.size() > 0)
				{
				    //currentlyPlaying = queue.pop_front();
				}
			}
		}
	
};

