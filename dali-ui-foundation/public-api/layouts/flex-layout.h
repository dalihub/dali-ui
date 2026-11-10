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
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
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
 * @SINCE_3_0.0
 */
class DALI_UI_API FlexLayout : public Layout
{
public:
  /**
   * @brief Creates an uninitialized FlexLayout handle.
   * @SINCE_3_0.0
   */
  FlexLayout();

  /**
   * @brief Creates a new FlexLayout.
   *
   * @SINCE_3_0.0
   * @return A handle to a newly allocated FlexLayout
   */
  static FlexLayout New();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_3_0.0
   * @param[in] layout Handle to copy
   */
  FlexLayout(const FlexLayout& layout);

  /**
   * @brief Move constructor.
   *
   * @SINCE_3_0.0
   * @param[in] rhs Handle to move
   */
  FlexLayout(FlexLayout&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~FlexLayout();

  /**
   * @brief Copy assignment operator.
   *
   * @SINCE_3_0.0
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  FlexLayout& operator=(const FlexLayout& handle);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_3_0.0
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  FlexLayout& operator=(FlexLayout&& rhs) noexcept;

  DALI_UI_VIEW_WITH(FlexLayout)

  /**
   * @brief Downcasts a handle to FlexLayout handle.
   *
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a FlexLayout or an uninitialized handle
   */
  static FlexLayout DownCast(BaseHandle handle);

public: // API
  /**
   * @brief Sets the flex direction.
   *
   * @SINCE_3_0.0
   * @param[in] direction The flex direction
   */
  void SetDirection(FlexDirection direction);

  /**
   * @brief Gets the flex direction.
   *
   * @SINCE_3_0.0
   * @return The flex direction
   */
  FlexDirection GetDirection() const;

  /**
   * @brief Sets the wrap behavior.
   *
   * @SINCE_3_0.0
   * @param[in] wrap The wrap behavior
   */
  void SetWrap(FlexWrap wrap);

  /**
   * @brief Gets the wrap behavior.
   *
   * @SINCE_3_0.0
   * @return The wrap behavior
   */
  FlexWrap GetWrap() const;

  /**
   * @brief Sets the justify content alignment.
   *
   * @SINCE_3_0.0
   * @param[in] justify The justify content value
   */
  void SetJustifyContent(FlexJustify justify);

  /**
   * @brief Gets the justify content alignment.
   *
   * @SINCE_3_0.0
   * @return The justify content value
   */
  FlexJustify GetJustifyContent() const;

  /**
   * @brief Sets the align items value.
   *
   * @SINCE_3_0.0
   * @param[in] align The align items value
   */
  void SetAlignItems(FlexAlign align);

  /**
   * @brief Gets the align items value.
   *
   * @SINCE_3_0.0
   * @return The align items value
   */
  FlexAlign GetAlignItems() const;

  /**
   * @brief Sets the align content value (for wrapped lines).
   *
   * @SINCE_3_0.0
   * @param[in] align The align content value
   */
  void SetAlignContent(FlexAlign align);

  /**
   * @brief Gets the align content value.
   *
   * @SINCE_3_0.0
   * @return The align content value
   */
  FlexAlign GetAlignContent() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Layout implementation
   */
  explicit FlexLayout(Integration::FlexLayoutImpl& implementation);

  /**
   * @brief Allows the creation of this Layout from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit FlexLayout(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui
} //namespace DALI_NAMESPACE
