## 개요

`Dali::Ui::View`의 Accessibility API는 View의 의미, 읽을 정보, 상태와 관계를 접근성 트리에 전달합니다. Screen Reader는 이 정보를 조회하여 현재 highlighted View를 발화하고 접근성 action을 View에 전달합니다.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

View button = View::New();
button.SetAccessibilityRole(Accessibility::Role::BUTTON);
button.SetAccessibilityName("확인");
button.SetAccessibilityDescription("입력한 내용을 저장합니다");
button.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::NAME);
button.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::ROLE);
button.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::DESCRIPTION);

window.Add(button);
```

> [!IMPORTANT]
> **Keyboard focus와 accessibility highlight는 서로 다른 상태입니다.** `FocusManager`의 focus는 키 입력 대상을 정하지만, accessibility highlight는 Screen Reader가 읽을 대상을 정합니다. Accessibility API를 설정해도 View가 focusable로 바뀌지 않으며, highlight를 옮겨도 keyboard focus는 바뀌지 않습니다.

<br/>

## 기본 정보 설정

Screen Reader가 View를 이해하는 데 필요한 기본 정보는 name, description, value, role입니다.

API | 용도 | 예
--|--|--
`SetAccessibilityName()` | 사용자가 대상을 식별할 짧은 이름 | `"Wi-Fi"`
`SetAccessibilityDescription()` | 이름을 보충하는 설명 | `"사용 가능한 네트워크를 엽니다"`
`SetAccessibilityValue()` | 현재 값 | `"50%"`, `"켜짐"`
`SetAccessibilityRole()` | 대상의 의미와 지원 동작 | `BUTTON`, `ENTRY`, `ADJUSTABLE`

```cpp
View volume = View::New();
volume.SetAccessibilityRole(Accessibility::Role::ADJUSTABLE);
volume.SetAccessibilityName("음량");
volume.SetAccessibilityValue("50%");
```

role은 View의 구현 클래스가 아니라 **사용자에게 제공하는 기능**에 맞게 선택합니다. 예를 들어 일반 `View`로 만든 클릭 가능한 컨트롤도 동작이 버튼이면 `BUTTON`을 사용합니다.

주로 사용하는 role은 다음과 같습니다.

분류 | Role
--|--
동작/선택 | `BUTTON`, `TOGGLE_BUTTON`, `CHECK_BOX`, `RADIO_BUTTON`, `LINK`
입력/값 | `ENTRY`, `PASSWORD_TEXT`, `ADJUSTABLE`, `SPIN_BUTTON`, `PROGRESS_BAR`, `SCROLL_BAR`
구조 | `CONTAINER`, `LIST`, `LIST_ITEM`, `TAB_LIST`, `TAB`, `MENU`, `MENU_ITEM`, `TOOL_BAR`
콘텐츠 | `TEXT`, `IMAGE`, `HEADER`, `SCENE_3D`, `MODEL`
컨텍스트 | `ALERT`, `NOTIFICATION`, `DIALOG`, `POPUP_MENU`

전체 role은 `Accessibility::Role`을 참고하세요. 의미 있는 role을 지정하면 기본적으로 highlightable 대상이 됩니다.

> [!NOTE]
> Custom View가 `ViewImpl::OnAccessibilityRequestName()`, `OnAccessibilityRequestDescription()`, `OnAccessibilityRequestValue()`에서 `true`를 반환하면 그 값이 우선합니다. `false`를 반환할 때 저장된 값으로 fallback합니다. 자세한 내용은 [Custom View 구현](#custom-view-구현)을 참고하세요.

<br/>

## 접근성 동작 속성

```cpp
view.SetAccessibilityHidden(false);
view.SetAccessibilityHighlightable(true);
view.SetAccessibilityScrollable(false);
view.SetAccessibilityModal(false);
view.SetAutomationId("settings-wifi-button");
```

API | 설명
--|--
`SetAccessibilityHidden(bool)` | `true`이면 View를 접근성 client에서 숨깁니다. 장식용 View처럼 읽히면 안 되는 대상에 사용합니다.
`SetAccessibilityHighlightable(bool)` | role 기반 기본값 대신 highlight 가능 여부를 명시적으로 override합니다.
`ResetAccessibilityHighlightable()` | 명시적 override를 지우고 role 기반 동작으로 되돌립니다. role이 `NONE`이 아니면 기본적으로 highlightable입니다.
`SetAccessibilityScrollable(bool)` | View가 접근성 scroll 동작을 제공하는지 나타냅니다.
`SetAccessibilityModal(bool)` | 접근성 탐색 범위를 제한해야 하는 modal context임을 나타냅니다.
`SetAutomationId()` | UI 자동화 도구가 사용할 안정적인 식별자를 설정합니다.

각 setter에는 대응하는 `Is...()` 또는 `Get...()` API가 있습니다.

> [!WARNING]
> `SetAccessibilityHidden(true)`는 화면의 visibility를 변경하지 않습니다. 반대로 화면에서 보이지 않는 View를 접근성에서 노출해야 한다는 뜻도 아닙니다. 시각 상태와 접근성 노출 상태를 일관되게 관리하세요.

<br/>

## 발화할 정보 선택

`Accessibility::ReadingInfo`는 Screen Reader가 View를 읽을 때 어떤 의미 정보를 사용할지 지정합니다. enum은 bit mask가 아니므로 항목별 API를 사용합니다.

```cpp
view.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::NAME);
view.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::ROLE);
view.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::DESCRIPTION);
view.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::STATE);

