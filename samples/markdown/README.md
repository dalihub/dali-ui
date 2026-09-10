# MarkdownView 샘플

[소스](markdown-view-example.cpp)와 [문서 데이터](markdown-samples.cpp)는 정적 문서,
스트리밍 중간 입력, 방향·배율·변환 API와 성능 계측을 보여 준다.

현재 소스는 foundation `UiConfig::New()`에 `SetLabelAsyncRendering(true)`를 설정해 적용하고,
`CreateMarkdownViewExampleStyle()`로 만든 명시적 style을 `MarkdownView::New(style)`에 전달한다.
components 애플리케이션의 권장 설정은 `Components::UiConfig` 한 번 적용이다.
이 샘플은 Components 기본 style provider 경로를 입증하지 않는다.
샘플을 그 경로로 옮길 때 기존 config에 추가로 Apply하지 말고 설정 진입점을 교체한다.

## 빌드와 실행

저장소 루트 기준이다. CMake·C++17 도구와 pkg-config로 검색 가능한
`dali2-core`, `dali2-adaptor`, `dali2-ui-foundation`, `dali2-ui-components`가 필요하다.
사용자 설치 prefix의 `lib/pkgconfig`를 `PKG_CONFIG_PATH`에 추가하고 런타임 라이브러리 검색 경로를
설정한다. 실행에는 DALi가 지원하는 실제 디스플레이 환경이 필요하다.

```sh
cmake -S samples/markdown -B /tmp/dali-ui-markdown-build
cmake --build /tmp/dali-ui-markdown-build --target markdown-view.example
./samples/markdown/bin/markdown-view.example
```

out-of-source 빌드여도 [공통 CMake](../common.cmake)는 실행 파일을 소스 디렉터리의 `bin/`에 만든다.

실행 타깃은 디렉터리 이름과 달리 `markdown-view.example`이다.
소스가 지정하는 SamsungOneUI 계열 글꼴이 없으면 글꼴 대체로 모양과 계측 결과가 달라질 수 있다.

## 조작과 기대 결과

- `Previous/Next`로 문서 케이스를 전환한다. 선택 문서와 제목·목록·코드·표 등
  해당 케이스의 구조가 갱신되어야 한다.
- `Start`로 스트리밍하고 `Step`으로 한 단계 진행한다. `Char/Word/Random`과
  `Faster/Slower`로 청크 방식·간격을 바꾸어 불완전한 중간 Markdown이 갱신되는 모습을 확인한다.
  `Reset`으로 다시 시작 상태를 확인한다.
- `LTR/RTL`과 배율 버튼을 바꾼다. 방향·글자/레이아웃 변경 시 내용이 사라지거나
  이전 렌더링이 중복되지 않아야 한다.
- `GetMarkdown`, `MarkdownToPlainText` 버튼으로 현재 소스 조회와
  `MarkdownView::ToPlainText()` 변환 경로를 확인한다.
- `Start Auto Test`는 샘플 내부 반복 실행/계측 기능이다.
  테스트 전체의 pass/fail 판정이나 접근성 인증을 의미하지 않는다.

계측 패널의 `Last SetMarkdown`, `Stream wall`, `SetMarkdown total` 등을
빌드·환경·문서 케이스와 함께 기록한다. 숫자만으로 프레임 렌더링 완료 시간을 단정하지 않는다.
화면 도움말에 키보드 단축키가 표시되며 Escape 또는 Back은 실제 종료 핸들러가 있다.

[공통 계약](../../wiki/ComponentConsistency.md).
이 문서는 사용 시나리오이며 샘플 빌드·GUI·AT 실행 완료를 주장하지 않는다.
