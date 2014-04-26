#ifndef DATBASE_H
#define DATBASE_H
#include "item.h"
#include <functional>

//the database will have its add_*, update_song, and delete_song methods called by the server whenever the FolderList sees a change in the set of songs on disk.
//it will call updated_cb after a song and its album/artist have been updated, and will call deleted_cb after a song has been deleted.
struct Database
{
	Database(std::function<void(const Song*)> updated_cb, std::function<void(id)> deleted_cb);//functions to call when a song is updated or deleted
	const Song* find_song(id n);//may be NULL
	const Album* find_album(id n);//may be NULL
	const Artist* find_artist(id n);//may be NULL
	void delete_song(id n);
	const char* get_song_data(id n);//may be NULL, returns the file data for playback... maybe this should return a FILE* or other readable interface?
	id add_album(const char* title);//creates the album if it doesn't exist, and returns the id for an album with that title
	id add_artist(const char* name);//creates the artist if it doesn't exist, and returns the id for an artist with that name
	id add_song();//creates a new song with no info
	void update_song(id n, const char* title, id artist, id album, unsigned int album_index, unsigned int duration);//fill in song info, maybe replacing old info
};
#endif