view.RemoveAccessibilityReadingInfo(Accessibility::ReadingInfo::DESCRIPTION);

if(view.HasAccessibilityReadingInfo(Accessibility::ReadingInfo::STATE))
{
  // STATE가 발화 정보에 포함되어 있음
}

view.ClearAccessibilityReadingInfo();
```

항목 | 전달하는 정보
--|--
`NAME` | `AccessibilityName`
`ROLE` | `AccessibilityRole`
`DESCRIPTION` | `AccessibilityDescription`
`STATE` | 현재 접근성 state

<br/>

## State 관리

애플리케이션이나 component가 소유하는 의미 상태는 `AddAccessibilityState()`, `RemoveAccessibilityState()`, `ClearAccessibilityStates()`로 관리합니다.

```cpp
checkBox.AddAccessibilityState(Accessibility::State::ENABLED);
checkBox.AddAccessibilityState(Accessibility::State::CHECKED);

checkBox.RemoveAccessibilityState(Accessibility::State::CHECKED);
bool checked = checkBox.HasAccessibilityState(Accessibility::State::CHECKED);
```

State | 사용 시점
--|--
`ENABLED` | 접근성 interaction이 가능한 상태
`SELECTED` | 목록, tab 등의 항목이 선택된 상태
`CHECKED` | check/toggle 값이 켜진 상태
`BUSY` | 값 또는 콘텐츠를 갱신 중인 상태
`EXPANDED` | 접힌 콘텐츠가 펼쳐진 상태

이 API로 설정한 state는 visibility, sensitivity, focus 등 DALi가 계산하는 runtime state와 합쳐져 accessibility client에 노출됩니다. 따라서 `FOCUSED`, `SHOWING`, `HIGHLIGHTED` 같은 runtime state를 이 API로 직접 추가하지 않습니다.

<br/>

## View 사이의 관계

화면 배치만으로 의미를 알 수 없는 경우 `Accessibility::RelationType`으로 View 사이의 관계를 표현합니다.

```cpp
View title = Label::New("비밀번호");
View input = View::New();
View error = Label::New("8자 이상 입력하세요");

title.AddAccessibilityRelation(Accessibility::RelationType::LABEL_FOR, input);
input.AddAccessibilityRelation(Accessibility::RelationType::LABELLED_BY, title);

