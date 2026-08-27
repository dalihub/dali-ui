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
#include <dali-ui-foundation/public-api/views/gl/gl-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_glview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_glview_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
constexpr float VIEW_SIZE = 100.0f;

/**
 * Stands in for the application's GL code. The init and render frame callbacks run on the
 * rendering thread, which the test harness drives from the test thread, so plain members
 * are enough here.
 */
struct GlCallbackRecorder
{
  void OnInit(const GlViewRenderInfo& info)
  {
    ++initCount;
    size = info.GetSize();
  }

  bool OnRenderFrame(const GlViewRenderInfo& info)
  {
    ++frameCount;
    clippingBox        = info.GetClippingBox();
    boundTextureCount  = info.GetBoundTextureNativeHandles().Count();
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
      Ui::Callback<void(const GlViewRenderInfo&)>::New(this, &GlCallbackRecorder::OnInit),
      Ui::Callback<bool(const GlViewRenderInfo&)>::New(this, &GlCallbackRecorder::OnRenderFrame),
      Ui::Callback<void()>::New(this, &GlCallbackRecorder::OnTerminate));
  }

  Ui::Callback<void()> CompletionCallback()
  {
    return Ui::Callback<void()>::New(this, &GlCallbackRecorder::OnTerminated);
  }

  uint32_t      initCount{0u};
  uint32_t      frameCount{0u};
  uint32_t      terminateCount{0u};
  uint32_t      terminatedCount{0u};
  Size          size{};
  BoundsInteger clippingBox{};
  uint32_t      boundTextureCount{0u};
};

/// Builds a view that is on the scene and has been drawn at least once.
GlView CreateDrawnGlView(UiTestApplication&  application,
                         GlCallbackRecorder& recorder,
                         GlView::BackendMode backendMode = GlView::BackendMode::DIRECT_RENDERING)
{
  GlView view = GlView::New(backendMode);
  view.SetRequestedWidth(VIEW_SIZE);
  view.SetRequestedHeight(VIEW_SIZE);
  recorder.Register(view);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}
} // namespace

// =============================================================================
// Construction
// =============================================================================

