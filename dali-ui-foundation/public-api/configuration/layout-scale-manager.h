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

namespace DALI_NAMESPACE
{
namespace Ui
{

namespace Internal
{
class LayoutScaleManagerImpl;
}

/**
 * @brief Holds the UI scale that the layout pass applies to every view.
 *
 * LayoutScaleManager is a handle to a process-wide singleton. It is not a LayoutManager
 * and lays nothing out itself. It holds the UI scale -- the system scale, usually set
 * through SetUiScale() by a system or accessibility handler -- and a process-wide switch
 * that enables or disables scaling. A view's effective scale is the UI scale or 1.0, as
 * the view's UiScalePolicy selects; ViewImpl::GetEffectiveLayoutScale() returns it. The
 * effective scale is applied in the layout pass, so rendering -- text size, borders, blur
 * and render effects -- and the conversion between requested units and on-screen pixels
 * follow it as well.
 *
 * @code
 *   LayoutScaleManager::Get().SetUiScale(1.5f);
 *   float uiScale = LayoutScaleManager::Get().GetUiScale();
 * @endcode
 */
class DALI_UI_API LayoutScaleManager : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized LayoutScaleManager handle.
   */
  LayoutScaleManager();

  /**
   * @brief Destructor.
   */
  ~LayoutScaleManager() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  LayoutScaleManager(const LayoutScaleManager& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  LayoutScaleManager(LayoutScaleManager&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  LayoutScaleManager& operator=(const LayoutScaleManager& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  LayoutScaleManager& operator=(LayoutScaleManager&& rhs) noexcept = default;

  /**
   * @brief Returns the singleton LayoutScaleManager instance.
   *
   * Creates the instance on first call. The returned handle is never empty, also when it
   * is requested before the application is initialized.
   *
   * @return A handle to the LayoutScaleManager singleton
   */
  static LayoutScaleManager Get();

  /**
   * @brief Downcasts a handle to a LayoutScaleManager handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a LayoutScaleManager or an uninitialized handle
   */
  static LayoutScaleManager DownCast(BaseHandle handle);

  /**
   * @brief Returns the UI scale.
   *
   * This is the stored value set through SetUiScale(), not the scale of a particular view;
   * ViewImpl::GetEffectiveLayoutScale() returns a view's effective scale. The stored value is
   * reported while scaling is disabled through SetScalable(false) as well.
   *
   * @return The current UI scale (1.0 by default)
   */
  float GetUiScale() const;

  /**
   * @brief Sets the UI scale.
   *
   * Called by system or accessibility handlers when the display scale changes. A new value
   * invalidates all registered layout roots and triggers a re-layout; setting the current
   * value again does nothing. While scaling is disabled through SetScalable(false) the
   * value is stored, and the re-layout is deferred until scaling is enabled again.
   *
   * @param[in] scale The new UI scale. It must be positive, finite and normal (>= FLT_MIN),
   *            because the layout also divides by it; other values are ignored and logged,
   *            leaving the current UI scale in place.
   */
  void SetUiScale(float scale);

  /**
   * @brief Returns whether scaling is enabled.
   *
   * @return true if scaling is enabled (the default), false if it is disabled process-wide
   */
  bool IsScalable() const;

  /**
   * @brief Enables or disables scaling process-wide.
   *
   * A process-wide master switch. While scaling is disabled every view's effective scale is
   * 1.0, whatever its UiScalePolicy and the value set through SetUiScale(); the stored value
   * is kept -- GetUiScale() keeps reporting it -- and applied again when scaling is enabled.
   * Changing the switch invalidates all registered layout roots and triggers a re-layout;
   * setting the current state again does nothing.
   *
   * @param[in] enable true to enable scaling, false to disable it
   */
  void SetScalable(bool enable);

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief Creates a handle from the implementation.
   *
   * @param[in] impl A pointer to the implementation
   */
  explicit DALI_INTERNAL LayoutScaleManager(Internal::LayoutScaleManagerImpl* impl);
  /// @endcond
};

} // namespace Ui
} //namespace DALI_NAMESPACE
