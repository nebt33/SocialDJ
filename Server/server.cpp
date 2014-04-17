#include <QtGui>
#include <QtNetwork/QtNetwork>
#include <unordered_set>

typedef struct
{
}
Queue;

//the zero ID refers to a nonexistent object. Artist, Album, and Song IDs are in separate namespaces such that Song 1 and Album 1 have no a priori dependence.
typedef unsigned int id;

//keeps track of which songs, albums, and artists the client has been told about; the client can send a message to clear these if forgets everything
typedef struct
{
	std::unordered_set<id> songs;
	std::unordered_set<id> albums;
	std::unordered_set<id> artists;
	QTcpSocket* socket;
}
Client;

typedef struct
{
	const char* get_name();//may be NULL, e.g. if the album title is unknown
	unsigned int get_n_tracks();//returns how many tracks the album has; if we don't have all tracks on the album this would be the highest known track number
	const id* get_tracks();//returns an array of track IDs, with 0s in the spots where no track is known
	id get_id();
}
Album;

typedef struct
{
	const char* get_name();
	id get_id();
}
Artist;

typedef struct
{
	id get_album();//may be 0
	id get_artist();//may be 0
	const char* get_title();
	unsigned int get_duration();//may be 0 if unknown, integer is in tenths of a second
	id get_id();
}
Song;

//the database will have its add_*, update_song, and delete_song methods called by the server whenever the FolderList sees a change in the set of songs on disk.
//it will call updated_cb after a song and its album/artist have been updated, and will call deleted_cb after a song has been deleted.
typedef struct
{
	void Database(std::function<void(const Song*)> updated_cb, std::function<void(id)> deleted_cb);//functions to call when a song is updated or deleted
	const Song* find_song(id n);//may be NULL
	const Album* find_album(id n);//may be NULL
	const Artist* find_artist(id n);//may be NULL
	void delete_song(id n);
	const char* get_song_data(id n);//may be NULL, returns the file data for playback... maybe this should return a FILE* or other readable interface?
	id add_album(const char* title);//creates the album if it doesn't exist, and returns the id for an album with that title
	id add_artist(const char* name);//creates the artist if it doesn't exist, and returns the id for an artist with that name
	id add_song();//creates a new song with no info
	void update_song(id n, const char* title, id artist, id album, unsigned int album_index, unsigned int duration);//fill in song info, maybe replacing old info
}
Database;

//the folderlist watches the filesystem and updates the database when it sees changes
typedef struct
{
	void FolderList(Database&){};
	void add_folders_by_choosing();
	void add_folder_by_path(const char* path);
}
FolderList;

class Server: public QObject
{
	private:
		Q_OBJECT
	public:
		bool quit;
		std::vector<Client*> clients;
		QTcpServer* listen_socket;
		Queue queue;
		FolderList folders;
		Server(QObject* parent=nullptr)
		{
			setParent(parent);
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
	public slots:
		void new_song(Song* s)
		{
			//add the song to the database; send the song to all quiescent clients
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
		void client_message(Client* c)
		{
			//handle messages from clients
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
