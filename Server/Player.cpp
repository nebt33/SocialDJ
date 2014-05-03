//Player.cpp
#include "Player.h"
#include <iostream>

//Called in the queue when a new song ends.
//If the player stops because there was no song remaining in the queue newSong is 
//used to start playing the next song.
void Player::newSong(const Song *song)
{
	playlist->clear();
	playlist->addMedia(QUrl::fromLocalFile(song->path));
	player->play();
}


//Skips the currently playing song
void Player::next()
{
    playlist->clear();
    player->play();
}


//Resumes play if the music was paused
void Player::play()
{
	player->play();
}


void Player::pause()
{
	player->pause();
}