error.AddAccessibilityRelation(Accessibility::RelationType::ERROR_MESSAGE, input);
input.AddAccessibilityRelation(Accessibility::RelationType::ERROR_FOR, error);
```

자주 사용하는 대응 관계는 다음과 같습니다.

소스 → 대상 | 대상 → 소스
--|--
`LABEL_FOR` | `LABELLED_BY`
`DESCRIPTION_FOR` | `DESCRIBED_BY`
`CONTROLLER_FOR` | `CONTROLLED_BY`
`FLOWS_TO` | `FLOWS_FROM`
`NODE_PARENT_OF` | `NODE_CHILD_OF`
`EMBEDS` | `EMBEDDED_BY`
`DETAILS` | `DETAILS_FOR`
`ERROR_MESSAGE` | `ERROR_FOR`

relation은 한 방향씩 저장되므로 client가 양쪽 관계를 조회해야 한다면 위 예제처럼 대응 관계를 각각 추가합니다.

```cpp
input.RemoveAccessibilityRelation(Accessibility::RelationType::LABELLED_BY, title);
input.ClearAccessibilityRelations();
bool exists = input.HasAccessibilityRelation(Accessibility::RelationType::LABELLED_BY, title);
```

대상 View는 weak handle로 보관되므로 대상이 소멸되었다고 relation 때문에 수명이 연장되지는 않습니다.

<br/>

## 다국어 접근성 문자열

### 번역 resource 연결

name과 description을 `UiLocalizationManager` resource에 연결하면 locale 변경 시 번역된 값이 갱신됩니다.

```cpp
view.SetTranslatableAccessibilityName("IDS_SETTINGS_WIFI", "settings");
view.SetTranslatableAccessibilityDescription("IDS_SETTINGS_WIFI_DESCRIPTION", "settings");

Dali::String nameResourceId = view.GetTranslatableAccessibilityName();

view.ClearTranslatableAccessibilityName();
view.ClearTranslatableAccessibilityDescription();
```

domain 인자를 생략하면 default domain을 사용합니다. `SetAccessibilityName()`이나 `SetAccessibilityDescription()`으로 명시적 문자열을 다시 설정하면 해당 translation binding과 language span은 해제됩니다. localization 설정 방법은 [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI-(kr))를 참고하세요.

### 한 문자열 안의 언어 구간

한 name 또는 description 안에 여러 언어가 섞여 있으면 language span을 추가합니다.

```cpp
view.SetAccessibilityName("Hello 세계");

bool english = view.AddAccessibilityNameLanguageSpan(0u, 5u, "en-US");
bool korean  = view.AddAccessibilityNameLanguageSpan(6u, 2u, "ko-KR");
```

`start`와 `length`는 UTF-8 byte offset이 아니라 **Unicode code point 기준**입니다. 길이가 0이거나 locale이 비어 있거나, 문자열 범위를 벗어나거나, 기존 span과 겹치면 `false`를 반환합니다.

description에는 `AddAccessibilityDescriptionLanguageSpan()`을 사용합니다. 문자열을 변경하면 기존 span이 제거되므로 문자열을 먼저 설정한 후 span을 추가하세요.

```cpp
view.ClearAccessibilityNameLanguageSpans();
view.ClearAccessibilityDescriptionLanguageSpans();
```

<br/>

## Collection 정보

목록처럼 반복되는 항목에는 collection container와 zero-based index를 설정할 수 있습니다.

```cpp
list.SetAccessibilityCollectionContainer(true);

for(int32_t index = 0; index < itemCount; ++index)
{
  items[index].SetAccessibilityCollectionIndex(index);
}

items[0].ClearAccessibilityCollectionIndex(); // Get 결과는 -1
```

`SetAccessibilityCollectionIndex(-1)`도 index를 제거합니다. 항목 삽입, 삭제, 정렬 후에는 실제 순서와 index가 일치하도록 갱신하세요.

<br/>

## 초기 highlight와 실행 중 highlight 이동

highlight 요청 시점에 따라 사용하는 API가 다릅니다.

상황 | API | 특징
--|--|--
페이지, window, modal이 처음 표시됨 | `View::SetRequestInitialAccessibilityHighlight(true)` | accessibility context를 구성할 때 Screen Reader가 초기 대상을 선택하도록 metadata를 제공합니다.
이미 표시된 안정적인 화면에서 대상을 즉시 이동 | `Extension::View::GrabAccessibilityHighlight(view)` | 현재 DALi accessibility highlight를 대상 View로 옮기고 `HIGHLIGHTED` 변경을 client에 알립니다.

### 페이지의 초기 highlight

페이지가 accessibility tree에 나타나기 전에 초기 대상을 지정합니다.

```cpp
View title = Label::New("네트워크 설정");
title.SetAccessibilityRole(Accessibility::Role::HEADER);
title.SetRequestInitialAccessibilityHighlight(true);

