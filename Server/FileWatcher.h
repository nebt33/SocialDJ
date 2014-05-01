#include <QFileSystemWatcher>

class FileWatcher : public QFileSystemWatcher
{
	private:
		Q_OBJECT
	
	public:
	signals:
		void directoryChanged(const QString &path);
};