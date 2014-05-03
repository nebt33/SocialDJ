#ifndef SONG_H
#define SONG_H
#include <vector>
#include <stdlib.h>
#include <string.h>

//the zero ID refers to a nonexistent object. Artist, Album, and Song IDs are in separate namespaces such that Song 1 and Album 1 have no a priori dependence.
typedef unsigned int id;
typedef long long client_id;

struct Album
{
	Album(id b, id artist, const char* album_name){bid=b; artist_id=artist; name=strdup(album_name);};
	~Album() {free(name);}
	void add_track(id t){};

	const char* get_name() const {return this->name;};//may be NULL, e.g. if the album title is unknown
	unsigned int get_n_tracks() const {return this->tracks.size();};//returns how many tracks the album has; if we don't have all tracks on the album this would be the highest known track number
	const id* get_tracks() const {return this->tracks.data();};//returns an array of track IDs, with 0s in the spots where no track is known
	id get_id() const {return this->bid;};
	
	void set_id_at(unsigned int at, id which)
	{
		while(tracks.size() < at+1)
		{
			tracks.push_back(0);
		}
		tracks[at]=which;
	}
	
	id bid;
	id artist_id;
	char* name=nullptr;
	std::vector<id> tracks;
};

struct Artist
{
	Artist(id a, const char* artist_name){aid=a; name=strdup(artist_name);};
	~Artist() {free(name);}
	const char* get_name() const {return this->name;};
	id get_id() const {return this->aid;};
	
	id aid;
	char* name=nullptr;
};

struct Song
{
	Song(id s, id album_id, id artist_id, const char* song_title, const char* path){sid=s; album=album_id; artist=artist_id; title=song_title?strdup(song_title):nullptr; path=strdup(path);};
	~Song(){free(title);free(path);}
	void set_duration(unsigned int tenths){this->duration = tenths;}
	
	id get_album() const {return this->album;};//may be 0
	id get_artist() const {return this->artist;};//may be 0
	const char* get_title() const {return this->title;};
	unsigned int get_duration() const {return this->duration;};//may be 0 if unknown, integer is in tenths of a second
	id get_id() const {return this->sid;};
	
	id sid;
	id album;
	id artist;
	char* title=nullptr;
	unsigned int duration=0;
	char* path;
};
#endif
