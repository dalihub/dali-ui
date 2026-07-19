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

#include <dali-ui-foundation/internal/text/replacement/replacement-placement.h>

// EXTERNAL INCLUDES
#include <algorithm>

#include <dali-ui-foundation/internal/text/final-glyph-geometry.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-glyph-helper.h>

namespace Dali::Ui::Text
{
namespace
{
struct SurroundingMetrics
{
  float ascender{0.0f};
  float descender{0.0f};
  bool  hasVisibleText{false};
};

void IncludeGlyphMetrics(const VisualModel&           visual,
                         const FinalElisionResult&    finalElision,
                         TextAbstraction::FontClient& fontClient,
                         const GlyphRun&              glyphRun,
                         SurroundingMetrics&          surrounding)
{
  FontId                       lastFontId = 0u;
  TextAbstraction::FontMetrics fontMetrics;
  const GlyphIndex             end =
    std::min<GlyphIndex>(glyphRun.glyphIndex + glyphRun.numberOfGlyphs, visual.mGlyphs.Count());
  for(GlyphIndex glyphIndex = glyphRun.glyphIndex; glyphIndex < end; ++glyphIndex)
  {
    const GlyphInfo& glyph = visual.mGlyphs[glyphIndex];
    if(glyph.fontId == 0u || IsSyntheticReplacementGlyph(glyph) ||
       !finalElision.IsOriginalGlyphVisible(glyphIndex))
    {
      continue;
    }

    if(glyph.fontId != lastFontId)
    {
      fontClient.GetFontMetrics(glyph.fontId, fontMetrics);
      lastFontId = glyph.fontId;
    }
    surrounding.ascender       = surrounding.hasVisibleText
                                   ? std::max(surrounding.ascender, fontMetrics.ascender)
                                   : fontMetrics.ascender;
    surrounding.descender      = surrounding.hasVisibleText
                                   ? std::min(surrounding.descender, fontMetrics.descender)
                                   : fontMetrics.descender;
    surrounding.hasVisibleText = true;
  }
}

Vector<SurroundingMetrics> ResolveSurroundingMetrics(const VisualModel&           visual,
                                                     const FinalElisionResult&    finalElision,
                                                     TextAbstraction::FontClient& fontClient,
                                                     FontId                       defaultFontId)
{
  Vector<SurroundingMetrics> metrics;
  metrics.Resize(visual.mLines.Count());

  TextAbstraction::FontMetrics defaultFontMetrics;
  const bool                   hasDefaultFont = defaultFontId != 0u;
  if(hasDefaultFont)
  {
    fontClient.GetFontMetrics(defaultFontId, defaultFontMetrics);
  }

  for(LineIndex lineIndex = 0u; lineIndex < visual.mLines.Count(); ++lineIndex)
  {
    const LineRun& line = visual.mLines[lineIndex];
    IncludeGlyphMetrics(visual, finalElision, fontClient, line.glyphRun, metrics[lineIndex]);
    if(line.isSplitToTwoHalves)
    {
      IncludeGlyphMetrics(visual, finalElision, fontClient, line.glyphRunSecondHalf, metrics[lineIndex]);
    }
    if(!metrics[lineIndex].hasVisibleText && hasDefaultFont)
    {
      metrics[lineIndex].ascender       = defaultFontMetrics.ascender;
      metrics[lineIndex].descender      = defaultFontMetrics.descender;
      metrics[lineIndex].hasVisibleText = true;
    }
  }
  return metrics;
}

float GetReplacementTop(const ReplacementMetrics& replacement, const SurroundingMetrics& surrounding)
{
  switch(replacement.verticalAlignment)
  {
    case ReplacementVerticalAlignment::TEXT_BOTTOM:
      return -surrounding.descender + replacement.verticalOffset - replacement.height;
    case ReplacementVerticalAlignment::TEXT_CENTER:
      return -0.5f * (surrounding.ascender + surrounding.descender) + replacement.verticalOffset -
             0.5f * replacement.height;
    case ReplacementVerticalAlignment::TEXT_BASELINE:
    default:
      return replacement.verticalOffset - replacement.height;
  }
}

} // unnamed namespace

void ExtractReplacementPlacements(const Model&                  model,
                                  const ReplacementProjection&  projection,
                                  const FinalElisionResult&     finalElision,
                                  TextAbstraction::FontClient&  fontClient,
                                  FontId                        defaultFontId,
                                  Vector<ReplacementPlacement>& placements)
{
  placements.Clear();
  if(!finalElision.resolved)
  {
    // A placement from another layout pass must never be combined with this
    // model. The caller will consequently materialize no stale ImageVisual.
    return;
  }
  const VisualModel&               visual = *model.mVisualModel;
  const Vector<SurroundingMetrics> lineMetrics =
    ResolveSurroundingMetrics(visual, finalElision, fontClient, defaultFontId);
  for(const ProjectedReplacementRun& replacement : projection.GetReplacementRuns())
  {
    ReplacementPlacement placement;
    placement.logicalCharacterRange = replacement.logicalCharacterRange;
    placement.sourceRunIndex        = replacement.sourceRunIndex;
    placement.occurrenceIdentity    = replacement.occurrenceIdentity;
    placement.size                  = Vector2(replacement.metrics.width, replacement.metrics.height);
    if(replacement.projectedCharacterIndex >= visual.mCharactersToGlyph.Count())
    {
      placement.elided = finalElision.textElided;
      placements.PushBack(placement);
      continue;
    }
    placement.syntheticGlyphIndex = visual.mCharactersToGlyph[replacement.projectedCharacterIndex];

    FinalGlyphGeometry geometry;
    placement.visible = GetFinalSourceGlyphGeometry(model,
                                                    finalElision,
                                                    placement.syntheticGlyphIndex,
                                                    geometry);
    if(placement.visible)
    {
      const LineRun& line                   = visual.mLines[geometry.lineIndex];
      placement.lineIndex                   = geometry.lineIndex;
      placement.lineDirection               = line.direction;
      placement.position.x                  = geometry.contentLocalPenPosition.x;
      const SurroundingMetrics& surrounding = lineMetrics[geometry.lineIndex];
      placement.position.y                  = geometry.baseline + GetReplacementTop(replacement.metrics, surrounding);
    }
    placement.elided = finalElision.textElided && !placement.visible;
    placements.PushBack(placement);
  }
}

} // namespace Dali::Ui::Text
