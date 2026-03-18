# Proposal: Moonlight Vitaminado (HDR & Vibepollo Integration)

## Intent

Enhance Moonlight-Qt for ROG Ally (Bazzite) by enabling native 10-bit HDR support via Vulkan/libplacebo and automating stream configuration through a dedicated integration with Vibepollo (advanced Sunshine fork).

## Scope

### In Scope
- **HDR Fix**: Update `PlVkRenderer` to correctly signal HDR10 metadata and swapchain flags for Wayland/Gamescope.
- **Vibepollo Integration**: New REST client to fetch host capabilities (GPU, thermal, encoder load) from Vibepollo API.
- **AutoConfig Engine**: Smart negotiation logic to detect the current display resolution (Ally screen or external TV) and sync it with the host automatically.
- **UI Settings**: Add "Smart Autoconfiguration" toggle in Streaming settings.

### Out of Scope
- Support for X11 HDR (Bazzite uses Wayland/Gamescope).
- Modifications to the host-side Vibepollo/Sunshine code.

## Approach

1. **Detection**: Use `StreamUtils` and Vulkan queries to identify ROG Ally hardware and Gamescope environment.
2. **Negotiation**: Extend `NvHTTP` or create `VibepolloClient` to call `https://host:47990/api/config` before session start.
3. **Injection**: Modify `Session::initialize` to override `STREAM_CONFIGURATION` with values calculated by the `AutoConfigEngine`.
4. **Rendering**: Patch `plvk.cpp` to force `VK_COLOR_SPACE_HDR10_ST2084_EXT` when HDR is enabled on Linux.

## Affected Areas

| Area | Impact | Description |
|------|--------|-------------|
| `app/streaming/video/ffmpeg-renderers/plvk.cpp` | Modified | HDR10 swapchain and metadata signaling. |
| `app/backend/nvhttp.cpp` | Modified | Injected Vibepollo API calls during handshake. |
| `app/streaming/session.cpp` | Modified | Integration point for the AutoConfigEngine. |
| `app/streaming/autoconfig/` | New | `AutoConfigEngine` and `VibepolloClient` classes. |

## Risks

| Risk | Likelihood | Mitigation |
|------|------------|------------|
| Incompatibility with standard Sunshine | Low | Use standard NVHTTP as fallback if Vibepollo API fails. |
| Performance overhead of REST calls | Very Low | Async calls during initial handshake (pre-stream). |
| Vulkan Driver / Gamescope bugs | Medium | Add environment variable overrides (`FORCE_HDR=1`). |

## Rollback Plan

Revert changes to `Session::initialize` and `PlVkRenderer.cpp`. The new `autoconfig/` directory can be kept but disabled via the UI toggle.

## Dependencies

- **libplacebo** >= v7.349.0 (with Vulkan support).
- **FFmpeg** 6.1+ (for 10-bit HEVC/AV1).
- **Vibepollo/Sunshine** host for full autoconfig features.

## Success Criteria

- [ ] HDR is correctly reported as "ON" in Gamescope overlay.
- [ ] 10-bit color depth is validated in debug logs.
- [ ] AutoConfig selects 1080p@120Hz automatically on ROG Ally startup.
- [ ] Latency remains < 20ms on local Gigabit network.