page.Add(title);
window.Add(page);
```

페이지가 사라지거나 재사용될 때 더 이상 초기 대상이 아니면 `SetRequestInitialAccessibilityHighlight(false)`로 해제합니다. 같은 context에 여러 초기 대상을 지정하면 최종 선택은 Screen Reader 정책에 따라 달라질 수 있으므로 논리적인 초기 대상 하나만 지정하는 것이 좋습니다.

> [!IMPORTANT]
> 새로운 페이지나 modal의 `SHOWING` 처리와 동시에 `GrabAccessibilityHighlight()`를 호출하면, Screen Reader의 context 재구성과 highlight 발화 요청이 서로 다른 비동기 작업으로 처리될 수 있습니다. 초기 화면은 `SetRequestInitialAccessibilityHighlight()`를 사용하고, `GrabAccessibilityHighlight()`는 화면 구성이 안정된 뒤 발생하는 명시적 사용자 동작에 사용하세요. 임의의 timeout으로 순서를 맞추는 방식은 권장하지 않습니다.

### 실행 중 강제 highlight 이동

이 API는 extension-api에 있으므로 extension header를 포함합니다.

```cpp
#include <dali-ui-foundation/extension-api/view.h>

bool moved = Dali::Ui::Extension::View::GrabAccessibilityHighlight(targetView);
if(!moved)
{
  // Screen Reader bridge가 비활성 상태이거나 highlight를 적용하지 못함
}
```

다른 View가 highlighted 상태라면 기존 highlight를 clear한 뒤 새 View로 이동합니다. `AccessibilityHighlightedSignal()`은 이전 View에 `false`, 새 View에 `true`로 발생하고 접근성 client에는 `HIGHLIGHTED` state 변경이 전달됩니다. Screen Reader는 이 변경을 바탕으로 새 대상을 발화할 수 있습니다.

> [!NOTE]
> 대상 View는 keyboard focusable일 필요가 없고 keyboard focus도 이동하지 않습니다. 다만 accessibility tree에 노출되고 화면 구성이 안정된 View를 대상으로 사용하세요. 일반적인 Screen Reader 탐색 가능 여부는 `SetAccessibilityHighlightable()`로 별도 관리합니다.

반환값의 의미는 다음과 같습니다.

* `true`: DALi에서 요청한 View가 highlight를 가진 상태입니다.
* `false`: accessibility bridge가 비활성 상태이거나 highlight를 적용/해제하지 못했습니다.

`true`는 Screen Reader가 발화를 시작하거나 완료했다는 acknowledgement가 아닙니다. 이미 같은 View가 highlighted 상태이면 `true`를 반환하지만 새로운 `HIGHLIGHTED` 이벤트를 보내지 않으므로 재발화 API로 사용할 수 없습니다.

```cpp
bool cleared = Dali::Ui::Extension::View::ClearAccessibilityHighlight(targetView);
```

`ClearAccessibilityHighlight()`는 해당 View가 현재 highlighted인 경우에만 `true`를 반환합니다.

<br/>

## Accessibility signal

### Highlight 변경

```cpp
view.AccessibilityHighlightedSignal().Connect(
  &tracker,
  [](View source, bool highlighted)
  {
    // source의 accessibility highlight가 변경됨
  });
```

signal type은 `Signal<void(View, bool)>`입니다. Screen Reader 탐색 또는 extension highlight API로 실제 상태가 변경될 때 발생합니다. signal은 상태 변경을 관찰하기 위한 것이므로 애플리케이션에서 직접 `Emit()`하지 않습니다.

### 읽기 lifecycle

```cpp
view.AccessibilityReadingStatusChangedSignal().Connect(
  &tracker,
  [](View source, Accessibility::ReadingStatus status)
  {
    switch(status)
    {
      case Accessibility::ReadingStatus::PAUSED:
        break;
      case Accessibility::ReadingStatus::RESUMED:
        break;
      default:
        break;
    }
  });
```

signal type은 `Signal<void(View, Accessibility::ReadingStatus)>`이며 다음 상태를 전달합니다.

Status | 의미
--|--
`SKIPPED` | 완료 전에 읽기가 건너뛰어짐
`PAUSED` | 읽기가 일시 정지됨
`RESUMED` | 일시 정지된 읽기가 재개됨
`CANCELLED` | 대기 중이거나 진행 중인 읽기가 취소됨
`STOPPED` | 읽기가 중지되거나 완료됨

<br/>

## Custom View 구현

새 component가 접근성 action이나 동적 값을 제공하려면 handle인 `View`가 아니라 `ViewImpl`의 virtual API를 override합니다.

```cpp
class VolumeViewImpl : public ViewImpl
{
public:
  bool OnAccessibilityActivate() override
  {
    ToggleMute();
    return true;
  }

