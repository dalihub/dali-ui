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
#include <dali-ui-foundation/public-api/layout-controller.h>
#include <dali-ui-foundation/public-api/layout.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stack>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/integration-api/layout-impl.h>
#include <dali-ui-foundation/integration-api/layout-manager.h>
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/layout/layout-params-impl.h>
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/view/view-visual-data.h>
#include <dali-ui-foundation/internal/visuals/color/color-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/controls/control-depth-index-ranges.h>
#include <dali-ui-foundation/public-api/controls/control.h>
#include <dali-ui-foundation/public-api/controls/image-view/image-view.h>
#include <dali-ui-foundation/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

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
DALI_TYPE_REGISTRATION_BEGIN(Ui::Integration::ViewImpl, Ui::View, Create)
DALI_TYPE_REGISTRATION_END()

inline bool FloatEqual(float a, float b, float epsilon = 0.001f)
{
  return std::abs(a - b) < epsilon;
}

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

void CreateClippingRenderer(ViewImpl& viewImpl)
{
  Actor self(viewImpl.Self());
  int   clippingMode = ClippingMode::DISABLED;
  if(self.GetProperty(Actor::Property::CLIPPING_MODE).Get(clippingMode))
  {
    Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);
    if(clippingMode == ClippingMode::CLIP_CHILDREN &&
       (DALI_UNLIKELY(!viewDataImpl.mVisualData) || viewDataImpl.mVisualData->mVisuals.Empty()) &&
       self.GetRendererCount() == 0u)
    {
      viewImpl.SetBackgroundColor(Color::TRANSPARENT);
    }
  }
}

void RegisterViewAccessibleGetter()
{
  static bool onceFlag = false;
  if(DALI_UNLIKELY(!onceFlag))
  {
    onceFlag = true;
    Accessibility::Accessible::RegisterExternalAccessibleGetter(
      [](Dali::Actor actor) -> std::pair<std::shared_ptr<Accessibility::Accessible>, bool>
    {
      auto view = Ui::View::DownCast(actor);
      if(!view)
      {
        return {nullptr, true};
      }
      if(view.IsCreateAccessibleEnabled())
      {
        auto& viewImpl = Integration::GetImpl(view);
        return {std::shared_ptr<ViewAccessible>(viewImpl.CreateAccessibleObject()), true};
      }
      return {nullptr, false};
    });
  }
}

} // namespace

ViewImplPtr ViewImpl::New()
{
  IntrusivePtr<ViewImpl> viewImpl = new ViewImpl();
  return ViewImplPtr(viewImpl);
}

