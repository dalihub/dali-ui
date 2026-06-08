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
#include <dali-ui-foundation/public-api/i-scroll-bar.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/scrollable-enum.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/property-map.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{
class ScrollBarImpl;
}

/**
 * @brief A class representing a scroll bar that can be used to control the scrolling of a view.
 */
class DALI_UI_API ScrollBar : public AbsoluteLayout, public IScrollBar
{
public:
  /**
   * @brief Creates an uninitialized ScrollBar handle.
   */
  ScrollBar();

  /**
   * @brief Copy constructor.
   */
  ScrollBar(const ScrollBar& scrollBar);

  /**
   * @brief Move constructor.
   */
  ScrollBar(ScrollBar&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~ScrollBar();

  /**
   * @brief Copy assignment operator.
   */
  ScrollBar& operator=(const ScrollBar& handle);

  /**
   * @brief Move assignment operator.
   */
  ScrollBar& operator=(ScrollBar&& rhs) noexcept;

public: // Creation & Destruction
  /**
   * @brief Creates an initialized ScrollBar.
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollBar New();

  /**
   * @brief Downcasts a handle to ScrollBar handle.
   * @param[in] handle Handle to an object.
   * @return A handle to a ScrollBar or an uninitialized handle.
   */
  static ScrollBar DownCast(BaseHandle handle);

public: // IScrollBar Implementation
  /**
   * @copydoc IScrollBar::GetTarget()
   */
  View GetTarget() const override;

  /**
   * @copydoc IScrollBar::GetVerticalScrollBarVisibility()
   */
  ScrollBarVisibility GetVerticalScrollBarVisibility() const override;

  /**
   * @copydoc IScrollBar::SetVerticalScrollBarVisibility()
   */
  void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility) override;

  /**
   * @copydoc IScrollBar::GetHorizontalScrollBarVisibility()
   */
  ScrollBarVisibility GetHorizontalScrollBarVisibility() const override;

  /**
   * @copydoc IScrollBar::SetHorizontalScrollBarVisibility()
   */
  void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility) override;

  /**
   * @copydoc IScrollBar::UpdateBarSize()
   */
  void UpdateBarSize(float scrollWidth, float scrollHeight, float viewportWidth, float viewportHeight) override;

  /**
   * @copydoc IScrollBar::UpdateScrollPosition()
   */
  void UpdateScrollPosition(const Vector2& position) override;

  /**
   * @copydoc IScrollBar::HideBar()
   */
  void HideBar() override;

public: // Properties
  /**
   * @brief Gets or sets the thickness of the scroll bar.
   * @return The thickness of the scroll bar.
   */
  float GetBarThickness() const;
  void  SetBarThickness(float thickness);

  /**
   * @brief Gets or sets the color of the scroll bar.
   * @return The color of the scroll bar.
   */
  Vector4 GetBarColor() const;
  void    SetBarColor(const Vector4& color);

  /**
   * @brief Gets or sets the minimum size of the scroll bar.
   * @return The minimum size of the scroll bar.
   */
  float GetBarMinSize() const;
  void  SetBarMinSize(float minSize);

  /**
   * @brief Gets or sets the current scroll position of the target view.
   * @return The current scroll position.
   */
  Vector2 GetScrollPosition() const;
  void    SetScrollPosition(const Vector2& position);

  /**
   * @brief Gets or sets the offset of bars from the edges.
   * This creates a small gap between the bar and the scroll bar container edges.
   * Padding can be set using View::SetPadding() which is inherited by ScrollBar.
   * @return The bar offset.
   */
  float GetBarOffset() const;
  void  SetBarOffset(float offset);

  /**
   * @brief Gets the corner radius of the scroll bar.
   * @return The corner radius for each corner (topLeft, topRight, bottomRight, bottomLeft).
   */
  Vector4 GetBarCornerRadius() const;

  /**
   * @brief Sets the corner radius of the scroll bar (same for all corners).
   * @param[in] radius The corner radius to apply to all corners.
   */
  void SetBarCornerRadius(float radius);

  /**
   * @brief Sets the corner radius of the scroll bar for each corner individually.
   * @param[in] radius The corner radius (topLeft, topRight, bottomRight, bottomLeft).
   */
  void SetBarCornerRadius(const Vector4& radius);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle from the Integration implementation.
   */
  explicit DALI_UI_API ScrollBar(Integration::ScrollBarImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   */
  explicit DALI_UI_API ScrollBar(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOUNDATION_SCROLL_BAR_H