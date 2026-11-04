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

// CLASS HEADER
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <limits>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/internal/layouts/measure-mode.h>
#include <dali-ui-foundation/internal/layouts/stack-layout-params-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

namespace
{

float GetChildWeight(ViewImpl& childImpl)
{
  auto* params = Internal::StackLayoutParamsImpl::Get(childImpl);
  return params ? params->GetWeight() : 0.0f;
}

LayoutAlignment GetChildAlignment(ViewImpl& childImpl)
{
  auto* params = Internal::StackLayoutParamsImpl::Get(childImpl);
  return params ? params->GetAlignment() : LayoutAlignment::START;
}

bool IsChildStandalone(ViewImpl& childImpl)
{
  return childImpl.GetLayoutMode() == LayoutMode::STANDALONE;
}

using Internal::MeasureMode;
using Internal::MeasureModeCheck;

/**
 * The measure-mode facts of one stack pass, read once from the stack's own view.
 *
 * modeAware is false for every view whose producer is not exactly this manager (a subclass,
 * or a view with a layout callback): such a stack measures every child in one step with the
 * custom-parent modes of its view (ViewDataImpl::DeriveChildModesAsCustomParent), today's
 * numbers, and never skips a re-measure. crossDetermined tells whether the cross size is
 * decided before the children are measured (an explicit or MATCH_PARENT cross axis, or a
 * WRAP_CONTENT one measured EXACTLY).
 */
struct StackModeContext
{
  bool                          modeAware{false};
  bool                          crossDetermined{true};
  const Internal::ViewDataImpl* data{nullptr}; ///< The stack's own view data
};

StackModeContext MakeStackModeContext(const Internal::ViewDataImpl& viewData, StackOrientation orientation)
{
  StackModeContext context;
  context.data      = &viewData;
  context.modeAware = (viewData.GetMeasureModeParent() == Internal::MeasureModeParent::STACK);
  if(context.modeAware)
  {
    context.crossDetermined = (orientation == StackOrientation::VERTICAL) ? viewData.IsWidthDetermined() : viewData.IsHeightDetermined();
  }
  return context;
}

// The modes a stack that is not mode-aware passes a child: the custom-parent rule of its view,
// which is also what the child's own derivation gives.
Internal::MeasureModes LegacyStackChildModes(const StackModeContext& context, ViewImpl& childImpl)
{
  return context.data->DeriveChildModesAsCustomParent(childImpl.GetRequestedWidth(), childImpl.GetRequestedHeight());
}

bool IsCrossMatchParent(ViewImpl& childImpl, StackOrientation orientation)
{
  return ((orientation == StackOrientation::VERTICAL) ? childImpl.GetRequestedWidth() : childImpl.GetRequestedHeight()) == MATCH_PARENT;
}

// A WRAP_CONTENT cross axis with FILL alignment: Arrange stretches it to the cross extent.
bool IsCrossStretched(ViewImpl& childImpl, StackOrientation orientation)
{
  return ((orientation == StackOrientation::VERTICAL) ? childImpl.GetRequestedWidth() : childImpl.GetRequestedHeight()) == WRAP_CONTENT &&
         GetChildAlignment(childImpl) == LayoutAlignment::FILL;
}

// The cross mode of a child whose cross extent this stack decides: EXACTLY once the extent is
// determined, AT_MOST before (a probe for MATCH_PARENT; the extent is resolved first).
MeasureMode StackCrossMode(const StackModeContext& context, ViewImpl& childImpl, StackOrientation orientation)
{
  if(context.modeAware && context.crossDetermined &&
     (IsCrossMatchParent(childImpl, orientation) || IsCrossStretched(childImpl, orientation)))
  {
    return MeasureMode::EXACTLY;
  }
  return MeasureMode::AT_MOST;
}

// The cross mode of a child at its FINAL slot (Arrange, and what DeriveMeasureModes describes):
// MATCH_PARENT is always assigned; a FILL WRAP_CONTENT child only when the cross extent is determined.
MeasureMode StackSlotCrossMode(const StackModeContext& context, ViewImpl& childImpl, StackOrientation orientation)
{
  if(IsCrossMatchParent(childImpl, orientation) || (context.crossDetermined && IsCrossStretched(childImpl, orientation)))
  {
    return MeasureMode::EXACTLY;
  }
  return MeasureMode::AT_MOST;
}

MeasuredSize MeasureStackChild(ViewImpl& childImpl, float width, float height, MeasureMode widthMode, MeasureMode heightMode, MeasureModeCheck check)
{
  return Internal::ViewDataImpl::Get(childImpl).Measure(width, height, widthMode, heightMode, check);
}

// The arrange-time re-measure of a MATCH_PARENT child at its final slot. A mode-aware stack
// passes the modes its derivation describes -- cross: MATCH_PARENT, or FILL on a determined cross
// extent, is assigned; main: a weight share is assigned, a MATCH_PARENT main axis without weight
// stays AT_MOST -- and skips the call when the child already holds a measurement for exactly
// this slot. A stack that is not mode-aware re-measures with the custom-parent modes of its
// view, never skipped.
void RemeasureStackChildForSlot(ViewImpl* owner, ViewImpl& childImpl, const LayoutRect& childBounds, StackOrientation orientation, const StackModeContext& context)
{
  Internal::ViewDataImpl& childData = Internal::ViewDataImpl::Get(childImpl);
  if(!context.modeAware)
  {
    const Internal::MeasureModes                         legacy = LegacyStackChildModes(context, childImpl);
    Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(owner);
    childData.Measure(childBounds.width, childBounds.height, legacy.width, legacy.height, MeasureModeCheck::MATCHES_DERIVATION);
    return;
  }

  const bool        vertical   = (orientation == StackOrientation::VERTICAL);
  const MeasureMode crossMode  = StackSlotCrossMode(context, childImpl, orientation);
  const MeasureMode mainMode   = (GetChildWeight(childImpl) > 0.0f) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
  const MeasureMode widthMode  = vertical ? crossMode : mainMode;
  const MeasureMode heightMode = vertical ? mainMode : crossMode;
  if(!childData.CanSkipArrangeRemeasure(childBounds.width, childBounds.height, widthMode, heightMode))
  {
    Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(owner);
    childData.Measure(childBounds.width, childBounds.height, widthMode, heightMode, MeasureModeCheck::MATCHES_DERIVATION);
  }
}

struct StackMeasureFirstPassResult
{
  float    mainAxisNonWeight{0.0f};
  float    maxCrossAxis{0.0f};
  float    ownCrossAxis{0.0f}; ///< maxCrossAxis over the non-weight children that are not MATCH_PARENT on the cross axis
  float    totalWeight{0.0f};
  uint32_t visibleChildCount{0};
};

StackMeasureFirstPassResult MeasureStackNonWeightChildren(std::vector<View>&         children,
                                                          std::vector<MeasuredSize>& workingSizes,
                                                          float                      contentWidth,
                                                          float                      contentHeight,
                                                          StackOrientation           orientation,
                                                          const StackModeContext&    context)
{
  StackMeasureFirstPassResult result;
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);

    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    result.visibleChildCount++;
    float weight = GetChildWeight(childImpl);
    if(weight > 0.0f)
    {
      result.totalWeight += weight;
      // On an undetermined cross size a MATCH_PARENT-cross weight child is probed here, at today's
      // budget on both axes, so the cross size includes what it reports -- a producer reporting more
      // than its minimum widens the stack exactly as before. The weight phase measures it exactly at
      // the resolved slot.
      if(context.modeAware && !context.crossDetermined && IsCrossMatchParent(childImpl, orientation))
      {
        const float        probeScale  = childImpl.GetEffectiveScale();
        const Insets       probeMargin = childImpl.GetMargin();
        const float        probeW      = static_cast<float>(probeMargin.start + probeMargin.end) * probeScale;
        const float        probeH      = static_cast<float>(probeMargin.top + probeMargin.bottom) * probeScale;
        const MeasuredSize probe       = MeasureStackChild(childImpl, std::max(0.0f, contentWidth - probeW), std::max(0.0f, contentHeight - probeH), MeasureMode::AT_MOST, MeasureMode::AT_MOST, MeasureModeCheck::NONE);
        result.maxCrossAxis            = std::max(result.maxCrossAxis, (orientation == StackOrientation::VERTICAL) ? probe.width + probeW : probe.height + probeH);
      }
      continue;
    }
    float  childScale            = childImpl.GetEffectiveScale();
    Insets margin                = childImpl.GetMargin();
    float  marginW               = static_cast<float>(margin.start + margin.end) * childScale;
    float  marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
    float  childWidthConstraint  = std::max(0.0f, contentWidth - marginW);
    float  childHeightConstraint = std::max(0.0f, contentHeight - marginH);

