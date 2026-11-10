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
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::Extension::LayoutScaleManager;

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

// ViewImpl::GetEffectiveScale() is deprecated in favour of Extension::View::GetEffectiveLayoutScale(),
// and the two report the same value for every view: a scaled root, a child that opts out of
// the UI scale, and the root again after the UI scale changes.
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

  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveScale(), Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(unscaled)), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(unscaled).GetEffectiveScale(), Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(unscaled)), TEST_LOCATION);

  manager.SetUiScale(2.0f);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveScale(), Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), TEST_LOCATION);

  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// === Window scale and scene membership ===
//
// The window term of a view's effective scale exists only while the view is in a Window (a
// view in no Window resolves it to 1.0). A view that leaves its window -- removed from it,
// under a plain Actor that is removed from it, or still in it when the window is destroyed --
// recomputes without that term, together with its whole subtree; adding it to a window
// applies that window's scale again. A view removed from a View parent was already reset by
// the parent's child-removal path, and is kept here as a regression guard.
//
// Every case sets the UI scale to 1.0 unless it says otherwise, so 1.0 is exactly "no window
// term", and resets every window scale it set on a live window before returning.

namespace
{
// Runs one full layout batch to completion.
void SettleWindowScaleLayout(UiTestApplication& application)
{
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
}
} // namespace

// A root removed from a window-scaled window, its descendants, a Measure() on the detached
// root -- the measure cache keys on the scale, so a stale scale would be served from the
// cache as well as computed with -- and the round trip back into the window.
int UtcDaliLayoutScaleManagerWindowScaleDroppedWhenRootLeavesWindowP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  Window             window           = application.GetWindow();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);
  manager.SetWindowScale(window, 2.0f);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(50.0f);
  View child = View::New();
  child.SetRequestedWidth(40.0f);
  child.SetRequestedHeight(20.0f);
  View grandChild = View::New();
  grandChild.SetRequestedWidth(10.0f);
  grandChild.SetRequestedHeight(5.0f);
  child.Add(grandChild);
  root.Add(child);

  window.Add(root);
  SettleWindowScaleLayout(application);

  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(grandChild)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetMeasuredSize().width, 200.0f, TEST_LOCATION);
  MeasuredSize measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 100.0f, TEST_LOCATION);

  window.Remove(root);
  DALI_TEST_CHECK(!root.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  // Detached: no window term, on the whole subtree, and the natural size.
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(grandChild)), 1.0f, TEST_LOCATION);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetMeasuredSize().width, 100.0f, TEST_LOCATION);
  measured = grandChild.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 5.0f, TEST_LOCATION);

  // Back in the window: its scale applies again, and the next pass measures at it.
  window.Add(root);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(grandChild)), 2.0f, TEST_LOCATION);
  SettleWindowScaleLayout(application);
  DALI_TEST_EQUALS(root.GetMeasuredSize().width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, TEST_LOCATION);

  manager.SetWindowScale(window, 1.0f);
  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// UiScalePolicy::DISABLED opts a root out of the UI scale, not out of the window scale: in the
// window it is laid out at the window scale alone, and once detached it has neither term. The
// UI scale is 1.5 here, so a UI term leaking in would read 1.5 rather than 1.0.
int UtcDaliLayoutScaleManagerWindowScaleDroppedWhenDisabledRootLeavesWindowP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  Window             window           = application.GetWindow();
  manager.SetScalable(true);
  manager.SetUiScale(1.5f);
  manager.SetWindowScale(window, 2.0f);

  View panel = View::New();
  panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
  panel.SetRequestedWidth(100.0f);
  panel.SetRequestedHeight(50.0f);
  View icon = View::New(); // INHERIT under the DISABLED root
  icon.SetRequestedWidth(20.0f);
  icon.SetRequestedHeight(10.0f);
  panel.Add(icon);

  window.Add(panel);
  SettleWindowScaleLayout(application);

  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(panel)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(icon)), 2.0f, TEST_LOCATION);
  MeasuredSize measured = panel.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 200.0f, TEST_LOCATION);

  window.Remove(panel);

  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(panel)), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(icon)), 1.0f, TEST_LOCATION);
  measured = panel.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 50.0f, TEST_LOCATION);

  manager.SetWindowScale(window, 1.0f);
  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// A detached view drops the window term only. The UI scale is not per window, so an INHERIT root
