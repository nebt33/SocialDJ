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
#include <QProcess>
#include <QRegularExpression>

#include "Database.h"
#include "FolderList.h"
#include "Queue.h"

//keeps track of which songs, albums, and artists the client has been told about; the client can send a message to clear these if forgets everything
struct Client : QObject
{
	private:
		Q_OBJECT
	public:
	std::function<void(Client* c, const QString& args)> handle_message;
	std::unordered_set<id> songs;
	std::unordered_set<id> albums;
	std::unordered_set<id> artists;
	QTcpSocket* socket;
	client_id mac=0;
	
	//returns whether successful
	bool set_mac()
	{
		if(mac != 0)
			return true;
		
		//shell out to the operating system's ARP table utility
		QProcess process;
		process.start(QString("arp -a %1").arg(socket->peerAddress().toIPv4Address()));
		process.waitForFinished(100);
		auto bytes=process.readAllStandardOutput();
		auto str=QString::fromLatin1(bytes);
		auto regex=QRegularExpression(QString("([\\ \\:\\-]([0-9a-f]{2})){6}"));
		auto index=str.indexOf(regex);
		if(index == -1)
			return false;
		mac=0;
		//parse MAC address from matched 18-char string
		int i;
		for(i=index; i<(index+6*3); i++)
		{
			char c=str[i].cell();
			int nibble=-1;
			if(c>='0' && c<='9')
				nibble=c-'0';
			else if(c>='a' && c<='f')
				nibble=c-'a'+10;
			else if(c>='A' && c<='F')
				nibble=c-'A'+10;
			if(nibble != -1)
				mac=(mac<<4)+nibble;
		}
		printf("parsed %llu\n", mac);
		return true;
	}
	
	Client(std::function<void(Client* c, const QString& args)> handler) : handle_message(handler)
	{
	}
	
	bool knows_song(id s)
	{
		return songs.find(s) != songs.end();
	}
	
	void send_song(const Song& s)
	{
		if(knows_song(s.get_id())) return;
		auto new_song_msg=QString("new_song|%1\n").arg(s.get_id());
		auto utf8=new_song_msg.toUtf8();
		socket->write(utf8);
		songs.insert(s.get_id());
	}
	void send_song_info(const Song& s)
	{
		auto info_msg=QString("song_info|%1").arg(s.get_id());
		if(s.duration != 0)
		{
			info_msg+=QString("|duration|%1").arg(s.duration);
		}
		if(s.aid != 0)
		{
			info_msg+=QString("|artist|%1").arg(s.aid);
		}
		if(s.bid != 0)
		{
			info_msg+=QString("|album|%1").arg(s.bid);
		}
		if(s.name != nullptr)
		{
			info_msg+=QString("|name|%1").arg(s.name);
		}
		info_msg+="\n";
		auto utf8=info_msg.toUtf8();
		socket->write(utf8);
	}

	bool knows_album(id b)
	{
		return albums.find(b) != albums.end();
	}

	void send_album(const Album& b)
	{
		if(knows_album(b.get_id())) return;
		auto album_message=QString("new_album|%1\n").arg(b.get_id());
		auto utf8=album_message.toUtf8();
		socket->write(utf8);
		auto info_message=QString("album_info|%1|%2|%3|").arg(b.get_id()).arg(b.name).arg(b.aid);
			unsigned int i;
			auto tracks=b.get_tracks();
			for(i=0; i<b.get_n_tracks(); i++)
			{
				if(tracks[i] == 0) continue;
					info_message+=QString("%1,").arg(tracks[i]);
			}
			info_message+="\n";
		utf8=info_message.toUtf8();
		socket->write(utf8);
		albums.insert(b.get_id());
	}

	bool knows_artist(id a)
	{
		return artists.find(a) != artists.end();
	}

	void send_artist(const Artist& a)
	{
		if(knows_artist(a.get_id())) return;
		auto artist_message=QString("new_artist|%1\n").arg(a.get_id());
		socket->write(artist_message.toUtf8());
		auto info_message=QString("artist_info|%1|%2\n").arg(a.get_id()).arg(a.name);
		socket->write(info_message.toUtf8());
		artists.insert(a.get_id());
	}
	
