# Dialog

[English](Dialog.md) · [컴포넌트](Components-(kr).md) · [Navigator](Navigator-(kr).md)

`Dialog`는 Header·Body·Footer를 조합하는 콘텐츠 View이며 `AlertDialog`는 제목·메시지·액션 버튼을 제공한다. 기본 표시 방법은 `Post(navigator)`다. 내부에서 DialogContainer를 생성하지만 기존 Navigator 모달 스택과 배치 방식을 그대로 사용한다.

## 기본 사용법

Components 설정을 적용하고 화면에 연결한 Navigator를 전달한다. 자동으로 최상위 Navigator를 찾지 않는다.

일반적인 Dialog와 scrim 색상·blur·비표시 설정에는 이 방식을 권장한다. 사용자 정의 scrim View나 복잡한 scrim 구성에만 아래 [DialogContainer 수동 방식](#dialogcontainer)을 사용하며, 표시 중 두 관리 방식을 혼용하지 않는다.

```cpp
auto alert = AlertDialog::New();
alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");
alert.SetRequestedWidth(440.0f);
alert.SetLayoutParams(AbsoluteLayoutParams::New()
  .SetBounds(LayoutRect(0.5f, 0.5f, WRAP_CONTENT, WRAP_CONTENT))
  .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
WeakHandle<AlertDialog> weakAlert(alert);
alert.AddActionButton("Cancel").ClickedSignal().Connect(this,
  [weakAlert](View, InputEvent) {
    if(auto value = weakAlert.GetHandle()) value.Dismiss();
  });
alert.Post(navigator);
```

버튼 클릭 자체는 자동 종료하지 않는다. 검증·저장을 완료한 뒤 해당 Dialog를 Dismiss한다. 자식 버튼의 콜백에서 Dialog를 강하게 캡처하면 순환 참조가 생길 수 있으므로 약한 핸들을 사용한다.

## 표시 API

| API | 동작 |
|---|---|
| `Post(navigator)` | 내부 Container를 생성·등록한다. 성공 시 true다. |
| `Post(navigator, options)` | 새 세션의 스타일·전환을 선택한다. |
| `Dismiss(animated = true)` | 해당 Dialog만 종료한다. 위에 다른 모달이 있어도 다른 모달을 닫지 않는다. |
| `IsPosted()` | 등록 또는 종료 중이면 true다. 다른 모달에 가려짐은 종료가 아니다. |
| `SetDismissPolicy` / `GetDismissPolicy` | `NONE`, `BACK`, `SCRIM`, 기본값 `BACK_AND_SCRIM`. |
| `DismissRequestedSignal()` | `bool(Dialog, DialogDismissReason)`. true이면 사용자 닫기 요청을 차단한다. |
| `ShownSignal()` | `void(Dialog)`. 세션의 최초 등장 완료 시 한 번 발생한다. |
| `HiddenSignal()` | `void(Dialog, DialogDismissReason)`. 정리 완료 후 한 번 발생하며 콜백에서 재Post할 수 있다. |

같은 호스트 재Post는 true이며 순서·옵션·신호를 변경하지 않는다. 다른 활성 호스트, 종료 중 또는 등록 거절 구간의 Post는 false다. 부모 없는 미표시 Dialog 또는 종료 중 Dialog의 반복 Dismiss는 무동작이다.

등장 전에 닫으면 Shown 없이 Hidden만 발생할 수 있다. 종료 이유는 `PROGRAMMATIC`, `BACK`, `SCRIM`, `HOST_REMOVED`다. Navigator의 BackRequested 차단을 먼저 검사한 뒤 정책과 Dialog 요청 신호를 적용한다. 명시적 Dismiss·Navigator PopModal/Remove/Clear·호스트 소멸은 사용자 요청 차단을 우회한다.

## 표시 옵션과 scrim

```cpp
DialogPostOptions options;
options.animated = false;
options.containerStyle = DialogContainerStyle::NoScrimPreset();
// options.transitionSpec = customModalTransition;
alert.Post(navigator, options);
```

빈 스타일·전환 핸들은 기존 기본값을 사용한다. `NoScrimPreset()`은 tint·blur만 없애며 입력 영역은 유지한다. 배경 입력은 통과하지 않고 바깥 클릭 종료는 닫기 정책을 따른다. 옵션은 신규 세션에만 적용된다.

`transitionSpec`의 애니메이션·snap 콜백은 **Dialog 콘텐츠**를 받으며 Container 전체를 움직이지 않는다. Navigator가 같은 Animation에 scrim의 독립적인 linear fade-in/out을 추가한다. 신규/제거 scrim만 fade하며, 가려지거나 다시 드러나는 하위 scrim은 재전환하지 않는다. scrim의 원래 opacity와 전체 입력 차단 영역은 보존한다. 수동 DialogContainer 경로도 같은 규칙이다.

기존 NavigationTransitionSpec·keyframe·duration API를 그대로 사용하며 ViewAnimationSpec용 추가 API는 없다. 콜백에서 대상을 DialogContainer로 downcast하던 코드는 수정해야 한다. 콘텐츠의 배치 위치를 임의로 원점으로 되돌리지 말고 사용자 정의 전환의 종료 상태는 snap 콜백으로 정리한다. 제거 직전에는 position·scale·opacity를 종료 전환 시작 값으로 복원하여 Hidden 안에서의 재Post를 보호한다. 그 밖의 사용자 정의 속성은 앱이 정리한다. Shown/Hidden은 scrim뿐 아니라 전체 전환과 기존 수명 조건을 만족한 뒤 발생한다.

기존 LayoutParams·자식 외형을 보존한다. 자동 중앙 정렬·크기 축소·ScrollView 래핑·새 포커스 제한을 추가하지 않는다. 포커스 이동·복원과 키 처리 등 기존 앱 책임도 유지된다.

## 부모 관리 혼용 금지

직접 Add/Remove와 Post/Dismiss 중 하나의 방식을 사용한다. 수동 부모가 있는 Dialog의 Post/Dismiss는 ALWAYS assertion이다. 관리형 표시 중에는 Dialog·내부 Container의 직접 Add/Remove/Unparent 및 내부 콘텐츠 해제·교체도 검출 시 ALWAYS assertion이다. 종료 애니메이션 중에도 보호한다. Hidden 이후에는 수동 조립이나 재Post가 가능하다.

검출 전에 Actor 트리가 일부 변경될 수 있으므로 예외를 잡아도 rollback·안전한 계속 사용은 보장하지 않는다. 정상 Navigator 제거 API와 호스트 소멸은 허용한다.

## Dialog

`SetHeaderView`, `SetBodyView`, `SetFooterView`와 대응 Get API로 슬롯을 구성한다. 빈 View를 전달하면 슬롯을 비운다. `SetSpacing`은 섹션 간격, `SetLayoutAlignment`는 섹션 정렬이다. 전달한 자식의 padding·색상·폰트는 자식에서 설정하며 Dialog가 덮어쓰지 않는다.

## AlertDialog

`SetTitle/GetTitle`, `SetMessage/GetMessage`, `AddActionButton`, `ClearActionButtons`를 제공한다. `AddActionButton`은 자연/최소 크기의 버튼을 중앙 액션 그룹에 추가하고 반환하며, 클릭 자체로 Dialog를 닫지 않는다. 제목·메시지 갱신은 기존 Label과 사용자 속성을 보존한다. 표시 API는 Dialog에서 상속한다.

## DialogContainer

사용자 정의 scrim View 교체나 여러 자식·효과를 조합한 복잡한 scrim 구성이 필요하면 기존 수동 조립 경로를 사용한다. 단순 색상·blur·비표시는 Post 옵션으로 충분하다. 아래는 기본 사용법과 독립적인 예제이며 활성 Post에 이어서 조립하는 코드가 아니다.

```cpp
auto manualDialog = AlertDialog::New();
manualDialog.SetTitle("Custom scrim");
auto customScrim = View::New();
customScrim.SetBackgroundColor(UiColor(0x000000u, 0.6f));
// Compose custom scrim content/effects before attaching it.
auto container = DialogContainer::New();
container.SetScrim(customScrim);
container.SetModalContent(manualDialog);
navigator.PushModal(container);
// Later: remove this exact container; do not call manualDialog.Dismiss().
navigator.Remove(container);
container.SetModalContent(View{});
```

`SetModalContent/GetModalContent`, `SetScrim/GetScrim`, `ScrimClickedSignal`을 제공한다. 콘텐츠 소유 중 접근성 modal 값을 설정하고 해제·소멸 시 원래 explicit 값을 복원한다. 수동 세션은 Post가 자동으로 인계받지 않는다.

사용자 scrim의 색상·효과는 유지되며 요청 크기는 Container를 채우도록 설정된다. 클릭을 `ScrimClickedSignal`로 전달하려면 `InteractiveView`를 사용해야 한다. 위 예시의 일반 `View`는 외형만 제공한다.

**동일한 표시에서 두 관리 방식을 혼용하지 않는다.** 수동형의 표시·종료·콘텐츠 해제는 앱이 관리하며 Dialog의 관리형 신호·정책은 수동 표시에 적용되지 않는다. Remove는 지정한 Container를 즉시 제거하고 PopModal은 최상위를 제거한다. 수동→Post 전환은 Container 종료와 콘텐츠 분리 후, Post→수동 전환은 Hidden 이후에만 한다. Dismiss 호출이 반환되더라도 종료 애니메이션 중에는 정리가 완료된 것이 아니다.

## 스타일

`Dialog::New(DialogStyle)`, `AlertDialog::New(AlertDialogStyle)`, `DialogContainer::New(DialogContainerStyle)`로 불변 스타일을 전달한다. 무인자 New는 Default를 사용한다. DefaultKey·DefaultPreset·Default·Configure·Builder를 지원한다.

- DialogStyle: 자체 배경·크기·padding·간격·테두리·모서리 등.
- AlertDialogStyle: DialogStyle·액션 TextButtonStyle·제목/메시지 타이포그래피.
- DialogContainerStyle: 자동 생성 scrim의 색상·blur. 사용자 scrim은 자체 외형을 보존한다.

기본 Dialog 표면은 OneUI fallback 외형을 사용한다. 절대 Corner radius 44, 흰색 alpha 0.6,
두 개의 모서리 cutout 그림자(blur 8/32, Y offset 0/16, 검정 alpha 0.05/0.1),
내부 테두리(두께 2, offset -1, #FCFCFF alpha 0.05)다. AlertDialog도 별도 DialogStyle을
지정하지 않으면 이 표면을 사용한다. 기본 scrim은 검정 alpha 0.4와 blur radius 100이며,
NoScrimPreset은 둘 다 명시적으로 해제한다. Corner·그림자는 기존 DALi UI scale 경로를
사용하며 OneUI 픽셀 환산을 추가하지 않는다.

이는 OneUI 전체 light/dark 테마 이식이 아니라 fallback 값이다. 테마별 값은 기존
UiConfig의 style creator로 재정의한다. 제목/메시지 타이포그래피와 액션 버튼 색상은
유지한다. 투명하고 각진 표면이 필요하면
Configure에서 배경 alpha 0, radius와 테두리 두께 0, 빈 ShadowStack을 명시한다.
스타일은 생성 시 적용되며 이후 인스턴스 설정을 덮어쓰지 않는다.

### 콘텐츠 배치 기본값

콘텐츠 높이를 사용하는 경우 폭은 RequestedWidth로 지정하고 AbsoluteLayoutParams의 크기는 `WRAP_CONTENT`로 둔다. 서로 다른 고정 요청 폭과 고정 bounds 폭을 중복 지정하면 텍스트 측정 폭과 최종 배치 폭이 달라질 수 있다.

OneUI처럼 제목 → 본문 → 버튼을 위에서부터 쌓으며 Footer 하단 고정이나 Body weight를 추가하지 않는다. Dialog 기본 폭은 908, 높이는 `WRAP_CONTENT`, 자체 padding은 `(44, 44, 36, 32)`, 섹션 간격은 12다. OneUI의 공통 좌우 슬롯 inset을 Dialog 자체 padding에 합쳐 적용하므로 전달한 자식 View의 padding은 덮어쓰지 않는다.

AlertDialog가 생성한 제목·본문 Label은 여러 줄을 허용한다. 액션 버튼은 균등하게 늘리지 않고 자연/최소 크기를 유지한 중앙 그룹으로 배치한다. 기본 버튼 최소 크기는 144×64, padding은 `(24, 4)`이며 명시적 ActionButtonStyle 또는 앱의 TextButtonStyle provider가 있으면 해당 버튼 배치를 우선한다. 생성 액션 행은 높이 `WRAP_CONTENT`, 위 padding 12, 버튼 간격 32가 기본이다. `SetActionRowHeight`, `SetActionRowPadding`, `SetActionButtonSpacing`은 생성 액션 행만 설정하며 외부 Footer에는 적용하지 않는다. 명시적인 0 이상 고정 행 높이도 지원한다.

호스트 크기와 버튼 개수·문구에 맞는 폭은 기존 스타일/배치 API로 지정한다(위 예제는 440). 고정 높이를 지정할 수도 있지만 남는 공간이나 콘텐츠 제약이 생길 수 있다. Post가 크기를 자동 축소하거나 버튼 행을 줄바꿈하거나 ScrollView를 추가하지 않는다.

표시 API 추가는 기존 공개 핸들 크기·상속·함수 서명을 유지한다. 다만 Integration 구현에 직접 의존하는 외부 파생 클래스는 내부 데이터·override 변경 때문에 재컴파일해야 한다. 전체 ABI를 바이너리 비교 도구로 인증했다는 의미는 아니다.

[빌드 가능한 예제](../samples/dialog/README.md) · [수동 검증 시나리오](../manual-tests/dali-ui-components/tc/tc-dialog-basics.md)
