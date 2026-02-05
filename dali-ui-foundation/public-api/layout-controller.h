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
#include <dali/public-api/adaptor-framework/window.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layout-types.h>

namespace Dali
{
namespace UI
{

// Forward declarations
namespace Integration
{
class ViewImpl;
class LayoutControllerImpl;
} // namespace Integration

/**
 * @brief Layout controller manages layout invalidation and processing.
 *
 * The layout controller is responsible for scheduling layout passes when
 * layouts become invalid and processing them during the render loop.
 * It integrates with the DALi adaptor system as a processor.
 *
 * Each Window has its own LayoutController instance. The controller
 * batches layout requests and processes them once per frame to optimize
 * performance.
 */
class DALI_UI_API LayoutController
{
public:
  /**
   * @brief Gets the layout controller instance for a window.
   *
   * Creates the controller if it doesn't exist for the given window.
   *
   * @param[in] window The window to get the controller for
   * @return Reference to the layout controller
   */
  static LayoutController& Get(Window window);

  /**
   * @brief Destructor.
   */
  ~LayoutController();

  /**
   * @brief Schedules a view with layout capability for layout processing.
   *
   * Layout roots are views that have a LayoutManager and are at the top
   * of the layout hierarchy (e.g. directly under the window).
   * The controller will batch these requests and process them
   * during the next frame.
   *
   * @param[in] view The view with layout capability to schedule
   */
  void RequestLayout(Integration::ViewImpl* view);

  /**
   * @brief Called when the window is resized.
   *
   * This invalidates all layout roots and triggers a layout pass.
   *
   * @param[in] width The new window width
   * @param[in] height The new window height
   */
  void OnWindowResize(int32_t width, int32_t height);

  /**
   * @brief Processes all pending layout requests immediately.
   *
   * This is called automatically by the system once per frame,
   * but can be called manually if immediate layout is needed.
   */
  void ProcessLayouts();

private:
  /**
   * @brief Private constructor.
   *
   * Use LayoutController::Get() to obtain an instance.
   *
   * @param[in] window The window this controller manages
   */
  explicit LayoutController(Window window);

  // Not copyable or movable
  LayoutController(const LayoutController&) = delete;
  LayoutController(LayoutController&&) = delete;
  LayoutController& operator=(const LayoutController&) = delete;
  LayoutController& operator=(LayoutController&&) = delete;

private:
  std::unique_ptr<Integration::LayoutControllerImpl> mImpl;
};

} // namespace UI
} // namespace Dali