    // Main axis: AT_MOST for every non-weight child, MATCH_PARENT included -- the stack sums
    // these main extents. Cross axis: assigned once determined; on an undetermined cross extent a
    // MATCH_PARENT child is a probe (it reports what it always did, so the extent below is
    // unchanged) and is re-measured exactly by the caller once the extent is resolved.
    const bool       vertical   = (orientation == StackOrientation::VERTICAL);
    MeasureMode      widthMode  = MeasureMode::AT_MOST;
    MeasureMode      heightMode = MeasureMode::AT_MOST;
    MeasureModeCheck check      = MeasureModeCheck::MATCHES_DERIVATION;
    if(context.modeAware)
    {
      const MeasureMode crossMode = StackCrossMode(context, childImpl, orientation);
      widthMode                   = vertical ? crossMode : MeasureMode::AT_MOST;
      heightMode                  = vertical ? MeasureMode::AT_MOST : crossMode;
      // A MATCH_PARENT cross axis on an undetermined cross size is a probe.
      if(IsCrossMatchParent(childImpl, orientation) && !context.crossDetermined)
      {
        check = MeasureModeCheck::NONE;
      }
    }
    else
    {
      const Internal::MeasureModes legacy = LegacyStackChildModes(context, childImpl);
      widthMode                           = legacy.width;
      heightMode                          = legacy.height;
    }
    MeasuredSize childSize = MeasureStackChild(childImpl, childWidthConstraint, childHeightConstraint, widthMode, heightMode, check);
    workingSizes[i]        = childSize;
    const float childCross = (orientation == StackOrientation::VERTICAL) ? childSize.width + marginW : childSize.height + marginH;
    if(orientation == StackOrientation::VERTICAL)
    {
      result.mainAxisNonWeight += childSize.height + marginH;
      result.maxCrossAxis = std::max(result.maxCrossAxis, childSize.width + marginW);
    }
    else
    {
      result.mainAxisNonWeight += childSize.width + marginW;
      result.maxCrossAxis = std::max(result.maxCrossAxis, childSize.height + marginH);
    }
    if(!IsCrossMatchParent(childImpl, orientation))
    {
      result.ownCrossAxis = std::max(result.ownCrossAxis, childCross);
    }
  }
  return result;
}

