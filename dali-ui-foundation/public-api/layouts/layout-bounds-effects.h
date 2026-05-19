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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Physical edge identifier used by bounds-effect factories.
 *
 * @c LayoutBoundsEdge values are physical screen edges — RTL-aware
 * @c START / @c END are intentionally omitted from the first revision so
 * that the factory's geometry semantics stay easy to test and document.
 * @c VisualBoundsOf already mirrors @c x on RTL parents, so a physical
 * @c LEFT slide still appears on the visual left for an RTL layout.
 */
enum class LayoutBoundsEdge : uint8_t
{
  TOP    = 0,
  BOTTOM = 1,
  LEFT   = 2,
  RIGHT  = 3
};

/**
 * @brief Factory helpers that build common @c LayoutBoundsEffect values.
 *
 * The factories return plain @c LayoutBoundsEffect values; the caller
 * passes them to @c LayoutTransition::SetEnterBoundsEffect /
 * @c SetExitBoundsEffect. Because the dispatcher interprets the effect
 * with @b mirror semantics (ENTER plays endpoint → base, EXIT plays
 * base → endpoint), the same factory output is reused for both directions:
 * @c SlideFrom and @c SlideTo are identical at the effect level.
 *
 * @code
 *   auto t = LayoutTransition::New();
 *   t.SetEnterBoundsEffect(
 *     LayoutBoundsEffects::SlideFrom(LayoutBoundsEdge::BOTTOM,
 *                                    {Duration(0.3f),
 *                                     AlphaFunction(AlphaFunction::EASE_OUT),
 *                                     Duration()}));
 * @endcode
 */
namespace LayoutBoundsEffects
{

/**
 * @brief Slide a view in from / out toward @p edge using the default
 * distance of one child width (LEFT/RIGHT) or one child height (TOP/BOTTOM).
 */
DALI_UI_API LayoutBoundsEffect SlideFrom(LayoutBoundsEdge              edge,
                                         const LayoutTransitionTiming& timing);

/**
 * @copydoc Dali::Ui::LayoutBoundsEffects::SlideFrom(LayoutBoundsEdge,const LayoutTransitionTiming&)
 *
 * Identical effect value as @c SlideFrom; mirror semantics in the
 * dispatcher mean the same descriptor produces an enter or an exit
 * transition based on which slot consumes it.
 */
DALI_UI_API LayoutBoundsEffect SlideTo(LayoutBoundsEdge              edge,
                                       const LayoutTransitionTiming& timing);

/**
 * @brief Slide using a custom distance. The @c value of @p distance is
 * treated as a signed magnitude; negative values reverse the direction
 * implied by @p edge.
 */
DALI_UI_API LayoutBoundsEffect SlideFrom(LayoutBoundsEdge              edge,
                                         LayoutBoundsLength            distance,
                                         const LayoutTransitionTiming& timing);

/**
 * @copydoc Dali::Ui::LayoutBoundsEffects::SlideFrom(LayoutBoundsEdge,LayoutBoundsLength,const LayoutTransitionTiming&)
 */
DALI_UI_API LayoutBoundsEffect SlideTo(LayoutBoundsEdge              edge,
                                       LayoutBoundsLength            distance,
                                       const LayoutTransitionTiming& timing);

/**
 * @brief Expand the view from a zero size pinned at the given anchor edge.
 *
 * @c LEFT / @c RIGHT anchors collapse along the x-axis and animate the
 * width; @c TOP / @c BOTTOM anchors collapse along the y-axis.
 */
DALI_UI_API LayoutBoundsEffect ExpandFrom(LayoutBoundsEdge              anchor,
                                          const LayoutTransitionTiming& timing);

/**
 * @copydoc Dali::Ui::LayoutBoundsEffects::ExpandFrom
 *
 * Identical effect value; mirror semantics select shrink or expand based
 * on slot.
 */
DALI_UI_API LayoutBoundsEffect ShrinkTo(LayoutBoundsEdge              anchor,
                                        const LayoutTransitionTiming& timing);

} // namespace LayoutBoundsEffects

} // namespace Ui
} // namespace Dali
