# Navigator 예제

일반 페이지 Push/Pop과 모달 전환을 비교한다. Show Dialog는 AlertDialog.Post로 표시하며 버튼은 약한 핸들을 통해 해당 Dialog만 Dismiss한다. Per-modal Custom Ani는 DialogPostOptions.transitionSpec을 사용한다. Back/Escape는 모달이 있으면 NavigateBack으로 사용자 요청을 전달한다.

Show Dialog는 콘텐츠 scale/fade, Per-modal Custom Ani는 콘텐츠 keyframe bounce를 사용한다. scrim은 움직이지 않고 Navigator가 독립적으로 fade한다. 모달 콜백은 Container가 아닌 콘텐츠를 받으므로, 샘플은 중앙 배치 좌표를 0으로 덮어쓰지 않는다. 일반 페이지의 slide 전환은 유지한다.

Dialog의 기본 사용법은 **Post/Dismiss를 권장**한다. 사용자 정의 scrim View 등 복잡한 scrim 구성에만 기존 수동 Container 조립 경로를 선택한다. 이 경우 표시·종료·콘텐츠 해제를 수동 경로로 관리하고 동일한 Dialog에 Post/Dismiss를 혼용하지 않는다. 단순 scrim 색상·blur·비표시는 `DialogPostOptions.containerStyle`로 설정할 수 있다. [Dialog 문서](../../wiki/Dialog-(kr).md)

```sh
cmake -S samples/navigator -B /tmp/dali-navigator-sample-build
cmake --build /tmp/dali-navigator-sample-build --parallel 4
samples/navigator/bin/navigator.example
```

현재 UI 라이브러리가 설치된 DESKTOP_PREFIX와 pkg-config·런타임 라이브러리 경로가 필요하다. GUI 실행에는 Adaptor의 디스플레이·그래픽 환경이 필요하며 빌드 성공이 화면 검증을 대신하지 않는다.

[Navigator API](../../wiki/Navigator-(kr).md) · [Dialog 표시 계약](../../wiki/Dialog-(kr).md)
