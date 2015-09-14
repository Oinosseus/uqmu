#include "servermanager_file.h"
#include <QFileInfo>
#include <QDir>

ServerManager_file::ServerManager_file(runLog *log, QDir cachePath, QDir targetPath, QObject *parent) : ServerManager(log, cachePath, targetPath, parent)
{
    stopRequested = false;
}

void ServerManager_file::startFetchFileList()
{
    if (ServerManager::busy) {
        QString msg = "Function called while busy!";
        ServerManager::myLog->log(__FILE__, __LINE__, "ServerManagerFile::startFetchFileList()()", runLog::sevPanic, msg);
        return;
    }
    ServerManager::busy = true;
    stopRequested = false;
    this->fetchSchemeFileListRecursive(this->url);
    bool success = (myServerFileList.length() > 0) ? true : false;
    ServerManager::busy = false;
    emit finishFetchFileList(success);
}

void ServerManager_file::startFileDownload(int fileIndex)
{
    if (ServerManager::busy) {
        QString msg = "Function called while busy!";
        ServerManager::myLog->log(__FILE__, __LINE__, "ServerManagerFile::startFetchFileList()()", runLog::sevPanic, msg);
        return;
    }
    ServerManager::busy = true;
    stopRequested = false;

    bool success = true;

    QString src = myServerFileList[fileIndex]->getServerUrl().path();
    QString dst = cachePath.path() + QString("/") + myServerFileList[fileIndex]->getRelativeCachePath();

    // create file path
    QFileInfo finfo(dst);
    QDir dir;
    dir.mkpath(finfo.path());

    // remove existing target file
    QFile file(dst);
    if (file.exists()) {
        file.remove();
        file.flush();
    }

    success = QFile::copy(src, dst);
    ServerManager::busy = false;
    emit finishedFileDownload(success);
}



void ServerManager_file::startFileInstall(int fileIndex)
{
    if (ServerManager::busy) {
        QString msg = "Function called while busy!";
        ServerManager::myLog->log(__FILE__, __LINE__, __FUNCTION__, runLog::sevPanic, msg);
        return;
    }
    ServerManager::busy = true;
    stopRequested = false;

    bool success = ServerManager::installFile(fileIndex);
    ServerManager::busy = false;
    emit finishedFileInstall(success);
}

void ServerManager_file::stopAllProcesses()
{
    stopRequested = true;
}

void ServerManager_file::fetchSchemeFileListRecursive(QUrl dirUrl)
{
    QDir dir(dirUrl.path());

    // check sub directory
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (QString entry, dir.entryList()) {
        QUrl entryUrl = dirUrl;
        entryUrl.setPath(dir.path() + QString("/") + entry);
        this->fetchSchemeFileListRecursive(entryUrl);
    }

    // get files in directory
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach (QString entry, dir.entryList()) {
        QUrl entryUrl = dirUrl;
        entryUrl.setPath(dir.path() + QString("/") + entry);

        QFileInfo serverFinfo(entryUrl.path());
        QString relativeCachePath = entryUrl.path().mid(ServerManager::url.path().length());

        // create new file
        ServerFile *file = new ServerFile(this);
        file->setServerUrl(entryUrl);
        file->setServerSize(serverFinfo.size());
        file->setServerTime(serverFinfo.lastModified());
        file->setRelativeCachePath(relativeCachePath);
        file->setNeedsDownload(ServerManager::cachePath);
        ServerManager::myServerFileList.append(file);
    }
}

