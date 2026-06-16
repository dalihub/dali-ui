## DALi

[DALi(Dynamic Animation Library)](https://github.sec.samsung.net/NUI/dali-guide_auto/blob/main/overview.md)는 고성능 그래픽 기반 UI 애플리케이션을 만들기 위한 UI 프레임워크입니다. 앱은 DALi를 통해 화면 객체를 만들고, 레이아웃과 애니메이션을 구성할 수 있습니다.

> [!NOTE]
DALi는 NUI(C#) 의 백엔드 엔진으로 기본 렌더링 기능을 동일하게 제공합니다.

<br/>

<img src="./assets/layered-structure.svg" style="display:block;margin:0 auto"/>

<br/>

DALi는 앱의 화면 구성과 렌더링을 담당하며, 크게 `dali-core`, `dali-adaptor`, `dali-ui` 계층으로 나뉩니다. 앱은 `dali-adaptor`를 통해 Tizen 실행 환경에 연결되고, `dali-core`로 객체를 화면에 나타내며, 필요하면 `dali-ui`의 레이아웃과 상위 레벨 API를 사용해 더 풍부한 UI 를 구성할 수 있습니다.


<br/>

| 라이브러리 | 역할 |
|---|---|
| **dali-core** | <ul><li>핵심 렌더링 기능</li><li>Scene graph, animation, shader 등 렌더링 기본 모델 제공</li><li>Property system, signal 등 베이스 기능 제공</li><li>[Github](https://github.com/dalihub/dali-core)</li></ul>
| **dali-adaptor** | <ul><li>플랫폼 및 그래픽 백엔드 연동 담당</li><li>Window, InputMethod, Widget 등</li><li>[Github](https://github.com/dalihub/dali-adaptor)</li></ul>
| **dali-ui-foundation** | <ul><li>레이아웃, 접근성, 테마 컬러 등 상위 UI Framework API 제공</li><li>`Label`/`ImageView`/`WebView` 등 기본 컴포넌트 제공</li><li>버튼 베이스 등 컴포넌트 기능 클래스 제공 (UX independent)</li><li>[Github(Sec)](https://github.sec.samsung.net/NUI/dali-ui)</li></ul> |
| **dali-ui-components** | <ul><li>`Button`, `Switch`, `ProgressBar`, `Slider`, `Dialog` 등 OneUI 기반 컴포넌트 [약 11종](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components#components-tbd) (계획) </li><li>[Github(Sec)](https://github.sec.samsung.net/NUI/dali-ui)</li></ul> |


<br/>

## API Levels

DALi는 안정성 보장 수준과 대상 독자에 따라 두 가지 API 레벨을 제공합니다.

| 레벨 | 위치 | 대상 | ABI 안정성 |
|------|------|------|------------|
| **Public API** | `public-api/` | 앱 개발자 | **보장** — 릴리즈 간 ABI가 깨져서는 안 됨 |
| **Integration API** | `integration-api/` | Framework 개발자 | 불필요 — dali-ui와 함께 재빌드 가능 |

**앱 개발자**라면 `public-api/`만 사용하세요. ABI 안정성이 보장되는 레이어입니다.

**Framework 개발자**라면 `integration-api/`도 사용할 수 있습니다. 다만 dali-ui 버전 간에 변경될 수 있으며, 변경 시 재빌드가 필요합니다.

<br/>

## View

`View`는 기본 UI building block 이며 주요 기능은 다음과 같습니다.

<img src="./assets/view-box-model.svg" style="display:block;margin:0 auto; width: 360px;"/>

* 레아이웃에 의해 배치되는 박스 모델
* Shadow, Background, Border 등 다층 비주얼 레이어 구성
* FOCUSED, DISABLED 등 상태 관리
* 접근성


`View`의 클래스 구조와 확장에 대한 내용은 [여기](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-(kr)) 를 참고하세요.

<br/>

## Tools

* [AI Agent Skills](https://github.sec.samsung.net/NUI/dali-ui/wiki/AI-Agent-Skills-(kr))

<br/>

## Development Guides

* ~~[Fluent API](https://github.sec.samsung.net/NUI/dali-ui/wiki/Fluent-API-(kr))~~ (deprecated)
* [Configuration](https://github.sec.samsung.net/NUI/dali-ui/wiki/Configuration-(kr))
* [Unit & Scale](https://github.sec.samsung.net/NUI/dali-ui/wiki/Unit-&-Scale-(kr))
* [Ui Scale](https://github.sec.samsung.net/NUI/dali-ui/wiki/Ui-Scale-(Kr))
* [Color & Theme](https://github.sec.samsung.net/NUI/dali-ui/wiki/Color-&-Theme-(kr))
* [View Architecture](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-(kr))
* [Layout](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-(kr))
* [Focus & Key](https://github.sec.samsung.net/NUI/dali-ui/wiki/Focus-&-Key-(kr))
* [Touch & Gesture](https://github.sec.samsung.net/NUI/dali-ui/wiki/Touch-&-Gesture-(kr))
* [State Management](https://github.sec.samsung.net/NUI/dali-ui/wiki/State-Management-(kr))
* [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI-(kr))
* [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)
  * [Text](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-(kr))
  * [Label](https://github.sec.samsung.net/NUI/dali-ui/wiki/Label-(kr))
  * [Text Input](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Input-(kr))
  * [ImageView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ImageView-(kr))
* Navigation
* [ScrollView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ScrollView-(kr))
* ListView & GridView
* Blur
* Animations
* [API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/)

<br/>
