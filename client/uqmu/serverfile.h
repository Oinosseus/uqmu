#ifndef SERVERFILE_H
#define SERVERFILE_H

// Data Class

#include <QObject>
#include <QUrl>
#include <QDir>
#include <QDateTime>
#include <QByteArray>

class ServerFile : public QObject
{
    Q_OBJECT
public:
    explicit ServerFile(QObject *parent = 0);

    QUrl getServerUrl() const;
    void setServerUrl(const QUrl &value);

    qint64 getServerSize() const;
    void setServerSize(const qint64 &value);

    bool getNeedsDownload() const;
    void setNeedsDownload(QDir cachePath);

    QString getRelativeCachePath() const;
    void setRelativeCachePath(const QString &value);

    QDateTime getServerTime() const;
    void setServerTime(const QDateTime &value);

public slots:


private:
    QUrl serverUrl;
    qint64 serverSize;
    QDateTime serverTime;
    bool needsDownload;
    QString relativeCachePath;
};

#endif // SERVERFILE_H
