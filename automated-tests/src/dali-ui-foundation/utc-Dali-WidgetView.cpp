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

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/widget/widget-view.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali::Ui;

void utc_dali_widget_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_widget_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliWidgetViewCopyMoveOperations(void)
{
  WidgetView source;
  WidgetView copy(source);
  DALI_TEST_CHECK(!copy);

  WidgetView copyAssigned;
  DALI_TEST_CHECK(&(copyAssigned = source) == &copyAssigned);
  DALI_TEST_CHECK(!copyAssigned);

  WidgetView moved(std::move(copy));
  DALI_TEST_CHECK(!moved);

  WidgetView moveAssigned;
  DALI_TEST_CHECK(&(moveAssigned = std::move(copyAssigned)) == &moveAssigned);
  DALI_TEST_CHECK(!moveAssigned);
  END_TEST;
}
