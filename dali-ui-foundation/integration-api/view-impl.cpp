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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali-ui-foundation/public-api/controls/control.h>
#include <dali-ui-foundation/public-api/layout.h>
#include <dali-ui-foundation/public-api/layout-controller.h>
#include <algorithm>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/integration-api/layout-manager.h>
#include <dali-ui-foundation/integration-api/layout-impl.h>
#include <dali-ui-foundation/internal/layout/layout-params-impl.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{

BaseHandle Create()
{
  return View::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(Ui::Integration::ViewImpl, Ui::Control, Create)
DALI_TYPE_REGISTRATION_END()

/**
 * @brief Checks if two float values are approximately equal.
 */
inline bool FloatEqual(float a, float b, float epsilon = 0.001f)
{
  return std::abs(a - b) < epsilon;
}

} // namespace

ViewImplPtr ViewImpl::New()
{
  return ViewImplPtr(new ViewImpl());
}

ViewImpl::ViewImpl()
  : Ui::Internal::Control(
        Ui::Internal::Control::ControlBehaviour(static_cast<int>(Ui::Internal::Control::CONTROL_BEHAVIOUR_DEFAULT) |
                                                static_cast<int>(Dali::CustomActorImpl::DISABLE_SIZE_NEGOTIATION))),
    mInteractionTrait(nullptr),
    mLayoutWidth(LayoutDimension::WrapContent),
    mLayoutHeight(LayoutDimension::WrapContent),
    mMinimumWidth(0.0f),
    mMinimumHeight(0.0f),
    mMaximumWidth(std::numeric_limits<float>::max()),
    mMaximumHeight(std::numeric_limits<float>::max()),
    mMargin(),
    mPadding(),
    mHorizontalAlignment(LayoutAlignment::START),
    mVerticalAlignment(LayoutAlignment::START),
    mDesiredSize{0.0f, 0.0f},
    mLastMeasuredConstraint{-1.0f, -1.0f},
    mArrangedBounds{0.0f, 0.0f, 0.0f, 0.0f},
    mArrangeValid(false)
{
}

ViewImpl::~ViewImpl()
{
  for (auto& iter : mTraits)
  {
    GetImpl(iter.second).OnViewDestroying(this);
  }

  // Unregister from LayoutController to prevent dangling pointer access.
  // Cannot call Self() here — DALi forbids it inside destructors (reference count == 1).
  if (HasLayoutManager())
  {
    LayoutController::UnregisterFromAll(this);
  }
}

void ViewImpl::OnInitialize()
{
  // Call base class initialization
  Ui::Internal::Control::OnInitialize();

  // Set default anchor point for layout positioning
  Self().SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  Self().SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
}

void ViewImpl::OnSceneConnection(int depth)
{
  Ui::Internal::Control::OnSceneConnection(depth);

  // When this view (layout root) is added to a window, ensure it is scheduled for layout.
  // This handles the case where invalidation occurred before the view was added to the window.
  if (IsLayout())
  {
    RegisterWithLayoutController();
  }
}

bool ViewImpl::OnKeyEvent(const Dali::KeyEvent& event)
{
  if (mInteractionTrait)
  {
    return mInteractionTrait->OnKeyEvent(View::DownCast(Self()), event);
  }
  return false;
}

void ViewImpl::OnKeyInputFocusGained()
{
  if (mInteractionTrait)
  {
    mInteractionTrait->OnFocusedChanged(View::DownCast(Self()), true);
  }
}

void ViewImpl::OnKeyInputFocusLost()
{
  if (mInteractionTrait)
  {
    mInteractionTrait->OnFocusedChanged(View::DownCast(Self()), false);
  }
}

void ViewImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  // dali-ui layout: View with LayoutManager is sized/positioned by LayoutController.
  // DALi size negotiation is disabled (DISABLE_SIZE_NEGOTIATION); this is a safety no-op.
  if (HasLayoutManager())
  {
    return;
  }
  Ui::Internal::Control::OnRelayout(size, container);
}

// =============================================================================
// API (size, position, parent origin, pivot)
// =============================================================================

float ViewImpl::GetSizeWidth() const
{
  return Self().GetProperty<float>(Actor::Property::SIZE_WIDTH);
}

