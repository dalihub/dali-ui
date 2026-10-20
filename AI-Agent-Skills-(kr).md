> [!CAUTION]
> 이 페이지는 실험적인 내용을 포함하고 있습니다.

# AI Agent Skills

dali-ui 는 개발에 도움이 되는 AI Coding Agent 스킬을 몇가지 제공합니다. 이 스킬들은 특정 상황에 적용할 수 있는 **코드 템플릿**을 이용하여 사용자 맞춤 코드를 작성해주어 앱의 뼈대를 빠르게 세워주는 역할을 합니다.

<br/>

## Skill List

[여기](https://github.com/dalihub/dali-skills#available-skills)를 참고하세요.

<br/>

## 설치 방법

[여기](https://github.com/dalihub/dali-skills#install-a-skill)를 참고하세요.

<br/>

## Cline에서 Tizen MCP Server 사용하기

AI Coding Agent 중 하나인 Cline 에서 Tizen MCP Server를 사용할 수 있습니다.

Tizen MCP Server는 Cline이 코드 작성을 도울 때 참고할 수 있는 Tizen 및 VD API 정보를 제공합니다. VD 리소스에 접근하려면 API 키가 필요합니다.

MCP (Model Context Protocol)는 AI 에이전트가 도구와 서비스에 연결하는 표준 방식입니다.

### Cline 설치

[Cline 확장 설치 가이드](https://github.sec.samsung.net/CodingAssistant/cline/wiki/Extension-%EC%84%A4%EC%B9%98)를 따라 VS Code에 Cline을 설치하세요.

<br/>

### 1. 필요한 경우 프록시 설정

삼성 보안 네트워크에서는 VS Code에 프록시 설정이 필요할 수 있습니다. 설정 방법은 [Cline MCP 가이드](https://github.sec.samsung.net/CodingAssistant/cline/wiki/MCP)를 참고하세요.

<br/>

### 2. Tizen MCP Server 추가

1. VS Code에서 Cline 확장을 여세요.
2. 오른쪽 상단의 **Customize** 버튼(렌치 아이콘)을 클릭한 뒤 **MCP** 탭을 선택하세요.
3. **MCP Store** 링크를 클릭하고 **Tizen MCP Server**를 검색하세요.
4. Store에서 서버의 JSON 설정을 복사하세요.
5. Cline의 **MCP** 탭으로 돌아와 **Edit Configuration**을 클릭하면 `cline_mcp_settings.json` 파일이 열립니다.
6. 복사한 서버 설정을 파일에 추가하고 저장하세요. 이미 다른 서버가 설정되어 있다면 기존 항목을 유지하고 Tizen MCP Server 항목을 함께 추가하세요.

추가가 완료되면 아래와 같이 **Installed** 목록에 **Tizen MCP Server**가 표시됩니다.

![Cline의 MCP 탭에 정상적으로 추가되고 활성화된 Tizen MCP Server](./assets/cline-mcp-configuration.png)

<br/>

### 3. 프롬프트 입력해 보기

dali-ui 스킬을 설치하고 Tizen MCP Server를 연결했다면, Cline을 열고 다음과 같이 요청하세요.

> 노란색 정사각형 하나를 그리는 dali-ui 앱을 만들어 줘.

이렇게 요청하면 시작할 수 있습니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr))
