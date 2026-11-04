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
#include <dali-ui-foundation/public-api/layouts/absolute-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/absolute-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/internal/layouts/measure-mode.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

namespace
{

LayoutRect GetChildBounds(Internal::AbsoluteLayoutParamsImpl* params)
{
  if(params)
  {
    return params->GetBounds();
  }
  return LayoutRect(0.0f, 0.0f, -1.0f, -1.0f);
}

AbsoluteLayoutFlags GetChildFlags(Internal::AbsoluteLayoutParamsImpl* params)
{
  if(params)
  {
    return params->GetFlags();
  }
  return AbsoluteLayoutFlags::NONE;
}

using Internal::MeasureMode;
using Internal::MeasureModeCheck;

bool HasFlag(AbsoluteLayoutFlags flags, AbsoluteLayoutFlags flag)
{
  return (static_cast<uint8_t>(flags) & static_cast<uint8_t>(flag)) != 0;
}

/**
 * One child of a mode-aware AbsoluteLayout measure pass: today's per-child quantities plus the
 * measure modes and the two-step bookkeeping.
 */
struct AbsoluteMeasureEntry
{
  ViewImpl*   impl{nullptr};
  LayoutRect  spec; ///< The layout params bounds
  bool        xProportional{false};
  bool        yProportional{false};
  bool        widthProportional{false};
  bool        heightProportional{false};
  float       childScale{1.0f};
  float       marginW{0.0f};
  float       marginH{0.0f};
  float       w{-1.0f};       ///< Resolved explicit / proportional width (negative: none)
  float       h{-1.0f};       ///< Resolved explicit / proportional height (negative: none)
  float       measureW{0.0f}; ///< The step-1 width number (today's)
  float       measureH{0.0f}; ///< The step-1 height number (today's)
  MeasureMode modeW{MeasureMode::AT_MOST};
  MeasureMode modeH{MeasureMode::AT_MOST};
  bool        fixedW{false};
  bool        fixedH{false};
  bool        matchW{false}; ///< MATCH_PARENT width without an explicit bounds width
  bool        matchH{false};
  bool        probeW{false}; ///< matchW on an undetermined width: measured exactly in step 2
  bool        probeH{false};
  bool        deferW{false}; ///< Proportional width on an undetermined width: measured in step 2 only
  bool        deferH{false};
  bool        hasBottom{false};
  float       bottom{0.0f}; ///< The maxBottom contribution
};

// The maxBottom contribution of one child with height @p childHeight (today's rules: a
// size-proportional child on a WRAP_CONTENT height is excluded, a position-only proportional
// one drops its circular offset).
void SetAbsoluteBottomContribution(AbsoluteMeasureEntry& entry, float childHeight, float contentHeight, bool containerWrapH)
{
  const float h = (entry.h < 0) ? childHeight : entry.h;
  const float y = entry.yProportional ? (contentHeight - h) * entry.spec.y : entry.spec.y * entry.childScale;
  if(containerWrapH && entry.heightProportional)
  {
    entry.hasBottom = false;
  }
  else if(containerWrapH && entry.yProportional)
  {
    entry.hasBottom = true;
    entry.bottom    = h + entry.marginH;
  }
  else
  {
    entry.hasBottom = true;
    entry.bottom    = y + h + entry.marginH;
  }
}

// The content extent an AbsoluteLayout is laid out at on one axis when its parent places it at
// its measured size: the measure dispatch adds the padding, Measure() clamps by the own
// min/max, the arrange dispatch takes the padding off again.
float AbsoluteContentSlot(float aggregate, float padding, float minimum, float maximum)
{
  return std::max(0.0f, std::min(std::max(aggregate + padding, minimum), maximum) - padding);
}

/**
 * The mode-aware measure of an AbsoluteLayout.
 *
 * Step 1 is today's loop, today's numbers, today's aggregation: an explicit or proportional
 * extent is measured EXACTLY, a MATCH_PARENT one EXACTLY at the content extent when that is
 * determined, otherwise as a probe (AT_MOST, today's budget, so the WRAP_CONTENT aggregate is
 * unchanged). A proportional extent on an undetermined axis cannot be resolved before the axis is,
 * so its child is measured after the others (step 2). Once an axis is resolved, the children
 * whose extent it decides are measured EXACTLY at their arrange slot; their results feed only the
 * OTHER axis' aggregate.
 */
MeasuredSize MeasureAbsoluteModeAware(std::vector<View>& children, float contentWidth, float contentHeight,
                                      bool containerWrapW, bool containerWrapH, bool widthDetermined, bool heightDetermined, ViewImpl& view)
{
  std::vector<AbsoluteMeasureEntry> entries;
  entries.reserve(children.size());

  float maxRight    = 0.0f;
  float maxRightOwn = 0.0f; ///< maxRight over the children that are not MATCH_PARENT on the width
  for(auto& child : children)
  {
    ViewImpl& childImpl = GetImpl(child);
    if(childImpl.GetLayoutMode() == LayoutMode::STANDALONE)
    {
      continue;
    }

    AbsoluteMeasureEntry entry;
    auto*                params     = Internal::AbsoluteLayoutParamsImpl::Get(childImpl);
    entry.impl                      = &childImpl;
    entry.spec                      = GetChildBounds(params);
    const AbsoluteLayoutFlags flags = GetChildFlags(params);
    entry.xProportional             = HasFlag(flags, AbsoluteLayoutFlags::X_PROPORTIONAL);
    entry.yProportional             = HasFlag(flags, AbsoluteLayoutFlags::Y_PROPORTIONAL);
    entry.widthProportional         = HasFlag(flags, AbsoluteLayoutFlags::WIDTH_PROPORTIONAL);
    entry.heightProportional        = HasFlag(flags, AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL);
    entry.childScale                = childImpl.GetEffectiveScale();

    // Today's extent resolution.
    entry.w = entry.spec.width;
    entry.h = entry.spec.height;
    if(entry.widthProportional)
    {
      entry.w *= contentWidth;
    }
    else if(entry.w > 0.0f)
    {
      entry.w *= entry.childScale;
    }
    if(entry.heightProportional)
    {
      entry.h *= contentHeight;
    }
    else if(entry.h > 0.0f)
    {
      entry.h *= entry.childScale;
    }

    Insets margin  = childImpl.GetMargin();
    entry.marginW  = (margin.start + margin.end) * entry.childScale;
    entry.marginH  = (margin.top + margin.bottom) * entry.childScale;
    entry.measureW = entry.w >= 0.0f ? entry.w : std::max(0.0f, contentWidth - entry.marginW);
    entry.measureH = entry.h >= 0.0f ? entry.h : std::max(0.0f, contentHeight - entry.marginH);

    // Modes. An explicit requested size is canonical (AT_MOST, today's number). An explicit
    // bounds extent (>= 0) is assigned: as is, or proportionally once the axis is determined. A
    // MATCH_PARENT extent without one is assigned the content extent once determined.
    entry.fixedW = childImpl.GetRequestedWidth() >= 0.0f;
    entry.fixedH = childImpl.GetRequestedHeight() >= 0.0f;
    entry.matchW = !entry.fixedW && entry.spec.width < 0.0f && childImpl.GetRequestedWidth() == MATCH_PARENT;
    entry.matchH = !entry.fixedH && entry.spec.height < 0.0f && childImpl.GetRequestedHeight() == MATCH_PARENT;
    if(!entry.fixedW && entry.spec.width >= 0.0f)
    {
      entry.deferW = entry.widthProportional && !widthDetermined;
      entry.modeW  = MeasureMode::EXACTLY;
    }
    else if(entry.matchW)
    {
      entry.probeW = !widthDetermined;
      entry.modeW  = widthDetermined ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
    }
    if(!entry.fixedH && entry.spec.height >= 0.0f)
    {
      entry.deferH = entry.heightProportional && !heightDetermined;
      entry.modeH  = MeasureMode::EXACTLY;
    }
    else if(entry.matchH)
    {
      entry.probeH = !heightDetermined;
      entry.modeH  = heightDetermined ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
    }

    // Step 1. A width-deferred child waits for the width: its width is excluded from maxRight
    // anyway (circular on a WRAP_CONTENT width) and its bottom is taken at its real width in
    // step 2a. A height-deferred child still contributes its width to maxRight, so it is
    // measured here with today's height number (AT_MOST, a probe) and again in step 2b.
    if(!entry.deferW)
    {
      const MeasureMode      heightMode = entry.deferH ? MeasureMode::AT_MOST : entry.modeH;
      const MeasureModeCheck check      = (entry.probeW || entry.probeH || entry.deferH) ? MeasureModeCheck::NONE : MeasureModeCheck::MATCHES_DERIVATION;
      MeasuredSize           childSize  = Internal::ViewDataImpl::Get(childImpl).Measure(entry.measureW, entry.measureH, entry.modeW, heightMode, check);

      const float w = (entry.w < 0) ? childSize.width : entry.w;
      const float x = entry.xProportional ? (contentWidth - w) * entry.spec.x : entry.spec.x * entry.childScale;
      if(containerWrapW && entry.widthProportional)
      {
        // Size-proportional on a WRAP axis: fully circular, exclude.
      }
      else if(containerWrapW && entry.xProportional)
      {
        maxRight = std::max(maxRight, w + entry.marginW);
        if(!entry.probeW)
        {
          maxRightOwn = std::max(maxRightOwn, w + entry.marginW);
        }
      }
      else
      {
        maxRight = std::max(maxRight, x + w + entry.marginW);
        if(!entry.probeW)
        {
          maxRightOwn = std::max(maxRightOwn, x + w + entry.marginW);
        }
      }
      if(!entry.deferH)
      {
        SetAbsoluteBottomContribution(entry, childSize.height, contentHeight, containerWrapH);
      }
    }
    entries.push_back(entry);
  }

  const Insets padding = view.GetPadding();
  const float  s       = view.GetEffectiveScale();

  // Step 2a: the width is resolved. Only a layout with a width of its own -- what its children that
  // are not MATCH_PARENT or proportional on the width and its own minimum give it, not its
  // MATCH_PARENT children's margins or reports -- hands out a slot: otherwise every MATCH_PARENT or
  // proportional child keeps today's numbers, whatever its margin.
  float resolvedWidth = contentWidth;
  bool  ownWidth      = true;
  if(!widthDetermined)
  {
    resolvedWidth = AbsoluteContentSlot(maxRight, static_cast<float>(padding.start + padding.end) * s, view.GetMinimumWidth() * s, view.GetMaximumWidth() * s);
    ownWidth      = Internal::SnapEmptySlot(AbsoluteContentSlot(maxRightOwn, static_cast<float>(padding.start + padding.end) * s, view.GetMinimumWidth() * s, view.GetMaximumWidth() * s)) > 0.0f;
    for(auto& entry : entries)
    {
      if(!(entry.probeW || entry.deferW))
      {
        continue;
      }
      // A child whose height is also decided in step 2b is measured there, once. A width-deferred
      // child with a MATCH_PARENT height was not measured in step 1: it is probed here with today's
      // numbers, so that its bottom (offset, margins, reported height) counts as it always did,
      // before the heights are summed.
      if(entry.probeH || entry.deferH)
      {
        if(entry.deferW && entry.probeH)
        {
          const MeasuredSize probe = Internal::ViewDataImpl::Get(*entry.impl).Measure(entry.measureW, entry.measureH, MeasureMode::AT_MOST, MeasureMode::AT_MOST, MeasureModeCheck::NONE);
          SetAbsoluteBottomContribution(entry, probe.height, contentHeight, containerWrapH);
        }
        continue;
      }
      const float width = ownWidth ? Internal::SnapEmptySlot(entry.probeW ? resolvedWidth - entry.marginW : entry.spec.width * resolvedWidth) : 0.0f;
      if(width <= 0.0f)
      {
        // An empty slot is never measured exactly: nothing gives this layout a width of its own. A
        // probe keeps its step-1 result (and bottom); a width-deferred child, never measured yet,
        // is measured once with today's numbers (its proportion of the budget, AT_MOST).
        if(entry.deferW)
        {
          MeasuredSize childSize = Internal::ViewDataImpl::Get(*entry.impl).Measure(entry.measureW, entry.measureH, MeasureMode::AT_MOST, entry.modeH, MeasureModeCheck::NONE);
          SetAbsoluteBottomContribution(entry, childSize.height, contentHeight, containerWrapH);
        }
        continue;
      }
      MeasuredSize childSize = Internal::ViewDataImpl::Get(*entry.impl).Measure(width, entry.measureH, MeasureMode::EXACTLY, entry.modeH, MeasureModeCheck::MATCHES_DERIVATION);
      SetAbsoluteBottomContribution(entry, childSize.height, contentHeight, containerWrapH);
    }
  }

  float maxBottom    = 0.0f;
  float maxBottomOwn = 0.0f; ///< maxBottom over the children that are not MATCH_PARENT on the height
  for(const auto& entry : entries)
  {
    if(entry.hasBottom)
    {
      maxBottom = std::max(maxBottom, entry.bottom);
      if(!entry.probeH)
      {
        maxBottomOwn = std::max(maxBottomOwn, entry.bottom);
      }
    }
  }

  // Step 2b: the height is resolved. The width of these calls is the child's final width rule;
  // widths are not re-aggregated.
  if(!heightDetermined)
  {
    const float resolvedHeight = AbsoluteContentSlot(maxBottom, static_cast<float>(padding.top + padding.bottom) * s, view.GetMinimumHeight() * s, view.GetMaximumHeight() * s);
    const bool  ownHeight      = Internal::SnapEmptySlot(AbsoluteContentSlot(maxBottomOwn, static_cast<float>(padding.top + padding.bottom) * s, view.GetMinimumHeight() * s, view.GetMaximumHeight() * s)) > 0.0f;
    for(auto& entry : entries)
    {
      if(!(entry.probeH || entry.deferH))
      {
        continue;
      }
      const float      height    = ownHeight ? Internal::SnapEmptySlot(entry.probeH ? resolvedHeight - entry.marginH : entry.spec.height * resolvedHeight) : 0.0f;
      float            width     = entry.measureW;
      MeasureMode      widthMode = entry.matchW ? MeasureMode::EXACTLY : entry.modeW;
      MeasureModeCheck check     = MeasureModeCheck::MATCHES_DERIVATION;
      const bool       widthRule = entry.matchW || (!entry.fixedW && entry.spec.width >= 0.0f && entry.widthProportional);
      if(entry.matchW)
      {
        width = Internal::SnapEmptySlot((widthDetermined ? contentWidth : resolvedWidth) - entry.marginW);
      }
      else if(widthRule)
      {
        width = Internal::SnapEmptySlot(entry.spec.width * resolvedWidth);
      }
      if(widthRule && !widthDetermined && (width <= 0.0f || !ownWidth))
      {
        // An empty width slot is never measured exactly: the width keeps today's number (the budget,
        // or its proportion of it), AT_MOST, while the derivation says EXACTLY.
        width     = entry.measureW;
        widthMode = MeasureMode::AT_MOST;
        check     = MeasureModeCheck::NONE;
      }
      if(height <= 0.0f)
      {
        // An empty height slot is never measured exactly either. A child measured in step 1 or
        // probed in step 2a keeps that result; one deferred on both axes, never measured yet, is
        // measured once with today's height number, AT_MOST.
        if(entry.deferW && entry.deferH)
        {
          Internal::ViewDataImpl::Get(*entry.impl).Measure(width, entry.measureH, widthMode, MeasureMode::AT_MOST, MeasureModeCheck::NONE);
        }
        continue;
      }
      Internal::ViewDataImpl::Get(*entry.impl).Measure(width, height, widthMode, MeasureMode::EXACTLY, check);
    }
  }

  return MeasuredSize(maxRight, maxBottom);
}

} // namespace

