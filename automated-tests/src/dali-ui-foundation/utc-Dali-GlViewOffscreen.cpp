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
#include <dali-ui-foundation/public-api/views/gl/gl-view-offscreen-config.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <dali.h>
#include <atomic>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_glview_offscreen_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_glview_offscreen_cleanup(void)
{
  test_return_value = TET_PASS;
}

/**
 * GlView::BackendMode::OFFSCREEN_RENDERING needs buffers from the platform, and the
 * harness has no graphics device behind it, so whether the backend can be brought up is
 * not something these tests can assume either way, nor is there anything to ask. What
 * they cover is the contract that holds in both cases: a platform that cannot provide
 * the buffers leaves the view inert rather than misbehaving.
 *
 * Nothing here asserts what the GL callbacks were handed. This backend runs them on a
 * thread of its own, which the harness does not drive, so any such assertion would be a
 * race against it. The rendering thread is covered by samples/gl-view instead.
 */
namespace
{
constexpr float VIEW_SIZE = 100.0f;

struct GlCallbackRecorder
{
  void OnInit(const GlViewRenderInfo& /*info*/)
  {
    ++initCount;
  }

  bool OnRenderFrame(const GlViewRenderInfo& info)
  {
    ++frameCount;
    boundTextureCount = info.GetBoundTextureNativeHandles().Count();
    return true;
  }

  void OnTerminate()
  {
    ++terminateCount;
  }

  void OnTerminated()
  {
    ++terminatedCount;
  }

  void Register(GlView view)
  {
    view.RegisterGlCallbacks(
      Dali::Callback<void(const GlViewRenderInfo&)>::New(this, &GlCallbackRecorder::OnInit),
      Dali::Callback<bool(const GlViewRenderInfo&)>::New(this, &GlCallbackRecorder::OnRenderFrame),
      Dali::Callback<void()>::New(this, &GlCallbackRecorder::OnTerminate));
  }

  Dali::Callback<void()> CompletionCallback()
  {
    return Dali::Callback<void()>::New(this, &GlCallbackRecorder::OnTerminated);
  }

  /// Counted from the rendering thread where the backend has one, so read them only once
  /// the view has been terminated and that thread joined.
  std::atomic_uint32_t initCount{0u};
  std::atomic_uint32_t frameCount{0u};
  std::atomic_uint32_t terminateCount{0u};
  std::atomic_uint32_t boundTextureCount{0u};

  /// Event thread only.
  uint32_t terminatedCount{0u};
};

GlView CreateOffscreenGlView(UiTestApplication&           application,
                             GlCallbackRecorder&          recorder,
                             const GlViewOffscreenConfig& config = {})
{
  GlView view = GlView::New(GlView::BackendMode::OFFSCREEN_RENDERING, config);
  view.SetRequestedWidth(VIEW_SIZE);
  view.SetRequestedHeight(VIEW_SIZE);
  recorder.Register(view);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}

/// Waits for the terminate completion however it arrives: straight out of Terminate()
/// where there was no rendering thread to ask, or through the event thread where there
/// was one.
void WaitForTerminateCompletion(UiTestApplication& application, const GlCallbackRecorder& recorder)
{
  if(recorder.terminatedCount == 0u)
  {
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  }
}
} // namespace

// =============================================================================
// GlViewOffscreenConfig
// =============================================================================

