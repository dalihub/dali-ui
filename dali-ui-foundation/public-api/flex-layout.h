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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layout.h>
#include <dali-ui-foundation/public-api/layout-types.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
namespace Integration
{
class FlexLayoutImpl;
}

/**
 * @brief FlexLayout arranges its children using the CSS Flexbox algorithm.
 *
 * FlexLayout provides a flexible way to lay out children in rows or columns,
 * with support for wrapping, alignment, and flexible sizing.
 *
 * It supports:
 * - Direction (Row, RowReverse, Column, ColumnReverse)
 * - Wrap behavior (NoWrap, Wrap, WrapReverse)
 * - Justify content (FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround, SpaceEvenly)
 * - Align items and align content
 */
class DALI_UI_API FlexLayout : public Layout
{
public:
  /**
   * @brief Creates an uninitialized FlexLayout handle.
   */
  FlexLayout();

  /**
   * @brief Creates a new FlexLayout.
   *
   * @return A handle to a newly allocated FlexLayout
   */
  static FlexLayout New();

  /**
   * @brief Copy constructor.
   *
   * @param[in] layout Handle to copy
   */
  FlexLayout(const FlexLayout& layout);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  FlexLayout(FlexLayout&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~FlexLayout();

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  FlexLayout& operator=(const FlexLayout& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  FlexLayout& operator=(FlexLayout&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to FlexLayout handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a FlexLayout or an uninitialized handle
   */
  static FlexLayout DownCast(BaseHandle handle);

public: // API

  /**
   * @brief Sets the flex direction.
   *
   * @param[in] direction The flex direction
   */
  void SetDirection(FlexDirection direction);

  /**
   * @brief Gets the flex direction.
   *
   * @return The flex direction
   */
  FlexDirection GetDirection() const;

  /**
   * @brief Sets the wrap behavior.
   *
   * @param[in] wrap The wrap behavior
   */
  void SetWrap(FlexWrap wrap);

  /**
   * @brief Gets the wrap behavior.
   *
   * @return The wrap behavior
   */
  FlexWrap GetWrap() const;

  /**
   * @brief Sets the justify content alignment.
   *
   * @param[in] justify The justify content value
   */
  void SetJustifyContent(FlexJustify justify);

  /**
   * @brief Gets the justify content alignment.
   *
   * @return The justify content value
   */
  FlexJustify GetJustifyContent() const;

  /**
   * @brief Sets the align items value.
   *
   * @param[in] align The align items value
   */
  void SetAlignItems(FlexAlign align);

  /**
   * @brief Gets the align items value.
   *
   * @return The align items value
   */
  FlexAlign GetAlignItems() const;

  /**
   * @brief Sets the align content value (for wrapped lines).
   *
   * @param[in] align The align content value
   */
  void SetAlignContent(FlexAlign align);

  /**
   * @brief Gets the align content value.
   *
   * @return The align content value
   */
  FlexAlign GetAlignContent() const;

public: // Static methods for attached properties

  /**
   * @brief Sets the flex grow factor for a child view.
   *
   * @param[in] view The child view
   * @param[in] grow The grow factor (default 0)
   */
  static void SetFlexGrow(View view, float grow);

  /**
   * @brief Gets the flex grow factor for a child view.
   *
   * @param[in] view The child view
   * @return The grow factor
   */
  static float GetFlexGrow(View view);

  /**
   * @brief Sets the flex shrink factor for a child view.
   *
   * @param[in] view The child view
   * @param[in] shrink The shrink factor (default 1)
   */
  static void SetFlexShrink(View view, float shrink);

  /**
   * @brief Gets the flex shrink factor for a child view.
   *
   * @param[in] view The child view
   * @return The shrink factor
   */
  static float GetFlexShrink(View view);

  /**
   * @brief Sets the flex basis for a child view.
   *
   * @param[in] view The child view
   * @param[in] basis The basis value (use LayoutDimension::WrapContent for auto)
   */
  static void SetFlexBasis(View view, float basis);

  /**
   * @brief Gets the flex basis for a child view.
   *
   * @param[in] view The child view
   * @return The basis value
   */
  static float GetFlexBasis(View view);

  /**
   * @brief Sets the align self for a child view.
   *
   * @param[in] view The child view
   * @param[in] align The align self value
   */
  static void SetAlignSelf(View view, FlexAlign align);

  /**
   * @brief Gets the align self for a child view.
   *
   * @param[in] view The child view
   * @return The align self value
   */
  static FlexAlign GetAlignSelf(View view);

public: // Chaining methods
  DALI_UI_CHAIN_LAYOUT_METHODS(FlexLayout)

  FlexLayout& Direction(FlexDirection direction)
  {
    SetDirection(direction);
    return *this;
  }

  FlexLayout& Wrap(FlexWrap wrap)
  {
    SetWrap(wrap);
    return *this;
  }

  FlexLayout& JustifyContent(FlexJustify justify)
  {
    SetJustifyContent(justify);
    return *this;
  }

  FlexLayout& AlignItems(FlexAlign align)
  {
    SetAlignItems(align);
    return *this;
  }

  FlexLayout& AlignContent(FlexAlign align)
  {
    SetAlignContent(align);
    return *this;
  }

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL FlexLayout(Integration::FlexLayoutImpl& implementation);
  explicit DALI_INTERNAL FlexLayout(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali