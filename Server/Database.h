#ifndef DATBASE_H
#define DATBASE_H
#include "item.h"
#include <functional>
#include <unordered_map>
#include <map>
#include <cstring>
#include <algorithm>
#include <vector>
#include <assert.h>
#define ID3LIB_LINKOPTION 1
#include <id3/tag.h>
#include <QString>
#include <QByteArray>
#include <QDebug>

#define cmp_class(Type) struct Type##Name\
{\
	bool operator()(const Type* const& a, const Type* const& b)\
	{\
		if(a->name)\
		{\
			if(b->name)\
				return strcasecmp(a->name,b->name)<0;\
			else\
				return 1;\
		}\
		else\
		{\
			if(b->name)\
				return 0;\
			else\
				return 1;\
		}\
	}\
}

//cmp_class(Song);
//cmp_class(Artist);
//cmp_class(Album);

struct StrSort
{
	bool operator()(const char* const& a, const char* const& b)
	{
		if(a)
		{
			if(b)
				return strcasecmp(a,b)<0;
			else
				return 1;
		}
		else
		{
			if(b)
				return 0;
			else
				return 1;
		}
	}
};

enum MetaItem
{
	ARTIST,
	ALBUM,
	NAME,
	DURATION,
};

struct ItemFilter
{
	enum MetaItem field;
	QByteArray value;
};

//the database will have its add_*, update_song, and delete_song methods called by the server whenever the FolderList sees a change in the set of songs on disk.
//it will call updated_cb after a song and its album/artist have been updated, and will call deleted_cb after a song has been deleted.
struct Database
{
	std::function<void(const Song*)> updated_cb;
	std::function<void(id)> deleted_cb;
	Database(std::function<void(const Song*)> updated, std::function<void(id)> deleted)
	{
		this->updated_cb=updated;
		this->deleted_cb=deleted;
	};//functions to call when a song is updated or deleted
	
		#define lookup(in)\
			if(n == 0) return NULL;\
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
				albums.erase(a->name);
				album_ids.erase(ait);
				delete a;
			}
		}
		songs.erase(s->name);
		
		//TODO: if the song's artist reaches 0 tracks, delete it
		
		delete s;
		deleted_cb(n);
	};
	
	//may be NULL, returns the file data for playback... maybe this should return a FILE* or other readable interface?
	const char* get_song_data(id n)
	{
		const Song* s=find_song(n);
		if(!s) return NULL;
	};
	
	//creates the album if it doesn't exist, and returns the id for an album with that name
	id add_album(id artist_id, const char* name)
	{
		if(!name) return 0;
		auto it=albums.find(name);
		if(it != albums.end())
			return std::get<1>(*it);
		++album_id;
		auto b=new Album(album_id, artist_id, strdup(name));
		album_ids[album_id]=b;
		albums[b->name]=album_id;
		return album_id;
	};
	
	//creates the artist if it doesn't exist, and returns the id for an artist with that name
	id add_artist(const char* name)
	{
		if(!name) return 0;
		auto it=artists.find(name);
		if(it != artists.end())
			return std::get<1>(*it);
		++artist_id;
		auto a=new Artist(artist_id, strdup(name));
		artist_ids[artist_id]=a;
		artists[a->name]=artist_id;
		return artist_id;
	};
	
	//creates a new song with only a filename
	id add_song(const char* path)
	{
		++song_id;
		printf("added song %u\n", song_id);
		auto s=new Song(song_id, 0, 0, nullptr, path);
		song_ids[song_id]=s;
		return song_id;
	};
	
	//fill in song info, maybe replacing old info
	void update_song(id which, const char* name, id artist, id album, unsigned int album_index, unsigned int duration)
	{
		Song* s=find_song(which);
		if(!s)
		{
			assert(0 && "updating nonexistent song!");
			return;
		}
		
		if(s->name)
		{
			songs.erase(s->name);
			delete s->name;
			s->name=nullptr;
		}
		s->name=strdup(name);
		songs[s->name]=song_id;
		s->aid=artist;
		s->bid=album;
		
		Album* b=find_album(s->bid);
		if(b)
		{
			b->set_id_at(album_index, which);
		}
		updated_cb(s);
	};
	
	#define list_filter(Type,type) std::vector<Type*> list_##type##s(std::vector<ItemFilter>& filt, int start, int count)\
	{\
		int index=0;\
		printf("called list_" #type "s %d %d\n", start, count);\
		std::vector<Type*> results;\
		for(auto i=type##s.begin(); i!=type##s.end(); ++i)\
		{\
			auto cand_id=std::get<1>(*i);\
			auto candidate=type##_ids[cand_id];\
			auto matches=true;\
			unsigned int j;\
			for(j=0; j<filt.size(); j++)\
			{\
				if(!matches)\
					break;\
				switch(filt[j].field)\
				{\
					case ARTIST:\
						{/*printf("checking aid %u == %u\n", filt[j].value.toUInt(), candidate->aid);*/\
						matches&=candidate->aid == filt[j].value.toUInt();\
						break;}\
					case ALBUM:\
						{/*printf("checking bid %u == %u\n", filt[j].value.toUInt(), candidate->bid);*/\
						matches&=candidate->bid == filt[j].value.toUInt();\
						break;}\
					case NAME:\
						{matches&=candidate->name && !!strstr(candidate->name, filt[j].value.constData());\
						break;}\
					case DURATION:\
						{/*TODO: nyi*/\
						matches&=false;\
						break;}\
				}\
			}\
			if(matches && (index>=start && (count == 0 || index<start+count)))\
				results.push_back(candidate);\
			if(matches)\
				index++;\
		}\
		return results;\
	}

#define list_simple(Type,type) std::vector<Type*> list_##type##s(const char* query, int start, int count)\
{\
	std::vector<Type*> results;\
	int index=0;\
	for(auto i=type##s.begin(); i!=type##s.end(); ++i)\
	{\
		auto val_id=std::get<1>(*i);\
		auto value=type##_ids[val_id];\
		if(!!strstr(value->name, query))\
		{\
			if(index>=start && (count == 0 || index<start+count))\
				results.push_back(value);\
			index++;\
		}\
	}\
	return results;\
}

	list_filter(Song,song)
	list_filter(Album,album)
	list_simple(Artist,artist)
	
	std::unordered_map<id,Song*> song_ids;
	std::map<const char*,id,StrSort> songs;
	id song_id=0;
	std::unordered_map<id,Album*> album_ids;
	std::map<const char*,id,StrSort> albums;
	id album_id=0;
	std::unordered_map<id,Artist*> artist_ids;
	std::map<const char*,id,StrSort> artists;
	id artist_id=0;
};
#endif
