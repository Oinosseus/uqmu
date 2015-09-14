#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QList>
#include <QDir>
#include <QDateTime>
#include <QUrl>
#include "runlog.h"
#include "networkaccessmanager.h"
#include "serverfile.h"

class ServerManager : public QObject
{
    Q_OBJECT
public:
    explicit ServerManager(runLog *log, QDir cachePath, QDir targetPath, QObject *parent = 0);
    void setUrl(const QUrl &url, int timeout);
    QUrl getUrl();
    QList<ServerFile*> getServerFileList();
    bool isBusy();
    virtual void startFetchFileList() = 0;
    virtual void startFileDownload(int fileIndex) = 0;
    virtual void startFileInstall(int fileIndex) = 0;
    virtual void stopAllProcesses() = 0;

signals:
    void finishFetchFileList(bool success);
    void fileDownloadProgress(int percent, qint64 bytes);
    void finishedFileDownload(bool success);
    void fileInstallProgress(int percent);
    void finishedFileInstall(bool success);

protected:
    bool checkNetReply(QNetworkReply *rply);
    bool receiveCacheFileData(int fileIndex, const QByteArray &data, bool finished);
    bool installFile(int fileIndex);
    runLog *myLog;
    QDir cachePath;
    QDir targetPath;
    QUrl url;
    int timeout;
    bool busy;
    QList<ServerFile*> myServerFileList;
};

#endif // SERVERMANAGER_H
