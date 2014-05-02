//player.h
/* http://qt-project.org/doc/qt-5/audiooverview.html */

struct Player
{
    QMediaPlayer *player;

    Player()
	{
	    player = new QMediaPlayer(0);
		player->setMedia(QUrl::fromLocalFile("C:/Users/Trey/Documents/cs397/SocialDJ/Server/Server/07 Head On A Plate.mp3"));
		player->play();
	}
   
};
