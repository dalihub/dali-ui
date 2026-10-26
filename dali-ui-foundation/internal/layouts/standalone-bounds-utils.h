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
#include <dali/public-api/adaptor-framework/window.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/measure-mode.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{

/**
 * @brief The extent a boundary view's own pass places one axis at.
 *
 * The parent-supplied extent for a MATCH_PARENT axis, the measured size otherwise.
 *
 * Unclamped here; DeriveStandaloneRootBounds applies the view's own min/max to the result.
 * DeriveStandaloneMeasureConstraint measures a MATCH_PARENT axis EXACTLY at that same clamped
 * value, so the measured size equals the slot and the clamp in DeriveStandaloneRootBounds and the
 * arrange-side clamp are idempotent on such an axis.
 *
 * @param[in] match True when this axis requested MATCH_PARENT
 * @param[in] available The extent the parent makes available on this axis
 * @param[in] measured The view's measured extent on this axis
 * @return The extent this axis is placed at
 */
inline float ResolveStandaloneExtent(bool match, float available, float measured)
{
  return match ? available : measured;
}

/**
 * @brief Whether a view's measure constraint is bounded by the window it is shown in.
 *
 * True for a STANDALONE view with at least one WRAP_CONTENT axis: only such an axis reads the
 * window (DeriveStandaloneMeasureConstraint); a MATCH_PARENT axis reads the parent and an
 * explicitly sized axis reads nothing. A resize reaches such a view because every on-scene
 * STANDALONE view is a tracked layout root of its window's LayoutController (it registers on
 * scene connection, ViewDataImpl::OnViewSceneConnection) and LayoutController::OnWindowResize
 * invalidates every tracked root; leaving the scene drops its caches, which were measured
 * against the window it leaves (ViewDataImpl::OnViewSceneDisconnection). A view with no
 * WRAP_CONTENT axis never reads the window SIZE, so its slot snapshots
 * (SnapshotStandaloneSlotInputs) skip the window lookup and leaving the scene drops nothing.
 * It still registers -- and looks its window up -- on scene connection, like every STANDALONE
 * view.
 *
 * The sentinel compares are exact: the requested-size setters snap a near-sentinel input to the
 * exact sentinel.
 *
 * @param[in] view The view
 * @return True when the view's measure constraint reads the window size
 */
inline bool IsStandaloneWindowBounded(const ViewImpl& view)
{
  return view.GetLayoutMode() == LayoutMode::STANDALONE &&
         (view.GetRequestedWidth() == WRAP_CONTENT || view.GetRequestedHeight() == WRAP_CONTENT);
}

/**
 * @brief The inputs of a boundary view's slot derivation, captured at ONE instant.
 *
 * Both derivations -- the framework-owned self pass and the parent-driven placement --
 * must be fed the SAME values, and each caller must feed the values it derived its
 * available extent from. A measure producer that mutates its own scale, margin or
 * requested size mid-pass would otherwise leave the extent resolved against one set of
 * values and the slot against another. Taking the snapshot before Measure() and passing
 * it through makes that mixing impossible BY CONSTRUCTION rather than by convention.
 *
 * It also carries the requested sizes the per-axis rule is chosen from and the window
 * extent a window-bounded view's WRAP_CONTENT axes are measured against, captured at the
 * same instant, so the measure constraint and the slot are derived from one snapshot.
 */
struct StandaloneSlotInputs
{
  float  scale;
  Insets margin;
  float  requestedX;
  float  requestedY;
  float  requestedWidth;  ///< Natural units, as stored: >= 0 is an explicit size, otherwise WRAP_CONTENT or MATCH_PARENT
  float  requestedHeight; ///< Natural units, as stored: >= 0 is an explicit size, otherwise WRAP_CONTENT or MATCH_PARENT
  float  minimumWidth;
  float  maximumWidth;
  float  minimumHeight;
  float  maximumHeight;
  bool   matchWidth;    ///< requestedWidth == MATCH_PARENT
  bool   matchHeight;   ///< requestedHeight == MATCH_PARENT
  bool   windowBounded; ///< True when the view is STANDALONE, in a Window, and WRAP_CONTENT on at least one axis
  float  windowWidth;   ///< Width of that Window, visual units; read only when windowBounded
  float  windowHeight;  ///< Height of that Window, visual units; read only when windowBounded
};

