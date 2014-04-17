#include <QtGui>
#include <QtNetwork/QtNetwork>

/*TODO: import qt, setup qt, write QT main function*/

typedef struct
{
}
Queue;

typedef struct
{
}
FolderList;

typedef struct
{
}
Socket;

class Server: public QObject
{
	private:
		Q_OBJECT
	public:
		Server(QObject* parent=nullptr)
		{
			setParent(parent);
			quit=false;
			client_sockets=std::vector<QTcpSocket*>();
			listen_socket=new QTcpServer(this);
			listen_socket->connect(listen_socket, SIGNAL(newConnection()), this, SLOT(client_connected_cb()));
			if(!listen_socket->listen(QHostAddress::Any, 8888))
			{
				QMessageBox::critical(nullptr, "Social DJ", listen_socket->errorString());
			}
		}
		bool quit;
		std::vector<QTcpSocket*> client_sockets;
		QTcpServer* listen_socket;
	public slots:
		void quit_cb()
		{
			quit=true;
		}
		void client_connected_cb()
		{
			printf("called\n");
			QTcpSocket *client=listen_socket->nextPendingConnection();
			client_sockets.push_back(client);
			client->write("new_song|1\n");
		}
};

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	Queue queue;
	FolderList folders;
	
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
				if(message==do_quit)
					quit=1
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
