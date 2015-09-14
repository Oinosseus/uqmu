#include "servermanager.h"
#include <QDir>
#include <QStringList>
#include <QNetworkRequest>
#include <QTime>
#include <QCoreApplication>
#include <cstdlib>
#include <QFile>
#include <QFileInfo>

ServerManager::ServerManager(runLog *log, QDir cachePath, QDir targetPath, QObject *parent) : QObject(parent)
{
    this->myLog = log;
    this->cachePath = cachePath;
    this->targetPath = targetPath;
    this->busy = false;
    this->myServerFileList.clear();
}

void ServerManager::setUrl(const QUrl &url, int timeout)
{
    this->url = url;
    this->timeout = timeout;
}

QUrl ServerManager::getUrl()
{
    return this->url;
}

QList<ServerFile *> ServerManager::getServerFileList()
{
    return this->myServerFileList;
}

bool ServerManager::isBusy()
{
    return this->busy;
}

bool ServerManager::checkNetReply(QNetworkReply *rply)
{
    // check for errors
    if (rply->error()) {
        QString errstr = "";
        QString rplyMessage = QString(rply->readAll());
        errstr += "Network reply error: ";
        errstr += QString::number(rply->error());
        errstr += "\n" + rply->errorString();
        errstr += "\n" + rplyMessage;
        myLog->log(__FILE__, __LINE__, "checkNetReply()", runLog::sevError, errstr);
        return false;
    }
    return true;
}

bool ServerManager::receiveCacheFileData(int fileIndex, const QByteArray &data, bool finished)
{
    static QFile *file = NULL;

    // create cache file
    if (file == NULL) {

        // new file object
        file = new QFile(this->cachePath.absolutePath() + "/" + myServerFileList[fileIndex]->getRelativeCachePath() + ".part");

        // create path
        QFileInfo finfo(*file);
        QDir dir;
        dir.mkpath(finfo.absolutePath());

        // open file
        if (!file->open(QIODevice::WriteOnly)) {
            file->close();
            delete(file);
            file = NULL;
            return false;
        }
    }

    // write data
    qint64 size = file->write(data);
    if (size != data.length()) {
        this->myLog->log(__FILE__, __LINE__, __FUNCTION__, runLog::sevPanic, "Wrong size written!");
    }

    // finish writing
    if (finished) {

        // delete existing cache file
        QFileInfo finfo_cache(this->cachePath.absolutePath() + "/" + myServerFileList[fileIndex]->getRelativeCachePath());
        if (finfo_cache.exists()) {
            QFile::remove(finfo_cache.absoluteFilePath());
        }

        // copy part file to cache file
        bool success = file->rename(this->cachePath.absolutePath() + "/" + myServerFileList[fileIndex]->getRelativeCachePath());

        // close cache file
        file->flush();
        file->close();
        file->deleteLater();
        file = NULL;

        return success;
    }

    return true;
}

bool ServerManager::installFile(int fileIndex)
{
//        QTime dieTime= QTime::currentTime().addSecs(1);
//        while (QTime::currentTime() < dieTime)
//            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    QDir dst(this->targetPath);
    QFileInfo src(this->cachePath.absolutePath() + QString("/") + this->myServerFileList[fileIndex]->getRelativeCachePath());
    QString cmd = "";

    // find correct command
    if (src.suffix().toLower() == "rar") {
        cmd  = "unrar x -y \"" + src.absoluteFilePath() + "\" \"" + dst.absolutePath() + "\"";

    } else if (src.suffix().toLower() == "zip") {
        cmd  = "unzip -o \"" + src.absoluteFilePath() + "\" -d \"" + dst.absolutePath() + "\"";

    } else if (src.suffix().toLower() == "7z") {
        cmd  = "7za x -y \"" + src.absoluteFilePath() + "\" -o\"" + dst.absolutePath() + "\"";

    } else if (src.fileName().endsWith(".tar.gz", Qt::CaseInsensitive)) {
        cmd  = "tar -xzf \"" + src.absoluteFilePath() + "\" -C \"" + dst.absolutePath() + "\"";

    }

    if (cmd.length() == 0) {
        this->myLog->log(__FILE__, __LINE__, __FUNCTION__, runLog::sevError, "No unpack command found for file " + src.fileName());
        return false;
    }

    int ret = std::system(cmd.toLocal8Bit());

    return (ret > 0 ) ? false : true;

}

//this->status = ServerManager::statFETCHINFO;
//    QString msg_success;
//    msg_success += "Server '";
//    msg_success += url;
//    msg_success += "' available";
//    QString msg_fail;
//    msg_fail += "Server '";
//    msg_fail += url;
//    msg_fail += "' not available";

//    if (this->url.scheme() == "file") {
//        QDir dir(this->url.path());
//        if (dir.exists()) {
//            myLog->log(__FILE__, __LINE__, "setUrl()", runLog::sevNotice, msg_success);
//            this->status = ServerManager::statSRVREADY;
//        } else {
//            myLog->log(__FILE__, __LINE__, "setUrl()", runLog::sevNotice, msg_fail);
//            this->status = ServerManager::statSRVDOWN;
//        }

//    } else if (this->url.scheme() == "http" || this->url.scheme() == "https") {
//        // start request
//        myNetMngr->get(url + QString("/aqmu.php"));
//        connect(myNetMngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotNetRequestFetchInfo(QNetworkReply*)));

//    } else {
//        QString msg;
//        msg += "Unknown protocol for server '";
//        msg += url;
//        msg += "'";
//        myLog->log(__FILE__, __LINE__, "setUrl()", runLog::sevError, msg);
//        this->status = ServerManager::statERROR;
//    }
//}

//void ServerManager::fetchFileList()
//{
//    myServerFiles->clear();

//    if (this->status != ServerManager::statSRVREADY) {
//        this->myLog->log(__FILE__, __LINE__, "fetchFileList()", runLog::sevPanic, "Function called while state not statSRVREADY!");
//        return;
//    }
//    this->status = ServerManager::statSRVFETCHFILELIST;

//    if (this->url.scheme() == "file") {
//        this->fetchSchemeFileListRecursive(url);
//        emit fileListFetched(myServerFiles);
//    } else {
//        QString msg = "Cannot fetch filelist from unkown scheme '";
//        msg += url.scheme();
//        msg += "'!";
//        this->myLog->log(__FILE__, __LINE__, "fetchFileList()", runLog::sevPanic, msg);
//    }
//}



//void ServerManager::slotNetRequestFetchInfo(QNetworkReply *rply)
//{
//    if (this->checkNetReply(rply)) {
//        this->status = ServerManager::statSRVREADY;
//        QString msg;
//        msg += "Server '";
//        msg += this->url.toString();
//        msg += "' ready.";
//        myLog->log(__FILE__, __LINE__, "slotNetRequestFetchInfo()", runLog::sevNotice, msg);
//    } else {
//        this->status = ServerManager::statSRVDOWN;
//        QString msg;
//        msg += "Server '";
//        msg += this->url.toString();
//        msg += "' down.";
//        myLog->log(__FILE__, __LINE__, "slotNetRequestFetchInfo()", runLog::sevWarning, msg);
//    }
//}

