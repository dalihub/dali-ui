# Windows LWE WebView 프레임 전달의 복사 비용 절감 검토

## 상태와 범위

- 상태: TODO / 장기 검토, 설계·측정 및 별도 승인 필요
- 작업 타입: WebView 렌더링 성능 분석, extension/adaptor 이미지 전달 계약 검토
- 관련 영역:
  - `dali-extension/dali-extension/web-engine-lwe/windows/web-engine-lwe-angle-renderer.*`
  - `dali-extension/dali-extension/web-engine-lwe/windows/web-engine-lwe-backend-win.*`
  - `dali-adaptor/dali/internal/imaging/windows/native-image-impl-win.*`
  - `dali-adaptor`의 NativeImage integration 계약과 ANGLE D3D11 backend
  - `dali-ui`에서 사용하는 Windows LWE WebView의 프레임 지연과 자원 사용량
- 목표: Windows LWE와 DALi 사이의 픽셀 복사·GPU 대기 비용을 측정하고, 제품 요구에
  비해 비용이 큰 경우 CPU readback과 재업로드를 줄이는 구조를 검토한다.

2026-09-14 현재 결정은 **adaptor 추가 수정 없이 ANGLE 기반 LWE를 사용**하는 것이다.
extension은 기존 NativeImage CPU 경로를 사용한다. 이 문서는 GPU 공유 구현의 도입
승인이나 즉시 실행 지시가 아니며, 현재 extension의 필수 의존성을 추가하지 않는다.

## 현재 확인된 구조

LWE는 기존 Windows 전용 스레드와 별도 EGL 컨텍스트에서 ANGLE pbuffer에 그린다.
전용 스레드는 WGL 전환 이전부터 존재했으며, DALi의 SDL loop와 LWE의 Win32 loop가
서로의 메시지를 소비하지 않도록 분리한 구조이다.

현재 프레임 전달은 다음 순서로 진행한다.

1. LWE의 GLES 렌더링 결과를 `glReadPixels`로 RGBA CPU 버퍼에 읽는다.
2. 행 순서를 상하 반전하여 top-down 이미지로 만든다.
3. DALi 이벤트 스레드에 최신 완료 프레임을 전달한다. 전달 대기 중인 이전 프레임은
   최신 프레임으로 교체하여 큐가 계속 늘어나지 않도록 한다.
4. `DevelNativeImage::SetPixels`가 기존 Windows NativeImage의 CPU 버퍼에 복사한다.
5. DALi 렌더 스레드에서 `glTexImage2D`로 텍스처에 업로드한다.

ANGLE 사용 자체에는 새로운 adaptor API가 필요하지 않다. 양쪽이 EGL/ANGLE을
사용한다는 사실만으로 컨텍스트의 GL 객체나 렌더링 결과가 자동으로 공유되지는 않는다.

## 검토 배경

현재 경로는 기존 adaptor와 연결하기 쉽지만 GPU → CPU → GPU 전송, CPU 버퍼 복사와
행 반전 비용이 매 프레임 발생한다. 동기 readback은 GPU 작업 완료를 기다릴 수 있다.
화면 크기, 갱신 빈도, WebView 수가 증가할수록 이 비용이 커질 가능성이 있다.

현재 Windows 실측 성능 자료는 없다. 정적인 작은 WebView에서도 변경 효과가 크다고
가정하거나, 공유 핸들 사용만으로 드라이버 내부까지 완전한 zero-copy라고 단정하지 않는다.

## 제안 방향: 별도 렌더링을 유지하고 완료된 GPU 이미지 공유

우선 검토할 후보는 LWE 전용 스레드·EGL 컨텍스트를 유지하면서 완료된 D3D 텍스처를
DALi에 전달하는 구조이다. DALi 이벤트 스레드나 렌더 스레드로 LWE 전체를 옮기는
작업과 구분하여 검토한다.

1. extension은 공유 가능한 ANGLE D3D11 렌더 타깃을 만들고 완료된 프레임을 게시한다.
2. adaptor는 일반적인 Windows native image source 계약으로 리소스를 받아, DALi의
   ANGLE display에서 import하고 텍스처로 샘플링한다. LWE 전용 정책을 adaptor에 넣지 않는다.
3. GPU 읽기·쓰기 완료와 소유권 반환을 연결하여 소비 중인 버퍼를 생산자가 덮어쓰지
   않도록 한다. 버퍼가 모두 사용 중이면 렌더링을 보류하고 이벤트 loop는 계속 처리한다.
