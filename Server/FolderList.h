#include "Database.h"
#include "FileWatcher.h"
#include <QWidget> 
#include <QFileSystemWatcher>
#include <QStringList>

//the folderlist watches the filesystem and updates the database when it sees changes
struct FolderList
{
	FolderList(Database& thedb);
	void add_folders_by_choosing(QWidget* parent);
	void add_folder_by_path(const char* path);
	void initFolderList();
	void scanDirs(QStringList dirs);
	
	//write
	void writeFolders();
	
	FileWatcher fileWatcher;
	Database* db;
};
