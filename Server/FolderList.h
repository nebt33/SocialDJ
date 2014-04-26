#include "Database.h"

//the folderlist watches the filesystem and updates the database when it sees changes
struct FolderList
{
	FolderList(Database&);
	void add_folders_by_choosing();
	void add_folder_by_path(const char* path);
};