ViewImpl::ViewImpl()
: CustomActorImpl(static_cast<ActorFlags>(
    static_cast<int>(VIEW_BEHAVIOUR_DEFAULT) |
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
  mArrangeValid(false),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->mFlags = static_cast<Ui::Integration::ViewImpl::ViewBehaviour>(
    static_cast<int>(VIEW_BEHAVIOUR_DEFAULT) |
    static_cast<int>(Dali::CustomActorImpl::DISABLE_SIZE_NEGOTIATION));
}

ViewImpl::~ViewImpl()
{
  for(auto& iter : mTraits)
  {
    GetImpl(iter.second).OnViewDestroying(this);
  }

  if(HasLayoutManager())
  {
    LayoutController::UnregisterFromAll(this);
  }

  ClearRenderEffect();

  delete mImpl;
}

void ViewImpl::OnInitialize()
{
  Self().SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  Self().SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
}

void ViewImpl::OnSceneConnection(int depth)
{
  mImpl->OnSceneConnection();
  CreateClippingRenderer(*this);

  if(IsLayout())
  {
    RegisterWithLayoutController();
  }
}

bool ViewImpl::OnKeyEvent(const Dali::KeyEvent& event)
{
  if(mInteractionTrait)
  {
    return mInteractionTrait->OnKeyEvent(View::DownCast(Self()), event);
  }
  return false;
}

void ViewImpl::OnKeyInputFocusGained()
{
  EmitKeyInputFocusSignal(true);
  if(mInteractionTrait)
  {
    mInteractionTrait->OnFocusedChanged(View::DownCast(Self()), true);
  }
}

void ViewImpl::OnKeyInputFocusLost()
{
  EmitKeyInputFocusSignal(false);
  if(mInteractionTrait)
  {
    mInteractionTrait->OnFocusedChanged(View::DownCast(Self()), false);
  }
}

void ViewImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  if(HasLayoutManager())
  {
    return;
  }

  if((mImpl->mPadding.start != 0) || (mImpl->mPadding.end != 0) || (mImpl->mPadding.top != 0) ||
     (mImpl->mPadding.bottom != 0) || (mImpl->mMargin.start != 0) || (mImpl->mMargin.end != 0) ||
     (mImpl->mMargin.top != 0) || (mImpl->mMargin.bottom != 0))
  {
    for(unsigned int i = 0, numChildren = Self().GetChildCount(); i < numChildren; ++i)
    {
      Actor   child = Self().GetChildAt(i);
      Vector2 newChildSize(size);

      Extents padding = mImpl->mPadding;

      Dali::CustomActor           ownerActor(GetOwner());
      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(
        ownerActor.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());

      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      newChildSize.width  = size.width - (padding.start + padding.end);
      newChildSize.height = size.height - (padding.top + padding.bottom);

      Vector2 childOffset(0.f, 0.f);
      childOffset.x += (mImpl->mMargin.start + padding.start);
      childOffset.y += (mImpl->mMargin.top + padding.top);

      child.SetProperty(Actor::Property::POSITION, Vector2(childOffset.x, childOffset.y));

      container.Add(child, newChildSize);
    }
  }

  if(Accessibility::IsUp())
  {
    auto accessible = GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      auto highlightFrame = accessible->GetHighlightActor();
      if(accessible->GetCurrentlyHighlightedActor() == this->Self() &&
         highlightFrame.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
      {
        highlightFrame.SetProperty(Actor::Property::SIZE, size);
        container.Add(highlightFrame, size);
      }
    }
  }

  mImpl->ApplyFittingMode(size);
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

UiColor ViewImpl::GetBackgroundColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(View::DownCast(Self()), this, &ViewImpl::SetBackgroundColorInternal, outColor))
  {
    return outColor;
  }
  return mImpl->mBackgroundColor;
}