// @param[in] context    The stack's measure-mode facts
// @param[in] slotCross  The cross content extent the stack resolved from its non-weight children;
//                       read only for a MATCH_PARENT-cross child of a mode-aware stack whose
//                       cross extent is not determined
void MeasureStackWeightChildren(std::vector<View>& children, std::vector<MeasuredSize>& workingSizes,
                                float contentMain, float contentWidth,
                                float contentHeight, float mainAxisNonWeight, float totalWeight,
                                uint32_t visibleChildCount, float spacing, StackOrientation orientation,
                                float& maxCrossAxisInOut, const StackModeContext& context, float slotCross)
{
  float spacingTotal  = (visibleChildCount > 1) ? spacing * (visibleChildCount - 1) : 0.0f;
  float remainingMain = contentMain - mainAxisNonWeight - spacingTotal;
  remainingMain       = std::max(0.0f, remainingMain);

  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);
    if(IsChildStandalone(childImpl))
    {
      continue;
    }
    float weight = GetChildWeight(childImpl);
    if(weight <= 0.0f)
    {
      continue;
    }

    float  share              = (weight / totalWeight) * remainingMain;
    float  childScale         = childImpl.GetEffectiveScale();
    Insets margin             = childImpl.GetMargin();
    float  marginH            = static_cast<float>(margin.top + margin.bottom) * childScale;
    float  marginW            = static_cast<float>(margin.start + margin.end) * childScale;
    float  mainAxisConstraint = std::max(0.0f, share - (orientation == StackOrientation::VERTICAL ? marginH : marginW));
    float  childWidthConstraint =
      (orientation == StackOrientation::VERTICAL) ? std::max(0.0f, contentWidth - marginW) : mainAxisConstraint;
    float childHeightConstraint =
      (orientation == StackOrientation::VERTICAL) ? mainAxisConstraint : std::max(0.0f, contentHeight - marginH);

    // The share is assigned (main EXACTLY). The cross axis follows the stack's cross rule; a
    // MATCH_PARENT cross axis of a stack whose cross extent is not determined is assigned the extent
    // the non-weight children resolved -- unless that slot is empty: the child is never measured
    // exactly there, its cross axis keeps the budget (the first-pass probe's number), at most.
    MeasureMode      widthMode  = MeasureMode::AT_MOST;
    MeasureMode      heightMode = MeasureMode::AT_MOST;
    MeasureModeCheck check      = MeasureModeCheck::MATCHES_DERIVATION;
    if(context.modeAware)
    {
      const bool  vertical  = (orientation == StackOrientation::VERTICAL);
      MeasureMode crossMode = StackSlotCrossMode(context, childImpl, orientation);
      if(IsCrossMatchParent(childImpl, orientation) && !context.crossDetermined)
      {
        float&      crossConstraint = vertical ? childWidthConstraint : childHeightConstraint;
        const float crossSlot       = Internal::SnapEmptySlot(slotCross - (vertical ? marginW : marginH));
        if(crossSlot > 0.0f)
        {
          crossConstraint = crossSlot;
        }
        else
        {
          crossMode = MeasureMode::AT_MOST;
          check     = MeasureModeCheck::NONE;
        }
      }
      widthMode  = vertical ? crossMode : MeasureMode::EXACTLY;
      heightMode = vertical ? MeasureMode::EXACTLY : crossMode;
    }
    else
    {
      const Internal::MeasureModes legacy = LegacyStackChildModes(context, childImpl);
      widthMode                           = legacy.width;
      heightMode                          = legacy.height;
    }
    MeasuredSize childSize = MeasureStackChild(childImpl, childWidthConstraint, childHeightConstraint, widthMode, heightMode, check);
    if(orientation == StackOrientation::VERTICAL)
    {
      workingSizes[i].width  = childSize.width;
      workingSizes[i].height = std::max(0.0f, share - marginH);
      maxCrossAxisInOut      = std::max(maxCrossAxisInOut, childSize.width + marginW);
    }
    else
    {
      workingSizes[i].width  = std::max(0.0f, share - marginW);
      workingSizes[i].height = childSize.height;
      maxCrossAxisInOut      = std::max(maxCrossAxisInOut, childSize.height + marginH);
    }
  }
}

} // namespace

