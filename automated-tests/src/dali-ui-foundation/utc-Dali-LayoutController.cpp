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
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_layoutcontroller_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_layoutcontroller_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayoutControllerGetP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliLayoutControllerOnWindowResizeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& controller = LayoutController::Get(window);
  controller.OnWindowResize(320, 240);
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliLayoutControllerProcessLayoutsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  // Add a view to the window so the controller has something to process.
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  window.Add(view);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliLayoutControllerGetSameWindowReturnsSameInstanceP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& a = LayoutController::Get(window);
  LayoutController& b = LayoutController::Get(window);
  DALI_TEST_CHECK(&a == &b);
  END_TEST;
}