void ViewImpl::SetSizeWidth(float width)
{
  Self().SetProperty(Actor::Property::SIZE_WIDTH, width);
}

float ViewImpl::GetSizeHeight() const
{
  return Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT);
}

void ViewImpl::SetSizeHeight(float height)
{
  Self().SetProperty(Actor::Property::SIZE_HEIGHT, height);
}

float ViewImpl::GetPositionX() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_X);
}

void ViewImpl::SetPositionX(float x)
{
  Self().SetProperty(Actor::Property::POSITION_X, x);
}

float ViewImpl::GetPositionY() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_Y);
}

void ViewImpl::SetPositionY(float y)
{
  Self().SetProperty(Actor::Property::POSITION_Y, y);
}

Vector3 ViewImpl::GetParentOrigin() const
{
  return Self().GetProperty<Vector3>(Actor::Property::PARENT_ORIGIN);
}

void ViewImpl::SetParentOrigin(const Vector3& point)
{
  Self().SetProperty(Actor::Property::PARENT_ORIGIN, point);
}

Vector3 ViewImpl::GetPivotPoint() const
{
  return Self().GetProperty<Vector3>(Actor::Property::ANCHOR_POINT);
}

void ViewImpl::SetPivotPoint(const Vector3& point)
{
  Self().SetProperty(Actor::Property::ANCHOR_POINT, point);
}

bool ViewImpl::IsFocusable() const
{
  return Self().GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE);
}

void ViewImpl::SetFocusable(bool focusable)
{
  Self().SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, focusable);
}

bool ViewImpl::IsTouchFocusable() const
{
  return Self().GetProperty<bool>(DevelActor::Property::TOUCH_FOCUSABLE);
}

void ViewImpl::SetTouchFocusable(bool touchFocusable)
{
  Self().SetProperty(DevelActor::Property::TOUCH_FOCUSABLE, touchFocusable);
}

void ViewImpl::SetTrait(TraitId id, Trait& trait)
{
  View self = View::DownCast(Self());
  auto& traitImpl = GetImpl(trait);

  if (id == ReservedTraitId::INTERACTION_TRAIT)
  {
    // NOTE Interaction trait는 한 번 설정되면 View 수명 동안 교체할 수 없음
    if (mInteractionTrait)
    {
      DALI_ASSERT_ALWAYS(false && "Interaction trait cannot be replaced once set");
      return;
    }

    IInteractionTrait* interactionTrait = dynamic_cast<IInteractionTrait*>(&traitImpl);
    DALI_ASSERT_ALWAYS(interactionTrait &&
                       "Trait for ReservedTraitId::INTERACTION_TRAIT must implement IInteractionTrait");
    mInteractionTrait = interactionTrait;
  }

  for (auto& entry : mTraits)
  {
    if (entry.first == id)
    {
      auto& oldTrait = entry.second;
      if (oldTrait == trait)
      {
        // Do nothing
        // The trait already exists with the same key
        return;
      }

      GetImpl(oldTrait).OnDetached(id, self);
      traitImpl.OnBeforeAttached(id, self);
      entry.second = trait;
      traitImpl.OnAttached(id, self);
      return;
    }
  }

  traitImpl.OnBeforeAttached(id, self);
  mTraits.emplace_back(id, trait);
  traitImpl.OnAttached(id, self);
}

Trait ViewImpl::GetTrait(TraitId id) const
{
  if (!mTraits.empty())
  {
    for (auto& entry : mTraits)
    {
      if (entry.first == id)
      {
        return entry.second;
      }
    }
  }
  return Trait();
}

bool ViewImpl::RemoveTrait(TraitId id)
{
  if (id == ReservedTraitId::INTERACTION_TRAIT)
  {
    // Interaction trait는 View 수명 동안 제거할 수 없다.
    DALI_ASSERT_ALWAYS(false && "Interaction trait cannot be removed once set");
    return false;
  }

  for (auto it = mTraits.begin(); it != mTraits.end(); ++it)
  {
    if (it->first == id)
    {
      View self = View::DownCast(Self());
      GetImpl(it->second).OnDetached(id, self);
      mTraits.erase(it);
      return true;
    }
  }
  return false;
}

// =============================================================================
// Measure / Arrange API
// =============================================================================

