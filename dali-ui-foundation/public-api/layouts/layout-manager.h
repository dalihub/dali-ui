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
   * @note The owning View caches the result and does NOT call this again while the
   * normalised constraint is unchanged and nothing has invalidated the owner's
   * layout. There is no measure counterpart to GetArrangePolicy(): measure
   * caching is unconditional, so this applies to every manager, including one
   * written outside this library. The override must therefore be a pure function of
   * the constraints, the owner's effective scale, the owner's effective layout
   * direction, the owner's layout-tracked state and the children's measured sizes.
   *
   * A manager that keeps hidden state of its own -- a spacing or orientation held on
   * the manager and mutated through its own setter -- is outside that envelope. Pair
   * every such setter with InvalidateOwnerMeasure() (or InvalidateOwnerArrange() when
   * only placement is affected), which is what the in-library managers do.
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
   * @param[in] bounds The available bounds for arranging children (owner-local content area)
   */
  virtual void Arrange(ViewImpl* view, const LayoutRect& bounds) = 0;

  // ============================================================
  // Non-virtual, library-internal API.
  //
  // Outside the virtual freeze above: it occupies no vtable slot and adds no data
  // member, so it is additive for both the vtable layout and the instance size.
  //
  // DALI_INTERNAL marks it as not-for-application-use. It does NOT reliably hide the
  // symbol: the macro is a hidden-visibility attribute only on ELF toolchains, and on
  // Windows the enclosing `class DALI_UI_API LayoutManager` is dllexport'ed as a
  // whole, so every member -- this one included -- is exported from the DLL. Treat
  // "internal" here as a contract, not as an enforcement.
  // ============================================================

  /**
   * @brief Returns this manager's arrange execution policy.
   *
   * The default is ArrangePolicy::IF_CHANGED. A subclass can select
   * ArrangePolicy::ALWAYS through the protected SetArrangePolicy() method.
   *
   * @return The active arrange execution policy
   * @note Internal: for use by this library only, and reserved for future change.
   */
  DALI_INTERNAL ArrangePolicy GetArrangePolicy() const;

  /**
   * @brief Records the View this manager has been attached to.
   *
   * Called once by the framework from View::AttachLayoutManager. A manager can never
   * be replaced or detached, so there is no reverse edge.
   *
   * @param[in] owner The attaching View
   * @note Internal: for use by this library only. See the DALI_INTERNAL note above.
   */
  DALI_INTERNAL void SetOwnerView(ViewImpl* owner);

protected:
  class Impl;

  /**
   * @brief Sets when this manager's Arrange() implementation must run.
   *
   * The default is ArrangePolicy::IF_CHANGED. Use
   * ArrangePolicy::ALWAYS when Arrange() reads state that is not tracked by
   * layout invalidation or performs work that must happen on every arrange pass.
   * The policy is inherited by subclasses and may be changed again by a subclass.
   *
   * @param[in] policy The arrange execution policy
   */
  void SetArrangePolicy(ArrangePolicy policy);

  /**
   * @brief Invalidates the owning View's MEASURE (and, with it, its arrange).
   *
   * Call this from any setter that changes state this manager's Measure() or
   * Arrange() reads. Both the measure cache and the arrange cache key on the owner's
   * layout state, and neither has any way to observe state held privately on a
   * manager, so a manager that mutates such state without saying so leaves the owner
   * serving a result computed against the old value.
   *
   * The in-library managers all do this, which is what makes their state -- a stack
   * orientation, a grid's row definitions, a flex justification -- part of the
   * layout-tracked inputs used by their Arrange() implementations.
   *
   * Safe before attach and after the owner is gone: a null owner makes it a no-op.
   */
  void InvalidateOwnerMeasure();

  /**
   * @brief Invalidates the owning View's ARRANGE only.
   *
   * The narrower counterpart to InvalidateOwnerMeasure(), for state that moves
   * children within an unchanged owner size and cannot change any measured result.
   * When in doubt use InvalidateOwnerMeasure(), which is always correct and merely
   * does more work.
   *
   * Safe before attach and after the owner is gone: a null owner makes it a no-op.
   */
  void InvalidateOwnerArrange();

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
   * @brief Gets the number of logical child views of the given view.
   *
   * Excludes in-flight EXIT ghosts and non-View actor children, matching
   * Ui::View::GetChildViewCount().
   *
   * @param[in] view The view to query
   * @return Number of child views
   */
  uint32_t GetChildViewCount(ViewImpl* view) const;

  /**
   * @brief Gets the logical child View at the given index.
   *
   * Matches Ui::View::GetChildViewAt(): returns an empty handle if @p index
   * is out of range.
   *
   * @param[in] view  The view to query
   * @param[in] index The child index, in [0, GetChildViewCount(view))
   * @return The child View handle, or an empty handle if out of range
   */
  View GetChildViewAt(ViewImpl* view, uint32_t index) const;

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
