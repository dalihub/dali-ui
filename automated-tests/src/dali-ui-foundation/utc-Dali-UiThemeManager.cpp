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
#include <dali-ui-foundation/public-api/ui-theme-manager.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

bool gThemeChangedCalled = false;

class ThemeChangedCallback : public ConnectionTracker
{
public:
  void OnThemeChanged()
  {
    gThemeChangedCalled = true;
  }
};

} // namespace

void utc_dali_uithememanager_startup(void)
{
  test_return_value    = TET_UNDEF;
  gThemeChangedCalled  = false;
  UiConfig::New().Apply();
}

void utc_dali_uithememanager_cleanup(void)
{
  test_return_value = TET_PASS;
}

// === Constructor ===

int UtcDaliUiThemeManagerConstructorP(void)
{
  TestApplication application;

  UiThemeManager manager;
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

// === Get Singleton ===

int UtcDaliUiThemeManagerGetP(void)
{
  TestApplication application;

  UiThemeManager manager = UiThemeManager::Get();
  DALI_TEST_CHECK(manager);

  END_TEST;
}

int UtcDaliUiThemeManagerGetSingletonP(void)
{
  TestApplication application;

  UiThemeManager manager1 = UiThemeManager::Get();
  UiThemeManager manager2 = UiThemeManager::Get();

  DALI_TEST_CHECK(manager1);
  DALI_TEST_CHECK(manager2);
  DALI_TEST_CHECK(manager1 == manager2);

  END_TEST;
}

// === Copy / Move ===

int UtcDaliUiThemeManagerCopyConstructorP(void)
{
  TestApplication application;

  UiThemeManager manager = UiThemeManager::Get();
  UiThemeManager copy(manager);

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(manager == copy);

  END_TEST;
}

int UtcDaliUiThemeManagerMoveConstructorP(void)
{
  TestApplication application;

  UiThemeManager manager = UiThemeManager::Get();
  DALI_TEST_EQUALS(2, manager.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  UiThemeManager moved(std::move(manager));
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

int UtcDaliUiThemeManagerCopyAssignmentP(void)
{
  TestApplication application;

  UiThemeManager manager = UiThemeManager::Get();
  UiThemeManager copy;
  copy = manager;

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(manager == copy);

  END_TEST;
}

int UtcDaliUiThemeManagerMoveAssignmentP(void)
{
  TestApplication application;

  UiThemeManager manager = UiThemeManager::Get();

  UiThemeManager moved;
  moved = std::move(manager);

  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

// === DownCast ===

int UtcDaliUiThemeManagerDownCastP(void)
{
  TestApplication application;

  UiThemeManager manager = UiThemeManager::Get();
  BaseHandle     handle(manager);

  UiThemeManager downCasted = UiThemeManager::DownCast(handle);
  DALI_TEST_CHECK(downCasted);

  END_TEST;
}

int UtcDaliUiThemeManagerDownCastN(void)
{
  TestApplication application;

  BaseHandle handle;
  UiThemeManager downCasted = UiThemeManager::DownCast(handle);
  DALI_TEST_CHECK(!downCasted);

  END_TEST;
}

// === GetCurrentThemeId ===

int UtcDaliUiThemeManagerGetCurrentThemeIdP(void)
{
  TestApplication application;

  UiThemeManager manager = UiThemeManager::Get();
  String themeId = manager.GetCurrentThemeId();

  // DefaultThemeLoader returns "default"
  DALI_TEST_EQUALS(themeId, String("default"), TEST_LOCATION);

  END_TEST;
}

// === ThemeChangedSignal ===

int UtcDaliUiThemeManagerThemeChangedSignalP(void)
{
  TestApplication application;

  UiThemeManager manager = UiThemeManager::Get();

  ThemeChangedCallback callback;
  manager.ThemeChangedSignal().Connect(&callback, &ThemeChangedCallback::OnThemeChanged);

  // Signal should be connectable without crashing
  DALI_TEST_CHECK(true);

  END_TEST;
}
