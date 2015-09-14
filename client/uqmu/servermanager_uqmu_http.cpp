#include "servermanager_uqmu_http.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "dialoguserauth.h"

#include <QDebug>

ServerManager_uqmu_http::ServerManager_uqmu_http(runLog *log, QDir cachePath, QDir targetPath, QObject *parent) : ServerManager(log, cachePath, targetPath, parent)
{
    this->myNetMngr = new NetworkAccessManager(this);
}

void ServerManager_uqmu_http::startFetchFileList()
{
    if (ServerManager::busy) {
        QString msg = "Function called while busy!";
        ServerManager::myLog->log(__FILE__, __LINE__, "ServerManagerFile::startFetchFileList()()", runLog::sevPanic, msg);
        return;
    }
    ServerManager::busy = true;

    srvLogin = "";
    srvPassword = "";

    // replace uqmu-http(s) scheme
    QUrl url = this->url;
    if (url.scheme().toLower() == "uqmu-http") {
        srvScheme = "http";
    } else if (url.scheme().toLower() == "uqmu-https") {
        srvScheme = "https";
    } else {
        this->myLog->log(__FILE__, __LINE__, __FUNCTION__, runLog::sevPanic, "Protocol '" + url.scheme() + "' not supported!");
        emit finishFetchFileList(false);
        return;
    }


    // start request
    url.setScheme(srvScheme);
    connect(this->myNetMngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotNetRequestFetchInfo(QNetworkReply*)));
    this->myNetMngr->get(url);
}

void ServerManager_uqmu_http::startFileDownload(int fileIndex)
{
    if (ServerManager::busy) {
        QString msg = "Function called while busy!";
        ServerManager::myLog->log(__FILE__, __LINE__, "ServerManagerFile::startFetchFileList()()", runLog::sevPanic, msg);
        return;
    }
    ServerManager::busy = true;

    srvFileDownloadIndex = fileIndex;
    dwnldsze = 0;

    // start request
    connect(this->myNetMngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotNetRequestFileDownloadFinished(QNetworkReply*)));
    srvFileDownloadReply = this->myNetMngr->get(myServerFileList[fileIndex]->getServerUrl());
    connect(srvFileDownloadReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(slotNetRequestFileDownloadProgress(qint64,qint64)));
}

void ServerManager_uqmu_http::startFileInstall(int fileIndex)
{
    if (ServerManager::busy) {
        QString msg = "Function called while busy!";
        ServerManager::myLog->log(__FILE__, __LINE__, "ServerManagerFile::startFetchFileList()()", runLog::sevPanic, msg);
        return;
    }
    ServerManager::busy = true;

    bool success = ServerManager::installFile(fileIndex);
    ServerManager::busy = false;
    emit finishedFileInstall(success);
}

void ServerManager_uqmu_http::stopAllProcesses()
{

}

void ServerManager_uqmu_http::slotNetRequestFetchInfo(QNetworkReply *rply)
{
    disconnect(this->myNetMngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotNetRequestFetchInfo(QNetworkReply*)));
    if (!this->checkNetReply(rply)) {
        ServerManager::busy = false;
        emit finishFetchFileList(false);
        return;
    }

    // read server information
    QJsonObject json = QJsonDocument::fromJson(rply->readAll()).object();
    srvVersion = json["ServerVersion"].toInt();

    if (srvVersion == 1) {

        // authorization dialog
        if (json["LoginRequired"].toBool()) {
            DialogUserAuth *authdialog = new DialogUserAuth();
            authdialog->setAttribute(Qt::WA_ShowModal);
            authdialog->exec();
            srvLogin = authdialog->getLogin();
            srvPassword = authdialog->getPassword();
            authdialog->deleteLater();
        }

        // calculate time deviation (server - local)
        unsigned int server_timestamp = json["TimeStamp"].toInt();
        unsigned int local_timestamp  = QDateTime::currentDateTime().toTime_t();
        srvTimeDeviation = server_timestamp - local_timestamp;

        // generate new network request for file list
        QUrl url = this->url;
        url.setQuery("REQUEST_ACTION=SHOW_FILE_LIST&REQUEST_LOGIN=" + srvLogin + "&REQUEST_PASSWORD=" + srvPassword);
        url.setScheme(srvScheme);
        connect(this->myNetMngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotNetRequestFileList(QNetworkReply*)));
        this->myNetMngr->get(url);
        return;

    } else {
        this->myLog->log(__FILE__, __LINE__, __FUNCTION__, runLog::sevPiffle, "Server uqmu-http(s) version '" + QString::number(srvVersion) + " not supported!");
        emit finishFetchFileList(false);
        return;
    }


    // execution should never run to hrere
    emit finishFetchFileList(false);
}

void ServerManager_uqmu_http::slotNetRequestFileList(QNetworkReply *rply)
{
    disconnect(this->myNetMngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotNetRequestFileList(QNetworkReply*)));
    if (!this->checkNetReply(rply)) {
        ServerManager::busy = false;
        emit finishFetchFileList(false);
        return;
    }

    // parse file list
    QJsonArray jsonarray = QJsonDocument::fromJson(rply->readAll()).array();
    QJsonObject json;
    for (int i=0; i < jsonarray.size(); i++) {
        json = jsonarray[i].toObject();
        QDateTime server_time = QDateTime::fromTime_t(json["Time"].toInt());
        QDateTime local_time  = server_time.addSecs(-1 * srvTimeDeviation);
        QUrl url = this->url;
        url.setScheme(this->srvScheme);
        url.setQuery("REQUEST_ACTION=GET_FILE&REQUEST_LOGIN=" + srvLogin + "&REQUEST_PASSWORD=" + srvPassword + "&REQUEST_FILE=" + json["Path"].toString());
        // create new file
        ServerFile *file = new ServerFile(this);
        file->setServerUrl(url);
        file->setServerSize(json["Size"].toInt());
        file->setServerTime(local_time);
        file->setRelativeCachePath(json["Path"].toString());
        file->setNeedsDownload(ServerManager::cachePath);
        ServerManager::myServerFileList.append(file);
    }

    ServerManager::busy = false;
    emit finishFetchFileList(true);
}

void ServerManager_uqmu_http::slotNetRequestFileDownloadFinished(QNetworkReply *rply)
{
    disconnect(this->myNetMngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotNetRequestFileDownloadFinished(QNetworkReply*)));
    disconnect(srvFileDownloadReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(slotNetRequestFileDownloadProgress(qint64,qint64)));

    bool success = ServerManager::checkNetReply(rply);
    dwnldsze += srvFileDownloadReply->size();
    dwnldsze /= 1024*1024;
    ServerManager::receiveCacheFileData(srvFileDownloadIndex, rply->readAll(), true);
    srvFileDownloadReply->deleteLater();
    ServerManager::busy = false;
    emit finishedFileDownload(success);
}

void ServerManager_uqmu_http::slotNetRequestFileDownloadProgress(qint64 received, qint64 total)
{
    dwnldsze += srvFileDownloadReply->size();
    ServerManager::receiveCacheFileData(srvFileDownloadIndex, srvFileDownloadReply->readAll(), false);
    emit fileDownloadProgress((100 * received) / total, received);
}

