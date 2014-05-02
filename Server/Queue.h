//Queue.h
#include "item.h"
#include <string.h>
#include <unordered_set>
#include <list>
#include <cctype>

struct Queue
{
	struct QueueObject
	{
		int numVotes;
		int submitterID;
		const Song* song;
		std::unordered_set<int> clientsVoted;
		
		QueueObject(int id, const Song *s)
		{
			song = s;
			numVotes = 0;
			submitterID = id;
			clientsVoted.insert(id);
		}
	};
	std::list<QueueObject> queue;
	
	const Song* currentlyPlaying;
	
	void insertSong(const Song *s, int submitterID);
	int evaluateVote(int increase, const Song *s, int submitterID);
};

