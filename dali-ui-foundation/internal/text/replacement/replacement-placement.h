#ifndef DALI_UI_TEXT_REPLACEMENT_PLACEMENT_H
#define DALI_UI_TEXT_REPLACEMENT_PLACEMENT_H

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

#include <dali-ui-foundation/internal/text/final-elision-result.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>
#include <dali-ui-foundation/internal/text/text-model.h>
#include <dali/devel-api/text-abstraction/font-client.h>

namespace Dali::Ui::Text
{
/**
 * @brief Extracts replacement placements from the final text layout.
 *
 * @param[in] model The model containing the final line layout.
 * @param[in] projection The active replacement projection.
 * @param[in] finalElision The final glyph sequence and visibility mapping.
 * @param[in] fontClient The font service used to resolve surrounding text metrics.
 * @param[in] defaultFontId The font used when a replacement line has no visible text glyph.
 * @param[out] placements The extracted content-local replacement placements.
 */
void ExtractReplacementPlacements(const Model&                  model,
                                  const ReplacementProjection&  projection,
                                  const FinalElisionResult&     finalElision,
                                  TextAbstraction::FontClient&  fontClient,
                                  FontId                        defaultFontId,
                                  Vector<ReplacementPlacement>& placements);

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_PLACEMENT_H
