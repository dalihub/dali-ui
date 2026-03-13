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
#include <dali-ui-foundation/public-api/controls/control-impl.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/interaction-trait-interface.h>
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{

namespace Ui
{

// Forward declarations
class Layout;
class LayoutManager;

namespace Integration
{

class ViewImpl;
using ViewImplPtr = IntrusivePtr<ViewImpl>;

/**
 * @brief This is the internal implementation class for View.
 *
 * ViewImpl extends ControlImpl to provide basic UI functionality.
 * It implements the p-impl pattern and provides the actual implementation
 * for the View public API.
 *
 * View can optionally have a LayoutManager to manage child layout.
 * When a LayoutManager is set, the View can manage children and delegate
 * layout calculations to the LayoutManager.
 *
 * @see Dali::Ui::View
 */
class DALI_UI_API ViewImpl : public Ui::Internal::Control
{
public:
  // Creation & Destruction

  /**
   * @brief Child data structure for layout calculations.
   *
   * Stores information about each child including the View handle,
   * measured size, and arranged bounds.
   */
  struct ChildData
  {
    Ui::View     view;           ///< Handle to the child view
    MeasuredSize measuredSize;   ///< Size from Measure pass
    LayoutRect   arrangedBounds; ///< Bounds from Arrange pass
  };

  /**
   * @brief Container type for children.
   */
  using ChildContainer = std::vector<ChildData>;

  /**
   * @brief Creates a new View.
   */
  static ViewImplPtr New();

protected:
  /**
   * @brief Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ViewImpl();

  // Construction

  /**
   * @brief View constructor.
   */
  ViewImpl();

public: // From Ui::Internal::Control
  /**
   * @copydoc Ui::Internal::Control::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Ui::Internal::Control::OnSceneConnection
   * When this View is a layout root and is connected to a window, registers
   * with LayoutController so that measure/arrange runs even if invalidation
   * occurred before the view was added to the window.
   */
  void OnSceneConnection(int depth) override;

  /**
   * @brief Override to separate dali-ui layout from DALi size negotiation.
   * When this View has a LayoutManager, size/position are driven by dali-ui
   * LayoutController; we no-op. Otherwise delegate to Control.
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Toolkit::Internal::Control::OnKeyInputFocusGained
   */
  void OnKeyInputFocusGained() override;

  /**
   * @copydoc Toolkit::Internal::Control::OnKeyInputFocusLost
   */
  void OnKeyInputFocusLost() override;

  /**
   * @copydoc Toolkit::Internal::Control::OnKeyEvent
   */
  bool OnKeyEvent(const Dali::KeyEvent& event) override;

public: // API (size, position, parent origin, pivot)
  /**
   * @copydoc Dali::Ui::View::GetSizeWidth
   */
  float GetSizeWidth() const;

  /**
   * @copydoc Dali::Ui::View::SetSizeWidth
   */
  void SetSizeWidth(float width);

  /**
   * @copydoc Dali::Ui::View::GetSizeHeight
   */
  float GetSizeHeight() const;

  /**
   * @copydoc Dali::Ui::View::SetSizeHeight
   */
  void SetSizeHeight(float height);

  /**
   * @copydoc Dali::Ui::View::GetPositionX
   */
  float GetPositionX() const;

  /**
   * @copydoc Dali::Ui::View::SetPositionX
   */
  void SetPositionX(float x);

  /**
   * @copydoc Dali::Ui::View::GetPositionY
   */
  float GetPositionY() const;

  /**
   * @copydoc Dali::Ui::View::SetPositionY
   */
  void SetPositionY(float y);

  /**
   * @copydoc Dali::Ui::View::GetParentOrigin
   */
  Vector3 GetParentOrigin() const;

  /**
   * @copydoc Dali::Ui::View::SetParentOrigin
   */
  void SetParentOrigin(const Vector3& point);

  /**
   * @copydoc Dali::Ui::View::GetPivotPoint
   */
  Vector3 GetPivotPoint() const;

  /**
   * @copydoc Dali::Ui::View::SetPivotPoint
   */
  void SetPivotPoint(const Vector3& point);

  /**
   * @copydoc Dali::Ui::View::IsFocusable
   */
  bool IsFocusable() const;

  /**
   * @copydoc Dali::Ui::View::SetFocusable
   */
  void SetFocusable(bool focusable);

  /**
   * @copydoc Dali::Ui::View::IsTouchFocusable
   */
  bool IsTouchFocusable() const;

  /**
   * @copydoc Dali::Ui::View::SetTouchFocusable
   */
  void SetTouchFocusable(bool touchFocusable);

  /**
   * @brief Sets a trait to this View.
   *
   * The trait will share the lifecycle with this View.
   *
   * For traits identified by a user-defined @p TraitId, calling this method with the
   * same id will replace the existing trait after calling OnDetached() on the old one.
   *
   * For the reserved id @c ReservedTraitId::INTERACTION_TRAIT:
   * - The trait must implement @c IInteractionTrait.
   * - It can be set only once for the lifetime of the View; attempting to replace or
   *   remove it is considered a programming error and will trigger an assertion.
   *
   * @throws DaliException If the trait already has an owner
   * @throws DaliException If the id is already registered with other trait
   *
   * @note **Strong Reference**
   * The view will hold the strong reference to the trait after attached.
   *
   * @param[in] id The unique key to identify the trait
   * @param[in] trait The trait object to attach
   */
  void SetTrait(TraitId id, Trait& trait);

  /**
   * @brief Gets a trait from this View.
   * @param[in] id The unique key to identify the trait
   * @return The trait handle
   */
  Trait GetTrait(TraitId id) const;

  /**
   * @brief Removes a trait from this View.
   *
   * For traits identified by a user-defined @p TraitId, this detaches the trait,
   * calls its OnDetached(), and returns true on success.
   *
   * For the reserved id @c ReservedTraitId::INTERACTION_TRAIT, removal is not allowed; an assertion
   * will be triggered and the method will always return false.
   *
   * @param[in] id The unique key to identify the trait
   * @return True if succeeded, false otherwise
   */
  bool RemoveTrait(TraitId id);

public: // Measure / Arrange API
  /**
   * @brief Measures the view with the given constraints.
   */
  MeasuredSize Measure(float widthConstraint, float heightConstraint);

  /**
   * @brief Arranges the view within the given bounds.
   */
  MeasuredSize Arrange(const LayoutRect& bounds);

  /**
   * @brief Invalidates the measure of this view and propagates up.
   */
  void InvalidateMeasure();

  /**
   * @brief Invalidates the arrange of this view.
   */
  void InvalidateArrange();

  /**
   * @brief Gets the desired size after measurement.
   */
  MeasuredSize GetDesiredSize() const;

  /**
   * @brief Sets the desired size directly.
   *
   * Used by LayoutManagers to override the desired size for children
   * whose final size is determined by the parent (e.g., weighted children
   * in StackLayout). This ensures that OnArrange uses the correct size.
   *
   * @param[in] size The desired size to set
   */
  void SetDesiredSize(const MeasuredSize& size);

  /**
   * @brief Checks if the measure is valid.
   */
  bool IsMeasureValid() const;

  /**
   * @brief Checks if the arrange is valid.
   */
  bool IsArrangeValid() const;

protected: // Virtual methods for derived classes (Template Method pattern)
  /**
   * @brief Called during measure pass.
   */
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);

  /**
   * @brief Called during arrange pass.
   */
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);

public: // Layout size API (LayoutWidth / LayoutHeight)
  void  SetLayoutWidth(float width);
  float GetLayoutWidth() const;
  void  SetLayoutHeight(float height);
  float GetLayoutHeight() const;
  void  SetMinimumWidth(float width);
  float GetMinimumWidth() const;
  void  SetMinimumHeight(float height);
  float GetMinimumHeight() const;
  void  SetMaximumWidth(float width);
  float GetMaximumWidth() const;
  void  SetMaximumHeight(float height);
  float GetMaximumHeight() const;

public: // Layout Params API
  /**
   * @copydoc Dali::Ui::View::SetLayoutParams
   */
  void SetLayoutParams(Ui::LayoutParams params);

  /**
   * @brief Retrieves a layout params trait by LayoutParamsType.
   */
  BaseHandle GetLayoutParamsTrait(LayoutParamsType type) const;

public: // Layout Properties API
  void            SetViewMargin(const Extents& margin);
  Extents         GetViewMargin() const;
  void            SetViewPadding(const Extents& padding);
  Extents         GetViewPadding() const;
  void            SetHorizontalAlignment(LayoutAlignment alignment);
  LayoutAlignment GetHorizontalAlignment() const;
  void            SetVerticalAlignment(LayoutAlignment alignment);
  LayoutAlignment GetVerticalAlignment() const;

public: // Parent Layout API
  Ui::Layout   GetParentLayout() const;
  virtual bool IsLayout() const;

public: // LayoutManager API (Optional layout capability)
  /**
   * @brief Sets the layout manager for this view.
   *
   * When a LayoutManager is set, the view can manage children and
   * will delegate OnMeasure/OnArrange to the LayoutManager.
   *
   * @param[in] layoutManager The layout manager (ownership transferred)
   */
  void SetLayoutManager(LayoutManager* layoutManager);

  /**
   * @brief Gets the layout manager.
   *
   * @return Pointer to the layout manager, or nullptr if not set
   */
  LayoutManager* GetLayoutManager() const;

  /**
   * @brief Checks if this view has a layout manager.
   *
   * @return True if a layout manager is set
   */
  bool HasLayoutManager() const;

public: // Child Management API (available when LayoutManager is set)
  /**
   * @brief Adds a view to this view (at the end).
   *
   * The view will be managed by this view's layout manager and added to the Actor hierarchy.
   *
   * @param[in] view The view to add
   */
  void AddView(Ui::View view);

  /**
   * @brief Adds a view at the specified index.
   *
   * @param[in] view The view to add
   * @param[in] index The index where to insert
   */
  void AddView(Ui::View view, uint32_t index);

  /**
   * @brief Removes a view from this view.
   *
   * @param[in] view The view to remove
   */
  void RemoveView(Ui::View view);

  /**
   * @brief Removes the view at the specified index.
   *
   * @param[in] index The index of the view to remove
   */
  void RemoveViewAt(uint32_t index);

  /**
   * @brief Removes all child views from this view.
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
  Ui::View GetChildAt(uint32_t index) const;

  /**
   * @brief Returns the index of the given child view, or -1 if not found.
   *
   * @param[in] view The child view to find
   * @return Index of the view, or -1 if not a child
   */
  int32_t IndexOfChild(Ui::View view) const;

  /**
   * @brief Adds a list of children (method chaining).
   *
   * @param[in] children The initializer list of View handles to add
   * @return Reference to this for method chaining
   */
  ViewImpl& Contents(std::initializer_list<Ui::View> children);

  /**
   * @brief Gets the children container for layout manager access.
   *
   * @return Reference to the children container
   */
  ChildContainer& GetChildren();

  /**
   * @brief Gets the children container (const version).
   *
   * @return Const reference to the children container
   */
  const ChildContainer& GetChildren() const;

protected:
  /**
   * @brief Applies min/max constraints to the size.
   */
  MeasuredSize ApplyConstraints(const MeasuredSize& size) const;

  /**
   * @brief Registers this layout with the LayoutController for processing.
   *
   * Called when this view is a Layout Root (top of layout hierarchy).
   */
  void RegisterWithLayoutController();

private:
  // Not copyable or movable
  ViewImpl(const ViewImpl&)            = delete;
  ViewImpl(ViewImpl&&)                 = delete;
  ViewImpl& operator=(const ViewImpl&) = delete;
  ViewImpl& operator=(ViewImpl&&)      = delete;

private:
  std::vector<std::pair<TraitId, Trait>> mTraits;
  IInteractionTrait*                     mInteractionTrait;

  // Layout size (LayoutWidth / LayoutHeight)
  float mLayoutWidth;
  float mLayoutHeight;
  float mMinimumWidth;
  float mMinimumHeight;
  float mMaximumWidth;
  float mMaximumHeight;

  // Layout Properties Data
  Extents         mMargin;
  Extents         mPadding;
  LayoutAlignment mHorizontalAlignment;
  LayoutAlignment mVerticalAlignment;

  // Measure/Arrange State (cache-based)
  // mLastMeasuredConstraint.width < 0 means no valid measure cache
  MeasuredSize mDesiredSize;
  MeasuredSize mLastMeasuredConstraint;
  LayoutRect   mArrangedBounds;
  bool         mArrangeValid;

  // Optional LayoutManager and Children (for layout capability)
  std::unique_ptr<LayoutManager> mLayoutManager;
  ChildContainer                 mChildren;
};

// Helpers for public-api forwarding methods

inline Integration::ViewImpl& GetImpl(Ui::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<Integration::ViewImpl&>(handle);
}

inline const Integration::ViewImpl& GetImpl(const Ui::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const Integration::ViewImpl&>(handle);
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
