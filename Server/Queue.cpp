#include "Queue.h"
#include <assert.h>

void Queue::insertSong(const Song *s, int submitterID)
{
	bool exists;
	QueueObject newSong = QueueObject(submitterID, s);
	
	for(std::list<QueueObject>::iterator it = this->queue.begin(); it != this->queue.end(); it++)
	{
	  if(it->song == s)
	      exists = true;
	}
	if(!exists)
	{
		bool inserted = false;
		for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
		{
			if(queue.size() == 0)
			{
				queue.push_front(newSong);
				inserted = true;
			}
			if(it->numVotes == 0 || it->numVotes < 0)
			{		
				queue.insert(it, newSong);
				inserted = true;
			}
		}
		if(!inserted)
			queue.push_back(newSong);
	}
	else
	{
		evaluateVote(true, s, submitterID);
	}
}

void Queue::evaluateVote(bool increase, const Song *s, int submitterID)
{
	QueueObject* currentSong = nullptr;
	for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
	{
	  if(it->song == s)
	      currentSong = &*it;
	}
	
	assert(currentSong != nullptr);
	
	//See if the submitter of this vote has voted for this song already
    std::unordered_set<int>::const_iterator got = currentSong->clientsVoted.find(submitterID);
	
	if(got != currentSong->clientsVoted.end())
	{
		for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
		{
			if(it->song == currentSong->song)
			{
			    if(increase)
				    it->numVotes++;
			    else
				    it->numVotes--;
				
				currentSong->clientsVoted.insert(submitterID);
			}
		}
	}
}