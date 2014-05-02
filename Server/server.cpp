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

#include "item.h"
#include "Database.h"
#include "FolderList.h"
#include "Queue.h"
#include "Player.h"

//keeps track of which songs, albums, and artists the client has been told about; the client can send a message to clear these if forgets everything
typedef struct
{
	std::unordered_set<id> songs;
	std::unordered_set<id> albums;
	std::unordered_set<id> artists;
	QTcpSocket* socket;
	
	bool knows_song(id s)
	{
		return songs.find(s) != songs.end();
	}

	void send_song(const Song& s)
	{
		if(knows_song(s.get_id())) return;
		auto new_song_msg=QString("new_song|%d\n").arg(s.get_id());
		socket->write(new_song_msg.toUtf8());
		songs.insert(s.get_id());
	}
	void send_song_info(const Song& s)
	{
		/*
		"song_info|%d"
		"|duration|%d"
		"|artist|%d"
		"|album|%d"
		"|title|%s"
		"\n";
		*/
	}

	bool knows_album(id b)
	{
		return albums.find(b) != albums.end();
	}

	void send_album(const Album& b)
	{
		if(knows_album(b.get_id())) return;
		auto album_message=QString("new_album|%d\n").arg(b.get_id());
		socket->write(album_message.toUtf8());
		auto info_message=QString("album_info|%d|%s|").arg(b.get_id()).arg(b.name);
			unsigned int i;
			for(i=0; i<b.get_n_tracks(); i++)
			{
				info_message+=QString("%d,").arg(b.get_tracks()[i]);
			}
			info_message+="\n";
		socket->write(info_message.toUtf8());
		albums.insert(b.get_id());
	}

	bool knows_artist(id a)
	{
		return artists.find(a) != artists.end();
	}

	void send_artist(const Artist& a)
	{
		if(knows_artist(a.get_id())) return;
		auto artist_message=QString("new_artist|%d\n").arg(a.get_id());
		socket->write(artist_message.toUtf8());
		auto info_message=QString("artist_info|%d|%s\n").arg(a.get_id()).arg(a.name);
		socket->write(info_message.toUtf8());
		artists.insert(a.get_id());
	}
}
Client;

void send_song_with_deps(Client* c, Song* s, Database* db)
{
	const Artist* a=db->find_artist(s->get_artist());
	const Album* b=db->find_album(s->get_album());
	if(!c->knows_artist(a->get_id()))
	{
		c->send_artist(*a);
	}
	c->send_song(*s);
	if(!c->knows_album(b->get_id()))
	{
		unsigned int i;
		for(i=0; i<b->get_n_tracks(); i++)
		{
			id n=b->get_tracks()[i];
			if(!c->knows_song(n))
			{
				c->send_song(*db->find_song(n));
			}
		}
		c->send_album(*b);
	}
	if(!c->knows_song(s->get_id()))
	{
		c->send_song(*s);
	}
	c->send_song(*s);
}

void send_album_with_deps(Client* c, Album* b, Database* db)
{
	auto tracks=b->get_tracks();
	unsigned int i;
	for(i=0; i<b->get_n_tracks(); i++)
	{
		if(!c->knows_song(tracks[i]))
		{
			c->send_song(*db->find_song(tracks[i]));
		}
	}
	for(i=0; i<b->get_n_tracks(); i++)
	{
		if(!c->knows_song(tracks[i]))
		{
			c->send_song(*db->find_song(tracks[i]));
		}
	}
}

