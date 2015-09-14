#include "networkaccessmanager.h"
#include <QSslConfiguration>

NetworkAccessManager::NetworkAccessManager(QObject *parent) : QNetworkAccessManager(parent)
{
}

QNetworkReply *NetworkAccessManager::get(const QUrl &url)
{
    // generate request
    QNetworkRequest req = QNetworkRequest(url);

    // create SSL configuration
    if (url.scheme() == "https") {
        // create SSL configuration
        QSslConfiguration sslConfiguration = QSslConfiguration::defaultConfiguration();
        sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
        req.setSslConfiguration(sslConfiguration);
    }

    return QNetworkAccessManager::get(req);
}