4. NativeImage의 기존 공개 클래스 크기와 vtable을 보존할 수 있는 integration/internal
   확장 여부와 필요한 최소 adaptor 버전을 검토한다.

Tizen의 native buffer 전달 경로도 역할 분리의 참고 대상으로 삼되, TBM과 Windows
D3D의 공유·동기화 계약이 같다고 가정하지 않는다. DALi EGL 컨텍스트를 직접 사용하는
방안은 LWE thread affinity, GL 상태 복구, DALi render callback 계약까지 별도로 검토한다.

## 기존 실험과 재검토할 사항

아래 변경은 구현 참고 자료이며 현재 채택된 경로가 아니다.

- extension 실험: Gerrit [354031 patchset 2](https://review.tizen.org/gerrit/c/platform/core/uifw/dali-extension/+/354031/2), 커밋 `dd15ab0`
- adaptor 실험: Gerrit [354036 patchset 1](https://review.tizen.org/gerrit/c/platform/core/uifw/dali-adaptor/+/354036/1), 커밋 `c3a2fb60b`

실험은 D3D 공유 핸들, native resource 소유권, 최대 3개 생산 버퍼, D3D event query에
의한 완료 확인을 사용했다. 리소스 작업 및 WebGL 컨텍스트에는 별도 작은 EGL surface를
사용하여 소비 중인 출력 프레임을 건드리지 않도록 했다.

모의 EGL/D3D를 사용한 ASan/UBSan 수명 테스트와 Windows 소스 크로스 컴파일·smoke
실행 파일 링크는 통과했지만, 실제 Windows ANGLE import·픽셀 방향·페이지 표시·성능은
검증하지 못했다. 해당 커밋을 그대로 제품에 적용할 근거로 사용하지 않는다.

## 미결 사항과 예상 영향

1. 제품별 해상도·프레임률·동시 WebView 수에서 현재 경로가 병목인지 먼저 측정한다.
2. 양쪽 ANGLE 버전, GPU adapter, D3D feature level 및 hardware/WARP 조합에서 공유
   가능한 형식과 확장을 확인한다. 불가능한 경우의 CPU fallback 또는 실패 정책을 정한다.
3. 생산자 GPU 쓰기 완료와 소비자 GPU 읽기 완료를 각각 보장할 동기화 방법을 정한다.
   CPU callback 호출이나 명령 제출 완료를 GPU 완료로 간주하지 않는다.
4. 공유 핸들과 native allocation의 소유권, 리사이즈·종료·device loss 시 해제 순서,
   버퍼 재사용 계약을 정의한다. 핸들 값만 보관하는 것으로 수명이 유지된다고 가정하지 않는다.
5. 프레임 큐·버퍼 수를 제한하고, hidden WebView나 렌더 중단 시에도 자원이 반환되도록 한다.
6. RGBA/BGRA, alpha와 premultiplication, 색 공간, 이미지 방향의 의미를 명시한다.
7. 공개 ABI, integration 계약, 최소 adaptor 버전과 SDK/런타임 배포 비용을 평가한다.
8. CPU 버퍼 재사용, 불필요한 복사·행 반전 절감 또는 비동기 readback만으로 요구 성능을
   만족할 수 있는지도 비교한다. 복사 횟수만 줄이고 지연이나 유지보수 비용이 커지지 않게 한다.

## 검증 항목

1. 동일 Windows 장치·SDK·페이지에서 현재 CPU 경로를 기준으로 CPU/GPU 시간,
   readback 대기, 프레임 지연, 메모리 사용량을 측정한다.
2. 텍스트·이미지·투명 합성·Canvas/WebGL과 고해상도 애니메이션의 출력 및 성능을 비교한다.
3. 픽셀 형식·상하 방향·alpha 결과가 기준 경로와 일치하는지 확인한다.
4. GPU 완료를 지연시켜 버퍼 재사용, 큐 상한, 이벤트 loop 응답성을 검사한다.
5. 리사이즈 반복, 여러 WebView, hidden/재표시, 컨테이너 종료 뒤 마지막 프레임 유지와
   그래픽 리소스 재생성에서 누수·덮어쓰기·use-after-free가 없는지 확인한다.
6. 공유 import 실패, 장치 손실 및 지원하지 않는 adapter 조합에서 정한 정책을 검증한다.
7. 실제 Windows Debug/Release 빌드와 DALi 앱에서 검증하고, 도입 효과와 의존성 비용을
   함께 기록한 뒤 구현 여부와 범위를 결정한다.
