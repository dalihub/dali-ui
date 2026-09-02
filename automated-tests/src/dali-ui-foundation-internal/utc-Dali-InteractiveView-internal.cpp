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
#include <dali/integration-api/events/touch-event-integ.h>

// INTERNAL INCLUDES
#define private public
#include <dali-ui-foundation/internal/state-effects/overlay-effect-impl.h>
#undef private

using namespace Dali;
using namespace Dali::Ui;

namespace
{

InteractiveView CreateTestInteractiveView(TestApplication& application, float width = 100.0f, float height = 100.0f)
{
  InteractiveView view = InteractiveView::New();
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}

void ProcessTouch(UiTestApplication& application, PointState::Type state, uint32_t time = 100u)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(Vector2(50.0f, 50.0f));
  point.SetDeviceId(1);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  application.ProcessEvent(touchEvent);
}

constexpr float OVERLAY_RECOIL_SCALE_FACTOR = 0.96f;

void FinishRecoilAnimation(UiTestApplication& application)
{
  application.SendNotification();
  application.Render(0);
  application.Render(150);
}

} // namespace

void utc_dali_interactiveview_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_interactiveview_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInteractiveViewOverlayEffectRecoilRestoreReleaseWaitsForFinishP(void)
{
  UiTestApplication application;
  InteractiveView   view   = CreateTestInteractiveView(application);
  OverlayEffect     effect = OverlayEffect::Plain();
  view.SetStateEffect(effect);
  view.SetScale(Vector3(1.2f, 0.8f, 1.0f));

  ProcessTouch(application, PointState::DOWN);
  FinishRecoilAnimation(application);

  Vector3 viewScale = view.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_EQUALS(viewScale.x, 1.2f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(viewScale.y, 0.8f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);

  GetImpl(effect).ReleaseOverlayEffectData(view, Dali::Ui::Internal::RecoilRestoreMode::ANIMATE);
  application.SendNotification();
  application.Render(0);

  viewScale = view.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_EQUALS(viewScale.x, 1.2f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(viewScale.y, 0.8f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);

  FinishRecoilAnimation(application);
  application.SendNotification();

  viewScale = view.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  DALI_TEST_EQUALS(viewScale.x, 1.2f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(viewScale.y, 0.8f, 0.001f, TEST_LOCATION);
  END_TEST;
}
