# Backend Specification (Vibepollo API)

## Purpose
Define the communication protocol between Moonlight-Qt and the Vibepollo (Sunshine fork) REST API.

## Requirements

### Requirement: REST Authentication
The system MUST support Authentication via API Token passed in the `Authorization` header for Vibepollo-specific endpoints.

#### Scenario: Authorized Connection
- GIVEN a valid Vibepollo API Token in settings
- WHEN the client requests host capabilities
- THEN the backend SHALL include the Bearer token in the HTTPS request.

### Requirement: Host Capability Discovery
The system SHALL query `https://<host>:47990/api/config` (or `/serverinfo` extensions) to retrieve detailed GPU load and thermal data.

#### Scenario: Capacity Check
- GIVEN an active connection to a Vibepollo host
- WHEN a streaming session is about to start
- THEN the client MUST fetch the current encoder load from the host.

### Requirement: Dynamic Profile Adjustment
The system SHOULD be able to send custom launch parameters (e.g., `low_latency_mode=1`) if supported by the host API.

#### Scenario: Low Latency Mode
- GIVEN a host that supports low latency optimization
- WHEN the network RTT is < 5ms
- THEN the launch POST request SHALL include the low latency flag.
