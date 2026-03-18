# AutoConfig Specification (Smart Negotiation)

## Purpose
Define the logic for automatic selection of optimal streaming parameters based on host, client, and network state.

## Requirements

### Requirement: Dynamic Resolution Detection
The system MUST detect the native resolution of the active display (e.g., ROG Ally screen or connected TV) and sync it with the host.

#### Scenario: Auto-detect Active Display
- GIVEN the application starts on any display
- WHEN the "Smart Autoconfiguration" is ON
- THEN the default resolution SHALL be set to match the current desktop resolution.

### Requirement: Bitrate Optimization
The system SHALL calculate the optimal bitrate as `min(Network_Bandwidth * 0.8, Max_Codec_Bitrate)`.

#### Scenario: High Bandwidth Network
- GIVEN a 1Gbps local network
- WHEN starting an AV1 10-bit stream
- THEN the bitrate SHALL NOT exceed 80Mbps (to prevent decoder lag).

### Requirement: HDR Auto-enable
The system SHALL enable HDR automatically IF the host reports HDR support AND the local display supports it.

#### Scenario: HDR Handshake
- GIVEN a host with an HDR-capable GPU
- WHEN the client display is HDR-ready
- THEN the session MUST start in HDR mode without user intervention.