class AbsoluteLayoutManager::Impl : public LayoutManager::Impl
{
};

AbsoluteLayoutManager::AbsoluteLayoutManager()
: LayoutManager(new Impl())
{
}

AbsoluteLayoutManager::~AbsoluteLayoutManager()
{
}

MeasuredSize AbsoluteLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  const uint32_t count         = GetChildViewCount(view);
  float          contentWidth  = widthConstraint;
  float          contentHeight = heightConstraint;

  // Snapshot children up front: a child's Measure callback may remove a
  // sibling mid-pass, which would invalidate a cached-index live traversal.
  std::vector<View> children;
  children.reserve(count);
  for(uint32_t i = 0; i < count; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  float maxRight  = 0.0f;
  float maxBottom = 0.0f;

  // Size circularity guard. When the container axis is WRAP_CONTENT,
  // its intrinsic size is what we are computing here, yet a proportional
  // child resolves its extent/position against that very (still undetermined)
  // size. Letting such a child feed maxRight/maxBottom would make the
  // proportional result define the basis it depends on. A proportional value
  // therefore does not contribute to the WRAP axis intrinsic size.
  const bool containerWrapW = (view->GetRequestedWidth() == WRAP_CONTENT);
  const bool containerWrapH = (view->GetRequestedHeight() == WRAP_CONTENT);

  // Measure modes: only a view whose producer is exactly this manager is mode-aware; anything
  // else (a subclass, a layout callback) measures every child below with the custom-parent modes
  // of its view, in one step.
  const Internal::ViewDataImpl& viewData = Internal::ViewDataImpl::Get(*view);
  if(viewData.GetMeasureModeParent() == Internal::MeasureModeParent::ABSOLUTE)
  {
    return MeasureAbsoluteModeAware(children, contentWidth, contentHeight, containerWrapW, containerWrapH, viewData.IsWidthDetermined(), viewData.IsHeightDetermined(), *view);
  }

  for(auto& child : children)
  {
    ViewImpl& childImpl = GetImpl(child);

    if(IsStandalone(&childImpl))
    {
      continue;
    }

    float               childScale = childImpl.GetEffectiveScale();
    auto*               params     = Internal::AbsoluteLayoutParamsImpl::Get(childImpl);
    LayoutRect          bounds     = GetChildBounds(params);
    AbsoluteLayoutFlags flags      = GetChildFlags(params);

    float x = bounds.x;
    float y = bounds.y;
    float w = bounds.width;
    float h = bounds.height;

    const uint8_t rawFlags = static_cast<uint8_t>(flags);
    const bool    xProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::X_PROPORTIONAL)) != 0;
    const bool yProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::Y_PROPORTIONAL)) != 0;
    const bool widthProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL)) != 0;
    const bool heightProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL)) != 0;

    if(widthProportional)
    {
      w *= contentWidth;
    }
    else if(w > 0.0f)
    {
      w *= childScale;
    }
    if(heightProportional)
    {
      h *= contentHeight;
    }
    else if(h > 0.0f)
    {
      h *= childScale;
    }

    Insets margin  = childImpl.GetMargin();
    float  marginW = (margin.start + margin.end) * childScale;
    float  marginH = (margin.top + margin.bottom) * childScale;

    // Always measure children. When the child has no explicit positive extent
    // on an axis (WRAP_CONTENT / MATCH_PARENT), the constraint is the available
    // content budget clamped to be non-negative; for explicit positive bounds
    // the constraint is the bounds value itself. Calling Measure unconditionally
    // keeps the child's measure cache and dirty state coherent with the layout
    // pass — required so a later InvalidateMeasure can propagate. The child's
    // own cache no-ops when constraints repeat.
    float measureW = w >= 0.0f ? w : std::max(0.0f, contentWidth - marginW);
    float measureH = h >= 0.0f ? h : std::max(0.0f, contentHeight - marginH);
    // A layout that is not mode-aware (a subclass, a layout callback) passes the custom-parent
    // modes of its view, which is also what the child's own derivation gives.
    const Internal::MeasureModes legacy    = viewData.DeriveChildModesAsCustomParent(childImpl.GetRequestedWidth(), childImpl.GetRequestedHeight());
    MeasuredSize                 childSize = Internal::ViewDataImpl::Get(childImpl).Measure(measureW, measureH, legacy.width, legacy.height, MeasureModeCheck::MATCHES_DERIVATION);

    if(w < 0)
    {
      w = childSize.width;
    }
    if(h < 0)
    {
      h = childSize.height;
    }

    // Proportional position: axis = (available - childExtent) * proportion
    if(xProportional)
    {
      x = (contentWidth - w) * bounds.x;
    }
    else
    {
      x *= childScale;
    }
    if(yProportional)
    {
      y = (contentHeight - h) * bounds.y;
    }
    else
    {
      y *= childScale;
    }

    // On a WRAP container axis the intrinsic size is what we are
    // computing, so only the genuinely circular contribution must be dropped.
    // A WIDTH_PROPORTIONAL child resolves its extent against that undetermined
    // size, so it is excluded entirely. A position-only proportional child
    // (X_PROPORTIONAL && !WIDTH_PROPORTIONAL) has a determinate extent w; only
    // its proportional position offset x = (contentWidth - w) * p is circular.
    // For it, contribute the determinate extent (w + marginW) while dropping
    // the circular position offset. Non-WRAP axes accumulate normally.
    if(containerWrapW && widthProportional)
    {
      // Size-proportional on a WRAP axis: fully circular, exclude.
    }
    else if(containerWrapW && xProportional)
    {
      // Position-only proportional on a WRAP axis: drop the circular x offset,
      // keep the determinate extent.
      maxRight = std::max(maxRight, w + marginW);
    }
    else
    {
      maxRight = std::max(maxRight, x + w + marginW);
    }

    if(containerWrapH && heightProportional)
    {
      // Size-proportional on a WRAP axis: fully circular, exclude.
    }
    else if(containerWrapH && yProportional)
    {
      maxBottom = std::max(maxBottom, h + marginH);
    }
    else
    {
      maxBottom = std::max(maxBottom, y + h + marginH);
    }
  }

  return MeasuredSize(maxRight, maxBottom);
}

void AbsoluteLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return;
  }

  const uint32_t count           = GetChildViewCount(view);
  float          availableWidth  = bounds.width;
  float          availableHeight = bounds.height;

  // Snapshot children up front: a child's Arrange callback may remove a
  // sibling mid-pass, which would invalidate a cached-index live traversal.
  std::vector<View> children;
  children.reserve(count);
  for(uint32_t i = 0; i < count; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  const Internal::ViewDataImpl& viewData  = Internal::ViewDataImpl::Get(*view);
  const bool                    modeAware = (viewData.GetMeasureModeParent() == Internal::MeasureModeParent::ABSOLUTE);

  for(auto& child : children)
  {
    ViewImpl& childImpl = GetImpl(child);

    if(IsStandalone(&childImpl))
    {
      continue;
    }

    float               childScale      = childImpl.GetEffectiveScale();
    auto*               params          = Internal::AbsoluteLayoutParamsImpl::Get(childImpl);
    LayoutRect          childBoundsSpec = GetChildBounds(params);
    AbsoluteLayoutFlags flags           = GetChildFlags(params);

    float x = childBoundsSpec.x;
    float y = childBoundsSpec.y;
    float w = childBoundsSpec.width;
    float h = childBoundsSpec.height;

    const uint8_t rawFlags = static_cast<uint8_t>(flags);
    const bool    xProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::X_PROPORTIONAL)) != 0;
    const bool yProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::Y_PROPORTIONAL)) != 0;
    const bool widthProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL)) != 0;
    const bool heightProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL)) != 0;

    if(widthProportional)
    {
      w *= availableWidth;
    }
    else if(w > 0.0f)
    {
      w *= childScale;
    }
    if(heightProportional)
    {
      h *= availableHeight;
    }
    else if(h > 0.0f)
    {
      h *= childScale;
    }

    if(w < 0 || h < 0)
    {
      // For MATCH_PARENT axes we recompute from available space here; for
      // WRAP_CONTENT axes we reuse the natural size already recorded on the
      // child by Measure().
      MeasuredSize childMeasured = childImpl.GetMeasuredSize();
      if(w < 0)
      {
        if(childImpl.GetRequestedWidth() == MATCH_PARENT)
        {
          Insets margin = childImpl.GetMargin();
          w             = std::max(0.0f, availableWidth - (margin.start + margin.end) * childScale);
        }
        else
        {
          w = childMeasured.width;
        }
      }
      if(h < 0)
      {
        if(childImpl.GetRequestedHeight() == MATCH_PARENT)
        {
          Insets margin = childImpl.GetMargin();
          h             = std::max(0.0f, availableHeight - (margin.top + margin.bottom) * childScale);
        }
        else
        {
          h = childMeasured.height;
        }
      }
    }

    // Proportional position: axis = (available - childExtent) * proportion
    if(xProportional)
    {
      x = (availableWidth - w) * childBoundsSpec.x;
    }
    else
    {
      x *= childScale;
    }
    if(yProportional)
    {
      y = (availableHeight - h) * childBoundsSpec.y;
    }
    else
    {
      y *= childScale;
    }

    Insets margin = childImpl.GetMargin();

    LayoutRect childBounds;
    childBounds.x      = bounds.x + x + margin.start * childScale;
    childBounds.y      = bounds.y + y + margin.top * childScale;
    childBounds.width  = w;
    childBounds.height = h;

    // Re-measure MATCH_PARENT children with their final (scaled) size. A mode-aware layout
    // assigns a MATCH_PARENT or explicit / proportional extent (EXACTLY) and skips the call when
    // the child already holds a measurement for exactly this slot.
    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      Internal::ViewDataImpl& childData = Internal::ViewDataImpl::Get(childImpl);
      if(modeAware)
      {
        const MeasureMode widthMode  = (childImpl.GetRequestedWidth() == MATCH_PARENT || childBoundsSpec.width >= 0.0f) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
        const MeasureMode heightMode = (childImpl.GetRequestedHeight() == MATCH_PARENT || childBoundsSpec.height >= 0.0f) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
        if(!childData.CanSkipArrangeRemeasure(w, h, widthMode, heightMode))
        {
          Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(view);
          childData.Measure(w, h, widthMode, heightMode, MeasureModeCheck::MATCHES_DERIVATION);
        }
      }
      else
      {
        const Internal::MeasureModes                         legacy = viewData.DeriveChildModesAsCustomParent(childImpl.GetRequestedWidth(), childImpl.GetRequestedHeight());
        Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(view);
        childData.Measure(w, h, legacy.width, legacy.height, MeasureModeCheck::MATCHES_DERIVATION);
      }
    }
    childImpl.Arrange(childBounds);
  }
}

} // namespace Ui
} //namespace DALI_NAMESPACE
