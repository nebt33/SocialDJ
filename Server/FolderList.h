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
		/* FolderList constructor
		 * Makes a new FolderList to track folders the user specifies. Attaches the
		 * database and a signal to get notified when a tracked folder changes
		 */
		FolderList(Database& thedb);
		
		/* add_folders_by_choosing()
		 * opens a dialog to let the user choose directories to add to look for
		 * songs in.
		 */
		void add_folders_by_choosing();
		
		/* add_folders(QStringList paths)
		 * adds the specified folders to the FileWatcher for tracking
		 */
		void add_folders(QStringList paths);
		
		/* initFolderList()
		 * reads in folders the user tracked last time the server ran
		 */
		void initFolderList();
		
		/* scanDirs(QStringList dirs)
		 * scans the specified directories and adds .mp3 files found to the database
		 */
		void scanDirs(QStringList dirs);
		
		/* writeFolders()
		 * writes all the tracked folders to a file so they are remembered next 
		 * time the server is opened
		 */
		void writeFolders();
		
		//keeps track of and adds songs from directories the user specifies 
		FileWatcher fileWatcher;
		
		//the database of song information found
		Database* db;
		
		//the absolute path of the file to track folders
		QString foldersPath;
		
		//map of mp3 path to id to track the songs we have added to the database
		//std::unordered_map<QString,id> addedSongs;
		
	public slots:
		//scans a directory and its subdirectories for mp3 files. to be called
		//after a dicrectory was changed
		void rescanDir(const QString &path);
};
