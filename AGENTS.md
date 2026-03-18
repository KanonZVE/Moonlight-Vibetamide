# Moonlight Vitaminado — AGENTS.md
# Agent workspace configuration for Antigravity

## Workspace Overview

This workspace contains the Moonlight-Qt game streaming client, modified for optimal
performance on Bazzite Linux / ROG Ally with HDR 10-bit support, Vibepollo integration,
and adaptive autoconfiguration.

## Skills Index

When working on this project, load the relevant skill(s) BEFORE writing any code.

| Skill | Trigger | Path |
|-------|---------|------|
| `sdd-init` | Initialize SDD in project | [`.agent/skills/sdd-init/SKILL.md`](.agent/skills/sdd-init/SKILL.md) |
| `sdd-explore` | Investigate codebase / think through feature | [`.agent/skills/sdd-explore/SKILL.md`](.agent/skills/sdd-explore/SKILL.md) |
| `sdd-propose` | Create change proposal | [`.agent/skills/sdd-propose/SKILL.md`](.agent/skills/sdd-propose/SKILL.md) |
| `sdd-spec` | Write specifications | [`.agent/skills/sdd-spec/SKILL.md`](.agent/skills/sdd-spec/SKILL.md) |
| `sdd-design` | Write technical design | [`.agent/skills/sdd-design/SKILL.md`](.agent/skills/sdd-design/SKILL.md) |
| `sdd-tasks` | Break down into tasks | [`.agent/skills/sdd-tasks/SKILL.md`](.agent/skills/sdd-tasks/SKILL.md) |
| `sdd-apply` | Implement code changes | [`.agent/skills/sdd-apply/SKILL.md`](.agent/skills/sdd-apply/SKILL.md) |
| `sdd-verify` | Validate implementation vs specs | [`.agent/skills/sdd-verify/SKILL.md`](.agent/skills/sdd-verify/SKILL.md) |
| `sdd-archive` | Archive completed change | [`.agent/skills/sdd-archive/SKILL.md`](.agent/skills/sdd-archive/SKILL.md) |
| `skill-registry` | Update skill registry | [`.agent/skills/skill-registry/SKILL.md`](.agent/skills/skill-registry/SKILL.md) |

## Active Rules

- [SDD Orchestrator](.agent/rules/sdd-orchestrator.md) — COORDINATOR role, delegate-first
- [Engram Memory Protocol](.agent/rules/engram-memory-protocol.md) — When/how to use persistent memory

## Project Context

- **Repo**: moonlight-qt/ (game streaming client, C++/Qt6/QML)
- **Build**: qmake6, `make release`, FFmpeg ≥6.1, libplacebo ≥v7.349.0
- **Target**: Bazzite Linux (Fedora-based) on ROG Ally (AMD RDNA GPU)
- **Persistence**: engram (MCP active) — project name `moonlight-vitaminado`
- **Model assignments**: Opus for spec/design/apply/verify; Gemini Flash for explore/propose/tasks/archive
