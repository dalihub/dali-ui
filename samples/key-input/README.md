# DALi Key Input parity sample

This sample displays every `Dali::KeyEvent` field before the focused text
control consumes the event. It also contains an `InputField` for testing normal
text and IME composition on Ubuntu and Windows. The observer always returns
`false`, so logging does not change normal input behavior.

---

## 한국어 가이드

### 빌드 및 실행

Ubuntu:

```sh
cd dali-ui/samples
./build.sh Debug
./key-input/bin/key-input.example
```

설치된 실행 파일은 `$DESKTOP_PREFIX/bin/key-input.example`에서도 실행할 수
있습니다. DALi 라이브러리는 테스트할 소스와 같은 revision으로 먼저 빌드하고
설치해야 합니다.

Windows PowerShell:

```powershell
cd dali-ui\samples
.\build.ps1 -Configuration Debug -Samples key-input
```

`-Samples`를 생략해도 `build.ps1`이 이 샘플을 자동 검색하여 전체 샘플 빌드에
포함합니다. 빌드 스크립트가 마지막에 출력하는 install prefix의
`bin\key-input.example.exe`를 실행하십시오. Visual Studio 빌드 트리에서는
보통 `key-input\bin\Debug\key-input.example.exe`에도 생성됩니다.

`build.sh`는 모든 샘플을 빌드하므로 별도 옵션 없이 이 샘플을 포함합니다.
Windows의 기본 CMake 샘플 목록에도 `key-input`이 등록되어 있습니다.

### 테스트 순서

1. 흰색 입력 필드를 클릭합니다. 시작 시 자동으로 focus를 요청하지만 창을
   전환한 뒤에는 다시 클릭하는 편이 확실합니다.
