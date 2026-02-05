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
#include <cstdint>
#include <initializer_list>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace UI
{

// Forward declarations
namespace Integration
{
class LayoutImpl;
}

/**
 * @brief Layout is a View that arranges child views using a layout algorithm.
 *
 * Layout extends View and always has a LayoutManager (from derived classes
 * like StackLayout, GridLayout). Only Layout (and subclasses) can have
 * children; the public API for adding/removing children is on Layout.
 *
 * - Always has a LayoutManager (from derived classes like StackLayout, GridLayout)
 * - Supports ClipsToBounds functionality
 * - Child Management API: AddView, RemoveView, RemoveAllViews, RemoveViewAt, GetChildCount, GetChildAt, IndexOfChild,
 * Contents(...)
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

public: // Layout Properties API

  /**
   * @brief Sets whether this layout clips its children to its bounds.
   *
   * @param[in] clips True to clip children to bounds
   */
  void SetClipsToBounds(bool clips);

  /**
   * @brief Gets whether this layout clips its children to its bounds.
   *
   * @return True if children are clipped to bounds
   */
  bool GetClipsToBounds() const;

public: // Child Management API

  /**
   * @brief Adds a view to this layout (at the end).
   *
   * The view is managed by this layout's layout manager and added to the Actor hierarchy.
   *
   * @param[in] view The view to add
   */
  void AddView(View view);

  /**
   * @brief Adds a view at the specified index.
   *
   * @param[in] view The view to add
   * @param[in] index The index where to insert
   */
  void AddView(View view, uint32_t index);

  /**
   * @brief Removes a view from this layout.
   *
   * @param[in] view The view to remove
   */
  void RemoveView(View view);

  /**
   * @brief Removes the view at the specified index.
   *
   * @param[in] index The index of the view to remove
   */
  void RemoveViewAt(uint32_t index);

  /**
   * @brief Removes all child views from this layout.
   */
  void RemoveAllViews();

  /**
   * @brief Gets the number of child views.
   *
   * @return The child count
   */
  uint32_t GetChildCount() const;

  /**
   * @brief Gets the child view at the specified index.
   *
   * @param[in] index The child index
   * @return The child view at the index
   */
  View GetChildAt(uint32_t index) const;

  /**
   * @brief Returns the index of the given child view, or -1 if not found.
   *
   * @param[in] view The child view to find
   * @return Index of the view, or -1 if not a child
   */
  int32_t IndexOfChild(View view) const;

  /**
   * @brief Adds a list of children in a declarative way (method chaining).
   *
   * @param[in] children The initializer list of View handles to add
   * @return Reference to this for method chaining
   */
  Layout& Contents(std::initializer_list<View> children);

public: // Chaining Methods
  /**
   * @brief Sets clips to bounds with method chaining.
   *
   * @param[in] clips True to clip children to bounds
   * @return Reference to this for method chaining
   */
  Layout& ClipsToBounds(bool clips)
  {
    SetClipsToBounds(clips);
    return *this;
  }

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL Layout(Integration::LayoutImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Integration::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL Layout(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace UI
} // namespace Dali