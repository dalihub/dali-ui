/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/video/video-source.h>
#include <dali-ui-foundation/public-api/views/video/video-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/devel-api/adaptor-framework/video-player.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_videoview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_videoview_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
struct VideoPlayerCalls
{
  uint32_t renderingTarget{0u};
  uint32_t sceneConnection{0u};
  uint32_t playback{0u};
  uint32_t displayArea{0u};
};

VideoPlayerCalls& GetVideoPlayerCalls()
{
  static VideoPlayerCalls calls;
  return calls;
}

// VideoSource::New() only forwards providerId/nativeSession opaquely to the adaptor
// plugin (see VideoSource::ToAdaptorDescriptor); it never dereferences nativeSession
// itself, and no platform video-player plugin is loaded in this test environment.
// A dummy address stands in for a real player_h/esplusplayer_handle.
int gDummySession = 0;

VideoSource CreateTestSource(VideoSourceOwnership ownership, VideoRenderingMode renderingMode)
{
  return VideoSource::New("test.provider", &gDummySession, ownership, renderingMode);
}

VideoSource CreateUnderlaySource()
{
  return CreateTestSource(VideoSourceOwnership::EXTERNAL, VideoRenderingMode::UNDERLAY);
}

VideoSource CreateNativeImageSource()
{
  return CreateTestSource(VideoSourceOwnership::EXTERNAL, VideoRenderingMode::NATIVE_IMAGE);
}
} // namespace

// The existing tests run without a platform plugin. Record the adaptor boundary
// calls so reconnect tests also detect an otherwise invisible target reset.
namespace DALI_NAMESPACE
{
void VideoPlayer::SetRenderingTarget(Any /* target */)
{
  ++GetVideoPlayerCalls().renderingTarget;
}

void VideoPlayer::SceneConnection()
{
  ++GetVideoPlayerCalls().sceneConnection;
}

void VideoPlayer::Play()
{
  ++GetVideoPlayerCalls().playback;
}

void VideoPlayer::Pause()
{
  ++GetVideoPlayerCalls().playback;
}

void VideoPlayer::Stop()
{
  ++GetVideoPlayerCalls().playback;
}

void VideoPlayer::SetPlayPosition(int /* millisecond */)
{
  ++GetVideoPlayerCalls().playback;
}

void VideoPlayer::SetDisplayArea(DisplayArea /* area */)
{
  ++GetVideoPlayerCalls().displayArea;
}
} // namespace DALI_NAMESPACE

// =============================================================================
// VideoSource - Construction
// =============================================================================

int UtcDaliVideoSourceConstructorP(void)
{
  UiTestApplication application;
  VideoSource       source;
  DALI_TEST_CHECK(!source);
  END_TEST;
}

int UtcDaliVideoSourceNewP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  DALI_TEST_CHECK(source);
  DALI_TEST_CHECK(source.IsValid());
  END_TEST;
}

int UtcDaliVideoSourceNewInvalidSessionN(void)
{
  UiTestApplication application;
  VideoSource       source = VideoSource::New("test.provider", nullptr, VideoSourceOwnership::EXTERNAL, VideoRenderingMode::UNDERLAY);
  DALI_TEST_CHECK(!source.IsValid());
  END_TEST;
}

int UtcDaliVideoSourceNewInvalidProviderIdN(void)
{
  UiTestApplication application;
  VideoSource       source = VideoSource::New(nullptr, &gDummySession, VideoSourceOwnership::EXTERNAL, VideoRenderingMode::UNDERLAY);
  DALI_TEST_CHECK(!source.IsValid());
  END_TEST;
}

int UtcDaliVideoSourceNewEmptyProviderIdN(void)
{
  UiTestApplication application;

  // An empty provider id names no plugin, so it is rejected like a null one.
  VideoSource source = VideoSource::New("", &gDummySession, VideoSourceOwnership::EXTERNAL, VideoRenderingMode::UNDERLAY);

  DALI_TEST_CHECK(!source);
  DALI_TEST_CHECK(!source.IsValid());
  END_TEST;
}

int UtcDaliVideoSourceCopyConstructorP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  VideoSource       copy(source);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.IsValid());
  END_TEST;
}

int UtcDaliVideoSourceMoveConstructorP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  VideoSource       moved  = std::move(source);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!source);
  END_TEST;
}

int UtcDaliVideoSourceAssignmentOperatorP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  VideoSource       copy;
  copy = source;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.IsValid());
  END_TEST;
}

int UtcDaliVideoSourceDownCastP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  BaseHandle        base   = source;
  VideoSource       result = VideoSource::DownCast(base);
  DALI_TEST_CHECK(result);
  END_TEST;
}

int UtcDaliVideoSourceDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        base;
  VideoSource       result = VideoSource::DownCast(base);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

// =============================================================================
// VideoSource - Ownership and RenderingMode
// =============================================================================

int UtcDaliVideoSourceRenderingModeUnderlayP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();

  DALI_TEST_EQUALS(static_cast<uint32_t>(source.GetRenderingMode()), static_cast<uint32_t>(VideoRenderingMode::UNDERLAY), TEST_LOCATION);
  END_TEST;
}

