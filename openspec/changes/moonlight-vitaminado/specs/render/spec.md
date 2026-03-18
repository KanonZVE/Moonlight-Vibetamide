# Render Specification (HDR10 Improvements)

## Purpose
Define the requirements for proper 10-bit HDR10 signaling and rendering on Linux (Wayland/Gamescope) using Vulkan and libplacebo.

## Requirements

### Requirement: HDR10 Swapchain Signaling
The system MUST request a Vulkan swapchain with `VK_COLOR_SPACE_HDR10_ST2084_EXT` when HDR is enabled and the physical device supports it.

#### Scenario: Successful HDR Activation
- GIVEN a ROG Ally with Bazzite (Gamescope)
- WHEN a 10-bit HDR stream starts
- THEN the renderer SHALL initialize the swapchain with ST2084 colorspace
- AND report "HDR: ON" to the OS/Compositor.

### Requirement: Static Metadata Injection
The system SHALL use `VK_EXT_hdr_metadata` to pass mastering display metadata (luminance, primaries) to the compositor.

#### Scenario: Metadata Signaling
- GIVEN a stream with HDR metadata from FFmpeg
- WHEN the frame is rendered via libplacebo
- THEN the renderer MUST update the Vulkan HDR metadata extension with the frame's metadata.

### Requirement: Color Space Mapping
The system SHALL map FFmpeg color profiles (BT.2020) to libplacebo `pl_color_space` HDR10 profile.

#### Scenario: 10-bit to HDR10 Mapping
- GIVEN an AV1 or HEVC 10-bit frame
- WHEN the frame enters the Vulkan renderer
- THEN libplacebo SHOULD be configured with `pl_color_space_hdr10`.
