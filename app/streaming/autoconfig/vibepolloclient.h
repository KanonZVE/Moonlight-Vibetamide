#pragma once

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

class VibepolloClient : public QObject
{
    Q_OBJECT

public:
    explicit VibepolloClient(QObject *parent = nullptr);

    void fetchHostCapabilities(const QString& hostAddress, const QString& apiToken = QString());

signals:
    void capabilitiesReady(const QJsonObject& capabilities);
    void errorOccurred(const QString& error);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_Nam;
};
