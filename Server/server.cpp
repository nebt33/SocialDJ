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

typedef struct
{
}
Queue;

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
		//TODO: send song messages
		songs.insert(s.get_id());
		"song_info|%d"
		"|duration|%d"
		"|artist|%d"
		"|album|%d"
		"|title|%s"
		"\n";
		//socket->write();
	}

	bool knows_album(id b)
	{
		return albums.find(b) != albums.end();
	}

	void send_album(const Album& b)
	{
		//TODO: send album messages
		"album_info|%d|%s|%d,%d,%d...\n";
		albums.insert(b.get_id());
	}

	bool knows_artist(id a)
	{
		return artists.find(a) != artists.end();
	}

	void send_artist(const Artist& a)
	{
		//TODO: send artist messages
		"artist_info|%d|%s\n";
		artists.insert(a.get_id());
	}
}
Client;

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
	    QMediaPlayer *player;

		Server(QObject* parent=nullptr)
		{
			db=nullptr;//new Database([&](const Song* s){this->song_updated(s);}, [&](id s){this->song_deleted(s);});
			folders= nullptr;// new FolderList(*db);
			
			std::cout << "HELLLO" << std::endl;
			player = new QMediaPlayer(0);
			//player->setMedia(QUrl::fromLocalFile("C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/07 Head On A Plate.mp3"));
			//player->play();
	
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
		
			 trayIconMenu->addAction(addDirectoriesAction);
			 //trayIconMenu->addSeparator();

			 trayIcon = new QSystemTrayIcon(this);
			 trayIcon->setContextMenu(trayIconMenu);

			 trayIcon->setIcon(QIcon(":/images/icon.png"));
		 }
	public slots:
		void song_updated(const Song* s)
		{
			//add the song to the database; send the song to all quiescent clients
			const Artist* a
			#ifdef HAVE_IMPL
			=db->find_artist(s->get_artist())
			#endif
			;
			const Album* b
			#ifdef HAVE_IMPL
			=db->find_album(s->get_album())
			#endif
			;
			
			assert(s && a && b);
			
			auto new_song_msg=QString("").sprintf("new_song|%d\n", s->get_id()).toUtf8();
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
					clients[i]->socket->write(new_song_msg);
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
			quit=true;
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

		void list_songs(Client* c, const QStringList& args)
		{
			//nyi
		}
		void list_albums(Client* c, const QStringList& args)
		{
			//nyi
		}
		void list_arists(Client* c, const QStringList& args)
		{
			//nyi
		}

		void delete_song(Client* c, const QStringList& args)
		{
			//db->delete_song(parse_id(args[1]));
		}

		void client_message(Client* c, const QString& line)
		{
			//handle messages from clients
			auto args=line.split ('|', QString::KeepEmptyParts);
			#define dispatch(message) if(args[0] == #message) { message(c, args); } else
			dispatch(delete_song)
			dispatch(list_songs)
			{}
			#undef if_starts_with
		}

		void addDirectories()
		{
			FolderList::add_folders_by_choosing((QWidget*)trayIconMenu);
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
