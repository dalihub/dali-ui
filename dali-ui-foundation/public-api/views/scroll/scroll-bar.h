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

#ifndef DALI_UI_FOUNDATION_SCROLL_BAR_H
#define DALI_UI_FOUNDATION_SCROLL_BAR_H
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/views/scroll/scrollable-enum.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/property-map.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace Integration
{
class ScrollBarImpl;
}

/**
 * @brief A class representing a scroll bar that displays the scroll position of a view.
 * @SINCE_3_0.0
 */
class DALI_UI_API ScrollBar : public AbsoluteLayout
{
public:
  /**
   * @brief Creates an uninitialized ScrollBar handle.
   * @SINCE_3_0.0
   */
  ScrollBar();

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   */
  ScrollBar(const ScrollBar& scrollBar);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   */
  ScrollBar(ScrollBar&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~ScrollBar();

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   */
  ScrollBar& operator=(const ScrollBar& handle);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   */
  ScrollBar& operator=(ScrollBar&& rhs) noexcept;

  DALI_UI_VIEW_WITH(ScrollBar)

public: // Creation & Destruction
  /**
   * @brief Creates an initialized ScrollBar.
   * @SINCE_3_0.0
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollBar New();

  /**
   * @brief Downcasts a handle to ScrollBar handle.
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object.
   * @return A handle to a ScrollBar or an uninitialized handle.
   */
  static ScrollBar DownCast(BaseHandle handle);

public: // Scroll bar state
  /**
   * @brief Gets the visibility of the vertical scroll bar.
   * @SINCE_3_0.0
   * @return The vertical scroll bar visibility.
   */
  ScrollBarVisibility GetVerticalScrollBarVisibility() const;

  /**
   * @brief Sets the visibility of the vertical scroll bar.
   * @SINCE_3_0.0
   * @param[in] visibility The vertical scroll bar visibility.
   */
  void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);

  /**
   * @brief Gets the visibility of the horizontal scroll bar.
   * @SINCE_3_0.0
   * @return The horizontal scroll bar visibility.
   */
  ScrollBarVisibility GetHorizontalScrollBarVisibility() const;

  /**
   * @brief Sets the visibility of the horizontal scroll bar.
   * @SINCE_3_0.0
   * @param[in] visibility The horizontal scroll bar visibility.
   */
  void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);

  /**
   * @brief Updates the size of the scroll bar based on the given scroll area and viewport size.
   * @SINCE_3_0.0
   * @param[in] scrollWidth The width of the scrollable area.
   * @param[in] scrollHeight The height of the scrollable area.
   * @param[in] viewportWidth The width of the visible area.
   * @param[in] viewportHeight The height of the visible area.
   */
  void UpdateBarSize(float scrollWidth, float scrollHeight, float viewportWidth, float viewportHeight);

  /**
   * @brief Updates the scroll position of the scroll bar based on the given position.
   * @SINCE_3_0.0
   * @param[in] position The new scroll position.
   */
  void UpdateScrollPosition(const Vector2& position);

  /**
   * @brief Hides the scroll bar (fade out).
   *
   * Only affects bars with Auto visibility.
   * @SINCE_3_0.0
   */
  void HideBar();

public: // Properties
  /**
   * @brief Gets or sets the thickness of the scroll bar.
   * @SINCE_3_0.0
   * @return The thickness of the scroll bar.
   */
  float GetBarThickness() const;
  void  SetBarThickness(float thickness);

  /**
   * @brief Gets or sets the color of the scroll bar.
   * @SINCE_3_0.0
   * @return The color of the scroll bar.
   */
  Vector4 GetBarColor() const;
  void    SetBarColor(const Vector4& color);

  /**
   * @brief Gets or sets the minimum size of the scroll bar.
   *
   * Like the thickness and the offset it is a length in natural units, multiplied by the effective
   * scale. A value <= 0 selects a twentieth of the scroll bar's longer side; GetBarMinSize() then
   * returns that length in natural units.
   * @SINCE_3_0.0
   * @return The minimum size of the scroll bar.
   */
  float GetBarMinSize() const;
  void  SetBarMinSize(float minSize);

  /**
   * @brief Gets or sets the current scroll position of the target view.
   * @SINCE_3_0.0
   * @return The current scroll position.
   */
  Vector2 GetScrollPosition() const;
  void    SetScrollPosition(const Vector2& position);

  /**
   * @brief Gets or sets the offset of bars from the edges.
   * This creates a small gap between the bar and the scroll bar container edges.
   * Padding can be set using View::SetPadding() which is inherited by ScrollBar.
   * @SINCE_3_0.0
   * @return The bar offset.
   */
  float GetBarOffset() const;
  void  SetBarOffset(float offset);

  /**
   * @brief Gets the corner radius of the scroll bar.
   * @SINCE_3_0.0
   * @return The corner radius for each corner (topLeft, topRight, bottomRight, bottomLeft).
   */
  Vector4 GetBarCornerRadius() const;

  /**
   * @brief Sets the corner radius of the scroll bar (same for all corners).
   * @SINCE_3_0.0
   * @param[in] radius The corner radius to apply to all corners.
   */
  void SetBarCornerRadius(float radius);

  /**
   * @brief Sets the corner radius of the scroll bar for each corner individually.
   * @SINCE_3_0.0
   * @param[in] radius The corner radius (topLeft, topRight, bottomRight, bottomLeft).
   */
  void SetBarCornerRadius(const Vector4& radius);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle from the Integration implementation.
   */
  explicit ScrollBar(Integration::ScrollBarImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   */
  explicit ScrollBar(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_FOUNDATION_SCROLL_BAR_H