	public slots:
	void read_lines()
	{
		while(socket->canReadLine())
		{
			handle_message(this, socket->readLine());
		}
	}
};

void send_song_with_deps(Client* c, const Song* s, Database* db);

void send_album_with_deps(Client* c, const Album* b, Database* db)
{
	auto tracks=b->get_tracks();
	unsigned int i;
	printf("n_tracks: %u\n", b->get_n_tracks());
	printf("artist_id: %u\n", b->aid);
	printf("sending tracks: %p\n", tracks);
	for(i=0; i<b->get_n_tracks(); i++)
	{
		if(tracks[i] != 0)
			printf("sending track %u\n", tracks[i]);
		if(!c->knows_song(tracks[i]))
		{
			auto s=db->find_song(tracks[i]);
			if(!s) continue;
			c->send_song(*s);
		}
	}
	c->send_album(*b);
	for(i=0; i<b->get_n_tracks(); i++)
	{
		auto s=db->find_song(tracks[i]);
		if(!s) continue;
		send_song_with_deps(c, s, db);
	}
}

void send_song_with_deps(Client* c, const Song* s, Database* db)
{
	const Artist* a=db->find_artist(s->get_artist());
	const Album* b=db->find_album(s->get_album());
	if(a && !c->knows_artist(a->get_id()))
	{
		c->send_artist(*a);
	}
	c->send_song(*s);
	if(b && !c->knows_album(b->get_id()))
	{
		send_album_with_deps(c, b, db);
	}
	
	c->send_song_info(*s);
}

void send_artist_with_deps(Client* c, const Artist* a, Database* db)
{
	c->send_artist(*a);
}

std::vector<ItemFilter> parse_filters(const QStringList& args, unsigned int* start, unsigned int* length)
{
	std::vector<ItemFilter> filters;
	auto it = args.constBegin();
	*start=(*++it).toInt();
	*length=(*++it).toInt();
	for(++it; it != args.constEnd(); ++it)
	{
		enum MetaItem m;
		if(*it == "artist")
			m=ARTIST;
		else if(*it == "album")
			m=ALBUM;
		else if(*it == "name")
			m=NAME;
		else if(*it == "duration")
			m=DURATION;
		else
			assert(0 && "unknown metaitem in list_* command!");
		++it;
		if(it == args.constEnd())
			break;
		ItemFilter a={m, (*it).toUtf8()};
		filters.push_back(a);
	}
	return filters;
}

void parse_lengths(const QStringList& args, unsigned int* start, unsigned int* length)
{
	std::vector<ItemFilter> filters;
	auto it = args.constBegin();
	*start=(*++it).toInt();
	*length=(*++it).toInt();
}

id parse_id(const QString& s)
{
	return s.toInt();
}

class Server: public QObject
{
	private:
		Q_OBJECT
	public:
		bool quit;
		std::vector<Client*> clients;
		QTcpServer* listen_socket;
		Queue* queue;
		FolderList* folders;
		Database* db;
		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu;
		Player *player;
		
		Server(QObject* parent=nullptr)
		{
			QApplication::setQuitOnLastWindowClosed(false);
			
			db=new Database([&](const Song* s){this->song_updated(s);}, [&](id s){this->song_deleted(s);});
			folders = new FolderList(*db);
			
			folders->initFolderList();
			player = new Player();
			queue = new Queue(player, [&](const Song* s){queue_top_removed(s);});
			
			setParent(parent);
			createTrayIcon();
			trayIcon->show();
			
			quit=false;
			clients=std::vector<Client*>();
			listen_socket=new QTcpServer(this);
			listen_socket->connect(listen_socket, SIGNAL(newConnection()), this, SLOT(client_connected_cb()));
			if(!listen_socket->listen(QHostAddress::Any, 8888))
			{
				QMessageBox::critical(nullptr, "Social DJ", listen_socket->errorString());
				quit=true;
			}
		}
		