MeasuredSize ViewImpl::Measure(float widthConstraint, float heightConstraint)
{
  // Cache hit: already measured with same constraints
  if (mLastMeasuredConstraint.width >= 0.0f && FloatEqual(mLastMeasuredConstraint.width, widthConstraint) &&
      FloatEqual(mLastMeasuredConstraint.height, heightConstraint))
  {
    return mDesiredSize;
  }

  // Account for margin in constraints
  float marginWidth = static_cast<float>(mMargin.start + mMargin.end);
  float marginHeight = static_cast<float>(mMargin.top + mMargin.bottom);

  float innerWidthConstraint = std::max(0.0f, widthConstraint - marginWidth);
  float innerHeightConstraint = std::max(0.0f, heightConstraint - marginHeight);

  // Call virtual OnMeasure (Template Method pattern)
  MeasuredSize measured = OnMeasure(innerWidthConstraint, innerHeightConstraint);

  // Apply min/max constraints
  measured = ApplyConstraints(measured);

  // Store desired size (without margin - margin is applied during arrange)
  mDesiredSize = measured;

  // Update measure cache
  mLastMeasuredConstraint.width = widthConstraint;
  mLastMeasuredConstraint.height = heightConstraint;

  return mDesiredSize;
}

MeasuredSize ViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // If LayoutManager is set, delegate to it and add padding to the result.
  // LayoutManager returns content size; padding is part of this View's desired size.
  // For MatchParent dimensions, use the constraint so the view sizes to the parent (e.g. window).
  if (mLayoutManager)
  {
    // When this view has a fixed size, use it as the constraint for children
    // instead of the parent's constraint. A fixed-size container measures
    // children against its own size, not the space allocated by the parent.
    float managerWidthConstraint = (mLayoutWidth > 0) ? mLayoutWidth : widthConstraint;
    float managerHeightConstraint = (mLayoutHeight > 0) ? mLayoutHeight : heightConstraint;
    MeasuredSize content = mLayoutManager->Measure(this, managerWidthConstraint, managerHeightConstraint);
    float pw = static_cast<float>(mPadding.start + mPadding.end);
    float ph = static_cast<float>(mPadding.top + mPadding.bottom);
    float resultWidth;
    float resultHeight;
    if (mLayoutWidth == LayoutDimension::MatchParent)
    {
      resultWidth = widthConstraint;
    }
    else if (mLayoutWidth > 0)
    {
      // Fixed width: treat as total size (padding is inside, not added on top)
      resultWidth = mLayoutWidth;
    }
    else
    {
      resultWidth = content.width + pw;
    }
    if (mLayoutHeight == LayoutDimension::MatchParent)
    {
      resultHeight = heightConstraint;
    }
    else if (mLayoutHeight > 0)
    {
      // Fixed height: treat as total size (padding is inside, not added on top)
      resultHeight = mLayoutHeight;
    }
    else
    {
      resultHeight = content.height + ph;
    }
    return MeasuredSize(resultWidth, resultHeight);
  }

  // Default implementation for views without LayoutManager
  MeasuredSize size;

  float paddingWidth = static_cast<float>(mPadding.start + mPadding.end);
  float paddingHeight = static_cast<float>(mPadding.top + mPadding.bottom);

  // Determine width
  if (mLayoutWidth > 0)
  {
    // Fixed width: total size (padding is inside, not added on top)
    size.width = mLayoutWidth;
  }
  else if (mLayoutWidth == LayoutDimension::MatchParent)
  {
    // Match parent
    size.width = widthConstraint;
  }
  else
  {
    // Auto or Unset - use natural size + padding
    Vector3 naturalSize = Self().GetNaturalSize();
    size.width = ((naturalSize.width > 0) ? naturalSize.width : 0.0f) + paddingWidth;
  }

  // Determine height
  if (mLayoutHeight > 0)
  {
    // Fixed height: total size (padding is inside, not added on top)
    size.height = mLayoutHeight;
  }
  else if (mLayoutHeight == LayoutDimension::MatchParent)
  {
    // Match parent
    size.height = heightConstraint;
  }
  else
  {
    // Auto or Unset - use natural size + padding
    Vector3 naturalSize = Self().GetNaturalSize();
    size.height = ((naturalSize.height > 0) ? naturalSize.height : 0.0f) + paddingHeight;
  }

  return size;
}

