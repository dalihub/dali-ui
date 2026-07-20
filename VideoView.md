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
   - [VideoSourceOptions](#42-videosourceoptions)
   - [VideoSourceCapabilities](#43-videosourcecapabilities)
5. [Playback Control](#5-playback-control)
6. [Lifecycle & Ordering](#6-lifecycle--ordering)
7. [Platform Backend Notes](#7-platform-backend-notes)
8. [Default Values](#8-default-values)
9. [Important Notes](#9-important-notes)

---

## 1. Overview & Ownership Model

`VideoView` is not a self-contained resource loader like `ImageView`. There is no `SetResourceUrl()` that fetches and prepares a video by itself. Instead, the application:

1. Creates and configures the native player (Tizen CAPI `player_h` for MMPlayer, or `esplusplayer_handle` for ESPlayer) itself — but does **not** prepare or play it yet.
2. Wraps the native handle as a `VideoSource` using a `Dali::Ui::Tizen::CreateVideoSourceFromXxx()` helper.
3. Creates a `VideoView` from that source and adds it to the window.
4. **Only then** prepares the native player and starts playback.
5. When done, clears the source and destroys the native handle itself.

```
App:  player_create() → player_set_uri()  (native handle stays IDLE)
App:  source = CreateVideoSourceFromMMPlayerUnderlay(player)
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
#include <dali-ui-foundation/public-api/video/tizen-video-source.h>
#include <player.h>

using namespace Dali::Ui;

// 1. Create and configure the native player (still IDLE)
player_h player;
player_create(&player);
player_set_uri(player, "video.mp4");

// 2. Wrap the handle as a VideoSource
VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromMMPlayerUnderlay(player);

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

Only the creation helper changes; the rest of the flow is identical to underlay.

```cpp
VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromMMPlayerNativeImage(player);
VideoView   videoView = VideoView::New(source);
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

// NativeImage (GPU texture):
VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromESPlayerNativeImage(player);
// Underlay (hole-punch), instead:
// VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromESPlayerUnderlay(player);

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

There is no separate mode-selection API — the rendering mode is determined entirely by **which creation helper you call**.

| Mode | Helpers | Pros | Cons |
|---|---|---|---|
| **Underlay** | `CreateVideoSourceFromMMPlayerUnderlay()`, `CreateVideoSourceFromESPlayerUnderlay()` | Platform-composited (hole-punch), lower overhead; geometry/ROI kept in sync with the view automatically | Video always renders beneath the UI layer; DALi render effects/blending cannot be applied directly to the frame |
| **NativeImage** | `CreateVideoSourceFromMMPlayerNativeImage()`, `CreateVideoSourceFromESPlayerNativeImage()` | Decoded frames become a regular GPU texture — supports corner radius, blending, and other View render effects like any other content | Extra decode-to-texture copy overhead |

### Helper → Player → Capability Mapping

| Helper | Player | Capability | Provider ID |
|---|---|---|---|
| `CreateVideoSourceFromMMPlayerUnderlay()` | MMPlayer | `SupportsUnderlay`, `SupportsSeek`, `SupportsVolume` | `"tizen.mmplayer"` |
| `CreateVideoSourceFromMMPlayerNativeImage()` | MMPlayer | `SupportsNativeImage`, `SupportsSeek`, `SupportsVolume` | `"tizen.mmplayer"` |
| `CreateVideoSourceFromESPlayerNativeImage()` | ESPlayer | `SupportsNativeImage` | `"tizen.esplayer"` |
| `CreateVideoSourceFromESPlayerUnderlay()` | ESPlayer | `SupportsUnderlay` | `"tizen.esplayer"` |

---

## 4. VideoSource

`VideoSource` is a lightweight, immutable description of a native player session. It does not own the native handle.

### 4.1 Creation Helpers

Declared in `<dali-ui-foundation/public-api/video/tizen-video-source.h>`:

```cpp
VideoSource CreateVideoSourceFromMMPlayerUnderlay(player_h player, const VideoSourceOptions& options = {});
VideoSource CreateVideoSourceFromMMPlayerNativeImage(player_h player, const VideoSourceOptions& options = {});
VideoSource CreateVideoSourceFromESPlayerNativeImage(esplusplayer_handle player, const VideoSourceOptions& options = {});
VideoSource CreateVideoSourceFromESPlayerUnderlay(esplusplayer_handle player, const VideoSourceOptions& options = {});
```

Each helper stamps the correct provider ID and capability flags, then calls `VideoSource::New()` internally. Applications should always go through these helpers, not `VideoSource::New()` directly.

---

### 4.2 VideoSourceOptions

```cpp
struct VideoSourceOptions
{
  VideoSourceOwnership ownership{VideoSourceOwnership::External};
  VideoControlPolicy   controlPolicy{VideoControlPolicy::ViewControlsPlayback};
};
```

| `VideoSourceOwnership` | Meaning |
|---|---|
| `External` (default) | The caller owns the native session; `VideoView` never prepares/unprepares/destroys it. |
| `Shared` | The native session is shared or reference-counted. |
| `Transfer` | Ownership is transferred, if the underlying provider supports it. |

| `VideoControlPolicy` | Meaning |
|---|---|
| `ViewControlsPlayback` (default) | `VideoView::Play()/Pause()/Stop()` forward to the native player. |
| `DisplayOnly` | `VideoView` only attaches display and keeps geometry in sync; the app must control playback directly on the native handle. |

```cpp
VideoSourceOptions options;
options.controlPolicy = VideoControlPolicy::DisplayOnly;
VideoSource source = Dali::Ui::Tizen::CreateVideoSourceFromMMPlayerUnderlay(player, options);
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

Capabilities are set automatically by the creation helper you call — you do not set them yourself. Query them back with `VideoSource::GetCapabilities()`, e.g. to check whether a source uses NativeImage rendering:

```cpp
bool isNativeImage = source.GetCapabilities().flags & VideoSourceCapabilities::SupportsNativeImage;
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

`Play()`/`Pause()`/`Stop()` only take effect when `VideoControlPolicy::ViewControlsPlayback` is used (the default). With `DisplayOnly`, these calls are no-ops — call the native player's own start/pause/stop functions directly (e.g. `player_start()`, `esplusplayer_start()`).

---

## 6. Lifecycle & Ordering

- **Attach display before preparing.** `window.Add(videoView)` synchronously attaches the display target (or creates the `NativeImage`) while the native player is still `IDLE`. Calling `player_prepare()`/`esplusplayer_prepare_async()` **before** the view is added to the window means the display binding APIs run too late — audio plays but no video is shown.
- **Scene attach/detach** is handled automatically: re-adding a `VideoView` to a window re-attaches the display; removing it detaches. The native player's own state (prepared/playing) is unaffected by scene attach/detach.
- **`ClearSource()` does not touch the native handle.** It only detaches display and releases `VideoView`'s own resources (visuals, `NativeImage`). The application must still call the native player's own stop/unprepare/close/destroy functions.

---

## 7. Platform Backend Notes

- **tcore vs. ecore is transparent to applications.** Whether the underlying `dali-extension` was built for the tcore or ecore Wayland backend is a build-time choice; the same `CreateVideoSourceFromESPlayerUnderlay()` (or MMPlayer underlay) call works either way.
- **ESPlayer sessions are always externally owned.** The application must call `esplusplayer_open()`/`esplusplayer_close()`/`esplusplayer_destroy()` itself; `VideoView` never does.

---

## 8. Default Values

| Property | Default |
|---|---|
| `VideoSourceOptions::ownership` | `VideoSourceOwnership::External` |
| `VideoSourceOptions::controlPolicy` | `VideoControlPolicy::ViewControlsPlayback` |
| `VideoSourceCapabilities::flags` | `0` (set by the creation helper used) |

---

## 9. Important Notes

- **Order matters.** Add the `VideoView` to the window (or otherwise trigger scene connection) **before** calling `player_prepare()`/`esplusplayer_prepare_async()`. Preparing first is a common mistake that results in audio-only playback with no video.
- **The application owns the native player's entire lifecycle** — creation, prepare, play/pause/stop (unless `DisplayOnly`), and final destruction. `VideoView` only attaches/detaches display and forwards commands.
- **`DisplayOnly` control policy** disables command forwarding entirely; the app is fully responsible for driving playback on the native handle.
- **ESPlayer preparation is asynchronous** and requires submitting elementary stream data (and EOS) from a `ready_to_prepare` callback before the player can reach `READY`.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
