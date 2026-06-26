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

#include <dali-ui-test-suite-utils.h>
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_components_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_components_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliComponentsStyleSheetNewP(void)
{
  UiStyleSheet styleSheet      = Components::StyleSheet::New();
  UiStyleSheet otherStyleSheet = Components::StyleSheet::New();
  UiStyleKey<> unregisteredKey = UiStyleKey<>::Alloc();

  DALI_TEST_CHECK(styleSheet);
  DALI_TEST_CHECK(otherStyleSheet);
  DALI_TEST_CHECK(styleSheet.GetObjectPtr() != otherStyleSheet.GetObjectPtr());
  DALI_TEST_CHECK(!styleSheet.IsFrozen());
  DALI_TEST_CHECK(!styleSheet.GetStyle(unregisteredKey));
  END_TEST;
}