MeasuredSize ViewImpl::Arrange(const LayoutRect& bounds)
{
  // Call virtual OnArrange (Template Method pattern)
  MeasuredSize arrangedSize = OnArrange(bounds);

  // Store arranged bounds
  mArrangedBounds = bounds;
  mArrangeValid = true;

  return arrangedSize;
}

MeasuredSize ViewImpl::OnArrange(const LayoutRect& bounds)
{
  // Use the bounds directly. The parent layout manager is responsible for
  // computing position and size based on the child's alignment properties.
  float x = bounds.x;
  float y = bounds.y;
  float width = bounds.width;
  float height = bounds.height;

  // Set actor position and size
  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, x);
  self.SetProperty(Actor::Property::POSITION_Y, y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, height);

  // If LayoutManager is set, arrange children
  if (mLayoutManager)
  {
    // Calculate content bounds (inside padding)
    LayoutRect contentBounds;
    contentBounds.x = static_cast<float>(mPadding.start);
    contentBounds.y = static_cast<float>(mPadding.top);
    contentBounds.width = width - static_cast<float>(mPadding.start + mPadding.end);
    contentBounds.height = height - static_cast<float>(mPadding.top + mPadding.bottom);

    mLayoutManager->ArrangeChildren(this, contentBounds);
  }

  return {width, height};
}

void ViewImpl::InvalidateMeasure()
{
  // Clear measure cache (always clear, no isValid guard)
  mLastMeasuredConstraint.width = -1.0f;
  mLastMeasuredConstraint.height = -1.0f;
  mArrangeValid = false;

  // Propagate to parent when present so its cache is cleared too.
  // Always propagate when parent exists: a child's size change affects parent measure/arrange.
  Ui::Layout parentLayout = GetParentLayout();
  if (parentLayout)
  {
    Integration::GetImpl(parentLayout).InvalidateMeasure();
    return;
  }

  // Reached a Layout Root: register with LayoutController for next layout pass
  if (IsLayout())
  {
    RegisterWithLayoutController();
  }
}

void ViewImpl::InvalidateArrange()
{
  mArrangeValid = false;

  // Propagate to layout root so ProcessLayouts runs (Arrange-only changes still need a pass).
  // Always propagate when parent exists: a child's change may require parent re-arrange.
  Ui::Layout parentLayout = GetParentLayout();
  if (parentLayout)
  {
    Integration::GetImpl(parentLayout).InvalidateArrange();
    return;
  }
  if (IsLayout())
  {
    RegisterWithLayoutController();
  }
}

void ViewImpl::RegisterWithLayoutController()
{
  // Find the window this view belongs to
  Actor self = Self();
  Window window = DevelWindow::Get(self);

  if (window)
  {
    // Get the LayoutController for this window and register this view
    LayoutController& controller = LayoutController::Get(window);
    controller.RequestLayout(this);
  }
}

MeasuredSize ViewImpl::GetDesiredSize() const
{
  return mDesiredSize;
}

void ViewImpl::SetDesiredSize(const MeasuredSize& size)
{
  mDesiredSize = size;
}

bool ViewImpl::IsMeasureValid() const
{
  return mLastMeasuredConstraint.width >= 0.0f;
}

bool ViewImpl::IsArrangeValid() const
{
  return mArrangeValid;
}

MeasuredSize ViewImpl::ApplyConstraints(const MeasuredSize& size) const
{
  MeasuredSize constrained = size;

  // Apply minimum constraints
  constrained.width = std::max(constrained.width, mMinimumWidth);
  constrained.height = std::max(constrained.height, mMinimumHeight);

  // Apply maximum constraints
  constrained.width = std::min(constrained.width, mMaximumWidth);
  constrained.height = std::min(constrained.height, mMaximumHeight);

  return constrained;
}

// =============================================================================
// Layout size API (LayoutWidth / LayoutHeight)
// =============================================================================

void ViewImpl::SetLayoutWidth(float width)
{
  if (!FloatEqual(mLayoutWidth, width))
  {
    mLayoutWidth = width;
    InvalidateMeasure();

    // For fixed size without LayoutManager, apply directly to Actor
    // (Parent layout will override this if View is managed by layout)
    if (width > 0 && !mLayoutManager)
    {
      Self().SetProperty(Actor::Property::SIZE_WIDTH, width);
    }
  }
}