		void createTrayIcon()
		{
			 trayIconMenu = new QMenu();
			 
			 auto addDirectoriesAction = new QAction(tr("Add Directories"), this);
			 connect(addDirectoriesAction, SIGNAL(triggered()), this, SLOT(addDirectories()));
			 
			 auto quitAction = new QAction(tr("Exit"), this);
			 connect(quitAction, SIGNAL(triggered()), this, SLOT(quit_cb()));
		
			 trayIconMenu->addAction(addDirectoriesAction);
			 trayIconMenu->addSeparator();
			 trayIconMenu->addAction(quitAction);
			 

			 trayIcon = new QSystemTrayIcon(this);
			 trayIcon->setContextMenu(trayIconMenu);

			 trayIcon->setIcon(QIcon(":/images/icon.png"));
		 }
	public slots:
		void song_updated(const Song* s)
		{
			//add the song to the database; send the song to all quiescent clients
			const Artist* a=db->find_artist(s->get_artist());
			const Album* b=db->find_album(s->get_album());
			
			assert(s);
			
			auto new_song_msg=QString("new_song|%1\n").arg(s->get_id());
			unsigned int i;
			for(i=0; i<clients.size(); i++)
			{
				if(a)
				{
					if(!clients[i]->knows_artist(a->get_id()))
					{
						clients[i]->send_artist(*a);
					}
				}
				if(b)
				{
					if(!clients[i]->knows_album(b->get_id()))
					{
						clients[i]->send_album(*b);
					}
				}
				if(!clients[i]->knows_song(s->get_id()))
				{
					clients[i]->send_song(*s);
				}
				clients[i]->send_song_info(*s);
			}
		}
		
		void song_deleted(id s)
		{
			//send the notification to all quiescent clients
			auto msg=QString("forget_song|%1\n").arg(s).toUtf8();
			unsigned int i;
			for(i=0; i<clients.size(); i++)
			{
				if(clients[i]->knows_song(s))
				{
					clients[i]->socket->write(msg);
					clients[i]->songs.erase(s);
				}
			}
		}
		
		void queue_top_removed(const Song* s)
		{
			//send the notification to all clients
			auto msg=QString("remove_top|%1\n").arg(s->get_id());
			auto utf8=msg.toUtf8();
			unsigned int i;
			for(i=0; i<clients.size(); i++)
			{
				clients[i]->socket->write(utf8);
			}
		}
		
		void quit_cb()
		{
			folders->writeFolders();
			QCoreApplication::quit();
		}
		
		//message handlers:
		
		void queue_add(Client* c, const QStringList& args)
		{
			if(args.size()>1)
			{
				//parse vote|song_id|value
				auto song_id=args[1].toInt();
				auto s=db->find_song(song_id);
				
				//parse queue_add|song_id
				if(s == nullptr)
				{
					return;
				}
				
				//update queue
				queue->insertSong(s, c->mac);
				auto score=queue->getScore(s);
				
				//send all clients the queue modification
				auto add_msg=QString("add_bottom|%1\n").arg(song_id);
				auto add_utf8=add_msg.toUtf8();
				auto score_msg=QString("score|%1|%2\n").arg(song_id).arg(score);
				auto score_utf8=score_msg.toUtf8();
				
				unsigned int i;
				for(i=0; i<clients.size(); i++)
				{
					if(!clients[i]->knows_song(song_id))
					{
						send_song_with_deps(clients[i], s, db);
					}
					clients[i]->socket->write(add_utf8);
					clients[i]->socket->write(score_utf8);
				}
			}
		}
		
		void vote(Client* c, const QStringList& args)
		{
			if(args.size()>2)
			{
				//parse vote|song_id|value
				auto song_id=args[1].toInt();
				auto value=args[2].toInt();
				auto s=db->find_song(song_id);
				
				//parse vote|song_id|value
				if(s == nullptr)
				{
					return;
				}
				
				//update queue
				auto score=queue->evaluateVote(value, s, c->mac);
				
				//send all clients the score modification
				auto score_msg=QString("score|%1|%2\n").arg(song_id).arg(score);
				auto utf8=score_msg.toUtf8();
				
				unsigned int i;
				for(i=0; i<clients.size(); i++)
				{
					clients[i]->socket->write(utf8);
				}
			}
		}
		
