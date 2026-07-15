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
   - [VideoSourceOptions](#42-videosourceoptions)
   - [VideoSourceCapabilities](#43-videosourcecapabilities)
5. [재생 제어](#5-재생-제어)
6. [생명주기와 순서](#6-생명주기와-순서)
7. [플랫폼 백엔드 관련 참고사항](#7-플랫폼-백엔드-관련-참고사항)
8. [기본값](#8-기본값)
9. [주의사항](#9-주의사항)

---

## 1. 개요와 소유권 모델

`VideoView`는 `ImageView`처럼 리소스를 알아서 불러오는 자기완결형 View가 아닙니다. `SetResourceUrl()`처럼 URL 하나 던지면 알아서 준비까지 해주는 API가 없습니다. 대신 앱이 다음 순서로 진행해야 합니다:

1. 네이티브 플레이어(MMPlayer용 Tizen CAPI `player_h`, 또는 ESPlayer용 `esplusplayer_handle`)를 직접 생성하고 설정합니다 — 단, 아직 prepare나 play는 하지 않습니다.
2. `Dali::Ui::Tizen::CreateVideoSourceFromXxx()` 헬퍼로 네이티브 핸들을 감싸 `VideoSource`를 만듭니다.
3. 그 source로 `VideoView`를 생성하고 window에 추가합니다.
4. **그 다음에야** 네이티브 플레이어를 prepare하고 재생을 시작합니다.
5. 종료 시 source를 해제하고, 네이티브 핸들도 앱이 직접 destroy합니다.

```
App:  player_create() → player_set_uri()  (네이티브 핸들은 아직 IDLE 상태)
App:  source = CreateVideoSourceFromMMPlayer(player)
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
#include <dali-ui-foundation/public-api/video/tizen-video-source.h>
#include <player.h>

using namespace Dali::Ui;

// 1. 네이티브 플레이어를 생성하고 설정 (아직 IDLE 상태)
player_h player;
player_create(&player);
player_set_uri(player, "video.mp4");

// 2. 핸들을 VideoSource로 감싸기
VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromMMPlayer(player);

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

생성 헬퍼 함수만 바뀌고 나머지 흐름은 underlay와 동일합니다.

```cpp
VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromMMPlayerNativeImage(player);
VideoView   videoView = VideoView::New(source);
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

// NativeImage (GPU 텍스처):
VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromESPlayer(player);
// 대신 Underlay (hole-punch)를 쓰려면:
// VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromESPlayerUnderlay(player);

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

별도의 모드 선택 API는 없습니다 — 렌더링 모드는 **어떤 생성 헬퍼를 호출하느냐**로 전적으로 결정됩니다.

| 모드 | 헬퍼 | 장점 | 단점 |
|---|---|---|---|
| **Underlay** | `CreateVideoSourceFromMMPlayer()`, `CreateVideoSourceFromESPlayerUnderlay()` | 플랫폼이 직접 합성(hole-punch)해서 오버헤드가 낮음; geometry/ROI가 view와 자동으로 동기화됨 | 영상이 항상 UI 레이어보다 아래에 그려짐; DALi 렌더 이펙트/블렌딩을 프레임 자체에 직접 적용할 수 없음 |
| **NativeImage** | `CreateVideoSourceFromMMPlayerNativeImage()`, `CreateVideoSourceFromESPlayer()` | 디코딩된 프레임이 일반 GPU 텍스처가 되어 다른 View 콘텐츠처럼 corner radius, 블렌딩, 렌더 이펙트 적용 가능 | 디코드 결과를 텍스처로 복사하는 추가 오버헤드 |

### 헬퍼 → 플레이어 → Capability 매핑

| 헬퍼 | 플레이어 | Capability | Provider ID |
|---|---|---|---|
| `CreateVideoSourceFromMMPlayer()` | MMPlayer | `SupportsUnderlay`, `SupportsSeek`, `SupportsVolume` | `"tizen.mmplayer"` |
| `CreateVideoSourceFromMMPlayerNativeImage()` | MMPlayer | `SupportsNativeImage`, `SupportsSeek`, `SupportsVolume` | `"tizen.mmplayer"` |
| `CreateVideoSourceFromESPlayer()` | ESPlayer | `SupportsNativeImage` | `"tizen.esplayer"` |
| `CreateVideoSourceFromESPlayerUnderlay()` | ESPlayer | `SupportsUnderlay` | `"tizen.esplayer"` |

---

## 4. VideoSource

`VideoSource`는 네이티브 플레이어 세션을 설명하는 가벼운 불변(immutable) 객체입니다. 네이티브 핸들을 소유하지 않습니다.

### 4.1 생성 헬퍼 함수

`<dali-ui-foundation/public-api/video/tizen-video-source.h>`에 선언되어 있습니다:

```cpp
VideoSource CreateVideoSourceFromMMPlayer(player_h player, const VideoSourceOptions& options = {});
VideoSource CreateVideoSourceFromMMPlayerNativeImage(player_h player, const VideoSourceOptions& options = {});
VideoSource CreateVideoSourceFromESPlayer(esplusplayer_handle player, const VideoSourceOptions& options = {});
VideoSource CreateVideoSourceFromESPlayerUnderlay(esplusplayer_handle player, const VideoSourceOptions& options = {});
```

각 헬퍼는 알맞은 provider ID와 capability 플래그를 설정한 뒤 내부적으로 `VideoSource::New()`를 호출합니다. 앱은 항상 이 헬퍼들을 거쳐야 하며, `VideoSource::New()`를 직접 호출해서는 안 됩니다.

---

### 4.2 VideoSourceOptions

```cpp
struct VideoSourceOptions
{
  VideoSourceOwnership ownership{VideoSourceOwnership::External};
  VideoControlPolicy   controlPolicy{VideoControlPolicy::ViewControlsPlayback};
};
```

| `VideoSourceOwnership` | 의미 |
|---|---|
| `External` (기본값) | 호출자가 네이티브 세션을 소유함; `VideoView`는 절대 prepare/unprepare/destroy하지 않음. |
| `Shared` | 네이티브 세션이 공유되거나 참조 카운트됨. |
| `Transfer` | 하위 provider가 지원하는 경우 소유권이 이전됨. |

| `VideoControlPolicy` | 의미 |
|---|---|
| `ViewControlsPlayback` (기본값) | `VideoView::Play()/Pause()/Stop()`이 네이티브 플레이어로 전달됨. |
| `DisplayOnly` | `VideoView`는 display 연결과 geometry 동기화만 담당함; 재생 제어는 앱이 네이티브 핸들에 직접 해야 함. |

```cpp
VideoSourceOptions options;
options.controlPolicy = VideoControlPolicy::DisplayOnly;
VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromMMPlayer(player, options);
```

---

### 4.3 VideoSourceCapabilities

```cpp
struct VideoSourceCapabilities
{
  enum Flag : uint32_t
  {
    SupportsUnderlay    = 1u << 0,
    SupportsNativeImage = 1u << 1,
    SupportsSeek        = 1u << 2,
    SupportsVolume      = 1u << 3,
  };
  uint32_t flags{0u};
};
```

Capability는 호출한 생성 헬퍼가 자동으로 설정하며, 앱이 직접 설정하지 않습니다. `VideoSource::GetCapabilities()`로 조회할 수 있습니다. 예를 들어 source가 NativeImage 렌더링을 쓰는지 확인하려면:

```cpp
bool isNativeImage = source.GetCapabilities().flags & VideoSourceCapabilities::SupportsNativeImage;
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

`Play()`/`Pause()`/`Stop()`은 `VideoControlPolicy::ViewControlsPlayback`(기본값)일 때만 동작합니다. `DisplayOnly`일 경우 이 호출들은 아무 동작도 하지 않습니다 — 네이티브 플레이어의 재생/일시정지/정지 함수(`player_start()`, `esplusplayer_start()` 등)를 직접 호출해야 합니다.

---

## 6. 생명주기와 순서

- **prepare보다 먼저 display를 연결하세요.** `window.Add(videoView)`는 네이티브 플레이어가 아직 `IDLE` 상태일 때 동기적으로 display target을 연결하거나 `NativeImage`를 생성합니다. view를 window에 추가하기 **전에** `player_prepare()`/`esplusplayer_prepare_async()`를 호출하면 display 연결 API가 너무 늦게 실행되어 — 소리는 나지만 화면에 영상이 안 보이는 문제가 생깁니다.
- **Scene attach/detach**는 자동으로 처리됩니다: `VideoView`를 window에 다시 추가하면 display가 재연결되고, 제거하면 연결이 해제됩니다. 네이티브 플레이어 자체의 상태(prepared/playing)는 scene attach/detach의 영향을 받지 않습니다.
- **`ClearSource()`는 네이티브 핸들을 건드리지 않습니다.** display 연결 해제와 `VideoView` 자체의 리소스(visual, `NativeImage`)만 해제합니다. 네이티브 플레이어의 stop/unprepare/close/destroy 함수는 앱이 직접 호출해야 합니다.

---

## 7. 플랫폼 백엔드 관련 참고사항

- **tcore/ecore 구분은 앱에게 투명합니다.** 하위 `dali-extension`이 tcore용으로 빌드됐는지 ecore용으로 빌드됐는지는 빌드 타임에 결정되는 사항이며, 동일한 `CreateVideoSourceFromESPlayerUnderlay()`(또는 MMPlayer underlay) 호출이 어느 쪽에서든 동작합니다.
- **ESPlayer 세션은 항상 외부 소유입니다.** 앱이 직접 `esplusplayer_open()`/`esplusplayer_close()`/`esplusplayer_destroy()`를 호출해야 하며, `VideoView`는 절대 이 함수들을 호출하지 않습니다.

---

## 8. 기본값

| 프로퍼티 | 기본값 |
|---|---|
| `VideoSourceOptions::ownership` | `VideoSourceOwnership::External` |
| `VideoSourceOptions::controlPolicy` | `VideoControlPolicy::ViewControlsPlayback` |
| `VideoSourceCapabilities::flags` | `0` (사용한 생성 헬퍼가 설정함) |

---

## 9. 주의사항

- **순서가 중요합니다.** `player_prepare()`/`esplusplayer_prepare_async()`를 호출하기 **전에** `VideoView`를 window에 추가하세요(또는 scene에 연결되도록 하세요). prepare를 먼저 하는 것은 흔한 실수이며, 소리는 나지만 영상이 안 보이는 결과로 이어집니다.
- **네이티브 플레이어의 전체 생명주기는 앱이 소유합니다** — 생성, prepare, play/pause/stop(`DisplayOnly`가 아닌 경우), 최종 destroy까지 전부. `VideoView`는 display 연결/해제와 명령 전달만 담당합니다.
- **`DisplayOnly` control policy**는 명령 전달을 완전히 비활성화합니다 — 앱이 네이티브 핸들의 재생을 전적으로 직접 제어해야 합니다.
- **ESPlayer prepare는 비동기입니다** — 플레이어가 `READY`에 도달하려면 `ready_to_prepare` 콜백에서 엘리멘터리 스트림 데이터(및 EOS)를 제출해야 합니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
