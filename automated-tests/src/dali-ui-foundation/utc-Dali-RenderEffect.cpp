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
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>

#include <algorithm>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t DEFAULT_BLUR_RENDER_TASK_COUNT            = 3u;
constexpr uint32_t INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT = 4u;
constexpr uint32_t MASK_RENDER_TASK_COUNT                    = 2u; ///< Target and source render tasks.

uint32_t GetRenderTaskCount(UiTestApplication& application)
{
  return application.GetScene().GetRenderTaskList().GetTaskCount();
}

View CreateView(UiTestApplication& application)
{
  View view = View::New();
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Dali::Ui::Extension::View::SetSizeWidth(view, 400.0f);
  Dali::Ui::Extension::View::SetSizeHeight(view, 400.0f);
  application.GetScene().Add(view);
  view.Arrange(LayoutRect(0.0f, 0.0f, 400.0f, 400.0f));
  application.SendNotification();
  application.Render();
  application.RunIdles();
  return view;
}

/// @brief Creates a view of the given size and puts it on the scene.
///
/// The size is requested through the layout system rather than written to the actor. A scene-root
/// view is registered with the LayoutController, whose idle pass measures it and arranges it to the
/// derived bounds -- for a view with no requested size that is WRAP_CONTENT, i.e. zero, which would
/// overwrite any size written here directly.
View CreateSizedView(UiTestApplication& application, float width, float height)
{
  View view = View::New();
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
  return view;
}

/// @brief Creates a view of the given size without putting it on the scene.
///
/// Used by the scene-on/scene-off tests, which assert straight after Add()/Unparent() without ever
/// letting an idle layout pass run. The size is therefore written to the actor directly: a
/// requested size would only take effect once the LayoutController has processed the view.
View CreateDetachedView(float width, float height)
{
  View view = View::New();
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Dali::Ui::Extension::View::SetSizeWidth(view, width);
  Dali::Ui::Extension::View::SetSizeHeight(view, height);
  return view;
}

/// @brief Resizes a view through the layout system, which is what drives the render effect refresh.
/// @see CreateSizedView for why the size is requested rather than written to the actor.
void ResizeView(UiTestApplication& application, View view, float width, float height)
{
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
}

/// @brief Writes the rendered size of a view without going through the layout system.
/// Arrange() asserts on negative or non-finite bounds by contract, so sizes that are deliberately
/// invalid have to be written straight to the actor render properties.
void SetRawViewSize(View view, float width, float height)
{
  Dali::Ui::Extension::View::SetSizeWidth(view, width);
  Dali::Ui::Extension::View::SetSizeHeight(view, height);
}

void CheckTaskFrameBufferSize(RenderTask task, uint32_t expectedWidth, uint32_t expectedHeight)
{
  FrameBuffer frameBuffer = task.GetFrameBuffer();
  DALI_TEST_CHECK(frameBuffer);

  Texture texture = frameBuffer.GetColorTexture();
  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUALS(texture.GetWidth(), expectedWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), expectedHeight, TEST_LOCATION);
}

/// @brief Snapshots every render task currently in the list.
std::vector<Dali::RenderTask> SnapshotRenderTasks(RenderTaskList taskList)
{
  std::vector<Dali::RenderTask> tasks;
  for(uint32_t i = 0u; i < taskList.GetTaskCount(); ++i)
  {
    tasks.push_back(taskList.GetTask(i));
  }
  return tasks;
}

/// @brief Returns the tasks present now but absent from the given snapshot, in list order.
std::vector<Dali::RenderTask> RenderTasksAddedSince(RenderTaskList taskList, const std::vector<Dali::RenderTask>& before)
{
  std::vector<Dali::RenderTask> added;
  for(uint32_t i = 0u; i < taskList.GetTaskCount(); ++i)
  {
    Dali::RenderTask task = taskList.GetTask(i);
    if(std::find(before.begin(), before.end(), task) == before.end())
    {
      added.push_back(task);
    }
  }
  return added;
}

/// @brief Renders once to settle a pending change, then measures the draw count of one further
/// steady-state frame. Measuring without settling first would fold the transition frames into the
/// count as well.
int MeasureSteadyDrawCount(UiTestApplication& application, TraceCallStack& drawTrace)
{
  application.SendNotification();
  application.Render();

  drawTrace.Reset();
  application.SendNotification();
  application.Render();
  return drawTrace.CountMethod("DrawArrays");
}

void BlurRenderingFinishedCallback(void)
{
  tet_infoline("blur rendering finished signal emitted");
}

View CreateChildView(UiTestApplication& application, View parent, const UiColor& color)
{
  View view = View::New();
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetBackgroundColor(color);
  parent.Add(view);
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
  return view;
}
} // namespace

