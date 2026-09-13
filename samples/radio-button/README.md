# RadioButton 샘플

[소스](radio-button-example.cpp)는 Components UiConfig를 적용하고 기본 RadioButton,
parent-auto/named 그룹, 프로그램 선택 해제, 48×32 아이콘 style, 텍스트 행 합성을 보여 준다.
RadioButton은 아이콘 전용이며 라벨 문자열을 받는 `New(text)` API가 없다.

## 빌드와 실행

저장소 루트 기준이다. CMake·C++17 도구와 pkg-config로 검색 가능한
`dali2-core`, `dali2-adaptor`, `dali2-ui-foundation`, `dali2-ui-components`가 필요하다.
사용자 설치 prefix의 `lib/pkgconfig`를 `PKG_CONFIG_PATH`에 추가하고 런타임 라이브러리 검색 경로를
설정한다. 실행에는 DALi가 지원하는 실제 디스플레이 환경이 필요하다.

```sh
cmake -S samples/radio-button -B /tmp/dali-ui-radio-button-build
cmake --build /tmp/dali-ui-radio-button-build --target radio-button.example
./samples/radio-button/bin/radio-button.example
```

out-of-source 빌드여도 [공통 CMake](../common.cmake)는 실행 파일을 소스 디렉터리의 `bin/`에 만든다.

## 조작과 기대 결과

1. Window에 직접 추가된 standalone 아이콘을 선택하고 재탭한다. 선택이 유지된다.
   Ui::View 부모가 없어도 컴포넌트 자체의 select-only 동작은 유지된다.
2. Auto 그룹의 아이콘을 차례로 선택한다. 같은 직접 부모 아래에서 하나만 선택된다.
3. Named 그룹에서 서로 다른 wrapper에 든 아이콘을 선택한다. group name이 같으므로
   하나만 선택되고 Auto 그룹에는 영향을 주지 않는다.
4. 비정방 아이콘과 텍스트 행을 선택한다. 텍스트 행은 바깥 GroupSelectableView가
   그룹·선택·접근성 이름을 소유하고 안쪽 RadioButton에 선택을 동기화하는 합성 예다.
5. `Clear auto / named / text groups`를 누른다. 세 그룹이 해제되고 상태 줄은
   `Groups cleared programmatically`가 된다. standalone은 이 버튼의 대상이 아니다.

기본 아이콘 리소스가 설치되어 있어야 한다. 각 아이콘은 접근성 이름을 가지지만
시각적 라벨이 전부 제공되는 것은 아니다. 전용 종료 키 코드는 없으므로 창 닫기 또는
실행 터미널의 Ctrl+C로 종료한다.

텍스트 행의 안쪽 indicator는 `SetClickable(false)`, `SetFocusable(false)`,
`SetAccessibilityHidden(true)`로 설정되어 있다. 실제 앱에 재사용할 때는
[public header](../../dali-ui-components/public-api/radio-button.h)의
`SetSensitive(false)` 및 long-press 관련 안내를 확인해 입력 소유자를 하나로 정리한다.
샘플만으로 모든 터치·long-press 전달이 검증된 것은 아니다.

[수동 TC](../../manual-tests/dali-ui-components/tc/tc-radio-button-basics.md)는
disabled/read-only, 애니메이션 모드, RTL, 테마 및 실제 스크린 리더 확인을 별도로 안내한다.
이 문서는 미실행 안내이며 [공통 계약](../../wiki/ComponentConsistency.md)을 함께 참고한다.