void ViewImpl::SetBackgroundColor(const UiColor& color)
{
  UiColorManager::Get().UpdateBinding(color, View::DownCast(Self()), this, &ViewImpl::SetBackgroundColorInternal);
  SetBackgroundColorInternal(color.Resolve());
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
  View  self      = View::DownCast(Self());
  auto& traitImpl = GetImpl(trait);

  if(id == ReservedTraitId::INTERACTION_TRAIT)
  {
    if(mInteractionTrait)
    {
      DALI_ASSERT_ALWAYS(false && "Interaction trait cannot be replaced once set");
      return;
    }
    IInteractionTrait* interactionTrait = dynamic_cast<IInteractionTrait*>(&traitImpl);
    DALI_ASSERT_ALWAYS(interactionTrait &&
                       "Trait for ReservedTraitId::INTERACTION_TRAIT must implement IInteractionTrait");
    mInteractionTrait = interactionTrait;
  }

  for(auto& entry : mTraits)
  {
    if(entry.first == id)
    {
      auto& oldTrait = entry.second;
      if(oldTrait == trait)
      {
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
  if(!mTraits.empty())
  {
    for(auto& entry : mTraits)
    {
      if(entry.first == id)
      {
        return entry.second;
      }
    }
  }
  return Trait();
}

bool ViewImpl::RemoveTrait(TraitId id)
{
  if(id == ReservedTraitId::INTERACTION_TRAIT)
  {
    DALI_ASSERT_ALWAYS(false && "Interaction trait cannot be removed once set");
    return false;
  }

  for(auto it = mTraits.begin(); it != mTraits.end(); ++it)
  {
    if(it->first == id)
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
  if(mLastMeasuredConstraint.width >= 0.0f && FloatEqual(mLastMeasuredConstraint.width, widthConstraint) &&
     FloatEqual(mLastMeasuredConstraint.height, heightConstraint))
  {
    return mDesiredSize;
  }

  float marginWidth           = static_cast<float>(mMargin.start + mMargin.end);
  float marginHeight          = static_cast<float>(mMargin.top + mMargin.bottom);
  float innerWidthConstraint  = std::max(0.0f, widthConstraint - marginWidth);
  float innerHeightConstraint = std::max(0.0f, heightConstraint - marginHeight);

  MeasuredSize measured          = OnMeasure(innerWidthConstraint, innerHeightConstraint);
  measured                       = ApplyConstraints(measured);
  mDesiredSize                   = measured;
  mLastMeasuredConstraint.width  = widthConstraint;
  mLastMeasuredConstraint.height = heightConstraint;

  return mDesiredSize;
}

MeasuredSize ViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  if(mLayoutManager)
  {
    float        managerWidthConstraint  = (mLayoutWidth > 0) ? mLayoutWidth : widthConstraint;
    float        managerHeightConstraint = (mLayoutHeight > 0) ? mLayoutHeight : heightConstraint;
    MeasuredSize content                 = mLayoutManager->Measure(this, managerWidthConstraint, managerHeightConstraint);
    float        pw                      = static_cast<float>(mPadding.start + mPadding.end);
    float        ph                      = static_cast<float>(mPadding.top + mPadding.bottom);
    float        resultWidth;
    float        resultHeight;
    if(mLayoutWidth == LayoutDimension::MatchParent)
    {
      resultWidth = widthConstraint;
    }
    else if(mLayoutWidth > 0)
    {
      resultWidth = mLayoutWidth;
    }
    else
    {
      resultWidth = content.width + pw;
    }
    if(mLayoutHeight == LayoutDimension::MatchParent)
    {
      resultHeight = heightConstraint;
    }
    else if(mLayoutHeight > 0)
    {
      resultHeight = mLayoutHeight;
    }
    else
    {
      resultHeight = content.height + ph;
    }
    return MeasuredSize(resultWidth, resultHeight);
  }

  MeasuredSize size;
  float        paddingWidth  = static_cast<float>(mPadding.start + mPadding.end);
  float        paddingHeight = static_cast<float>(mPadding.top + mPadding.bottom);

  if(mLayoutWidth > 0)
  {
    size.width = mLayoutWidth;
  }
  else if(mLayoutWidth == LayoutDimension::MatchParent)
  {
    size.width = widthConstraint;
  }
  else
  {
    Vector3 naturalSize = Self().GetNaturalSize();
    size.width          = ((naturalSize.width > 0) ? naturalSize.width : 0.0f) + paddingWidth;
  }

  if(mLayoutHeight > 0)
  {
    size.height = mLayoutHeight;
  }
  else if(mLayoutHeight == LayoutDimension::MatchParent)
  {
    size.height = heightConstraint;
  }
  else
  {
    Vector3 naturalSize = Self().GetNaturalSize();
    size.height         = ((naturalSize.height > 0) ? naturalSize.height : 0.0f) + paddingHeight;
  }

  return size;
}

MeasuredSize ViewImpl::Arrange(const LayoutRect& bounds)
{
  MeasuredSize arrangedSize = OnArrange(bounds);
  mArrangedBounds           = bounds;
  mArrangeValid             = true;
  return arrangedSize;
}

MeasuredSize ViewImpl::OnArrange(const LayoutRect& bounds)
{
  float x      = bounds.x;
  float y      = bounds.y;
  float width  = bounds.width;
  float height = bounds.height;

  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, x);
  self.SetProperty(Actor::Property::POSITION_Y, y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, height);

  if(mLayoutManager)
  {
    LayoutRect contentBounds;
    contentBounds.x      = static_cast<float>(mPadding.start);
    contentBounds.y      = static_cast<float>(mPadding.top);
    contentBounds.width  = width - static_cast<float>(mPadding.start + mPadding.end);
    contentBounds.height = height - static_cast<float>(mPadding.top + mPadding.bottom);
    mLayoutManager->ArrangeChildren(this, contentBounds);
  }

  return {width, height};
}

void ViewImpl::InvalidateMeasure()
{
  mLastMeasuredConstraint.width  = -1.0f;
  mLastMeasuredConstraint.height = -1.0f;
  mArrangeValid                  = false;

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    Integration::GetImpl(parentLayout).InvalidateMeasure();
    return;
  }

  if(IsLayout())
  {
    RegisterWithLayoutController();
  }
}

void ViewImpl::InvalidateArrange()
{
  mArrangeValid = false;

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    Integration::GetImpl(parentLayout).InvalidateArrange();
    return;
  }
  if(IsLayout())
  {
    RegisterWithLayoutController();
  }
}

