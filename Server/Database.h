#ifndef DATBASE_H
#define DATBASE_H
#include "item.h"
#include <functional>
#include <unordered_map>
#include <map>
#include <cstring>
#include <algorithm>
#include <assert.h>

#define cmp_class(Name,Type,field) struct Name\
{\
	bool operator()(const Type* const& a, const Type* const& b)\
	{\
		if(a->field)\
		{\
			if(b->field)\
				return strcasecmp(a->field,b->field)<0;\
			else\
				return 1;\
		}\
		else\
		{\
			if(b->field)\
				return 0;\
			else\
				return 1;\
		}\
	}\
}

cmp_class(SongTitle,Song,title);
cmp_class(ArtistName,Artist,name);
cmp_class(AlbumName,Album,name);

//the database will have its add_*, update_song, and delete_song methods called by the server whenever the FolderList sees a change in the set of songs on disk.
//it will call updated_cb after a song and its album/artist have been updated, and will call deleted_cb after a song has been deleted.
struct Database
{
	std::function<void(const Song*)> updated_cb;
	std::function<void(id)> deleted_cb;
	Database(std::function<void(const Song*)> updated_cb, std::function<void(id)> deleted_cb);//functions to call when a song is updated or deleted
	
		#define lookup(in)\
			auto it=in.find(n);\
			if(it == in.end()) return NULL;\
			return std::get<1>(*it);
	
	Song* find_song(id n) const { lookup(song_ids) };//may be NULL
	Album* find_album(id n) const { lookup(album_ids) };//may be NULL
	const Artist* find_artist(id n) const { lookup(artist_ids) };//may be NULL
	void delete_song(id n)
	{
		auto sit=song_ids.find(n);
		assert(sit != song_ids.end());
		auto s=std::get<1>(*sit);
		
		auto ait=album_ids.find(s->get_album());
		if(ait != album_ids.end())
		{
			//if the song's album reaches 0 tracks, delete it
			auto a=std::get<1>(*ait);
			std::remove(a->tracks.begin(), a->tracks.end(), n);
			if(a->get_n_tracks() == 0)
			{
				std::remove(a->tracks.begin(), a->tracks.end(), n);
				albums.erase(a);
				album_ids.erase(ait);
				delete a;
			}
		}
		
		//if the song's artist reaches 0 tracks, delete it
		delete s;
		deleted_cb(n);
	};
	
	//may be NULL, returns the file data for playback... maybe this should return a FILE* or other readable interface?
	const char* get_song_data(id n)
	{
		const Song* s=find_song(n);
		if(!s) return NULL;
	};
	
	//creates the album if it doesn't exist, and returns the id for an album with that title
	id add_album(const char* title)
	{
		++album_id;
		album_ids[album_id]=new Album(album_id, title?strdup(title):nullptr);
		return album_id;
	};
	
	//creates the artist if it doesn't exist, and returns the id for an artist with that name
	id add_artist(const char* name)
	{
		++artist_id;
		artist_ids[artist_id]=new Artist(artist_id, name?strdup(name):nullptr);
		return song_id;
	};
	
	//creates a new song with no info
	id add_song()
	{
		++song_id;
		song_ids[song_id]=new Song(song_id, 0, 0, nullptr);
		return song_id;
	};
	
	//fill in song info, maybe replacing old info
	void update_song(id which, const char* title, id artist, id album, unsigned int album_index, unsigned int duration)
	{
		Song* s=find_song(which);
		if(!s)
		{
			assert(0 && "updating nonexistent song!");
			return;
		}
		
		if(s->title)
		{
			delete s->title;
			s->title=nullptr;
		}
		s->title=strdup(title);
		s->artist=artist;
		s->album=album;
		
		Album* b=find_album(s->album);
		if(b)
		{
			b->set_id_at(album_index, which);
		}
		updated_cb(s);
	};
	
	std::unordered_map<id,Song*> song_ids;
	std::map<Song*,bool,SongTitle> songs;
	id song_id=0;
	std::unordered_map<id,Album*> album_ids;
	std::map<Album*,bool,AlbumName> albums;
	id album_id=0;
	std::unordered_map<id,Artist*> artist_ids;
	std::map<Artist*,bool,ArtistName> artists;
	id artist_id=0;
};
#endif