class StackLayoutManager::Impl : public LayoutManager::Impl
{
public:
  Impl(StackOrientation orientation, float spacing)
  : mOrientation(orientation),
    mSpacing(spacing)
  {
  }

  StackOrientation mOrientation;
  float            mSpacing;
};

StackLayoutManager::StackLayoutManager(StackOrientation orientation, float spacing)
: LayoutManager(new Impl(orientation, spacing))
{
}

StackLayoutManager::~StackLayoutManager()
{
}

void StackLayoutManager::SetOrientation(StackOrientation orientation)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mOrientation == orientation)
  {
    return;
  }
  impl->mOrientation = orientation;

  // This state is read by both Measure() and Arrange(), and neither the measure cache
  // key nor the arrange cache key can see it, so the owner has to be told. Reaching
  // this manager directly (through the protected View::GetLayoutManager()) used to
  // leave the change unscheduled; now it invalidates exactly as StackLayout's own
  // setter does. This also keeps ArrangePolicy::IF_CHANGED valid: the orientation is
  // layout-tracked precisely because of this call.
  InvalidateOwnerMeasure();
}

StackOrientation StackLayoutManager::GetOrientation() const
{
  return GetImplAs<Impl>()->mOrientation;
}

void StackLayoutManager::SetSpacing(float spacing)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mSpacing == spacing)
  {
    return;
  }
  impl->mSpacing = spacing;

  // Spacing changes the accumulated main-axis extent, so it is a MEASURE input.
  // See SetOrientation for why the owner has to be invalidated from here.
  InvalidateOwnerMeasure();
}

float StackLayoutManager::GetSpacing() const
{
  return GetImplAs<Impl>()->mSpacing;
}

