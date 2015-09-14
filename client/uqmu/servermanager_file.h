#ifndef SERVERMANAGER_FILE_H
#define SERVERMANAGER_FILE_H

#include <QObject>
#include <QDir>
#include "runlog.h"
#include "servermanager.h"

class ServerManager_file : public ServerManager
{
    Q_OBJECT

public:
    ServerManager_file(runLog *log, QDir cachePath, QDir targetPath, QObject *parent = 0);
    void startFetchFileList();
    void startFileDownload(int fileIndex);
    void startFileInstall(int fileIndex);
    void stopAllProcesses();

private:
    void fetchSchemeFileListRecursive(QUrl dirUrl);

private slots:

private:
    bool stopRequested;
};

#endif // SERVERMANAGER_FILE_H
