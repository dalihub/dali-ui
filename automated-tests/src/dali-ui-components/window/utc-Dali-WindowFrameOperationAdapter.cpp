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

#include <dali-ui-components/internal/window/window-operation-adapter.h>
#include <dali-ui-components/public-api/window/window-frame-options.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

int UtcDaliWindowFrameAdapterDefaultFeatureConfig(void)
{
  const WindowFrameFeatureConfig config;
  DALI_TEST_CHECK(config.moveEnabled);
  DALI_TEST_CHECK(config.resizeEnabled);
  DALI_TEST_CHECK(config.minimizeEnabled);
  DALI_TEST_CHECK(config.maximizeRestoreEnabled);
  END_TEST;
}

int UtcDaliWindowFrameAdapterAspectRatioHintRejectsEmptyWindowN(void)
{
  WindowFrameOptions options;
  auto               adapter = CreateDaliWindowOperationAdapter(Dali::Window(), WindowOperationAdapter::CloseCallback(), options);

  DALI_TEST_CHECK(!adapter->ApplyResizeAspectRatioHint(true));
  DALI_TEST_CHECK(!adapter->ApplyResizeAspectRatioHint(false));
  adapter->RestoreResizeAspectRatioHint();
  END_TEST;
}

int UtcDaliWindowFrameAdapterObservesStateWhenRequestsDisabledP(void)
{
  UiTestApplication  application;
  Dali::Window       window = application.GetWindow();
  WindowFrameOptions options;
  options.SetFeatureEnabled(WindowFrameFeature::MAXIMIZE_RESTORE, false);
  options.SetFeatureEnabled(WindowFrameFeature::MINIMIZE, false);
  auto adapter = CreateDaliWindowOperationAdapter(window, WindowOperationAdapter::CloseCallback(), options);

  window.Maximize(true);
  DALI_TEST_CHECK(adapter->GetWindowState().maximized);

  window.Maximize(false);
  window.Minimize(true);
  DALI_TEST_CHECK(adapter->GetWindowState().minimized);
  END_TEST;
}
