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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
class View;

namespace Internal
{
class UiScaleManagerImpl;
}

/**
 * @brief Provides access to the system-driven adaptive UI scale.
 *
 * UiScaleManager is a handle-based singleton that exposes the current system
 * scale factor and propagates scale changes to all registered layout roots.
 *
 * @code
 *   float scale = UiScaleManager::Get().GetScale();
 * @endcode
 */
class DALI_UI_API UiScaleManager : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized UiScaleManager handle.
   */
  UiScaleManager();

  /**
   * @brief Destructor.
   */
  ~UiScaleManager() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  UiScaleManager(const UiScaleManager& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  UiScaleManager(UiScaleManager&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  UiScaleManager& operator=(const UiScaleManager& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  UiScaleManager& operator=(UiScaleManager&& rhs) noexcept = default;

  /**
   * @brief Returns the singleton UiScaleManager instance.
   *
   * Creates the instance on first call.
   *
   * @return A handle to the UiScaleManager singleton
   */
  static UiScaleManager Get();

  /**
   * @brief Downcasts a handle to a UiScaleManager handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a UiScaleManager or an uninitialized handle
   */
  static UiScaleManager DownCast(BaseHandle handle);

  /**
   * @brief Returns the current system scale factor.
   *
   * @return The current scale value (1.0 by default)
   */
  float GetScale() const;

  /**
   * @brief Sets the system scale factor.
   *
   * Called by system/vconf handlers when the display scale changes.
   * Invalidates all registered layout roots and triggers re-layout.
   *
   * @param[in] scale The new scale factor
   */
  void SetScale(float scale);

public: // Not intended for Application developers
  /// @cond internal
  explicit UiScaleManager(Internal::UiScaleManagerImpl* impl);

  /**
   * @brief Registers a layout root to be invalidated when the system scale changes.
   *
   * Called automatically by ViewImpl when a view becomes a layout root.
   *
   * @param[in] root The root view to register
   */
  void RegisterLayoutRoot(View root);

  /**
   * @brief Unregisters a layout root.
   *
   * Called automatically by ViewImpl when a root view leaves the scene.
   *
   * @param[in] root The root view to unregister
   */
  void UnregisterLayoutRoot(View root);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
