#include "serverfile.h"

ServerFile::ServerFile(QObject *parent) : QObject(parent)
{
    this->needsDownload = false;
    this->serverSize    = false;
}
QUrl ServerFile::getServerUrl() const
{
    return serverUrl;
}

void ServerFile::setServerUrl(const QUrl &value)
{
    serverUrl = value;
}

qint64 ServerFile::getServerSize() const
{
    return serverSize;
}

void ServerFile::setServerSize(const qint64 &value)
{
    serverSize = value;
}
bool ServerFile::getNeedsDownload() const
{
//    return TRUE; // debug
    return needsDownload;
}

void ServerFile::setNeedsDownload(QDir cachePath)
{
    needsDownload = false;

    QFileInfo finfo(cachePath.path() + QString("/") + relativeCachePath);

    if (!finfo.exists()) needsDownload = true;
    if (serverTime > finfo.lastModified()) needsDownload = true;
    if (serverSize != finfo.size()) needsDownload = true;
}
QString ServerFile::getRelativeCachePath() const
{
    return relativeCachePath;
}

void ServerFile::setRelativeCachePath(const QString &value)
{
    relativeCachePath = value;
}

QDateTime ServerFile::getServerTime() const
{
    return serverTime;
}

void ServerFile::setServerTime(const QDateTime &value)
{
    serverTime = value;
}





