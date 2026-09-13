#ifndef DALI_UI_TEXT_ELLIPSIS_PLANNER_H
#define DALI_UI_TEXT_ELLIPSIS_PLANNER_H

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
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/glyph-info.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/character-spacing-glyph-run.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace DALI_NAMESPACE::Ui::Text
{
/**
 * @brief Provides non-owning input for END ellipsis planning.
 */
struct EndEllipsisInputView
{
  const TextAbstraction::GlyphInfo*       glyphs{nullptr};
  const Vector2*                          glyphPositions{nullptr};
  const Character*                        text{nullptr};
  const CharacterIndex*                   glyphToCharacterMap{nullptr};
  const Vector<CharacterSpacingGlyphRun>* characterSpacingRuns{nullptr};
  Length                                  numberOfGlyphs{0u};
  GlyphIndex                              glyphPositionStartIndex{0u};
  Length                                  numberOfGlyphPositions{0u};
  Length                                  numberOfCharacters{0u};
  GlyphIndex                              startIndex{0u};
  float                                   lineWidth{0.0f};
  float                                   positionOffset{0.0f};
  float                                   modelCharacterSpacing{0.0f};
};

/**
 * @brief Stores the result of END ellipsis planning.
 */
struct EndEllipsisPlan
{
  static constexpr GlyphIndex INVALID_GLYPH_INDEX = std::numeric_limits<GlyphIndex>::max();

  TextAbstraction::GlyphInfo ellipsisGlyph{};
  Vector2                    ellipsisPosition{Vector2::ZERO};
  GlyphIndex                 ellipsisGlyphIndex{INVALID_GLYPH_INDEX};
  GlyphIndex                 firstRemovedReplacementGlyphIndex{INVALID_GLYPH_INDEX};
  GlyphIndex                 lastRemovedReplacementGlyphIndex{INVALID_GLYPH_INDEX};
  Length                     numberOfRemovedGlyphs{0u};
  bool                       resolved{false};
};

/**
 * @brief Resolves the existing END ellipsis font policy for a source glyph.
 *
 * A font-backed source is returned directly. A synthetic replacement source
 * uses the nearest font-backed glyph, with the preceding glyph preferred at
 * equal distance. Other font-free glyphs do not trigger a surrounding search.
 *
 * @param[in] glyphs The source glyph buffer.
 * @param[in] numberOfGlyphs The number of source glyphs.
 * @param[in] glyphIndex The END ellipsis font source.
 * @return The resolved font id, or zero when no valid source exists.
 */
FontId ResolveEndEllipsisFontId(const TextAbstraction::GlyphInfo* glyphs,
                                Length                            numberOfGlyphs,
                                GlyphIndex                        glyphIndex);

/**
 * @brief Resolves the END ellipsis removal boundary without modifying the input.
 *
 * @param[in] input The END ellipsis input.
 * @param[in] fontClient The font client used to resolve ellipsis metrics.
 * @return The resolved END ellipsis plan.
 */
EndEllipsisPlan ResolveEndEllipsisPlan(const EndEllipsisInputView&  input,
                                       TextAbstraction::FontClient& fontClient);

} //namespace DALI_NAMESPACE::Ui::Text

#endif // DALI_UI_TEXT_ELLIPSIS_PLANNER_H
