#pragma once

#include "nvapp.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QUrl>

class MoonDeckClient : public QObject
{
    Q_OBJECT

public:
    explicit MoonDeckClient(QObject *parent = nullptr);

    void fetchNonSteamApps(const QString& host, quint16 port, const QString& clientId);
    void launchApp(const QString& host, quint16 port, const QString& clientId, const QString& moondeckId);
    void launchSteam(const QString& host, quint16 port, const QString& clientId, bool bpm);

signals:
    void nonSteamAppsFetched(const QVector<NvApp>& apps);
    void errorOccurred(const QString& error);
    void launchCompleted(bool success);

private slots:
    void handleFetchReply(QNetworkReply* reply);
    void handleLaunchReply(QNetworkReply* reply);

private:
    QNetworkAccessManager m_Nam;
    QString m_ClientId;

    QNetworkRequest createRequest(const QString& host, quint16 port, const QString& endpoint, const QString& clientId);
};
