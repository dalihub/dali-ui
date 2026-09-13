# IconButton 샘플

[소스](icon-button-example.cpp)는 `Components::UiConfig::New().Apply()` 후
`IconButtonStyle::AddPreset()`, `BackPreset()`, `MorePreset()`을 typed `New(style)`에 전달한다.
기본 provider를 지역 변경하는 API는 `IconButtonStyle::Default().Configure()…Build()`이며,
이 샘플의 preset 선택과 구분한다.

## 빌드와 실행

저장소 루트 기준이다. CMake·C++17 도구와 pkg-config로 검색 가능한
`dali2-core`, `dali2-adaptor`, `dali2-ui-foundation`, `dali2-ui-components`가 필요하다.
사용자 설치 prefix의 `lib/pkgconfig`를 `PKG_CONFIG_PATH`에 추가하고 런타임 라이브러리 검색 경로를
설정한다. 실행에는 DALi가 지원하는 실제 디스플레이 환경이 필요하다.

```sh
cmake -S samples/icon-button -B /tmp/dali-ui-icon-button-build
cmake --build /tmp/dali-ui-icon-button-build --target icon-button.example
./samples/icon-button/bin/icon-button.example
```

out-of-source 빌드여도 [공통 CMake](../common.cmake)는 실행 파일을 소스 디렉터리의 `bin/`에 만든다.

## 화면·조작·기대 결과

상단의 독립 ImageView는 Back preset의 SVG URL을 직접 로드한다.
아래에는 Add, Back, More별 클릭 횟수와 활성(왼쪽)·비활성(오른쪽) 버튼 쌍이 있다.
Add 외곽은 56×56, Back/More는 52×52로 명시한다.

활성 버튼을 눌러 대응하는 `Add/Back/More clicks: N`이 한 번씩 증가하는지 확인한다.
비활성 버튼은 `SetEnabled(false)` 상태다. 다만 비활성 버튼에는 카운터 콜백 자체가
연결되지 않으므로 카운터가 그대로라는 관찰만으로 비활성 클릭 시그널 억제를 증명할 수 없다.
입력 억제 자체는 자동 테스트 또는 별도 계측으로 확인한다.
독립 SVG와 버튼 아이콘이 모두 보이지 않으면 먼저 설치된 component SVG 리소스를 확인한다.

모든 열이 보이는 창 폭으로 검사한다. 전용 종료 키 코드는 없으므로 창 닫기 또는
실행 터미널의 Ctrl+C로 종료한다. 샘플은 접근성 이름을 별도로 설정하지 않으므로
실제 앱에서는 각 아이콘의 기능을 설명하는 `SetAccessibilityName()`을 추가해야 한다.

[상세 안내](../../wiki/IconButton.md), [공통 계약](../../wiki/ComponentConsistency.md).
이 문서는 빌드·GUI·AT 실행 증거가 아니다.
