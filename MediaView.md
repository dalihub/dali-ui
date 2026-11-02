# DALi UI Foundation - MediaView

[→ 한국어 문서](MediaView-(kr).md)

`Dali::Ui::MediaView` displays frames from an application-owned MMFW
`media_frame_source_h`. The application creates, configures, starts, stops,
and destroys the player or camera. MediaView only handles presentation.

Include `<dali-ui-foundation/public-api/views/media/media-view.h>` or the
foundation umbrella header. Tizen source connections require the optional
`dali3-extension-media-frame-source-plugin` package and the matching
mediatool, player, and camera implementation.

Apply `Dali::Ui::UiConfig::New().Apply()` after creating the Application and
before entering its main loop. Set the view's requested width and height
through Ui's layout APIs.

## Connecting a source on Tizen

Obtain the player source while the player is IDLE, with display NONE and
gapless disabled. Connect the view before preparing the player. Wrap the
source in `Dali::Any` with its original `media_frame_source_h` type.

```cpp
// player already exists and has its URI configured; check each CAPI result.
media_frame_source_h source = nullptr;
if(player_get_media_frame_source(player, &source) != PLAYER_ERROR_NONE)
{
  return;
}

auto view = Dali::Ui::MediaView::New(Dali::Any(source));
if(!view)
{
  return; // Plugin unavailable, source unsupported, or already connected.
}

view.SetRequestedWidth(640.0f);
view.SetRequestedHeight(360.0f);
window.Add(view);
// Now call player_prepare(player), then player_start(player).
```

For camera, set display NONE and obtain the source with
`camera_get_media_frame_source()` in the CREATED state. Connect and add the
view before `camera_start_preview()`. A source connection and a public
media-packet preview callback are mutually exclusive.

## Presentation and lifecycle

| API | Behavior |
|---|---|
| `New()` | Creates a view without a source |
| `New(Any)` | Returns a connected view, or an empty handle on failure |
| `SetSource(Any)` | Releases the previous connection; failure leaves the view disconnected |
| `GetSource()` | Returns the borrowed source, or an empty Any |
| `ClearSource()` | Disconnects callbacks and returns retained packets; repeated calls are safe |
| `SetSourceCrop(Rect<float>)` | Sets a normalized crop before source rotation; invalid values preserve the previous crop |
| `SetHorizontalFlipEnabled(bool)` / `SetVerticalFlipEnabled(bool)` | Reflects the image along the display axes after source rotation |

The default crop is `(0, 0, 1, 1)` and both flips are disabled. Crop values
must be finite, remain inside the source, and have positive width and height.
The cropped, rotated image preserves its aspect ratio within the view.

Use all MediaView APIs on the DALi event thread. One source supports one
MediaView. Removing the view from the scene returns its packets and clears
its texture; adding it again resumes presentation when new frames arrive.

For shutdown, stop the producer, call `view.ClearSource()`, unparent/reset
the view, then unprepare/destroy the player or destroy the camera. The
producer owns the source; MediaView does not extend the producer's lifetime.

The backend keeps one pending frame and the three most recently presented
packets. RETURN_BUFFER returns all packets immediately. This policy does
not track GPU completion. Gapless playback and keep_screen are unsupported.

## Tizen validation executables

Build player/camera with `--define 'media_view_tests 1'` in GBS. Run
`player_test_media_view_ui 30` or `camera_test_media_view_ui 30` with
`XDG_RUNTIME_DIR=/run WAYLAND_DISPLAY=wayland-0`. Player uses
`/home/video-01.mp4`; camera needs a configured preview source. These tests
exercise crop, flips, resize, scene removal/reconnection, ten stop/start
cycles, and teardown. Confirm actual screen contents as well as `RESULT PASS`.

## Rendering and natural size

MediaView supplies frames to the internal StreamedImageVisual, which declares
corner radius and borderline capabilities. It shares the image vertex and
fragment shaders without adding state to ImageVisual. Crop and flips use
signed pixelArea; quarter turns enable the IS_REQUIRED_SWAP_UV shader variant.
MediaView converts the backend's displayed size to the uncropped, unrotated
content size expected by the visual, keeping texture padding out of layout.
The cropped and rotated frame dimensions determine wrap-content measurement
and aspect fitting. A 256×64 crop fits a 200×200 view as 200×50. Requested
dimensions take precedence; with one requested dimension, the other follows
the frame aspect ratio. Frame geometry changes invalidate measurement even
when the Texture handle is unchanged. No frame means zero content natural size.