void ViewImpl::RegisterWithLayoutController()
{
  Actor  self   = Self();
  Window window = DevelWindow::Get(self);
  if(window)
  {
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
  constrained.width        = std::max(constrained.width, mMinimumWidth);
  constrained.height       = std::max(constrained.height, mMinimumHeight);
  constrained.width        = std::min(constrained.width, mMaximumWidth);
  constrained.height       = std::min(constrained.height, mMaximumHeight);
  return constrained;
}

// =============================================================================
// Layout size API (LayoutWidth / LayoutHeight)
// =============================================================================

void ViewImpl::SetLayoutWidth(float width)
{
  if(!FloatEqual(mLayoutWidth, width))
  {
    mLayoutWidth = width;
    InvalidateMeasure();
    if(width > 0 && !mLayoutManager)
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
  if(!FloatEqual(mLayoutHeight, height))
  {
    mLayoutHeight = height;
    InvalidateMeasure();
    if(height > 0 && !mLayoutManager)
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
  if(!FloatEqual(mMinimumWidth, width))
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
  if(!FloatEqual(mMinimumHeight, height))
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
  if(!FloatEqual(mMaximumWidth, width))
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
  if(!FloatEqual(mMaximumHeight, height))
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
  if(mMargin != margin)
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
  if(mPadding != padding)
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
  if(mHorizontalAlignment != alignment)
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
  if(mVerticalAlignment != alignment)
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
  if(parent)
  {
    return Ui::Layout::DownCast(parent);
  }
  return Ui::Layout();
}

bool ViewImpl::IsLayout() const
{
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
  if(!view)
  {
    return;
  }
  ChildData childData;
  childData.view           = view;
  childData.measuredSize   = {0.0f, 0.0f};
  childData.arrangedBounds = {0.0f, 0.0f, 0.0f, 0.0f};
  mChildren.push_back(childData);
  Self().Add(view);
  InvalidateMeasure();
}

void ViewImpl::AddView(Ui::View view, uint32_t index)
{
  if(!view)
  {
    return;
  }
  if(index > mChildren.size())
  {
    index = static_cast<uint32_t>(mChildren.size());
  }
  ChildData childData;
  childData.view           = view;
  childData.measuredSize   = {0.0f, 0.0f};
  childData.arrangedBounds = {0.0f, 0.0f, 0.0f, 0.0f};
  mChildren.insert(mChildren.begin() + index, childData);
  Self().Add(view);
  InvalidateMeasure();
}

void ViewImpl::RemoveView(Ui::View view)
{
  if(!view)
  {
    return;
  }
  auto it =
    std::find_if(mChildren.begin(), mChildren.end(), [&view](const ChildData& data)
  { return data.view == view; });
  if(it != mChildren.end())
  {
    mChildren.erase(it);
    Self().Remove(view);
    InvalidateMeasure();
  }
}

void ViewImpl::RemoveViewAt(uint32_t index)
{
  if(index >= mChildren.size())
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
  for(auto& childData : mChildren)
  {
    Self().Remove(childData.view);
  }
  mChildren.clear();
  InvalidateMeasure();
}

uint32_t ViewImpl::GetChildCount() const
{
  return static_cast<uint32_t>(mChildren.size());
}

Ui::View ViewImpl::GetChildAt(uint32_t index) const
{
  if(index < mChildren.size())
  {
    return mChildren[index].view;
  }
  return Ui::View();
}

int32_t ViewImpl::IndexOfChild(Ui::View view) const
{
  if(!view)
  {
    return -1;
  }
  for(size_t i = 0; i < mChildren.size(); ++i)
  {
    if(mChildren[i].view == view)
    {
      return static_cast<int32_t>(i);
    }
  }
  return -1;
}

Integration::ViewImpl& ViewImpl::Contents(std::initializer_list<Ui::View> children)
{
  for(const auto& child : children)
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
  switch(type)
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
  return TraitId(ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS);
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

// =============================================================================
// From control-impl.cpp
// =============================================================================

ViewImpl::ViewImpl(ViewBehaviour behaviourFlags)
: CustomActorImpl(static_cast<ActorFlags>(behaviourFlags)),
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
  mArrangeValid(false),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->mFlags = static_cast<Ui::Integration::ViewImpl::ViewBehaviour>(behaviourFlags);
}

void ViewImpl::Initialize()
{
  if(!(mImpl->mFlags & Ui::View::DISABLE_VISUALS))
  {
    mImpl->InitializeVisualData();
  }

  RegisterViewAccessibleGetter();

  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  if(mImpl->mFlags & Ui::View::REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)
  {
    SetKeyboardNavigationSupport(true);
  }
}

void ViewImpl::SetBackgroundColorInternal(const Vector4& color)
{
  mImpl->mBackgroundColor = color;

  Property::Map map;
  map.Insert(Ui::Visual::Property::TYPE, Ui::Visual::COLOR);
  map.Insert(Ui::ColorVisual::Property::MIX_COLOR, color);

  Ui::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr && visualImplPtr->GetType() == Ui::Visual::COLOR)
  {
    // Update background color only
    visualImplPtr->DoAction(DevelVisual::Action::UPDATE_PROPERTY, map);
    return;
  }

  SetBackground(map);
}

void ViewImpl::SetBackground(const Property::Map& map)
{
  Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(map);
  visual.SetName("background");
  if(visual)
  {
    // Ignore corner radius for offscreen case.
    Ui::GetImplementation(visual).CornerRadiusIgnoredAtOffscreenRendering(true);
    mImpl->RegisterVisual(Ui::View::Property::BACKGROUND, visual, DepthIndex::BACKGROUND);
    mImpl->EnableCornerPropertiesOverridden(visual, true);

    // Trigger a size negotiation request that may be needed by the new visual to relayout its contents.
    RelayoutRequest();
  }
}

void ViewImpl::ClearBackground()
{
  mImpl->UnregisterVisual(Ui::View::Property::BACKGROUND);
  mImpl->mBackgroundColor = Color::TRANSPARENT;

  // Trigger a size negotiation request that may be needed when unregistering a visual.
  RelayoutRequest();
}

void ViewImpl::SetRenderEffect(Ui::RenderEffect effect)
{
  ClearRenderEffect();

  if(effect)
  {
    Internal::RenderEffectImpl* object = dynamic_cast<Internal::RenderEffectImpl*>(effect.GetObjectPtr());
    DALI_ASSERT_ALWAYS(object && "Given render effect is not valid.");

    Dali::Ui::View ownerView(GetOwner());
    object->SetOwnerView(ownerView);

    mImpl->mRenderEffect = object;
  }
  else
  {
    mImpl->mRenderEffect.Reset();
  }
}

RenderEffect ViewImpl::GetRenderEffect() const
{
  return RenderEffect(mImpl->mRenderEffect.Get());
}

void ViewImpl::ClearRenderEffect()
{
  if(mImpl->mRenderEffect)
  {
    Dali::Ui::Internal::RenderEffectImplPtr effectImpl = std::move(mImpl->mRenderEffect);

    // Reset handle first to avoid circular reference
    mImpl->mRenderEffect.Reset();

    effectImpl->ClearOwnerView();
  }
}

void ViewImpl::SetResourceReady()
{
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(*this);
  viewDataImpl.ResourceReady();
}

Internal::ViewDataImpl& ViewImpl::GetViewDataImpl() const
{
  return *mImpl;
}

Dali::Actor ViewImpl::GetOffScreenRenderableSourceActor()
{
  // Need to override this in FORWARD OffScreenRenderable
  return Dali::Actor();
}

bool ViewImpl::IsOffScreenRenderTaskExclusive()
{
  return false;
}

std::shared_ptr<Ui::ViewAccessible> ViewImpl::GetAccessibleObject()
{
  return mImpl->GetAccessibleObject();
}

void ViewImpl::EnableGestureDetection(GestureType::Value type)
{
  if((type & GestureType::PINCH) && !mImpl->mPinchGestureDetector)
  {
    mImpl->mPinchGestureDetector = PinchGestureDetector::New();
    mImpl->mPinchGestureDetector.DetectedSignal().Connect(mImpl, &Internal::ViewDataImpl::PinchDetected);
    mImpl->mPinchGestureDetector.Attach(Self());
  }

  if((type & GestureType::PAN) && !mImpl->mPanGestureDetector)
  {
    mImpl->mPanGestureDetector = PanGestureDetector::New();
    mImpl->mPanGestureDetector.SetMaximumTouchesRequired(2);
    mImpl->mPanGestureDetector.DetectedSignal().Connect(mImpl, &Internal::ViewDataImpl::PanDetected);
    mImpl->mPanGestureDetector.Attach(Self());
  }

  if((type & GestureType::TAP) && !mImpl->mTapGestureDetector)
  {
    mImpl->mTapGestureDetector = TapGestureDetector::New();
    mImpl->mTapGestureDetector.DetectedSignal().Connect(mImpl, &Internal::ViewDataImpl::TapDetected);
    mImpl->mTapGestureDetector.Attach(Self());
  }

  if((type & GestureType::LONG_PRESS) && !mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector = LongPressGestureDetector::New();
    mImpl->mLongPressGestureDetector.DetectedSignal().Connect(mImpl, &Internal::ViewDataImpl::LongPressDetected);
    mImpl->mLongPressGestureDetector.Attach(Self());
  }
}

void ViewImpl::DisableGestureDetection(GestureType::Value type)
{
  if((type & GestureType::PINCH) && mImpl->mPinchGestureDetector)
  {
    mImpl->mPinchGestureDetector.Detach(Self());
    mImpl->mPinchGestureDetector.Reset();
  }

  if((type & GestureType::PAN) && mImpl->mPanGestureDetector)
  {
    mImpl->mPanGestureDetector.Detach(Self());
    mImpl->mPanGestureDetector.Reset();
  }

  if((type & GestureType::TAP) && mImpl->mTapGestureDetector)
  {
    mImpl->mTapGestureDetector.Detach(Self());
    mImpl->mTapGestureDetector.Reset();
  }

  if((type & GestureType::LONG_PRESS) && mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector.Detach(Self());
    mImpl->mLongPressGestureDetector.Reset();
  }
}

PinchGestureDetector ViewImpl::GetPinchGestureDetector() const
{
  return mImpl->mPinchGestureDetector;
}

PanGestureDetector ViewImpl::GetPanGestureDetector() const
{
  return mImpl->mPanGestureDetector;
}

TapGestureDetector ViewImpl::GetTapGestureDetector() const
{
  return mImpl->mTapGestureDetector;
}

LongPressGestureDetector ViewImpl::GetLongPressGestureDetector() const
{
  return mImpl->mLongPressGestureDetector;
}

void ViewImpl::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool ViewImpl::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void ViewImpl::SetKeyInputFocus()
{
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Ui::KeyInputFocusManager::Get().SetFocus(Ui::View::DownCast(Self()));
  }
}

bool ViewImpl::HasKeyInputFocus()
{
  bool result = false;
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Ui::View view = Ui::KeyInputFocusManager::Get().GetCurrentFocusView();
    if(Self() == view)
    {
      result = true;
    }
  }
  return result;
}

