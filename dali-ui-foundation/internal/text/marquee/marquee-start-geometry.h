#ifndef DALI_UI_TEXT_MARQUEE_START_GEOMETRY_H
#define DALI_UI_TEXT_MARQUEE_START_GEOMETRY_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali::Ui::Text
{
struct FinalElisionResult;
class VisualModel;

/**
 * @brief Stable source identity and static control geometry retained across
 * the ellipsis-to-marquee transition.
 *
 * The character index plus the glyph's occurrence within that character is
 * stable across the separate static and natural-layout requests used by the
 * async path because both requests shape the same authored source. Font,
 * locale, direction, content, and render changes invalidate the descriptor;
 * request cancellation prevents an older async result from republishing it.
 */
struct MarqueeStartAnchor
{
  CharacterIndex characterIndex{0u};
  Length         glyphOccurrence{0u};
  float          staticControlX{0.0f}; ///< Logical pixels in the Label content box.
  bool           valid{false};
};

/** @brief Position of the retained source anchor in the generated marquee texture. */
struct MarqueeTextureAnchor
{
  float textureX{0.0f}; ///< Logical pixels from the marquee texture's left edge.
  bool  valid{false};
};

/** @brief Optional initial value for the horizontal marquee shader's uDelta. */
struct MarqueeInitialDelta
{
  float value{0.0f}; ///< Logical pixels in the scroller control/texture domain.
  bool  valid{false};
};

/**
 * @brief Resolves a retained source anchor when the visible geometry is rigid.
 *
 * The generated ellipsis and non-rendering glyphs are not geometry anchors.
 * Any malformed mapping or non-rigid retained geometry conservatively returns
 * an invalid result so marquee keeps its legacy start position.
 */
MarqueeStartAnchor ResolveMarqueeStartAnchor(const FinalElisionResult* finalElision,
                                             const VisualModel*        sourceModel);

/**
 * @brief Returns the existing horizontal marquee alignment factor.
 *
 * Overflowing horizontal marquee always uses logical START. Fitting text keeps
 * its requested alignment. The values are the established uHorizontalAlign
 * contract: -0.5 left, 0 center, +0.5 right.
 */
inline float ResolveHorizontalMarqueeAlignment(bool               isTextContentOverflow,
                                               CharacterDirection direction,
                                               Alignment          requestedAlignment)
{
  const Alignment alignment = isTextContentOverflow ? Alignment::START : requestedAlignment;
  switch(alignment)
  {
    case Alignment::START:
      return direction ? 0.5f : -0.5f;
    case Alignment::CENTER:
      return 0.0f;
    case Alignment::END:
      return direction ? -0.5f : 0.5f;
  }
  return 0.0f;
}

/**
 * @brief Resolves the texture-space origin selected by the existing shader.
 *
 * This is the exact pixel-domain expansion of text-scroller-shader.vert for
 * uDelta == 0 and control-local X == 0.
 */
inline float ResolveLegacyHorizontalMarqueeViewportOrigin(float horizontalAlignment,
                                                          float textureWidth,
                                                          float controlWidth,
                                                          float wrapGap)
{
  return (horizontalAlignment + 0.5f) * (textureWidth - controlWidth - wrapGap);
}

/** @brief Solves the shader delta that preserves the retained anchor's control X. */
inline MarqueeInitialDelta ResolveMarqueeInitialDelta(const MarqueeStartAnchor&   staticAnchor,
                                                      const MarqueeTextureAnchor& textureAnchor,
                                                      float                       horizontalAlignment,
                                                      float                       textureWidth,
                                                      float                       controlWidth,
                                                      float                       wrapGap)
{
  if(!staticAnchor.valid || !textureAnchor.valid ||
     !std::isfinite(staticAnchor.staticControlX) || !std::isfinite(textureAnchor.textureX) ||
     !std::isfinite(horizontalAlignment) || !std::isfinite(textureWidth) ||
     !std::isfinite(controlWidth) || !std::isfinite(wrapGap))
  {
    return {};
  }

  const float viewportOrigin = ResolveLegacyHorizontalMarqueeViewportOrigin(horizontalAlignment,
                                                                            textureWidth,
                                                                            controlWidth,
                                                                            wrapGap);
  const float initialDelta   = textureAnchor.textureX - viewportOrigin - staticAnchor.staticControlX;
  return std::isfinite(initialDelta) ? MarqueeInitialDelta{initialDelta, true} : MarqueeInitialDelta{};
}

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_MARQUEE_START_GEOMETRY_H
