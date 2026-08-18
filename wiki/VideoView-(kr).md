# DALi UI Foundation - VideoView

[→ English Documentation](https://github.sec.samsung.net/NUI/dali-ui/wiki/VideoView)

`VideoView`는 외부에서 생성된 Tizen 플레이어 세션(MMPlayer 또는 ESPlayer)을 화면에 표시하는 View입니다. 다른 리소스 기반 View들과 달리, `VideoView`는 네이티브 플레이어를 직접 생성하거나 소유하지 않습니다 — 앱이 직접 플랫폼 플레이어를 생성/관리하고, `VideoView`는 그것을 화면에 붙여서 재생 명령을 전달하는 역할만 합니다.

---

## 목차

1. [개요와 소유권 모델](#1-개요와-소유권-모델)
2. [기본 설정](#2-기본-설정)
   - [MMPlayer Underlay](#21-mmplayer-underlay)
   - [MMPlayer NativeImage](#22-mmplayer-nativeimage)
   - [ESPlayer NativeImage / Underlay](#23-esplayer-nativeimage--underlay)
3. [렌더링 모드 선택](#3-렌더링-모드-선택)
4. [VideoSource](#4-videosource)
   - [생성 헬퍼 함수](#41-생성-헬퍼-함수)
   - [VideoSourceOwnership](#42-videosourceownership)
   - [VideoRenderingMode](#43-videorenderingmode)
5. [재생 제어](#5-재생-제어)
6. [생명주기와 순서](#6-생명주기와-순서)
7. [플랫폼 백엔드 관련 참고사항](#7-플랫폼-백엔드-관련-참고사항)
8. [기본값](#8-기본값)
9. [주의사항](#9-주의사항)

---

## 1. 개요와 소유권 모델

`VideoView`는 `ImageView`처럼 리소스를 알아서 불러오는 자기완결형 View가 아닙니다. `SetResourceUrl()`처럼 URL 하나 던지면 알아서 준비까지 해주는 API가 없습니다. 대신 앱이 다음 순서로 진행해야 합니다:

1. 네이티브 플레이어(MMPlayer용 Tizen CAPI `player_h`, 또는 ESPlayer용 `esplusplayer_handle`)를 직접 생성하고 설정합니다 — 단, 아직 prepare나 play는 하지 않습니다.
2. 네이티브 핸들을 `VideoSource`로 감쌉니다: 앱이 `Dali::VideoSourceDescriptor`(provider id + 네이티브 핸들 + 렌더링 모드)를 채워 `Dali::Ui::CreateVideoSource()`에 넘깁니다.
3. 그 source로 `VideoView`를 생성하고 window에 추가합니다.
4. **그 다음에야** 네이티브 플레이어를 prepare하고 재생을 시작합니다.
5. 종료 시 source를 해제하고, 네이티브 핸들도 앱이 직접 destroy합니다.

```
App:  player_create() → player_set_uri()  (네이티브 핸들은 아직 IDLE 상태)
App:  descriptor{providerId, nativeSession, flags};  source = Ui::CreateVideoSource(descriptor)
App:  videoView = VideoView::New(source);  window.Add(videoView)
                        ↓ 동기적으로 실행됨 — 플레이어가 IDLE 상태일 때 display가 연결됨
App:  player_prepare()  →  videoView.Play()
   ...
App:  videoView.ClearSource();  player_unprepare(player);  player_destroy(player)
```

`VideoView`는 네이티브 핸들에 대해 `prepare`, `unprepare`, `destroy`를 절대 호출하지 않습니다 — 이 생명주기는 전적으로 앱의 책임입니다. 위 순서가 왜 중요한지는 [생명주기와 순서](#6-생명주기와-순서)를 참고하세요.

---

## 2. 기본 설정

### 2.1 MMPlayer Underlay

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/adaptor-framework/video-source-descriptor.h>
#include <player.h>

using namespace Dali::Ui;

// 1. 네이티브 플레이어를 생성하고 설정 (아직 IDLE 상태)
player_h player;
player_create(&player);
player_set_uri(player, "video.mp4");

// 2. descriptor를 채워 VideoSource로 감싸기
Dali::VideoSourceDescriptor descriptor;
descriptor.SetProviderId("tizen.mmplayer");
descriptor.SetNativeSession(Dali::Any(static_cast<void*>(player)));
descriptor.SetRenderingMode(Dali::VideoRenderingMode::UNDERLAY);
VideoSource source = Dali::Ui::CreateVideoSource(descriptor);

// 3. VideoView를 생성하고 window에 추가
VideoView videoView = VideoView::New(source);
videoView.SetRequestedWidth(640.0f);
videoView.SetRequestedHeight(360.0f);
window.Add(videoView); // 이 시점에 동기적으로 display가 연결됨 (플레이어는 IDLE 상태)

// 4. prepare 후 재생
player_prepare(player);
videoView.Play();

// ... 이후 종료 시점 ...
videoView.ClearSource();
player_unprepare(player);
player_destroy(player);
```

---

### 2.2 MMPlayer NativeImage

렌더링 모드만 바뀌고(`UNDERLAY` → `NATIVE_IMAGE`) 나머지 흐름은 underlay와 동일합니다.

```cpp
descriptor.SetRenderingMode(Dali::VideoRenderingMode::NATIVE_IMAGE);
VideoSource source     = Dali::Ui::CreateVideoSource(descriptor);
VideoView   videoView  = VideoView::New(source);
window.Add(videoView);

player_prepare(player);
videoView.Play();
```

---

### 2.3 ESPlayer NativeImage / Underlay

ESPlayer 세션은 엘리멘터리 스트림 기반 `esplusplayer_capi`를 사용하며 비동기로 prepare됩니다. 앱은 `ready_to_prepare` 콜백에서 스트림 데이터(및 EOS)를 제출해야 하고, 플레이어가 `ESPLUSPLAYER_STATE_READY` 상태가 되면 재생을 시작해야 합니다.

```cpp
#include <esplusplayer_capi/esplusplayer_capi.h>

esplusplayer_handle player = esplusplayer_create();
esplusplayer_open(player);
esplusplayer_set_video_stream_info(player, &videoInfo);
esplusplayer_set_ready_to_prepare_cb(player, OnReadyToPrepare, userData);
esplusplayer_set_prepare_async_done_cb(player, OnPrepareAsyncDone, userData);

Dali::VideoSourceDescriptor descriptor;
descriptor.SetProviderId("tizen.esplayer");
descriptor.SetNativeSession(Dali::Any(static_cast<void*>(player)));
// NativeImage (GPU 텍스처):
descriptor.SetRenderingMode(Dali::VideoRenderingMode::NATIVE_IMAGE);
// 대신 Underlay (hole-punch)를 쓰려면:
// descriptor.SetRenderingMode(Dali::VideoRenderingMode::UNDERLAY);
VideoSource source = Dali::Ui::CreateVideoSource(descriptor);

VideoView videoView = VideoView::New(source);
window.Add(videoView);

esplusplayer_prepare_async(player);

// OnReadyToPrepare에서: ES 패킷을 제출한 뒤, 각 스트림에 EOS를 제출해야
// prepare_async가 완료될 수 있습니다 (esplusplayer_submit_packet /
// submit_eos_packet 참고). 플레이어가 ESPLUSPLAYER_STATE_READY에 도달하면
// (폴링하거나 OnPrepareAsyncDone에 반응해서) videoView.Play()를 호출합니다.
```

> 4가지 모드를 모두 다루는 실행 가능한 예제는 `samples/video-view/video-view-example.cpp`를 참고하세요 (`es`, `es-underlay`, `mn` 인자 또는 인자 없이 실행).

---

## 3. 렌더링 모드 선택

별도의 모드 선택 API는 없습니다 — 렌더링 모드는 `SetRenderingMode()`로 설정한 값으로 전적으로 결정됩니다.

| 모드 | `VideoRenderingMode` | 장점 | 단점 |
|---|---|---|---|
| **Underlay** | `UNDERLAY` | 플랫폼이 직접 합성(hole-punch)해서 오버헤드가 낮음; geometry/ROI가 view와 자동으로 동기화됨 | 영상이 항상 UI 레이어보다 아래에 그려짐; DALi 렌더 이펙트/블렌딩을 프레임 자체에 직접 적용할 수 없음 |
| **NativeImage** | `NATIVE_IMAGE` | 디코딩된 프레임이 일반 GPU 텍스처가 되어 다른 View 콘텐츠처럼 corner radius, 블렌딩, 렌더 이펙트 적용 가능 | 디코드 결과를 텍스처로 복사하는 추가 오버헤드 |

### 플레이어 → Provider ID → 렌더링 모드

`Dali::VideoSourceDescriptor`에 `providerId`와 `renderingMode`를 직접 설정합니다. 모드 값은 `Dali::VideoRenderingMode::` 열거자입니다.

| 플레이어 | Provider ID | 렌더링 모드 |
|---|---|---|
| MMPlayer | `"tizen.mmplayer"` | `UNDERLAY` 또는 `NATIVE_IMAGE` |
| ESPlayer | `"tizen.esplayer"` | `UNDERLAY` 또는 `NATIVE_IMAGE` |

---

## 4. VideoSource

`VideoSource`는 네이티브 플레이어 세션을 설명하는 가벼운 불변(immutable) 객체입니다. 네이티브 핸들을 소유하지 않습니다.

### 4.1 VideoSource 만들기

생성은 두 단계 조합으로 이루어지며, 덕분에 dali-ui 자체는 Tizen 플레이어 헤더에 대한 의존성이 전혀 없습니다:

1. 앱이 `Dali::VideoSourceDescriptor`(`<dali/public-api/adaptor-framework/video-source-descriptor.h>`에 선언)를 채웁니다 — provider id, 네이티브 세션 핸들, 렌더링 모드. 별도의 provider별 헬퍼는 없으며, [위 매핑 표](#플레이어--provider-id--렌더링-모드)를 참고해 앱이 필드를 직접 설정합니다:

```cpp
Dali::VideoSourceDescriptor descriptor;
descriptor.SetProviderId("tizen.mmplayer");
descriptor.SetNativeSession(Dali::Any(static_cast<void*>(player)));
descriptor.SetRenderingMode(Dali::VideoRenderingMode::UNDERLAY);
```

2. **브리지** `Dali::Ui::CreateVideoSource()` — `<dali-ui-foundation/public-api/video/video-source.h>`에 선언되어 있으며, descriptor를 `VideoSource`로 변환합니다:

```cpp
VideoSource CreateVideoSource(const SourceDescriptor& descriptor, VideoSourceOwnership ownership = VideoSourceOwnership::EXTERNAL);
```

```cpp
VideoSource source = Dali::Ui::CreateVideoSource(descriptor);
```

앱은 항상 `CreateVideoSource()`를 거쳐야 하며, `VideoSource::New()`를 직접 호출해서는 안 됩니다.

---

### 4.2 VideoSourceOwnership

`VideoSourceOwnership`은 `<dali/public-api/adaptor-framework/video-source-descriptor.h>`에
선언된 `Dali::VideoSourceOwnership`입니다 — `dali-ui`는 별도로 선언하지 않고 이걸
그대로 재사용합니다. 동일한 이름의 adaptor enum과 `using namespace`로 함께 스코프에
들어왔을 때 이름이 충돌하는 걸 피하기 위해서입니다. `CreateVideoSource()`는 이 값을
별도 options 객체로 감싸지 않고 그냥 파라미터로 직접 받습니다 — source에 붙일 수 있는
설정이 ownership 하나뿐이기 때문입니다.

| `VideoSourceOwnership` | 의미 |
|---|---|
| `EXTERNAL` (기본값) | 호출자가 네이티브 세션을 소유함; `VideoView`는 절대 prepare/unprepare/destroy하지 않음. |
| `SHARED` | 네이티브 세션이 공유되거나 참조 카운트됨. |
| `TRANSFER` | 하위 provider가 지원하는 경우 소유권이 이전됨. |

```cpp
VideoSource source = Dali::Ui::CreateVideoSource(descriptor, VideoSourceOwnership::SHARED);
```

---

### 4.3 VideoRenderingMode

`VideoRenderingMode`도 마찬가지로 `Dali::VideoRenderingMode`(위 `VideoSourceOwnership`과
같은 헤더)이며, 별도로 재선언하지 않고 재사용합니다:

```cpp
enum class VideoRenderingMode : uint32_t
{
  UNDERLAY     = 0, // 플랫폼 합성 hole-punch; UI 아래에 렌더링됨.
  NATIVE_IMAGE = 1, // 디코딩 프레임이 GPU 텍스처가 됨; UI 렌더 이펙트 적용 가능.
};
```

모드는 descriptor를 만들 때 `SetRenderingMode()`로 설정합니다. `VideoSource::GetRenderingMode()`로 조회할 수 있습니다:

```cpp
bool isNativeImage = source.GetRenderingMode() == VideoRenderingMode::NATIVE_IMAGE;
```

---

## 5. 재생 제어

```cpp
bool attached = videoView.SetSource(source); // source가 유효하지 않으면 false 반환
VideoSource current = videoView.GetSource();
videoView.ClearSource();

videoView.Play();
videoView.Pause();
videoView.Stop();
```

`Play()`/`Pause()`/`Stop()`은 네이티브 플레이어로 전달됩니다. 앱이 재생을 직접 제어하고 싶으면 이 호출들을 쓰지 않고 네이티브 플레이어의 재생/일시정지/정지 함수(`player_start()`, `esplusplayer_start()` 등)를 직접 호출하면 됩니다.

---

## 6. 생명주기와 순서

- **prepare보다 먼저 display를 연결하세요.** `window.Add(videoView)`는 네이티브 플레이어가 아직 `IDLE` 상태일 때 동기적으로 display target을 연결하거나 `NativeImage`를 생성합니다. view를 window에 추가하기 **전에** `player_prepare()`/`esplusplayer_prepare_async()`를 호출하면 display 연결 API가 너무 늦게 실행되어 — 소리는 나지만 화면에 영상이 안 보이는 문제가 생깁니다.
- **Scene attach/detach**는 자동으로 처리됩니다: `VideoView`를 window에 다시 추가하면 display가 재연결되고, 제거하면 연결이 해제됩니다. 네이티브 플레이어 자체의 상태(prepared/playing)는 scene attach/detach의 영향을 받지 않습니다.
- **`ClearSource()`는 네이티브 핸들을 건드리지 않습니다.** display 연결 해제와 `VideoView` 자체의 리소스(visual, `NativeImage`)만 해제합니다. 네이티브 플레이어의 stop/unprepare/close/destroy 함수는 앱이 직접 호출해야 합니다.

---

## 7. 플랫폼 백엔드 관련 참고사항

- **tcore/ecore 구분은 앱에게 투명합니다.** 하위 `dali-extension`이 tcore용으로 빌드됐는지 ecore용으로 빌드됐는지는 빌드 타임에 결정되는 사항이며, 동일한 ESPlayer underlay descriptor(`"tizen.esplayer"` + `VideoRenderingMode::UNDERLAY`)가 어느 쪽에서든 동작합니다.
- **ESPlayer 세션은 항상 외부 소유입니다.** 앱이 직접 `esplusplayer_open()`/`esplusplayer_close()`/`esplusplayer_destroy()`를 호출해야 하며, `VideoView`는 절대 이 함수들을 호출하지 않습니다.

---

## 8. 기본값

| 프로퍼티 | 기본값 |
|---|---|
| `CreateVideoSource()`의 ownership 인자 | `VideoSourceOwnership::EXTERNAL` |
| `VideoSourceDescriptor::renderingMode` | `VideoRenderingMode::UNDERLAY` |

---

## 9. 주의사항

- **순서가 중요합니다.** `player_prepare()`/`esplusplayer_prepare_async()`를 호출하기 **전에** `VideoView`를 window에 추가하세요(또는 scene에 연결되도록 하세요). prepare를 먼저 하는 것은 흔한 실수이며, 소리는 나지만 영상이 안 보이는 결과로 이어집니다.
- **네이티브 플레이어의 전체 생명주기는 앱이 소유합니다** — 생성, prepare, play/pause/stop, 최종 destroy까지 전부. `VideoView`는 display 연결/해제와 전달받은 Play/Pause/Stop 명령 전달만 담당합니다.
- **ESPlayer prepare는 비동기입니다** — 플레이어가 `READY`에 도달하려면 `ready_to_prepare` 콜백에서 엘리멘터리 스트림 데이터(및 EOS)를 제출해야 합니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