		void play(Client* c, const QStringList& args)
		{
			player->play();
			unsigned int i;
			for(i=0; i<clients.size(); i++)
			{
				clients[i]->socket->write("playing\n");
			}
		}
		
		void pause(Client* c, const QStringList& args)
		{
			player->pause();
			unsigned int i;
			for(i=0; i<clients.size(); i++)
			{
				clients[i]->socket->write("paused\n");
			}
		}
		
		void skip(Client* c, const QStringList& args)
		{
			player->next();
			unsigned int i;
			for(i=0; i<clients.size(); i++)
			{
				clients[i]->socket->write("skip\n");
			}
		}
		
		void download_song(Client* c, const QStringList& args)
		{
			//nyi
		}
		
		#define do_list(foo, field, parse) \
			unsigned int start, length;\
			parse;\
			auto matches=db->list_##foo##s(filters, start, length);\
			unsigned int i;\
			for(i=0; i<matches.size(); i++)\
			{\
				printf("sending %s %s\n", #foo, matches[i]->field);\
				send_##foo##_with_deps(c, matches[i], db);\
			}
		
		void list_songs(Client* c, const QStringList& args)
		{
			do_list(song, name, auto filters=parse_filters(args, &start, &length))
		}
		void list_albums(Client* c, const QStringList& args)
		{
			do_list(album, name, auto filters=parse_filters(args, &start, &length))
		}
		void list_artists(Client* c, const QStringList& args)
		{
			if(args.size()>3)
			{
				auto utf8=args[3].toUtf8();
				do_list(artist, name, auto filters=utf8.constData(); parse_lengths(args, &start, &length))
			}
		}
		
		void delete_song(Client* c, const QStringList& args)
		{
			db->delete_song(parse_id(args[1]));
		}
		
		void client_message(Client* c, const QString& line)
		{
			//handle messages from clients
			auto args=line.trimmed().split ('|', QString::KeepEmptyParts);
			#define dispatch(message) if(args[0] == #message) { printf("got " #message "\n");message(c, args); } else
			dispatch(delete_song)
			dispatch(list_songs)
			dispatch(list_albums)
			dispatch(list_artists)
			dispatch(play)
			dispatch(pause)
			dispatch(skip)
			dispatch(vote)
			dispatch(queue_add)
			{}
			#undef dispatch
		}
		
		void client_connected_cb()
		{
			printf("client connected\n");
			auto c=new Client([&](Client* c, const QString& line){printf("got: %s\n", line.toUtf8().constData()); this->client_message(c, line);});
			c->socket=listen_socket->nextPendingConnection();
			c->set_mac();
			clients.push_back(c);
			
			auto send_queue_song=[&](const Song* s, int score)
			{
				auto song_id=s->get_id();
				send_song_with_deps(c, s, db);
				auto add_msg=QString("add_bottom|%1\n").arg(song_id);
				auto utf8=add_msg.toUtf8();
				c->socket->write(utf8);
				auto score_msg=QString("score|%1|%2\n").arg(song_id).arg(score);
				utf8=score_msg.toUtf8();
				c->socket->write(utf8);
			};
			
			if(queue->currentlyPlaying)
			{
				send_queue_song(queue->currentlyPlaying, INT_MAX);
			}
			
			//send the current state of the queue
			for(auto it=queue->queue.begin(); it!=queue->queue.end(); ++it)
			{
				send_queue_song((*it).song, (*it).numVotes);
			}
			
			c->socket->connect(c->socket, SIGNAL(readyRead()), c, SLOT(read_lines()));
		}
		
		void addDirectories()
		{
			folders->add_folders_by_choosing();
		}
};

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	
	Server s(&app);
	app.exec();
	return 0;
}

#include "server.moc"
