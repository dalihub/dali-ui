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

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/internal/views/view/selectable-trait-impl.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

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
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(selectable);
  END_TEST;
}

int UtcDaliSelectableTraitCopyConstructorP(void)
{
  UiTestApplication application;
  SelectableTrait selectable = SelectableTrait::New();
  SelectableTrait copy(selectable);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == selectable);
  END_TEST;
}

int UtcDaliSelectableTraitDownCastP(void)
{
  UiTestApplication application;
  SelectableTrait selectable = SelectableTrait::New();
  BaseHandle handle(selectable);
  SelectableTrait downcast = SelectableTrait::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliSelectableTraitIsSelectedDefaultP(void)
{
  UiTestApplication application;
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitIsToggleByClickEnabledDefaultP(void)
{
  UiTestApplication application;
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());
  END_TEST;
}

int UtcDaliSelectableTraitEnableToggleByClickP(void)
{
  UiTestApplication application;
  SelectableTrait selectable = SelectableTrait::New();

  selectable.EnableToggleByClick();
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());

  selectable.EnableToggleByClick(false);
  DALI_TEST_CHECK(!selectable.IsToggleByClickEnabled());
  END_TEST;
}
