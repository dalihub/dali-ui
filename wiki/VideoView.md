# DALi UI Foundation - VideoView

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/VideoView-(kr))

`VideoView` is a view for displaying video from an externally created Tizen player session (MMPlayer or ESPlayer). Unlike other resource-backed views, `VideoView` does not create or own the native player — the application creates and manages the platform player itself, and `VideoView` only attaches it for display and forwards playback commands.

---

## Table of Contents

1. [Overview & Ownership Model](#1-overview--ownership-model)
2. [Basic Setup](#2-basic-setup)
   - [MMPlayer Underlay](#21-mmplayer-underlay)
   - [MMPlayer NativeImage](#22-mmplayer-nativeimage)
   - [ESPlayer NativeImage / Underlay](#23-esplayer-nativeimage--underlay)
3. [Choosing a Rendering Mode](#3-choosing-a-rendering-mode)
4. [VideoSource](#4-videosource)
   - [Creation Helpers](#41-creation-helpers)
   - [VideoSourceOwnership](#42-videosourceownership)
   - [VideoRenderingMode](#43-videorenderingmode)
5. [Playback Control](#5-playback-control)
6. [Lifecycle & Ordering](#6-lifecycle--ordering)
7. [Platform Backend Notes](#7-platform-backend-notes)
8. [Default Values](#8-default-values)
9. [Important Notes](#9-important-notes)

---

## 1. Overview & Ownership Model

`VideoView` is not a self-contained resource loader like `ImageView`. There is no `SetResourceUrl()` that fetches and prepares a video by itself. Instead, the application:

1. Creates and configures the native player (Tizen CAPI `player_h` for MMPlayer, or `esplusplayer_handle` for ESPlayer) itself — but does **not** prepare or play it yet.
2. Wraps the native handle as a `VideoSource`: the app fills a `Dali::VideoSourceDescriptor` (provider id + native handle + rendering mode) and passes it to `Dali::Ui::CreateVideoSource()`.
3. Creates a `VideoView` from that source and adds it to the window.
4. **Only then** prepares the native player and starts playback.
5. When done, clears the source and destroys the native handle itself.

```
App:  player_create() → player_set_uri()  (native handle stays IDLE)
App:  descriptor{providerId, nativeSession, flags};  source = Ui::CreateVideoSource(descriptor)
App:  videoView = VideoView::New(source);  window.Add(videoView)
                        ↓ synchronous — display is attached here, while the player is IDLE
App:  player_prepare()  →  videoView.Play()
   ...
App:  videoView.ClearSource();  player_unprepare(player);  player_destroy(player)
```

`VideoView` never calls `prepare`, `unprepare`, or `destroy` on the native handle — that lifecycle is entirely the application's responsibility. See [Lifecycle & Ordering](#6-lifecycle--ordering) for why the order above matters.

---

## 2. Basic Setup

### 2.1 MMPlayer Underlay

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/adaptor-framework/video-source-descriptor.h>
#include <player.h>

using namespace Dali::Ui;

// 1. Create and configure the native player (still IDLE)
player_h player;
player_create(&player);
player_set_uri(player, "video.mp4");

// 2. Fill a descriptor and wrap it as a VideoSource
Dali::VideoSourceDescriptor descriptor;
descriptor.SetProviderId("tizen.mmplayer");
descriptor.SetNativeSession(Dali::Any(static_cast<void*>(player)));
descriptor.SetRenderingMode(Dali::VideoRenderingMode::UNDERLAY);
VideoSource source = Dali::Ui::CreateVideoSource(descriptor);

// 3. Create the VideoView and add it to the window
VideoView videoView = VideoView::New(source);
videoView.SetRequestedWidth(640.0f);
videoView.SetRequestedHeight(360.0f);
window.Add(videoView); // display attaches here, synchronously, while the player is IDLE

// 4. Prepare and play
player_prepare(player);
videoView.Play();

// ... later, on teardown ...
videoView.ClearSource();
player_unprepare(player);
player_destroy(player);
```

---

### 2.2 MMPlayer NativeImage

Only the rendering mode changes (`NATIVE_IMAGE` instead of `UNDERLAY`); the rest of the flow is identical to underlay.

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

ESPlayer sessions use the elementary-stream `esplusplayer_capi` and prepare asynchronously. The app must submit stream data (and EOS) from a `ready_to_prepare` callback, and start playback once the player reaches `ESPLUSPLAYER_STATE_READY`.

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
// NativeImage (GPU texture):
descriptor.SetRenderingMode(Dali::VideoRenderingMode::NATIVE_IMAGE);
// Underlay (hole-punch), instead:
// descriptor.SetRenderingMode(Dali::VideoRenderingMode::UNDERLAY);
VideoSource source = Dali::Ui::CreateVideoSource(descriptor);

VideoView videoView = VideoView::New(source);
window.Add(videoView);

esplusplayer_prepare_async(player);

// In OnReadyToPrepare: submit ES packets, then submit EOS on each stream so
// prepare_async can finish (see esplusplayer_submit_packet / submit_eos_packet).
// Once the player reaches ESPLUSPLAYER_STATE_READY (poll, or react to
// OnPrepareAsyncDone), call videoView.Play().
```

> For a runnable reference covering all four modes, see `samples/video-view/video-view-example.cpp` (run with `es`, `es-underlay`, `mn`, or no argument).

---

## 3. Choosing a Rendering Mode

There is no separate mode-selection API — the rendering mode is determined entirely by the rendering mode you set (`SetRenderingMode()`).

| Mode | `VideoRenderingMode` | Pros | Cons |
|---|---|---|---|
| **Underlay** | `UNDERLAY` | Platform-composited (hole-punch), lower overhead; geometry/ROI kept in sync with the view automatically | Video always renders beneath the UI layer; DALi render effects/blending cannot be applied directly to the frame |
| **NativeImage** | `NATIVE_IMAGE` | Decoded frames become a regular GPU texture — supports corner radius, blending, and other View render effects like any other content | Extra decode-to-texture copy overhead |

### Player → Provider ID → Rendering mode

Set `providerId` and `renderingMode` on the `Dali::VideoSourceDescriptor` yourself; the mode values are `Dali::VideoRenderingMode::` enumerators.

| Player | Provider ID | Rendering mode |
|---|---|---|
| MMPlayer | `"tizen.mmplayer"` | `UNDERLAY` or `NATIVE_IMAGE` |
| ESPlayer | `"tizen.esplayer"` | `UNDERLAY` or `NATIVE_IMAGE` |

---

## 4. VideoSource

`VideoSource` is a lightweight, immutable description of a native player session. It does not own the native handle.

### 4.1 Building a VideoSource

Creation is a two-step composition, so dali-ui itself carries no dependency on the Tizen player headers:

1. The app fills a `Dali::VideoSourceDescriptor` (declared in `<dali/public-api/adaptor-framework/video-source-descriptor.h>`) — provider id, native session handle, and rendering mode. There is no per-provider helper; the app sets the fields itself using the [mapping table above](#player--provider-id--rendering-mode):

```cpp
Dali::VideoSourceDescriptor descriptor;
descriptor.SetProviderId("tizen.mmplayer");
descriptor.SetNativeSession(Dali::Any(static_cast<void*>(player)));
descriptor.SetRenderingMode(Dali::VideoRenderingMode::UNDERLAY);
```

2. The **bridge** `Dali::Ui::CreateVideoSource()`, declared in `<dali-ui-foundation/public-api/video/video-source.h>`, turns that descriptor into a `VideoSource`:

```cpp
VideoSource CreateVideoSource(const SourceDescriptor& descriptor, VideoSourceOwnership ownership = VideoSourceOwnership::EXTERNAL);
```

```cpp
VideoSource source = Dali::Ui::CreateVideoSource(descriptor);
```

Applications should always go through `CreateVideoSource()`, not `VideoSource::New()` directly.

---

### 4.2 VideoSourceOwnership

`VideoSourceOwnership` is `Dali::VideoSourceOwnership`, declared in
`<dali/public-api/adaptor-framework/video-source-descriptor.h>` — `dali-ui` reuses
it rather than declaring its own, to avoid a name collision with the same-named
adaptor enum when both are in scope via `using namespace`. `CreateVideoSource()`
takes it directly as a plain parameter rather than wrapping it in an options
object; ownership is the only setting a caller can attach to a source.

| `VideoSourceOwnership` | Meaning |
|---|---|
| `EXTERNAL` (default) | The caller owns the native session; `VideoView` never prepares/unprepares/destroys it. |
| `SHARED` | The native session is shared or reference-counted. |
| `TRANSFER` | Ownership is transferred, if the underlying provider supports it. |

```cpp
VideoSource source = Dali::Ui::CreateVideoSource(descriptor, VideoSourceOwnership::SHARED);
```

---

### 4.3 VideoRenderingMode

`VideoRenderingMode` is also `Dali::VideoRenderingMode` (same header as
`VideoSourceOwnership` above), shared rather than redeclared:

```cpp
enum class VideoRenderingMode : uint32_t
{
  UNDERLAY     = 0, // Platform-composited hole-punch; renders beneath the UI.
  NATIVE_IMAGE = 1, // Decoded frames become a GPU texture; supports UI render effects.
};
```

You set the mode when building the descriptor (`SetRenderingMode()`). Query it back with `VideoSource::GetRenderingMode()`:

```cpp
bool isNativeImage = source.GetRenderingMode() == VideoRenderingMode::NATIVE_IMAGE;
```

---

## 5. Playback Control

```cpp
bool attached = videoView.SetSource(source); // returns false if the source is invalid
VideoSource current = videoView.GetSource();
videoView.ClearSource();

videoView.Play();
videoView.Pause();
videoView.Stop();
```

`Play()`/`Pause()`/`Stop()` forward to the native player. An application that prefers to drive playback itself can simply not call them and instead use the native player's own start/pause/stop functions directly (e.g. `player_start()`, `esplusplayer_start()`).

---

## 6. Lifecycle & Ordering

- **Attach display before preparing.** `window.Add(videoView)` synchronously attaches the display target (or creates the `NativeImage`) while the native player is still `IDLE`. Calling `player_prepare()`/`esplusplayer_prepare_async()` **before** the view is added to the window means the display binding APIs run too late — audio plays but no video is shown.
- **Scene attach/detach** is handled automatically: re-adding a `VideoView` to a window re-attaches the display; removing it detaches. The native player's own state (prepared/playing) is unaffected by scene attach/detach.
- **`ClearSource()` does not touch the native handle.** It only detaches display and releases `VideoView`'s own resources (visuals, `NativeImage`). The application must still call the native player's own stop/unprepare/close/destroy functions.

---

## 7. Platform Backend Notes

- **tcore vs. ecore is transparent to applications.** Whether the underlying `dali-extension` was built for the tcore or ecore Wayland backend is a build-time choice; the same ESPlayer underlay descriptor (`"tizen.esplayer"` + `VideoRenderingMode::UNDERLAY`) works either way.
- **ESPlayer sessions are always externally owned.** The application must call `esplusplayer_open()`/`esplusplayer_close()`/`esplusplayer_destroy()` itself; `VideoView` never does.

---

## 8. Default Values

| Property | Default |
|---|---|
| `CreateVideoSource()` ownership argument | `VideoSourceOwnership::EXTERNAL` |
| `VideoSourceDescriptor::renderingMode` | `VideoRenderingMode::UNDERLAY` |

---

## 9. Important Notes

- **Order matters.** Add the `VideoView` to the window (or otherwise trigger scene connection) **before** calling `player_prepare()`/`esplusplayer_prepare_async()`. Preparing first is a common mistake that results in audio-only playback with no video.
- **The application owns the native player's entire lifecycle** — creation, prepare, play/pause/stop, and final destruction. `VideoView` only attaches/detaches display and forwards the Play/Pause/Stop commands it is given.
- **ESPlayer preparation is asynchronous** and requires submitting elementary stream data (and EOS) from a `ready_to_prepare` callback before the player can reach `READY`.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
