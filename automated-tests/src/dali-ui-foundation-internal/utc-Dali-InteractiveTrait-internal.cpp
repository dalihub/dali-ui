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
#include <dali-ui-foundation/internal/views/view/interactive-trait-impl.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_interactivetrait_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_interactivetrait_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInteractiveTraitNewP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  DALI_TEST_CHECK(clickable);
  END_TEST;
}

int UtcDaliInteractiveTraitCopyConstructorP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  InteractiveTrait copy(clickable);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == clickable);
  END_TEST;
}

int UtcDaliInteractiveTraitDownCastP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  BaseHandle handle(clickable);
  InteractiveTrait downcast = InteractiveTrait::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliInteractiveTraitIsClickableDefaultP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  DALI_TEST_CHECK(clickable.IsClickable());
  END_TEST;
}

int UtcDaliInteractiveTraitSetClickableP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();

  clickable.SetClickable(false);
  DALI_TEST_CHECK(!clickable.IsClickable());

  clickable.SetClickable(true);
  DALI_TEST_CHECK(clickable.IsClickable());
  END_TEST;
}

int UtcDaliInteractiveTraitIsPressedDefaultP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  DALI_TEST_CHECK(!clickable.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitPseudoDisabledP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();

  DALI_TEST_CHECK(!clickable.IsPseudoDisabled());

  clickable.SetPseudoDisabled(true);
  DALI_TEST_CHECK(clickable.IsPseudoDisabled());

  clickable.SetPseudoDisabled(false);
  DALI_TEST_CHECK(!clickable.IsPseudoDisabled());
  END_TEST;
}

int UtcDaliInteractiveTraitKeyClickPolicyDefaultP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();

  KeyClickPolicy policy = clickable.GetKeyClickPolicy();
  DALI_TEST_CHECK(static_cast<uint32_t>(policy) <= static_cast<uint32_t>(KeyClickPolicy::RESERVED));
  END_TEST;
}

int UtcDaliInteractiveTraitSetKeyClickPolicyP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();

  clickable.SetKeyClickPolicy(KeyClickPolicy::DISABLED);
  DALI_TEST_EQUALS(static_cast<uint32_t>(clickable.GetKeyClickPolicy()),
                   static_cast<uint32_t>(KeyClickPolicy::DISABLED),
                   TEST_LOCATION);
  END_TEST;
}
