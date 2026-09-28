# Navigator

[English](Navigator.md) · [컴포넌트](Components-(kr).md) · [Dialog](Dialog-(kr).md)

Navigator는 일반 페이지와 모달의 두 스택을 관리한다. Dialog의 기본 사용법은 `dialog.Post(navigator)`와 `dialog.Dismiss()`이며 내부 모달 스택은 Navigator가 소유한다.

## 일반 페이지

```cpp
auto navigator = Navigator::New();
navigator.SetRequestedWidth(MATCH_PARENT);
navigator.SetRequestedHeight(MATCH_PARENT);
root.Add(navigator);
navigator.Push(page);
navigator.Pop();
```

`Push`, `Pop`, `InsertBefore`, `Remove`, `Clear`로 페이지를 관리한다. `GetNavigationStackCount`, `GetNavigationStackItem`, `GetModalStackCount`, `GetModalStackItem`, `GetCurrentView`로 조회한다. 초기화되지 않은 핸들의 동작은 각 공개 API 사전조건을 따른다.

## Dialog 표시

```cpp
auto dialog = AlertDialog::New();
dialog.SetTitle("Information");
dialog.SetMessage("Operation finished.");
DialogPostOptions options;
options.containerStyle = DialogContainerStyle::NoScrimPreset();
dialog.Post(navigator, options);
dialog.Dismiss();
```

투명 scrim도 입력을 차단한다. Dialog 배치는 기존 LayoutParams로 설정한다. 창 전체 모달이 필요하면 앱이 창 전체를 담당하는 Navigator를 전달한다. 자동 호스트 탐색은 없다.

## Back과 명시적 종료

사용자 Back은 `NavigateBack()`으로 전달한다. `BackRequestedSignal` 및 관리형 Dialog의 닫기 정책·요청 veto를 존중한다. 앱이 의도적으로 실행하는 `PopModal`, `Remove`, `Clear`는 정책을 우회한다. 아래 Dialog만 닫으려면 그 Dialog의 Dismiss를 사용한다.

수동 `container.SetModalContent(dialog); navigator.PushModal(container);` 경로도 유지한다. 이 경우 종료는 Navigator로 관리하며 같은 콘텐츠에 Post/Dismiss를 혼용하지 않는다. 관리형 콘텐츠/Container의 직접 Actor Add/Remove/Unparent는 금지된다.

사용자 정의 scrim View나 복잡한 scrim 조합이 필요한 경우에 이 수동 경로를 선택한다. 단순 색상·blur·비표시는 Post의 containerStyle 옵션으로 충분하다. 종료 애니메이션 중에도 두 방식을 혼용하지 않으며 기존 소유권 정리가 끝난 뒤에만 전환한다. [수동 조립 예제와 해제 순서](Dialog-(kr).md#dialogcontainer)

## 전환과 신호

`SetPageTransitionAnimationEnabled`, `SetModalTransitionAnimationEnabled`로 전환을 켜거나 끈다. `SetPageTransitionSpec`, `SetModalTransitionSpec`, `SetPageModalTransitionSpec`으로 NavigationTransitionSpec을 선택한다. Post의 `options.transitionSpec`은 해당 표시 세션의 전환을 선택한다.

DialogContainer의 전환·snap 콜백 대상은 **콘텐츠 View**다. spec 등록 키와 페이지 신호 대상은 계속 Container이며, Container 자체의 위치·크기는 전환하지 않는다. scrim은 같은 Animation에서 독립적인 linear fade를 사용한다. duration 설정을 따르며 기본 시간은 0.25초다. 콜백이 더 긴 속성 애니메이션을 추가하면 전체 완료는 그 종료까지 기다린다. 일반 페이지와 Container가 아닌 모달의 대상은 변경하지 않는다. 기존 콜백 형식과 keyframe은 유지하며 ViewAnimationSpec API는 추가하지 않는다.

`PageWillAppearSignal`, `PageDidAppearSignal`, `PageWillDisappearSignal`, `PageDidDisappearSignal`, `TransitionFinishedSignal`은 페이지 전환 신호다. 가려짐도 페이지 사라짐 신호의 원인이므로 Dialog의 실제 종료 판단에는 `HiddenSignal`을 사용한다. [전환 상세](Navigator.md#transition-specification) · [실행 예제](../samples/navigator/README.md)
