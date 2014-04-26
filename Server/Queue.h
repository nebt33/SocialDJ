//Queue.h



typedef struct
{
	class CompareSongs
	{
	    public:
		    bool operator()(Song& song1, Song& song2)
		    {
			    if (song1.numVotes >= song2.numVotes) 
					return true;
				else
					return false;
			}
	};

	priority_queue<Song, vector<Song>, CompareSongs> queue;
}
Queue;


void insertSong(Song