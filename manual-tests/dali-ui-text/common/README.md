# 공용 이미지 검사 모듈

모든 TC에서 재사용할 수 있는 실제 렌더링 검사 도구다. DALi internal/integration/
extension/devel API는 사용하지 않는다. 이미지 처리는 표준 C++와 libpng 1.6을 사용한다.

## libpng 의존성

libpng는 text 기능 자체가 아니라 캡처 PNG의 RGBA 디코딩과 실패 이미지 저장에만 사용한다.
공용 비교·색상·이동량 알고리즘은 libpng와 분리되어 있다. 별도 codec을 vendoring하지 않고
대상 배포판의 system library를 링크한다.

| 환경 | 공식 패키지 |
|---|---|
| Tizen Unified armv7l | [libpng / libpng-devel 1.6.44](https://download.tizen.org/snapshots/TIZEN/Tizen/Tizen-Unified/latest/repos/standard/packages/armv7l/) |
| Ubuntu | [libpng-dev](https://packages.ubuntu.com/libpng-dev) (22.04: 1.6.37, 24.04: 1.6.43 계열) |
| Debian | [libpng-dev](https://packages.debian.org/bookworm/libpng-dev) (Bookworm: 1.6.39 계열) |
| Fedora | [libpng-devel](https://packages.fedoraproject.org/pkgs/libpng/libpng-devel/) |

주요 Linux 배포판에서 구할 수 있는 의존성이지만 **개발 헤더의 기본 설치나 모든 Linux에서의
동작을 보장하는 뜻은 아니다**. 빌드에는 development package, 실행에는 해당 runtime library와
그 의존성(zlib 등)이 필요하다. CPU architecture/ABI와 타깃 sysroot에 맞춰 빌드해야 한다.
Tizen 저장소의 패키지 존재는 확인했으나 해당 RPM의 바이너리 심볼과 타깃 실행은 별도 검증 사항이다.

기본 `TEXT_PIXEL_CHECKS=ON` 빌드는 libpng 1.6 이상뿐 아니라 simplified PNG 파일 읽기·쓰기
API가 활성화되어 실제로 링크되는지도 configure에서 확인한다. 배포판의 일반 패키지는 이 API를
제공하지만 최소 구성으로 직접 빌드한 libpng는 다를 수 있다.

`-DTEXT_PIXEL_CHECKS=OFF`는 libpng를 찾거나 직접 링크하지 않는다. 수동 조작과 Text Layout
자동 검증을 사용할 수 있고, 픽셀 자동 검증 요청은 ERROR로 처리한다. DALi 자체가 사용하는
간접적인 image codec 의존성까지 제거하는 옵션은 아니다. 기본 Tizen spec은 전체 검증을 위해
`pkgconfig(libpng)`를 BuildRequires로 선언한다.

## 구성

| 파일 | 역할 |
|---|---|
| `image-capture.h/.cpp` | Public Capture → 임시 PNG → RGBA 픽셀. 비동기 요청·결과 회수·callback 해제 |
| `image-check.h/.cpp` | ROI/crop, 색상 수·bounding box, foreground 수, 이미지 비교, 차이 이미지, 이동량 추정, PNG I/O |
| `../tests/image-check-test.cpp` | 렌더러와 무관한 합성 이미지 기반 검증. 누락·offset·빈 화면·잘못된 입력·PNG roundtrip |
| `run-report.h/.cpp` | TC별 실행 로그 분리와 지정 경로의 누적 로그 보존 |
| `../tests/run-report-test.cpp` | 기존 파일·다른 TC·재실행·객체 재생성 시 보존, 잘못된 경로 거부 |

`Capture::GetNativeImage()` / `NativeImage::GetPixels()`가 모든 backend에서 동작하지
않으므로 Public Capture의 파일 저장 경로를 사용한다. 현재 DALi Capture 구현은 GLES가
필요하다. 이를 사용할 수 없는 플랫폼은 검증 불가 ERROR이며 다른 backend로 몰래 전환하지 않는다.
매 캡처의 scratch 디렉터리는 `/tmp`에 고유하게 생성하고 결과 회수/취소 시 제거한다.

## TC에서 사용하는 순서

1. 검사할 View를 window에 추가하고, 필요하면 clipping 부모와 주변 여백까지 fixture에 포함한다.
2. 실제 layout/render 완료를 기다린다. Async는 해당 View의 완료 signal을 먼저 확인한다.
3. TC timer에서 `ImageCapture::Request(source, screenTopLeft, pixelSize, error)`를 호출한다.
4. 다음 timer tick에서 `Take(result)`로 결과를 회수한다. `result.Ok()`가 false면 ERROR로 처리한다.
5. 좌표와 색상 마커, 불투명 배경을 검사하고 `ImageCheck` 함수로 판정한다.
6. 실패 시 expected/actual/diff와 ROI·허용 오차·case 설정을 기록한다.
7. `OnExit()`에서 timer 중지와 `ImageCapture::Cancel()`을 호출한다.

한 번에 한 요청만 허용한다. Capture 완료 signal 내부에서 다음 캡처를 재시작하지 않는다.
TC는 완료 대기 timeout을 반드시 제공해야 한다. 회전·scale·clipping 등 scene 조건을
캡처만으로 자동 추론하지 않으며, 기존 두 TC는 1:1 축 정렬 fixture를 calibration한다.

## 비교 규칙

- Image는 top-left 원점의 tightly packed RGBA8, Rect는 `[x, x+width) × [y, y+height)`이다.
- 서로 다른 크기나 잘못된 ROI를 자동 보정하지 않는다. 결과의 `valid`와 `equal`을 모두 확인한다.
- 기본 비교는 불투명 흰 배경을 사용한다. 투명/premultiplied 입력은 TC에서 합성 조건을 먼저 정의해야 한다.
- 채널 차이 16 이하는 허용한다. 그보다 큰 차이는 foreground 합집합의 0.1% 이하이면서 4 pixel 이하여야 동일하다.
- 넓은 흰 배경으로 오류 비율이 희석되지 않으며, 빈 텍스트 이미지 두 장은 기본적으로 valid 비교가 아니다.
- Overflow 전환은 동일 조건으로 새로 생성한 CLIP/ELLIPSIS Label과 비교한다. 넘치는 text라도 보이는 픽셀이 반드시 바뀐다고 가정하지 않는다.
- 혼합 BiDi는 elision 경계에서 run 배치가 달라질 수 있어 Marquee 시작 전후의 고정 ROI 동등 비교를 하지 않는다. `NOT_APPLICABLE`을 남기며, 순수 LTR/RTL의 위치 비교와 모든 case의 이동·정지·복귀 검사는 유지한다.
- `EstimateTranslation()`은 이동의 진단/관찰용이다. 정적인 위치 비교 전에 이미지를 이동시켜 offset 오류를 숨기면 안 된다.
- Sync/Async parity와 전환 후 복귀는 독립적인 정답 검사가 아니다. 동일한 결함을 공유하는 경우를 놓칠 수 있다.

## 실행 및 결과

각 TC의 Auto 실행이 공용 모듈을 사용한다. `run-report.h/.cpp`는 지정 경로에 누적 로그를,
고유한 `<지정 경로>.<test-id>.XXXXXX/report.log`에 실행별 로그를 저장한다.
개별 Report 옆에 `<report>.images-XXXXXX/`를 생성하며 로그와 PNG 모두 이전 실행을 덮어쓰지 않는다.
Report 로그에는 경로와 case별 판정이 기록된다.
PNG의 magenta 영역은 허용 채널 오차를 넘는 차이다. 일부 영역만 비교한 검사는 로그의
ROI를 기준으로 읽는다. 전체 diff에는 의도적으로 바뀐 비교 영역 밖도 표시될 수 있다.

이미지 알고리즘 자체의 테스트는 Ubuntu에서 다음과 같이 실행한다.

```sh
cmake -S manual-tests/dali-ui-text -B /tmp/dali-ui-text-build \
  -DCMAKE_BUILD_TYPE=Release -DTEXT_IMAGE_CHECK_SELF_TESTS=ON
cmake --build /tmp/dali-ui-text-build -j4
cd /tmp/dali-ui-text-build
ctest --output-on-failure
```

Marquee는 기본 1초 delay와 약 3초 이동으로 관찰 시간을 확보하고, 1.5초 delay/큰 gap도 검사한다. 캡처 완료 간격이 650ms를
넘거나 개별 캡처가 500ms보다 오래 걸리면 cycle을 놓칠 수 있으므로 ERROR다.
이 캡처 경로에는 GPU readback·PNG I/O 비용이 있으므로 성능 측정에 사용하지 않는다.
