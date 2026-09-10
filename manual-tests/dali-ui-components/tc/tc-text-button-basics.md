# TextButton 기본 동작

[소스](tc-text-button-basics.cpp)의 `TextButton Basics` 항목을 선택한다. 이 TC는
명시적 `TextButtonStyle` 없이 생성 후 setter로 변경하는 경로를 확인한다.

## 준비와 실행

저장소 루트에서 실행한다. C++17 도구와 pkg-config로 검색 가능한
`dali2-core`, `dali2-adaptor`, `dali2-ui-foundation`, `dali2-ui-components` 설치가 필요하다.
사용자 설치 prefix가 있다면 해당 `lib/pkgconfig` 경로를 `PKG_CONFIG_PATH`에 추가한다.
실제 화면을 제공하는 DALi 실행 환경이 필요하며, 런처가 Components UiConfig를 적용한다.

```sh
cmake -S manual-tests/dali-ui-components -B /tmp/dali-ui-components-manual-build
cmake --build /tmp/dali-ui-components-manual-build
./manual-tests/dali-ui-components/bin/manual-test-dali-ui-components
```

## 화면·조작·기대 결과

세로 스크롤 안에 네 구역이 있다. 화면이 작으면 아래로 스크롤한다.

| 구역 | 조작 | 기대 결과 |
|---|---|---|
| `1. Dynamic alignment` | `Change horizontal alignment`를 세 번 누른다. | 초기 CENTER에서 END → START → CENTER로 상태 문구와 고정 폭 버튼 내부 텍스트 정렬이 함께 바뀐다. |
| `2. WRAP_CONTENT dynamic text` | `Toggle wrap text length`를 두 번 누른다. | short → long → short 문구로 바뀌며 버튼의 내용 기반 크기가 사용 가능한 레이아웃 범위에서 재측정된다. |
| `3. Min/max constrained dynamic size` | `Cycle requested size`를 반복한다. | 초기 요청 100×32는 160×48로 제한된다. 다음 요청 220×64는 그대로, 340×120은 260×80으로 제한되고 요청 가이드·상태 문구는 입력 요청을 표시한다. |
| 같은 구역 | 각 크기에서 `Toggle min/max text length`를 누른다. | 짧고 긴 텍스트가 바뀌어도 min/max 제약은 유지된다. |
| `4. Fixed-size dynamic text` | `Toggle fixed-size text length`를 반복한다. | 텍스트와 상태 문구는 바뀌지만 버튼 외곽은 240×64로 유지된다. |

`< Back` 또는 하드웨어 Back으로 목록에 돌아간 뒤 재진입하면 초기 문구·정렬·요청 크기로
돌아와야 한다. Escape는 앱 종료다. 부모의 사용 가능 폭이 고정 폭 가이드보다 작은 환경은
별도 기록하고, 픽셀 크기 비교는 해당 가이드가 들어가는 창 크기에서 수행한다.

## 통과 기준과 기록

네 구역 모두 상태 문구와 실제 화면이 일치하고, 반복 조작에서 이전 텍스트나 크기가
남지 않아야 한다. 기기·화면 크기·배율·빌드 버전과 구역별 결과를 기록한다.
이 문서는 시나리오이며 실행 결과가 아니다. 스타일 provider, invalid 수치 assertion,
키보드·스크린 리더 지원은 이 TC의 화면 통과만으로 검증되지 않는다.
