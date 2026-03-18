#include "vibepolloclient.h"
#include <QDebug>

VibepolloClient::VibepolloClient(QObject *parent)
    : QObject(parent),
      m_Nam(new QNetworkAccessManager(this))
{
}

void VibepolloClient::fetchHostCapabilities(const QString& hostAddress, const QString& apiToken)
{
    QUrl url(QString("https://%1:47990/api/config").arg(hostAddress));
    QNetworkRequest request(url);

    if (!apiToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(apiToken).toUtf8());
    }

    // Ignore SSL errors for local Sunshine certificates (same as NvHTTP)
    QSslConfiguration config = request.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);

    QNetworkReply* reply = m_Nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void VibepolloClient::onReplyFinished(QNetworkReply* reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull() || !doc.isObject()) {
        emit errorOccurred("Invalid JSON response from host");
        return;
    }

    emit capabilitiesReady(doc.object());
}
