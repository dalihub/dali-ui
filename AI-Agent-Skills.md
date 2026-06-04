[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/AI-Agent-Skills-(kr))

# AI Agent Skills

dali-ui provides several AI Coding Agent skills that help with development. The skills below help developers avoid missing rules that must be followed in specific situations.

> [!WARNING]
> Skills are aimed at reviewing rules for specific situations to ensure code correctness, so they are different from using MCP server.

<br/>

## Skill Index

| Skill | When to use | Last update |
|---|---|---|
| [View Inheritance](#view-inheritance) | When creating a new UI class derived from `View` | 2026-06-04T05:02:19Z |

<br/>

## View Inheritance

This skill is used when defining a new class derived from `View`. It helps the agent follow the DALi UI View inheritance pattern, including the handle/impl class structure, `ViewImpl` inheritance, type registration, `New()`, `DownCast()`, `GetImpl()`, and `Self()`.

<br/>

### When to Use

Use this skill for requests such as:

- Creating a new custom component derived from `View`
- Generating both handle and impl classes
- Creating a new class that follows the DALi UI View inheritance rules

<br/>

### Installation

<details>
<summary><ins>Codex</ins></summary>

```bash
git clone --filter=blob:none --sparse https://github.sec.samsung.net/NUI/dali-ui.git /tmp/dali-ui-skill-codex
git -C /tmp/dali-ui-skill-codex sparse-checkout set skills/codex/dali-ui-view-inheritance
```

Copy it into your Codex skills directory and then restart Codex. The example below installs it into `~/.codex/skills` so it applies globally. For project-local installation, the usual location is `{project-directory}/.codex/skills`.

```bash
mkdir -p ~/.codex/skills
cp -R /tmp/dali-ui-skill-codex/skills/codex/dali-ui-view-inheritance ~/.codex/skills/
```

</details>

<details>
<summary><ins>Claude</ins></summary>

```bash
git clone --filter=blob:none --sparse https://github.sec.samsung.net/NUI/dali-ui.git /tmp/dali-ui-skill-claude
git -C /tmp/dali-ui-skill-claude sparse-checkout set skills/claude/dali-ui-view-inheritance
```

Copy it into your Claude skills directory and then restart Claude. The example below installs it into `~/.claude/skills` so it applies globally. For project-local installation, the usual location is `{project-directory}/.claude/skills`.

```bash
mkdir -p ~/.claude/skills
cp -R /tmp/dali-ui-skill-claude/skills/claude/dali-ui-view-inheritance ~/.claude/skills/
```

</details>

<br/>

### Prompt Examples

```text
Create a View-derived class named ProductCardView.
```

```text
Add a FocusRingView component derived from View.
```

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki)
