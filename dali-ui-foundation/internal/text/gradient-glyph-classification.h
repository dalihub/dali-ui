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

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_GLYPH_CLASSIFICATION_H
#define DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_GLYPH_CLASSIFICATION_H

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{
struct GradientGlyphInfo
{
  bool usesGradientFill{false};
  bool isColorGlyph{false};
  bool hasExplicitColor{false};
};

/**
 * @brief Classifies whether a glyph is eligible for TextGradient fill.
 *
 * The helper only classifies one glyph. It does not check whether TextGradient
 * is enabled and must only be called from a future gradient-enabled branch.
 *
 * @param[in] fontClient The font client used for color glyph detection.
 * @param[in] glyph The glyph to classify.
 * @param[in] colorIndices Optional glyph color index buffer.
 * @param[in] glyphIndex The glyph's index in @p colorIndices.
 * @return The gradient classification result.
 */
GradientGlyphInfo ClassifyGradientGlyph(TextAbstraction::FontClient& fontClient,
                                        const GlyphInfo&             glyph,
                                        const ColorIndex*            colorIndices,
                                        GlyphIndex                   glyphIndex);

} // namespace Internal

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_GLYPH_CLASSIFICATION_H
