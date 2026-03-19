#include "moondeckclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>

MoonDeckClient::MoonDeckClient(QObject *parent)
    : QObject(parent)
{
}

QNetworkRequest MoonDeckClient::createRequest(const QString& host, quint16 port, const QString& endpoint, const QString& clientId)
{
    QUrl url(QString("https://%1:%2/v1%3").arg(host).arg(port).arg(endpoint));
    QNetworkRequest request(url);
    
    // MoonDeck Buddy authentication (Basic Auth with Client ID)
    QByteArray auth = clientId.toUtf8().toBase64();
    request.setRawHeader("Authorization", "Basic " + auth);
    
    // MoonDeck Buddy uses self-signed certs
    QSslConfiguration config = request.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);
    
    return request;
}

void MoonDeckClient::fetchNonSteamApps(const QString& host, quint16 port, const QString& clientId)
{
    // FIXME: We need to know the User ID (long) to fetch shortcuts. 
    // Usually it's the first one found or we can try to guess/prompt.
    // For now, let's assume a default or common path.
    // MoonDeck Buddy expects /nonSteamAppData?userId=...
    
    // Actually, MoonDeck Buddy might have a default or we need to find the user ID.
    // Let's try to fetch user info first or just guess.
    
    qInfo() << "Fetching MoonDeck Non-Steam apps from" << host << ":" << port;
    
    // In many cases, we can try to get the user ID from the host info
    QNetworkRequest request = createRequest(host, port, "/nonSteamAppData", clientId);
    // Note: We might need a userId parameter here if Buddy doesn't default to the active one.
    
    QNetworkReply* reply = m_Nam.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleFetchReply(reply);
    });
}

void MoonDeckClient::handleFetchReply(QNetworkReply* reply)
{
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        return;
    }
    
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        emit errorOccurred("Invalid JSON response from MoonDeck Buddy");
        return;
    }
    
    QJsonObject obj = doc.object();
    QJsonArray appsArray = obj["apps"].toArray();
    
    QVector<NvApp> apps;
    for (const QJsonValue& val : appsArray) {
        QJsonObject appObj = val.toObject();
        NvApp app;
        app.name = appObj["name"].toString();
        
        // Map 64-bit ID to 32-bit (Virtual ID)
        QString moondeckIdStr = appObj["id"].toString();
        bool ok;
        quint64 moondeckId = moondeckIdStr.toULongLong(&ok);
        if (ok) {
            app.id = static_cast<int>(moondeckId & 0x7FFFFFFF) | 0x40000000;
            app.moondeckId = moondeckIdStr;
            app.isMoonDeckApp = true;
        }
        
        app.isAppCollectorGame = true; // Mark it as special
        apps.append(app);
    }
    
    emit nonSteamAppsFetched(apps);
}

void MoonDeckClient::launchApp(const QString& host, quint16 port, const QString& clientId, const QString& moondeckId)
{
    QNetworkRequest request = createRequest(host, port, "/launchSteamApp", clientId);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject obj;
    obj["appId"] = moondeckId;
    
    QNetworkReply* reply = m_Nam.post(request, QJsonDocument(obj).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleLaunchReply(reply);
    });
}

void MoonDeckClient::launchSteam(const QString& host, quint16 port, const QString& clientId, bool bpm)
{
    QNetworkRequest request = createRequest(host, port, "/launchSteam", clientId);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject obj;
    obj["bigPictureMode"] = bpm;
    
    QNetworkReply* reply = m_Nam.post(request, QJsonDocument(obj).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleLaunchReply(reply);
    });
}

void MoonDeckClient::handleLaunchReply(QNetworkReply* reply)
{
    reply->deleteLater();
    emit launchCompleted(reply->error() == QNetworkReply::NoError);
}