int UtcDaliRenderEffectDitherNoiseStrengthP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectDitherNoiseStrengthP");

  BackgroundBlurEffect backgroundBlur = BackgroundBlurEffect::New();
  DALI_TEST_EQUALS(backgroundBlur.GetDitherNoiseStrength(), 0.1f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetDitherNoiseStrength(0.5f);
  DALI_TEST_EQUALS(backgroundBlur.GetDitherNoiseStrength(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetDitherNoiseStrength(-1.0f);
  DALI_TEST_EQUALS(backgroundBlur.GetDitherNoiseStrength(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetDitherNoiseStrength(2.0f);
  DALI_TEST_EQUALS(backgroundBlur.GetDitherNoiseStrength(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  GaussianBlurEffect gaussianBlur = GaussianBlurEffect::New();
  DALI_TEST_EQUALS(gaussianBlur.GetDitherNoiseStrength(), 0.1f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetDitherNoiseStrength(0.25f);
  DALI_TEST_EQUALS(gaussianBlur.GetDitherNoiseStrength(), 0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetDitherNoiseStrength(-1.0f);
  DALI_TEST_EQUALS(gaussianBlur.GetDitherNoiseStrength(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetDitherNoiseStrength(2.0f);
  DALI_TEST_EQUALS(gaussianBlur.GetDitherNoiseStrength(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectBlurDownscaleFactorP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectBlurDownscaleFactorP");

  BackgroundBlurEffect backgroundBlur = BackgroundBlurEffect::New();
  DALI_TEST_EQUALS(backgroundBlur.GetBlurDownscaleFactor(), 0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetBlurDownscaleFactor(0.5f);
  DALI_TEST_EQUALS(backgroundBlur.GetBlurDownscaleFactor(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetBlurDownscaleFactor(0.0f);
  DALI_TEST_EQUALS(backgroundBlur.GetBlurDownscaleFactor(), 0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetBlurDownscaleFactor(2.0f);
  DALI_TEST_EQUALS(backgroundBlur.GetBlurDownscaleFactor(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  GaussianBlurEffect gaussianBlur = GaussianBlurEffect::New();
  DALI_TEST_EQUALS(gaussianBlur.GetBlurDownscaleFactor(), 0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetBlurDownscaleFactor(0.5f);
  DALI_TEST_EQUALS(gaussianBlur.GetBlurDownscaleFactor(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetBlurDownscaleFactor(0.0f);
  DALI_TEST_EQUALS(gaussianBlur.GetBlurDownscaleFactor(), 0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetBlurDownscaleFactor(2.0f);
  DALI_TEST_EQUALS(gaussianBlur.GetBlurDownscaleFactor(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectOffScreenRenderingPropertyP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectOffScreenRenderingPropertyP");

  View view = CreateView(application);

  DALI_TEST_CHECK(view.OffScreenRenderingFinishedSignal().Empty());

  view.SetProperty(View::Property::OFFSCREEN_RENDERING, View::REFRESH_ALWAYS);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING), static_cast<int32_t>(View::REFRESH_ALWAYS), TEST_LOCATION);

  view.SetProperty(View::Property::OFFSCREEN_RENDERING, View::REFRESH_ONCE);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING), static_cast<int32_t>(View::REFRESH_ONCE), TEST_LOCATION);

  view.SetProperty(View::Property::OFFSCREEN_RENDERING, View::NONE);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING), static_cast<int32_t>(View::NONE), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectBackgroundBlurIntermediateDownsampleP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectBackgroundBlurIntermediateDownsampleP");

  View     view           = CreateView(application);
  uint32_t baseTaskCount  = GetRenderTaskCount(application);
  auto     backgroundBlur = BackgroundBlurEffect::New();

  backgroundBlur.SetBlurDownscaleFactor(0.5f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 400.0f, 400.0f));
  view.SetRenderEffect(backgroundBlur);
  application.SendNotification();
  application.Render();
  backgroundBlur.Activate();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  backgroundBlur.SetBlurDownscaleFactor(0.25f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);

  backgroundBlur.SetBlurOnce(true);
  backgroundBlur.SetBlurOnce(false);
  backgroundBlur.Refresh();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);

  view.ClearRenderEffect();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectGaussianBlurIntermediateDownsampleP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectGaussianBlurIntermediateDownsampleP");

  View     view          = CreateView(application);
  uint32_t baseTaskCount = GetRenderTaskCount(application);
  auto     gaussianBlur  = GaussianBlurEffect::New();

  gaussianBlur.SetBlurDownscaleFactor(0.5f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 400.0f, 400.0f));
  view.SetRenderEffect(gaussianBlur);
  application.SendNotification();
  application.Render();
  gaussianBlur.Activate();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  gaussianBlur.SetBlurDownscaleFactor(0.25f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);

  gaussianBlur.SetBlurOnce(true);
  gaussianBlur.SetBlurOnce(false);
  gaussianBlur.Refresh();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);

  view.ClearRenderEffect();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaskEffectRenderOnce(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliMaskEffectRenderOnce");

  View view = CreateSizedView(application, 400.0f, 400.0f);

  {
    tet_printf("Set render-once before the effect is set on the view\n");

    View     maskView      = CreateSizedView(application, 400.0f, 400.0f);
    uint32_t baseTaskCount = GetRenderTaskCount(application);

    MaskEffect maskEffect = MaskEffect::New(maskView);

    DALI_TEST_EQUALS(maskEffect.IsTargetRenderOnce(), false, TEST_LOCATION);
    DALI_TEST_EQUALS(maskEffect.IsSourceRenderOnce(), false, TEST_LOCATION);

    maskEffect.SetTargetRenderOnce(true);
    DALI_TEST_EQUALS(maskEffect.IsTargetRenderOnce(), true, TEST_LOCATION);

    maskEffect.SetSourceRenderOnce(true);
    DALI_TEST_EQUALS(maskEffect.IsSourceRenderOnce(), true, TEST_LOCATION);

    view.SetRenderEffect(maskEffect);
    application.SendNotification();
    application.Render();
    maskEffect.Activate();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    // The render-once tasks are finished by now, but the toolkit tests could not observe the
    // stopped tasks either, so only the flags are asserted here.
    DALI_TEST_EQUALS(maskEffect.IsTargetRenderOnce(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(maskEffect.IsSourceRenderOnce(), true, TEST_LOCATION);

    view.ClearRenderEffect();
    application.GetScene().Remove(maskView);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  }

  {
    tet_printf("Toggle render-once while the effect is activated\n");

    View     maskView      = CreateSizedView(application, 400.0f, 400.0f);
    uint32_t baseTaskCount = GetRenderTaskCount(application);

    MaskEffect maskEffect = MaskEffect::New(maskView);
    maskEffect.SetTargetRenderOnce(true);
    maskEffect.SetSourceRenderOnce(true);

    view.SetRenderEffect(maskEffect);
    application.SendNotification();
    application.Render();
    maskEffect.Activate();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    // Repeated and redundant setting must not disturb the already activated effect.
    maskEffect.SetTargetRenderOnce(false);
    maskEffect.SetTargetRenderOnce(false);
    maskEffect.SetTargetRenderOnce(true);
    maskEffect.SetTargetRenderOnce(true);
    maskEffect.SetTargetRenderOnce(false);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

    maskEffect.SetSourceRenderOnce(false);
    maskEffect.SetSourceRenderOnce(false);
    maskEffect.SetSourceRenderOnce(true);
    maskEffect.SetSourceRenderOnce(true);
    maskEffect.SetSourceRenderOnce(false);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(maskEffect.IsTargetRenderOnce(), false, TEST_LOCATION);
    DALI_TEST_EQUALS(maskEffect.IsSourceRenderOnce(), false, TEST_LOCATION);

    maskEffect.SetTargetRenderOnce(true);
    maskEffect.SetSourceRenderOnce(true);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(maskEffect.IsTargetRenderOnce(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(maskEffect.IsSourceRenderOnce(), true, TEST_LOCATION);

    view.ClearRenderEffect();
    application.GetScene().Remove(maskView);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliBackgroundBlurEffectSetSourceView(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliBackgroundBlurEffectSetSourceView");

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  View grandParentView = CreateSizedView(application, 400.0f, 400.0f);
  grandParentView.SetBackgroundColor(UiColor(0.0f, 0.0f, 0.0f, 1.0f));

  View parentView = CreateChildView(application, grandParentView, UiColor(1.0f, 0.0f, 0.0f, 1.0f));
  View view1      = CreateChildView(application, parentView, UiColor(0.0f, 1.0f, 0.0f, 1.0f));
  View view2      = CreateChildView(application, parentView, UiColor(0.0f, 0.0f, 1.0f, 1.0f));
  View view3      = CreateChildView(application, parentView, UiColor(1.0f, 1.0f, 1.0f, 1.0f));

  BackgroundBlurEffect effect = BackgroundBlurEffect::New(200u);
  DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

  // Everything below is expressed relative to this baseline, so that whatever the main window
  // already draws does not have to be counted here.
  const int baseDrawCount = MeasureSteadyDrawCount(application, drawTrace);
  tet_printf("Baseline draw count without any effect : %d\n", baseDrawCount);

  view2.SetRenderEffect(effect);
  application.SendNotification();
  application.Render();
  effect.Activate();

  tet_printf("Drawing the grand parent, the parent and view1 offscreen, plus the effect itself\n");
  DALI_TEST_EQUALS(MeasureSteadyDrawCount(application, drawTrace), baseDrawCount + 4, TEST_LOCATION);

  effect.SetSourceView(parentView);
  tet_printf("The grand parent drops out of the offscreen pass\n");
  DALI_TEST_EQUALS(MeasureSteadyDrawCount(application, drawTrace), baseDrawCount + 3, TEST_LOCATION);

  effect.SetStopperView(view1);
  tet_printf("view1 drops out of the offscreen pass as well\n");
  DALI_TEST_EQUALS(MeasureSteadyDrawCount(application, drawTrace), baseDrawCount + 2, TEST_LOCATION);

  effect.SetSourceView(View()); // Set empty handle
  tet_printf("The grand parent comes back, the stopper still cuts view1 off\n");
  DALI_TEST_EQUALS(MeasureSteadyDrawCount(application, drawTrace), baseDrawCount + 3, TEST_LOCATION);

  effect.SetStopperView(View()); // Set empty handle
  tet_printf("Back to the whole ancestor chain\n");
  DALI_TEST_EQUALS(MeasureSteadyDrawCount(application, drawTrace), baseDrawCount + 4, TEST_LOCATION);

  effect.SetSourceView(view1); // Not an ancestor of view2, so it must be ignored
  tet_printf("A non-ancestor source view is ignored, so nothing changes\n");
  DALI_TEST_EQUALS(MeasureSteadyDrawCount(application, drawTrace), baseDrawCount + 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectNewP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewP");

  RenderEffect blurEffect = BackgroundBlurEffect::New();
  DALI_TEST_CHECK(blurEffect);

  RenderEffect blurEffect2 = BackgroundBlurEffect::New(10);
  DALI_TEST_CHECK(blurEffect2);

  View view = View::New();

  RenderEffect maskEffect1 = MaskEffect::New(view);
  DALI_TEST_CHECK(maskEffect1);

  RenderEffect maskEffect2 = MaskEffect::New(view, MaskEffect::MaskMode::LUMINANCE, Vector2(0.f, 0.f), Vector2(1.f, 1.f));
  DALI_TEST_CHECK(maskEffect2);

  MaskEffect maskEffect3 = MaskEffect::New(view);
  DALI_TEST_CHECK(maskEffect3);

  MaskEffect maskEffect4 = maskEffect3;
  DALI_TEST_CHECK(maskEffect4);

  END_TEST;
}

int UtcDaliRenderEffectNewN(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectNewN");

  tet_printf("Check some invalid parameters clamp internally\n");

  RenderEffect blurEffect1 = BackgroundBlurEffect::New(10);
  RenderEffect blurEffect2 = BackgroundBlurEffect::New(0);
  RenderEffect blurEffect3 = BackgroundBlurEffect::New(2147483647);

  DALI_TEST_CHECK(blurEffect1);
  DALI_TEST_CHECK(blurEffect2);
  DALI_TEST_CHECK(blurEffect3);

  END_TEST;
}

int UtcDaliRenderEffectActivateP01(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectActivateP01");

  View view = CreateSizedView(application, 1.0f, 1.0f);

  View childView = View::New();
  childView.SetRequestedWidth(1.0f);
  childView.SetRequestedHeight(1.0f);
  view.Add(childView);

  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  uint32_t baseTaskCount = GetRenderTaskCount(application);

  childView.SetRenderEffect(BackgroundBlurEffect::New());
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  View maskView = View::New();
  childView.SetRenderEffect(MaskEffect::New(maskView));
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectActivateP02(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectActivateP02");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  RenderEffect blurEffect = BackgroundBlurEffect::New();
  view.SetRenderEffect(blurEffect);

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(true, blurEffect.IsActivated(), TEST_LOCATION);

  // The same effect handle moves to another view. It must not leave the first view's tasks behind.
  View view2 = CreateSizedView(application, 1.0f, 1.0f);
  view2.SetRenderEffect(blurEffect);

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectActivateP03(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectActivateP03");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  View         maskView   = View::New();
  RenderEffect maskEffect = MaskEffect::New(maskView);
  view.SetRenderEffect(maskEffect);

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

  View view2 = CreateSizedView(application, 1.0f, 1.0f);
  view2.SetRenderEffect(maskEffect);

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectDeactivateP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectDeactivateP");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  uint32_t             count      = view.GetRendererCount();
  BackgroundBlurEffect blurEffect = BackgroundBlurEffect::New();
  view.SetRenderEffect(blurEffect);

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, blurEffect.IsActivated(), TEST_LOCATION);

  view.ClearRenderEffect();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, blurEffect.IsActivated(), TEST_LOCATION);

  View maskView = View::New();
  count         = view.GetRendererCount();
  view.SetRenderEffect(MaskEffect::New(maskView));

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

  view.ClearRenderEffect();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectDeactivateN(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectDeactivateN");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  view.ClearRenderEffect(); // Nothing happens
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectActivateDeactivateInplace(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectActivateDeactivateInplace");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  RenderEffect blurEffect = BackgroundBlurEffect::New();
  view.SetRenderEffect(blurEffect);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  blurEffect.Deactivate();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

  blurEffect.Activate();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectReassign(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectReassign");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  RenderEffect blurEffect = BackgroundBlurEffect::New();
  view.SetRenderEffect(blurEffect); // Duplicate actions will be ignored
  view.SetRenderEffect(blurEffect); // Duplicate actions will be ignored
  view.SetRenderEffect(blurEffect); // Duplicate actions will be ignored

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectResize01(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectResize01");

  View view = CreateSizedView(application, 0.0f, 0.0f);

  uint32_t baseTaskCount = GetRenderTaskCount(application);
  uint32_t count         = view.GetRendererCount();

  ////////////////////////////////////////////
  {
    tet_infoline("resize test on BackgroundBlurEffect");

    BackgroundBlurEffect effect = BackgroundBlurEffect::New();
    effect.SetBlurOnce(true);
    view.SetRenderEffect(effect);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    tet_infoline("size zero owner view's effect is not activated.");

    ResizeView(application, view, 30.0f, 30.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);
    tet_infoline("Background blur effect activated.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    // The blur-once tasks are finished by now. The toolkit test could not observe the stopped
    // tasks either, so only the renderer count is asserted here.
    DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

    ResizeView(application, view, 10.0f, 10.0f);

    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 10.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);
    tet_infoline("Background blur effect refreshed.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

    ResizeView(application, view, 0.0f, 0.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);
    tet_infoline("Background blur effect deactivated.");
  }
  /////////////////////////////////////////////
  {
    tet_infoline("resize test on GaussianBlurEffect");

    GaussianBlurEffect effect = GaussianBlurEffect::New(20u);
    effect.SetBlurOnce(true);
    view.SetRenderEffect(effect);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    tet_infoline("size zero owner view's effect is not activated.");

    ResizeView(application, view, 30.0f, 30.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);
    tet_infoline("Blur effect activated.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer
    DALI_TEST_EQUALS(effect.GetBlurRadius(), 20u, TEST_LOCATION);

    ResizeView(application, view, 10.0f, 10.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 10.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);
    tet_infoline("Blur effect refreshed.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer

    ResizeView(application, view, 0.0f, 0.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);
    tet_infoline("Blur effect deactivated.");
  }
  /////////////////////////////////////////////
  {
    tet_infoline("resize test on MaskEffect");

    View sourceView = CreateSizedView(application, 200.0f, 200.0f);
    sourceView.SetBackgroundColor(UiColor(1.0f, 1.0f, 1.0f, 0.5f));

    uint32_t maskBaseTaskCount = GetRenderTaskCount(application);

    MaskEffect effect = MaskEffect::New(sourceView);
    effect.SetTargetRenderOnce(true);
    effect.SetSourceRenderOnce(true);
    view.SetRenderEffect(effect);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), maskBaseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    tet_infoline("size zero owner view's effect is not activated.");

    ResizeView(application, view, 30.0f, 30.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), maskBaseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);
    tet_infoline("Mask effect activated.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer

    ResizeView(application, view, 10.0f, 10.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), maskBaseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 10.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);
    tet_infoline("Mask effect refreshed.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer

    ResizeView(application, view, 0.0f, 0.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), maskBaseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);
    tet_infoline("Mask effect deactivated.");

    sourceView.Unparent();
  }

  END_TEST;
}

int UtcDaliRenderEffectResize02(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectResize02 - Parent size changed while the child matches the parent");

  View parent = CreateSizedView(application, 0.0f, 0.0f);

  View view = View::New();
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(MATCH_PARENT);
  parent.Add(view);

  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  uint32_t baseTaskCount = GetRenderTaskCount(application);
  uint32_t count         = view.GetRendererCount();

  ////////////////////////////////////////////
  {
    tet_infoline("resize test on BackgroundBlurEffect");

    BackgroundBlurEffect effect = BackgroundBlurEffect::New();
    effect.SetBlurOnce(true);
    view.SetRenderEffect(effect);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    tet_infoline("size zero owner view's effect is not activated.");

    ResizeView(application, parent, 30.0f, 30.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);
    tet_infoline("Background blur effect activated.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

    ResizeView(application, parent, 10.0f, 10.0f);

    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 10.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);
    tet_infoline("Background blur effect refreshed.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

    ResizeView(application, parent, 0.0f, 0.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);
    tet_infoline("Background blur effect deactivated.");
  }
  /////////////////////////////////////////////
  {
    tet_infoline("resize test on GaussianBlurEffect");

    GaussianBlurEffect effect = GaussianBlurEffect::New(20u);
    effect.SetBlurOnce(true);
    view.SetRenderEffect(effect);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    tet_infoline("size zero owner view's effect is not activated.");

    ResizeView(application, parent, 30.0f, 30.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);
    tet_infoline("Blur effect activated.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer
    DALI_TEST_EQUALS(effect.GetBlurRadius(), 20u, TEST_LOCATION);

    ResizeView(application, parent, 10.0f, 10.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 10.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);
    tet_infoline("Blur effect refreshed.");

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION); // Uses cache renderer

    ResizeView(application, parent, 0.0f, 0.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);
    tet_infoline("Blur effect deactivated.");
  }

  END_TEST;
}

int UtcDaliRenderEffectSynchronizeViewCornerRadius(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectSynchronizeViewCornerRadius");

  RenderEffect effect = BackgroundBlurEffect::New(40);

  View view = View::New();
  DALI_TEST_CHECK(view.GetRendererCount() == 0u);

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetRequestedWidth(1.0f);
  view.SetRequestedHeight(1.0f);
  view.SetProperty(View::Property::CORNER_RADIUS, Vector4(30.0f, 30.0f, 30.0f, 30.0f));
  view.SetProperty(View::Property::CORNER_RADIUS_POLICY, Ui::Visual::Transform::Policy::ABSOLUTE);
  view.SetProperty(View::Property::CORNER_SQUARENESS, Vector4(0.7f, 0.7f, 0.7f, 0.7f));
  application.GetScene().Add(view);

  view.SetBackgroundColor(UiColor(0.0f, 0.0f, 0.0f, 0.2f));
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(view.GetRendererCount() == 1u);

  view.SetRenderEffect(effect);
  DALI_TEST_CHECK(view.GetRendererCount() == 2u);

  Renderer renderer = view.GetRendererAt(1u);

  Vector4 radius = Vector4::ZERO;
  renderer.GetProperty(renderer.GetPropertyIndex(Dali::String("uCornerRadius"))).Get(radius);

  Vector4 squareness = Vector4::ZERO;
  renderer.GetProperty(renderer.GetPropertyIndex(Dali::String("uCornerSquareness"))).Get(squareness);

  Ui::Visual::Transform::Policy::Type policy;
  renderer.GetProperty(renderer.GetPropertyIndex(Dali::String("uCornerRadiusPolicy"))).Get(policy);
  DALI_TEST_CHECK(policy == Ui::Visual::Transform::Policy::ABSOLUTE);

  DALI_TEST_EQUALS(radius, Vector4(30.0f, 30.0f, 30.0f, 30.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(squareness, Vector4(0.7f, 0.7f, 0.7f, 0.7f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectInvalidTargetSize(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectInvalidTargetSize");

  const float maxTextureSize = static_cast<float>(Dali::GetMaxTextureSize());

  // The sizes below are deliberately out of contract for Arrange(), so they are written straight to
  // the actor. UpdateTargetSize() is what has to cope with them: an oversized request is scaled
  // down to the maximum texture size, and a negative one collapses to zero.
  View view = View::New();
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(view);

  SetRawViewSize(view, maxTextureSize + 1000.0f, maxTextureSize + 1000.0f);
  view.SetRenderEffect(BackgroundBlurEffect::New(40));

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(true); // no error

  SetRawViewSize(view, -10.0f, -10.0f);

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(true); // no error

  END_TEST;
}

int UtcDaliRenderEffectViewSceneOnAndSceneOff01(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectViewSceneOnAndSceneOff01");

  Dali::Integration::Scene scene = application.GetScene();

  uint32_t baseTaskCount = GetRenderTaskCount(application);

  View     view  = CreateDetachedView(1.0f, 1.0f);
  uint32_t count = view.GetRendererCount();

  // Add render effect during scene off.
  view.SetRenderEffect(BackgroundBlurEffect::New());

  // Still render effect is not activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  scene.Add(view);
  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.Unparent();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  scene.Add(view);
  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.Unparent();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  view.ClearRenderEffect();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  scene.Add(view);
  // Render effect not activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectViewSceneOnAndSceneOff02(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectViewSceneOnAndSceneOff02");

  Dali::Integration::Scene scene = application.GetScene();

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);
  uint32_t count         = view.GetRendererCount();

  // Add render effect during scene on.
  view.SetRenderEffect(BackgroundBlurEffect::New());

  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.Unparent();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  scene.Add(view);
  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.Unparent();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  view.ClearRenderEffect();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  scene.Add(view);
  // Render effect not activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectViewVisiblityChanged01(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectViewVisiblityChanged01");

  Dali::Integration::Scene scene = application.GetScene();

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);
  uint32_t count         = view.GetRendererCount();

  // Add render effect during invisible.
  view.SetProperty(Actor::Property::VISIBLE, false);
  view.SetRenderEffect(BackgroundBlurEffect::New());

  // Still render effect is not activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  // Render effect activated.
  view.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.Unparent();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  // Render effect still deactivated.
  view.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  // Render effect activated.
  scene.Add(view);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  view.ClearRenderEffect();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  view.SetProperty(Actor::Property::VISIBLE, true);
  // Render effect not activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectViewVisiblityChanged02(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectViewVisiblityChanged02");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);
  uint32_t count         = view.GetRendererCount();

  // Add render effect during scene on.
  view.SetRenderEffect(BackgroundBlurEffect::New());

  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  view.SetProperty(Actor::Property::VISIBLE, true);
  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(count + 1, view.GetRendererCount(), TEST_LOCATION);

  // Render effect deactivated.
  view.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  view.ClearRenderEffect();
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  view.SetProperty(Actor::Property::VISIBLE, true);
  // Render effect not activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(count, view.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectRenderTaskOrdering(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectRenderTaskOrdering");

  Dali::Integration::Scene scene    = application.GetScene();
  RenderTaskList           taskList = scene.GetRenderTaskList();

  uint32_t baseTaskCount = GetRenderTaskCount(application);

  View view1 = CreateDetachedView(1.0f, 1.0f);

  // Add render effect during scene off.
  view1.SetRenderEffect(BackgroundBlurEffect::New());
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

  scene.Add(view1);
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  Dali::RenderTask sourceTaskView1         = taskList.GetTask(taskList.GetTaskCount() - 3);
  Dali::RenderTask horizontalBlurTaskView1 = taskList.GetTask(taskList.GetTaskCount() - 2);
  Dali::RenderTask verticalBlurTaskView1   = taskList.GetTask(taskList.GetTaskCount() - 1);

  // Unlike dali-toolkit, which leaves the order index at 0 until the next SendNotification(),
  // dali-ui assigns the offscreen order as soon as the effect is activated. Only the settled
  // values are asserted here.
  application.SendNotification();

  DALI_TEST_EQUALS(INT32_MIN, sourceTaskView1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, horizontalBlurTaskView1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, verticalBlurTaskView1.GetOrderIndex(), TEST_LOCATION);

  View view2 = CreateDetachedView(1.0f, 1.0f);

  // dali-ui does not necessarily append a newly activated effect's tasks to the end of the list, so
  // view2's tasks are identified by diffing against a snapshot rather than by their position.
  const std::vector<Dali::RenderTask> tasksBeforeView2 = SnapshotRenderTasks(taskList);

  view2.SetRenderEffect(BackgroundBlurEffect::New());
  scene.Add(view2);

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + 2u * DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  const std::vector<Dali::RenderTask> view2Tasks = RenderTasksAddedSince(taskList, tasksBeforeView2);
  DALI_TEST_EQUALS(static_cast<uint32_t>(view2Tasks.size()), DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);

  Dali::RenderTask sourceTaskView2         = view2Tasks[0];
  Dali::RenderTask horizontalBlurTaskView2 = view2Tasks[1];
  Dali::RenderTask verticalBlurTaskView2   = view2Tasks[2];

  application.SendNotification();

  // dali-ui orders the offscreen passes by the order the effects were activated in, so view1 keeps
  // the indices it already had and view2 is appended behind it. dali-toolkit does the opposite,
  // giving the most recently activated effect the front of the order.
  DALI_TEST_EQUALS(INT32_MIN, sourceTaskView1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, horizontalBlurTaskView1.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, verticalBlurTaskView1.GetOrderIndex(), TEST_LOCATION);

  DALI_TEST_EQUALS(INT32_MIN + 3, sourceTaskView2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 4, horizontalBlurTaskView2.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 5, verticalBlurTaskView2.GetOrderIndex(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectReInitialize(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectReInitialize");

  Dali::Integration::Scene scene    = application.GetScene();
  RenderTaskList           taskList = scene.GetRenderTaskList();

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  // Add render effect during scene on.
  view.SetRenderEffect(BackgroundBlurEffect::New());
  application.SendNotification();

  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);

  BackgroundBlurEffect effect = BackgroundBlurEffect::New(40u);
  view.SetRenderEffect(effect);
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 40u, TEST_LOCATION);

  application.SendNotification();

  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);

  effect.SetBlurRadius(20u);
  application.SendNotification();

  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 20u, TEST_LOCATION);

  effect.SetBlurRadius(0u); // invalid blur radius value(too small)
  application.SendNotification();

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 0u, TEST_LOCATION);

  view.SetRenderEffect(GaussianBlurEffect::New(0u)); // invalid blur radius value(too small)
  application.SendNotification();

  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(effect.GetBlurRadius(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliBlurEffectBlurOnce(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliBlurEffectBlurOnce");

  Dali::Integration::Scene scene    = application.GetScene();
  RenderTaskList           taskList = scene.GetRenderTaskList();

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  {
    BackgroundBlurEffect effect = BackgroundBlurEffect::New(20u);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.SetBlurOnce(true);
    effect.SetBlurOnce(false);
    effect.SetBlurOnce(false);
    effect.SetBlurOnce(true);
    effect.FinishedSignal().Connect(&application, &BlurRenderingFinishedCallback);
    view.SetRenderEffect(effect);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    // The blur-once pass is done, so the tasks are gone again.
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

    effect.SetBlurOnce(false);
    effect.SetBlurOnce(false);
    effect.SetBlurOnce(true);
    effect.SetBlurOnce(true);
    effect.SetBlurOnce(false);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    application.SendNotification(); // reordering render tasks

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
    DALI_TEST_EQUALS(INT32_MIN + 2, taskList.GetTask(taskList.GetTaskCount() - 1).GetOrderIndex(), TEST_LOCATION);
  }
  {
    ResizeView(application, view, 100.0f, 100.0f);
    GaussianBlurEffect effect = GaussianBlurEffect::New(20u);
    effect.SetBlurDownscaleFactor(0.25f);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.SetBlurOnce(true);
    effect.SetBlurOnce(false);
    effect.SetBlurOnce(false);
    effect.SetBlurOnce(true);
    effect.FinishedSignal().Connect(&application, &BlurRenderingFinishedCallback);
    view.SetRenderEffect(effect);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    // Only the empty source task remains as the exclusive-rendering marker.
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + 1u, TEST_LOCATION);
    DALI_TEST_CHECK(!taskList.GetTask(taskList.GetTaskCount() - 1u).GetFrameBuffer());

    effect.Refresh();
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT + 1u, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + 1u, TEST_LOCATION);
    DALI_TEST_CHECK(!taskList.GetTask(taskList.GetTaskCount() - 1u).GetFrameBuffer());

    effect.SetBlurOnce(false);
    effect.SetBlurOnce(false);
    effect.SetBlurOnce(true);
    effect.SetBlurOnce(true);
    effect.SetBlurOnce(false);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT + 1u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliBlurEffectDownscaleFactor(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliBlurEffectDownscaleFactor");

  Dali::Integration::Scene scene = application.GetScene();

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  {
    tet_printf("test BackgroundBlurEffect\n");

    BackgroundBlurEffect effect = BackgroundBlurEffect::New(200);
    view.SetRenderEffect(effect);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.25f, TEST_LOCATION); // Default

    effect.Deactivate();
    effect.SetBlurDownscaleFactor(0.16f); // update while deactivated
    effect.Activate();
    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.25f, TEST_LOCATION);
    DALI_TEST_EQUALS(effect.IsActivated(), true, TEST_LOCATION);

    effect.SetBlurDownscaleFactor(0.5f); // update while activated
    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.5f, TEST_LOCATION);

    effect.Refresh();
    DALI_TEST_EQUALS(effect.IsActivated(), true, TEST_LOCATION);

    effect.Deactivate();
    effect.Refresh();
    DALI_TEST_EQUALS(effect.IsActivated(), true, TEST_LOCATION);

    effect.Deactivate();
  }
  {
    tet_printf("test GaussianBlurEffect\n");

    GaussianBlurEffect effect = GaussianBlurEffect::New(200);
    view.SetRenderEffect(effect);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.25f, TEST_LOCATION); // Default

    effect.Deactivate();
    effect.SetBlurDownscaleFactor(0.16f); // update while deactivated
    effect.Activate();
    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.25f, TEST_LOCATION);

    effect.SetBlurDownscaleFactor(0.5f); // update while activated
    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.5f, TEST_LOCATION);

    effect.SetBlurRadius(30);
    effect.Deactivate();
  }

  ResizeView(application, view, 100.0f, 100.0f);

  {
    tet_printf("test BackgroundBlurEffect intermediate downsample task\n");

    BackgroundBlurEffect effect = BackgroundBlurEffect::New(40u);
    effect.SetBlurDownscaleFactor(0.25f);
    view.SetRenderEffect(effect);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(effect.GetBlurDownscaleFactor(), 0.25f, TEST_LOCATION);
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);

    RenderTaskList taskList = scene.GetRenderTaskList();
    uint32_t       first    = taskList.GetTaskCount() - INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT;
    CheckTaskFrameBufferSize(taskList.GetTask(first + 0u), 50u, 50u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 1u), 25u, 25u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 2u), 25u, 25u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 3u), 25u, 25u);
  }

  END_TEST;
}

int UtcDaliMaskEffect(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliMaskEffect");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  View     maskView      = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  // Add render effect during scene on.
  view.SetRenderEffect(MaskEffect::New(maskView));

  application.SendNotification();
  application.Render();

  ResizeView(application, view, 3.0f, 3.0f);

  // send notification twice to refresh.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaskEffectScaleN(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliMaskEffectScaleN");

  View     view          = CreateSizedView(application, 1.0f, 1.0f);
  View     maskView      = CreateSizedView(application, 1.0f, 1.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);

  // A zero scale must be adjusted to epsilon internally rather than breaking the effect.
  view.SetRenderEffect(MaskEffect::New(maskView, MaskEffect::MaskMode::ALPHA, Vector2(0.f, 0.f), Vector2(0.f, 0.f)));

  application.SendNotification();
  application.Render();

  // Render effect activated.
  DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + MASK_RENDER_TASK_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRenderEffectBlurStrengthAnimation(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectBlurStrengthAnimation");

  View     view          = CreateSizedView(application, 100.0f, 100.0f);
  uint32_t baseTaskCount = GetRenderTaskCount(application);
  auto     taskList      = application.GetScene().GetRenderTaskList();

  {
    BackgroundBlurEffect effect = BackgroundBlurEffect::New();
    view.SetRenderEffect(effect);

    float     durationSeconds = 0.05f;
    Animation animation       = Animation::New(durationSeconds);

    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
    uint32_t first = taskList.GetTaskCount() - DEFAULT_BLUR_RENDER_TASK_COUNT;
    CheckTaskFrameBufferSize(taskList.GetTask(first + 0u), 100u, 100u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 1u), 100u, 100u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 2u), 100u, 100u);

    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u /*just beyond the animation duration*/);
    application.SendNotification();
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    // Strength 1 restores the configured 0.25 downscale after the animation.
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    first = taskList.GetTaskCount() - INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT;
    CheckTaskFrameBufferSize(taskList.GetTask(first + 0u), 50u, 50u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 1u), 25u, 25u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 2u), 25u, 25u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 3u), 25u, 25u);

    Animation clearAnimation = Animation::New(durationSeconds);
    effect.AddBlurStrengthAnimation(clearAnimation, AlphaFunction::BuiltinFunction::EASE_IN,
                                    TimePeriod(0, durationSeconds), 1.0f, 0.0f);
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
    clearAnimation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u);
    application.SendNotification();
    clearAnimation.Clear();

    // Strength 0 is the original image, so the whole background-blur path is bypassed.
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    effect.Refresh();
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

    Animation restoreAnimation = Animation::New(durationSeconds);
    effect.AddBlurStrengthAnimation(restoreAnimation, AlphaFunction::BuiltinFunction::EASE_IN,
                                    TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
    restoreAnimation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u);
    application.SendNotification();
    restoreAnimation.Clear();
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    effect.SetBlurRadius(0u);
    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
  }
  {
    GaussianBlurEffect effect = GaussianBlurEffect::New();
    view.SetRenderEffect(effect);

    float     durationSeconds = 0.05f;
    Animation animation       = Animation::New(durationSeconds);

    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);

    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
    uint32_t first = taskList.GetTaskCount() - DEFAULT_BLUR_RENDER_TASK_COUNT;
    CheckTaskFrameBufferSize(taskList.GetTask(first + 0u), 100u, 100u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 1u), 100u, 100u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 2u), 100u, 100u);

    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u /*just beyond the animation duration*/);
    application.SendNotification();
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    // Strength 1 restores the configured 0.25 downscale after the animation.
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);
    first = taskList.GetTaskCount() - INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT;
    CheckTaskFrameBufferSize(taskList.GetTask(first + 0u), 50u, 50u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 1u), 25u, 25u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 2u), 25u, 25u);
    CheckTaskFrameBufferSize(taskList.GetTask(first + 3u), 25u, 25u);

    Animation clearAnimation = Animation::New(durationSeconds);
    effect.AddBlurStrengthAnimation(clearAnimation, AlphaFunction::BuiltinFunction::EASE_IN,
                                    TimePeriod(0, durationSeconds), 1.0f, 0.0f);
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
    clearAnimation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u);
    application.SendNotification();
    clearAnimation.Clear();

    // Strength 0 is the original image, so the whole gaussian-blur path is bypassed.
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);
    effect.Refresh();
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount, TEST_LOCATION);

    Animation restoreAnimation = Animation::New(durationSeconds);
    effect.AddBlurStrengthAnimation(restoreAnimation, AlphaFunction::BuiltinFunction::EASE_IN,
                                    TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + DEFAULT_BLUR_RENDER_TASK_COUNT, TEST_LOCATION);
    restoreAnimation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u);
    application.SendNotification();
    restoreAnimation.Clear();
    DALI_TEST_EQUALS(GetRenderTaskCount(application), baseTaskCount + INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    effect.SetBlurRadius(0u);
    effect.AddBlurStrengthAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
  }

  END_TEST;
}

int UtcDaliRenderEffectBlurOpacityAnimation(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectBlurOpacityAnimation");

  View view = CreateSizedView(application, 1.0f, 1.0f);

  {
    BackgroundBlurEffect effect = BackgroundBlurEffect::New();
    view.SetRenderEffect(effect);

    float     durationSeconds = 0.05f;
    Animation animation       = Animation::New(durationSeconds);

    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u /*just beyond the animation duration*/);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    effect.SetBlurRadius(0u);
    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
  }
  {
    GaussianBlurEffect effect = GaussianBlurEffect::New();
    view.SetRenderEffect(effect);

    float     durationSeconds = 0.05f;
    Animation animation       = Animation::New(durationSeconds);

    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 1u /*just beyond the animation duration*/);
    DALI_TEST_EQUALS(effect.GetBlurOnce(), false, TEST_LOCATION);

    effect.SetBlurOnce(true);
    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
    DALI_TEST_EQUALS(effect.GetBlurOnce(), true, TEST_LOCATION);

    effect.SetBlurRadius(0u);
    effect.AddBlurOpacityAnimation(animation, AlphaFunction::BuiltinFunction::EASE_IN, TimePeriod(0, durationSeconds), 0.0f, 1.0f);
    // animation will not be added but cannot check
    animation.Clear();
  }

  END_TEST;
}
