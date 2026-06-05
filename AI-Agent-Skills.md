[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/AI-Agent-Skills-(kr))

# AI Agent Skills

dali-ui provides several AI Coding Agent skills that assist with development. These skills use **code templates** applicable to specific situations to write customized code, helping you quickly scaffold your app.

> [!IMPORTANT]
> dali-ui skills are intended to provide templates suitable for specific situations, so they are different in nature from using MCP servers.

<br/>

## Skill List

| Skill | When to use | Last update |
|---|---|---|
| [dali-ui-view-inheritance](#view-inheritance) | When creating a new UI class derived from `View` | 2026-06-04T05:02:19Z |

<br/>

FYI, available skills are the remote branches in the `dali-skills` repository:

```bash
git ls-remote --heads https://github.com/dalihub/dali-skills.git
```
<br/>

## Installation

Download the skill to your desired location using the code below, then restart the agent.

```bash
git clone -b {skill-name} --single-branch https://github.com/dalihub/dali-skills.git {agent-skills-dir}
```
<br/>

* **skill-name**: The name of the skill. See [Skill list](https://github.sec.samsung.net/NUI/dali-ui/wiki/AI-Agent-Skills#skill-list) for available skills.
* **agent-skills-dir**: Installation folder. For Claude, global is `~/.claude/skills`, project-local is `/path/to/project/.claude/skills`.

<br/>

#### 예: Install to Claude global settings

```bash
git clone -b dali-ui-view-inheritance --single-branch https://github.com/dalihub/dali-skills.git ~/.claude/skills
```

#### 예: Install to Codex project settings

```bash
git clone -b dali-ui-view-inheritance --single-branch https://github.com/dalihub/dali-skills.git /path/to/project/.codex/skills
```

<br/>

# Skills Description

## View Inheritance

Use this skill for requests such as:

- Creating a new custom component derived from `View`
- Generating both handle and impl classes
- Creating a new class that follows the DALi UI View inheritance rules

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
