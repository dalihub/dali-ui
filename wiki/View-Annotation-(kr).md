[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-Annotation)

# App Entity를 위한 View Annotation

이 문서는 dali-ui 애플리케이션이 `Dali::Ui::View`에 애플리케이션 정의
entity 정보를 연결하는 방법을 설명합니다. Tizen application entity
서비스는 annotation이 설정된 View를 `Tizen.Action.View`로 노출할 수
있습니다.

Annotation API는 `Dali::Ui::View`를 통해 사용하며, entity를 표현하는
실제 View에 annotation을 설정합니다.

<br/>

## 1. Annotation 설정

Annotation은 애플리케이션이 정의하는 identifier, type, 자유 형식 정보
문자열 하나씩으로 구성됩니다.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

Dali::Ui::View lightCard = Dali::Ui::View::New();
lightCard.SetAnnotation(
  "living-room.light",
  "Tizen.Entity.Light",
  R"({"power":true,"level":72})");
```

`SetAnnotation()`을 다시 호출하면 세 값이 모두 교체됩니다. 문자열은
View에 저장되며 View가 살아 있거나 `ClearAnnotation()`을
호출하기 전까지 유지됩니다.

> [!NOTE]
> Annotation은 event-side metadata입니다. 등록된 View property가 아니며 property
> notification, serialization, update/render thread에 참여하지 않습니다.

<br/>

## 2. 읽기와 삭제

```cpp
Dali::String entityId;
Dali::String entityType;
Dali::String entityInfo;

if(lightCard.GetAnnotation(entityId, entityType, entityInfo))
{
  // 저장된 값을 사용합니다.
}

lightCard.ClearAnnotation();
```

Annotation이 없으면 `GetAnnotation()`은 `false`를 반환하며 세 출력값은
변경하지 않습니다. Annotation이 존재하더라도 `entityInfo`는 비어 있을
수 있습니다.

<br/>

## 3. View ID와 Entity ID

두 identifier는 수명과 목적이 다릅니다.

| Identifier | 출처 | 용도 |
|---|---|---|
| View ID | `view.GetId()` | `FindById`가 사용하는 runtime identifier. View 수명 동안 유효합니다. |
| Entity ID | `SetAnnotation()`의 첫 번째 값 | `living-room.light`처럼 애플리케이션이 정의하는 논리적 identity입니다. |

View ID를 application entity ID로 저장하지 마세요. View를 다시 만들면
View ID는 달라질 수 있지만 논리적인 entity ID는 그대로 유지되어야
합니다.

<br/>

## 4. Focus 연동

평소와 같이 `Dali::Ui::FocusManager`를 사용합니다.

```cpp
lightCard.SetFocusable(true);
window.Add(lightCard);

Dali::Ui::FocusManager::Get().SetCurrentFocusView(lightCard);
```

dali-ui는 `FocusManager`가 현재 focus한 View를 dali-adaptor에 자동으로
전달합니다. 애플리케이션이 provider를 직접 등록할 필요가 없습니다.
따라서 일반적인 dali-ui focus 설정 후 `GetFocusedView`는 현재 focus된
View를 반환합니다.

<br/>

## 5. Tizen Action 매핑

Tizen에서 dali-adaptor는 View 상태를 표준 action contract로 변환합니다.

| Action | 의미 |
|---|---|
| `FindById` | 10진수 View ID로 View를 찾습니다. |
| `GetAnnotatedViews` | Annotation과 visibility 조건을 만족하는 View를 반환합니다. |
| `GetFocusedView` | `Ui::FocusManager`가 현재 관리하는 View를 반환합니다. |
| `ToPresentation` | 플랫폼 presentation document를 생성합니다. |

Action 이름의 `View`는 외부 `Tizen.Action.View` contract의 용어입니다.
애플리케이션 코드는 일반적인 `Dali::Ui::View` API를 계속 사용합니다.

<br/>

## 6. Tizen RPC 권한

rpc-port를 통해 Action에 연결하는 Tizen client는 애플리케이션 manifest에
표준 RPC 연결 권한을 선언해야 합니다.

```xml
<privileges>
  <privilege>http://tizen.org/privilege/appmanager.launch</privilege>
  <privilege>http://tizen.org/privilege/datasharing</privilege>
</privileges>
```

이 권한은 Tizen client/transport 경로에만 필요하며 portable Ubuntu 또는
Windows 샘플에는 필요하지 않습니다. 추가 method 권한은 dali-ui가 아니라
canonical `Tizen.Action.View` contract에서 정의합니다.

<br/>

## 7. 데스크톱 Interactive Sample

[`app-entity-annotation`](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/app-entity-annotation) 샘플은
Ubuntu와 Windows에서 실행할 수 있습니다. 실제 View annotation/focus
API를 사용하고 Tizen TIDL/rpc-port transport 대신 Local Action Console을
제공합니다.

샘플에서 다음 내용을 확인할 수 있습니다.

- 현실적인 dashboard 형태의 annotation View 카드 세 개
- click, touch, keyboard focus 변경
- runtime annotation 삭제와 복원
- View ID와 application entity ID의 차이
- 로컬 `FindById`, `GetAnnotatedViews`, `GetFocusedView`,
  `ToPresentation` 요청 흐름

Local simulator는 Tizen RPC serialization, privilege, host lifecycle을
검증한다고 가정하지 않습니다. 해당 항목은 dali-adaptor integration
test에서 검증합니다.

<br/>

## 8. 권장 사항

- View를 다시 생성해도 유지되는 entity ID를 사용하세요.
- 애플리케이션 또는 플랫폼 contract가 소유하는 namespaced entity type을 사용하세요.
- `entityInfo`는 간결하게 유지하고 consumer가 의존한다면 형식을 versioning하세요.
- View가 더 이상 검색되지 않아야 하면 annotation을 삭제하세요.
- View ID가 process나 View 수명을 넘어 유지된다고 가정하지 마세요.

<br/>

---

[← 목록으로](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
