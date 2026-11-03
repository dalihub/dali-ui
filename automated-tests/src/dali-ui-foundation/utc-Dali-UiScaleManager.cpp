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
#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::Extension::LayoutScaleManager;

// UiScaleManager is deprecated. It keeps every released symbol and forwards to the state
// LayoutScaleManager owns, so every test that uses it lives in this file: the released
// behaviour seen through the deprecated class, and the sharing between the two classes.
//
// Each case runs in a process of its own (test-harness.cpp), so the first Get() a case
// makes is the first Get() of its process; the two Get-order cases rely on that.

void utc_dali_ui_scale_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_ui_scale_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
// Adds a 50 x 50 INHERIT root to the window and runs the layout.
View AddSettledRoot(UiTestApplication& application)
{
  View root = View::New();
  root.SetRequestedWidth(50.0f);
  root.SetRequestedHeight(50.0f);
  application.GetWindow().Add(root);
  application.SendNotification();
  application.SendNotification();
  return root;
}
} // namespace

// === Handle ===

int UtcDaliUiScaleManagerConstructorP(void)
{
  UiTestApplication application;

  UiScaleManager manager;
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

int UtcDaliUiScaleManagerGetP(void)
{
  UiTestApplication application;

  UiScaleManager manager1 = UiScaleManager::Get();
  UiScaleManager manager2 = UiScaleManager::Get();

  DALI_TEST_CHECK(manager1);
  DALI_TEST_CHECK(manager1 == manager2);

  END_TEST;
}

int UtcDaliUiScaleManagerDownCastP(void)
{
  UiTestApplication application;

  BaseHandle     handle(UiScaleManager::Get());
  UiScaleManager manager = UiScaleManager::DownCast(handle);

  DALI_TEST_CHECK(manager);
  DALI_TEST_CHECK(manager == UiScaleManager::Get());

  END_TEST;
}

int UtcDaliUiScaleManagerDownCastN(void)
{
  UiTestApplication application;

  BaseHandle handle;
  DALI_TEST_CHECK(!UiScaleManager::DownCast(handle));

  END_TEST;
}

// The two classes share their state but not their handles: each handle refers to an object
// of its own class, so neither DownCast() accepts the other.
int UtcDaliUiScaleManagerHandlesAreNotInterchangeableN(void)
{
  UiTestApplication application;

  DALI_TEST_CHECK(!UiScaleManager::DownCast(LayoutScaleManager::Get()));
  DALI_TEST_CHECK(!LayoutScaleManager::DownCast(UiScaleManager::Get()));

  END_TEST;
}

// === Shared state ===

int UtcDaliUiScaleManagerSetScaleSharedWithLayoutScaleManagerP(void)
{
  UiTestApplication application;

  UiScaleManager     legacy        = UiScaleManager::Get();
  LayoutScaleManager manager       = LayoutScaleManager::Get();
  const float        originalScale = manager.GetUiScale();

  legacy.SetScale(1.25f);
  DALI_TEST_EQUALS(manager.GetUiScale(), 1.25f, TEST_LOCATION);
  DALI_TEST_EQUALS(legacy.GetScale(), 1.25f, TEST_LOCATION);

  manager.SetUiScale(1.75f);
  DALI_TEST_EQUALS(legacy.GetScale(), 1.75f, TEST_LOCATION);

  manager.SetUiScale(originalScale);
  END_TEST;
}

int UtcDaliUiScaleManagerSetScalableSharedWithLayoutScaleManagerP(void)
{
  UiTestApplication application;

  UiScaleManager     legacy           = UiScaleManager::Get();
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const bool         originalScalable = manager.IsScalable();

  legacy.SetScalable(false);
  DALI_TEST_CHECK(!manager.IsScalable());
  DALI_TEST_CHECK(!legacy.IsScalable());

  manager.SetScalable(true);
  DALI_TEST_CHECK(legacy.IsScalable());

  manager.SetScalable(originalScalable);
  END_TEST;
}

// The deprecated-path copy of UtcDaliViewUnscaledWhenNotScalableP (utc-Dali-View.cpp): the
// scale and the master switch set through UiScaleManager move the laid-out geometry.
int UtcDaliUiScaleManagerSetScalableUnscalesGeometryP(void)
{
  UiTestApplication application;

  UiScaleManager legacy           = UiScaleManager::Get();
  const float    originalScale    = legacy.GetScale();
  const bool     originalScalable = legacy.IsScalable();
  legacy.SetScalable(true);
  legacy.SetScale(2.0f);

  View root = AddSettledRoot(application);

  // Scaling on, scale 2.0: 50 x 2.0 = 100.
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, TEST_LOCATION);

  // Master switch off: the tree re-arranges unscaled despite the stored 2.0.
  legacy.SetScalable(false);
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 50.0f, TEST_LOCATION);

  // Master switch back on: the preserved 2.0 is re-applied.
  legacy.SetScalable(true);
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, TEST_LOCATION);

  legacy.SetScale(originalScale);
  legacy.SetScalable(originalScalable);
  END_TEST;
}