/**
 * @brief Captures every input DeriveStandaloneMeasureConstraint and
 * DeriveStandaloneRootBounds need from @p view.
 *
 * Call this BEFORE the pass that may re-measure @p view, and hand the result to
 * ResolveStandaloneAvailableExtent, DeriveStandaloneMeasureConstraint and
 * DeriveStandaloneRootBounds.
 *
 * @param[in] view The boundary view being placed
 * @return The view's slot inputs at this instant
 */
inline StandaloneSlotInputs SnapshotStandaloneSlotInputs(const ViewImpl& view)
{
  StandaloneSlotInputs inputs;

  inputs.scale           = view.GetEffectiveScale();
  inputs.margin          = view.GetMargin();
  inputs.requestedX      = view.GetRequestedX();
  inputs.requestedY      = view.GetRequestedY();
  inputs.requestedWidth  = view.GetRequestedWidth();
  inputs.requestedHeight = view.GetRequestedHeight();
  inputs.minimumWidth    = view.GetMinimumWidth();
  inputs.maximumWidth    = view.GetMaximumWidth();
  inputs.minimumHeight   = view.GetMinimumHeight();
  inputs.maximumHeight   = view.GetMaximumHeight();
  inputs.matchWidth      = inputs.requestedWidth == MATCH_PARENT;
  inputs.matchHeight     = inputs.requestedHeight == MATCH_PARENT;
  inputs.windowBounded   = false;
  inputs.windowWidth     = 0.0f;
  inputs.windowHeight    = 0.0f;

  // Gated on this snapshot's OWN requested sizes (the same predicate as
  // IsStandaloneWindowBounded), so the lookup and the derivation that reads it can never
  // disagree. Window::Get() hands back an empty handle for a view that is in no Window, which
  // selects the parent-extent fallback. The bound is the whole Window::GetPositionSize()
  // extent: platform insets, input-method panels and window decorations are not accounted for.
  if(view.GetLayoutMode() == LayoutMode::STANDALONE &&
     (inputs.requestedWidth == WRAP_CONTENT || inputs.requestedHeight == WRAP_CONTENT))
  {
    Dali::Window window = Dali::Window::Get(view.Self());
    if(window)
    {
      const PositionSize positionSize = window.GetPositionSize();
      inputs.windowBounded            = true;
      inputs.windowWidth              = static_cast<float>(std::max(0, positionSize.width));
      inputs.windowHeight             = static_cast<float>(std::max(0, positionSize.height));
    }
  }

  return inputs;
}

/**
 * @brief A width/height pair in visual (scale-applied) units.
 */
struct StandaloneExtent
{
  float width;
  float height;
};

/**
 * @brief The extent a boundary view's parent makes available to it: the parent extent less the
 * view's own margin, per axis, never negative. The parent's padding is ignored on purpose.
 *
 * The ONE expression for it, shared by every caller, so that the value a MATCH_PARENT axis is
 * measured at (DeriveStandaloneMeasureConstraint) and the value it is placed at
 * (DeriveStandaloneRootBounds) are bit-identical at every call site.
 *
 * @param[in] inputs The view's slot inputs, snapshotted before the pass
 * @param[in] parentWidth The parent extent on the width axis, visual units
 * @param[in] parentHeight The parent extent on the height axis, visual units
 * @return The available extent
 */
