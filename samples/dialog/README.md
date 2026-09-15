# Dialog 예제

일반적인 Dialog 표시는 **`Post(navigator)` / `Dismiss()`를 권장**한다. 이 샘플도 내부 Container를 직접 만들거나 Dialog를 root에 Add하지 않고 이 방식만 사용한다. Post 버튼은 선택한 `DialogPostOptions.transitionSpec`·`containerStyle`·`animated`로 표시한다.

단순 색상·blur·비표시 설정에는 옵션으로 충분하다. 사용자 정의 scrim View 교체나 복잡한 배경 구성이 필요하면 기존 `DialogContainer.SetScrim`·`SetModalContent`·`Navigator.PushModal` 경로를 사용한다. **동일한 표시에서 수동 Add/Remove와 Post/Dismiss를 혼용하지 않는다.** 자세한 선택 기준과 독립적인 수동 예제는 [Dialog 문서](../../wiki/Dialog-(kr).md)를 참고한다.

세 계열의 기본/사용자 style과 modal 제거·재설치를 함께 관찰하려면
[등록된 Dialog 수동 시나리오](../../manual-tests/dali-ui-components/tc/tc-dialog-basics.md)를 사용한다.

`Components::UiConfig`를 적용한 뒤 `AlertDialog::New()`로 기본 스타일을 사용한다. 제목·본문·action 내용과 이후의 spacing 변경은 인스턴스 API가 담당한다. 사용자 외형은 `DialogStyle::Default().Configure()`와 `AlertDialogStyle`의 조합으로 재정의할 수 있다.

배경색을 샘플에서 덮어쓰지 않으므로 기본 OneUI fallback 표면(둥근 모서리, 반투명 흰색 배경, 2단 그림자, 내부 테두리)을 확인할 수 있다. Scrim 설정의 Default는 기본 tint·blur를 사용하고 None은 둘 다 끈다. 전체 OneUI 테마나 타이포그래피를 재현하는 예제는 아니다.

OneUI처럼 제목 → 본문 → 버튼을 위에서부터 쌓고 높이는 콘텐츠에 맞춘다. Dialog 자체 여백은 좌우 44·위 36·아래 32, 기본 섹션 간격은 12다. 생성한 제목·본문은 여러 줄을 허용한다. 버튼은 기본 최소 크기 144×64·간격 32로 중앙에 모이며, 생성 액션 행에만 위 여백 12를 적용한다. Footer 하단 고정이나 Body weight는 없다.

샘플은 `SetRequestedWidth(440)`으로 측정 폭을 지정하고, 기존 AbsoluteLayoutParams에는 중앙 위치와 `WRAP_CONTENT` 크기를 지정한다. 높이는 콘텐츠에 맞춘다. 기본 스타일 폭 908과 다른 고정 배치 폭을 중복 지정하면 텍스트 측정 폭과 배치 폭이 달라질 수 있으므로 피한다. 440은 480 폭의 샘플 창에 맞춘 값이다. 더 작은 호스트, 큰 UI scale, 긴 버튼 문구나 많은 액션은 앱에서 폭·버튼 스타일 또는 별도 콘텐츠 구성을 선택해야 한다. Post가 자동으로 축소하거나 스크롤을 추가하지 않는다.

DialogStyle의 padding은 Dialog 자체의 여백이다. 사용자 정의 Header·Body·Footer의 padding과 외형은 해당 View에 직접 설정하며, Dialog는 이를 덮어쓰지 않는다. AlertDialog가 직접 생성하는 제목·본문·버튼의 스타일은 AlertDialogStyle에서 설정한다.

## 빌드와 실행

DALi Core/Adaptor와 수정된 dali-ui가 설치된 환경에서 저장소 루트를 작업 위치로 사용한다. `DESKTOP_PREFIX`는 실제 설치 경로여야 한다.

```sh
export PKG_CONFIG_PATH="$DESKTOP_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH="$DESKTOP_PREFIX/lib:$LD_LIBRARY_PATH"
cmake -S samples/dialog -B /tmp/dali-dialog-sample-build
cmake --build /tmp/dali-dialog-sample-build --parallel 4
samples/dialog/bin/dialog.example
```

GUI 실행에는 해당 DALi Adaptor의 디스플레이·그래픽 환경이 필요하다. 빌드 성공만으로 화면 실행이나 스크린리더 검증을 대신하지 않는다.

## 조작과 기대 결과

