#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>

class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit NetworkAccessManager(QObject *parent = 0);
    QNetworkReply* get(const QUrl &url);

signals:

public slots:

private:
};

#endif // NETWORKACCESSMANAGER_H
