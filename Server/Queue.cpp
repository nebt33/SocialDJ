#include "Queue.h"
#include <assert.h>

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
	bool exists;
	QueueObject newSong = QueueObject(submitterID, s);
	
	for(std::list<QueueObject>::iterator it = this->queue.begin(); it != this->queue.end(); it++)
	{
	  if(it->song == s)
	      exists = true;
	}
	if(!exists && s != currentlyPlaying)
	{
	    queue.push_front(newSong);
	    queue.sort(compareVotes);
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
	for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
	{
	  if(it->song == s)
	      currentSong = &*it;
	}
	
	assert(currentSong != nullptr);
	
	//See if the submitter of this vote has voted for this song already
    std::unordered_set<int>::const_iterator got = currentSong->clientsVoted.find(submitterID);
	
	if(got != currentSong->clientsVoted.end() && currentSong->song != currentlyPlaying)
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