void ViewImpl::ClearKeyInputFocus()
{
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Ui::KeyInputFocusManager::Get().RemoveFocus(Ui::View::DownCast(Self()));
  }
}

void ViewImpl::SetAsKeyboardFocusGroup(bool isFocusGroup)
{
  mImpl->mIsKeyboardFocusGroup = isFocusGroup;

  // The following line will be removed when the deprecated API in KeyboardFocusManager is deleted
  Ui::KeyboardFocusManager::Get().SetAsFocusGroup(Self(), isFocusGroup);
}

bool ViewImpl::IsKeyboardFocusGroup()
{
  return Ui::KeyboardFocusManager::Get().IsFocusGroup(Self());
}

void ViewImpl::KeyboardEnter()
{
  // Inform deriving classes
  OnKeyboardEnter();
}

bool ViewImpl::OnAccessibilityActivated()
{
  if(Ui::KeyboardFocusManager::Get().SetCurrentFocusActor(Self()))
  {
    return OnKeyboardEnter();
  }
  return false;
}

bool ViewImpl::OnKeyboardEnter()
{
  return false; // Keyboard enter is not handled by default
}

bool ViewImpl::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool ViewImpl::OnAccessibilityValueChange(bool isIncrease)
{
  return false; // Accessibility value change action is not handled by default
}