void send_artist_with_deps(Client* c, Artist* a, Database* db)
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
		else if(*it == "title")
			m=TITLE;
		else if(*it == "duration")
			m=DURATION;
		else
			assert(0 && "unknown metaitem in list_* command!");
		ItemFilter a={m, (*++it).toUtf8().constData()};
		filters.push_back(a);
	}
	return filters;
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
		Queue queue;
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
			 
			 auto addDirectoriesAction = new QAction(tr("Select Directories"), this);
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
			
			assert(s && a && b);
			
			auto new_song_msg=QString("new_song|%d\n").arg(s->get_id());
			unsigned int i;
			for(i=0; i<clients.size(); i++)
			{
				if(!clients[i]->knows_artist(a->get_id()))
				{
					clients[i]->send_artist(*a);
				}
				if(!clients[i]->knows_album(b->get_id()))
				{
					clients[i]->send_album(*b);
				}
				if(!clients[i]->knows_song(s->get_id()))
				{
					clients[i]->socket->write(new_song_msg.toUtf8());
				}
				clients[i]->send_song(*s);
			}
		}
		void song_deleted(id s)
		{
			//send the notification to all quiescent clients
			auto msg=QString("").sprintf("forget_song|%d\n", s).toUtf8();
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
		void quit_cb()
		{
			folders->writeFolders();
			QCoreApplication::quit();
		}
		void client_connected_cb()
		{
			printf("called\n");
			auto c=new Client();
			c->socket=listen_socket->nextPendingConnection();
			clients.push_back(c);
			
			
			c->socket->connect(c->socket, SIGNAL(readyRead()), this, SLOT(client_read_cb));
			auto socket=c->socket;
			socket->write("new_song|1\n");
			socket->write("new_artist|1\n");
			socket->write("artist_info|1|The Physics\n");
			socket->write("new_album|1\n");
			socket->write("album_info|1|High Society EP|1\n");
			socket->write("song_info|1|album|1|artist|1|duration|173.2\n");
			socket->write("song_info|1|title|The Session\n");
			socket->write("add_bottom|1\n");
			socket->write("score|1|4\n");
			socket->write("new_song|2\n");
			socket->write("new_artist|2\n");
			socket->write("artist_info|2|Miami Horror\n");
			socket->write("song_info|2|artist|2|duration|251.1|title|Sometimes\n");
			socket->write("add_bottom|2\n");
		}
		void client_read_cb(Client* c)
		{
			while(c->socket->canReadLine())
			{
				client_message(c, c->socket->readLine());
			}
		}
		
		void download_song(Client* c, const QStringList& args)
		{
			//nyi
		}
		
		#define do_list(foo, parse) \
			unsigned int start, length;\
			parse;\
			auto matches=db->list_##foo##s(filters, start, length);\
			unsigned int i;\
			for(i=0; i<matches.size(); i++)\
			{\
				send_##foo##_with_deps(c, matches[i], db);\
			}
		
		void list_songs(Client* c, const QStringList& args)
		{
			do_list(song, auto filters=parse_filters(args, &start, &length))
		}
		void list_albums(Client* c, const QStringList& args)
		{
			do_list(album, auto filters=args[3].toUtf8().constData())
		}
		void list_artists(Client* c, const QStringList& args)
		{
			do_list(artist, auto filters=args[3].toUtf8().constData())
		}
		
		void delete_song(Client* c, const QStringList& args)
		{
			db->delete_song(parse_id(args[1]));
		}
		
		void client_message(Client* c, const QString& line)
		{
			//handle messages from clients
			auto args=line.split ('|', QString::KeepEmptyParts);
			#define dispatch(message) if(args[0] == #message) { message(c, args); } else
			dispatch(delete_song)
			dispatch(list_songs)
			dispatch(list_albums)
			dispatch(list_artists)
			{}
			#undef dispatch
		}
		
		void addDirectories()
		{
			folders->add_folders_by_choosing((QWidget*)trayIconMenu);
		}
		
};

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	
	auto s=new Server(&app);
	//folders.add_folder_by_choosing();
	app.exec();
	return 0;
	while(!s->quit)
	{
		//select(listen_socket, sockets, qt)
		//listen_socket->exec();
		/*switch(source)
		{
			case qt:
				if(message==do_choose)
					folders.add_folder_by_choosing();
				break;
			case listen_clients:
					client_sockets.add_client()
				break;
			case client_sockets:
				Socket active_client=message.sender;
				switch(message)
				{
					case start_download:
						//do that
					case play:
					case pause:
					case skip:
						//pass to queue
						break;
					case search_songs:
						folders.search(message.query);
						break;
					case search_albums:
						folders.search(message.query);
						break;
				}
				break;
		}*/
	}
	return 0;
}

#include "server.moc"
