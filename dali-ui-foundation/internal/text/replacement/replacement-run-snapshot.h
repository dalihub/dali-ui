#ifndef DALI_UI_TEXT_REPLACEMENT_RUN_SNAPSHOT_H
#define DALI_UI_TEXT_REPLACEMENT_RUN_SNAPSHOT_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>
#include <cstdint>
#include <limits>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/character-run.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace Dali::Ui::Text
{
/**
 * @brief Enumerates the replacement types known by the text processing layer.
 */
enum class ReplacementType : uint8_t
{
  GENERIC,
  IMAGE
};

/**
 * @brief Enumerates replacement alignment relative to surrounding text.
 */
enum class ReplacementVerticalAlignment : uint8_t
{
  TEXT_BOTTOM = 0,
  TEXT_BASELINE,
  TEXT_CENTER
};

/**
 * @brief Identifies a non-font glyph used to reserve replacement layout space.
 */
constexpr GlyphIndex SYNTHETIC_REPLACEMENT_GLYPH_ID = std::numeric_limits<GlyphIndex>::max();

/**
 * @brief Checks whether a glyph represents an inline replacement unit.
 *
 * @param[in] glyph The glyph to inspect.
 * @return true if the glyph is a synthetic replacement glyph.
 */
inline bool IsSyntheticReplacementGlyph(const GlyphInfo& glyph)
{
  return glyph.fontId == 0u && glyph.index == SYNTHETIC_REPLACEMENT_GLYPH_ID;
}

/**
 * @brief Stores copy-safe authored replacement metrics.
 *
 * All values use content-local logical pixels.
 */
struct ReplacementMetrics
{
  float                        width{0.0f};
  float                        height{0.0f};
  float                        verticalOffset{0.0f};
  ReplacementVerticalAlignment verticalAlignment{ReplacementVerticalAlignment::TEXT_BOTTOM};
};

/**
 * @brief Stores the copy-safe authored ImageSpan descriptor.
 *
 * Runtime handles and decoded resources are deliberately excluded.
 */
struct ImageReplacementDescriptor
{
  std::string source;
};

/**
 * @brief Stores a copy-safe authored replacement source.
 *
 * No BaseHandle, Visual, Actor, Texture or application callback is retained.
 * The character range uses the original UTF-32 domain.
 */
struct ReplacementRunSnapshot
{
  CharacterRun               logicalCharacterRange{};
  ReplacementMetrics         metrics{};
  ReplacementType            type{ReplacementType::GENERIC};
  uint64_t                   occurrenceIdentity{0u};
  ImageReplacementDescriptor image{};
};

/**
 * @brief Stores request-local replacement source data.
 *
 * The snapshot is shared by the synchronous controller and asynchronous worker.
 */
struct ReplacementSourceSnapshot
{
  Vector<ReplacementRunSnapshot> runs;
  uint64_t                       sourceRevision{0u};
  bool                           hasValidReplacementSource{false};
};

/**
 * @brief Stores the font metric used to draw a caret at a replacement boundary.
 */
struct ReplacementCaretMetric
{
  float ascender{0.0f};
  float height{0.0f};
};

/**
 * @brief Stores the final placement of a replacement.
 *
 * The position is content-local after text alignment and before control padding.
 * A visible placement contains a synthetic unit that survived END ellipsis.
 */
struct ReplacementPlacement
{
  CharacterRun           logicalCharacterRange{}; ///< Original logical UTF-32 domain.
  uint32_t               sourceRunIndex{0u};      ///< Index in the request-local authored snapshot.
  uint64_t               occurrenceIdentity{0u};
  GlyphIndex             syntheticGlyphIndex{0u}; ///< Glyph domain; never inferred from charactersPerGlyph.
  LineIndex              lineIndex{0u};
  Vector2                position{};
  Vector2                size{};
  ReplacementCaretMetric leadingCaretMetric{};
  ReplacementCaretMetric trailingCaretMetric{};
  float                  baseline{0.0f};
  CharacterDirection     lineDirection{false};
  bool                   visible{false};
  bool                   elided{false};
};

/**
 * @brief Stores normalized Reveal timing for one visible replacement visual.
 *
 * The occurrence identity is stable within the corresponding source revision.
 * Runtime visual handles and final glyph indices are deliberately excluded.
 */
struct ReplacementRevealTiming
{
  uint64_t occurrenceIdentity{0u};
  float    start{0.0f};
  float    fadeDuration{0.0f};
  float    progressionSpan{0.0f};
  bool     rightToLeft{false};
};

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_RUN_SNAPSHOT_H
