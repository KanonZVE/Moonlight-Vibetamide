#include "boxartmanager.h"
#include "../path.h"

#include <QImageReader>
#include <QImageWriter>

#include "steamgriddbmanager.h"

BoxArtManager::BoxArtManager(QObject *parent) :
    QObject(parent),
    m_BoxArtDir(Path::getBoxArtCacheDir()),
    m_ThreadPool(this),
    m_Computer(nullptr)
{
    // ... setup ...
    m_ThreadPool.setMaxThreadCount(4);
    if (!m_BoxArtDir.exists()) {
        m_BoxArtDir.mkpath(".");
    }

    // Connect to SteamGridDB fallback
    connect(&SteamGridDBManager::instance(), &SteamGridDBManager::boxArtDownloaded,
            this, &BoxArtManager::handleSteamGridDBFinished);
}

void BoxArtManager::setComputer(NvComputer* computer)
{
    m_Computer = computer;
}

void BoxArtManager::handleSteamGridDBFinished(const QString& computerUuid, int appId, const QUrl& localFile)
{
    if (m_Computer && m_Computer->uuid == computerUuid) {
        NvApp app;
        app.id = appId;
        // Search for the app in the computer's app list to get its name if possible
        for (const NvApp& a : m_Computer->appList) {
            if (a.id == appId) {
                app = a;
                break;
            }
        }
        handleBoxArtLoadComplete(m_Computer, app, localFile);
    }
}

QString
BoxArtManager::getFilePathForBoxArt(NvComputer* computer, int appId)
{
    QDir dir = m_BoxArtDir;

    // Create the cache directory if it did not already exist
    if (!dir.exists(computer->uuid)) {
        dir.mkdir(computer->uuid);
    }

    // Change to this computer's box art cache folder
    dir.cd(computer->uuid);

    // Try to open the cached file
    return dir.filePath(QString::number(appId) + ".png");
}

class NetworkBoxArtLoadTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    NetworkBoxArtLoadTask(BoxArtManager* boxArtManager, NvComputer* computer, NvApp& app)
        : m_Bam(boxArtManager),
          m_Computer(computer),
          m_App(app)
    {
        connect(this, &NetworkBoxArtLoadTask::boxArtFetchCompleted,
                boxArtManager, &BoxArtManager::handleBoxArtLoadComplete);
    }

signals:
    void boxArtFetchCompleted(NvComputer* computer, NvApp app, QUrl image);

private:
    void run()
    {
        QUrl image = m_Bam->loadBoxArtFromNetwork(m_Computer, m_App.id);
        if (image.isEmpty()) {
            // Give it another shot if it fails once
            image = m_Bam->loadBoxArtFromNetwork(m_Computer, m_App.id);
        }
        emit boxArtFetchCompleted(m_Computer, m_App, image);
    }

    BoxArtManager* m_Bam;
    NvComputer* m_Computer;
    NvApp m_App;
};

QUrl BoxArtManager::loadBoxArt(NvComputer* computer, NvApp& app)
{
    // Try to open the cached file if it exists and contains data
    QFile cacheFile(getFilePathForBoxArt(computer, app.id));
    if (cacheFile.exists() && cacheFile.size() > 0) {
        return QUrl::fromLocalFile(cacheFile.fileName());
    }

    // If we get here, we need to fetch asynchronously.
    // Kick off a worker on our thread pool to do just that.
    NetworkBoxArtLoadTask* netLoadTask = new NetworkBoxArtLoadTask(this, computer, app);
    m_ThreadPool.start(netLoadTask);

    // Return the placeholder then we can notify the caller
    // later when the real image is ready.
    return QUrl("qrc:/res/no_app_image.png");
}

void BoxArtManager::deleteBoxArt(NvComputer* computer)
{
    QDir dir(Path::getBoxArtCacheDir());

    // Delete everything in this computer's box art directory
    if (dir.cd(computer->uuid)) {
        dir.removeRecursively();
    }
}

void BoxArtManager::clearAllBoxArt()
{
    QDir dir(m_BoxArtDir);

    // Delete everything in the box art cache directory
    dir.removeRecursively();
    dir.mkpath(".");
}

void BoxArtManager::handleBoxArtLoadComplete(NvComputer* computer, NvApp app, QUrl image)
{
    if (!image.isEmpty()) {
        emit boxArtLoadComplete(computer, app, image);
    }
}

QUrl BoxArtManager::loadBoxArtFromNetwork(NvComputer* computer, int appId)
{
    NvHTTP http(computer);

    QString cachePath = getFilePathForBoxArt(computer, appId);
    QImage image;
    try {
        image = http.getBoxArt(appId);
    } catch (...) {}

    // Cache the box art on disk if it loaded
    if (!image.isNull()) {
        if (image.save(cachePath)) {
            return QUrl::fromLocalFile(cachePath);
        }
        else {
            // A failed save() may leave a zero byte file. Make sure that's removed.
            QFile(cachePath).remove();
        }
    } else {
        // Moonlight Vibetamide: SteamGridDB Fallback
        NvApp app;
        app.id = appId;
        // We find the app name from the computer's app list
        for (const NvApp& a : computer->appList) {
            if (a.id == appId) {
                app.name = a.name;
                break;
            }
        }
        
        if (!app.name.isEmpty()) {
            SteamGridDBManager::instance().fetchBoxArt(app.name, appId, computer->uuid);
        }
    }

    return QUrl();
}

#include "boxartmanager.moc"
