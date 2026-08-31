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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/internal/views/view/selectable-trait-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <test-gesture-generator.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// Creates an on-scene View sized for tapping at the scene origin.
View CreateSceneView(UiTestApplication& application)
{
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  return view;
}
} // namespace

void utc_dali_selectabletrait_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_selectabletrait_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliSelectableTraitNewP(void)
{
  UiTestApplication application;
  SelectableTrait   selectable = SelectableTrait::New();
  DALI_TEST_CHECK(selectable);
  END_TEST;
}

int UtcDaliSelectableTraitCopyConstructorP(void)
{
  UiTestApplication application;
  SelectableTrait   selectable = SelectableTrait::New();
  SelectableTrait   copy(selectable);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == selectable);
  END_TEST;
}

int UtcDaliSelectableTraitDownCastP(void)
{
  UiTestApplication application;
  SelectableTrait   selectable = SelectableTrait::New();
  BaseHandle        handle(selectable);
  SelectableTrait   downcast = SelectableTrait::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliSelectableTraitIsSelectedDefaultP(void)
{
  UiTestApplication application;
  SelectableTrait   selectable = SelectableTrait::New();
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitIsToggleByClickEnabledDefaultP(void)
{
  UiTestApplication application;
  SelectableTrait   selectable = SelectableTrait::New();
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());
  END_TEST;
}

int UtcDaliSelectableTraitSetToggleByClickEnabledP(void)
{
  UiTestApplication application;
  SelectableTrait   selectable = SelectableTrait::New();

  selectable.SetToggleByClickEnabled();
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());

  selectable.SetToggleByClickEnabled(false);
  DALI_TEST_CHECK(!selectable.IsToggleByClickEnabled());
  END_TEST;
}

// With select-only-by-click set, a click is select-only: the first tap selects, a
// second tap is a no-op (stays selected). A programmatic SetSelected(false) still unselects
// because it does not go through the click path.
int UtcDaliSelectableTraitSelectOnlyByClickP(void)
{
  UiTestApplication application;
  View              view = CreateSceneView(application);

  SelectableTrait selectable = view.AsSelectable();
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());
  DALI_TEST_CHECK(!selectable.IsSelected());

  // Make clicks select-only: now OnClickedForToggle only ever selects.
  GetImpl(selectable).EnablePersistentSelectOnlyByClick();
  DALI_TEST_CHECK(GetImpl(selectable).IsSelectOnlyByClickEnabled());

  // First tap selects.
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(selectable.IsSelected());

  // Second tap is a no-op: still selected (a click can never unselect).
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(selectable.IsSelected());

  // Programmatic SetSelected(false) is unaffected by the flag: it unselects.
  selectable.SetSelected(false);
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}

// With the select-only-by-click flag clear (the default), a click toggles the selected
// state as usual: tap selects, tap again unselects.
int UtcDaliSelectableTraitSelectOnlyByClickDefaultTogglesP(void)
{
  UiTestApplication application;
  View              view = CreateSceneView(application);

  SelectableTrait selectable = view.AsSelectable();
  DALI_TEST_CHECK(!GetImpl(selectable).IsSelectOnlyByClickEnabled());

  // First tap selects.
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(selectable.IsSelected());

  // Second tap toggles back to unselected.
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}
