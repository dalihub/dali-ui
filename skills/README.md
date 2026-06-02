# DALi UI AI Agent Skills

This directory contains AI agent skills for DALi UI development.

## Available Skills

- `codex/dali-ui-view-inheritance`: Codex skill for creating new classes derived from `Dali::Ui::View`.
- `claude/dali-ui-view-inheritance`: Claude skill for creating new classes derived from `Dali::Ui::View`.

## Sparse Checkout

Clone only the Codex skill directory:

```bash
git clone --filter=blob:none --sparse https://github.sec.samsung.net/NUI/dali-ui.git /tmp/dali-ui-skill-codex
git -C /tmp/dali-ui-skill-codex sparse-checkout set skills/codex/dali-ui-view-inheritance
```

Clone only the Claude skill directory:

```bash
git clone --filter=blob:none --sparse https://github.sec.samsung.net/NUI/dali-ui.git /tmp/dali-ui-skill-claude
git -C /tmp/dali-ui-skill-claude sparse-checkout set skills/claude/dali-ui-view-inheritance
```

## Install and Restart

Copy the Codex skill into your Codex skills directory. The example below installs it into `~/.codex/skills` so it applies globally. For project-local installation, the usual location is `{project-directory}/.codex/skills`.

```bash
mkdir -p ~/.codex/skills
cp -R /tmp/dali-ui-skill-codex/skills/codex/dali-ui-view-inheritance ~/.codex/skills/
```

Copy the Claude skill into your Claude skills directory. The example below installs it into `~/.claude/skills` so it applies globally. For project-local installation, the usual location is `{project-directory}/.claude/skills`.

```bash
mkdir -p ~/.claude/skills
cp -R /tmp/dali-ui-skill-claude/skills/claude/dali-ui-view-inheritance ~/.claude/skills/
```

After copying the skill directory, it needs to restart the service.