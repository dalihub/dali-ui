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
#include <dali/public-api/common/dali-vector.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/character-run.h>
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{

using GradientSpanPaintIndex = uint32_t;

/**
 * @brief Immutable authored values for one distinct GradientSpan attachment.
 */
struct GradientSpanPaint
{
  Gradient::Style          style{};
  GradientSpan::BoundsMode boundsMode{GradientSpan::BoundsMode::SPAN_BOUND};
};

/**
 * @brief A non-overlapping logical range owned by one gradient paint.
 */
struct GradientSpanCharacterRun
{
  CharacterRun           characterRun{};
  GradientSpanPaintIndex paintIndex{0u}; ///< One-based index into GradientSpanModelData::paints.
};

/**
 * @brief Optional canonical GradientSpan data carried through shaping and rendering.
 *
 * The containing LogicalModel allocates this sidecar only when a renderable
 * GradientSpan wins at least one character. @p glyphPaintIndices is populated
 * after shaping and uses zero for ordinary foreground paint. Keeping authored
 * paint data and glyph paint IDs separate from the current raster backend also
 * lets a scrolling texture backend consume the same model in a future GPU
 * paint-ID/LUT composition path without changing the public StyledText model.
 */
struct GradientSpanModelData
{
  Dali::Vector<GradientSpanPaint>        paints;
  Dali::Vector<GradientSpanCharacterRun> characterRuns;
  Dali::Vector<GradientSpanPaintIndex>   glyphPaintIndices;
};

/**
 * @brief Projects canonical logical GradientSpan runs into the shaped glyph domain.
 */
void SetGradientSpanSegmentationInfo(GradientSpanModelData&    data,
                                     const Vector<GlyphIndex>& charactersToGlyph,
                                     const Vector<Length>&     glyphsPerCharacter,
                                     CharacterIndex            startCharacterIndex,
                                     GlyphIndex                startGlyphIndex,
                                     Length                    numberOfCharacters);

} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali
