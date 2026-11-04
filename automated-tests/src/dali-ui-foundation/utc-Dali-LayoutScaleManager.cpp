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


#include <stdlib.h>
#include <iostream>
#include <limits>

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/configuration/layout-scale-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

// LayoutScaleManager keeps its state in a process-wide singleton, so every case below
// records the state it moves and restores it before returning.

void utc_dali_layout_scale_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_layout_scale_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
int gMeasureCount = 0;

MeasuredSize CountMeasure(View, float, float)
{
  ++gMeasureCount;
  return MeasuredSize(50.0f, 50.0f);
}
} // namespace

// === Handle ===

int UtcDaliLayoutScaleManagerConstructorP(void)
{
  UiTestApplication application;

  LayoutScaleManager manager;
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

int UtcDaliLayoutScaleManagerGetP(void)
{
  UiTestApplication application;

  LayoutScaleManager manager = LayoutScaleManager::Get();
  DALI_TEST_CHECK(manager);

  END_TEST;
}

int UtcDaliLayoutScaleManagerGetSingletonP(void)
{
  UiTestApplication application;

  LayoutScaleManager manager1 = LayoutScaleManager::Get();
  LayoutScaleManager manager2 = LayoutScaleManager::Get();

  DALI_TEST_CHECK(manager1);
  DALI_TEST_CHECK(manager2);
  DALI_TEST_CHECK(manager1 == manager2);

  END_TEST;
}

int UtcDaliLayoutScaleManagerCopyConstructorP(void)
{
  UiTestApplication application;

  LayoutScaleManager manager = LayoutScaleManager::Get();
  LayoutScaleManager copy(manager);

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(manager == copy);

  END_TEST;
}

int UtcDaliLayoutScaleManagerMoveConstructorP(void)
{
  UiTestApplication application;

  LayoutScaleManager manager = LayoutScaleManager::Get();
  LayoutScaleManager moved(std::move(manager));

  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!manager);
  DALI_TEST_CHECK(moved == LayoutScaleManager::Get());

  END_TEST;
}

int UtcDaliLayoutScaleManagerCopyAssignmentP(void)
{
  UiTestApplication application;

  LayoutScaleManager manager = LayoutScaleManager::Get();
  LayoutScaleManager copy;
  copy = manager;

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(manager == copy);

  END_TEST;
}

int UtcDaliLayoutScaleManagerMoveAssignmentP(void)
{
  UiTestApplication application;

  LayoutScaleManager manager = LayoutScaleManager::Get();
  LayoutScaleManager moved;
  moved = std::move(manager);

  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!manager);
  DALI_TEST_CHECK(moved == LayoutScaleManager::Get());

  END_TEST;
}

int UtcDaliLayoutScaleManagerDownCastP(void)
{
  UiTestApplication application;

  BaseHandle         handle(LayoutScaleManager::Get());
  LayoutScaleManager manager = LayoutScaleManager::DownCast(handle);

  DALI_TEST_CHECK(manager);
  DALI_TEST_CHECK(manager == LayoutScaleManager::Get());

  END_TEST;
}

int UtcDaliLayoutScaleManagerDownCastN(void)
{
  UiTestApplication application;

  BaseHandle handle;
  DALI_TEST_CHECK(!LayoutScaleManager::DownCast(handle));
  DALI_TEST_CHECK(!LayoutScaleManager::DownCast(View::New()));

  END_TEST;
}

// === UI scale ===

int UtcDaliLayoutScaleManagerDefaultsP(void)
{
  UiTestApplication application;

  LayoutScaleManager manager = LayoutScaleManager::Get();
  DALI_TEST_EQUALS(manager.GetUiScale(), 1.0f, TEST_LOCATION);
  DALI_TEST_CHECK(manager.IsScalable());

  END_TEST;
}

int UtcDaliLayoutScaleManagerSetUiScaleP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager       = LayoutScaleManager::Get();
  const float        originalScale = manager.GetUiScale();

  manager.SetUiScale(1.5f);
  DALI_TEST_EQUALS(manager.GetUiScale(), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetUiScale(), 1.5f, TEST_LOCATION);

  manager.SetUiScale(originalScale);
  END_TEST;
}

