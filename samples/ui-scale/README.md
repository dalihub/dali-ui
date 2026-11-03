# UiScale 샘플

[소스](ui-scale-example.cpp)는 시스템 스케일(UI 스케일), 윈도우별 스케일, 스케일링 마스터 스위치와
`UiScalePolicy`(`INHERIT`/`ENABLED`/`DISABLED`)가 뷰의 유효 스케일을 어떻게 정하는지 한 화면에서 보여 준다.
뷰의 유효 스케일은 그 뷰가 속한 윈도우의 윈도우별 스케일과 `UiScalePolicy`가 고른 시스템 스케일 항의 곱이다.

사용하는 API는 다음과 같다. `Extension::`으로 시작하는 API는 플랫폼 통합 개발자와 모듈 제공자를 위한
extension API(`dali-ui-foundation/extension-api/`)이며 앱 개발자용 공개 API 목록에는 포함되지 않는다.

- `Extension::LayoutScaleManager::SetUiScale()`, `GetUiScale()`: 시스템 스케일
- `Extension::LayoutScaleManager::SetWindowScale()`, `GetWindowScale()`: 윈도우별 스케일
- `Extension::LayoutScaleManager::SetScalable()`, `IsScalable()`: 두 스케일을 함께 켜고 끄는 마스터 스위치
- `View::SetUiScalePolicy()`
- `Window::New()`, `LayoutController::Remove()`: Zone I의 두 번째 Window

## 로컬 빌드와 실행

일치하는 DALi Core/Adaptor/Foundation/Components 설치를 `PKG_CONFIG_PATH`와
`LD_LIBRARY_PATH`에 설정한다. 저장소 루트에서 실행한다. DALi가 지원하는 디스플레이 환경이
필요하며, Zone I은 여러 Window를 지원하는 백엔드에서만 두 번째 Window를 연다.

```sh
cmake -S samples/ui-scale -B /tmp/dali-ui-scale-sample-build
cmake --build /tmp/dali-ui-scale-sample-build
samples/ui-scale/bin/ui-scale.example
```

[공통 CMake](../common.cmake)는 실행 파일을 소스 디렉터리의 `bin/`에 만든다.

## 조작과 기대 결과

컨트롤 패널의 상태 문구는 `System x.. * Window x.. = Final x..`(`Final`이 유효 스케일)와 스위치 상태를
보여 준다. 아래 기대 결과는 윈도우별 스케일이 `win x1.00`일 때를 기준으로 한다.

- `x0.80` ~ `x2.00`, 또는 `Custom scale` 입력 후 `Apply`: 시스템 스케일을 바꾼다. Zone A의 `INHERIT` 박스와
  Zone B·C의 `ENABLED` 박스는 함께 커지거나 작아지고, `DISABLED` 컨테이너 안의 `INHERIT` 박스는 원래 크기를 유지한다.
- `win x1.00` ~ `win x2.00`: 메인 Window의 윈도우별 스케일을 바꾼다. `DISABLED` 박스를 포함해 모든 박스가
  함께 커진다. Window의 픽셀 크기는 그대로이고 한 화면에 보이는 내용이 줄어든다. `win x1.00`이 리셋이다.
- `Scaling: ON (tap to disable)` / `Scaling: OFF (tap to enable)`: 마스터 스위치를 전환한다. OFF이면
  시스템 스케일과 윈도우별 스케일이 모두 꺼져 모든 박스가 1.0으로 그려지고 상태 문구가 `Final x1.00`이
  된다. 저장된 값은 유지되며 ON으로 돌리면 다시 적용된다.
- Zone D·E·F: 모서리 반경, 보더 두께, VisualBase 장식, RenderEffect가 유효 스케일에 비례해 바뀐다.
- Zone G: `→ INHERIT 부모로` / `← DISABLED 부모로`로 박스를 옮기면 새 부모 기준의 스케일이 바로 적용된다.
- Zone H: `① Unparent` 후 위 패널에서 시스템 스케일이나 윈도우별 스케일을 바꾸고 `③ Re-add`를 누르면 바뀐
  스케일로 다시 배치된다. 상태 문구는 박스의 유효 스케일과 두 항을 보여 준다(`x1.50`, `win x2.00`에서
  `Scale x3.00 적용됨 (System x1.50 * Window x2.00)`).
- Zone I: `비교 Window 열기 (win x2.00)`는 같은 비교 카드를 가진 두 번째 Window를 메인 Window 옆에 연다.
  각 카드는 자기 Window의 `System x.. * Window x.. = Final x..`를 표시하고, 패널에서 시스템 스케일이나 스위치를
  바꾸면 두 카드가 함께 갱신된다. 두 번째 Window의 `win x..` 행은 그 Window에만 적용된다.
  `비교 Window 닫기`, `이 Window 닫기 (Esc)` 또는 그 Window에서 `Esc`로 닫는다.
- 메인 Window에서 `Esc` 또는 `Back`: 두 번째 Window를 닫고 샘플을 종료한다.

이 문서는 기대 동작을 설명하며 샘플 빌드·실행 결과를 주장하지 않는다.
