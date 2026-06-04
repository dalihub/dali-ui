# AI Agent Skills

dali-ui 는 개발에 도움이 되는 AI Coding Agent 스킬을 몇가지 제공합니다. 아래 스킬들은 특정 상황에 반드시 지켜야 할 규칙을 개발자가 놓치지 않도록 도움을 줍니다.

> [!WARNING]
> Skill은 특정 상황에 대한 규칙을 검토하여 코드가 올바르게 동작함을 보장하는 것을 목적으로 하므로 MCP 서버를 사용하는 것과는 성격이 다릅니다.

<br/>

## Skill 목록

| Skill | 사용 상황 | 마지막 업데이트 |
|---|---|---|
| [View Inheritance](#view-inheritance) | `View`를 상속한 새 UI 클래스를 만들 때 | 2026-06-04T05:02:19Z |

<br/>

## View Inheritance

`View`를 상속한 새 클래스를 정의할 때 사용하는 skill입니다. handle/impl 클래스 구조, `ViewImpl` 상속, 타입 등록, `New()`, `DownCast()`, `GetImpl()`, `Self()` 같은 DALi UI View 상속 패턴을 따르도록 돕습니다.

<br/>

### 사용 상황

이 skill은 다음과 같은 요청에서 사용합니다.

- `View`를 상속한 새 커스텀 컴포넌트를 만들 때
- handle 클래스와 impl 클래스를 함께 생성해야 할 때
- DALi UI View 상속 규칙에 맞는 새 클래스를 만들 때

<br/>

### 설치 방법

<details>
<summary><ins>Codex</ins></summary>

```bash
git clone --filter=blob:none --sparse https://github.sec.samsung.net/NUI/dali-ui.git /tmp/dali-ui-skill-codex
git -C /tmp/dali-ui-skill-codex sparse-checkout set skills/codex/dali-ui-view-inheritance
```

원하는 Codex skills 디렉터리로 복사후 Codex를 재실행 합니다. 아래는 `~/.codex/skills`에 설치하여 전역으로 적용되는 예시입니다. 특정 프로젝트 한정 설치는 일반적으로 `{프로젝트폴더}/.codex/skills`를 사용합니다.

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

원하는 Claude skills 디렉터리로 복사후 Claude를 재실행 합니다. 아래는 `~/.claude/skills`에 설치하여 전역으로 적용되는 예시입니다. 특정 프로젝트 한정 설치는 일반적으로 `{프로젝트폴더}/.claude/skills`를 사용합니다.

```bash
mkdir -p ~/.claude/skills
cp -R /tmp/dali-ui-skill-claude/skills/claude/dali-ui-view-inheritance ~/.claude/skills/
```

</details>

<br/>

### 프롬프트 예시

```text
ProductCardView라는 View 상속 클래스를 만들어줘.
```

```text
View를 상속해서 FocusRingView 컴포넌트를 추가해줘.
```

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr))