bool ViewImpl::OnAccessibilityZoom()
{
  return false; // Accessibility zoom action is not handled by default
}

ViewAccessible* ViewImpl::CreateAccessibleObject()
{
  return new ViewAccessible(Self());
}

Actor ViewImpl::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Ui::View::KeyboardFocus::Direction direction,
                                              bool loopEnabled)
{
  return Actor();
}

void ViewImpl::OnKeyboardFocusChangeCommitted(Actor committedFocusableActor)
{
}

Ui::View::KeyEventSignalType& ViewImpl::KeyEventSignal()
{
  return mImpl->mKeyEventSignal;
}

Ui::View::KeyInputFocusSignalType& ViewImpl::KeyInputFocusGainedSignal()
{
  return mImpl->mKeyInputFocusGainedSignal;
}

Ui::View::KeyInputFocusSignalType& ViewImpl::KeyInputFocusLostSignal()
{
  return mImpl->mKeyInputFocusLostSignal;
}

bool ViewImpl::EmitKeyEventSignal(const KeyEvent& event)
{
  // Guard against destruction during signal emission
  Dali::Ui::View handle(GetOwner());

  bool consumed = false;

  consumed = mImpl->FilterKeyEvent(event);

  // signals are allocated dynamically when someone connects
  if(!consumed && !mImpl->mKeyEventSignal.Empty())
  {
    consumed = mImpl->mKeyEventSignal.Emit(handle, event);
  }

  if(!consumed)
  {
    // Notification for derived classes
    consumed = OnKeyEvent(event);
  }

  return consumed;
}

