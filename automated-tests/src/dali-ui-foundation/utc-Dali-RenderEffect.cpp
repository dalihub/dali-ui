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
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t DEFAULT_BLUR_RENDER_TASK_COUNT              = 3u;
constexpr uint32_t INTERMEDIATE_DOWNSAMPLE_RENDER_TASK_COUNT  = 4u;

uint32_t GetRenderTaskCount(UiTestApplication& application)
{
  return application.GetScene().GetRenderTaskList().GetTaskCount();
}

View CreateView(UiTestApplication& application)
{
  View view = View::New();
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE_WIDTH, 400.0f);
  view.SetProperty(Actor::Property::SIZE_HEIGHT, 400.0f);
  application.GetScene().Add(view);
  view.Arrange(LayoutRect(0.0f, 0.0f, 400.0f, 400.0f));
  application.SendNotification();
  application.Render();
  application.RunIdles();
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

  View     view         = CreateView(application);
  uint32_t baseTaskCount = GetRenderTaskCount(application);
  auto     gaussianBlur = GaussianBlurEffect::New();

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
