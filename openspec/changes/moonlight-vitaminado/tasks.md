# Tasks: Moonlight Vitaminado (HDR & Vibepollo)

## Phase 1: Foundation & Data Structures
- [ ] 1.1 Create directory `app/streaming/autoconfig/`
- [ ] 1.2 Implement `VibepolloClient` class (JSON REST client) in `app/streaming/autoconfig/vibepolloclient.h/.cpp`
- [ ] 1.3 Implement `AutoConfigEngine` singleton and `optimizeConfiguration()` method in `app/streaming/autoconfig/autoconfigengine.h/.cpp`
- [ ] 1.4 Add new "Smart Autoconfiguration" toggle to `StreamingPreferences.h/.cpp`

## Phase 2: Backend & Negotiation (Vibepollo)
- [ ] 2.1 Update `NvHTTP::startApp` to call `AutoConfigEngine` before the launch request.
- [ ] 2.2 Implement `https://host:47990/api/config` query in `VibepolloClient` to fetch host caps.
- [ ] 2.3 Modify `NvHTTP` to inject calculated HDR and bitrate parameters into the launch query string.

## Phase 3: Renderer & HDR (Vulkan)
- [ ] 3.1 Modify `PlVkRenderer::initialize` in `plvk.cpp` to force `VK_COLOR_SPACE_HDR10_ST2084_EXT` if 10-bit is enabled.
- [ ] 3.2 Ensure `VK_EXT_hdr_metadata` is correctly used in `plvk.cpp` for frame submission on Linux.
- [ ] 3.3 Patch libplacebo initialization code to use `pl_color_space_hdr10` profile.

## Phase 4: Integration
- [ ] 4.1 Update `Session::initialize` in `session.cpp` to trigger the AutoConfig handshake.
- [ ] 4.2 Wire the UI toggle in QML to enable/disable the `AutoConfigEngine` logic.

## Phase 5: Verification & Testing
- [ ] 5.1 Unit test: Verify `AutoConfigEngine` resolution detection logic.
- [ ] 5.2 Manual test: Verify HDR "ON" in Gamescope OSD on ROG Ally.
- [ ] 5.3 Manual test: Verify dynamic resolution autoselection (e.g., TV vs Ally screen).
