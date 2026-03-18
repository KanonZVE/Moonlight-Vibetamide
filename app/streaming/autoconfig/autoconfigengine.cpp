#include "autoconfigengine.h"
#include "streamingpreferences.h"
#include <SDL.h>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <algorithm>

AutoConfigEngine::AutoConfigEngine(QObject *parent)
    : QObject(parent),
      m_Vibepollo(new VibepolloClient(this))
{
    connect(m_Nam, &VibepolloClient::communityProfileReady, this, [this](const QJsonObject& profile) {
        m_LastCommunityProfile = profile;
        qInfo() << "AutoConfig: Received community profile from Vibepollo!";
    });
}

AutoConfigEngine& AutoConfigEngine::instance()
{
    static AutoConfigEngine engine;
    return engine;
}

QSize AutoConfigEngine::getCurrentDisplayResolution()
{
    // Default fallback
    QSize res(1280, 720);

    if (SDL_WasInit(SDL_INIT_VIDEO)) {
        SDL_DisplayMode mode;
        // Specifically get the mode of the first display (primary)
        // In Gamescope, this will be the virtual "screen"
        if (SDL_GetCurrentDisplayMode(0, &mode) == 0) {
            res.setWidth(mode.w);
            res.setHeight(mode.h);
            qInfo() << "AutoConfig: Detected display resolution:" << mode.w << "x" << mode.h << "@" << mode.refresh_rate << "Hz";
        }
    }

    return res;
}

int AutoConfigEngine::calculateOptimalBitrate(int width, int height, int fps)
{
    // Reuse existing Moonlight logic but capped for high-res stability on WiFi
    int baseBitrate = StreamingPreferences::getDefaultBitrate(width, height, fps, false);
    
    // Cap at 80Mbps for stability on handhelds/TVs over WiFi
    return std::min(baseBitrate, 80000);
}

void AutoConfigEngine::optimizeConfiguration(PSTREAM_CONFIGURATION config, const QString& hostAddress)
{
    StreamingPreferences* prefs = StreamingPreferences::get();
    if (!prefs->enableSmartAutoconfig) {
        return;
    }

    qInfo() << "AutoConfig: Running optimization for host:" << hostAddress;

    // 1. Detect Resolution
    QSize nativeRes = getCurrentDisplayResolution();
    config->width = nativeRes.width();
    config->height = nativeRes.height();

    // Moonlight Vitaminado: Apply community profile if available
    if (!m_LastCommunityProfile.isEmpty()) {
        if (m_LastCommunityProfile.contains("width")) 
            config->width = m_LastCommunityProfile["width"].toInt();
        if (m_LastCommunityProfile.contains("height")) 
            config->height = m_LastCommunityProfile["height"].toInt();
        if (m_LastCommunityProfile.contains("bitrate"))
            config->bitrate = m_LastCommunityProfile["bitrate"].toInt();
        
        qInfo() << "AutoConfig: Applied community profile overrides!";
    }

    // 2. Adjust Bitrate based on detected resolution
    config->bitrate = calculateOptimalBitrate(config->width, config->height, config->fps);

    // 3. Auto-enable HDR if display and host support it
    if (prefs->enableHdr) {
        config->remoteHdrMode = 1;
    }

    // Moonlight Vitaminado: Platform-specific overrides
    if (isRunningInGamescope()) {
        qInfo() << "AutoConfig: Gamescope detected! Forcing Borderless Windowed mode.";
        prefs->windowMode = StreamingPreferences::WM_FULLSCREEN_DESKTOP;
    }

    if (isRogAlly()) {
        qInfo() << "AutoConfig: ROG Ally detected! Optimizing touch input.";
        // On Ally, virtual trackpad is better for desktop navigation via touch
        prefs->absoluteTouchMode = false; 
    }

    qInfo() << "AutoConfig: Optimized to" << config->width << "x" << config->height 
            << "at" << config->bitrate << "kbps";
}

bool AutoConfigEngine::isRunningOnBazzite() const
{
    QFile osRelease("/etc/os-release");
    if (osRelease.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = osRelease.readAll();
        return content.contains("bazzite", Qt::CaseInsensitive);
    }
    return false;
}

bool AutoConfigEngine::isRunningInGamescope() const
{
    return qEnvironmentVariableIsSet("GAMESCOPE_WAYLAND_DISPLAY") || 
           qgetenv("XDG_CURRENT_DESKTOP").toLower().contains("gamescope");
}

bool AutoConfigEngine::isRogAlly() const
{
    QFile boardName("/sys/class/dmi/id/board_name");
    if (boardName.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = boardName.readAll().trimmed();
        return content.contains("RC71L") || content.contains("RC72L");
    }
    return false;
}