// The contract is a positive, finite and NORMAL UI scale: the layout divides by it, and a
// subnormal value makes that reciprocal overflow. Every rejected value leaves the current
// UI scale in place.
int UtcDaliLayoutScaleManagerSetUiScaleInvalidValuesIgnoredN(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager       = LayoutScaleManager::Get();
  const float        originalScale = manager.GetUiScale();

  manager.SetUiScale(1.0f);
  DALI_TEST_EQUALS(manager.GetUiScale(), 1.0f, TEST_LOCATION);

  const float invalidScales[] = {
    std::numeric_limits<float>::denorm_min(),
    std::numeric_limits<float>::min() / 2.0f,
    std::numeric_limits<float>::quiet_NaN(),
    std::numeric_limits<float>::infinity(),
    -std::numeric_limits<float>::infinity(),
    0.0f,
    -1.5f};

  for(float invalid : invalidScales)
  {
    manager.SetUiScale(invalid);
    DALI_TEST_EQUALS(manager.GetUiScale(), 1.0f, TEST_LOCATION);
  }

  manager.SetUiScale(originalScale);
  END_TEST;
}

// Setting the current UI scale again invalidates nothing: the cycle after it measures
// exactly what an idle control cycle measures (nothing on a settled tree). A new value
// measures more (the anti-vacuity check).
int UtcDaliLayoutScaleManagerSetUiScaleSameValueIsNoOpP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  manager.SetScalable(true);
  manager.SetUiScale(2.0f);

  gMeasureCount = 0;
  View root     = View::New();
  root.SetMeasureCallback(MeasureCallback::New(&CountMeasure));
  application.GetWindow().Add(root);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(gMeasureCount > 0);

  // Control: what one idle cycle measures.
  int before = gMeasureCount;
  application.SendNotification();
  application.Render();
  const int idleDelta = gMeasureCount - before;

  before = gMeasureCount;
  manager.SetUiScale(2.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(gMeasureCount - before, idleDelta, TEST_LOCATION);

  before = gMeasureCount;
  manager.SetUiScale(1.5f);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(gMeasureCount - before > idleDelta);

  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// === Switch ===

int UtcDaliLayoutScaleManagerSetScalableP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const bool         originalScalable = manager.IsScalable();

  manager.SetScalable(false);
  DALI_TEST_CHECK(!manager.IsScalable());
  manager.SetScalable(true);
  DALI_TEST_CHECK(manager.IsScalable());

  manager.SetScalable(originalScalable);
  END_TEST;
}

// A UI scale set while scaling is disabled is stored and reported, leaves the geometry
// unscaled while scaling stays disabled, and is applied when scaling is enabled again.
// Observed through geometry: a 50 x 50 root. Geometry cannot tell whether a re-layout ran
// while scaling was disabled (it would also give 50), so the deferral of that re-layout is
// pinned by the internal case UtcDaliUiScaleSetScaleWhileDisabledStoresButDefersP instead.
int UtcDaliLayoutScaleManagerSetUiScaleStoredWhileNotScalableAppliedOnEnableP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);

  View root = View::New();
  root.SetRequestedWidth(50.0f);
  root.SetRequestedHeight(50.0f);
  application.GetWindow().Add(root);
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  manager.SetScalable(false);
  manager.SetUiScale(2.0f);
  DALI_TEST_EQUALS(manager.GetUiScale(), 2.0f, TEST_LOCATION);
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  manager.SetScalable(true);
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 100.0f, TEST_LOCATION);

  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// === Effective layout scale ===

// ViewImpl::GetEffectiveScale() is deprecated in favour of GetEffectiveLayoutScale() and
// forwards to it, so the two report the same value for every view: a scaled root, a child
// that opts out of the UI scale, and the root again after the UI scale changes.
int UtcDaliLayoutScaleManagerDeprecatedGetEffectiveScaleMatchesP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  manager.SetScalable(true);
  manager.SetUiScale(1.5f);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  View unscaled = View::New();
  unscaled.SetUiScalePolicy(UiScalePolicy::DISABLED);
  unscaled.SetRequestedWidth(50.0f);
  unscaled.SetRequestedHeight(50.0f);
  root.Add(unscaled);
  application.GetWindow().Add(root);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveScale(), GetImpl(root).GetEffectiveLayoutScale(), TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(unscaled).GetEffectiveLayoutScale(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(unscaled).GetEffectiveScale(), GetImpl(unscaled).GetEffectiveLayoutScale(), TEST_LOCATION);

  manager.SetUiScale(2.0f);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveScale(), GetImpl(root).GetEffectiveLayoutScale(), TEST_LOCATION);

  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}