int UtcDaliGlViewOffscreenConfigDefaultsP(void)
{
  UiTestApplication application;

  GlViewOffscreenConfig config;

  DALI_TEST_EQUALS(static_cast<int>(config.GetColorFormat()),
                   static_cast<int>(GlViewOffscreenConfig::ColorFormat::RGBA8888),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(config.IsDepthEnabled(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(config.IsStencilEnabled(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetSampleCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(config.GetGraphicsApiVersion()),
                   static_cast<int>(GlViewOffscreenConfig::GraphicsApiVersion::GLES_VERSION_3_0),
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOffscreenConfigSettersP(void)
{
  UiTestApplication application;

  GlViewOffscreenConfig config;
  config.SetColorFormat(GlViewOffscreenConfig::ColorFormat::RGB888);
  config.SetDepthEnabled(true);
  config.SetStencilEnabled(true);
  config.SetSampleCount(4u);
  config.SetGraphicsApiVersion(GlViewOffscreenConfig::GraphicsApiVersion::GLES_VERSION_2_0);

  DALI_TEST_EQUALS(static_cast<int>(config.GetColorFormat()),
                   static_cast<int>(GlViewOffscreenConfig::ColorFormat::RGB888),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(config.IsDepthEnabled(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(config.IsStencilEnabled(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetSampleCount(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(config.GetGraphicsApiVersion()),
                   static_cast<int>(GlViewOffscreenConfig::GraphicsApiVersion::GLES_VERSION_2_0),
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOffscreenConfigCopyAndAssignmentP(void)
{
  UiTestApplication application;

  GlViewOffscreenConfig config;
  config.SetDepthEnabled(true);
  config.SetSampleCount(2u);

  // A value, not a handle: the copy keeps what it was given whatever the source does next.
  GlViewOffscreenConfig copy(config);
  config.SetSampleCount(8u);

  DALI_TEST_EQUALS(copy.IsDepthEnabled(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSampleCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetSampleCount(), 8u, TEST_LOCATION);

  GlViewOffscreenConfig assigned;
  assigned = copy;
  DALI_TEST_EQUALS(assigned.GetSampleCount(), 2u, TEST_LOCATION);

  GlViewOffscreenConfig moved(std::move(copy));
  DALI_TEST_EQUALS(moved.GetSampleCount(), 2u, TEST_LOCATION);

  GlViewOffscreenConfig moveAssigned;
  moveAssigned = std::move(moved);
  DALI_TEST_EQUALS(moveAssigned.GetSampleCount(), 2u, TEST_LOCATION);

  END_TEST;
}

// =============================================================================
// Construction
// =============================================================================

int UtcDaliGlViewOffscreenNewP(void)
{
  UiTestApplication application;

  // The config is optional, and the backend is what New() records either way.
  GlView withoutConfig = GlView::New(GlView::BackendMode::OFFSCREEN_RENDERING);
  DALI_TEST_CHECK(withoutConfig);
  DALI_TEST_EQUALS(static_cast<int>(withoutConfig.GetBackendMode()),
                   static_cast<int>(GlView::BackendMode::OFFSCREEN_RENDERING),
                   TEST_LOCATION);

  GlViewOffscreenConfig config;
  config.SetDepthEnabled(true);

  GlView withConfig = GlView::New(GlView::BackendMode::OFFSCREEN_RENDERING, config);
  DALI_TEST_CHECK(withConfig);
  DALI_TEST_EQUALS(static_cast<int>(withConfig.GetBackendMode()),
                   static_cast<int>(GlView::BackendMode::OFFSCREEN_RENDERING),
                   TEST_LOCATION);

  DALI_TEST_CHECK(GlView::DownCast(withConfig));

  END_TEST;
}

int UtcDaliGlViewOffscreenConfigIgnoredByDirectBackendsP(void)
{
  UiTestApplication application;

  GlViewOffscreenConfig config;
  config.SetSampleCount(16u);
  config.SetStencilEnabled(true);

  // Read only for the offscreen backend, so the direct backends are unaffected by a
  // configuration they cannot honour - they draw into the window surface.
  GlView view = GlView::New(GlView::BackendMode::DIRECT_RENDERING, config);
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(static_cast<int>(view.GetBackendMode()),
                   static_cast<int>(GlView::BackendMode::DIRECT_RENDERING),
                   TEST_LOCATION);

  END_TEST;
}

// =============================================================================
// Rendering mode
// =============================================================================

int UtcDaliGlViewOffscreenRenderingModeP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateOffscreenGlView(application, recorder);

  DALI_TEST_EQUALS(static_cast<int>(view.GetRenderingMode()),
                   static_cast<int>(GlView::RenderingMode::CONTINUOUS),
                   TEST_LOCATION);

  view.SetRenderingMode(GlView::RenderingMode::ON_DEMAND);
  DALI_TEST_EQUALS(static_cast<int>(view.GetRenderingMode()),
                   static_cast<int>(GlView::RenderingMode::ON_DEMAND),
                   TEST_LOCATION);

  // Wakes the backend's own thread rather than asking for a DALi frame, and is safe
  // whether or not there is a thread to wake.
  view.RenderOnce();

  application.SendNotification();
  application.Render();

  view.SetRenderingMode(GlView::RenderingMode::CONTINUOUS);
  DALI_TEST_EQUALS(static_cast<int>(view.GetRenderingMode()),
                   static_cast<int>(GlView::RenderingMode::CONTINUOUS),
                   TEST_LOCATION);

  // Off the scene there is no window to ask for a frame, and the call stays safe.
  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();

  view.RenderOnce();
  DALI_TEST_CHECK(view);

  END_TEST;
}

// =============================================================================
// Texture bindings
// =============================================================================

int UtcDaliGlViewOffscreenBindTextureResourcesN(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateOffscreenGlView(application, recorder);

  Dali::Vector<Dali::Texture> textures;
  textures.PushBack(Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 16u, 16u));
  view.BindTextureResources(textures);

  application.SendNotification();
  application.Render();

  view.Terminate(recorder.CompletionCallback());
  WaitForTerminateCompletion(application, recorder);

  // Ignored rather than honoured: this backend renders on a context that shares none of
  // DALi's resources, so DALi's textures have no name there to hand over.
  DALI_TEST_EQUALS(recorder.boundTextureCount.load(), 0u, TEST_LOCATION);

  END_TEST;
}

// =============================================================================
// Terminate
// =============================================================================

int UtcDaliGlViewOffscreenTerminateP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateOffscreenGlView(application, recorder);

  view.Terminate(recorder.CompletionCallback());
  WaitForTerminateCompletion(application, recorder);

  // Reported exactly once, whether the backend had a thread to stop or never came up.
  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOffscreenTerminateThenSceneRemovalP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateOffscreenGlView(application, recorder);

  // What replacing a view does, and the order that matters: the scene removal pauses the
  // backend after it has already been asked to stop. Stopping has to win, or the thread
  // waits for a notification that is never coming and the terminate never completes.
  view.Terminate(recorder.CompletionCallback());
  application.GetScene().Remove(view);

  WaitForTerminateCompletion(application, recorder);

  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOffscreenTerminateNeverOnSceneP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  // Never shown, so the backend's thread was never started.
  GlView view = GlView::New(GlView::BackendMode::OFFSCREEN_RENDERING);
  recorder.Register(view);

  application.SendNotification();
  application.Render();

  view.Terminate(recorder.CompletionCallback());
  WaitForTerminateCompletion(application, recorder);

  // Nothing the application registered can have anything to release, but it is still
  // told that the terminate it asked for is done.
  DALI_TEST_EQUALS(recorder.initCount.load(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminateCount.load(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOffscreenTerminateTwiceP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateOffscreenGlView(application, recorder);

  view.Terminate(recorder.CompletionCallback());
  WaitForTerminateCompletion(application, recorder);

  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  // Idempotent - the second request is dropped, completion callback included.
  GlCallbackRecorder second;
  view.Terminate(second.CompletionCallback());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(second.terminatedCount, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOffscreenDestroyedWithoutTerminateP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  {
    GlView view = CreateOffscreenGlView(application, recorder);

    // Let the view go without ever asking for a terminate. Its thread is stopped and
    // joined by the destructor, so nothing outlives this scope.
    application.GetScene().Remove(view);
  }

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Nothing the application registered is invoked against a view it has let go of.
  DALI_TEST_EQUALS(recorder.terminateCount.load(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminatedCount, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOffscreenSceneConnectionP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateOffscreenGlView(application, recorder);

  // Taken off and put back: the backend pauses and resumes its thread rather than
  // rebuilding anything, and the view survives the round trip.
  for(int i = 0; i < 3; ++i)
  {
    application.GetScene().Remove(view);
    application.SendNotification();
    application.Render();

    application.GetScene().Add(view);
    application.SendNotification();
    application.Render();
  }

  DALI_TEST_CHECK(view);

  view.Terminate(recorder.CompletionCallback());
  WaitForTerminateCompletion(application, recorder);

  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOffscreenResizeP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateOffscreenGlView(application, recorder);

  // Each layout resizes the queue's buffers underneath a thread that may be drawing into
  // them, which is what the surface lock is for.
  for(float size = 40.0f; size <= 120.0f; size += 20.0f)
  {
    view.SetRequestedWidth(size);
    view.SetRequestedHeight(size + 10.0f);

    application.SendNotification();
    application.Render();
  }

  DALI_TEST_CHECK(view);

  view.Terminate(recorder.CompletionCallback());
  WaitForTerminateCompletion(application, recorder);

  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  END_TEST;
}
