#include "steamgriddbmanager.h"
#include "../path.h"
#include <QDir>
#include <QNetworkRequest>
#include <QImageReader>

#include "../settings/streamingpreferences.h"

SteamGridDBManager::SteamGridDBManager(QObject *parent) : QObject(parent)
{
    m_Nam = new QNetworkAccessManager(this);
    m_ApiKey = StreamingPreferences::get()->steamGridDbApiKey;
    
    // Update API Key if it changes
    connect(StreamingPreferences::get(), &StreamingPreferences::steamGridDbApiKeyChanged, [this]() {
        m_ApiKey = StreamingPreferences::get()->steamGridDbApiKey;
    });
}

SteamGridDBManager& SteamGridDBManager::instance()
{
    static SteamGridDBManager _instance;
    return _instance;
}

void SteamGridDBManager::fetchBoxArt(const QString& gameName, int appId, const QString& computerUuid)
{
    if (m_ApiKey.isEmpty()) {
        emit errorOccurred("SteamGridDB API Key not set");
        return;
    }
    searchGame(gameName, appId, computerUuid);
}

void SteamGridDBManager::searchGame(const QString& gameName, int appId, const QString& computerUuid)
{
    QUrl url("https://www.steamgriddb.com/api/v2/search/autocomplete/game/" + QUrl::toPercentEncoding(gameName));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + m_ApiKey.toUtf8());

    QNetworkReply* reply = m_Nam->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply, gameName, appId, computerUuid]() {
        handleSearchResponse(reply, gameName, appId, computerUuid);
    });
}

void SteamGridDBManager::handleSearchResponse(QNetworkReply* reply, const QString& gameName, int appId, const QString& computerUuid)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("Search failed: " + reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();
    if (obj["success"].toBool()) {
        QJsonArray data = obj["data"].toArray();
        if (!data.isEmpty()) {
            int gameId = data[0].toObject()["id"].toInt();
            getGrids(gameId, appId, computerUuid);
        }
    }
}

void SteamGridDBManager::getGrids(int gameDbId, int appId, const QString& computerUuid)
{
    QUrl url(QString("https://www.steamgriddb.com/api/v2/grids/game/%1?dimensions=600x900,342x482,660x930").arg(gameDbId));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + m_ApiKey.toUtf8());

    QNetworkReply* reply = m_Nam->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply, appId, computerUuid]() {
        handleGridsResponse(reply, appId, computerUuid);
    });
}

void SteamGridDBManager::handleGridsResponse(QNetworkReply* reply, int appId, const QString& computerUuid)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) return;

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();
    if (obj["success"].toBool()) {
        QJsonArray data = obj["data"].toArray();
        if (!data.isEmpty()) {
            QString imageUrl = data[0].toObject()["url"].toString();
            
            QDir dir(Path::getBoxArtCacheDir());
            dir.mkdir(computerUuid);
            dir.cd(computerUuid);
            QString cachePath = dir.filePath(QString::number(appId) + ".png");

            QNetworkRequest request(imageUrl);
            QNetworkReply* imgReply = m_Nam->get(request);
            connect(imgReply, &QNetworkReply::finished, [this, imgReply, appId, computerUuid, cachePath]() {
                handleImageDownload(imgReply, appId, computerUuid, cachePath);
            });
        }
    }
}

void SteamGridDBManager::handleImageDownload(QNetworkReply* reply, int appId, const QString& computerUuid, const QString& cachePath)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) return;

    QImage image = QImageReader(reply).read();
    if (!image.isNull()) {
        if (image.save(cachePath)) {
            emit boxArtDownloaded(computerUuid, appId, QUrl::fromLocalFile(cachePath));
        }
    }
}