Dali::Texture ViewImpl::GetOffScreenRenderingOutput() const
{
  if(mImpl->mOffScreenRenderingType != Ui::View::OffScreenRenderingType::REFRESH_ONCE)
  {
    DALI_LOG_ERROR(
      "Precondition unsatisfied: Set property OFFSCREEN_RENDERING to OffScreenRenderingType::REFRESH_ONCE\n");
    return Dali::Texture();
  }
  return mImpl->mOffScreenRenderingImpl->GetTexture();
}

void ViewImpl::EmitKeyInputFocusSignal(bool focusGained)
{
  Dali::Ui::View handle(GetOwner());

  if(Accessibility::IsUp())
  {
    auto accessible = GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      accessible->EmitFocused(focusGained);
      auto parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
      if(parent && !accessible->GetStates()[Dali::Accessibility::State::MANAGES_DESCENDANTS])
      {
        parent->EmitActiveDescendantChanged(accessible.get());
      }
    }
  }

  if(focusGained)
  {
    // signals are allocated dynamically when someone connects
    if(!mImpl->mKeyInputFocusGainedSignal.Empty())
    {
      mImpl->mKeyInputFocusGainedSignal.Emit(handle);
    }
  }
  else
  {
    // signals are allocated dynamically when someone connects
    if(!mImpl->mKeyInputFocusLostSignal.Empty())
    {
      mImpl->mKeyInputFocusLostSignal.Emit(handle);
    }
  }
}

void ViewImpl::OnSceneDisconnection()
{
  mImpl->OnSceneDisconnection();
}

void ViewImpl::OnChildAdd(Actor& child)
{
}

void ViewImpl::OnChildRemove(Actor& child)
{
}

void ViewImpl::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  // If the clipping mode has been set, we may need to create a renderer.
  // Only do this if we are already on-stage as the OnSceneConnection will handle the off-stage clipping controls.
  switch(index)
  {
    case Actor::Property::CLIPPING_MODE:
    {
      if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
      {
        // Note: This method will handle whether creation of the renderer is required.
        CreateClippingRenderer(*this);
      }
      break;
    }
    case DevelActor::Property::USER_INTERACTION_ENABLED:
    {
      const bool enabled = propertyValue.Get<bool>();
      if(!enabled && Self() == Dali::Ui::KeyboardFocusManager::Get().GetCurrentFocusActor())
      {
        Dali::Ui::KeyboardFocusManager::Get().ClearFocus();
      }
      break;
    }
  }
}

