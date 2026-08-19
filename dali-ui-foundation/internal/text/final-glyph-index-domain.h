#ifndef DALI_UI_TEXT_FINAL_GLYPH_INDEX_DOMAIN_H
#define DALI_UI_TEXT_FINAL_GLYPH_INDEX_DOMAIN_H

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

#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace Dali::Ui::Text
{
/**
 * @brief Resolves a final glyph index to its authored style glyph.
 *
 * The style map is indexed in the complete final-glyph domain. Generated
 * glyphs therefore inherit authored style without acquiring source identity.
 */
inline GlyphIndex ResolveFinalStyleSourceGlyph(const GlyphIndex* finalToStyleGlyphIndices,
                                               GlyphIndex        finalGlyphIndex)
{
  return finalToStyleGlyphIndices ? finalToStyleGlyphIndices[finalGlyphIndex] : finalGlyphIndex;
}

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_FINAL_GLYPH_INDEX_DOMAIN_H