float ViewImpl::GetLayoutWidth() const
{
  return mLayoutWidth;
}

void ViewImpl::SetLayoutHeight(float height)
{
  if (!FloatEqual(mLayoutHeight, height))
  {
    mLayoutHeight = height;
    InvalidateMeasure();

    // For fixed size without LayoutManager, apply directly to Actor
    // (Parent layout will override this if View is managed by layout)
    if (height > 0 && !mLayoutManager)
    {
      Self().SetProperty(Actor::Property::SIZE_HEIGHT, height);
    }
  }
}

float ViewImpl::GetLayoutHeight() const
{
  return mLayoutHeight;
}

void ViewImpl::SetMinimumWidth(float width)
{
  if (!FloatEqual(mMinimumWidth, width))
  {
    mMinimumWidth = width;
    InvalidateMeasure();
  }
}

float ViewImpl::GetMinimumWidth() const
{
  return mMinimumWidth;
}

void ViewImpl::SetMinimumHeight(float height)
{
  if (!FloatEqual(mMinimumHeight, height))
  {
    mMinimumHeight = height;
    InvalidateMeasure();
  }
}

float ViewImpl::GetMinimumHeight() const
{
  return mMinimumHeight;
}

void ViewImpl::SetMaximumWidth(float width)
{
  if (!FloatEqual(mMaximumWidth, width))
  {
    mMaximumWidth = width;
    InvalidateMeasure();
  }
}

float ViewImpl::GetMaximumWidth() const
{
  return mMaximumWidth;
}

void ViewImpl::SetMaximumHeight(float height)
{
  if (!FloatEqual(mMaximumHeight, height))
  {
    mMaximumHeight = height;
    InvalidateMeasure();
  }
}

float ViewImpl::GetMaximumHeight() const
{
  return mMaximumHeight;
}

// =============================================================================
// Layout Properties API
// =============================================================================

void ViewImpl::SetViewMargin(const Extents& margin)
{
  if (mMargin != margin)
  {
    mMargin = margin;
    InvalidateMeasure();
  }
}

Extents ViewImpl::GetViewMargin() const
{
  return mMargin;
}

void ViewImpl::SetViewPadding(const Extents& padding)
{
  if (mPadding != padding)
  {
    mPadding = padding;
    InvalidateMeasure();
  }
}

Extents ViewImpl::GetViewPadding() const
{
  return mPadding;
}

void ViewImpl::SetHorizontalAlignment(LayoutAlignment alignment)
{
  if (mHorizontalAlignment != alignment)
  {
    mHorizontalAlignment = alignment;
    InvalidateArrange();
  }
}

LayoutAlignment ViewImpl::GetHorizontalAlignment() const
{
  return mHorizontalAlignment;
}

void ViewImpl::SetVerticalAlignment(LayoutAlignment alignment)
{
  if (mVerticalAlignment != alignment)
  {
    mVerticalAlignment = alignment;
    InvalidateArrange();
  }
}

LayoutAlignment ViewImpl::GetVerticalAlignment() const
{
  return mVerticalAlignment;
}

// =============================================================================
// Parent Layout API
// =============================================================================

Ui::Layout ViewImpl::GetParentLayout() const
{
  Actor parent = Self().GetParent();
  if (parent)
  {
    return Ui::Layout::DownCast(parent);
  }
  return Ui::Layout();
}

bool ViewImpl::IsLayout() const
{
  // View is considered a layout if it has a LayoutManager
  return mLayoutManager != nullptr;
}

// =============================================================================
// LayoutManager API
// =============================================================================

void ViewImpl::SetLayoutManager(LayoutManager* layoutManager)
{
  mLayoutManager.reset(layoutManager);
  InvalidateMeasure();
}

LayoutManager* ViewImpl::GetLayoutManager() const
{
  return mLayoutManager.get();
}

bool ViewImpl::HasLayoutManager() const
{
  return mLayoutManager != nullptr;
}

// =============================================================================
// Child Management API
// =============================================================================

