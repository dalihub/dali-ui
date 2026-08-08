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
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
View gFallbackTarget;
int gFallbackCallCount = 0;

FocusNavigationResult MoveFallback(View, FocusNavigationContext)
{
  ++gFallbackCallCount;
  return FocusNavigationResult::MoveTo(gFallbackTarget);
}

FocusNavigationResult NotHandledFallback(View, FocusNavigationContext)
{
  ++gFallbackCallCount;
  return FocusNavigationResult::NotHandled();
}
} // namespace

void utc_dali_focusmanager_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_focusmanager_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliFocusManagerFallbackRunsWithDefaultAlgorithmDisabledInternalP(void)
{
  UiTestApplication application;

  View current = View::New();
  current.SetFocusable(true);
  View fallbackTarget = View::New();
  fallbackTarget.SetFocusable(true);
  application.GetScene().Add(current);
  application.GetScene().Add(fallbackTarget);
  application.SendNotification();
  application.Render();

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager.RequestFocus(current));
  GetImpl(manager).EnableDefaultAlgorithm(false);
  gFallbackTarget = fallbackTarget;
  gFallbackCallCount = 0;
  manager.SetFocusNavigationFallback(FocusNavigationCallback::New(&MoveFallback));

  DALI_TEST_CHECK(manager.MoveFocus(FocusDirection::RIGHT));
  DALI_TEST_CHECK(manager.GetCurrentFocusView() == fallbackTarget);
  DALI_TEST_CHECK(gFallbackCallCount == 1);

  manager.SetFocusNavigationFallback(FocusNavigationCallback::New(&NotHandledFallback));
  DALI_TEST_CHECK(!manager.MoveFocus(FocusDirection::LEFT));
  DALI_TEST_CHECK(manager.GetCurrentFocusView() == fallbackTarget);
  DALI_TEST_CHECK(gFallbackCallCount == 2);

  GetImpl(manager).EnableDefaultAlgorithm(true);
  manager.SetFocusNavigationFallback({});
  gFallbackTarget.Reset();
  END_TEST;
}
