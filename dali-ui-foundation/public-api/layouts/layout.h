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
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
namespace Integration
{
class LayoutImpl;
}

#include "layout.autogen.h"
/**
 * @brief Layout is a View that arranges child views using a layout algorithm.
 *
 * Layout extends View and always has a LayoutManager (from derived classes
 * like StackLayout, GridLayout).
 *
 * Child management uses Actor::Add/Remove (inherited from View's base class).
 * Insert(index, View) and RemoveAllChildren() are provided by View.
 */
class DALI_UI_API Layout : public View
{
public:
  /**
   * @brief Creates an uninitialized Layout handle.
   */
  Layout();

  /**
   * @brief Creates an initialized Layout.
   *
   * @return A handle to a newly allocated Layout
   */
  static Layout New();

  /**
   * @brief Copy constructor.
   *
   * @param[in] layout Handle to copy
   */
  Layout(const Layout& layout);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  Layout(Layout&& rhs) noexcept;

  /**
   * @brief Destructor.
   *
   * This is non-virtual since Handle classes must not have virtual methods.
   */
  ~Layout();

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  Layout& operator=(const Layout& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  Layout& operator=(Layout&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to Layout handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a Layout or an uninitialized handle
   */
  static Layout DownCast(BaseHandle handle);

public:
  // @CHAIN_START(Layout, View)
  // @CHAIN_END

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  explicit DALI_INTERNAL Layout(Integration::LayoutImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Integration::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL Layout(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  DALI_UI_CHAIN_VIEW_METHODS(Layout)
};

} // namespace Ui
} // namespace Dali