inline StandaloneExtent ResolveStandaloneAvailableExtent(const StandaloneSlotInputs& inputs, float parentWidth, float parentHeight)
{
  const float marginW = static_cast<float>(inputs.margin.start + inputs.margin.end) * inputs.scale;
  const float marginH = static_cast<float>(inputs.margin.top + inputs.margin.bottom) * inputs.scale;
  return StandaloneExtent{std::max(0.0f, parentWidth - marginW), std::max(0.0f, parentHeight - marginH)};
}

/**
 * @brief THE one derivation of the constraint a boundary (STANDALONE) view is measured at.
 *
 * Three places measure a boundary view -- its parent's measure pass (MeasureStandaloneChildren),
 * its parent's placement (ArrangeStandaloneChild) and its own layout-root pass
 * (LayoutController::ProcessLayoutRoot) -- and all three call this with the extent THEY know for
 * the parent, already reduced to the available extent by ResolveStandaloneAvailableExtent. Per
 * axis:
 *  - MATCH_PARENT: the available extent clamped by the view's own min/max -- the very value
 *    DeriveStandaloneRootBounds places it at; the axis is measured EXACTLY
 *    (DeriveStandaloneMeasureModes), so the measured size equals the slot;
 *  - WRAP_CONTENT: the size of the window the view is shown in, less its own margin -- the
 *    whole Window::GetPositionSize() extent: platform insets, input-method panels and window
 *    decorations are not accounted for. It does not depend on the parent, so it is the same on
 *    all three paths, and a WRAP_CONTENT boundary view is not clipped to a parent it merely
 *    floats over. A view that is in no window (a direct Measure() on an off-scene tree) uses
 *    the available extent instead. There is no own pass off scene, but the parent's measure
 *    (its provisional extent) and its placement (its final extent) can still hand such an
 *    axis two different extents, so the placement may re-measure it;
 *  - an explicit size: the requested size x scale, with or without a window. No margin is
 *    subtracted: the margin lies outside the view's box (DeriveStandaloneRootBounds offsets the
 *    position by it).
 *
 * The explicit-size rule differs from the two derivations a boundary view was measured by
 * before this one. The regular layout-root path (LayoutController::ProcessLayoutRoot for a view
 * that is not STANDALONE or has no parent, still in use for those) measures an explicit axis at
 * the requested size x scale less the margin. And the parent's measure pass used to hand a
 * standalone child the parent's min/max-clamped incoming constraint, less the child's margin.
 * A producer that returns its requested size for an explicit axis -- the default measure does
 * -- reports the same size under all three; only a producer that reads its constraint on such
 * an axis can tell them apart.
 *
 * The window bound is FINITE by choice, and it is a trade-off. Measure() floors a negative
 * ("unbounded") constraint at the view's minimum and container producers clamp their content
 * constraint to zero, so a negative sentinel would collapse a WRAP_CONTENT container's
 * children. A large finite value would not -- ScrollView hands its scrollable axes
 * std::numeric_limits<float>::max() -- but the window size keeps a wrapped standalone
 * container's children and a Label's wrapping width finite as well, at the cost that a
 * WRAP_CONTENT axis never measures larger than the window. Which convention a producer prefers
 * is not settled here. Nor is the window an absolute display bound: the view's own min/max and
 * an explicit size may exceed it, its position is not taken into account, and a clipping
 * ancestor still clips it.
 *
 * So, in a window, every axis that is not MATCH_PARENT is measured at the same constraint on all
 * three paths, and a MATCH_PARENT axis at the parent extent the path knows: the parent's
 * PROVISIONAL size at the parent's measure; the parent's LAYOUT TARGET -- the bounds the parent
 * was last arranged at (ViewDataImpl::GetArrangedBounds()) -- at the placement, which receives
 * exactly those bounds from ArrangeStandaloneChildren, and at the view's own pass, which reads
 * them back (and falls back to the parent actor SIZE only when the parent is not a View or holds
 * no arrange result).
 * Given the same snapshot, the same parent layout target and the same window, the placement and
 * the own pass therefore derive the same constraint, available extent and slot: whichever of
 * them follows the other is a measure cache HIT, and the view's slot does not depend on which of
 * them ran last. That is the premise
 * ViewDataImpl::InvalidateParentArrangeCacheForOutOfBandArrange's framework-root-pass exemption
 * rests on. The parent's arrange result is not a freshness guarantee across every lifecycle
 * (HasArrangeResult() is cleared only for a reparented direct child): it is the parent's LAST
 * layout target, which the parent's next pass re-derives. The parent's measure matches the two
 * on a MATCH_PARENT axis only when the parent's provisional size is its final size; otherwise
 * the placement re-measures that axis.
 *
 * The window size enters ONLY through the returned constraint, which is the measure cache key, so
 * producers stay pure functions of their constraint. A resize reaches the view because every
 * on-scene STANDALONE view is a tracked layout root of its window's LayoutController (it
 * registers on scene connection, ViewDataImpl::OnViewSceneConnection), and
 * LayoutController::OnWindowResize invalidates every tracked root.
 *
 * @param[in] inputs The view's slot inputs, snapshotted before the pass
 * @param[in] available The available extent (ResolveStandaloneAvailableExtent)
 * @return The constraint to measure the view at, in visual units, never negative
 */
