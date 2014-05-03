//Player.cpp
#include "Player.h"
#include <iostream>

void Player::newSong(const Song *song)
{
	playlist->clear();
	playlist->addMedia(QUrl::fromLocalFile(song->path));
	player->play();
}



void Player::next()
{
    playlist->clear();
    player->play();
}


void Player::Play()
{
	player->play();
}


void Player::pause()
{
	player->pause();
}