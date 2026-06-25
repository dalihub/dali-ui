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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
namespace Internal
{
class AbsoluteLayoutParamsImpl;
}

/**
 * @brief AbsoluteLayoutParams stores per-child layout parameters for AbsoluteLayout.
 *
 * This handle provides direct access to bounds and flags attached to a child view.
 * Use View::SetLayoutParams() to attach parameters to a child view.
 *
 * @code
 * view.SetLayoutParams(AbsoluteLayoutParams::New()
 *   .SetBounds(LayoutRect(10, 20, 100, 200))
 *   .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
 * @endcode
 */
class DALI_UI_API AbsoluteLayoutParams : public LayoutParams
{
public:
  /**
   * @brief Creates an uninitialized handle.
   */
  AbsoluteLayoutParams();

  /**
   * @brief Creates a new AbsoluteLayoutParams with default values.
   *
   * @return A handle to a newly allocated AbsoluteLayoutParams
   */
  static AbsoluteLayoutParams New();

  /**
   * @brief Creates a new AbsoluteLayoutParams by copying values from an existing one.
   *
   * @param[in] other The params to copy from
   * @return A handle to a newly allocated AbsoluteLayoutParams with copied values
   */
  static AbsoluteLayoutParams New(const AbsoluteLayoutParams& other);

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  AbsoluteLayoutParams(const AbsoluteLayoutParams& handle);

  /**
   * @brief Destructor.
   */
  ~AbsoluteLayoutParams();

  /**
   * @brief Downcasts a handle to AbsoluteLayoutParams.
   *
   * @param[in] handle Handle to an object
   * @return A valid handle or an uninitialized handle
   */
  static AbsoluteLayoutParams DownCast(BaseHandle handle);

  /**
   * @brief Returns the layout params type identifier.
   *
   * @return LayoutParamsType::ABSOLUTE
   */
  static LayoutParamsType GetLayoutParamsType();

  /**
   * @brief Sets the absolute position and size of the child within the layout.
   *
   * @param[in] bounds The layout bounds (x, y, width, height)
   * @return Reference to this for chaining
   * @note RTL mirror: under a RIGHT_TO_LEFT effective layout direction the X
   * supplied here is mirrored about the parent width after arrange
   * (newX = parentWidth - x - childWidth). This applies to every child;
   * the only opt-out is placing the child in LayoutMode STANDALONE.
   */
  AbsoluteLayoutParams& SetBounds(const LayoutRect& bounds);

  /**
   * @brief Gets the absolute position and size of the child within the layout.
   *
   * @return The layout bounds
   */
  LayoutRect GetBounds() const;

  /**
   * @brief Sets the X position of the child within the layout.
   *
   * @param[in] x The X position
   * @return Reference to this for chaining
   */
  AbsoluteLayoutParams& SetX(float x);

  /**
   * @brief Gets the X position of the child within the layout.
   *
   * @return The X position
   */
  float GetX() const;

  /**
   * @brief Sets the Y position of the child within the layout.
   *
   * @param[in] y The Y position
   * @return Reference to this for chaining
   */
  AbsoluteLayoutParams& SetY(float y);

  /**
   * @brief Gets the Y position of the child within the layout.
   *
   * @return The Y position
   */
  float GetY() const;

  /**
   * @brief Sets the width of the child within the layout.
   *
   * @param[in] width The width (-1 means use View's own size)
   * @return Reference to this for chaining
   */
  AbsoluteLayoutParams& SetWidth(float width);

  /**
   * @brief Gets the width of the child within the layout.
   *
   * @return The width
   */
  float GetWidth() const;

  /**
   * @brief Sets the height of the child within the layout.
   *
   * @param[in] height The height (-1 means use View's own size)
   * @return Reference to this for chaining
   */
  AbsoluteLayoutParams& SetHeight(float height);

  /**
   * @brief Gets the height of the child within the layout.
   *
   * @return The height
   */
  float GetHeight() const;

  /**
   * @brief Sets the layout flags that control proportional sizing behavior.
   *
   * @param[in] flags The absolute layout flags
   * @return Reference to this for chaining
   */
  AbsoluteLayoutParams& SetFlags(AbsoluteLayoutFlags flags);

  /**
   * @brief Gets the layout flags that control proportional sizing behavior.
   *
   * @return The absolute layout flags
   */
  AbsoluteLayoutFlags GetFlags() const;

public: // Not intended for application developers
  /// @cond internal
  explicit AbsoluteLayoutParams(Internal::AbsoluteLayoutParamsImpl* implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