void ViewImpl::AddView(Ui::View view)
{
  if (!view)
  {
    return;
  }

  // Add to children container
  ChildData childData;
  childData.view = view;
  childData.measuredSize = {0.0f, 0.0f};
  childData.arrangedBounds = {0.0f, 0.0f, 0.0f, 0.0f};
  mChildren.push_back(childData);

  // Add to Actor hierarchy
  Self().Add(view);

  // Invalidate layout
  InvalidateMeasure();
}

void ViewImpl::AddView(Ui::View view, uint32_t index)
{
  if (!view)
  {
    return;
  }

  // Clamp index to valid range
  if (index > mChildren.size())
  {
    index = static_cast<uint32_t>(mChildren.size());
  }

  // Add to children container
  ChildData childData;
  childData.view = view;
  childData.measuredSize = {0.0f, 0.0f};
  childData.arrangedBounds = {0.0f, 0.0f, 0.0f, 0.0f};
  mChildren.insert(mChildren.begin() + index, childData);

  // Add to Actor hierarchy
  Self().Add(view);

  // Invalidate layout
  InvalidateMeasure();
}

void ViewImpl::RemoveView(Ui::View view)
{
  if (!view)
  {
    return;
  }

  // Find and remove from container
  auto it =
      std::find_if(mChildren.begin(), mChildren.end(), [&view](const ChildData& data) { return data.view == view; });

  if (it != mChildren.end())
  {
    mChildren.erase(it);

    // Remove from Actor hierarchy
    Self().Remove(view);

    // Invalidate layout
    InvalidateMeasure();
  }
}

void ViewImpl::RemoveViewAt(uint32_t index)
{
  if (index >= mChildren.size())
  {
    return;
  }

  Ui::View view = mChildren[index].view;
  mChildren.erase(mChildren.begin() + index);
  Self().Remove(view);
  InvalidateMeasure();
}

void ViewImpl::RemoveAllViews()
{
  // Remove all from Actor hierarchy
  for (auto& childData : mChildren)
  {
    Self().Remove(childData.view);
  }

  // Clear container
  mChildren.clear();

  // Invalidate layout
  InvalidateMeasure();
}

uint32_t ViewImpl::GetChildCount() const
{
  return static_cast<uint32_t>(mChildren.size());
}

Ui::View ViewImpl::GetChildAt(uint32_t index) const
{
  if (index < mChildren.size())
  {
    return mChildren[index].view;
  }
  return Ui::View();
}

int32_t ViewImpl::IndexOfChild(Ui::View view) const
{
  if (!view)
  {
    return -1;
  }
  for (size_t i = 0; i < mChildren.size(); ++i)
  {
    if (mChildren[i].view == view)
    {
      return static_cast<int32_t>(i);
    }
  }
  return -1;
}

Integration::ViewImpl& ViewImpl::Contents(std::initializer_list<Ui::View> children)
{
  for (const auto& child : children)
  {
    AddView(child);
  }
  return *this;
}

ViewImpl::ChildContainer& ViewImpl::GetChildren()
{
  return mChildren;
}

const ViewImpl::ChildContainer& ViewImpl::GetChildren() const
{
  return mChildren;
}

namespace
{

TraitId ToTraitId(LayoutParamsType type)
{
  switch (type)
  {
    case LayoutParamsType::ABSOLUTE:
      return TraitId(ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS);
    case LayoutParamsType::STACK:
      return TraitId(ReservedTraitId::STACK_LAYOUT_PARAMS);
    case LayoutParamsType::GRID:
      return TraitId(ReservedTraitId::GRID_LAYOUT_PARAMS);
    case LayoutParamsType::FLEX:
      return TraitId(ReservedTraitId::FLEX_LAYOUT_PARAMS);
  }
  DALI_ASSERT_ALWAYS(false && "Unknown LayoutParamsType");
  return TraitId(ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS); // unreachable
}

} // unnamed namespace

BaseHandle ViewImpl::GetLayoutParamsTrait(LayoutParamsType type) const
{
  return GetTrait(ToTraitId(type));
}

void ViewImpl::SetLayoutParams(Ui::LayoutParams params)
{
  auto& paramsImpl = static_cast<Internal::LayoutParamsImpl&>(Ui::GetImpl(params));

  SetTrait(paramsImpl.GetTraitId(), params);

  InvalidateMeasure();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
