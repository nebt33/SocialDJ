#include <assert.h>
#include "Queue.h"


//Sort function for the queue, highest votes moved to the front of the queue
bool compareVotes(Queue::QueueObject &first, Queue::QueueObject &second)
{
	if(first.numVotes >= second.numVotes)
		return true;
	else
		return false;
}

void Queue::insertSong(const Song *s, client_id submitterID)
{
	bool exists = false;
	QueueObject newSong = QueueObject(submitterID, s, 1);
	
	//Checks if the song is already in the queue, if so do not add and call evaluate vote for that song
	for(std::list<QueueObject>::iterator it = this->queue.begin(); it != this->queue.end(); it++)
	{
		if(it->song == s)
			exists = true;
	}
	if(!exists && s != currentlyPlaying)
	{
		queue.push_front(newSong);
		queue.sort(compareVotes);
		
		//If added song is the only song in the queue, remove it and begin playing it
		if(queue.size() == 1 && connectedPlayer->playlist->isEmpty())
		{
			currentlyPlaying = queue.front().song;
			queue.pop_front();
			connectedPlayer->newSong(currentlyPlaying);
		}
	}
	else
	{
		evaluateVote(1, s, submitterID);
	}
}


//Increase is 1 for an upvote and -1 for a downvote
int Queue::evaluateVote(int increase, const Song *s, client_id submitterID)
{
	int score = 0;
	QueueObject* currentSong = nullptr;
	
	//Find the queueObject that corresponds to the song being voted on
	for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
	{
		if(it->song == s)
			currentSong = &*it;
	}
	
	//if the song is not found in the queue, it's currentlyPlaying which has a pseudoscore of 1000000
	if(currentSong == nullptr)
		return 1000000;
	
	//See if the submitter of this vote has voted for this song already
	auto clientVoted = currentSong->clientsVoted.find(submitterID);
	
	
	//If the client has already upvoted or downvoted a song and they have changed their minds
	//This vote counts as two in the oppisite direction
	if(clientVoted != currentSong->clientsVoted.end() && currentSong->clientsVoted.at(submitterID) != increase)
	{
		for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
		{
			if(it->song == currentSong->song)
			{
				if(increase == 1)
					it->numVotes+= 2;
				else
					it->numVotes-= 2;
				
				queue.sort(compareVotes);
				score = it->numVotes;
				currentSong->clientsVoted[submitterID] = increase;
			}
		}
	}
	//Only increase the votes if the song is not the song that is currently playing and if the client has not
	//already voted for this song
	if(clientVoted == currentSong->clientsVoted.end() && currentSong->song != currentlyPlaying)
	{
		for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
		{
			if(it->song == currentSong->song)
			{
				if(increase == 1)
					it->numVotes++;
				else
					it->numVotes--;
				
				queue.sort(compareVotes);
				score = it->numVotes;
				currentSong->clientsVoted[submitterID] = increase;
			}
		}
	}
	
	return score;
}


void Queue::removeSong(const Song *s)
{
	//Checks if the song is already in the queue, if so remove it
	for(std::list<QueueObject>::iterator it = this->queue.begin(); it != this->queue.end(); it++)
	{
		if(it->song == s)
			queue.erase(it);
	}
}