2. `a`, `Shift+A`, 숫자, `Shift+숫자`, OEM 문장부호
   (``; = , - . / ` [ \ ] '``)를 누릅니다.
3. 좌/우 `Shift`, 좌/우 `Ctrl`, 좌/우 `Alt`, `Backspace`, `Delete`,
   `Enter`, 화살표, `Home/End`, `Page Up/Down`, `Insert`, `F1`–`F12`를
   확인합니다.
4. Num Lock을 켜고 끄면서 숫자 키패드와 keypad Enter를 확인합니다.
5. 키 하나를 길게 눌러 첫 `DOWN` 뒤의 반복 `DOWN`을 확인합니다.
6. 장치에 있는 경우 볼륨, 음소거, 재생/정지, 브라우저, 메일 키를 확인합니다.
7. Ubuntu에서는 IBus/Fcitx, Windows에서는 Microsoft IME를 한국어로
   전환하고 입력 필드에 `한글 테스트`를 입력한 뒤 확정합니다.

### 기대 결과

- `DOWN`과 `UP`이 모두 기록되고 카운터가 증가합니다. `UP`의
  `keyString`/`compose`는 비어 있는 것이 정상입니다.
- 물리 키 이름 `keyName`은 두 플랫폼에서 XKB 계열 이름으로 최대한
  통일됩니다. 예: `BackSpace`, `Return`, `Delete`, `Shift_L`,
  `Shift_R`, `Control_L`, `Control_R`, `Left`, `F1`, `KP_Enter`.
- `logicalKey`는 modifier가 적용된 의미입니다. 예를 들어 `a`는 `a`,
  `Shift+A`는 `A`, `Shift+1`은 `exclam`으로 보여야 합니다.
- `keyString`은 `DOWN`에서 실제 입력 문자열을 가집니다. 문자 입력이 아닌
  기능 키는 비어 있을 수 있습니다.
- `canonical`은 DALi key lookup과 일치한 경우
  `DALI_KEY_BACKSPACE`, `DALI_KEY_DELETE`, `DALI_KEY_RETURN` 같은 이름을
  표시합니다. 해당 public/devel DALi enum이 없는 일반 문자/F-key는
  `<none>`이어도 정상입니다.
- Windows에서 키를 길게 누르면 첫 이벤트는 `repeat=false`, 이후 자동 반복
  `DOWN`은 `repeat=true`여야 합니다. Ubuntu backend는 현재 반복 플래그를
  별도로 제공하지 않으므로 반복 이벤트가 와도 `false`일 수 있습니다.
- 상단 안내 줄의 `Repeat API`는 Windows에서 현재 OS 값을 초 단위
  `rate`/`delay`로 표시합니다. Ubuntu backend가 이 API를 노출하지 않는
  현재 구성에서는 `not exposed by this backend`가 정상입니다.
- modifier의 공통 하위 비트는 Shift=`0x1`, Ctrl=`0x2`, Alt=`0x4`입니다.
  Windows에서는 Win=`0x8`, Scroll Lock=`0x10`, Num Lock=`0x20`,
  Caps Lock=`0x40`, AltGr=`0x400`도 raw modifier 값에 반영됩니다.
- Windows의 `receiveTime`은 0이 아니고 일반적으로 `time`과 가깝습니다.
  Windows device는 `keyboard` / `KEYBOARD`로 표시됩니다.
- 한글 IME 조합 중에는 입력 필드에서 preedit가 보이고, 확정 후 `한글 테스트`가
  입력 필드와 녹색 문자열 줄에 남아야 합니다. IME 처리용
  `Process` 키의 raw `keyString`이 비어 있는 것은 정상이며, 확정 문자열은
  Windows IMM composition 경로로 전달됩니다.

### 의도적으로 같은 값이 아닌 항목

- `keyCode`는 platform-native 값입니다. Ubuntu/X11 keycode와 Windows
  virtual-key 값은 서로 달라야 정상이며 애플리케이션은 직접 비교하지 말고
  `keyName` 또는 `Dali::IsKey()`를 사용해야 합니다.
- Windows는 Raw Input 장치 식별을 하지 않으므로 모든 일반 키 이벤트를
  `keyboard`라는 generic 장치로 보고합니다.
- DALi key grab의 Tizen 전역/우선순위 의미는 Win32 `RegisterHotKey`와
  동일하지 않아 이번 변경에서 Windows에 잘못된 호환 구현을 추가하지
  않았습니다.
- Windows repeat 설정 API는 OS가 제공하는 32단계 속도와 4단계 지연으로
  반올림됩니다. 시스템 전역 설정이므로 이 샘플은 값을 변경하지 않습니다.

---

## English guide

### Build and run

Ubuntu:

```sh
cd dali-ui/samples
./build.sh Debug
./key-input/bin/key-input.example
```

The installed executable is also available as
`$DESKTOP_PREFIX/bin/key-input.example`. Build and install the DALi libraries
from the revision under test first.

Windows PowerShell:

```powershell
cd dali-ui\samples
.\build.ps1 -Configuration Debug -Samples key-input
```

Omitting `-Samples` also works: `build.ps1` discovers this directory and
includes it in the all-samples build. Run `bin\key-input.example.exe` under the
install prefix printed by the script. A Visual Studio build commonly also
places it at `key-input\bin\Debug\key-input.example.exe`.

`build.sh` already builds every discovered sample, and `key-input` is included
in the default Windows CMake sample list.

### Test procedure

1. Click the white input field. The sample requests focus at startup, but
   clicking it again after switching windows is the most reliable method.
2. Test `a`, `Shift+A`, digits, shifted digits, and OEM punctuation
   (``; = , - . / ` [ \ ] '``).
3. Test left/right Shift, Ctrl and Alt, then Backspace, Delete, Enter, arrows,
   Home/End, Page Up/Down, Insert, and F1–F12.
4. Toggle Num Lock and test the numeric keypad and keypad Enter.
5. Hold a key and inspect the first and subsequent `DOWN` events.
6. If available, test volume, mute, playback, browser, and mail keys.
7. Select a Korean IBus/Fcitx engine on Ubuntu or Microsoft IME on Windows,
   type `한글 테스트`, and commit the composition.

### Expected results

- Both `DOWN` and `UP` are logged. An empty `keyString`/`compose` on `UP` is
  expected.
- Physical `keyName` values are normalized to XKB-style names across the two
  platforms where possible: `BackSpace`, `Return`, `Delete`, `Shift_L`,
  `Shift_R`, `Control_L`, `Control_R`, `Left`, `F1`, and `KP_Enter`.
- `logicalKey` reflects modifiers: `a`, `A`, and `exclam` are expected for
  `a`, `Shift+A`, and `Shift+1`.
- Printable `DOWN` events contain the input text in `keyString`; non-text keys
  may have an empty string.
- `canonical` names a matching DALi enum. `<none>` is valid for ordinary
  characters and keys for which DALi has no public/devel enum.
- On Windows, the initial `DOWN` has `repeat=false` and auto-repeat events have
  `repeat=true`. The current Ubuntu backend may report `false` for repeated
  events because it does not expose that flag separately.
- The top `Repeat API` line shows the current Windows OS `rate` and `delay` in
  seconds. `not exposed by this backend` is expected from the current Ubuntu
  backend.
- Common modifier bits are Shift=`0x1`, Ctrl=`0x2`, and Alt=`0x4`. Windows
  additionally reports Win=`0x8`, Scroll Lock=`0x10`, Num Lock=`0x20`,
  Caps Lock=`0x40`, and AltGr=`0x400` in the raw modifier value.
- On Windows, `receiveTime` should be non-zero and normally close to `time`;
  the device should read `keyboard` / `KEYBOARD`.
- During Korean composition, preedit should be visible in the field. After
  commit, `한글 테스트` should remain in both the field and the green text
  status line. An empty raw `keyString` for an IME `Process` key is expected;
  committed text travels through the Windows IMM composition path.

### Intentionally platform-specific results

- `keyCode` remains platform-native. X11 keycodes and Win32 virtual-key values
  are expected to differ; applications should use `keyName` or
  `Dali::IsKey()` instead of comparing those integers.
- Windows currently identifies regular keyboard events with one generic
  `keyboard` device because this path does not correlate events with Raw Input
  device handles.
- Tizen key-grab priority/global semantics do not match Win32
  `RegisterHotKey`, so this change does not claim an incompatible Windows key
  grab implementation.
- Windows repeat settings are rounded to the OS's 32 speed levels and four
  delay levels. They are global settings, so this sample deliberately does not
  modify them.