int UtcDaliVideoSourceRenderingModeNativeImageP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateNativeImageSource();

  DALI_TEST_EQUALS(static_cast<uint32_t>(source.GetRenderingMode()), static_cast<uint32_t>(VideoRenderingMode::NATIVE_IMAGE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliVideoSourceDefaultOwnershipP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();

  DALI_TEST_EQUALS(static_cast<uint32_t>(source.GetOwnership()), static_cast<uint32_t>(VideoSourceOwnership::EXTERNAL), TEST_LOCATION);
  END_TEST;
}

int UtcDaliVideoSourceCustomOwnershipP(void)
{
  UiTestApplication application;

  VideoSource source = CreateTestSource(VideoSourceOwnership::SHARED, VideoRenderingMode::UNDERLAY);

  DALI_TEST_EQUALS(static_cast<uint32_t>(source.GetOwnership()), static_cast<uint32_t>(VideoSourceOwnership::SHARED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliVideoSourceCreateFromDescriptorP(void)
{
  UiTestApplication application;

  // CreateVideoSource() views the temporary Dali::String that
  // descriptor.GetProviderId() returns by value, so the source must own a copy of
  // the provider id rather than point into the caller's buffer.
  Dali::VideoSourceDescriptor descriptor;
  descriptor.SetProviderId("tizen.mmplayer");
  descriptor.SetNativeSession(Dali::Any(static_cast<void*>(&gDummySession)));
  descriptor.SetRenderingMode(Dali::VideoRenderingMode::UNDERLAY);

  VideoSource source = CreateVideoSource(descriptor);

  DALI_TEST_CHECK(source);
  DALI_TEST_CHECK(source.IsValid());
  DALI_TEST_EQUALS(static_cast<uint32_t>(source.GetRenderingMode()), static_cast<uint32_t>(VideoRenderingMode::UNDERLAY), TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// VideoView - Construction
// =============================================================================

int UtcDaliVideoViewConstructorP(void)
{
  UiTestApplication application;
  VideoView         view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliVideoViewNewP(void)
{
  UiTestApplication application;
  VideoView         view = VideoView::New();
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(!view.GetSource());
  END_TEST;
}

int UtcDaliVideoViewNewWithSourceP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  VideoView         view   = VideoView::New(source);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(view.GetSource());
  END_TEST;
}

int UtcDaliVideoViewCopyConstructorP(void)
{
  UiTestApplication application;
  VideoView         view = VideoView::New();
  VideoView         copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliVideoViewMoveConstructorP(void)
{
  UiTestApplication application;
  VideoView         view  = VideoView::New();
  VideoView         moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliVideoViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  VideoView         view = VideoView::New();
  VideoView         copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliVideoViewMoveAssignmentOperatorP(void)
{
  UiTestApplication application;
  VideoView         view = VideoView::New();
  VideoView         moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

int UtcDaliVideoViewDownCastP(void)
{
  UiTestApplication application;
  VideoView         view   = VideoView::New();
  View              base   = view;
  VideoView         result = VideoView::DownCast(base);
  DALI_TEST_CHECK(result);
  END_TEST;
}

int UtcDaliVideoViewDownCastN(void)
{
  UiTestApplication application;
  View              base   = View::New();
  VideoView         result = VideoView::DownCast(base);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

// =============================================================================
// VideoView - SetSource / GetSource / ClearSource
// =============================================================================

int UtcDaliVideoViewSetSourceP(void)
{
  UiTestApplication application;
  VideoView         view   = VideoView::New();
  VideoSource       source = CreateUnderlaySource();

  bool result = view.SetSource(source);
  DALI_TEST_CHECK(result);

  VideoSource attached = view.GetSource();
  DALI_TEST_CHECK(attached);
  DALI_TEST_CHECK(attached == source);
  END_TEST;
}

int UtcDaliVideoViewSetSourceInvalidN(void)
{
  UiTestApplication application;
  VideoView         view = VideoView::New();

  VideoSource emptySource;
  bool        result = view.SetSource(emptySource);
  DALI_TEST_CHECK(!result);
  DALI_TEST_CHECK(!view.GetSource());
  END_TEST;
}

int UtcDaliVideoViewSetSourceReplacesExistingP(void)
{
  UiTestApplication application;
  VideoView         view = VideoView::New();

  VideoSource firstSource  = CreateUnderlaySource();
  VideoSource secondSource = CreateNativeImageSource();

  DALI_TEST_CHECK(view.SetSource(firstSource));
  DALI_TEST_CHECK(view.SetSource(secondSource));

  VideoSource attached = view.GetSource();
  DALI_TEST_CHECK(attached == secondSource);
  END_TEST;
}

int UtcDaliVideoViewClearSourceP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  VideoView         view   = VideoView::New(source);
  DALI_TEST_CHECK(view.GetSource());

  view.ClearSource();
  DALI_TEST_CHECK(!view.GetSource());
  END_TEST;
}

// =============================================================================
// VideoView - Playback control and scene connection
//
// No platform video-player plugin is loaded in this test environment, so these
// checks record VideoPlayer calls without actual playback or display binding.
// =============================================================================

int UtcDaliVideoViewPlaybackControlNoCrashP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  VideoView         view   = VideoView::New(source);

  view.Play();
  view.Pause();
  view.Stop();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliVideoViewSceneConnectionP(void)
{
  UiTestApplication application;
  VideoSource       source = CreateUnderlaySource();
  VideoView         view   = VideoView::New(source);
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliVideoViewSyncModeBeforeSourceP(void)
{
  UiTestApplication application;
  VideoView         view = VideoView::New();
  DALI_TEST_CHECK(view.GetSyncMode() == Ui::VideoSyncMode::DISABLED);
  view.SetSyncMode(Ui::VideoSyncMode::ENABLED);
  DALI_TEST_CHECK(view.GetSyncMode() == Ui::VideoSyncMode::ENABLED);
  view.SetSource(CreateUnderlaySource());
  view.SetSyncMode(Ui::VideoSyncMode::DISABLED);
  DALI_TEST_CHECK(view.GetSyncMode() == Ui::VideoSyncMode::ENABLED);
  view.ClearSource();
  view.SetSyncMode(Ui::VideoSyncMode::DISABLED);
  DALI_TEST_CHECK(view.GetSyncMode() == Ui::VideoSyncMode::DISABLED);
  DALI_TEST_CHECK(view.SetSource(CreateUnderlaySource()));
  DALI_TEST_CHECK(view.GetSyncMode() == Ui::VideoSyncMode::DISABLED);
  END_TEST;
}

int UtcDaliVideoViewSyncModeWithSourcePreservesTargetP(void)
{
  UiTestApplication application;
  GetVideoPlayerCalls() = {};
  VideoSource source    = CreateUnderlaySource();
  VideoView   view      = VideoView::New(source);
  application.GetWindow().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  Renderer renderer = view.GetRendererAt(0u);

  view.SetSyncMode(Ui::VideoSyncMode::DISABLED);
  view.SetSyncMode(Ui::VideoSyncMode::ENABLED);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.GetSyncMode() == Ui::VideoSyncMode::DISABLED);
  DALI_TEST_CHECK(view.GetSource() == source);
  DALI_TEST_CHECK(view.GetRendererAt(0u) == renderer);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().playback, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVideoViewUnderlayAttachmentWithoutAdaptorN(void)
{
  UiTestApplication application;
  GetVideoPlayerCalls() = {};
  Window      window    = application.GetWindow();
  VideoSource source    = CreateUnderlaySource();
  VideoView   view      = VideoView::New(source);

  Test::UiAdaptor::SetAdaptorAvailableForce(false);
  window.Add(view);
  Test::UiAdaptor::SetAdaptorAvailableForce(true);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().sceneConnection, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().displayArea, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(view.GetSource() == source);

  window.Remove(view);
  window.Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().sceneConnection, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(GetVideoPlayerCalls().displayArea > 0u);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().playback, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(view.GetSource() == source);
  END_TEST;
}

int UtcDaliVideoViewSameWindowReconnectionPreservesTargetP(void)
{
  UiTestApplication application;
  GetVideoPlayerCalls() = {};
  Window    window      = application.GetWindow();
  VideoView view        = VideoView::New(CreateUnderlaySource());
  window.Add(view);
  application.SendNotification();
  application.Render();
  const auto connections = window.ResizedSignal().GetConnectionCount();
  for(uint32_t i = 0u; i < 3u; ++i)
  {
    window.Remove(view);
    window.Add(view);
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().playback, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(window.ResizedSignal().GetConnectionCount(), connections, TEST_LOCATION);

  view.ClearSource();
  view.SetSource(CreateUnderlaySource());
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVideoViewDifferentWindowReconnectionRejectedP(void)
{
  UiTestApplication application;
  GetVideoPlayerCalls() = {};
  Window    first       = application.GetWindow();
  Window    second      = Window::New(PositionSize(0, 0, 100, 100), "video-second");
  VideoView view        = VideoView::New(CreateUnderlaySource());
  first.Add(view);
  first.Remove(view);
  const auto displayAreaCalls = GetVideoPlayerCalls().displayArea;
  second.Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().sceneConnection, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().playback, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().displayArea, displayAreaCalls, TEST_LOCATION);

  second.Remove(view);
  first.Add(view);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().sceneConnection, 2u, TEST_LOCATION);

  first.Remove(view);
  second.Add(view);
  view.ClearSource();
  DALI_TEST_CHECK(view.SetSource(CreateUnderlaySource()));
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().sceneConnection, 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVideoViewNativeImageReconnectionPreservesTargetP(void)
{
  UiTestApplication application;
  GetVideoPlayerCalls() = {};
  Window    first       = application.GetWindow();
  Window    second      = Window::New(PositionSize(0, 0, 100, 100), "native-second");
  VideoView view        = VideoView::New(CreateNativeImageSource());
  first.Add(view);
  first.Remove(view);
  second.Add(view);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().renderingTarget, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().sceneConnection, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVideoPlayerCalls().playback, 0u, TEST_LOCATION);
  END_TEST;
}
