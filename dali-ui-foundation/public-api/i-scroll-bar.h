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

#ifndef DALI_UI_FOUNDATION_I_SCROLL_BAR_H
#define DALI_UI_FOUNDATION_I_SCROLL_BAR_H

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/scrollable-enum.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{

namespace Ui
{

/**
 * @brief Defines the interface for a scroll bar, which is responsible for displaying and controlling the scrolling of a target view.
 */
class DALI_UI_API IScrollBar
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~IScrollBar() = default;

  /**
   * @brief Gets the target view for which the scroll bar is responsible.
   * @return The target view.
   */
  virtual View GetTarget() const = 0;

  /**
   * @brief Gets the visibility of the vertical scroll bar.
   * @return The vertical scroll bar visibility.
   */
  virtual ScrollBarVisibility GetVerticalScrollBarVisibility() const = 0;

  /**
   * @brief Sets the visibility of the vertical scroll bar.
   * @param[in] visibility The vertical scroll bar visibility.
   */
  virtual void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility) = 0;

  /**
   * @brief Gets the visibility of the horizontal scroll bar.
   * @return The horizontal scroll bar visibility.
   */
  virtual ScrollBarVisibility GetHorizontalScrollBarVisibility() const = 0;

  /**
   * @brief Sets the visibility of the horizontal scroll bar.
   * @param[in] visibility The horizontal scroll bar visibility.
   */
  virtual void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility) = 0;

  /**
   * @brief Updates the size of the scroll bar based on the given scroll area and viewport size.
   * @param[in] scrollWidth The width of the scrollable area.
   * @param[in] scrollHeight The height of the scrollable area.
   * @param[in] viewportWidth The width of the visible area.
   * @param[in] viewportHeight The height of the visible area.
   */
  virtual void UpdateBarSize(float scrollWidth, float scrollHeight, float viewportWidth, float viewportHeight) = 0;

  /**
   * @brief Updates the scroll position of the scroll bar based on the given position.
   * @param[in] position The new scroll position.
   */
  virtual void UpdateScrollPosition(const Vector2& position) = 0;

  /**
   * @brief Hides the scroll bar (fade out).
   *
   * Called when scrolling ends. Only affects bars with Auto visibility.
   */
  virtual void HideBar() = 0;
};

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOUNDATION_I_SCROLL_BAR_H