// and its subtree keep it off-scene: a detached view reads 1.0 only when its UI term is 1.0 (the
// UI scale is 1.0, scaling is switched off, or its chain resolves the UI term as DISABLED).
int UtcDaliLayoutScaleManagerWindowScaleDroppedUiScaleKeptWhenRootLeavesWindowP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  Window             window           = application.GetWindow();
  manager.SetScalable(true);
  manager.SetUiScale(1.5f);
  manager.SetWindowScale(window, 2.0f);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(50.0f);
  View child = View::New();
  child.SetRequestedWidth(40.0f);
  child.SetRequestedHeight(20.0f);
  root.Add(child);

  window.Add(root);
  SettleWindowScaleLayout(application);

  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 3.0f, TEST_LOCATION);
  MeasuredSize measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 300.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 150.0f, TEST_LOCATION);

  window.Remove(root);

  // Detached: the UI term stays on the whole subtree, the window term is gone.
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 1.5f, TEST_LOCATION);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 75.0f, TEST_LOCATION);
  measured = child.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 30.0f, TEST_LOCATION);

  // Back in the window: both terms again.
  window.Add(root);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 3.0f, TEST_LOCATION);
  SettleWindowScaleLayout(application);
  DALI_TEST_EQUALS(root.GetMeasuredSize().width, 300.0f, TEST_LOCATION);

  manager.SetWindowScale(window, 1.0f);
  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// A view removed from a View parent. The parent's child-removal reset already covered this
// before the window term existed, and it must keep doing so; the parent stays in the window
// and keeps its scale.
int UtcDaliLayoutScaleManagerWindowScaleDroppedWhenViewLeavesViewParentP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  Window             window           = application.GetWindow();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);
  manager.SetWindowScale(window, 2.0f);

  View root = View::New();
  root.SetRequestedWidth(300.0f);
  root.SetRequestedHeight(200.0f);
  View card = View::New();
  card.SetRequestedWidth(100.0f);
  card.SetRequestedHeight(50.0f);
  View label = View::New();
  label.SetRequestedWidth(20.0f);
  label.SetRequestedHeight(10.0f);
  card.Add(label);
  root.Add(card);

  window.Add(root);
  SettleWindowScaleLayout(application);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(card)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(label)), 2.0f, TEST_LOCATION);

  root.Remove(card);
  DALI_TEST_CHECK(!card.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(card)), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(label)), 1.0f, TEST_LOCATION);
  MeasuredSize measured = card.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 2.0f, TEST_LOCATION);

  manager.SetWindowScale(window, 1.0f);
  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// A view whose parent is a plain Actor is a scale root of its own: it takes the window term
// itself, and it must drop it when the Actor above it leaves the window.
int UtcDaliLayoutScaleManagerWindowScaleDroppedWhenPlainActorParentLeavesWindowP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  Window             window           = application.GetWindow();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);
  manager.SetWindowScale(window, 2.0f);

  Actor holder = Actor::New();
  View  root   = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(50.0f);
  View child = View::New();
  child.SetRequestedWidth(40.0f);
  child.SetRequestedHeight(20.0f);
  root.Add(child);
  holder.Add(root);

  window.Add(holder);
  SettleWindowScaleLayout(application);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 2.0f, TEST_LOCATION);

  window.Remove(holder);
  DALI_TEST_CHECK(!root.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 1.0f, TEST_LOCATION);
  MeasuredSize measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 50.0f, TEST_LOCATION);

  manager.SetWindowScale(window, 1.0f);
  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// A view still in a window when that window is destroyed leaves it inside the window's own
// teardown, which disconnects the window's top-level views one after the other. The first of
// them to disconnect already prunes the dead window's scale record, so when `survivor` -- added
// second on purpose -- disconnects, no window scale is recorded any more; it must still drop the
// window term it memoized. The application never resets the window scale here.
int UtcDaliLayoutScaleManagerWindowScaleDroppedWhenWindowDestroyedP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);

  View survivor = View::New();
  survivor.SetRequestedWidth(100.0f);
  survivor.SetRequestedHeight(50.0f);
  View survivorChild = View::New();
  survivorChild.SetRequestedWidth(40.0f);
  survivorChild.SetRequestedHeight(20.0f);
  survivor.Add(survivorChild);

  {
    Window extra = Window::New(PositionSize(0, 0, 480, 800), "layout-scale-manager-destroyed-window");

    View first = View::New();
    first.SetRequestedWidth(10.0f);
    first.SetRequestedHeight(10.0f);
    extra.Add(first); // disconnects before `survivor` in the teardown
    extra.Add(survivor);

    manager.SetWindowScale(extra, 2.0f);
    DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(survivor)), 2.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(survivorChild)), 2.0f, TEST_LOCATION);
  }

  // `extra` is gone. Checked first so that a window outliving its handle fails here, by name.
  DALI_TEST_CHECK(!survivor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(survivor)), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(survivorChild)), 1.0f, TEST_LOCATION);
  MeasuredSize measured = survivor.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 50.0f, TEST_LOCATION);

  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// === UI scale and scene membership ===
//
// A view that is not on a scene belongs to no registered layout root, so SetUiScale() and
// SetScalable() lay nothing out again for it. It does not need them to: off a scene the
// effective scale is computed on every read rather than cached, so GetEffectiveLayoutScale()
// and View::Measure() follow the current UI scale and switch at once -- for a view that was
// never added as for one removed from its window -- and adding the view to a window lays it
// out at the current scale, that window's term included.