// The documented contract is a positive, finite, NORMAL scale. A subnormal scale is not
// a usable display scale and would make the layout's reciprocal overflow, so it is
// rejected together with the non-finite values rather than clamped.
int UtcDaliUiScaleManagerSetScaleSubnormalRejectedN(void)
{
  UiTestApplication application;
  tet_infoline("A subnormal or non-finite UI scale is rejected, leaving the previous scale in place");

  const float originalScale = UiScaleManager::Get().GetScale();

  UiScaleManager::Get().SetScale(1.0f);
  DALI_TEST_EQUALS(UiScaleManager::Get().GetScale(), 1.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(std::numeric_limits<float>::denorm_min());
  DALI_TEST_EQUALS(UiScaleManager::Get().GetScale(), 1.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(std::numeric_limits<float>::quiet_NaN());
  DALI_TEST_EQUALS(UiScaleManager::Get().GetScale(), 1.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// === Layout-root registry ===

namespace
{
int gRegistryMeasureCount = 0;

MeasuredSize CountRegistryMeasure(View, float, float)
{
  ++gRegistryMeasureCount;
  return MeasuredSize(50.0f, 50.0f);
}
} // namespace

// A root registered through the deprecated RegisterLayoutRoot() enters the registry that a
// LayoutScaleManager scale change walks. The root is detached first, so the library has
// already unregistered it and only the deprecated call can register it again. A detached
// view caches no effective scale -- it follows a scale change by itself -- so the walk is
// observed through what it does besides: it drops the root's layout caches. The root opts
// out of the UI scale (UiScalePolicy::DISABLED), so its effective scale stays 1.0 and its
// measure entry's scale never moves: only that walk can make the next same-constraint
// Measure() run the producer again. The control step shows the entry surviving a scale
// change while the root is unregistered; the last step shows the deprecated registration
// made it reachable, while the root is still detached.
int UtcDaliUiScaleManagerRegisterLayoutRootReachesDetachedRootP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);

  View root = AddSettledRoot(application);
  application.GetWindow().Remove(root);
  root.SetUiScalePolicy(UiScalePolicy::DISABLED);
  root.SetMeasureCallback(MeasureCallback::New(&CountRegistryMeasure));
  gRegistryMeasureCount = 0;

  // An entry measured off scene, and the hit that shows it is live.
  root.Measure(200.0f, 200.0f);
  root.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gRegistryMeasureCount, 1, TEST_LOCATION);

  // Control: detached and unregistered, no walk reaches the root, and its entry survives.
  manager.SetUiScale(1.5f);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.0f, TEST_LOCATION);
  root.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gRegistryMeasureCount, 1, TEST_LOCATION);

  // Registered through the deprecated API, the detached root is reached by the next change.
  UiScaleManager::Get().RegisterLayoutRoot(root);
  manager.SetUiScale(2.0f);
  root.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gRegistryMeasureCount, 2, TEST_LOCATION);

  UiScaleManager::Get().UnregisterLayoutRoot(root);
  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// The mirror: a root unregistered through the deprecated UnregisterLayoutRoot() leaves that