- Transition: Fade → Slide → Scale 순으로 선택한다. 초기값은 Slide다.
- Scrim: Default ↔ None을 선택한다. None은 tint·blur를 없애지만 배경 입력 차단은 유지된다.
- Animation: On ↔ Off를 선택한다. Post·Cancel/OK뿐 아니라 Back·scrim 닫기에도 적용된다.
- Speed: Normal(0.35초) ↔ Slow(1.4초)를 선택한다. 콘텐츠와 scrim의 움직임을 천천히 비교할 수 있다.
- Post: 선택한 설정으로 AlertDialog를 표시한다. 표시 중에는 배경 조작 버튼을 사용할 수 없으므로 닫은 뒤 다음 설정을 선택한다.
- Cancel/OK: last action을 기록하고 해당 Dialog를 Dismiss한다. 종료 후 다시 Post할 수 있다.
- Spacing +/-: section 간격이 8씩 변하고 0 미만으로 내려가지 않는다.
- Align: FILL → START → CENTER → END 순으로 정렬을 변경한다.
- Toggle Msg: 본문 제거와 재생성을 반복한다. 제목과 action 버튼은 유지된다.
- Escape/Back: Dialog 표시 중에는 NavigateBack으로 닫기 요청을 전달하고, 미표시 상태에서는 앱을 종료한다.

상태 영역에는 전환 종류·scrim·애니메이션·속도, 마지막 Post의 Accepted/Rejected 결과, IsPosted, Shown/Hidden 횟수와 마지막 종료 이유가 표시된다. Post Accepted는 등장 완료가 아니다. Shown/Hidden은 실제 신호 콜백에서만 증가하며, 표시를 정상적으로 완료한 후 닫으면 각각 한 번 증가한다. 종료 중 Back을 반복해도 배경 페이지를 Pop하지 않는다.

## 전환 비교 순서

1. 초기 Slide / Default / On 상태에서 Speed를 Slow로 바꾸고 Post한다. Dialog는 중앙 도착 위치보다 80px 아래에서 올라오고, scrim은 제자리에서 fade-in해야 한다. 콘텐츠 자체의 opacity는 Slide에서 바꾸지 않는다.
2. Cancel/OK로 닫는다. Dialog는 아래로 내려가고 scrim은 제자리에서 fade-out해야 한다. 다시 Post하여 위치가 누적 이동하지 않는지 확인한다.
3. Transition을 Scale로 바꾼다. Dialog만 0.8 → 1.0 배율로 커지고 scrim 영역은 확대되지 않아야 한다.
4. Transition을 Fade로 바꾼다. Dialog는 이동하거나 확대되지 않고 opacity만 변해야 한다.
5. Scrim을 None으로 바꾸고 각 전환을 반복한다. 배경막은 보이지 않지만 배경 버튼은 눌리지 않아야 한다.
6. Animation을 Off로 바꾼다. 표시·Cancel/OK·Back·scrim 닫기에서 전환이 생략되는지 확인한다.

`dialog-transition.h`는 이 샘플 전용 전환 팩터리다. Slide 도착 위치는 현재 Navigator 크기와 콘텐츠 Measure 결과로 구하며, 기존 중앙·WRAP_CONTENT 배치를 유지한다. 임의의 LayoutParams를 처리하는 범용 위치 계산기가 아니다. 배치 설정을 바꾸면 그에 맞는 도착 좌표를 사용해야 한다. 첫 프레임의 레이아웃과 충돌하지 않도록 Slide는 위치 keyframe을 사용하고 snap에서 도착 좌표를 복원한다.

전환을 비교하는 동안에는 창 크기를 유지한다. Slide의 도착 좌표는 Post 시점에 계산하므로, 표시 중 창 크기가 달라지는 경우의 종료 방향까지 동적으로 보정하는 샘플은 아니다. 창 크기를 바꾸려면 먼저 Dialog를 닫은 후 변경하고 다시 Post한다.

이 예제의 위치·크기는 기존 AbsoluteLayoutParams로 설정한다. 종료 애니메이션 중에도 직접 Add/Remove/Unparent를 혼용하지 않는다. 다른 관리 방식으로 바꾸려면 Hidden 이후 부모 연결이 정리된 상태에서 시작한다. [Dialog 안내](../../wiki/Dialog-(kr).md)와 수동 테스트에서 기존 수동 조립 경로를 확인할 수 있다. 샘플 조작은 회귀 스모크를 위한 권장 확인이며, 전체 스크린리더·키보드 UX 평가는 이번 API 편의성 패치의 필수 완료 조건이 아니다.