MeasuredSize StackLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto* impl = GetImplAs<Impl>();

  float s           = view->GetEffectiveScale();
  float visSpacing  = impl->mSpacing * s;
  float contentMain = (impl->mOrientation == StackOrientation::VERTICAL) ? heightConstraint : widthConstraint;

  // Collect children once and pass to helpers (helpers run twice).
  const uint32_t    count = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(count);
  for(uint32_t i = 0; i < count; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  // Local working buffer (seed later from child measurements): weight
  // distribution writes allocation values here without persisting on the
  // child, so repeated layout passes do not accumulate.
  std::vector<MeasuredSize> workingSizes(children.size());

  // Measure modes: only a view whose producer is exactly this manager is mode-aware (a
  // subclass or a view with a layout callback passes the custom-parent modes in one step).
  const bool             vertical = (impl->mOrientation == StackOrientation::VERTICAL);
  const StackModeContext context  = MakeStackModeContext(Internal::ViewDataImpl::Get(*view), impl->mOrientation);

  StackMeasureFirstPassResult first = MeasureStackNonWeightChildren(
    children, workingSizes, widthConstraint, heightConstraint, impl->mOrientation, context);

  // Phase 1b, only when the cross extent was not determined: the non-weight children (and the
  // probed MATCH_PARENT-cross weight children) have resolved it, so the children whose cross
  // extent the stack assigns are re-measured EXACTLY at the slot Arrange will give them -- every
  // MATCH_PARENT-cross child (measured as a probe above), and a FILL WRAP_CONTENT child narrower
  // than the slot when its content's size depends on its cross size (HasSizeDependentContent;
  // widening a child whose content ignores it changes nothing but its own report). Their new main
  // extents replace their contributions to the main sum; the cross aggregate is final.
  float slotCross = 0.0f;
  if(context.modeAware && !context.crossDetermined)
  {
    const Insets padding  = view->GetPadding();
    const float  padCross = vertical ? static_cast<float>(padding.start + padding.end) * s
                                     : static_cast<float>(padding.top + padding.bottom) * s;
    const float  minCross = (vertical ? view->GetMinimumWidth() : view->GetMinimumHeight()) * s;
    const float  maxCross = (vertical ? view->GetMaximumWidth() : view->GetMaximumHeight()) * s;
    // The content extent this stack is laid out at when its parent places it at its measured
    // size: the measure dispatch adds the padding, Measure() clamps by the own min/max, and
    // the arrange dispatch takes the padding off again. Only a stack with a cross size of its
    // own -- what its children that are not MATCH_PARENT there and its own minimum give it, not its
    // MATCH_PARENT children's margins or reports -- hands out a slot: otherwise every
    // MATCH_PARENT-cross child keeps its probe, whatever its margin.
    const bool hasOwnCross = Internal::SnapEmptySlot(std::min(std::max(first.ownCrossAxis + padCross, minCross), maxCross) - padCross) > 0.0f;
    slotCross              = hasOwnCross ? std::max(0.0f, std::min(std::max(first.maxCrossAxis + padCross, minCross), maxCross) - padCross) : 0.0f;

    for(uint32_t i = 0; i < children.size(); ++i)
    {
      ViewImpl& childImpl = GetImpl(children[i]);
      if(IsChildStandalone(childImpl) || GetChildWeight(childImpl) > 0.0f)
      {
        continue;
      }

      float       childScale = childImpl.GetEffectiveScale();
      Insets      margin     = childImpl.GetMargin();
      float       marginW    = static_cast<float>(margin.start + margin.end) * childScale;
      float       marginH    = static_cast<float>(margin.top + margin.bottom) * childScale;
      const float crossSlot  = Internal::SnapEmptySlot(slotCross - (vertical ? marginW : marginH));
      const bool  matchCross = IsCrossMatchParent(childImpl, impl->mOrientation);
      const float step1Cross = vertical ? workingSizes[i].width : workingSizes[i].height;
      // A MATCH_PARENT cross axis is never measured exactly at an empty slot: nothing gives the stack
      // a cross size of its own, so the step-1 probe (the child's at-most report) stands.
      const bool exactMatch  = matchCross && crossSlot > 0.0f;
      const bool widenedFill = !matchCross && Internal::IsNarrowerThanSlot(step1Cross, crossSlot) && IsCrossStretched(childImpl, impl->mOrientation) &&
                               Internal::ViewDataImpl::Get(childImpl).HasSizeDependentContent(vertical);
      if(!exactMatch && !widenedFill)
      {
        continue;
      }

      // The main axis keeps its step-1 number and mode.
      const float  mainConstraint = std::max(0.0f, (vertical ? heightConstraint - marginH : widthConstraint - marginW));
      MeasuredSize exact          = MeasureStackChild(childImpl,
                                             vertical ? crossSlot : mainConstraint,
                                             vertical ? mainConstraint : crossSlot,
                                             vertical ? MeasureMode::EXACTLY : MeasureMode::AT_MOST,
                                             vertical ? MeasureMode::AT_MOST : MeasureMode::EXACTLY,
                                             exactMatch ? MeasureModeCheck::MATCHES_DERIVATION : MeasureModeCheck::NONE);
      const float  oldMain        = vertical ? workingSizes[i].height : workingSizes[i].width;
      const float  newMain        = vertical ? exact.height : exact.width;
      first.mainAxisNonWeight += newMain - oldMain;
      workingSizes[i] = exact;
    }
  }

  float maxCrossAxis = first.maxCrossAxis;
  float mainAxisTotal;

  if(first.totalWeight > 0.0f && first.visibleChildCount > 0)
  {
    float requestedMain = (impl->mOrientation == StackOrientation::VERTICAL) ? view->GetRequestedHeight() : view->GetRequestedWidth();
    // A WRAP_CONTENT main axis measured EXACTLY distributes its weights over the given size, like an
    // explicit one (a mode-aware stack only; any other keeps today's rule).
    const bool mainDetermined = vertical ? Internal::ViewDataImpl::Get(*view).IsHeightDetermined() : Internal::ViewDataImpl::Get(*view).IsWidthDetermined();
    bool       isMainAxisWrap = (requestedMain != MATCH_PARENT && requestedMain < 0.0f) && !(context.modeAware && mainDetermined);

    float spacingTotal = (first.visibleChildCount > 1) ? visSpacing * (first.visibleChildCount - 1) : 0.0f;
    float wrappedMain  = first.mainAxisNonWeight + spacingTotal;

    // Determine the target main-axis size for weight distribution.
    float targetMain;
    if(isMainAxisWrap)
    {
      Insets padding        = view->GetPadding();
      float  paddingMain    = (impl->mOrientation == StackOrientation::VERTICAL)
                                ? static_cast<float>(padding.top + padding.bottom) * s
                                : static_cast<float>(padding.start + padding.end) * s;
      float  minMain        = (impl->mOrientation == StackOrientation::VERTICAL) ? view->GetMinimumHeight() : view->GetMinimumWidth();
      float  minMainContent = std::max(0.0f, minMain * s - paddingMain);
      targetMain            = std::max(wrappedMain, minMainContent);
    }
    else
    {
      targetMain = contentMain;
    }

    if(targetMain > wrappedMain)
    {
      MeasureStackWeightChildren(children, workingSizes, targetMain, widthConstraint, heightConstraint,
                                 first.mainAxisNonWeight, first.totalWeight, first.visibleChildCount, visSpacing,
                                 impl->mOrientation, maxCrossAxis, context, slotCross);
      mainAxisTotal = targetMain;
    }
    else
    {
      for(uint32_t i = 0; i < children.size(); ++i)
      {
        ViewImpl& childImpl = GetImpl(children[i]);
        if(IsChildStandalone(childImpl))
        {
          continue;
        }
        float weight = GetChildWeight(childImpl);
        if(weight <= 0.0f)
        {
          continue;
        }

        float  childScale            = childImpl.GetEffectiveScale();
        Insets margin                = childImpl.GetMargin();
        float  marginW               = static_cast<float>(margin.start + margin.end) * childScale;
        float  marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
        float  childWidthConstraint  = std::max(0.0f, widthConstraint - marginW);
        float  childHeightConstraint = std::max(0.0f, heightConstraint - marginH);

        // No room to share: the weight child is measured at its natural main extent (AT_MOST;
        // its arrange-time re-measure assigns the share), the cross axis as for any child.
        MeasureMode      widthMode  = MeasureMode::AT_MOST;
        MeasureMode      heightMode = MeasureMode::AT_MOST;
        MeasureModeCheck check      = MeasureModeCheck::NONE;
        if(context.modeAware)
        {
          const MeasureMode crossMode = StackCrossMode(context, childImpl, impl->mOrientation);
          widthMode                   = vertical ? crossMode : MeasureMode::AT_MOST;
          heightMode                  = vertical ? MeasureMode::AT_MOST : crossMode;
        }
        else
        {
          const Internal::MeasureModes legacy = LegacyStackChildModes(context, childImpl);
          widthMode                           = legacy.width;
          heightMode                          = legacy.height;
          check                               = MeasureModeCheck::MATCHES_DERIVATION;
        }
        MeasuredSize childSize = MeasureStackChild(childImpl, childWidthConstraint, childHeightConstraint, widthMode, heightMode, check);
        workingSizes[i]        = childSize;

        if(impl->mOrientation == StackOrientation::VERTICAL)
        {
          first.mainAxisNonWeight += childSize.height + marginH;
          maxCrossAxis = std::max(maxCrossAxis, childSize.width + marginW);
        }
        else
        {
          first.mainAxisNonWeight += childSize.width + marginW;
          maxCrossAxis = std::max(maxCrossAxis, childSize.height + marginH);
        }
      }
      mainAxisTotal = first.mainAxisNonWeight + spacingTotal;
    }
  }
  else
  {
    mainAxisTotal = first.mainAxisNonWeight;
    if(first.visibleChildCount > 1)
    {
      mainAxisTotal += visSpacing * (first.visibleChildCount - 1);
    }
  }

  MeasuredSize totalSize(0.0f, 0.0f);
  if(impl->mOrientation == StackOrientation::VERTICAL)
  {
    totalSize.width  = maxCrossAxis;
    totalSize.height = mainAxisTotal;
  }
  else
  {
    totalSize.width  = mainAxisTotal;
    totalSize.height = maxCrossAxis;
  }
  return totalSize;
}

void StackLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return;
  }

  auto* impl = GetImplAs<Impl>();

  float s          = view->GetEffectiveScale();
  float visSpacing = impl->mSpacing * s;

  const uint32_t    count = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(count);
  for(uint32_t i = 0; i < count; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  float availableWidth  = bounds.width;
  float availableHeight = bounds.height;
  float currentX        = bounds.x;
  float currentY        = bounds.y;

  // The same measure-mode facts the measure pass used (the stored modes are those of this
  // view's latest measurement).
  const StackModeContext context = MakeStackModeContext(Internal::ViewDataImpl::Get(*view), impl->mOrientation);

  // Local allocation buffer: seeded from each child's current measured size.
  std::vector<MeasuredSize> allocations(children.size());
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    allocations[i] = GetImpl(children[i]).GetMeasuredSize();
  }

  // Re-distribute weight among weight children using actual arrange bounds.
  {
    float    totalWeight       = 0.0f;
    float    nonWeightMain     = 0.0f;
    uint32_t visibleChildCount = 0;

    for(uint32_t i = 0; i < children.size(); ++i)
    {
      ViewImpl& childImpl = GetImpl(children[i]);
      if(IsChildStandalone(childImpl))
      {
        continue;
      }
      visibleChildCount++;
      float weight = GetChildWeight(childImpl);
      if(weight > 0.0f)
      {
        totalWeight += weight;
      }
      else
      {
        float  childScale = childImpl.GetEffectiveScale();
        Insets margin     = childImpl.GetMargin();
        float  marginM    = (impl->mOrientation == StackOrientation::VERTICAL) ? static_cast<float>(margin.top + margin.bottom) * childScale
                                                                               : static_cast<float>(margin.start + margin.end) * childScale;
        float  mainSize   = (impl->mOrientation == StackOrientation::VERTICAL) ? allocations[i].height : allocations[i].width;
        nonWeightMain += mainSize + marginM;
      }
    }

    if(totalWeight > 0.0f)
    {
      float availableMain = (impl->mOrientation == StackOrientation::VERTICAL) ? availableHeight : availableWidth;
      float spacingTotal  = (visibleChildCount > 1) ? visSpacing * (visibleChildCount - 1) : 0.0f;
      float remainingMain = std::max(0.0f, availableMain - nonWeightMain - spacingTotal);

      for(uint32_t i = 0; i < children.size(); ++i)
      {
        ViewImpl& childImpl = GetImpl(children[i]);
        if(IsChildStandalone(childImpl))
        {
          continue;
        }
        float weight = GetChildWeight(childImpl);
        if(weight <= 0.0f)
        {
          continue;
        }

        float  share      = (weight / totalWeight) * remainingMain;
        float  childScale = childImpl.GetEffectiveScale();
        Insets margin     = childImpl.GetMargin();
        float  marginW    = static_cast<float>(margin.start + margin.end) * childScale;
        float  marginH    = static_cast<float>(margin.top + margin.bottom) * childScale;

        float childWidthConstraint;
        float childHeightConstraint;
        if(impl->mOrientation == StackOrientation::VERTICAL)
        {
          childWidthConstraint  = std::max(0.0f, availableWidth - marginW);
          childHeightConstraint = std::max(0.0f, share - marginH);
        }
        else
        {
          childWidthConstraint  = std::max(0.0f, share - marginW);
          childHeightConstraint = std::max(0.0f, availableHeight - marginH);
        }

        // The share is assigned (main EXACTLY), the cross axis per the stack's cross rule. Never
        // skipped: its result is consumed below.
        MeasureMode widthMode  = MeasureMode::AT_MOST;
        MeasureMode heightMode = MeasureMode::AT_MOST;
        if(context.modeAware)
        {
          const MeasureMode crossMode = StackSlotCrossMode(context, childImpl, impl->mOrientation);
          const bool        vertical  = (impl->mOrientation == StackOrientation::VERTICAL);
          widthMode                   = vertical ? crossMode : MeasureMode::EXACTLY;
          heightMode                  = vertical ? MeasureMode::EXACTLY : crossMode;
        }
        else
        {
          const Internal::MeasureModes legacy = LegacyStackChildModes(context, childImpl);
          widthMode                           = legacy.width;
          heightMode                          = legacy.height;
        }
        MeasuredSize childSize;
        {
          Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(view);
          childSize = MeasureStackChild(childImpl, childWidthConstraint, childHeightConstraint, widthMode, heightMode, MeasureModeCheck::MATCHES_DERIVATION);
        }

        if(impl->mOrientation == StackOrientation::VERTICAL)
        {
          allocations[i].width  = childSize.width;
          allocations[i].height = std::max(0.0f, share - marginH);
        }
        else
        {
          allocations[i].width  = std::max(0.0f, share - marginW);
          allocations[i].height = childSize.height;
        }
      }
    }
  }

  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);

    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    float      childScale = childImpl.GetEffectiveScale();
    Insets     margin     = childImpl.GetMargin();
    float      marginW    = static_cast<float>(margin.start + margin.end) * childScale;
    float      marginH    = static_cast<float>(margin.top + margin.bottom) * childScale;
    LayoutRect childBounds;

    if(impl->mOrientation == StackOrientation::VERTICAL)
    {
      const float crossAvailable = std::max(0.0f, availableWidth - marginW);
      float       childHeight    = allocations[i].height;
      if(childImpl.GetRequestedHeight() == MATCH_PARENT && GetChildWeight(childImpl) <= 0.0f)
      {
        childHeight = std::max(0.0f, availableHeight - marginH);
      }
      const float slotHeight = childHeight + marginH;
      float       childWidth = allocations[i].width;
      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        childWidth = crossAvailable;
      }

      LayoutAlignment crossAlign = GetChildAlignment(childImpl);
      float           crossX     = currentX + static_cast<float>(margin.start) * childScale;
      float           finalWidth = childWidth;
      switch(crossAlign)
      {
        case LayoutAlignment::FILL:
          if(childImpl.GetRequestedWidth() == WRAP_CONTENT)
          {
            finalWidth = crossAvailable;
          }
          break;
        case LayoutAlignment::CENTER:
          crossX += (crossAvailable - childWidth) * 0.5f;
          break;
        case LayoutAlignment::END:
          crossX += crossAvailable - childWidth;
          break;
        case LayoutAlignment::START:
        default:
          break;
      }

      childBounds.width  = finalWidth;
      childBounds.height = childHeight;
      childBounds.x      = crossX;
      childBounds.y      = currentY + static_cast<float>(margin.top) * childScale;

      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        RemeasureStackChildForSlot(view, childImpl, childBounds, impl->mOrientation, context);
      }
      childImpl.Arrange(childBounds);

      currentY += slotHeight + visSpacing;
    }
    else
    {
      const float crossAvailable = std::max(0.0f, availableHeight - marginH);
      float       childWidth     = allocations[i].width;
      if(childImpl.GetRequestedWidth() == MATCH_PARENT && GetChildWeight(childImpl) <= 0.0f)
      {
        childWidth = std::max(0.0f, availableWidth - marginW);
      }
      const float slotWidth   = childWidth + marginW;
      float       childHeight = allocations[i].height;
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childHeight = crossAvailable;
      }

      LayoutAlignment crossAlign  = GetChildAlignment(childImpl);
      float           crossY      = currentY + static_cast<float>(margin.top) * childScale;
      float           finalHeight = childHeight;
      switch(crossAlign)
      {
        case LayoutAlignment::FILL:
          if(childImpl.GetRequestedHeight() == WRAP_CONTENT)
          {
            finalHeight = crossAvailable;
          }
          break;
        case LayoutAlignment::CENTER:
          crossY += (crossAvailable - childHeight) * 0.5f;
          break;
        case LayoutAlignment::END:
          crossY += crossAvailable - childHeight;
          break;
        case LayoutAlignment::START:
        default:
          break;
      }

      childBounds.width  = childWidth;
      childBounds.height = finalHeight;
      childBounds.x      = currentX + static_cast<float>(margin.start) * childScale;
      childBounds.y      = crossY;

      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        RemeasureStackChildForSlot(view, childImpl, childBounds, impl->mOrientation, context);
      }
      childImpl.Arrange(childBounds);

      currentX += slotWidth + visSpacing;
    }
  }
}

} // namespace Ui
} //namespace DALI_NAMESPACE
