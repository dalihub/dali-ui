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
#include <dali-ui-foundation/public-api/layouts/layout-bounds-effects.h>

namespace Dali
{
namespace Ui
{
namespace LayoutBoundsEffects
{
namespace
{
LayoutBoundsLength NegatedLength(const LayoutBoundsLength& length)
{
  return {-length.value, length.unit};
}
} // namespace

LayoutBoundsEffect SlideFrom(LayoutBoundsEdge              edge,
                             const LayoutTransitionTiming& timing)
{
  // Default slide distance is one self-fraction along the slide axis.
  return SlideFrom(edge, LayoutBoundsLength::SelfFraction(1.0f), timing);
}

LayoutBoundsEffect SlideTo(LayoutBoundsEdge              edge,
                           const LayoutTransitionTiming& timing)
{
  return SlideTo(edge, LayoutBoundsLength::SelfFraction(1.0f), timing);
}

LayoutBoundsEffect SlideFrom(LayoutBoundsEdge              edge,
                             LayoutBoundsLength            distance,
                             const LayoutTransitionTiming& timing)
{
  LayoutBoundsEffect effect;
  effect.SetTiming(timing);

  // Edge selects the default sign; the distance value is signed so callers
  // can pass a negative magnitude to flip the direction.
  switch(edge)
  {
    case LayoutBoundsEdge::TOP:
      effect.SetOffset(LayoutBoundsLength::Pixel(0.0f), NegatedLength(distance));
      break;
    case LayoutBoundsEdge::BOTTOM:
      effect.SetOffset(LayoutBoundsLength::Pixel(0.0f), distance);
      break;
    case LayoutBoundsEdge::LEFT:
      effect.SetOffset(NegatedLength(distance), LayoutBoundsLength::Pixel(0.0f));
      break;
    case LayoutBoundsEdge::RIGHT:
      effect.SetOffset(distance, LayoutBoundsLength::Pixel(0.0f));
      break;
  }

  return effect;
}

LayoutBoundsEffect SlideTo(LayoutBoundsEdge              edge,
                           LayoutBoundsLength            distance,
                           const LayoutTransitionTiming& timing)
{
  // Mirror semantics in the dispatcher (ENTER endpoint → base,
  // EXIT base → endpoint) mean SlideTo reuses the same effect value.
  return SlideFrom(edge, distance, timing);
}

LayoutBoundsEffect ExpandFrom(LayoutBoundsEdge              anchor,
                              const LayoutTransitionTiming& timing)
{
  LayoutBoundsEffect effect;
  effect.SetTiming(timing);

  switch(anchor)
  {
    case LayoutBoundsEdge::LEFT:
      effect.SetSizeFactor(0.0f, 1.0f).SetAnchor(0.0f, 0.5f);
      break;
    case LayoutBoundsEdge::RIGHT:
      effect.SetSizeFactor(0.0f, 1.0f).SetAnchor(1.0f, 0.5f);
      break;
    case LayoutBoundsEdge::TOP:
      effect.SetSizeFactor(1.0f, 0.0f).SetAnchor(0.5f, 0.0f);
      break;
    case LayoutBoundsEdge::BOTTOM:
      effect.SetSizeFactor(1.0f, 0.0f).SetAnchor(0.5f, 1.0f);
      break;
  }

  return effect;
}

LayoutBoundsEffect ShrinkTo(LayoutBoundsEdge              anchor,
                            const LayoutTransitionTiming& timing)
{
  // Mirror semantics: identical effect, EXIT slot consumes it as
  // base → endpoint (full size → zero collapsed at anchor).
  return ExpandFrom(anchor, timing);
}

} // namespace LayoutBoundsEffects
} // namespace Ui
} // namespace Dali
