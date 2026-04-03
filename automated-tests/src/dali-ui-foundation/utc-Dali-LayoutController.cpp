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
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/adaptor-framework/window.h>

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

// int UtcDaliLayoutControllerGetP(void)
// {
//   UiTestApplication application;
//   Dali::Window window = Dali::Window::New(Dali::PositionSize(0, 0, 480, 800), "LayoutControllerTest", true);
//   if (!window)
//   {
//     tet_result(TET_PASS);
//     return 0;
//   }
//   LayoutController& controller = LayoutController::Get(window);
//   controller.ProcessLayouts();
//   END_TEST;
// }

// int UtcDaliLayoutControllerOnWindowResizeP(void)
// {
//   UiTestApplication application;
//   Dali::Window window = Dali::Window::New(Dali::PositionSize(0, 0, 480, 800), "LayoutControllerResizeTest", true);
//   if (!window)
//   {
//     tet_result(TET_PASS);
//     return 0;
//   }
//   LayoutController& controller = LayoutController::Get(window);
//   controller.OnWindowResize(320, 240);
//   END_TEST;
// }

// int UtcDaliLayoutControllerProcessLayoutsP(void)
// {
//   UiTestApplication application;
//   Dali::Window window = Dali::Window::New(Dali::PositionSize(0, 0, 480, 800), "LayoutControllerProcessTest", true);
//   if (!window)
//   {
//     tet_result(TET_PASS);
//     return 0;
//   }
//   LayoutController& controller = LayoutController::Get(window);
//   controller.ProcessLayouts();
//   END_TEST;
// }

// int UtcDaliLayoutControllerGetSameWindowReturnsSameInstanceP(void)
// {
//   UiTestApplication application;
//   Dali::Window window = Dali::Window::New(Dali::PositionSize(0, 0, 100, 100), "LayoutControllerSameTest", true);
//   if (!window)
//   {
//     tet_result(TET_PASS);
//     return 0;
//   }
//   LayoutController& a = LayoutController::Get(window);
//   LayoutController& b = LayoutController::Get(window);
//   DALI_TEST_EQUALS(&a, &b, TEST_LOCATION);
//   END_TEST;
// }
