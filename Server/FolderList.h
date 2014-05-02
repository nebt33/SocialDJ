#include "Database.h"
#include "FileWatcher.h"
#include <QWidget> 
#include <QFileSystemWatcher>

//the folderlist watches the filesystem and updates the database when it sees changes
struct FolderList
{
	FolderList(Database& thedb);
	void add_folders_by_choosing(QWidget* parent);
	void add_folder_by_path(const char* path);
	void initFolderList();
	
	//write
	void writeFolders();
	
	FileWatcher fileWatcher;
	Database* db;
};
