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
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/absolute-layout-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/scroll/scrollable-enum.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class ScrollBarImpl;
using ScrollBarImplPtr = IntrusivePtr<ScrollBarImpl>;

/**
 * @brief This is the internal implementation class for ScrollBar.
 *
 * ScrollBarImpl extends ViewImpl and provides scroll bar functionality
 * for ScrollView and RecyclerView components.
 */
class DALI_UI_API ScrollBarImpl : public AbsoluteLayoutImpl
{
public:
  // Creation & Destruction

  /**
   * @brief Creates a new ScrollBar.
   */
  static ScrollBarImplPtr New();

protected:
  /**
   * @brief Destructor.
   */
  virtual ~ScrollBarImpl();

  // Construction

  /**
   * @brief ScrollBar constructor.
   */
  ScrollBarImpl();

public: // From Ui::Internal::Control
  /**
   * @copydoc Ui::Internal::Control::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @brief Initializes the scroll bar.
   */
  void Initialize();

public: // IScrollBar Implementation
  /**
   * @brief Gets the target view.
   */
  View GetTarget() const;

  /**
   * @brief Gets the vertical scroll bar visibility.
   */
  ScrollBarVisibility GetVerticalScrollBarVisibility() const;

  /**
   * @brief Sets the vertical scroll bar visibility.
   */
  void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);

  /**
   * @brief Gets the horizontal scroll bar visibility.
   */
  ScrollBarVisibility GetHorizontalScrollBarVisibility() const;

  /**
   * @brief Sets the horizontal scroll bar visibility.
   */
  void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);

  /**
   * @brief Updates the size of the scroll bar.
   */
  void UpdateBarSize(float scrollWidth, float scrollHeight, float viewportWidth, float viewportHeight);

  /**
   * @brief Updates the scroll position.
   */
  void UpdateScrollPosition(const Vector2& position);

  /**
   * @brief Hides the scroll bar (fade out).
   */
  void HideBar();

public: // Properties
  /**
   * @brief Gets the bar thickness.
   */
  float GetBarThickness() const;

  /**
   * @brief Sets the bar thickness.
   */
  void SetBarThickness(float thickness);

  /**
   * @brief Gets the bar color.
   */
  Vector4 GetBarColor() const;

  /**
   * @brief Sets the bar color.
   */
  void SetBarColor(const Vector4& color);

  /**
   * @brief Gets the bar minimum size.
   */
  float GetBarMinSize() const;

  /**
   * @brief Sets the bar minimum size.
   */
  void SetBarMinSize(float minSize);

  /**
   * @brief Gets the scroll position.
   */
  Vector2 GetScrollPosition() const;

  /**
   * @brief Sets the scroll position.
   */
  void SetScrollPosition(const Vector2& position);

  /**
   * @brief Gets the bar offset.
   */
  float GetBarOffset() const;

  /**
   * @brief Sets the bar offset.
   */
  void SetBarOffset(float offset);

  /**
   * @brief Gets the bar corner radius.
   */
  Vector4 GetBarCornerRadius() const;

  /**
   * @brief Sets the bar corner radius (same for all corners).
   */
  void SetBarCornerRadius(float radius);

  /**
   * @brief Sets the bar corner radius for each corner.
   */
  void SetBarCornerRadius(const Vector4& radius);

protected:
  /**
   * @brief Updates the bar size.
   */
  void UpdateBarSize();

  /**
   * @brief Shows the bar.
   */
  void ShowBar();

  /**
   * @brief Plays fade in animation.
   */
  void PlayFadeIn();

  /**
   * @brief Called when fade in animation finishes.
   */
  void OnFadeInFinished(Animation animation);

  /**
   * @brief Called when the auto-hide timer fires (2-second delay after last ShowBar).
   */
  bool OnHideTimerFired();

  /**
   * @brief Plays fade out animation.
   */
  void PlayFadeOut();

  /**
   * @brief Updates the position.
   */
  void UpdatePosition(const Vector2& position);

  /**
   * @brief Removes vertical bar.
   */
  void RemoveVBar();

  /**
   * @brief Creates vertical bar.
   */
  void CreateVBar();

  /**
   * @brief Updates vertical bar size.
   */
  void UpdateVBarSize(float ratio);

  /**
   * @brief Removes horizontal bar.
   */
  void RemoveHBar();

  /**
   * @brief Creates horizontal bar.
   */
  void CreateHBar();

  /**
   * @brief Updates horizontal bar size.
   */
  void UpdateHBarSize(float ratio);

  /**
   * @brief Sets vertical bar bounds with change detection.
   */
  void SetVBarBounds(float x, float y, float width, float height);

  /**
   * @brief Sets horizontal bar bounds with change detection.
   */
  void SetHBarBounds(float x, float y, float width, float height);

  /**
   * @brief Applies corner radius to vertical and horizontal bars.
   */
  void ApplyCornerRadiusToBars();

private:
  // Not copyable or movable
  ScrollBarImpl(const ScrollBarImpl&)            = delete;
  ScrollBarImpl(ScrollBarImpl&&)                 = delete;
  ScrollBarImpl& operator=(const ScrollBarImpl&) = delete;
  ScrollBarImpl& operator=(ScrollBarImpl&&)      = delete;

private:
  // Views
  View mVerticalBar;   ///< The vertical bar view
  View mHorizontalBar; ///< The horizontal bar view

  // Properties
  float   mBarThickness;    ///< The thickness of the scroll bar
  Vector4 mBarColor;        ///< The color of the scroll bar
  float   mBarMinSize;      ///< The minimum size of the scroll bar
  Vector2 mScrollPosition;  ///< The current scroll position
  float   mBarOffset;       ///< The offset of bars from the edges (padding is inherited from View)
  Vector4 mBarCornerRadius; ///< The corner radius for each corner (topLeft, topRight, bottomRight, bottomLeft)

  // Scroll bar visibility
  ScrollBarVisibility mVerticalVisibility;   ///< Vertical scroll bar visibility
  ScrollBarVisibility mHorizontalVisibility; ///< Horizontal scroll bar visibility

  // Size tracking
  float      mScrollableWidth;  ///< Scrollable content width
  float      mScrollableHeight; ///< Scrollable content height
  float      mViewPortWidth;    ///< Viewport width
  float      mViewPortHeight;   ///< Viewport height
  LayoutRect mLastVBarBounds;   ///< Last vertical bar bounds (for change detection)
  LayoutRect mLastHBarBounds;   ///< Last horizontal bar bounds (for change detection)

  // Animation
  Animation mFadeInAnimation;  ///< Fade in animation
  Animation mFadeOutAnimation; ///< Fade out animation
  bool      mIsBarVisible;     ///< True once bars are fully opaque (fade-in complete)

  // Auto-hide timer
  Timer mHideTimer; ///< Fires 2 seconds after the last ShowBar call to trigger fade-out
};

} // namespace Integration

} // namespace Ui

} // namespace Dali