void ViewImpl::OnSizeSet(const Vector3& targetSize)
{
  Vector2 size(targetSize);

  Ui::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr)
  {
    visualImplPtr->SetControlSize(size); // Send an empty map as we do not want to modify the visual's set transform
  }

  // Apply FittingMode here
  mImpl->mSize = Vector2(targetSize);
  mImpl->RegisterProcessorOnce();

  // Refresh render effects
  if(mImpl->mRenderEffect)
  {
    mImpl->mRenderEffect->Refresh();
  }

  if(mImpl->mOffScreenRenderingImpl)
  {
    mImpl->mOffScreenRenderingImpl->Refresh();
  }
}

void ViewImpl::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?

  // TODO : Could we clear animation constraint when size animation stopped?
  mImpl->CreateAnimationConstraints(animation.GetBaseObject(), Dali::Actor::Property::SIZE);
}

void ViewImpl::OnAnimateAnimatableProperty(Animation& animation, Property::Index index, Animation::State state)
{
  if(state == Animation::State::PLAYING)
  {
    mImpl->CreateAnimationConstraints(animation.GetBaseObject(), index);
  }
  else if(state == Animation::State::STOPPED)
  {
    mImpl->ClearAnimationConstraints(animation.GetBaseObject(), index);
  }
}

void ViewImpl::OnConstraintAnimatableProperty(Constraint& constraint, Property::Index index, bool applied)
{
  if(applied)
  {
    mImpl->CreateAnimationConstraints(constraint.GetBaseObject(), index);
  }
  else
  {
    mImpl->ClearAnimationConstraints(constraint.GetBaseObject(), index);
  }
}

void ViewImpl::GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward)
{
  if(mImpl->mRenderEffect)
  {
    mImpl->mRenderEffect->GetOffScreenRenderTasks(tasks, isForward);
  }
  if(mImpl->mOffScreenRenderingImpl)
  {
    mImpl->mOffScreenRenderingImpl->GetOffScreenRenderTasks(tasks, isForward);
  }
}

bool ViewImpl::IsResourceReady() const
{
  const Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(*this);
  return viewDataImpl.IsResourceReady();
}

void ViewImpl::OnPinch(const PinchGesture& pinch)
{
  if(!(mImpl->mStartingPinchScale))
  {
    // lazy allocate
    mImpl->mStartingPinchScale = new Vector3;
  }

  if(pinch.GetState() == GestureState::STARTED)
  {
    *(mImpl->mStartingPinchScale) = Self().GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  }
  Self().SetProperty(Actor::Property::SCALE, *(mImpl->mStartingPinchScale) * pinch.GetScale());
}

void ViewImpl::OnPan(const PanGesture& pan)
{
}

void ViewImpl::OnTap(const TapGesture& tap)
{
}

void ViewImpl::OnLongPress(const LongPressGesture& longPress)
{
}

void ViewImpl::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 ViewImpl::GetNaturalSize()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "ViewImpl::GetNaturalSize for %s\n",
                Self().GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr());
  Ui::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr)
  {
    Vector2 naturalSize;
    visualImplPtr->GetNaturalSize(naturalSize);
    naturalSize.width += (mImpl->mPadding.start + mImpl->mPadding.end);
    naturalSize.height += (mImpl->mPadding.top + mImpl->mPadding.bottom);
    return Vector3(naturalSize);
  }
  return Vector3::ZERO;
}

float ViewImpl::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return CalculateChildSizeBase(child, dimension);
}

float ViewImpl::GetHeightForWidth(float width)
{
  return GetHeightForWidthBase(width);
}

float ViewImpl::GetWidthForHeight(float height)
{
  return GetWidthForHeightBase(height);
}

bool ViewImpl::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return RelayoutDependentOnChildrenBase(dimension);
}

void ViewImpl::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void ViewImpl::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
}

void ViewImpl::RelayoutRequestToView()
{
  RelayoutRequest();
}

void ViewImpl::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalConnected(slotObserver, callback);
}

void ViewImpl::SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalDisconnected(slotObserver, callback);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
