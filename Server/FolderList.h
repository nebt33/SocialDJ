#include "Database.h"
#include "FileWatcher.h"
#include <QWidget> 
#include <QFileSystemWatcher>

//the folderlist watches the filesystem and updates the database when it sees changes
struct FolderList
{
	FolderList(Database& thedb);
	//temporary change to static until there is a working folder list object
	void add_folders_by_choosing(QWidget* parent);
	void add_folder_by_path(const char* path);
	void initFolderList();
	
	
	FileWatcher fileWatcher;
	Database* db;
};