int UtcDaliGlViewConstructorP(void)
{
  UiTestApplication application;
  GlView            view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliGlViewNewP(void)
{
  UiTestApplication application;

  GlView direct = GlView::New(GlView::BackendMode::DIRECT_RENDERING);
  DALI_TEST_CHECK(direct);
  DALI_TEST_EQUALS(static_cast<int>(direct.GetBackendMode()),
                   static_cast<int>(GlView::BackendMode::DIRECT_RENDERING),
                   TEST_LOCATION);

  GlView unsafeDirect = GlView::New(GlView::BackendMode::UNSAFE_DIRECT_RENDERING);
  DALI_TEST_CHECK(unsafeDirect);
  DALI_TEST_EQUALS(static_cast<int>(unsafeDirect.GetBackendMode()),
                   static_cast<int>(GlView::BackendMode::UNSAFE_DIRECT_RENDERING),
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewCopyAndAssignmentP(void)
{
  UiTestApplication application;

  GlView view = GlView::New(GlView::BackendMode::DIRECT_RENDERING);

  GlView copy(view);
  DALI_TEST_CHECK(copy == view);

  GlView assigned;
  assigned = view;
  DALI_TEST_CHECK(assigned == view);

  GlView moved(std::move(view));
  DALI_TEST_CHECK(moved == copy);

  GlView moveAssigned;
  moveAssigned = std::move(moved);
  DALI_TEST_CHECK(moveAssigned == copy);

  END_TEST;
}

int UtcDaliGlViewDownCastP(void)
{
  UiTestApplication application;

  GlView     view   = GlView::New(GlView::BackendMode::DIRECT_RENDERING);
  BaseHandle handle = view;

  GlView downCast = GlView::DownCast(handle);
  DALI_TEST_CHECK(downCast);
  DALI_TEST_CHECK(downCast == view);

  END_TEST;
}

int UtcDaliGlViewDownCastN(void)
{
  UiTestApplication application;

  BaseHandle handle   = Handle::New();
  GlView     downCast = GlView::DownCast(handle);
  DALI_TEST_CHECK(!downCast);

  GlView fromEmpty = GlView::DownCast(BaseHandle());
  DALI_TEST_CHECK(!fromEmpty);

  END_TEST;
}

// =============================================================================
// Rendering control
// =============================================================================

int UtcDaliGlViewRenderingModeP(void)
{
  UiTestApplication application;

  GlView view = GlView::New(GlView::BackendMode::DIRECT_RENDERING);
  DALI_TEST_EQUALS(static_cast<int>(view.GetRenderingMode()),
                   static_cast<int>(GlView::RenderingMode::CONTINUOUS),
                   TEST_LOCATION);

  view.SetRenderingMode(GlView::RenderingMode::ON_DEMAND);
  DALI_TEST_EQUALS(static_cast<int>(view.GetRenderingMode()),
                   static_cast<int>(GlView::RenderingMode::ON_DEMAND),
                   TEST_LOCATION);

  view.SetRenderingMode(GlView::RenderingMode::CONTINUOUS);
  DALI_TEST_EQUALS(static_cast<int>(view.GetRenderingMode()),
                   static_cast<int>(GlView::RenderingMode::CONTINUOUS),
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewRenderOnceP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateDrawnGlView(application, recorder);
  DALI_TEST_CHECK(recorder.frameCount > 0u);

  view.SetRenderingMode(GlView::RenderingMode::ON_DEMAND);

  application.SendNotification();
  application.Render();

  const uint32_t frameCount = recorder.frameCount;

  view.RenderOnce();

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(recorder.frameCount > frameCount);

  // Off the scene there is no window to drive a frame from, but the call is still safe.
  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();

  view.RenderOnce();
  DALI_TEST_CHECK(view);

  END_TEST;
}

// =============================================================================
// GL callbacks
// =============================================================================

int UtcDaliGlViewRegisterGlCallbacksP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateDrawnGlView(application, recorder);

  // Init runs once, immediately before the first render frame callback.
  DALI_TEST_EQUALS(recorder.initCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(recorder.frameCount > 0u);
  DALI_TEST_EQUALS(recorder.terminateCount, 0u, TEST_LOCATION);

  const uint32_t frameCount = recorder.frameCount;

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(recorder.initCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(recorder.frameCount > frameCount);

  END_TEST;
}

int UtcDaliGlViewRegisterGlCallbacksAfterTerminateN(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateDrawnGlView(application, recorder);

  view.Terminate(recorder.CompletionCallback());

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));

  const uint32_t initCount  = recorder.initCount;
  const uint32_t frameCount = recorder.frameCount;

  // Ignored - a terminated view cannot be revived.
  GlCallbackRecorder ignored;
  ignored.Register(view);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(ignored.initCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(ignored.frameCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.initCount, initCount, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.frameCount, frameCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewBindTextureResourcesP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateDrawnGlView(application, recorder);

  DALI_TEST_EQUALS(recorder.boundTextureCount, 0u, TEST_LOCATION);

  Dali::Vector<Dali::Texture> textures;
  textures.PushBack(Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 16u, 16u));
  textures.PushBack(Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 16u, 16u));
  view.BindTextureResources(textures);

  application.SendNotification();
  application.Render();

  // Reported in the order they were bound, whether or not the upload has finished.
  DALI_TEST_EQUALS(recorder.boundTextureCount, 2u, TEST_LOCATION);

  // An empty list unbinds.
  view.BindTextureResources(Dali::Vector<Dali::Texture>());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(recorder.boundTextureCount, 0u, TEST_LOCATION);

  END_TEST;
}

// =============================================================================
// Terminate
// =============================================================================

int UtcDaliGlViewTerminateP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateDrawnGlView(application, recorder);
  DALI_TEST_CHECK(recorder.frameCount > 0u);

  view.Terminate(recorder.CompletionCallback());

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));

  // The GL resources are released on the rendering thread, and the completion is reported
  // back on the event thread.
  DALI_TEST_EQUALS(recorder.terminateCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  // The render frame callback stops being invoked.
  const uint32_t frameCount = recorder.frameCount;

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(recorder.frameCount, frameCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewTerminateWithoutCompletionCallbackP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateDrawnGlView(application, recorder);

  // The completion callback is optional.
  view.Terminate();

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));

  DALI_TEST_EQUALS(recorder.terminateCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminatedCount, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewTerminateTwiceP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateDrawnGlView(application, recorder);

  view.Terminate(recorder.CompletionCallback());

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));

  DALI_TEST_EQUALS(recorder.terminateCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  // Idempotent - the second request is dropped, completion callback included.
  GlCallbackRecorder second;
  view.Terminate(second.CompletionCallback());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(recorder.terminateCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(second.terminatedCount, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewTerminateNeverDrawnP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  // Never added to the scene, so none of the GL callbacks have run.
  GlView view = GlView::New(GlView::BackendMode::DIRECT_RENDERING);
  recorder.Register(view);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(recorder.initCount, 0u, TEST_LOCATION);

  view.Terminate(recorder.CompletionCallback());

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));

  // Nothing was created, so there is nothing for the terminate callback to release - but
  // the completion is reported all the same.
  DALI_TEST_EQUALS(recorder.initCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminateCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewTerminateWhileOffSceneP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  GlView view = CreateDrawnGlView(application, recorder);
  DALI_TEST_EQUALS(recorder.initCount, 1u, TEST_LOCATION);

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();

  view.Terminate(recorder.CompletionCallback());

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));

  DALI_TEST_EQUALS(recorder.terminatedCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewDestroyedWithoutTerminateP(void)
{
  UiTestApplication  application;
  GlCallbackRecorder recorder;

  {
    GlView view = CreateDrawnGlView(application, recorder);
    DALI_TEST_EQUALS(recorder.initCount, 1u, TEST_LOCATION);

    // Let the view go without ever asking for a terminate.
    application.GetScene().Remove(view);
  }

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Nothing the application registered is invoked against a view it has let go of.
  DALI_TEST_EQUALS(recorder.terminateCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.terminatedCount, 0u, TEST_LOCATION);

  END_TEST;
}
