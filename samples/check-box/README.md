# CheckBox 샘플

[소스](check-box-example.cpp)는 기본·라벨·초기 선택·클릭 선택 금지·애니메이션 모드와
커스텀 Lottie icon generator를 보여 준다. `Create()`에서
`Components::UiConfig::New().Apply()`를 먼저 호출한다.

## 빌드와 실행

저장소 루트 기준이다. CMake·C++17 도구와 pkg-config로 검색 가능한
`dali2-core`, `dali2-adaptor`, `dali2-ui-foundation`, `dali2-ui-components`가 필요하다.
사용자 설치 prefix의 `lib/pkgconfig`를 `PKG_CONFIG_PATH`에 추가하고 런타임 라이브러리 검색 경로를
설정한다. 실행에는 DALi가 지원하는 실제 디스플레이 환경이 필요하다.

```sh
cmake -S samples/check-box -B /tmp/dali-ui-check-box-build
cmake --build /tmp/dali-ui-check-box-build --target check-box.example
./samples/check-box/bin/check-box.example
```

out-of-source 빌드여도 [공통 CMake](../common.cmake)는 실행 파일을 소스 디렉터리의 `bin/`에 만든다.

## 조작과 기대 결과

- box-only, `I agree to the terms`, `Subscribe (pre-checked)`를 반복 탭한다.
  선택 표시와 상태 줄의 `checked/unchecked`가 함께 바뀐다.
  초기 Subscribe 선택도 시그널을 발생시키므로 최초 상태 줄에 해당 이름이 표시될 수 있다.
- `Read-only (click ignored)`는 탭으로 바뀌지 않는다.
  이것은 `SetToggleByClickEnabled(false)` 설정이며 프로그램 변경 금지를 의미하지 않는다.
- `Always animate`와 `Never animate`는 동일한 선택 결과를 각각 애니메이션과 즉시 전환으로 표시한다.
- `Custom Lottie frame ranges`는 기본 리소스 URL을 재사용하는 generator 예제다.
  선택 프레임 0–19, 해제 20–38과 `check_box .inner_fill.color` 키 경로를 명시한다.
  커스텀 리소스를 쓸 때는 자기 파일의 프레임·키 경로로 교체한다.

실제 설정 높이는 56이며 box-only 폭도 56이다. 라벨 항목은 WRAP_CONTENT 폭을 사용한다.
소스의 오래된 48×48 주석보다 실제 상수 값을 기준으로 확인한다.
기본 checkbox Lottie 리소스가 설치되지 않으면 그림이 보이지 않을 수 있다.
이 샘플에는 종료 키 연결 코드가 없으므로 Escape/Back 종료를 보장하지 않는다.
창 닫기 또는 실행 터미널의 Ctrl+C로 종료한다.

[수동 TC 시나리오](../../manual-tests/dali-ui-components/tc/tc-check-box-basics.md)와
[공통 API 안내](../../wiki/ComponentConsistency.md)를 함께 참고한다.
이 문서는 실행 결과가 아니며 GUI·스크린 리더 검증은 별도로 수행하고 기록해야 한다.
