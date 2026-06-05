> [!CAUTION]
> WIP! 이 페이지는 내용이 정식으로 검증되지 않았습니다.

<br/>

# AI Agent Skills

dali-ui 는 개발에 도움이 되는 AI Coding Agent 스킬을 몇가지 제공합니다. 이 스킬들은 특정 상황에 적용할 수 있는 **코드 템플릿**을 이용하여 사용자 맞춤 코드를 작성해주어 앱의 뼈대를 빠르게 세워주는 역할을 합니다.

> [!IMPORTANT]
> dali-ui 스킬은 특정 상황에 적합한 템플릿을 제공하는 것을 목적으로 하므로 MCP 서버를 사용하는 것과는 성격이 다릅니다.

<br/>

## Skill List

| Skill | 사용 상황 | 마지막 업데이트 |
|---|---|---|
| [dali-ui-view-inheritance](#view-inheritance) | `View`를 상속한 새 UI 클래스를 만들 때 | 2026-06-04T05:02:19Z |

<br/>

다른 방법으로, `dali-skills` repository의 브랜치 목록으로 조회해 볼 수도 있습니다:

```bash
git ls-remote --heads https://github.com/dalihub/dali-skills.git
```
<br/>

## 설치 방법

아래 코드로 원하는 위치에 스킬을 다운받고 agent를 재시작 합니다.

```bash
git clone -b {skill-name} --single-branch https://github.com/dalihub/dali-skills.git {agent-skills-dir}
```
<br/>

* **skill-name** : 스킬의 이름. [스킬 목록](https://github.sec.samsung.net/NUI/dali-ui/wiki/AI-Agent-Skills-(kr)#skill-list)에서 확인 가능
* **agent-skills-dir** : 설치 폴더. Claude의 경우 전역은 `~/.claude/skills`, 프로젝트는 `/path/to/project/.claude/skills`

<br/>

#### 예: Claude 전역 폴더에 설치

```bash
git clone -b dali-ui-view-inheritance --single-branch https://github.com/dalihub/dali-skills.git ~/.claude/skills
```

#### 예: Codex 프로젝트 폴더에 설치

```bash
git clone -b dali-ui-view-inheritance --single-branch https://github.com/dalihub/dali-skills.git /path/to/project/.codex/skills
```

<br/>

# Skills Description

## View Inheritance

이 skill은 다음과 같은 요청에서 사용합니다.

- `View`를 상속한 새 커스텀 컴포넌트를 만들 때
- handle 클래스와 impl 클래스를 함께 생성해야 할 때
- `View` 상속 규칙에 맞는 새 클래스를 만들 때

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
