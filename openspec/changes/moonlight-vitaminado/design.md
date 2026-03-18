# Design: Moonlight Vitaminado (HDR & Vibepollo)

## Technical Approach
The integration follows a "Negotiation Phase" pattern. Before the streaming session starts, the `AutoConfigEngine` queries both the host (via `VibepolloClient`) and the local hardware to determine the best parameters, which are then injected into the `STREAM_CONFIGURATION`.

## Architecture Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| **REST Client** | `QNetworkAccessManager` | Consistency with existing Qt/Moonlight architecture. |
| **Discovery** | Async Handshake | Prevents blocking the UI thread during capability discovery. |
| **HDR Signaling** | Force ST2084 on Vulkan | Gamescope/Wayland requires explicit swapchain signaling for HDR activation. |

## Data Flow
```
Session::initialize ──→ AutoConfigEngine ──→ VibepolloClient (REST)
          │                 │                      │
          │                 └←── Host Caps ────────┘
          ▼
   Apply STREAM_CONFIG ──→ NvHTTP::startApp ──→ Renderer::PlVk (HDR)
```

## File Changes

| File | Action | Description |
|------|--------|-------------|
| `app/streaming/autoconfig/vibepolloclient.h/.cpp` | Create | Handles JSON requests to Vibepollo API (:47990). |
| `app/streaming/autoconfig/autoconfigengine.h/.cpp` | Create | Singleton orchestrator for optimal config selection. |
| `app/streaming/video/ffmpeg-renderers/plvk.cpp` | Modify | Patch `initialize()` to force HDR10 swapchain flags. |
| `app/backend/nvhttp.cpp` | Modify | Add `hdrMode=1` and custom flags if Vibepollo is detected. |
| `app/streaming/session.cpp` | Modify | Intercept `initialize()` to call the `AutoConfigEngine`. |

## Interfaces / Contracts

### VibepolloClient API
```cpp
class VibepolloClient : public QObject {
    Q_OBJECT
public:
    void fetchHostCapabilities(const QString& host, const QString& token);
signals:
    void capabilitiesReady(const QJsonObject& caps);
};
```

### AutoConfigEngine Interface
```cpp
class AutoConfigEngine {
public:
    static AutoConfigEngine& instance();
    void optimizeConfiguration(PSTREAM_CONFIGURATION config);
private:
    QSize getCurrentDisplayResolution();
};
```

## Testing Strategy

| Layer | What to Test | Approach |
|-------|-------------|----------|
| Unit | `AutoConfigEngine` | Mock host capabilities and verify chosen bitrate/resolution. |
| Integration | `VibepolloClient` | Test against a local mock REST server. |
| Manual | Vulkan HDR | Run on ROG Ally (Bazzite) and check Gamescope OSD for "HDR: ON". |

## Migration / Rollout
Feature will be gated by a UI toggle in Settings -> Streaming -> "Smart Autoconfiguration". Default: OFF.
