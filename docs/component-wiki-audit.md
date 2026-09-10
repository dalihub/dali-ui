# 컴포넌트 위키 누락 점검 및 보완

## 범위와 기준

`wiki/Components.md`와 `wiki/Components-(kr).md`의 `dali-ui-components` 영역에서 링크된 구현 컴포넌트 15개를 점검했다. 파일의 존재뿐 아니라 생성·설정 방법, 실제 API, 사용 제약, 접근성 주의사항과 예제·소스 링크가 있는지를 기준으로 삼았다. Layouts·Basics의 foundation API 문서 전체 개편은 이번 변경 범위가 아니다.

## 보완 대상과 결과

| 컴포넌트 | 기존 상태 | 반영 |
|---|---|---|
| TextButton | 전용 위키 없이 공개 헤더로 연결 | [영문](../wiki/TextButton.md)·[한국어](../wiki/TextButton-(kr).md) 신규 작성 |
| CheckBox | 전용 위키 없이 공개 헤더로 연결 | [영문](../wiki/CheckBox.md)·[한국어](../wiki/CheckBox-(kr).md) 신규 작성 |
| ProgressBar | 전용 위키 없이 공개 헤더로 연결 | [영문](../wiki/ProgressBar.md)·[한국어](../wiki/ProgressBar-(kr).md) 신규 작성 |
| MarkdownView | 전용 위키 없이 공개 헤더로 연결 | [영문](../wiki/MarkdownView.md)·[한국어](../wiki/MarkdownView-(kr).md) 신규 작성 |
| DummyComponent | 전용 위키 없이 공개 헤더·공통 가이드로 연결 | [영문](../wiki/DummyComponent.md)·[한국어](../wiki/DummyComponent-(kr).md) 신규 작성 |
| IconButton | 짧은 크기 규칙 중심의 문서만 존재, 한국어판 없음 | [영문](../wiki/IconButton.md) 전면 보강·[한국어](../wiki/IconButton-(kr).md) 신규 작성 |

ChartView, WindowFrame, Navigator, Dialog, AlertDialog, DialogContainer, Switch, RadioButton, Toast는 기존 가이드가 있다. AlertDialog와 DialogContainer는 각각 독립 파일이 아니라 Dialog 통합 문서의 해당 절로 연결된다. 기존 Navigator·Dialog 가이드의 한국어판은 없으므로 한국어 목록에서도 영문 가이드 연결을 유지했다.

아직 링크되지 않은 예정 항목 ProgressCicle, Loading, Slider에는 사용 가능한 기능처럼 보이는 가이드를 만들지 않았다.

## 주요 내용

- TextButton: 생성 오버로드, 크기 제약과 Build 검증, 타이포그래피, 클릭 신호, 접근성 이름.
- CheckBox: 선택 신호, 사용자/프로그램 변경과 애니메이션 모드, box-only 형태, 아이콘 0의 의미, 사용자 생성기, 접근성 상태.
- ProgressBar: 값/범위/스냅, 독립적인 구분선, 모드/RTL/역방향, geometry 신호, 앱 소유 툴팁·라벨 동기화.
- MarkdownView: 문서 교체·원문 조회·일반 텍스트 변환, 작업 체크박스, 너비·스크롤, 현재 링크·이미지·HTML 표시 제약.
- DummyComponent: 기능 컨트롤이 아닌 호환용 View 타입임을 명시하고 일반 용도에는 View를 안내. 통합 헤더에 포함되지 않아 별도 헤더가 필요하다는 점도 기재.
- IconButton: 생성·프리셋·런타임 변경, 명시 크기/natural size/fallback, 비동기 재측정, 클릭·접근성. Default/Add/원시 Builder의 padding은 0이며 Back/More만 6이라는 실제 기본값을 반영.

영문·한국어 Components 목록의 해당 이름을 새 가이드로 연결했다. MarkdownView 목록의 스타일 설명에서 공개 API에 없는 간격 설정도 제외했다. 코드 주석은 두 언어 문서 모두 영어로 유지했다.

## 검증

- 신규·보강 가이드 12개의 C++ 코드 블록을 각각 추출하여 현재 저장소 헤더와 설치된 Core/Adaptor 헤더로 C++17 문법 검사를 통과했다.
- 6개 컴포넌트의 영문·한국어 코드 블록이 동일함을 확인했다.
- 가이드와 Components 목록 14개 문서의 위키·저장소 링크 230개를 로컬 대상 파일/디렉터리와 제목 앵커에 대조했다.
- 양쪽 목록의 구현 컴포넌트 15개가 모두 위키 가이드로 연결됨을 확인했다.
- 기존 문서 변경은 `git diff --check`, 신규 문서는 공백 오류 검사로 확인한다.

링크 검증은 저장소의 파일과 앵커 기준이다. 사내 서버의 게시 여부·권한·HTTP 응답까지 검증한 것은 아니다. 예제의 GUI 실행, 실제 기기 접근성 테스트, 위키 서버 게시와 커밋·푸시는 수행하지 않았다.
