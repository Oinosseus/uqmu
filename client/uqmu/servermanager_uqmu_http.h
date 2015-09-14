#ifndef SERVERMANAGER_UQMU_HTTP_H
#define SERVERMANAGER_UQMU_HTTP_H

#include <QObject>
#include <QDir>
#include <QNetworkReply>
#include "runlog.h"
#include "servermanager.h"
#include "networkaccessmanager.h"

class ServerManager_uqmu_http : public ServerManager
{
    Q_OBJECT

public:
    ServerManager_uqmu_http(runLog *log, QDir cachePath, QDir targetPath, QObject *parent = 0);
    void startFetchFileList();
    void startFileDownload(int fileIndex);
    void startFileInstall(int fileIndex);
    void stopAllProcesses();

private:
    NetworkAccessManager *myNetMngr;
    int srvVersion;
    int srvTimeDeviation; // server seconds - local seconds
    QString srvLogin;
    QString srvPassword;
    QString srvScheme;
    QNetworkReply *srvFileDownloadReply;
    int srvFileDownloadIndex;
    qint64 dwnldsze;

private slots:
    void slotNetRequestFetchInfo(QNetworkReply*rply);
    void slotNetRequestFileList(QNetworkReply*rply);
    void slotNetRequestFileDownloadFinished(QNetworkReply*rply);
    void slotNetRequestFileDownloadProgress(qint64 received, qint64 total);
};

#endif // SERVERMANAGER_UQMU_HTTP_H
