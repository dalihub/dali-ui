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

// FILE HEADER
#include <dali-ui-foundation/internal/text/gradient-glyph-classification.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/color-glyph-helper.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{
namespace
{

bool HasExplicitColor(const ColorIndex* colorIndices, GlyphIndex glyphIndex)
{
  return (nullptr != colorIndices) && (0u != *(colorIndices + glyphIndex));
}

} // namespace

GradientGlyphInfo ClassifyGradientGlyph(TextAbstraction::FontClient& fontClient,
                                        const GlyphInfo&             glyph,
                                        const ColorIndex*            colorIndices,
                                        GlyphIndex                   glyphIndex)
{
  GradientGlyphInfo classification;
  classification.isColorGlyph     = IsRenderableColorGlyph(fontClient, glyph.fontId, glyph.index);
  classification.hasExplicitColor = HasExplicitColor(colorIndices, glyphIndex);
  // FontClient reserves fontId 0 plus a non-zero cache index for embedded
  // items. A zero font id alone is also used as a generic "not found" value.
  classification.isEmbeddedItem   = (0u == glyph.fontId) && (0u != glyph.index);
  classification.usesGradientFill = !classification.isColorGlyph && !classification.hasExplicitColor &&
                                    !classification.isEmbeddedItem;

  return classification;
}

} // namespace Internal

} // namespace Text

} // namespace Ui

} // namespace Dali