// A view that was never added to a scene, and its child.
int UtcDaliLayoutScaleManagerNeverAddedViewFollowsUiScaleP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);

  View view = View::New();
  view.SetRequestedWidth(40.0f);
  view.SetRequestedHeight(20.0f);
  View child = View::New();
  child.SetRequestedWidth(10.0f);
  child.SetRequestedHeight(6.0f);
  view.Add(child);

  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(view)), 1.0f, TEST_LOCATION);
  MeasuredSize measured = view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 20.0f, TEST_LOCATION);

  manager.SetUiScale(1.5f);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(view)), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 1.5f, TEST_LOCATION);
  measured = view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 30.0f, TEST_LOCATION);
  measured = child.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 15.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 9.0f, TEST_LOCATION);

  manager.SetScalable(false);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(view)), 1.0f, TEST_LOCATION);
  measured = view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 20.0f, TEST_LOCATION);

  manager.SetScalable(true);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(view)), 1.5f, TEST_LOCATION);

  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// A root removed from a window-scaled window keeps following the UI scale and the switch
// while it is detached, and the pass after it is added back lays it out at both terms.
int UtcDaliLayoutScaleManagerDetachedViewFollowsUiScaleP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  Window             window           = application.GetWindow();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);
  manager.SetWindowScale(window, 2.0f);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(50.0f);
  View child = View::New();
  child.SetRequestedWidth(40.0f);
  child.SetRequestedHeight(20.0f);
  root.Add(child);

  window.Add(root);
  SettleWindowScaleLayout(application);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 2.0f, TEST_LOCATION);

  window.Remove(root);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 1.0f, TEST_LOCATION);
  MeasuredSize measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);

  // Detached: the UI scale reaches the whole subtree at once, values and Measure() alike.
  manager.SetUiScale(1.5f);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 1.5f, TEST_LOCATION);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 75.0f, TEST_LOCATION);
  measured = child.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 30.0f, TEST_LOCATION);

  // ...and so does the switch.
  manager.SetScalable(false);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 1.0f, TEST_LOCATION);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  manager.SetScalable(true);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 1.5f, TEST_LOCATION);

  // Back in the window: both terms, and the next pass measures at them.
  window.Add(root);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(child)), 3.0f, TEST_LOCATION);
  SettleWindowScaleLayout(application);
  DALI_TEST_EQUALS(root.GetMeasuredSize().width, 300.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 300.0f, TEST_LOCATION);

  manager.SetWindowScale(window, 1.0f);
  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}

// A view that opts out of the UI scale (UiScalePolicy::DISABLED) holding one that opts back in
// (ENABLED), under an INHERIT root, never added to a window. The DISABLED view's own effective
// scale stays 1.0 whatever the UI scale is, while its ENABLED child follows the UI scale, so its
// measured size must follow the child although its own scale -- the scale its measure entry is
// keyed on -- never moves; the root re-measures it whenever the root's own scale moves and must
// then get the fresh size. The step from 2.0 back to 1.0 leaves the DISABLED view's scale EQUAL
// to the UI term, which is why the policy, not a comparison of the two values, decides.
int UtcDaliLayoutScaleManagerOffSceneDisabledContainerFollowsEnabledChildP(void)
{
  UiTestApplication  application;
  LayoutScaleManager manager          = LayoutScaleManager::Get();
  const float        originalScale    = manager.GetUiScale();
  const bool         originalScalable = manager.IsScalable();
  manager.SetScalable(true);
  manager.SetUiScale(1.0f);

  View root  = View::New(); // INHERIT, WRAP_CONTENT
  View panel = View::New(); // WRAP_CONTENT
  panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
  View badge = View::New();
  badge.SetUiScalePolicy(UiScalePolicy::ENABLED);
  badge.SetRequestedWidth(10.0f);
  badge.SetRequestedHeight(10.0f);
  panel.Add(badge);
  root.Add(panel);

  MeasuredSize measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 10.0f, TEST_LOCATION);
  measured = panel.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 10.0f, TEST_LOCATION);

  manager.SetUiScale(2.0f);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(root)), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(panel)), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(badge)), 2.0f, TEST_LOCATION);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 20.0f, TEST_LOCATION);
  measured = panel.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 20.0f, TEST_LOCATION);

  // Back to 1.0: the DISABLED view's scale and the UI term are both 1.0 now.
  manager.SetUiScale(1.0f);
  measured = panel.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 10.0f, TEST_LOCATION);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 10.0f, TEST_LOCATION);

  manager.SetUiScale(2.0f);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 20.0f, TEST_LOCATION);

  // The switch moves the child alone as well, both ways.
  manager.SetScalable(false);
  measured = panel.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 10.0f, TEST_LOCATION);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 10.0f, TEST_LOCATION);

  manager.SetScalable(true);
  measured = panel.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 20.0f, TEST_LOCATION);
  measured = root.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(measured.width, 20.0f, TEST_LOCATION);

  manager.SetUiScale(originalScale);
  manager.SetScalable(originalScalable);
  END_TEST;
}
