#pragma once

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
#include <memory>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <test-application.h>
#include <dali-ui/ui-adaptor-impl.h>

namespace Dali
{

class Adaptor;
class Window;

/**
 * Adds adaptor and window mock infrastructure on top of TestApplication,
 * required for gesture testing (e.g. TestGenerateTap).
 *
 * Use TestApplication for tests that do not need gesture simulation.
 * Use UiTestApplication for tests that require adaptor timer (tap, pan, etc.).
 */
class UiTestApplication : public TestApplication
{
public:
  UiTestApplication(size_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                    size_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                    float  horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                    float  verticalDpi   = DEFAULT_VERTICAL_DPI);

  ~UiTestApplication() override;

  void InitializeAdaptor();
  void CreateSceneFromMainWindow();
  void EmitApplicationInitialize();

  /**
   * @brief Executes the idle callbacks.
   */
  void RunIdles();

  Dali::Window GetWindow()
  {
    return mMainWindow;
  }

  Dali::Adaptor& GetAdaptor()
  {
    return *mAdaptor;
  }

public:
  static bool ADD_IDLE_SUCCESS; ///< Default as true.

private:
  Dali::Window             mMainWindow;
  std::unique_ptr<Adaptor> mAdaptor;
};

} // namespace Dali
