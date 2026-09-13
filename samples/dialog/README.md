# Dialog 예제

세 계열의 기본/사용자 style과 modal 제거·재설치를 함께 관찰하려면
[등록된 Dialog 수동 시나리오](../../manual-tests/dali-ui-components/tc/tc-dialog-basics.md)를 사용한다.

`Components::UiConfig`를 적용한 뒤 `DialogStyle::Default().Configure()`로 외형을 복사하고, `AlertDialogStyle`에 조합해 `AlertDialog::New(style)`로 생성한다. 제목·본문·action 내용과 이후의 spacing 변경은 인스턴스 API가 담당한다.

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

- Cancel/OK: 하단 상태의 last action이 누른 버튼 이름으로 바뀐다.
- Spacing +/-: section 간격이 8씩 변하고 0 미만으로 내려가지 않는다.
- Align: FILL → START → CENTER → END 순으로 정렬을 변경한다.
- Toggle Msg: 본문 제거와 재생성을 반복한다. 제목과 action 버튼은 유지된다.
- Escape/Back: 종료한다.

이 예제는 AlertDialog를 일반 레이아웃에 배치한다. DialogContainer의 modal 생명주기·scrim·접근성 전환은 [Dialog 안내](../../wiki/Dialog.md) 및 Components UTC와 구분한다. 실제 스크린리더 발화와 키보드 이동은 별도 수동 확인이 필요하다.
