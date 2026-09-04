#ifndef DALI_UI_TEXT_ELLIPSIS_RESOLVER_H
#define DALI_UI_TEXT_ELLIPSIS_RESOLVER_H

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
#include <dali/public-api/actors/actor-enumerations.h>
#include <limits>
// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/final-elision-result.h>
#include <dali-ui-foundation/internal/text/layouts/layout-engine.h>
#include <dali-ui-foundation/internal/text/text-model.h>

namespace Dali::Ui::Text
{

/**
 * @brief Provides request-local glyph positions for an END candidate line.
 *
 * LayoutEngine can resolve a candidate into a temporary position buffer before
 * that buffer is committed to VisualModel. Positions outside this view continue
 * to come from the model.
 */
struct EndEllipsisGlyphPositionView
{
  const Vector2* glyphPositions{nullptr};
  GlyphIndex     startIndex{0u};
  Length         numberOfGlyphs{0u};
};

/**
 * @brief Stores the authoritative source-retention boundary for an END line.
 */
struct EndEllipsisSourceRetention
{
  static constexpr CharacterIndex INVALID_CHARACTER_INDEX = std::numeric_limits<CharacterIndex>::max();

  CharacterIndex firstRemovedCharacterIndex{INVALID_CHARACTER_INDEX};
  bool           resolved{false};

  bool IsRetained(CharacterIndex characterIndex) const
  {
    return resolved &&
           (firstRemovedCharacterIndex == INVALID_CHARACTER_INDEX ||
            characterIndex < firstRemovedCharacterIndex);
  }
};

/**
 * @brief Resolves only the authoritative source-retention boundary for an END candidate.
 *
 * This uses the same candidate selection as ResolveEndEllipsis(). It exists so
 * replacement-aware line metrics can exclude only source units that the final
 * END result actually removes.
 */
EndEllipsisSourceRetention ResolveEndEllipsisRetention(
  const Model&                        model,
  const LineRun&                      line,
  const Size&                         controlSize,
  TextAbstraction::FontClient&        fontClient,
  FontId                              syntheticReplacementDefaultFontId,
  const EndEllipsisGlyphPositionView& glyphPositionView = {});

/**
 * @brief Resolves an END-elided line from source-layout topology.
 *
 * A cluster-safe logical prefix is retained. Its source-shaped glyphs and
 * physical order are projected into the final domain, and U+2026 occupies the
 * physical slot of the first removed source cluster with that cluster's
 * resolved direction affinity. No neutral ellipsis character is reintroduced
 * into BiDi resolution. The source model remains immutable; @p result is the
 * sole final-layout authority.
 *
 * The caller supplies a model for which effective elision is enabled and
 * LayoutEngine has either marked an END candidate or produced no visible line.
 * @p syntheticReplacementDefaultFontId is used only when a synthetic
 * replacement boundary has no font-backed source glyph.
 *
 * @return true when an authoritative END result, including an intentional omission, was resolved.
 */
bool ResolveEndEllipsis(const Model&                 model,
                        const Size&                  controlSize,
                        TextAbstraction::FontClient& fontClient,
                        FinalElisionResult&          result,
                        FontId                       syntheticReplacementDefaultFontId = 0u);

/**
 * @brief Aligns final-domain lines and builds View/atlas coordinates.
 */
void FinalizeEndEllipsisGeometry(const Model&                model,
                                 const Size&                 controlSize,
                                 Dali::LayoutDirection::Type layoutDirection,
                                 bool                        matchLayoutDirection,
                                 Layout::Engine&             layoutEngine,
                                 FinalElisionResult&         result);

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_ELLIPSIS_RESOLVER_H
