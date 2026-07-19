#ifndef DALI_UI_TEXT_REPLACEMENT_GLYPH_HELPER_H
#define DALI_UI_TEXT_REPLACEMENT_GLYPH_HELPER_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/metrics.h>
#include <dali-ui-foundation/internal/text/multi-language-support.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-processing-source.h>
#include <dali-ui-foundation/internal/text/shaper.h>
#include <dali-ui-foundation/internal/text/visual-model-impl.h>

namespace Dali::Ui::Text
{
/**
 * @brief Validates fonts while preserving synthetic replacement units.
 *
 * @param[in] multilanguageSupport The service used to validate fonts.
 * @param[in] fontClient The font client used by the text pipeline.
 * @param[in] source The text processing source.
 * @param[in] scripts The script runs for the processing text.
 * @param[in] defaultFontDescription The default font description.
 * @param[in] defaultPointSize The default point size.
 * @param[in] effectiveScale The effective font scale.
 * @param[in] startIndex The first character to validate.
 * @param[in] numberOfCharacters The number of characters to validate.
 * @param[out] validFonts The resulting validated font runs.
 * @param[in] variationsMapPtr The optional font variation map.
 */
void ValidateFontsForProcessingSource(MultilanguageSupport&                   multilanguageSupport,
                                      TextAbstraction::FontClient&            fontClient,
                                      const TextProcessingSource&             source,
                                      const Vector<ScriptRun>&                scripts,
                                      const TextAbstraction::FontDescription& defaultFontDescription,
                                      TextAbstraction::PointSize26Dot6        defaultPointSize,
                                      float                                   effectiveScale,
                                      CharacterIndex                          startIndex,
                                      Length                                  numberOfCharacters,
                                      Vector<FontRun>&                        validFonts,
                                      Property::Map*                          variationsMapPtr);

/**
 * @brief Shapes ordinary text and emits synthetic replacement glyphs.
 *
 * @param[in] shaping The shaping service used by the text pipeline.
 * @param[in] fontClient The font client used by the text pipeline.
 * @param[in] source The text processing source.
 * @param[in] textToShape The processing text to shape.
 * @param[in] lineBreakInfo The line-break data for the processing text.
 * @param[in] scripts The script runs for the processing text.
 * @param[in] validFonts The validated font runs.
 * @param[in] startIndex The first character to shape.
 * @param[in] startGlyphIndex The first output glyph index.
 * @param[in] numberOfCharacters The number of characters to shape.
 * @param[in,out] visualModel The visual model receiving shaped glyph data.
 * @param[out] newParagraphGlyphs The glyph indices that begin new paragraphs.
 */
void ShapeTextForProcessingSource(TextAbstraction::Shaping&    shaping,
                                  TextAbstraction::FontClient& fontClient,
                                  const TextProcessingSource&  source,
                                  const Vector<Character>&     textToShape,
                                  const Vector<LineBreakInfo>& lineBreakInfo,
                                  const Vector<ScriptRun>&     scripts,
                                  const Vector<FontRun>&       validFonts,
                                  CharacterIndex               startIndex,
                                  GlyphIndex                   startGlyphIndex,
                                  Length                       numberOfCharacters,
                                  VisualModel&                 visualModel,
                                  Vector<GlyphIndex>&          newParagraphGlyphs);

/**
 * @brief Retrieves metrics for ordinary glyphs while preserving replacement metrics.
 *
 * @param[in] metrics The glyph metrics service.
 * @param[in] source The text processing source.
 * @param[in,out] glyphs The glyph buffer to update.
 * @param[in] startGlyphIndex The first glyph to update.
 * @param[in] numberOfGlyphs The number of glyphs to update.
 * @param[in] newParagraphGlyphs The glyph indices that begin new paragraphs.
 */
void GetGlyphMetricsForProcessingSource(Metrics&                    metrics,
                                        const TextProcessingSource& source,
                                        Vector<GlyphInfo>&          glyphs,
                                        GlyphIndex                  startGlyphIndex,
                                        Length                      numberOfGlyphs,
                                        const Vector<GlyphIndex>&   newParagraphGlyphs);

/**
 * @brief Converts logical character style runs to visual glyph style runs.
 *
 * Replacement units are omitted from the resulting glyph styles.
 *
 * @param[in] logicalModel The logical model containing character style runs.
 * @param[in,out] visualModel The visual model receiving glyph style runs.
 */
void CopyProcessingCharacterStylesToVisual(const LogicalModel& logicalModel, VisualModel& visualModel);

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_GLYPH_HELPER_H