  bool OnAccessibilityValueChange(bool isIncreased) override
  {
    ChangeVolume(isIncreased ? 1 : -1);
    return true;
  }

  bool OnAccessibilityRequestValue(Dali::String& value) override
  {
    value = GetVolumeText();
    return true;
  }
};
```

Virtual API | 접근성 요청
--|--
`OnAccessibilityActivate()` | 대상 실행
`OnAccessibilityEscape()` | 현재 context 닫기 또는 뒤로 이동
`OnAccessibilityValueChange(bool isIncreased)` | 값 증가/감소
`OnAccessibilityScrollToChild(View child)` | scroll container가 child를 화면에 표시
`OnAccessibilityPan(PanGesture)` | 접근성 pan 처리
`OnAccessibilityZoom()` | 접근성 zoom 처리
`OnAccessibilityRequestName()` | 동적 name 조회
`OnAccessibilityRequestDescription()` | 동적 description 조회
`OnAccessibilityRequestValue()` | 동적 value 조회

action callback은 요청을 처리했으면 `true`, 지원하지 않거나 처리하지 못했으면 `false`를 반환합니다. 동적 문자열 callback의 반환 규칙은 특히 중요합니다.

* `true`: 출력 인자의 값을 사용합니다. 의도적으로 빈 문자열을 반환하는 것도 가능합니다.
* `false`: `SetAccessibilityName()`, `SetAccessibilityDescription()`, `SetAccessibilityValue()`로 저장된 값으로 fallback합니다.

일반 애플리케이션은 이 virtual을 직접 호출하지 않습니다. Accessibility bridge가 Screen Reader 요청을 받아 적절한 callback으로 dispatch합니다.

Custom View의 handle/impl 구조는 [View Architecture](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-(kr)#4-view-inheritance)를 참고하세요.

<br/>

## Raw attribute

정형 API가 제공되지 않는 backend attribute가 꼭 필요한 경우에만 raw attribute API를 사용합니다.

```cpp
view.AppendAccessibilityAttribute("vendor-key", "vendor-value");
view.RemoveAccessibilityAttribute("vendor-key");
```

> [!WARNING]
> `ClearAccessibilityAttributes()`는 직접 추가한 raw attribute뿐 아니라 initial highlight, collection, reading info 같은 typed attribute와 name/description language span도 함께 제거합니다. 하나의 설정만 해제하려면 해당 typed API 또는 `RemoveAccessibilityAttribute()`를 사용하세요.

<br/>

## 문제 확인

현상 | 확인할 내용
--|--
View가 읽히지 않음 | role과 name이 설정되었는지, `SetAccessibilityHidden(true)` 상태가 아닌지 확인합니다.
일반 탐색으로 highlight되지 않음 | `IsAccessibilityHighlightable()`과 role을 확인합니다.
`GrabAccessibilityHighlight()`가 `false` 반환 | Screen Reader/accessibility bridge가 활성화되어 있는지 확인합니다.
`GrabAccessibilityHighlight()`가 `true`인데 다시 읽지 않음 | 이미 같은 View가 highlighted인지 확인합니다. 이 API는 재발화 API가 아닙니다.
새 페이지에서 다른 대상이 읽힘 | `GrabAccessibilityHighlight()` 대신 표시 전에 `SetRequestInitialAccessibilityHighlight(true)`를 설정합니다.
키 입력 대상이 바뀌지 않음 | highlight는 keyboard focus를 변경하지 않습니다. [FocusManager](https://github.sec.samsung.net/NUI/dali-ui/wiki/Focus-&-Key-(kr))를 별도로 사용합니다.
language span 추가가 실패 | code-point 범위, 빈 locale, 길이 0, 기존 span과의 겹침을 확인합니다.

전체 API를 실행하고 결과를 화면과 stdout으로 확인하는 예제는 [accessibility-view-api sample](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/accessibility-view-api)을 참고하세요.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
