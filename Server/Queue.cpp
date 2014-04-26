#include "Queue.h"

void insertSong(const Song *s, int submitterID)
{
	bool exists;
	QueueObject newSong = QueueObject(submitterID, s);
	
	for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
	{
	  if(strcmp(it->title, s->title))
	      exists = true;
	}
	if(!exists)
	{
		for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
		{
			bool inserted = false;
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

void evaluateVote(bool increase, const Song *s, int submitterID)
{
	for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
	{
	  if(it->song->id == s->id)
	      QueueObject currentSong = *(it);
	}
	
	//See if the submitter of this vote has voted for this song already
    std::unordered_set<int>::const_iterator got = currentSong.clientsVoted.find(submitterID);
	
	if(got != currentSong.clientsVoted.end())
	{
		for(std::list<QueueObject>::iterator it = queue.begin(); it != queue.end(); it++)
		{
			if(strcmp(it->title, currentSong.song->title))
			{
			    if(increase)
				    it->numVotes++;
			    else
				    it->numVotes--;
				
				clientsVoted.insert(submitterID);
			}
		}
	}
}