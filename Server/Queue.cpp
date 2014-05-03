#include <assert.h>
#include "Queue.h"
//CLIENT STILL NEEDS A SUBMITTER ID


//Sort function for the queue, highest votes moved to the front of the queue
bool compareVotes(Queue::QueueObject &first, Queue::QueueObject &second)
{
    if(first.numVotes >= second.numVotes)
        return true;
    else
		return false;
}

void Queue::insertSong(const Song *s, int submitterID)
{
	bool exists = false;
	QueueObject newSong = QueueObject(submitterID, s);
	
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
		evaluateVote(true, s, submitterID);
	}
}


int Queue::evaluateVote(int increase, const Song *s, int submitterID)
{
	int score = 0;
	QueueObject* currentSong = nullptr;
	//Find the queueObject that corresponds to the song being voted on
	for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
	{
	  if(it->song == s)
	      currentSong = &*it;
	}
	
	assert(currentSong != nullptr);
	
	//See if the submitter of this vote has voted for this song already
    std::unordered_set<int>::const_iterator clientVoted = currentSong->clientsVoted.find(submitterID);
	
	//Only increase the votes if the song is not the song that is currently playing and if the client has not
	//already voted for this song
	if(clientVoted != currentSong->clientsVoted.end() && currentSong->song != currentlyPlaying)
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
				currentSong->clientsVoted.insert(submitterID);
			}
		}
	}
	
	return score;
}