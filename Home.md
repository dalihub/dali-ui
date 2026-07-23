[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr))

## DALi

[DALi (Dynamic Animation Library)](https://github.sec.samsung.net/NUI/dali-guide_auto/blob/main/overview.md) is a UI framework for building high-performance graphics-based UI applications. Applications use DALi to create screen objects, build layouts and animations.

> [!NOTE]
DALi serves as the backend engine for NUI (C#), providing the same core rendering capabilities.


<br/>

<img src="./assets/layered-structure.svg" style="display:block;margin:0 auto"/>

<br/>

DALi handles screen composition and rendering, and is organized into `dali-core`, `dali-adaptor`, and `dali-ui` layers. Applications connect to the Tizen runtime environment through `dali-adaptor`, present objects on screen through `dali-core`, and can use `dali-ui` layouts and higher-level APIs to build richer UI structures.

<br/>

| Library | Role |
|---|---|
| **dali-core** | <ul><li>Core rendering functionality</li><li>Provides fundamental rendering models such as scene graph, animation, and shader</li><li>Provides base features such as property system and signal</li><li>[Github](https://github.com/dalihub/dali-core)</li></ul> |
| **dali-adaptor** | <ul><li>Handles platform and graphics backend integration</li><li>Window, InputMethod, Widget, etc.</li><li>[Github](https://github.com/dalihub/dali-adaptor)</li></ul> |
| **dali-ui-foundation** | <ul><li>Provides higher-level UI Framework APIs such as layout, accessibility, and theme color</li><li>Provides base components such as `Label`, `ImageView`, and `WebView`</li><li>Provides component feature classes such as button base (UX independent)</li><li>[Github(Sec)](https://github.sec.samsung.net/NUI/dali-ui)</li></ul> |
| **dali-ui-components** | <ul><li>OneUI-based components such as `Button`, `Switch`, `ProgressBar`, `Slider`, and `Dialog`, with [about 11 planned components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components#components-tbd)</li><li>[Github(Sec)](https://github.sec.samsung.net/NUI/dali-ui)</li></ul> |

<br/>

## API Levels

DALi exposes three API levels with different stability guarantees and intended audiences:

| Level | Audience | ABI Stability | Umbrella Header |
|---|---|---|---|
| **Public API** | App developers | **Guaranteed** — must not break across releases | `<dali-ui-foundation/dali-ui-foundation.h>` |
| **Extension API** | DALi UI module providers | **Guaranteed** — must not break across releases | `<dali-ui-foundation/dali-ui-foundation-extension.h>` |
| **Integration API** | Framework developers | Not required — consumers rebuild alongside dali-ui | `<dali-ui-foundation/dali-ui-foundation-integ.h>` |

> **Note:** The former `provider-api` level has been renamed to `extension-api`.

**As an app developer**, use the Public API umbrella header or individual `public-api/` headers. This is the layer with guaranteed ABI stability.

**As a DALi UI module provider**, use the Extension API umbrella header when you need ABI-stable implementation-level extension points.

**As a framework developer**, you may also depend on the Integration API umbrella header, with the understanding
that it may change between dali-ui versions and will require a rebuild.

<br/>

## View

`View` is the basic UI building block and provides the following main features.

<img src="./assets/view-box-model.svg" style="display:block;margin:0 auto; width: 360px;"/>

* Box model arranged by layouts
* Multi-layer visual composition such as Shadow, Background, and Border
* State management such as FOCUSED and DISABLED
* Accessibility

For details about the class structure and extensibility of `View`, see [here](https://github.sec.samsung.net/NUI/dali-ui/wiki/View).

<br/>

## Tools

* [AI Agent Skills](https://github.sec.samsung.net/NUI/dali-ui/wiki/AI-Agent-Skills)

<br/>

## Development Guides

* ~~[Fluent API](https://github.sec.samsung.net/NUI/dali-ui/wiki/Fluent-API)~~ (deprecated)
* [Configuration](https://github.sec.samsung.net/NUI/dali-ui/wiki/Configuration)
* [AppFW Integration](https://github.sec.samsung.net/NUI/dali-ui/wiki/AppFW-Integration)
* [Unit & Scale](https://github.sec.samsung.net/NUI/dali-ui/wiki/Unit-&-Scale)
* [Ui Scale](https://github.sec.samsung.net/NUI/dali-ui/wiki/Ui-Scale)
* [Color & Theme](https://github.sec.samsung.net/NUI/dali-ui/wiki/Color-&-Theme)
* [View Architecture](https://github.sec.samsung.net/NUI/dali-ui/wiki/View)
* [Layout](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout)
* [Focus & Key](https://github.sec.samsung.net/NUI/dali-ui/wiki/Focus-&-Key)
* [Accessibility](https://github.sec.samsung.net/NUI/dali-ui/wiki/Accessibility)
* [Touch & Gesture](https://github.sec.samsung.net/NUI/dali-ui/wiki/Touch-&-Gesture)
* [State Management](https://github.sec.samsung.net/NUI/dali-ui/wiki/State-Management)
* [State Effect](https://github.sec.samsung.net/NUI/dali-ui/wiki/State-Effect)
* [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI)
* [Create New Component](https://github.sec.samsung.net/NUI/dali-ui/wiki/Create-New-Component)
* [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)
  * [Text](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text)
  * [ImageView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ImageView)
  * [VideoView](https://github.sec.samsung.net/NUI/dali-ui/wiki/VideoView)
  * [CanvasView](https://github.sec.samsung.net/NUI/dali-ui/wiki/CanvasView)
  * [ChartView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ChartView)
  * [Navigator](https://github.sec.samsung.net/NUI/dali-ui/wiki/Navigator)
  * [Dialog](https://github.sec.samsung.net/NUI/dali-ui/wiki/Dialog)
* [ScrollView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ScrollView)
* [WebView](https://github.sec.samsung.net/NUI/dali-ui/wiki/WebView)
* ListView & GridView
* Blur
* [Animation](https://github.sec.samsung.net/NUI/dali-ui/wiki/Animation)
* [API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/)

<br/>