inline StandaloneExtent DeriveStandaloneMeasureConstraint(const StandaloneSlotInputs& inputs, const StandaloneExtent& available)
{
  StandaloneExtent constraint = available;

  // MATCH_PARENT axes take the available extent clamped by the view's own min/max: the slot
  // DeriveStandaloneRootBounds places them in (the same expression, so the same value). They
  // are measured EXACTLY, which reports the constraint, so the measured size equals the slot.
  if(inputs.matchWidth)
  {
    constraint.width = std::min(std::max(available.width, inputs.minimumWidth * inputs.scale), inputs.maximumWidth * inputs.scale);
  }
  if(inputs.matchHeight)
  {
    constraint.height = std::min(std::max(available.height, inputs.minimumHeight * inputs.scale), inputs.maximumHeight * inputs.scale);
  }

  const float marginW = static_cast<float>(inputs.margin.start + inputs.margin.end) * inputs.scale;
  const float marginH = static_cast<float>(inputs.margin.top + inputs.margin.bottom) * inputs.scale;

  // An explicit size is measured as is, window or not; the margin lies outside the box. A
  // WRAP_CONTENT axis is bounded by the window when there is one, else it keeps the
  // available extent.
  if(inputs.requestedWidth >= 0.0f)
  {
    constraint.width = inputs.requestedWidth * inputs.scale;
  }
  else if(!inputs.matchWidth && inputs.windowBounded)
  {
    constraint.width = std::max(0.0f, inputs.windowWidth - marginW);
  }

  if(inputs.requestedHeight >= 0.0f)
  {
    constraint.height = inputs.requestedHeight * inputs.scale;
  }
  else if(!inputs.matchHeight && inputs.windowBounded)
  {
    constraint.height = std::max(0.0f, inputs.windowHeight - marginH);
  }

  return constraint;
}

/**
 * @brief The measure modes matching DeriveStandaloneMeasureConstraint().
 *
 * A MATCH_PARENT axis is measured at exactly the extent DeriveStandaloneRootBounds() places it
 * in, so it is EXACTLY; every other axis keeps the at-most budget (an explicit size is
 * canonicalised to AT_MOST by Measure() anyway). Used by all three places that measure a boundary
 * view and by the regular layout-root path, so the same snapshot yields the same modes -- and,
 * with the same constraint, the same measure cache KEY -- on every path.
 *
 * @param[in] inputs The view's slot inputs, snapshotted before the pass
 * @return The modes to measure the view at
 */
