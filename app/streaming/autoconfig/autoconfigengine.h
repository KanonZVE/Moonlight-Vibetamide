#pragma once

#include <QObject>
#include <QSize>
#include "Limelight.h"
#include "streaming/autoconfig/vibepolloclient.h"

class AutoConfigEngine : public QObject
{
    Q_OBJECT

public:
    static AutoConfigEngine& instance();

    // Main entry point to optimize the stream configuration
    void optimizeConfiguration(PSTREAM_CONFIGURATION config, const QString& hostAddress);

    bool isRunningOnBazzite() const;
    bool isRunningInGamescope() const;
    bool isRogAlly() const;

private:
    explicit AutoConfigEngine(QObject *parent = nullptr);
    
    QSize getCurrentDisplayResolution();
    int calculateOptimalBitrate(int width, int height, int fps);

    VibepolloClient* m_Vibepollo;
    QJsonObject m_LastCommunityProfile;
};
