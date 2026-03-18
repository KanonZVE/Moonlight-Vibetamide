#include "vibepolloclient.h"
#include <QDebug>
#include <QUrlQuery>

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

void VibepolloClient::fetchCommunityProfile(const QString& gameName, const QString& hardwareId)
{
    QUrl url("https://api.vibepollo.io/v1/profiles");
    QUrlQuery query;
    query.addQueryItem("game", gameName);
    query.addQueryItem("hw", hardwareId);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_Nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(QString("Vibepollo Cloud Error: %1").arg(reply->errorString()));
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (!doc.isNull() && doc.isObject()) {
            emit communityProfileReady(doc.object());
        }
    });
}
