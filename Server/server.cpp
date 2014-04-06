/*TODO: import qt, setup qt, write QT main function*/

int main()
{
	Queue queue;
	FolderList folders(queue);
	folders.add_folder_by_choosing();
	
	Socket listen_socket=listen(port#);
	
	Socket client_sockets[];
	bool quit=false;
	while(!quit)
	{
		select(listen_socket, sockets, qt)
		switch(source)
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
		}
	}
	return 0;
}
