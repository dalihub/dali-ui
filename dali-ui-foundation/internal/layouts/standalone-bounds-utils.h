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
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
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
 * UNCLAMPED on purpose: a caller that re-measures the view against the extent it is
 * about to be placed in must feed it THIS value and not the clamped result, so that
 * the measure cache KEY converges too. The clamp belongs to the placement (see
 * DeriveStandaloneRootBounds), not to the constraint the producer is run at.
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
 * @brief The inputs of a boundary view's slot derivation, captured at ONE instant.
 *
 * Both derivations -- the framework-owned self pass and the parent-driven placement --
 * must be fed the SAME values, and each caller must feed the values it derived its
 * available extent from. A measure producer that mutates its own scale, margin or
 * requested size mid-pass would otherwise leave the extent resolved against one set of
 * values and the slot against another. Taking the snapshot before Measure() and passing
 * it through makes that mixing impossible BY CONSTRUCTION rather than by convention.
 */
struct StandaloneSlotInputs
{
  float  scale;
  Insets margin;
  float  requestedX;
  float  requestedY;
  float  minimumWidth;
  float  maximumWidth;
  float  minimumHeight;
  float  maximumHeight;
  bool   matchWidth;
  bool   matchHeight;
};

/**
 * @brief Captures every input DeriveStandaloneRootBounds needs from @p view.
 *
 * Call this BEFORE the pass that may re-measure @p view, and hand the result to both
 * the available-extent computation and DeriveStandaloneRootBounds.
 *
 * @param[in] view The boundary view being placed
 * @return The view's slot inputs at this instant
 */
inline StandaloneSlotInputs SnapshotStandaloneSlotInputs(const ViewImpl& view)
{
  StandaloneSlotInputs inputs;

  inputs.scale         = view.GetEffectiveScale();
  inputs.margin        = view.GetMargin();
  inputs.requestedX    = view.GetRequestedX();
  inputs.requestedY    = view.GetRequestedY();
  inputs.minimumWidth  = view.GetMinimumWidth();
  inputs.maximumWidth  = view.GetMaximumWidth();
  inputs.minimumHeight = view.GetMinimumHeight();
  inputs.maximumHeight = view.GetMaximumHeight();
  inputs.matchWidth    = view.GetRequestedWidth() == MATCH_PARENT;
  inputs.matchHeight   = view.GetRequestedHeight() == MATCH_PARENT;

  return inputs;
}

/**
 * @brief THE one derivation of a boundary (STANDALONE) view's own layout slot.
 *
 * Called by BOTH LayoutController::ProcessLayoutRoot -- the framework-owned self pass
 * of a boundary view driven as a layout root in its own right -- and by
 * ArrangeStandaloneChild, the parent-driven placement of the same view. Sharing one
 * function is what makes the two results converge BY CONSTRUCTION, which is the
 * premise ViewDataImpl::InvalidateParentArrangeCacheForOutOfBandArrange's
 * framework-root-pass exemption rests on: that exemption leaves the parent's arrange
 * entry standing across a boundary view's self pass, and it may only do so while the
 * self pass produces the very bounds the parent's next miss would hand it.
 *
 * @p measured is passed in rather than read from the view so that a caller which
 * re-measures after taking its snapshot still derives the slot from the SNAPSHOT --
 * the value the extents were resolved against. @p inputs is passed in for the same
 * reason and covers every OTHER property the derivation reads: nothing is re-read from
 * the view here, so a producer that mutated its own scale, margin or requested size
 * mid-pass cannot make the extent and the slot disagree.
 *
 * @param[in] inputs The view's slot inputs, snapshotted before the pass
 * @param[in] availableWidth The width extent the parent (or the root pass) makes available
 * @param[in] availableHeight The height extent the parent (or the root pass) makes available
 * @param[in] measured The view's measured size snapshot
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

  // A boundary view has no parent layout to clamp it, so its own min/max is enforced
  // here. For a MATCH_PARENT axis the measured value was discarded above, so this is
  // the only place min/max reaches it; for the others ApplyConstraints already applied
  // the same clamp during Measure and this is idempotent.
  bounds.width  = std::min(std::max(bounds.width, inputs.minimumWidth * inputs.scale), inputs.maximumWidth * inputs.scale);
  bounds.height = std::min(std::max(bounds.height, inputs.minimumHeight * inputs.scale), inputs.maximumHeight * inputs.scale);

  return bounds;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
