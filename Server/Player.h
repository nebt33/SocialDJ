//player.h
/* http://qt-project.org/doc/qt-5/audiooverview.html */

#include <QMediaPlaylist>
#include <QtMultimedia/QMediaPlayer>

struct Player : public QObject
{	
	private:
		Q_OBJECT
		
	public:
		QMediaPlayer *player;
		QMediaPlaylist *playlist;
		
		Player()
		{
			player = new QMediaPlayer(0);
			
			playlist = new QMediaPlaylist(0);
		}	  
		
	public slots:
		//void newTop(Song *newTop)
		///{
		 // std::cout << "IN THE NEWTOP" << std::endl;
		//}
};