inline MeasureModes DeriveStandaloneMeasureModes(const StandaloneSlotInputs& inputs)
{
  return MeasureModes{inputs.matchWidth ? MeasureMode::EXACTLY : MeasureMode::AT_MOST,
                      inputs.matchHeight ? MeasureMode::EXACTLY : MeasureMode::AT_MOST};
}

/**
 * @brief THE one derivation of a boundary (STANDALONE) view's own layout slot.
 *
 * Called by BOTH LayoutController::ProcessLayoutRoot -- the framework-owned self pass
 * of a boundary view driven as a layout root in its own right -- and by
 * ArrangeStandaloneChild, the parent-driven placement of the same view, each right
 * after measuring the view at DeriveStandaloneMeasureConstraint() of the same snapshot
 * and available extent. Both paths take the available extent from the same parent
 * LAYOUT TARGET -- the bounds the parent was last arranged at -- so, given the same
 * snapshot, the same parent target and the same window, sharing both functions makes
 * the two results identical. That is the premise
 * ViewDataImpl::InvalidateParentArrangeCacheForOutOfBandArrange's
 * framework-root-pass exemption rests on: that exemption leaves the parent's arrange
 * entry standing across a boundary view's self pass, and it may only do so while the
 * self pass produces the very bounds the parent's next miss would hand it. The parent
 * target is the parent's LAST arrange result, not a freshness guarantee across every
 * lifecycle (HasArrangeResult() is cleared only for a reparented direct child); the
 * parent's next pass re-derives it and re-places the view.
 *
 * @p measured is passed in rather than read from the view: it is the result of the
 * single Measure() the caller has just issued at DeriveStandaloneMeasureConstraint() of
 * the same snapshot and available extent, and the slot is derived from THAT measurement.
 * @p inputs is passed in for the same reason and covers every OTHER property the
 * derivation reads: nothing is re-read from the view here, so a producer that mutated
 * its own scale, margin or requested size mid-pass cannot make the extent and the slot
 * disagree.
 *
 * @param[in] inputs The view's slot inputs, snapshotted before the pass
 * @param[in] availableWidth The available extent (ResolveStandaloneAvailableExtent) on the width axis
 * @param[in] availableHeight The available extent (ResolveStandaloneAvailableExtent) on the height axis
 * @param[in] measured The measured size that Measure() returned
 * @return The view's own layout slot, in visual (scale-applied) units
 */
inline LayoutRect DeriveStandaloneRootBounds(const StandaloneSlotInputs& inputs, float availableWidth, float availableHeight, const MeasuredSize& measured)
{
  LayoutRect bounds;

  // ONE float association for the position, deliberately: the arrange cache KEY is an
  // EXACT compare (SameLayoutRect), so associating the multiply differently on the two
  // call sites could produce two values that differ in the last bit at scale != 1 and
  // could then never match.
  bounds.x = (inputs.requestedX + static_cast<float>(inputs.margin.start)) * inputs.scale;
  bounds.y = (inputs.requestedY + static_cast<float>(inputs.margin.top)) * inputs.scale;

  bounds.width  = ResolveStandaloneExtent(inputs.matchWidth, availableWidth, measured.width);
  bounds.height = ResolveStandaloneExtent(inputs.matchHeight, availableHeight, measured.height);

  // Its own min/max is enforced here, so the slot itself carries the clamp every view
  // applies at arrange (ViewDataImpl::ClampToOwnSizeBounds), which is then idempotent.
  // For a MATCH_PARENT axis the measured value was discarded above in favour of the
  // available extent, which this clamps to the value DeriveStandaloneMeasureConstraint
  // measured it at; for the others ApplyConstraints already applied the same clamp during
  // Measure. Idempotent either way.
  bounds.width  = std::min(std::max(bounds.width, inputs.minimumWidth * inputs.scale), inputs.maximumWidth * inputs.scale);
  bounds.height = std::min(std::max(bounds.height, inputs.minimumHeight * inputs.scale), inputs.maximumHeight * inputs.scale);

  return bounds;
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
