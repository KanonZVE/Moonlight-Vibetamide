#include "autoconfigengine.h"
#include "streamingpreferences.h"
#include <SDL.h>
#include <QDebug>
#include <algorithm>

AutoConfigEngine::AutoConfigEngine(QObject *parent)
    : QObject(parent),
      m_Vibepollo(new VibepolloClient(this))
{
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

    // 2. Adjust Bitrate based on detected resolution
    config->bitrate = calculateOptimalBitrate(config->width, config->height, config->fps);

    // 3. Auto-enable HDR if display and host support it (simplified for now, full check later)
    if (prefs->enableHdr) {
        config->remoteHdrMode = 1;
    }

    qInfo() << "AutoConfig: Optimized to" << config->width << "x" << config->height 
            << "at" << config->bitrate << "kbps";
}
