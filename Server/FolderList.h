#include "Database.h"
#include <QWidget> 

//the folderlist watches the filesystem and updates the database when it sees changes
struct FolderList
{
	FolderList(Database&);
	//temporary change to static until there is a working folder list object
	static void add_folders_by_choosing(QWidget* parent);
	void add_folder_by_path(const char* path);
};
