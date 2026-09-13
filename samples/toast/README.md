# Toast 샘플

텍스트·선택적 아이콘, SHORT/LONG 표시 시간, 긴 텍스트 줄바꿈, 명시적 닫기,
Shown/Hidden 신호와 사용자 ToastStyle을 보여준다. Components UiConfig를 한 번 적용한다.

## 로컬 빌드와 실행

일치하는 DALi Core/Adaptor/Foundation/Components 설치를 `PKG_CONFIG_PATH`와
`LD_LIBRARY_PATH`에 설정한다. 저장소 루트에서 실행한다.

```sh
cmake -S samples/toast -B /tmp/dali-toast-sample-build
cmake --build /tmp/dali-toast-sample-build
samples/toast/bin/toast.example
```

## 조작과 기대 결과

- `Post SHORT toast (1.5 s)` / `Post LONG toast (2.5 s)`: 기본 timeout 후 닫힌다.
- `Post toast with icon`: 아이콘과 텍스트를 함께 표시한다. 리소스 로딩은 별도 확인한다.
- `Post long-text toast`: 긴 문장의 줄바꿈과 화면 경계를 확인한다.
- `Post custom-style toast`: 사용자 style 외형을 확인한다.
- `Dismiss current toast`: 현재 Toast를 명시적으로 닫는다.
- 상태 문구의 `ShownSignal` / `HiddenSignal`과 실제 화면 상태를 함께 관찰한다.

Toast는 일반 child Add가 아니라 `Post(window)`로 표시한다. 내용·duration 변경과
immutable style의 생성 시 적용은 별개이며 duration 0은 허용하지 않는다.
빌드 성공은 실제 표시 시간·시각 품질·스크린리더 발화 검증이 아니다.
API와 접근성 한계는 [Toast Wiki](../../wiki/Toast.md),
[공통 사용 안내](../../wiki/ComponentConsistency.md)를 참조한다.

## GBS 빌드

GBS 패키징은 별도로 내보낸 독립 샘플 저장소에서 수행한다. 부모 dali-ui 저장소에서
실행하지 않는다. 다음 명령은 독립 export가 이미 Git 저장소로 준비됐다는 전제다.

```sh
cd samples/toast
gbs build -A armv7l --include-all --packaging-dir packaging
```

패키지 ID는 `com.samsung.dali.toast`, 설치 실행 파일은 `toast.example`이다.
이번 문서 보완에서 GBS 빌드·디바이스 설치는 실행하지 않았다.
