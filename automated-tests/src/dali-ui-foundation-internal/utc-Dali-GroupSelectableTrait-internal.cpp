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
#include <dali-ui-foundation/public-api/traits/group-selectable-trait.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_groupselectabletrait_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_groupselectabletrait_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// Construction / Handle (internal New() method tests)
// ============================================================================

int UtcDaliGroupSelectableTraitNewP(void)
{
  UiTestApplication    application;
  GroupSelectableTrait trait = GroupSelectableTrait::New();
  DALI_TEST_CHECK(trait);
  END_TEST;
}

int UtcDaliGroupSelectableTraitCopyConstructorP(void)
{
  UiTestApplication    application;
  GroupSelectableTrait trait = GroupSelectableTrait::New();
  GroupSelectableTrait copy(trait);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == trait);
  END_TEST;
}

int UtcDaliGroupSelectableTraitDownCastP(void)
{
  UiTestApplication    application;
  GroupSelectableTrait trait = GroupSelectableTrait::New();
  BaseHandle           handle(trait);
  GroupSelectableTrait downcast = GroupSelectableTrait::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliGroupSelectableTraitHandleChainP(void)
{
  UiTestApplication    application;
  GroupSelectableTrait trait = GroupSelectableTrait::New();

  // GroupSelectableTrait IS-A SelectableTrait IS-A InteractiveTrait by static type.
  SelectableTrait  asSelectable  = trait;
  InteractiveTrait asInteractive = trait;
  DALI_TEST_CHECK(asSelectable);
  DALI_TEST_CHECK(asInteractive);

  // Same underlying object across the whole chain.
  DALI_TEST_CHECK(asSelectable == trait);
  DALI_TEST_CHECK(asInteractive == trait);

  // Selectable APIs are reachable through the group handle.
  DALI_TEST_CHECK(!trait.IsSelected());
  // Interactive APIs are reachable through the group handle.
  trait.ClickedSignal();
  END_TEST;
}
