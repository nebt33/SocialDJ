#include "Database.h"
#include "FileWatcher.h"
#include <QWidget> 
#include <QFileSystemWatcher>
#include <QStringList>

//the folderlist watches the filesystem and updates the database when it sees changes
struct FolderList : public QObject
{
	private:
		Q_OBJECT
	public:
		FolderList(Database& thedb);
		void add_folders_by_choosing(QWidget* parent);
		void add_folders(QStringList paths);
		void initFolderList();
		void scanDirs(QStringList dirs);
		
		//write
		void writeFolders();
		
		FileWatcher fileWatcher;
		Database* db;
		QString foldersPath;
		
	public slots:
		void rescanDir(const QString &path);
};
