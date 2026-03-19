#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QImage>
#include <QUrl>

class SteamGridDBManager : public QObject
{
    Q_OBJECT

public:
    explicit SteamGridDBManager(QObject *parent = nullptr);
    static SteamGridDBManager& instance();

    // Fetches the best boxart for a game by name.
    // Returns the image via a signal or callback.
    void fetchBoxArt(const QString& gameName, int appId, const QString& computerUuid);

signals:
    void boxArtDownloaded(const QString& computerUuid, int appId, const QUrl& localFile);
    void errorOccurred(const QString& message);

private slots:
    void handleSearchResponse(QNetworkReply* reply, const QString& gameName, int appId, const QString& computerUuid);
    void handleGridsResponse(QNetworkReply* reply, int appId, const QString& computerUuid);
    void handleImageDownload(QNetworkReply* reply, int appId, const QString& computerUuid, const QString& cachePath);

private:
    QNetworkAccessManager* m_Nam;
    QString m_ApiKey;

    void searchGame(const QString& gameName, int appId, const QString& computerUuid);
    void getGrids(int gameDbId, int appId, const QString& computerUuid);
};
