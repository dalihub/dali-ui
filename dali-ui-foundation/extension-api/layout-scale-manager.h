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

namespace Extension
{
/**
 * @brief Holds the UI scale and the window scales that the layout pass applies to every view.
 *
 * LayoutScaleManager is a handle to a process-wide singleton. It is not a LayoutManager
 * and lays nothing out itself. It holds the UI scale -- the system scale, usually set
 * through SetUiScale() by a system or accessibility handler --, one window scale per
 * Window, set through SetWindowScale(), and a process-wide switch that enables or
 * disables both. Their product is each view's effective scale:
 * @code
 *   effectiveScale = GetWindowScale(theViewsWindow) * theViewsUiScaleTerm
 * @endcode
 * where the UI-scale term is the UI scale or 1.0, as the view's UiScalePolicy selects;
 * Extension::View::GetEffectiveLayoutScale() returns the product. The effective scale is
 * applied in the layout pass, so rendering -- text size, borders, blur and render effects --
 * and the conversion between requested units and on-screen pixels follow it as well.
 *
 * This is an extension API for platform integrators and module providers -- the system or
 * accessibility handler that applies the display scale, or a module that scales the content
 * of a window -- and not part of the application-facing public API: an application gets both
 * scales applied by the layout without calling it. It is installed with the public API and
 * remains usable.
 *
 * @code
 *   Extension::LayoutScaleManager::Get().SetUiScale(1.5f);
 *   Extension::LayoutScaleManager::Get().SetWindowScale(window, 2.0f); // views in window: 2.0 * 1.5
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
   * Extension::View::GetEffectiveLayoutScale() returns a view's effective scale. The stored value is
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
   * A view that is not on a scene belongs to no registered layout root and is not laid out
   * again, but it caches no effective scale either: Extension::View::GetEffectiveLayoutScale() and
   * Ui::View::Measure() on it use the new value at once, and it is laid out at that value when
   * it is added to a scene.
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
   * A process-wide master switch for both scales. While scaling is disabled every view's
   * effective scale is 1.0, whatever its UiScalePolicy, the value set through SetUiScale()
   * and any window scale set through SetWindowScale(); both stored values are kept --
   * GetUiScale() and GetWindowScale() keep reporting them -- and applied again when scaling
   * is enabled. Changing the switch invalidates all registered layout roots and triggers a
   * re-layout; setting the current state again does nothing. A view that is not on a scene
   * follows the switch at once, as it follows SetUiScale().
   *
   * @param[in] enable true to enable scaling, false to disable it
   */
  void SetScalable(bool enable);

  /**
   * @brief Returns the window scale applied to every view in @p window.
   *
   * This is the per-window term of a view's effective scale; see SetWindowScale(). It is
   * the STORED value: like GetUiScale(), it keeps reporting it while scaling is disabled
   * through SetScalable(false), although no view is scaled by it then.
   *
   * @param[in] window The window to query
   * @return The window's scale, or 1.0 when none was set, when it was reset, or when
   *         @p window is empty. A scale recorded for a window that has since been
   *         destroyed is never reported for another window, and its record is dropped
   *         automatically
   */
  float GetWindowScale(Window window) const;

  /**
   * @brief Sets the window scale applied to every view in @p window.
   *
   * A view's effective scale is the product of two independent terms:
   * @code
   *   effectiveScale = GetWindowScale(theViewsWindow) * theViewsUiScaleTerm
   * @endcode
   * where the UI-scale term is the one UiScalePolicy selects (GetUiScale(), or 1.0 for
   * UiScalePolicy::DISABLED). The effective scale multiplies the pixel sizes handed to the
   * view, so both layout and rendering happen at that scale.
   *
   * The window scale is INDEPENDENT of UiScalePolicy: it is applied to every view in
   * the window, including views whose policy is UiScalePolicy::DISABLED. It is NOT
   * independent of SetScalable(): while scaling is disabled every view's effective
   * scale is 1.0, this term included. A window scale set while scaling is disabled is
   * stored -- GetWindowScale() reports it -- and takes effect when scaling is enabled
   * again, exactly like a value set through SetUiScale(). It does not change the
   * window's own pixel size -- it changes how much logical content fits in it.
   *
   * Passing 1.0 restores the default and drops the window's record. Setting a scale
   * invalidates only the layout roots registered in @p window and triggers a re-layout
   * of that window; other windows are untouched. While scaling is disabled that
   * re-layout is deferred to SetScalable(true).
   *
   * Destroying a window needs no reset: its record is dropped automatically, and a
   * scale recorded for a destroyed window never applies to any later window.
   *
   * The window scale applies to a view only while the view is in @p window. A view that
   * leaves it -- removed from it, or still in it when it is destroyed -- loses the window
   * term together with its whole subtree: until it is added to a window again, its
   * effective scale (Extension::View::GetEffectiveLayoutScale()) and Ui::View::Measure() on it use
   * the UI-scale term alone, the current one: they follow SetUiScale() and SetScalable()
   * while the view is detached.
   *
   * @param[in] window The window whose views are affected. An empty window is ignored
   *            and logged.
   * @param[in] scale The window scale. It must be positive, finite and normal
   *            (>= FLT_MIN), because the layout also divides by the effective scale;
   *            other values are ignored and logged, leaving the current window scale in
   *            place. The same requirement applies to the PRODUCT of this scale and the
   *            UI scale, which is the value the layout actually divides by: each term
   *            passing the check on its own does not keep the product finite and normal
   *            (1e20 x 1e20 overflows, 1e-25 x 1e-25 underflows to zero), and the product
   *            is deliberately not validated here because that would make either term's
   *            validity depend on the order the two were set in. Keep both terms within
   *            a sane display range.
   */
  void SetWindowScale(Window window, float scale);

public: // Not intended for Application developers
  /// @cond internal
  /**
   * @brief Creates a handle from the implementation.
   *
   * @param[in] impl A pointer to the implementation
   */
  explicit DALI_INTERNAL LayoutScaleManager(Ui::Internal::LayoutScaleManagerImpl* impl);
  /// @endcond
};

} // namespace Extension
} // namespace Ui
} //namespace DALI_NAMESPACE
