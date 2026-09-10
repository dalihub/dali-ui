# Switch 샘플

[소스](switch-example.cpp)는 Components UiConfig 적용 후 기본 OFF, 초기 ON,
사용자 정의 style OFF, disabled ON 상태를 보여 준다. 사용자 style은
`SwitchStyle::Default().Configure()`에서 track 72×40, thumb padding 4,
회색/초록 track 색상을 변경한 뒤 `Switch::New(style)`에 전달한다.

## 빌드와 실행

저장소 루트 기준이다. CMake·C++17 도구와 pkg-config로 검색 가능한
`dali2-core`, `dali2-adaptor`, `dali2-ui-foundation`, `dali2-ui-components`가 필요하다.
사용자 설치 prefix의 `lib/pkgconfig`를 `PKG_CONFIG_PATH`에 추가하고 런타임 라이브러리 검색 경로를
설정한다. 실행에는 DALi가 지원하는 실제 디스플레이 환경이 필요하다.

```sh
cmake -S samples/switch -B /tmp/dali-ui-switch-build
cmake --build /tmp/dali-ui-switch-build --target switch.example
./samples/switch/bin/switch.example
```

out-of-source 빌드여도 [공통 CMake](../common.cmake)는 실행 파일을 소스 디렉터리의 `bin/`에 만든다.

## 조작과 기대 결과

- 활성 Switch를 탭하거나 가로로 드래그해 상태를 바꾼다. 해당 행의 ON/OFF,
  thumb 위치와 `SelectionChanged callbacks` 횟수가 실제 상태 변경과 함께 갱신된다.
  상태가 변하지 않은 입력에 매번 콜백이 발생한다고 가정하지 않는다.
- 키보드로 Switch에 포커스를 준 뒤 Enter를 누르면 선택 상태가 전환된다.
- `Toggle default programmatically` 버튼은 첫 Switch의 `SetSelected(!IsSelected())`를
  호출한다. 첫 행의 상태·카운터가 바뀌고 마지막 이벤트에는
  `Default (programmatic or drag)`가 표시된다.
- 비활성 Switch는 초기 ON을 유지하고 직접 탭·드래그·Enter를 무시해야 한다.
  이 행에는 카운터가 없으므로 해당 시그널 횟수를 화면만으로 판단하지 않는다.

마지막 콜백 문구는 소스의 `InputEvent::IsProgrammatic()` 분기에 따른 것이다.
드래그도 `programmatic or drag`로 묶이므로 이 문구만으로 입력 원인을 더 세분하지 않는다.
각 Switch에는 접근성 이름이 설정되지만 실제 읽기 결과는 스크린 리더에서 따로 확인한다.
전용 종료 키 코드는 없으므로 창 닫기 또는 실행 터미널의 Ctrl+C로 종료한다.

[Switch 안내](../../wiki/Switch.md), [공통 계약](../../wiki/ComponentConsistency.md).
이 문서는 실행 결과가 아니며 시각·키보드·AT 확인 결과를 별도로 기록한다.
