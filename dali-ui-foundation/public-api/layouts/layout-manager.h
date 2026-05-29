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
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
class View;
class ViewImpl;

/**
 * @brief Abstract base class for layout managers.
 *
 * Layout managers are responsible for measuring and arranging child views
 * within a container. They implement the specific layout algorithms
 * for different layout types (stack, grid, flex, etc.).
 *
 * A LayoutManager can be attached to any View via View::AttachLayoutManager.
 * After attach, the View's layout pass calls LayoutManager::Measure and
 * LayoutManager::Arrange unless a MeasureCallback / ArrangeCallback is set
 * (which take priority over the manager).
 *
 * Custom layout algorithms can be implemented by subclassing LayoutManager
 * directly and using the provided protected helpers to traverse children.
 */
class DALI_UI_API LayoutManager
{
public:
  // ============================================================
  // ABI-frozen virtual API.
  // WARNING: Do NOT add, reorder, remove, or change signatures.
  // ============================================================

  /**
   * @brief Virtual destructor.
   */
  virtual ~LayoutManager();

  /**
   * @brief Measures the view and its children.
   *
   * This method calculates the desired size of the view based on the
   * constraints and the sizes of its children.
   *
   * @param[in] view The view to measure
   * @param[in] widthConstraint The available width constraint
   * @param[in] heightConstraint The available height constraint
   * @return The measured size
   */
  virtual MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) = 0;

  /**
   * @brief Arranges the children within the view bounds.
   *
   * This method positions and sizes the children within the given bounds
   * according to the layout algorithm.
   *
   * @param[in] view The view whose children to arrange
   * @param[in] bounds The available bounds for arranging children
   * @return The final arranged size
   */
  virtual MeasuredSize Arrange(ViewImpl* view, const LayoutRect& bounds) = 0;

protected:
  class Impl;

  /**
   * @brief Default constructor.
   */
  LayoutManager();

  /**
   * @brief Constructor for derived classes that provide their own implementation storage.
   *
   * @param[in] impl The implementation storage (ownership transferred)
   * @note Internal: intended only for in-library layout managers and is not part of
   *       the public ABI. Externally derived layout managers must use the default
   *       constructor.
   */
  DALI_INTERNAL explicit LayoutManager(Impl* impl);

  /**
   * @brief Gets the number of children of the given view.
   *
   * @param[in] view The view to query
   * @return Number of child views
   */
  uint32_t GetChildCount(ViewImpl* view) const;

  /**
   * @brief Gets the child View at the given index.
   *
   * @param[in] view  The view to query
   * @param[in] index The child index
   * @return The child View handle
   */
  View GetChildAt(ViewImpl* view, uint32_t index) const;

  /**
   * @brief Returns whether the child's LayoutMode is STANDALONE.
   *
   * Standalone children are measured and arranged by the framework outside
   * the layout manager's pass and should be skipped during child iteration.
   *
   * @param[in] child The child view implementation
   * @return True if the child is STANDALONE
   */
  bool IsStandalone(ViewImpl* child) const;

  /**
   * @brief Gets the implementation storage as a derived implementation type.
   *
   * @tparam LayoutManagerImplType The derived implementation type that was supplied to
   *         the LayoutManager(Impl*) constructor. Behaviour is undefined if a different
   *         type is requested.
   * @return The typed implementation storage
   * @note Internal: intended only for in-library layout managers and is not part of the
   *       public ABI.
   */
  template<typename LayoutManagerImplType>
  DALI_INTERNAL LayoutManagerImplType* GetImplAs()
  {
    DALI_ASSERT_DEBUG(dynamic_cast<LayoutManagerImplType*>(mImpl) && "Requested Impl type does not match the stored LayoutManager::Impl.");
    return static_cast<LayoutManagerImplType*>(mImpl);
  }

  /**
   * @brief Gets the implementation storage as a derived implementation type (const overload).
   *
   * @tparam LayoutManagerImplType The derived implementation type that was supplied to
   *         the LayoutManager(Impl*) constructor. Behaviour is undefined if a different
   *         type is requested.
   * @return The typed implementation storage
   * @note Internal: intended only for in-library layout managers and is not part of the
   *       public ABI.
   */
  template<typename LayoutManagerImplType>
  DALI_INTERNAL const LayoutManagerImplType* GetImplAs() const
  {
    DALI_ASSERT_DEBUG(dynamic_cast<const LayoutManagerImplType*>(mImpl) && "Requested Impl type does not match the stored LayoutManager::Impl.");
    return static_cast<const LayoutManagerImplType*>(mImpl);
  }

private:
  // Not copyable or movable
  LayoutManager(const LayoutManager&)            = delete;
  LayoutManager(LayoutManager&&)                 = delete;
  LayoutManager& operator=(const LayoutManager&) = delete;
  LayoutManager& operator=(LayoutManager&&)      = delete;

  Impl* mImpl{nullptr};
};

} // namespace Ui
} // namespace Dali
