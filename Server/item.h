#ifndef SONG_H
#define SONG_H
//the zero ID refers to a nonexistent object. Artist, Album, and Song IDs are in separate namespaces such that Song 1 and Album 1 have no a priori dependence.
typedef unsigned int id;

typedef struct
{
	const char* get_name() const;//may be NULL, e.g. if the album title is unknown
	unsigned int get_n_tracks() const;//returns how many tracks the album has; if we don't have all tracks on the album this would be the highest known track number
	const id* get_tracks() const;//returns an array of track IDs, with 0s in the spots where no track is known
	id get_id() const;
}
Album;

typedef struct
{
	const char* get_name() const;
	id get_id() const;
}
Artist;

typedef struct
{
	id get_album() const;//may be 0
	id get_artist() const;//may be 0
	const char* get_title() const;
	unsigned int get_duration() const;//may be 0 if unknown, integer is in tenths of a second
	id get_id() const;
}
Song;
#endif
