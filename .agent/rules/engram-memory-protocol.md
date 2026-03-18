# Engram Memory Protocol

This rule teaches agents WHEN and HOW to use Engram's MCP tools for persistent memory.
Project: `moonlight-vitaminado`

---

## WHEN TO SAVE (mandatory — not optional)

Call `mem_save` IMMEDIATELY after any of these:
- Bug fix completed
- Architecture or design decision made
- Non-obvious discovery about the codebase
- Configuration change or environment setup
- Pattern established (naming, structure, convention)
- User preference or constraint learned

### Format for `mem_save`
- **title**: Verb + what — short, searchable (e.g. "Fixed HDR swapchain color space", "Chose libplacebo for tonemapping")
- **type**: `bugfix` | `decision` | `architecture` | `discovery` | `pattern` | `config` | `preference`
- **scope**: `project` (default) | `personal`
- **topic_key** (optional, for evolving decisions): stable key like `architecture/hdr-renderer`
- **project**: always `moonlight-vitaminado`
- **content**:
  ```
  **What**: One sentence — what was done
  **Why**: What motivated it (bug, performance, user request, etc.)
  **Where**: Files or paths affected
  **Learned**: Gotchas, edge cases, things that surprised you (omit if none)
  ```

---

## WHEN TO SEARCH MEMORY

When the user asks to recall something — "remember", "recall", "what did we do", "cómo lo resolvimos", "qué hicimos":
1. First call `mem_context` — checks recent session history (fast, cheap)
2. If not found, call `mem_search` with relevant keywords
3. If you find a match, use `mem_get_observation` for full untruncated content

Also search PROACTIVELY when:
- Starting work on something that might have been done before
- The user mentions a topic you have no context on

---

## SESSION CLOSE PROTOCOL (mandatory)

Before ending any session or saying "done" / "listo", call `mem_session_summary`:

```
## Goal
[What we were working on this session]

## Instructions
[User preferences discovered — skip if none]

## Discoveries
- [Technical findings, gotchas, non-obvious learnings]

## Accomplished
- [Completed items with key details]

## Next Steps
- [What remains to be done]

## Relevant Files
- path/to/file — [what it does or what changed]
```

---

## AFTER COMPACTION

If you see a compaction notice or "FIRST ACTION REQUIRED":
1. IMMEDIATELY call `mem_session_summary` with the compacted summary content
2. Then call `mem_context` to recover additional context
3. Only THEN continue working

---

## Key Learnings Pattern

At the end of every task completion, include:

```
## Key Learnings:
1. [finding 1]
2. [finding 2]
```