// registry, so its cached effective scale stays stale across a scale change. Nothing
// invalidates the root between the deprecated call and the scale change.
int UtcDaliUiScaleManagerUnregisterLayoutRootDetachesRootP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);

  View root = AddSettledRoot(application);

  // Registered by the library on connection: a scale change reaches it.
  manager.SetUiScale(1.5f);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.5f, TEST_LOCATION);

  // Unregistered through the deprecated API: the next scale change does not.
  UiScaleManager::Get().UnregisterLayoutRoot(root);
  manager.SetUiScale(2.0f);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.5f, TEST_LOCATION);

  UiScaleManager::Get().RegisterLayoutRoot(root);
  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// Both deprecated registration calls accept an empty View and return without effect: the
// facade forwards them to the empty-handle guards of LayoutScaleManagerImpl. This is a
// no-crash and coverage case for those two forwards; the read-back after SetUiScale() only
// shows that the shared state is still usable and does not discriminate on its own.
int UtcDaliUiScaleManagerRegisterEmptyLayoutRootIgnoredN(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager       = LayoutScaleManager::Get();
  const float        originalScale = manager.GetUiScale();

  UiScaleManager::Get().RegisterLayoutRoot(View());
  UiScaleManager::Get().UnregisterLayoutRoot(View());

  manager.SetUiScale(1.5f);
  DALI_TEST_EQUALS(manager.GetUiScale(), 1.5f, TEST_LOCATION);

  manager.SetUiScale(originalScale);
  END_TEST;
}

// === Get() order and lifetime ===

// The deprecated class first: this process's first Get() is the deprecated one, made before
// the test application exists, so its object creates the shared state. The process exits
// after the case, so the two singletons are also destroyed in this order's reverse.
int UtcDaliUiScaleManagerGetBeforeLayoutScaleManagerP(void)
{
  UiScaleManager legacy = UiScaleManager::Get();

  UiTestApplication  application;
  LayoutScaleManager manager = LayoutScaleManager::Get();
  DALI_TEST_CHECK(legacy);
  DALI_TEST_CHECK(manager);

  legacy.SetScale(1.25f);
  DALI_TEST_EQUALS(manager.GetUiScale(), 1.25f, TEST_LOCATION);
  manager.SetUiScale(1.75f);
  DALI_TEST_EQUALS(legacy.GetScale(), 1.75f, TEST_LOCATION);

  legacy.SetScalable(false);
  DALI_TEST_CHECK(!manager.IsScalable());
  manager.SetScalable(true);
  DALI_TEST_CHECK(legacy.IsScalable());

  manager.SetUiScale(1.0f);
  END_TEST;
}

// The new class first, then the deprecated one.
int UtcDaliUiScaleManagerGetAfterLayoutScaleManagerP(void)
{
  LayoutScaleManager manager = LayoutScaleManager::Get();

  UiTestApplication application;
  UiScaleManager    legacy = UiScaleManager::Get();
  DALI_TEST_CHECK(manager);
  DALI_TEST_CHECK(legacy);

  legacy.SetScale(1.25f);
  DALI_TEST_EQUALS(manager.GetUiScale(), 1.25f, TEST_LOCATION);
  manager.SetUiScale(1.75f);
  DALI_TEST_EQUALS(legacy.GetScale(), 1.75f, TEST_LOCATION);

  legacy.SetScalable(false);
  DALI_TEST_CHECK(!manager.IsScalable());
  manager.SetScalable(true);
  DALI_TEST_CHECK(legacy.IsScalable());

  manager.SetUiScale(1.0f);
  END_TEST;
}

// The deprecated object holds exactly one counted reference to the object that owns the
// state, taken when it is created, so a held UiScaleManager handle keeps that state alive
// whatever order the two singletons are destroyed in at exit.
int UtcDaliUiScaleManagerKeepsLayoutScaleManagerAliveP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager = LayoutScaleManager::Get();
  const uint32_t     before  = manager.GetBaseObject().ReferenceCount();

  UiScaleManager legacy = UiScaleManager::Get();
  DALI_TEST_EQUALS(manager.GetBaseObject().ReferenceCount(), before + 1u, TEST_LOCATION);

  // More handles to the deprecated object add no reference to the shared one.
  UiScaleManager again = UiScaleManager::Get();
  DALI_TEST_CHECK(again == legacy);
  DALI_TEST_EQUALS(manager.GetBaseObject().ReferenceCount(), before + 1u, TEST_LOCATION);

  END_TEST;
}
