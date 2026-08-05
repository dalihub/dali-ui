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
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/ellipsis/end-ellipsis-metrics.h>
#include <dali-ui-foundation/internal/text/ellipsis/end-ellipsis-planner.h>
#include <dali-ui-foundation/internal/text/final-glyph-geometry.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/replacement/editable-inline-replacement-data.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-manager.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-test-suite-utils.h>
#include "replacement-layout-test-adapter.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Text::ReplacementRunSnapshot Candidate(Text::CharacterIndex start, Text::Length length, float width = 20.0f,
                                       float height = 18.0f, uint32_t id = 0u)
{
  Text::ReplacementRunSnapshot candidate;
  candidate.logicalCharacterRange = Text::CharacterRun{start, length};
  candidate.metrics.width         = width;
  candidate.metrics.height        = height;
  candidate.occurrenceIdentity    = id;
  return candidate;
}

Vector<Text::Character> Utf32(const std::string& utf8)
{
  const auto*             bytes = reinterpret_cast<const uint8_t*>(utf8.data());
  Vector<Text::Character> characters;
  characters.Resize(Text::GetNumberOfUtf8Characters(bytes, utf8.size()));
  const uint32_t converted = Text::Utf8ToUtf32(bytes, utf8.size(), characters.Begin());
  characters.Resize(converted);
  return characters;
}

Text::ReplacementLayoutTestServices MakeLayoutServices()
{
  return {TextAbstraction::Segmentation::Get(),
          TextAbstraction::BidirectionalSupport::Get(),
          TextAbstraction::Shaping::Get(),
          TextAbstraction::FontClient::Get(),
          Text::MultilanguageSupport::Get()};
}

uint32_t CountSyntheticGlyphs(const Text::ReplacementRenderState& state)
{
  if(!state.processingModel)
  {
    return 0u;
  }

  uint32_t count = 0u;
  for(const TextAbstraction::GlyphInfo& glyph : state.processingModel->mVisualModel->mGlyphs)
  {
    count += Text::IsSyntheticReplacementGlyph(glyph) ? 1u : 0u;
  }
  return count;
}

std::vector<Text::GlyphIndex> FindActuallyRemovedHostLineReplacements(
  const Text::Model&               model,
  const Text::FinalElisionResult& finalElision,
  const Text::LineRun&             line)
{
  std::vector<Text::GlyphIndex>   removed;
  const Text::GlyphIndex          lineEnd =
    std::min<Text::GlyphIndex>(line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs,
                               model.mVisualModel->mGlyphs.Count());
  for(Text::GlyphIndex glyphIndex = line.glyphRun.glyphIndex; glyphIndex < lineEnd; ++glyphIndex)
  {
    if(Text::IsSyntheticReplacementGlyph(model.mVisualModel->mGlyphs[glyphIndex]) &&
       !finalElision.IsOriginalGlyphVisible(glyphIndex))
    {
      removed.push_back(glyphIndex);
    }
  }
  return removed;
}

Vector<Vector2> BuildAlignedGlyphPositions(const Text::Model& model)
{
  const Text::VisualModel& visual    = *model.mVisualModel;
  Vector<Vector2>          positions = visual.mGlyphPositions;
  float                    lineTop   = 0.0f;
  for(const Text::LineRun& line : visual.mLines)
  {
    const float baseline =
      lineTop + line.ascender + Text::GetPreOffsetVerticalLineAlignment(line, model.GetVerticalLineAlignment());
    const auto alignRun = [&](const Text::GlyphRun& run)
    {
      const Text::GlyphIndex end =
        std::min<Text::GlyphIndex>(run.glyphIndex + run.numberOfGlyphs, positions.Count());
      for(Text::GlyphIndex index = run.glyphIndex; index < end; ++index)
      {
        positions[index].x += line.alignmentOffset;
        positions[index].y += baseline;
      }
    };
    alignRun(line.glyphRun);
    if(line.isSplitToTwoHalves)
    {
      alignRun(line.glyphRunSecondHalf);
    }
    lineTop += Text::GetLineHeight(line, false);
  }
  return positions;
}

Text::EndEllipsisPlan ResolveFinalHostLinePlan(const Text::Model&          model,
                                               const Text::LineRun&        line,
                                               TextAbstraction::FontClient fontClient)
{
  const Text::VisualModel&   visual           = *model.mVisualModel;
  Vector<Vector2>            alignedPositions = BuildAlignedGlyphPositions(model);
  Text::EndEllipsisInputView input;
  input.glyphs                  = visual.mGlyphs.Begin();
  input.glyphPositions          = alignedPositions.Begin();
  input.text                    = model.mLogicalModel->mText.Begin();
  input.glyphToCharacterMap     = visual.mGlyphsToCharacters.Begin();
  input.characterSpacingRuns    = &visual.GetCharacterSpacingGlyphRuns();
  input.numberOfGlyphs          = visual.mGlyphs.Count();
  input.glyphPositionStartIndex = 0u;
  input.numberOfGlyphPositions  = alignedPositions.Count();
  input.numberOfCharacters      = model.mLogicalModel->mText.Count();
  input.startIndex              = line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs - 1u;
  input.lineWidth               = line.width;
  input.positionOffset          = 0.0f;
  input.modelCharacterSpacing   = visual.GetCharacterSpacing();
  input.metricsContext          = &fontClient;
  input.resolveMetrics          = Text::ResolveFontClientEndEllipsisMetrics;
  return Text::ResolveEndEllipsisPlan(input);
}

struct FixedEllipsisMetrics
{
  Text::GlyphInfo glyph;
  uint32_t        resolveCount{0u};
};

bool ResolveFixedEllipsisMetrics(void*            context,
                                 Text::FontId     fontId,
                                 bool             allowDefaultFont,
                                 Text::GlyphInfo& metrics)
{
  auto& fixed = *static_cast<FixedEllipsisMetrics*>(context);
  ++fixed.resolveCount;
  if(fontId == 0u && !allowDefaultFont)
  {
    return false;
  }
  metrics = fixed.glyph;
  return true;
}

uint32_t CountVisibleOriginalGlyphs(const Text::FinalElisionResult& result)
{
  uint32_t count = 0u;
  for(Text::GlyphIndex finalIndex : result.sourceToFinalGlyphIndices)
  {
    count += finalIndex != Text::FinalElisionResult::INVALID_GLYPH_INDEX ? 1u : 0u;
  }
  return count;
}

Text::GlyphIndex FindSourceGlyphIndex(const Text::FinalElisionResult& result, Text::GlyphIndex finalGlyphIndex)
{
  for(Text::GlyphIndex sourceGlyphIndex = 0u;
      sourceGlyphIndex < result.sourceToFinalGlyphIndices.Count();
      ++sourceGlyphIndex)
  {
    if(result.sourceToFinalGlyphIndices[sourceGlyphIndex] == finalGlyphIndex)
    {
      return sourceGlyphIndex;
    }
  }
  return Text::FinalElisionResult::INVALID_GLYPH_INDEX;
}

uint32_t CountGeneratedFinalGlyphs(const Text::FinalElisionResult& result)
{
  return result.glyphs.Count() - CountVisibleOriginalGlyphs(result);
}

bool IsGeneratedEllipsisDrawable(const Text::ReplacementRenderState& state)
{
  const Text::FinalElisionResult& result = state.finalElision;
  if(!result.applied || result.ellipsisFinalGlyphIndex >= result.glyphs.Count())
  {
    return false;
  }

  const Text::GlyphInfo& glyph    = result.glyphs[result.ellipsisFinalGlyphIndex];
  const Vector2&         position = result.viewGlyphPositions[result.ellipsisFinalGlyphIndex];
  const Size&            control  = state.processingModel->mVisualModel->mControlSize;
  return position.x + glyph.width > 0.0f && position.x < control.width &&
         position.y + glyph.height > 0.0f && position.y < control.height;
}

Text::LineIndex FindEllipsisLine(const Text::VisualModel& visual)
{
  for(Text::LineIndex lineIndex = 0u; lineIndex < visual.mLines.Count(); ++lineIndex)
  {
    if(visual.mLines[lineIndex].ellipsis)
    {
      return lineIndex;
    }
  }
  return Text::FinalElisionResult::INVALID_LINE_INDEX;
}

void CheckFinalElisionContract(const Text::ReplacementRenderState& state)
{
  DALI_TEST_CHECK(state.processingModel);
  const Text::FinalElisionResult& finalElision = state.finalElision;
  DALI_TEST_CHECK(finalElision.resolved);
  uint32_t ellipsisLineCount = 0u;
  for(const Text::LineRun& line : state.processingModel->mVisualModel->mLines)
  {
    ellipsisLineCount += line.ellipsis ? 1u : 0u;
  }
  DALI_TEST_CHECK(ellipsisLineCount <= 1u);
  DALI_TEST_EQUALS(finalElision.textElided, ellipsisLineCount == 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(finalElision.glyphs.Count(), finalElision.viewGlyphPositions.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(finalElision.glyphs.Count(), finalElision.lineLocalGlyphPositions.Count(), TEST_LOCATION);
  const uint32_t generatedEllipsisCount = CountGeneratedFinalGlyphs(finalElision);
  DALI_TEST_CHECK(generatedEllipsisCount <= 1u);
  DALI_TEST_EQUALS(finalElision.ellipsisUnitCount, generatedEllipsisCount, TEST_LOCATION);
  DALI_TEST_EQUALS(finalElision.applied, generatedEllipsisCount == 1u, TEST_LOCATION);
  for(Text::GlyphIndex sourceIndex = 0u; sourceIndex < finalElision.sourceToFinalGlyphIndices.Count();
      ++sourceIndex)
  {
    const Text::GlyphIndex finalIndex = finalElision.sourceToFinalGlyphIndices[sourceIndex];
    if(finalIndex != Text::FinalElisionResult::INVALID_GLYPH_INDEX)
    {
      DALI_TEST_CHECK(finalIndex < finalElision.glyphs.Count());
      DALI_TEST_CHECK(finalElision.IsOriginalGlyphVisible(sourceIndex));
    }
  }
  if(finalElision.textElided)
  {
    DALI_TEST_EQUALS(generatedEllipsisCount, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(finalElision.applied);
    DALI_TEST_EQUALS(finalElision.ellipsisLineIndex,
                     FindEllipsisLine(*state.processingModel->mVisualModel),
                     TEST_LOCATION);
    DALI_TEST_CHECK(finalElision.ellipsisFinalGlyphIndex < finalElision.glyphs.Count());
    DALI_TEST_CHECK(IsGeneratedEllipsisDrawable(state));
  }

  for(const Text::ReplacementPlacement& placement : state.placements)
  {
    DALI_TEST_EQUALS(placement.visible,
                     finalElision.IsOriginalGlyphVisible(placement.syntheticGlyphIndex),
                     TEST_LOCATION);
    DALI_TEST_EQUALS(placement.elided, finalElision.textElided && !placement.visible, TEST_LOCATION);
    Text::FinalGlyphGeometry geometry;
    DALI_TEST_EQUALS(Text::GetFinalSourceGlyphGeometry(*state.processingModel,
                                                       finalElision,
                                                       placement.syntheticGlyphIndex,
                                                       geometry),
                     placement.visible,
                     TEST_LOCATION);
    if(placement.visible)
    {
      DALI_TEST_EQUALS(geometry.lineIndex, placement.lineIndex, TEST_LOCATION);
      DALI_TEST_EQUALS(geometry.contentLocalPenPosition.x,
                       placement.position.x,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      const Text::LineRun& line = state.processingModel->mVisualModel->mLines[geometry.lineIndex];
      DALI_TEST_CHECK(placement.position.y <=
                      geometry.baseline - line.descender + Math::MACHINE_EPSILON_1000);
      Text::GlyphIndex finalGlyphIndex = placement.syntheticGlyphIndex;
      if(finalElision.textElided)
      {
        DALI_TEST_CHECK(finalElision.FindFinalGlyphIndex(placement.syntheticGlyphIndex, finalGlyphIndex));
      }
      const Text::GlyphInfo& finalGlyph = finalElision.textElided
                                            ? finalElision.glyphs[finalGlyphIndex]
                                            : state.processingModel->mVisualModel->mGlyphs[placement.syntheticGlyphIndex];
      DALI_TEST_CHECK(Text::IsSyntheticReplacementGlyph(finalGlyph));
      DALI_TEST_EQUALS(finalGlyph.advance,
                       placement.size.x,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      if(finalElision.textElided)
      {
        DALI_TEST_EQUALS(finalElision.viewGlyphPositions[finalGlyphIndex].x,
                         geometry.contentLocalPenPosition.x,
                         Math::MACHINE_EPSILON_1000,
                         TEST_LOCATION);
      }
    }
  }
}

bool IsExactLogicalBoundary(const Text::ReplacementProjection& projection, Text::CharacterIndex boundary)
{
  return projection.NormalizeLogicalBoundary(boundary, Text::ReplacementProjection::BoundaryAffinity::LEADING) ==
           boundary &&
         projection.NormalizeLogicalBoundary(boundary, Text::ReplacementProjection::BoundaryAffinity::TRAILING) ==
           boundary;
}

void CheckLaidOutBoundariesAreAtomic(const Text::ReplacementProjection&  projection,
                                     const Text::ReplacementRenderState& result)
{
  const Vector<Text::LineRun>& lines = result.processingModel->mVisualModel->mLines;
  for(const Text::LineRun& line : lines)
  {
    const Text::CharacterIndex firstStart =
      projection.ProjectedBoundaryToLogical(line.characterRun.characterIndex);
    const Text::CharacterIndex firstEnd = projection.ProjectedBoundaryToLogical(
      line.characterRun.characterIndex + line.characterRun.numberOfCharacters);
    DALI_TEST_CHECK(IsExactLogicalBoundary(projection, firstStart));
    DALI_TEST_CHECK(IsExactLogicalBoundary(projection, firstEnd));

    if(line.characterRunForSecondHalfLine.numberOfCharacters > 0u)
    {
      const Text::CharacterIndex secondStart =
        projection.ProjectedBoundaryToLogical(line.characterRunForSecondHalfLine.characterIndex);
      const Text::CharacterIndex secondEnd = projection.ProjectedBoundaryToLogical(
        line.characterRunForSecondHalfLine.characterIndex +
        line.characterRunForSecondHalfLine.numberOfCharacters);
      DALI_TEST_CHECK(IsExactLogicalBoundary(projection, secondStart));
      DALI_TEST_CHECK(IsExactLogicalBoundary(projection, secondEnd));
    }
  }
}

} // unnamed namespace

void utc_dali_replacement_projection_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_replacement_projection_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliReplacementProjectionPathsAndMappingP(void)
{
  Vector<Text::Character> text = Utf32("AiconB");

  Vector<Text::ReplacementRunSnapshot> noCandidates;
  Text::ReplacementProjection          none = Text::ReplacementProjection::Build(text, noCandidates);
  DALI_TEST_EQUALS(static_cast<uint32_t>(none.GetMode()),
                   static_cast<uint32_t>(Text::ReplacementProjection::Mode::NONE), TEST_LOCATION);
  DALI_TEST_CHECK(!none.HasReplacements());
  DALI_TEST_CHECK(none.GetProcessingText().Begin() == text.Begin());

  Vector<Text::Character>              identityText = Utf32("A\uFFFCB");
  Vector<Text::ReplacementRunSnapshot> identityCandidates;
  identityCandidates.PushBack(Candidate(1u, 1u, 16.0f, 14.0f, 10u));
  Text::ReplacementProjection identity =
    Text::ReplacementProjection::Build(identityText, identityCandidates);
  DALI_TEST_EQUALS(static_cast<uint32_t>(identity.GetMode()),
                   static_cast<uint32_t>(Text::ReplacementProjection::Mode::IDENTITY), TEST_LOCATION);
  DALI_TEST_CHECK(identity.UsesOriginalTextBuffer());
  DALI_TEST_CHECK(identity.GetProcessingText().Begin() == identityText.Begin());
  DALI_TEST_EQUALS(identity.GetLogicalCharacterCount(), identity.GetProcessingCharacterCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(identity.GetReplacementRuns()[0u].projectedCharacterIndex, 1u, TEST_LOCATION);

  Text::ReplacementProjection scaledIdentity =
    Text::ReplacementProjection::Build(identityText, identityCandidates, 1.5f);
  DALI_TEST_EQUALS(scaledIdentity.GetReplacementRuns()[0u].metrics.width, 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(scaledIdentity.GetReplacementRuns()[0u].metrics.height, 21.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(identityCandidates[0u].metrics.width, 16.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(identityCandidates[0u].metrics.height, 14.0f, TEST_LOCATION);

  // Multiple and adjacent canonical objects retain the same source buffer and
  // identity indices. Their source descriptor, size, alignment and offset do
  // not require a processing-text or character-map allocation.
  Vector<Text::Character>              multipleIdentityText = Utf32("A\uFFFC\uFFFCx\uFFFCD");
  Vector<Text::ReplacementRunSnapshot> multipleIdentityCandidates;
  multipleIdentityCandidates.PushBack(Candidate(1u, 1u, 8.0f, 8.0f, 12u));
  multipleIdentityCandidates.PushBack(Candidate(2u, 1u, 24.0f, 24.0f, 13u));
  multipleIdentityCandidates.PushBack(Candidate(4u, 1u, 80.0f, 48.0f, 14u));
  multipleIdentityCandidates[0u].image.source              = "same.png";
  multipleIdentityCandidates[1u].image.source              = "same.png";
  multipleIdentityCandidates[2u].image.source              = "different.png";
  multipleIdentityCandidates[1u].metrics.verticalAlignment = Text::ReplacementVerticalAlignment::TEXT_CENTER;
  multipleIdentityCandidates[2u].metrics.verticalAlignment = Text::ReplacementVerticalAlignment::TEXT_BASELINE;
  multipleIdentityCandidates[2u].metrics.verticalOffset    = -4.0f;
  Text::ReplacementProjection multipleIdentity =
    Text::ReplacementProjection::Build(multipleIdentityText, multipleIdentityCandidates);
  DALI_TEST_EQUALS(static_cast<uint32_t>(multipleIdentity.GetMode()),
                   static_cast<uint32_t>(Text::ReplacementProjection::Mode::IDENTITY), TEST_LOCATION);
  DALI_TEST_CHECK(multipleIdentity.UsesOriginalTextBuffer());
  DALI_TEST_CHECK(multipleIdentity.GetProcessingText().Begin() == multipleIdentityText.Begin());
  DALI_TEST_EQUALS(multipleIdentity.GetProcessingCharacterCount(), multipleIdentityText.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(multipleIdentity.GetReplacementRuns().Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleIdentity.GetReplacementRuns()[0u].projectedCharacterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleIdentity.GetReplacementRuns()[1u].projectedCharacterIndex, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleIdentity.GetReplacementRuns()[2u].projectedCharacterIndex, 4u, TEST_LOCATION);

  // A programmatic one-character replacement still has identity indices, but its underlying bidi/script class must
  // be neutralized for processing. Only canonical U+FFFC can reuse the exact source buffer.
  Vector<Text::Character>              identityOrdinaryText = Utf32("AxB");
  Vector<Text::ReplacementRunSnapshot> identityOrdinaryCandidates;
  identityOrdinaryCandidates.PushBack(Candidate(1u, 1u, 16.0f, 14.0f, 11u));
  Text::ReplacementProjection identityOrdinary = Text::ReplacementProjection::Build(
    identityOrdinaryText, identityOrdinaryCandidates);
  DALI_TEST_EQUALS(static_cast<uint32_t>(identityOrdinary.GetMode()),
                   static_cast<uint32_t>(Text::ReplacementProjection::Mode::IDENTITY), TEST_LOCATION);
  DALI_TEST_CHECK(!identityOrdinary.UsesOriginalTextBuffer());
  DALI_TEST_EQUALS(identityOrdinary.GetLogicalCharacterCount(), identityOrdinary.GetProcessingCharacterCount(),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(identityOrdinary.GetProcessingText()[1u],
                   Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER, TEST_LOCATION);
  DALI_TEST_EQUALS(identityOrdinary.GetLogicalText()[1u], static_cast<Text::Character>('x'), TEST_LOCATION);

  Vector<Text::ReplacementRunSnapshot> multiCandidates;
  multiCandidates.PushBack(Candidate(1u, 4u, 24.0f, 18.0f, 20u));
  Text::ReplacementProjection compact =
    Text::ReplacementProjection::Build(text, multiCandidates);
  DALI_TEST_EQUALS(static_cast<uint32_t>(compact.GetMode()),
                   static_cast<uint32_t>(Text::ReplacementProjection::Mode::COMPACT), TEST_LOCATION);
  DALI_TEST_CHECK(!compact.UsesOriginalTextBuffer());
  DALI_TEST_EQUALS(compact.GetLogicalCharacterCount(), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(compact.GetProcessingCharacterCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(compact.GetProcessingText()[0u], static_cast<Text::Character>('A'), TEST_LOCATION);
  DALI_TEST_EQUALS(compact.GetProcessingText()[1u], Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(compact.GetProcessingText()[2u], static_cast<Text::Character>('B'), TEST_LOCATION);
  for(Text::CharacterIndex logicalIndex = 1u; logicalIndex < 5u; ++logicalIndex)
  {
    DALI_TEST_EQUALS(compact.LogicalCharacterToProjected(logicalIndex), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(compact.FindByLogicalCharacter(logicalIndex) != nullptr);
  }
  DALI_TEST_EQUALS(compact.ProjectedCharacterToLogical(1u), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(compact.ProjectedBoundaryToLogical(2u), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(compact.LogicalBoundaryToProjected(
                     1u, Text::ReplacementProjection::BoundaryAffinity::LEADING),
                   1u, TEST_LOCATION);
  DALI_TEST_EQUALS(compact.LogicalBoundaryToProjected(
                     5u, Text::ReplacementProjection::BoundaryAffinity::TRAILING),
                   2u, TEST_LOCATION);

  // Building the compact view must not modify the source buffer.
  const Vector<Text::Character> expectedText = Utf32("AiconB");
  DALI_TEST_EQUALS(text.Count(), expectedText.Count(), TEST_LOCATION);
  for(uint32_t index = 0u; index < text.Count(); ++index)
  {
    DALI_TEST_EQUALS(text[index], expectedText[index], TEST_LOCATION);
  }

  // Adjacent replacements remain distinct processing units.
  Vector<Text::Character>              adjacentText = Utf32("AabcdE");
  Vector<Text::ReplacementRunSnapshot> adjacentCandidates;
  adjacentCandidates.PushBack(Candidate(1u, 2u, 12.0f, 12.0f, 31u));
  adjacentCandidates.PushBack(Candidate(3u, 2u, 13.0f, 12.0f, 32u));
  Text::ReplacementProjection adjacent =
    Text::ReplacementProjection::Build(adjacentText, adjacentCandidates);
  DALI_TEST_EQUALS(adjacent.GetReplacementRuns().Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(adjacent.GetReplacementRuns()[0u].projectedCharacterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(adjacent.GetReplacementRuns()[1u].projectedCharacterIndex, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(adjacent.GetProcessingCharacterCount(), 4u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliReplacementProjectionInvalidRangesFallbackP(void)
{
  Vector<Text::Character> text = Utf32(
    "a\xCC\x81"
    "bc\ndefgh"); // a + COMBINING ACUTE + bc + LF + defgh

  Vector<Text::ReplacementRunSnapshot> candidates;
  candidates.PushBack(Candidate(1u, 1u, 10.0f, 10.0f, 1u));  // combining mark only: accepted
  candidates.PushBack(Candidate(2u, 0u, 10.0f, 10.0f, 2u));  // empty
  candidates.PushBack(Candidate(99u, 1u, 10.0f, 10.0f, 3u)); // out of range
  candidates.PushBack(Candidate(3u, 2u, 10.0f, 10.0f, 4u));  // includes LF
  candidates.PushBack(Candidate(5u, 2u, 10.0f, 10.0f, 5u));  // accepted
  candidates.PushBack(Candidate(5u, 2u, 10.0f, 10.0f, 6u));  // duplicate
  candidates.PushBack(Candidate(6u, 1u, 10.0f, 10.0f, 7u));  // nested
  candidates.PushBack(Candidate(8u, 1u, -1.0f, 10.0f, 8u));  // invalid metric
  Text::ReplacementRunSnapshot invalidPayloadCandidate = Candidate(9u, 1u, 10.0f, 10.0f, 9u);
  invalidPayloadCandidate.type                         = Text::ReplacementType::IMAGE;
  candidates.PushBack(invalidPayloadCandidate);

  Text::ReplacementProjection projection =
    Text::ReplacementProjection::Build(text, candidates);
  DALI_TEST_EQUALS(projection.GetReplacementRuns().Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(projection.GetReplacementRuns()[0u].logicalCharacterRange.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(projection.GetReplacementRuns()[1u].logicalCharacterRange.characterIndex, 5u, TEST_LOCATION);
  Vector<Text::Character> paragraphText = Utf32(
    "ab\xE2\x80\xA9"
    "cd");
  Vector<Text::ReplacementRunSnapshot> paragraphCandidates;
  paragraphCandidates.PushBack(Candidate(1u, 3u, 10.0f, 10.0f, 10u));
  Text::ReplacementProjection paragraphProjection =
    Text::ReplacementProjection::Build(paragraphText, paragraphCandidates);
  DALI_TEST_EQUALS(paragraphProjection.GetReplacementRuns().Count(), 0u, TEST_LOCATION);
  // Winner selection follows authored attachment order, not the lowest logical start. Runs are sorted only after
  // validation so later layout/mapping can still use binary lookup and linear sweeps.
  Vector<Text::Character>              winnerText = Utf32("abcdef");
  Vector<Text::ReplacementRunSnapshot> winnerCandidates;
  winnerCandidates.PushBack(Candidate(2u, 3u, 10.0f, 10.0f, 90u));
  winnerCandidates.PushBack(Candidate(1u, 2u, 10.0f, 10.0f, 91u));
  Text::ReplacementProjection insertionWinner =
    Text::ReplacementProjection::Build(winnerText, winnerCandidates);
  DALI_TEST_EQUALS(insertionWinner.GetReplacementRuns().Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(insertionWinner.GetReplacementRuns()[0u].logicalCharacterRange.characterIndex, 2u, TEST_LOCATION);

  Vector<Text::ReplacementRunSnapshot> unsortedCandidates;
  unsortedCandidates.PushBack(Candidate(4u, 1u, 10.0f, 10.0f, 92u));
  unsortedCandidates.PushBack(Candidate(1u, 1u, 10.0f, 10.0f, 93u));
  Text::ReplacementProjection unsorted =
    Text::ReplacementProjection::Build(winnerText, unsortedCandidates);
  DALI_TEST_EQUALS(unsorted.GetReplacementRuns().Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(unsorted.GetReplacementRuns()[0u].logicalCharacterRange.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(unsorted.GetReplacementRuns()[1u].logicalCharacterRange.characterIndex, 4u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliReplacementProjectionExactUtf32InteriorRangesP(void)
{
  UiTestApplication                   application;
  Text::ReplacementLayoutTestServices services = MakeLayoutServices();

  struct ExactRangeCase
  {
    const char*          name;
    const char*          utf8;
    Text::CharacterIndex start;
    Text::Length         length;
  };

  const ExactRangeCase cases[] = {
    {"combining-base",
     "Xa\xCC\x81"
     "Y",
     1u, 1u},
    {"combining-mark",
     "Xa\xCC\x81"
     "Y",
     2u, 1u},
    {"combining-whole",
     "Xa\xCC\x81"
     "Y",
     1u, 2u},
    {"variation-base",
     "X\xE2\x9D\xA4\xEF\xB8\x8F"
     "Y",
     1u, 1u},
    {"variation-selector",
     "X\xE2\x9D\xA4\xEF\xB8\x8F"
     "Y",
     2u, 1u},
    {"variation-whole",
     "X\xE2\x9D\xA4\xEF\xB8\x8F"
     "Y",
     1u, 2u},
    {"modifier-base",
     "X\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"
     "Y",
     1u, 1u},
    {"modifier-only",
     "X\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"
     "Y",
     2u, 1u},
    {"modifier-whole",
     "X\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"
     "Y",
     1u, 2u},
    {"zwj-first",
     "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
     "Y",
     1u, 1u},
    {"zwj-only",
     "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
     "Y",
     2u, 1u},
    {"zwj-last",
     "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
     "Y",
     3u, 1u},
    {"zwj-partial",
     "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
     "Y",
     1u, 2u},
    {"zwj-whole",
     "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
     "Y",
     1u, 3u},
    {"regional-first",
     "X\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"
     "Y",
     1u, 1u},
    {"regional-second",
     "X\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"
     "Y",
     2u, 1u},
    {"regional-whole",
     "X\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"
     "Y",
     1u, 2u},
    {"latin-fi-first", "XfiY", 1u, 1u},
    {"latin-fi-second", "XfiY", 2u, 1u},
    {"latin-ffi-middle", "XffiY", 2u, 1u},
    {"arabic-lam",
     "X\xD9\x84\xD8\xA7"
     "Y",
     1u, 1u},
    {"arabic-alef",
     "X\xD9\x84\xD8\xA7"
     "Y",
     2u, 1u},
  };

  for(uint32_t caseIndex = 0u; caseIndex < sizeof(cases) / sizeof(cases[0u]); ++caseIndex)
  {
    const ExactRangeCase&                testCase     = cases[caseIndex];
    Vector<Text::Character>              text         = Utf32(testCase.utf8);
    const Vector<Text::Character>        originalText = text;
    Vector<Text::ReplacementRunSnapshot> candidates;
    candidates.PushBack(Candidate(testCase.start, testCase.length, 24.0f, 18.0f, caseIndex + 1u));

    Text::ReplacementProjection projection = Text::ReplacementProjection::Build(text, candidates);
    DALI_TEST_EQUALS(projection.GetReplacementRuns().Count(), 1u, TEST_LOCATION);
    const Text::ProjectedReplacementRun& replacement = projection.GetReplacementRuns()[0u];
    DALI_TEST_EQUALS(replacement.logicalCharacterRange.characterIndex, testCase.start, TEST_LOCATION);
    DALI_TEST_EQUALS(replacement.logicalCharacterRange.numberOfCharacters, testCase.length, TEST_LOCATION);
    DALI_TEST_EQUALS(projection.GetProcessingCharacterCount(),
                     text.Count() - testCase.length + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(projection.GetProcessingText()[replacement.projectedCharacterIndex],
                     Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER, TEST_LOCATION);
    for(Text::CharacterIndex logicalIndex = testCase.start;
        logicalIndex < testCase.start + testCase.length;
        ++logicalIndex)
    {
      DALI_TEST_EQUALS(projection.LogicalCharacterToProjected(logicalIndex),
                       replacement.projectedCharacterIndex, TEST_LOCATION);
    }

    DALI_TEST_EQUALS(text.Count(), originalText.Count(), TEST_LOCATION);
    for(uint32_t index = 0u; index < text.Count(); ++index)
    {
      DALI_TEST_EQUALS(text[index], originalText[index], TEST_LOCATION);
    }

    Text::ReplacementLayoutTestOptions options;
    options.contentSize = Size(240.0f, 80.0f);
    Text::ReplacementRenderState result;
    DALI_TEST_CHECK(Text::LayoutReplacementForTest(projection, services, options, result));
    DALI_TEST_EQUALS(CountSyntheticGlyphs(result), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(result.placements.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(result.placements[0u].logicalCharacterRange.characterIndex, testCase.start, TEST_LOCATION);
    DALI_TEST_EQUALS(result.placements[0u].logicalCharacterRange.numberOfCharacters,
                     testCase.length, TEST_LOCATION);
    result.Clear(services.bidirectionalSupport);
  }

  END_TEST;
}

int UtcDaliReplacementProjectionStyleCursorHitDeleteP(void)
{
  Vector<Text::Character>              text = Utf32("AiconBC");
  Vector<Text::ReplacementRunSnapshot> candidates;
  candidates.PushBack(Candidate(1u, 4u, 30.0f, 18.0f, 42u));
  Text::ReplacementProjection projection =
    Text::ReplacementProjection::Build(text, candidates);

  Vector<Text::CharacterRun> glyphStyleRuns;
  glyphStyleRuns.PushBack(Text::CharacterRun{0u, static_cast<Text::Length>(text.Count())});
  Vector<Text::ProjectedStyleSegment> projectedStyleSegments;
  DALI_TEST_CHECK(projection.ProjectGlyphStyleRuns(glyphStyleRuns, projectedStyleSegments));
  DALI_TEST_EQUALS(projectedStyleSegments.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(projectedStyleSegments[0u].projectedCharacterRange.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(projectedStyleSegments[0u].projectedCharacterRange.numberOfCharacters, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(projectedStyleSegments[1u].projectedCharacterRange.characterIndex, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(projectedStyleSegments[1u].projectedCharacterRange.numberOfCharacters, 2u, TEST_LOCATION);

  DALI_TEST_EQUALS(projection.NormalizeLogicalBoundary(
                     3u, Text::ReplacementProjection::BoundaryAffinity::LEADING),
                   1u, TEST_LOCATION);
  DALI_TEST_EQUALS(projection.NormalizeLogicalBoundary(
                     3u, Text::ReplacementProjection::BoundaryAffinity::TRAILING),
                   5u, TEST_LOCATION);
  DALI_TEST_EQUALS(projection.LogicalBoundaryToProjected(
                     1u, Text::ReplacementProjection::BoundaryAffinity::LEADING) +
                     1u,
                   projection.LogicalBoundaryToProjected(
                     5u, Text::ReplacementProjection::BoundaryAffinity::TRAILING),
                   TEST_LOCATION);

  DALI_TEST_EQUALS(projection.HitTestLogicalBoundary(1u, 2.0f, 30.0f, false), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(projection.HitTestLogicalBoundary(1u, 28.0f, 30.0f, false), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(projection.HitTestLogicalBoundary(1u, 2.0f, 30.0f, true), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(projection.HitTestLogicalBoundary(1u, 28.0f, 30.0f, true), 1u, TEST_LOCATION);

  const Text::CharacterRun backspace = projection.GetDeletionRange(5u, true);
  const Text::CharacterRun deleteRun = projection.GetDeletionRange(1u, false);
  DALI_TEST_EQUALS(backspace.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(backspace.numberOfCharacters, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleteRun.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleteRun.numberOfCharacters, 4u, TEST_LOCATION);

  const Text::CharacterRun backspaceAtStart = projection.GetDeletionRange(1u, true);
  const Text::CharacterRun deleteAtEnd      = projection.GetDeletionRange(5u, false);
  DALI_TEST_EQUALS(backspaceAtStart.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(backspaceAtStart.numberOfCharacters, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleteAtEnd.characterIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleteAtEnd.numberOfCharacters, 1u, TEST_LOCATION);

  Vector<Text::Character>              adjacentText = Utf32("abcdef");
  Vector<Text::ReplacementRunSnapshot> adjacentCandidates;
  adjacentCandidates.PushBack(Candidate(1u, 2u, 10.0f, 10.0f, 51u));
  adjacentCandidates.PushBack(Candidate(3u, 2u, 10.0f, 10.0f, 52u));
  Text::ReplacementProjection adjacent =
    Text::ReplacementProjection::Build(adjacentText, adjacentCandidates);
  const Text::CharacterRun adjacentBackspace = adjacent.GetDeletionRange(3u, true);
  const Text::CharacterRun adjacentDelete    = adjacent.GetDeletionRange(3u, false);
  DALI_TEST_EQUALS(adjacentBackspace.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(adjacentBackspace.numberOfCharacters, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(adjacentDelete.characterIndex, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(adjacentDelete.numberOfCharacters, 2u, TEST_LOCATION);

  const Text::ProjectedReplacementRun* semanticLookup = projection.FindByLogicalCharacter(3u);
  DALI_TEST_CHECK(semanticLookup != nullptr);
  DALI_TEST_EQUALS(semanticLookup->logicalCharacterRange.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(semanticLookup->logicalCharacterRange.numberOfCharacters, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(semanticLookup->metrics.width, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(semanticLookup->metrics.height, 18.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliReplacementProjectionLtrLineBreakLayoutP(void)
{
  UiTestApplication                   application;
  Text::ReplacementLayoutTestServices services = MakeLayoutServices();

  Vector<Text::Character>              text = Utf32("AAa b cBB");
  Vector<Text::ReplacementRunSnapshot> candidates;
  candidates.PushBack(Candidate(2u, 5u, 34.0f, 22.0f, 100u));
  Text::ReplacementProjection projection =
    Text::ReplacementProjection::Build(text, candidates);

  Text::ReplacementLayoutTestOptions options;
  options.contentSize = Vector2(300.0f, 100.0f);
  Text::ReplacementRenderState result;
  DALI_TEST_CHECK(Text::LayoutReplacementForTest(projection, services, options, result));
  DALI_TEST_EQUALS(CountSyntheticGlyphs(result), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.processingModel->mLogicalModel->mLineBreakInfo.Count(),
                   projection.GetProcessingCharacterCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(result.placements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(result.placements[0u].visible);
  DALI_TEST_CHECK(!result.placements[0u].elided);
  DALI_TEST_EQUALS(result.placements[0u].logicalCharacterRange.numberOfCharacters, 5u, TEST_LOCATION);
  const Text::GlyphInfo& syntheticGlyph =
    result.processingModel->mVisualModel->mGlyphs[result.placements[0u].syntheticGlyphIndex];
  DALI_TEST_EQUALS(syntheticGlyph.fontId, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!syntheticGlyph.isShaped);
  DALI_TEST_EQUALS(syntheticGlyph.advance, 34.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(syntheticGlyph.height, 22.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  CheckLaidOutBoundariesAreAtomic(projection, result);

  // Adjacent replacements exercise wrapping before/after an object and replacement-only lines.
  Vector<Text::Character>              adjacentText = Utf32("abcdef");
  Vector<Text::ReplacementRunSnapshot> adjacentCandidates;
  adjacentCandidates.PushBack(Candidate(0u, 2u, 24.0f, 18.0f, 201u));
  adjacentCandidates.PushBack(Candidate(2u, 2u, 24.0f, 18.0f, 202u));
  adjacentCandidates.PushBack(Candidate(4u, 2u, 24.0f, 18.0f, 203u));
  Text::ReplacementProjection adjacent =
    Text::ReplacementProjection::Build(adjacentText, adjacentCandidates);
  Text::ReplacementLayoutTestOptions wrapOptions;
  wrapOptions.contentSize  = Vector2(30.0f, 200.0f);
  wrapOptions.layoutType   = Text::Layout::Engine::MULTI_LINE_BOX;
  wrapOptions.lineWrapMode = Text::LineWrapMode::CHARACTER;
  Text::ReplacementRenderState wrapped;
  DALI_TEST_CHECK(Text::LayoutReplacementForTest(adjacent, services, wrapOptions, wrapped));
  DALI_TEST_EQUALS(wrapped.placements.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(wrapped.processingModel->mVisualModel->mLines.Count(), 3u, TEST_LOCATION);
  for(const Text::ReplacementPlacement& placement : wrapped.placements)
  {
    DALI_TEST_CHECK(placement.visible);
  }
  CheckLaidOutBoundariesAreAtomic(adjacent, wrapped);

  // Whitespace and ordinary glyphs on both sides force a break before and after the indivisible object. Break
  // opportunities inside the logical "a b c" range have disappeared from the processing line-break table.
  Vector<Text::Character>              surroundedText = Utf32("AA a b c BB");
  Vector<Text::ReplacementRunSnapshot> surroundedCandidates;
  surroundedCandidates.PushBack(Candidate(3u, 5u, 24.0f, 18.0f, 250u));
  Text::ReplacementProjection surrounded =
    Text::ReplacementProjection::Build(surroundedText, surroundedCandidates);
  Text::ReplacementLayoutTestOptions surroundedOptions = wrapOptions;
  surroundedOptions.contentSize                        = Vector2(30.0f, 200.0f);
  Text::ReplacementRenderState surroundedResult;
  DALI_TEST_CHECK(Text::LayoutReplacementForTest(
    surrounded, services, surroundedOptions, surroundedResult));
  DALI_TEST_CHECK(surroundedResult.processingModel->mVisualModel->mLines.Count() >= 3u);
  DALI_TEST_CHECK(surroundedResult.placements[0u].visible);
  uint32_t objectLine = surroundedResult.processingModel->mVisualModel->mLines.Count();
  for(uint32_t lineIndex = 0u;
      lineIndex < surroundedResult.processingModel->mVisualModel->mLines.Count(); ++lineIndex)
  {
    const Text::LineRun& line = surroundedResult.processingModel->mVisualModel->mLines[lineIndex];
    if(surroundedResult.placements[0u].syntheticGlyphIndex >= line.glyphRun.glyphIndex &&
       surroundedResult.placements[0u].syntheticGlyphIndex < line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs)
    {
      objectLine = lineIndex;
      break;
    }
  }
  DALI_TEST_CHECK(objectLine > 0u);
  DALI_TEST_CHECK(objectLine + 1u < surroundedResult.processingModel->mVisualModel->mLines.Count());
  CheckLaidOutBoundariesAreAtomic(surrounded, surroundedResult);

  // An oversized object remains one unit and must not abort the remaining
  // paragraph merely because its own line is narrower than the object.
  Vector<Text::Character>              oversizedText = Utf32("beforeXYafter\nend");
  Vector<Text::ReplacementRunSnapshot> oversizedCandidates;
  oversizedCandidates.PushBack(Candidate(6u, 2u, 100.0f, 18.0f, 301u));
  Text::ReplacementProjection oversized =
    Text::ReplacementProjection::Build(oversizedText, oversizedCandidates);
  Text::ReplacementLayoutTestOptions oversizedOptions = wrapOptions;
  oversizedOptions.contentSize                        = Vector2(40.0f, 80.0f);
  Text::ReplacementRenderState oversizedResult;
  DALI_TEST_CHECK(Text::LayoutReplacementForTest(oversized, services, oversizedOptions, oversizedResult));
  DALI_TEST_EQUALS(CountSyntheticGlyphs(oversizedResult), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(oversized.GetProcessingCharacterCount(), 16u, TEST_LOCATION);
  DALI_TEST_EQUALS(oversizedResult.placements[0u].logicalCharacterRange.numberOfCharacters, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(oversizedResult.placements[0u].visible);
  const Vector<Text::LineRun>& oversizedLines = oversizedResult.processingModel->mVisualModel->mLines;
  DALI_TEST_CHECK(oversizedLines.Count() >= 4u);
  const Text::LineRun& lastOversizedLine = oversizedLines[oversizedLines.Count() - 1u];
  DALI_TEST_CHECK(lastOversizedLine.characterRun.characterIndex +
                    lastOversizedLine.characterRun.numberOfCharacters >=
                  oversized.GetProcessingCharacterCount());

  END_TEST;
}

int UtcDaliReplacementProjectionBackgroundIncludesAtomicBoxP(void)
{
  UiTestApplication application;

  Text::ModelPtr originalModel        = Text::Model::New();
  originalModel->mLogicalModel->mText = Utf32("AiconB");
  Text::ColorRun background;
  background.characterRun = Text::CharacterRun{2u, 1u}; // Partial intersection with [1, 5).
  background.color        = Color::MAGENTA;
  originalModel->mLogicalModel->mBackgroundColorRuns.PushBack(background);

  Text::ReplacementSourceSnapshot source;
  source.runs.PushBack(Candidate(1u, 4u, 24.0f, 18.0f, 311u));
  source.sourceRevision            = 12u;
  source.hasValidReplacementSource = true;

  Text::ReplacementLayoutTestOptions options;
  options.contentSize      = Vector2(100.0f, 40.0f);
  options.sourceRevision   = source.sourceRevision;
  options.layoutGeneration = 7u;

  Text::ReplacementLayoutTestServices services = MakeLayoutServices();
  Text::ReplacementRenderState        result;
  DALI_TEST_CHECK(Text::LayoutReplacementSourceForTest(*originalModel, source, services, options, result));
  DALI_TEST_CHECK(result.processingModel);
  DALI_TEST_EQUALS(result.processingModel->mLogicalModel->mBackgroundColorRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.processingModel->mLogicalModel->mBackgroundColorRuns[0u].characterRun.characterIndex,
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(result.processingModel->mLogicalModel->mBackgroundColorRuns[0u].characterRun.numberOfCharacters,
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(result.processingModel->mLogicalModel->mBackgroundColorRuns[0u].color,
                   Color::MAGENTA,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliReplacementSyntheticGlyphIdentityP(void)
{
  Text::GlyphInfo ordinaryFontZero;
  ordinaryFontZero.fontId = 0u;
  ordinaryFontZero.index  = 7u;
  DALI_TEST_CHECK(!Text::IsSyntheticReplacementGlyph(ordinaryFontZero));

  Text::GlyphInfo maxIndexOrdinaryFont;
  maxIndexOrdinaryFont.fontId = 9u;
  maxIndexOrdinaryFont.index  = Text::SYNTHETIC_REPLACEMENT_GLYPH_ID;
  DALI_TEST_CHECK(!Text::IsSyntheticReplacementGlyph(maxIndexOrdinaryFont));

  Text::GlyphInfo replacement;
  replacement.fontId = 0u;
  replacement.index  = Text::SYNTHETIC_REPLACEMENT_GLYPH_ID;
  DALI_TEST_CHECK(Text::IsSyntheticReplacementGlyph(replacement));

  END_TEST;
}

int UtcDaliReplacementCutoutFallsBackWithoutMutatingLogicalModelP(void)
{
  UiTestApplication application;

  Text::ModelPtr originalModel        = Text::Model::New();
  originalModel->mLogicalModel->mText = Utf32("AiconB");
  originalModel->mVisualModel->SetCutoutEnabled(true);
  originalModel->mVisualModel->SetBackgroundWithCutoutEnabled(true);

  Text::ReplacementSourceSnapshot source;
  source.runs.PushBack(Candidate(1u, 4u, 24.0f, 18.0f, 312u));
  source.sourceRevision            = 13u;
  source.hasValidReplacementSource = true;

  Text::ReplacementLayoutTestOptions options;
  options.contentSize      = Vector2(100.0f, 40.0f);
  options.sourceRevision   = source.sourceRevision;
  options.layoutGeneration = 8u;

  Text::ReplacementLayoutTestServices services = MakeLayoutServices();
  Text::ReplacementRenderState        result;
  DALI_TEST_CHECK(Text::LayoutReplacementSourceForTest(*originalModel, source, services, options, result));
  DALI_TEST_CHECK(result.processingModel);
  DALI_TEST_CHECK(originalModel->IsCutoutEnabled());
  DALI_TEST_CHECK(originalModel->IsBackgroundWithCutoutEnabled());
  DALI_TEST_CHECK(!result.processingModel->IsCutoutEnabled());
  DALI_TEST_CHECK(!result.processingModel->IsBackgroundWithCutoutEnabled());

  END_TEST;
}

int UtcDaliReplacementVerticalAlignmentLineContainmentP(void)
{
  UiTestApplication                   application;
  Text::ReplacementLayoutTestServices services = MakeLayoutServices();

  const Text::ReplacementVerticalAlignment alignments[] = {
    Text::ReplacementVerticalAlignment::TEXT_BASELINE,
    Text::ReplacementVerticalAlignment::TEXT_BOTTOM,
    Text::ReplacementVerticalAlignment::TEXT_CENTER};
  const float offsets[] = {-4.0f, 4.0f};

  auto checkLayout = [&services](const std::string&                   utf8,
                                 Vector<Text::ReplacementRunSnapshot> candidates,
                                 bool                                 multiline)
  {
    const Vector<Text::Character> text       = Utf32(utf8);
    Text::ReplacementProjection   projection = Text::ReplacementProjection::Build(text, candidates);

    Text::ReplacementLayoutTestOptions options;
    options.contentSize   = Vector2(92.0f, 240.0f);
    options.layoutType    = multiline ? Text::Layout::Engine::MULTI_LINE_BOX
                                      : Text::Layout::Engine::SINGLE_LINE_BOX;
    options.lineWrapMode  = Text::LineWrapMode::CHARACTER;
    options.fontPixelSize = 18.0f;

    Text::ReplacementRenderState result;
    DALI_TEST_CHECK(Text::LayoutReplacementForTest(projection, services, options, result));
    DALI_TEST_EQUALS(result.placements.Count(), candidates.Count(), TEST_LOCATION);

    const Vector<Text::LineRun>& lines = result.processingModel->mVisualModel->mLines;
    for(uint32_t placementIndex = 0u; placementIndex < result.placements.Count(); ++placementIndex)
    {
      const Text::ReplacementPlacement& placement = result.placements[placementIndex];
      DALI_TEST_CHECK(placement.visible);
      DALI_TEST_CHECK(!placement.elided);
      DALI_TEST_CHECK(placement.lineIndex < lines.Count());

      float lineTop = 0.0f;
      for(uint32_t lineIndex = 0u; lineIndex < placement.lineIndex; ++lineIndex)
      {
        lineTop += Text::GetLineHeight(lines[lineIndex], false);
      }
      const Text::LineRun& line       = lines[placement.lineIndex];
      const float          lineBottom = lineTop + line.ascender - line.descender;
      DALI_TEST_CHECK(placement.position.y >= lineTop - Math::MACHINE_EPSILON_1000);
      DALI_TEST_CHECK(placement.position.y + placement.size.y <=
                      lineBottom + Math::MACHINE_EPSILON_1000);

      const Text::ReplacementMetrics& metrics = candidates[placement.sourceRunIndex].metrics;
      TextAbstraction::FontMetrics     surroundingMetrics;
      bool                             hasSurroundingMetrics = false;
      const auto includeTextMetrics = [&](const Text::GlyphRun& glyphRun)
      {
        const Vector<Text::GlyphInfo>& glyphs = result.processingModel->mVisualModel->mGlyphs;
        const Text::GlyphIndex end =
          std::min<Text::GlyphIndex>(glyphRun.glyphIndex + glyphRun.numberOfGlyphs, glyphs.Count());
        for(Text::GlyphIndex glyphIndex = glyphRun.glyphIndex; glyphIndex < end; ++glyphIndex)
        {
          if(glyphs[glyphIndex].fontId == 0u || !result.finalElision.IsOriginalGlyphVisible(glyphIndex))
          {
            continue;
          }
          TextAbstraction::FontMetrics fontMetrics;
          services.fontClient.GetFontMetrics(glyphs[glyphIndex].fontId, fontMetrics);
          surroundingMetrics.ascender = hasSurroundingMetrics
                                          ? std::max(surroundingMetrics.ascender, fontMetrics.ascender)
                                          : fontMetrics.ascender;
          surroundingMetrics.descender = hasSurroundingMetrics
                                           ? std::min(surroundingMetrics.descender, fontMetrics.descender)
                                           : fontMetrics.descender;
          hasSurroundingMetrics = true;
        }
      };
      includeTextMetrics(line.glyphRun);
      if(line.isSplitToTwoHalves)
      {
        includeTextMetrics(line.glyphRunSecondHalf);
      }
      if(!hasSurroundingMetrics)
      {
        TextAbstraction::FontDescription defaultFontDescription;
        const Text::FontId defaultFontId = services.fontClient.GetFontId(defaultFontDescription,
                                                                         options.fontPointSize);
        services.fontClient.GetFontMetrics(defaultFontId, surroundingMetrics);
        hasSurroundingMetrics = true;
      }
      if(metrics.verticalAlignment == Text::ReplacementVerticalAlignment::TEXT_BASELINE)
      {
        DALI_TEST_EQUALS(placement.position.y + placement.size.y,
                         lineTop + line.ascender + metrics.verticalOffset,
                         Math::MACHINE_EPSILON_1000,
                         TEST_LOCATION);
      }
      else if(metrics.verticalAlignment == Text::ReplacementVerticalAlignment::TEXT_BOTTOM)
      {
        DALI_TEST_CHECK(hasSurroundingMetrics);
        DALI_TEST_EQUALS(placement.position.y + placement.size.y,
                         lineTop + line.ascender - surroundingMetrics.descender + metrics.verticalOffset,
                         Math::MACHINE_EPSILON_1000,
                         TEST_LOCATION);
      }
      else
      {
        DALI_TEST_CHECK(hasSurroundingMetrics);
        DALI_TEST_EQUALS(placement.position.y + 0.5f * placement.size.y,
                         lineTop + line.ascender -
                           0.5f * (surroundingMetrics.ascender + surroundingMetrics.descender) +
                           metrics.verticalOffset,
                         Math::MACHINE_EPSILON_1000,
                         TEST_LOCATION);
      }
    }
  };

  for(const Text::ReplacementVerticalAlignment alignment : alignments)
  {
    for(const float offset : offsets)
    {
      Vector<Text::ReplacementRunSnapshot> mixed;
      mixed.PushBack(Candidate(1u, 4u, 34.0f, 44.0f, 401u));
      mixed.PushBack(Candidate(8u, 4u, 18.0f, 12.0f, 402u));
      mixed[0u].metrics.verticalAlignment = alignment;
      mixed[0u].metrics.verticalOffset    = offset;
      mixed[1u].metrics.verticalAlignment = alignment;
      mixed[1u].metrics.verticalOffset    = -offset;
      checkLayout("AiconB\nCiconD", mixed, true);

      Vector<Text::ReplacementRunSnapshot> replacementOnly;
      replacementOnly.PushBack(Candidate(0u, 4u, 30.0f, 38.0f, 403u));
      replacementOnly[0u].metrics.verticalAlignment = alignment;
      replacementOnly[0u].metrics.verticalOffset    = offset;
      checkLayout("icon", replacementOnly, false);
    }
  }

  // Typesetter centers overflowing CLIP content with a negative offset. The
  // separately registered image visual must use that same offset or it drifts
  // down into later lines while the text is clipped around the center.
  Vector<Text::ReplacementRunSnapshot> overflowCandidates;
  overflowCandidates.PushBack(Candidate(13u, 7u, 180.0f, 130.0f, 404u));
  overflowCandidates[0u].metrics.verticalAlignment = Text::ReplacementVerticalAlignment::TEXT_CENTER;
  const Vector<Text::Character> overflowText =
    Utf32("prefix words [large] trailing words that wrap across several lines");
  Text::ReplacementProjection overflowProjection = Text::ReplacementProjection::Build(
    overflowText,
    overflowCandidates);

  Text::ReplacementLayoutTestOptions overflowOptions;
  overflowOptions.contentSize         = Vector2(192.0f, 100.0f);
  overflowOptions.layoutType          = Text::Layout::Engine::MULTI_LINE_BOX;
  overflowOptions.lineWrapMode        = Text::LineWrapMode::WORD;
  overflowOptions.horizontalAlignment = Text::Alignment::CENTER;
  overflowOptions.verticalAlignment   = Text::Alignment::CENTER;
  overflowOptions.fontPixelSize       = 28.0f;

  Text::ReplacementRenderState overflowResult;
  DALI_TEST_CHECK(Text::LayoutReplacementForTest(overflowProjection,
                                                 services,
                                                 overflowOptions,
                                                 overflowResult));
  DALI_TEST_CHECK(overflowResult.layoutSize.height > overflowOptions.contentSize.height);
  DALI_TEST_EQUALS(overflowResult.placements.Count(), 1u, TEST_LOCATION);
  const Text::ReplacementPlacement& overflowPlacement = overflowResult.placements[0u];
  DALI_TEST_CHECK(overflowPlacement.visible);

  const Vector<Text::LineRun>& overflowLines    = overflowResult.processingModel->mVisualModel->mLines;
  float                        unalignedLineTop = 0.0f;
  for(uint32_t lineIndex = 0u; lineIndex < overflowPlacement.lineIndex; ++lineIndex)
  {
    unalignedLineTop += Text::GetLineHeight(overflowLines[lineIndex], false);
  }
  const float expectedVerticalOffset =
    0.5f * (overflowOptions.contentSize.height - overflowResult.layoutSize.height);
  const Text::LineRun& overflowLine      = overflowLines[overflowPlacement.lineIndex];
  const float          alignedLineTop    = unalignedLineTop + expectedVerticalOffset;
  const float          alignedLineBottom = alignedLineTop + overflowLine.ascender - overflowLine.descender;
  DALI_TEST_CHECK(expectedVerticalOffset < 0.0f);
  DALI_TEST_CHECK(overflowPlacement.position.y < unalignedLineTop);
  DALI_TEST_CHECK(overflowPlacement.position.y >= alignedLineTop - Math::MACHINE_EPSILON_1000);
  DALI_TEST_CHECK(overflowPlacement.position.y + overflowPlacement.size.y <=
                  alignedLineBottom + Math::MACHINE_EPSILON_1000);

  END_TEST;
}

int UtcDaliReplacementControllerModelGeometryAndAffinityContractP(void)
{
  UiTestApplication application;

  Text::ControllerPtr     controller     = Text::Controller::New();
  Text::Controller::Impl& controllerImpl = Text::Controller::Impl::GetImplementation(*controller.Get());
  controller->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  controller->SetText("AiconB");

  Text::ReplacementSourceSnapshot source;
  source.runs.PushBack(Candidate(1u, 4u, 32.0f, 24.0f, 501u));
  source.sourceRevision                                 = 21u;
  source.hasValidReplacementSource                      = true;
  controllerImpl.GetOrCreateReplacementSourceSnapshot() = source;
  controller->Relayout(Size(180.0f, 80.0f));

  const Text::ReplacementRenderState& replacement = controllerImpl.GetReplacementRenderState();
  DALI_TEST_CHECK(replacement.processingModel);
  DALI_TEST_EQUALS(controller->GetLogicalTextModel()->GetNumberOfCharacters(), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetRenderTextModel()->GetNumberOfCharacters(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(controller->GetLogicalTextModel() != controller->GetRenderTextModel());
  DALI_TEST_EQUALS(controller->GetLineCount(180.0f),
                   replacement.processingModel->GetNumberOfLines(),
                   TEST_LOCATION);

  const Text::ReplacementPlacement& placement       = replacement.placements[0u];
  const Bounds                      characterBounds = controller->GetCharacterBoundingRectangle(2u);
  DALI_TEST_EQUALS(characterBounds.x, placement.position.x, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(characterBounds.y, placement.position.y, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(characterBounds.width, placement.size.x, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(characterBounds.height, placement.size.y, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  const Vector<Vector2> sizes     = controller->GetTextSize(2u, 2u);
  const Vector<Vector2> positions = controller->GetTextPosition(2u, 2u);
  DALI_TEST_EQUALS(sizes.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(positions.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(sizes[0u], placement.size, TEST_LOCATION);
  DALI_TEST_EQUALS(positions[0u], placement.position, TEST_LOCATION);

  const Bounds rangeBounds = controller->GetTextBoundingRectangle(1u, 4u);
  DALI_TEST_CHECK(rangeBounds.width >= placement.size.x - Math::MACHINE_EPSILON_1000);
  DALI_TEST_CHECK(rangeBounds.height >= placement.size.y - Math::MACHINE_EPSILON_1000);

  const int physicalLeft  = controller->GetCharacterIndexAtPosition(placement.position.x + 1.0f,
                                                                    placement.position.y + placement.size.y * 0.5f);
  const int physicalRight = controller->GetCharacterIndexAtPosition(placement.position.x + placement.size.x - 1.0f,
                                                                    placement.position.y + placement.size.y * 0.5f);
  DALI_TEST_EQUALS(physicalLeft, placement.lineDirection ? 5 : 1, TEST_LOCATION);
  DALI_TEST_EQUALS(physicalRight, placement.lineDirection ? 1 : 5, TEST_LOCATION);

  Text::ControllerPtr     rtlController = Text::Controller::New();
  Text::Controller::Impl& rtlImpl       = Text::Controller::Impl::GetImplementation(*rtlController.Get());
  rtlController->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  rtlController->SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  rtlController->SetText("\xD7\x90ICON\xD7\x91");
  Text::ReplacementSourceSnapshot rtlSource;
  rtlSource.runs.PushBack(Candidate(1u, 4u, 36.0f, 24.0f, 502u));
  rtlSource.sourceRevision                       = 22u;
  rtlSource.hasValidReplacementSource            = true;
  rtlImpl.GetOrCreateReplacementSourceSnapshot() = rtlSource;
  rtlController->Relayout(Size(180.0f, 80.0f), LayoutDirection::RIGHT_TO_LEFT);
  // The internal harness interposes no-op bidi reorder. Force the placement's
  // already-computed visual direction to exercise Controller's RTL edge affinity.
  Text::ReplacementPlacement& rtlPlacement = rtlImpl.GetOrCreateReplacementRenderState().placements[0u];
  rtlPlacement.lineDirection               = true;
  DALI_TEST_EQUALS(rtlController->GetCharacterIndexAtPosition(
                     rtlPlacement.position.x + 1.0f,
                     rtlPlacement.position.y + rtlPlacement.size.y * 0.5f),
                   5,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(rtlController->GetCharacterIndexAtPosition(
                     rtlPlacement.position.x + rtlPlacement.size.x - 1.0f,
                     rtlPlacement.position.y + rtlPlacement.size.y * 0.5f),
                   1,
                   TEST_LOCATION);

  Text::ControllerPtr     adjacentController = Text::Controller::New();
  Text::Controller::Impl& adjacentImpl       = Text::Controller::Impl::GetImplementation(*adjacentController.Get());
  adjacentController->SetText("Aabcd");
  Text::ReplacementSourceSnapshot adjacentSource;
  adjacentSource.runs.PushBack(Candidate(1u, 2u, 20.0f, 18.0f, 503u));
  adjacentSource.runs.PushBack(Candidate(3u, 2u, 22.0f, 20.0f, 504u));
  adjacentSource.sourceRevision                       = 23u;
  adjacentSource.hasValidReplacementSource            = true;
  adjacentImpl.GetOrCreateReplacementSourceSnapshot() = adjacentSource;
  adjacentController->Relayout(Size(120.0f, 60.0f));
  DALI_TEST_EQUALS(adjacentImpl.GetReplacementRenderState().placements.Count(), 2u, TEST_LOCATION);
  const Bounds firstAdjacent = adjacentController->GetCharacterBoundingRectangle(2u);
  const Bounds lastCharacter = adjacentController->GetCharacterBoundingRectangle(4u);
  DALI_TEST_EQUALS(firstAdjacent.width, 20.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(lastCharacter.width, 22.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Text::ControllerPtr     elidedController = Text::Controller::New();
  Text::Controller::Impl& elidedImpl       = Text::Controller::Impl::GetImplementation(*elidedController.Get());
  elidedController->SetTextElideEnabled(true);
  elidedController->SetEllipsisPosition(Text::EllipsisPosition::END);
  elidedController->SetText("AabcdefghijZ");
  Text::ReplacementSourceSnapshot elidedSource;
  for(uint32_t index = 0u; index < 5u; ++index)
  {
    elidedSource.runs.PushBack(Candidate(1u + index * 2u, 2u, 20.0f, 18.0f, 510u + index));
  }
  elidedSource.sourceRevision                       = 24u;
  elidedSource.hasValidReplacementSource            = true;
  elidedImpl.GetOrCreateReplacementSourceSnapshot() = elidedSource;
  elidedController->Relayout(Size(62.0f, 40.0f));
  bool foundElided = false;
  for(const Text::ReplacementPlacement& elidedPlacement : elidedImpl.GetReplacementRenderState().placements)
  {
    if(elidedPlacement.elided)
    {
      foundElided                             = true;
      const Text::CharacterIndex index        = elidedPlacement.logicalCharacterRange.characterIndex;
      const Bounds               elidedBounds = elidedController->GetCharacterBoundingRectangle(index);
      DALI_TEST_EQUALS(elidedBounds.width, 0.0f, TEST_LOCATION);
      DALI_TEST_EQUALS(elidedBounds.height, 0.0f, TEST_LOCATION);
      DALI_TEST_EQUALS(elidedController->GetTextSize(index, index).Count(), 0u, TEST_LOCATION);
      DALI_TEST_EQUALS(elidedController->GetTextBoundingRectangle(index, index).width, 0.0f, TEST_LOCATION);
      break;
    }
  }
  DALI_TEST_CHECK(foundElided);

  // CENTER + END ellipsis repositions the final compacted sequence. Public
  // geometry and hit testing must consume the same placement used by the
  // renderer and ImageVisual, not the pre-elision LineRun alignment.
  Text::ControllerPtr     centeredController = Text::Controller::New();
  Text::Controller::Impl& centeredImpl       = Text::Controller::Impl::GetImplementation(*centeredController.Get());
  centeredController->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  centeredController->SetHorizontalAlignment(Text::Alignment::CENTER);
  centeredController->SetTextElideEnabled(true);
  centeredController->SetEllipsisPosition(Text::EllipsisPosition::END);
  centeredController->SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  centeredController->SetText("Long prefix words\uFFFCtrailing text");
  Text::ReplacementSourceSnapshot centeredSource;
  centeredSource.runs.PushBack(Candidate(17u, 1u, 70.0f, 28.0f, 520u));
  centeredSource.sourceRevision                       = 25u;
  centeredSource.hasValidReplacementSource            = true;
  centeredImpl.GetOrCreateReplacementSourceSnapshot() = centeredSource;

  bool foundCenteredVisibleElision = false;
  for(float width = 80.0f; width <= 360.0f && !foundCenteredVisibleElision; width += 2.0f)
  {
    centeredController->Relayout(Size(width, 80.0f));
    const Text::ReplacementRenderState& centeredState = centeredImpl.GetReplacementRenderState();
    if(!centeredState.finalElision.textElided || centeredState.placements.Empty() || !centeredState.placements[0u].visible)
    {
      continue;
    }

    const Text::ReplacementPlacement& centeredPlacement = centeredState.placements[0u];
    const Bounds                      centeredBounds    = centeredController->GetCharacterBoundingRectangle(17u);
    DALI_TEST_EQUALS(centeredBounds.x,
                     centeredPlacement.position.x,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(centeredBounds.width,
                     centeredPlacement.size.x,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    const Vector<Vector2> centeredPositions = centeredController->GetTextPosition(17u, 17u);
    DALI_TEST_EQUALS(centeredPositions.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(centeredPositions[0u], centeredPlacement.position, TEST_LOCATION);
    DALI_TEST_EQUALS(centeredController->GetCharacterIndexAtPosition(
                       centeredPlacement.position.x + 1.0f,
                       centeredPlacement.position.y + centeredPlacement.size.y * 0.5f),
                     17,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(centeredController->GetCharacterIndexAtPosition(
                       centeredPlacement.position.x + centeredPlacement.size.x - 1.0f,
                       centeredPlacement.position.y + centeredPlacement.size.y * 0.5f),
                     18,
                     TEST_LOCATION);
    foundCenteredVisibleElision = true;
  }
  DALI_TEST_CHECK(foundCenteredVisibleElision);

  DALI_TEST_CHECK(!controller->GetTextSize(std::numeric_limits<Text::CharacterIndex>::max(),
                                           std::numeric_limits<Text::CharacterIndex>::max())
                     .Count());
  DALI_TEST_CHECK(controller->GetTextSize(1u, std::numeric_limits<Text::CharacterIndex>::max()).Count() > 0u);

  controllerImpl.GetOrCreateReplacementSourceSnapshot().runs.Clear();
  controller->SetText("plain");
  controller->Relayout(Size(181.0f, 80.0f));
  DALI_TEST_CHECK(controller->GetLogicalTextModel() == controller->GetRenderTextModel());
  DALI_TEST_EQUALS(controller->GetLogicalTextModel()->GetNumberOfCharacters(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliReplacementEditableCaretAndVisualLayerP(void)
{
  UiTestApplication application;

  const auto createEditableController = [](bool multiline = true)
  {
    Text::ControllerPtr controller = Text::Controller::New();
    Text::DecoratorPtr  decorator  = Text::Decorator::New(*controller, *controller);
    Dali::InputMethodContext inputMethodContext;
    controller->EnableTextInput(decorator, inputMethodContext);
    controller->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(multiline);
    controller->SetTextElideEnabled(false);
    return controller;
  };

  Text::ControllerPtr     controller = createEditableController();
  Text::Controller::Impl& impl       = Text::Controller::Impl::GetImplementation(*controller.Get());
  controller->SetText("AiconB");

  Text::ReplacementSourceSnapshot source;
  source.runs.PushBack(Candidate(1u, 4u, 84.0f, 112.0f, 531u));
  source.sourceRevision                       = 31u;
  source.hasValidReplacementSource            = true;
  impl.GetOrCreateReplacementSourceSnapshot() = source;
  controller->Relayout(Size(220.0f, 180.0f));

  const Text::ReplacementPlacement& placement = impl.GetReplacementRenderState().placements[0u];
  DALI_TEST_CHECK(placement.visible);
  DALI_TEST_CHECK(placement.leadingCaretMetric.height > 0.0f);
  DALI_TEST_CHECK(placement.trailingCaretMetric.height > 0.0f);

  Text::CursorInfo before;
  Text::CursorInfo after;
  impl.GetCursorPosition(1u, before);
  impl.GetCursorPosition(5u, after);

  DALI_TEST_CHECK(before.hasPrimaryCaretGeometry);
  DALI_TEST_CHECK(after.hasPrimaryCaretGeometry);
  DALI_TEST_EQUALS(before.primaryPosition.x,
                   placement.position.x,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(after.primaryPosition.x,
                   placement.position.x + placement.size.x,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(before.primaryCaretPosition.y,
                   placement.baseline - placement.leadingCaretMetric.ascender,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(after.primaryCaretPosition.y,
                   placement.baseline - placement.trailingCaretMetric.ascender,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_CHECK(after.primaryCaretHeight < placement.size.y);
  DALI_TEST_CHECK(after.lineHeight > after.primaryCaretHeight);

  impl.UpdateCursorPosition(after);
  const Vector2& storedCursorPosition = impl.mEventData->mDecorator->GetPosition(Text::PRIMARY_CURSOR);
  DALI_TEST_EQUALS(storedCursorPosition,
                   after.primaryPosition + impl.mModel->mScrollPosition,
                   TEST_LOCATION);
  float storedX;
  float storedY;
  float storedCursorHeight;
  float storedLineHeight;
  impl.mEventData->mDecorator->GetPosition(Text::PRIMARY_CURSOR,
                                           storedX,
                                           storedY,
                                           storedCursorHeight,
                                           storedLineHeight);
  DALI_TEST_EQUALS(storedCursorHeight,
                   after.primaryCursorHeight,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(storedLineHeight, after.lineHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  const auto [visibleTop, visibleBottom] = impl.CalculateScrollTarget(after);
  DALI_TEST_CHECK(visibleBottom - visibleTop >= after.lineHeight - Math::MACHINE_EPSILON_1000);

  struct CaretCase
  {
    Text::ReplacementPlacement placement;
    Text::CursorInfo            before;
    Text::CursorInfo            after;
  };
  const auto layoutCaretCase = [&createEditableController](const char*                         text,
                                                           Text::CharacterIndex                start,
                                                           Text::Length                        length,
                                                           Text::ReplacementRunSnapshot        run,
                                                           const Size&                         controlSize,
                                                           bool                                multiline)
  {
    Text::ControllerPtr     caseController = createEditableController(multiline);
    Text::Controller::Impl& caseImpl = Text::Controller::Impl::GetImplementation(*caseController.Get());
    caseController->SetText(text);
    run.logicalCharacterRange = Text::CharacterRun{start, length};
    Text::ReplacementSourceSnapshot caseSource;
    caseSource.runs.PushBack(run);
    caseSource.sourceRevision                       = run.occurrenceIdentity;
    caseSource.hasValidReplacementSource            = true;
    caseImpl.GetOrCreateReplacementSourceSnapshot() = caseSource;
    caseController->Relayout(controlSize);

    CaretCase result;
    result.placement = caseImpl.GetReplacementRenderState().placements[0u];
    caseImpl.GetCursorPosition(start, result.before);
    caseImpl.GetCursorPosition(start + length, result.after);
    return result;
  };

  Text::ReplacementRunSnapshot smallRun = Candidate(1u, 4u, 18.0f, 14.0f, 533u);
  const CaretCase small = layoutCaretCase("AiconB", 1u, 4u, smallRun, Size(220.0f, 80.0f), false);
  DALI_TEST_EQUALS(small.before.primaryCaretHeight,
                   before.primaryCaretHeight,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(small.after.primaryCaretHeight,
                   placement.trailingCaretMetric.height,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_CHECK(small.placement.size.y < placement.size.y);

  const Text::ReplacementVerticalAlignment alignments[] = {
    Text::ReplacementVerticalAlignment::TEXT_BASELINE,
    Text::ReplacementVerticalAlignment::TEXT_BOTTOM,
    Text::ReplacementVerticalAlignment::TEXT_CENTER};
  const float offsets[] = {-9.0f, 7.0f};
  uint64_t    occurrence = 534u;
  for(const Text::ReplacementVerticalAlignment alignment : alignments)
  {
    for(const float offset : offsets)
    {
      Text::ReplacementRunSnapshot alignedRun = Candidate(1u, 4u, 88.0f, 74.0f, occurrence++);
      alignedRun.metrics.verticalAlignment     = alignment;
      alignedRun.metrics.verticalOffset        = offset;
      const CaretCase aligned = layoutCaretCase("AiconB",
                                                1u,
                                                4u,
                                                alignedRun,
                                                Size(220.0f, 160.0f),
                                                true);
      DALI_TEST_EQUALS(aligned.before.primaryCaretHeight,
                       small.before.primaryCaretHeight,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(aligned.after.primaryCaretHeight,
                       small.after.primaryCaretHeight,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(aligned.before.primaryCaretPosition.y,
                       aligned.placement.baseline - aligned.placement.leadingCaretMetric.ascender,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_CHECK(std::isfinite(aligned.before.primaryCaretPosition.x));
      DALI_TEST_CHECK(std::isfinite(aligned.before.primaryCaretPosition.y));
      DALI_TEST_CHECK(std::isfinite(aligned.before.primaryCaretHeight));
    }
  }

  Text::ReplacementRunSnapshot tallerThanControlRun = Candidate(1u, 4u, 92.0f, 260.0f, occurrence++);
  const CaretCase tallerThanControl = layoutCaretCase("AiconB",
                                                      1u,
                                                      4u,
                                                      tallerThanControlRun,
                                                      Size(220.0f, 160.0f),
                                                      true);
  DALI_TEST_CHECK(tallerThanControl.after.primaryCaretHeight < tallerThanControl.placement.size.y);
  DALI_TEST_CHECK(tallerThanControl.after.lineHeight > tallerThanControl.after.primaryCaretHeight);

  // The internal harness interposes a no-op bidi reorder. Exercise the
  // production RTL boundary branch with the final placement direction that
  // the real-layout diagnostic obtains from the bidi service.
  Text::ReplacementPlacement& rtlPlacement = impl.GetOrCreateReplacementRenderState().placements[0u];
  rtlPlacement.lineDirection               = true;
  impl.GetCursorPosition(1u, before);
  impl.GetCursorPosition(5u, after);
  DALI_TEST_EQUALS(before.primaryPosition.x,
                   rtlPlacement.position.x + rtlPlacement.size.x,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(after.primaryPosition.x,
                   rtlPlacement.position.x,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);

  Text::ControllerPtr     adjacentController = createEditableController(false);
  Text::Controller::Impl& adjacentImpl =
    Text::Controller::Impl::GetImplementation(*adjacentController.Get());
  adjacentController->SetText("AabcdwxyzB");
  Text::ReplacementSourceSnapshot adjacentSource;
  adjacentSource.runs.PushBack(Candidate(1u, 4u, 38.0f, 28.0f, occurrence++));
  adjacentSource.runs.PushBack(Candidate(5u, 4u, 64.0f, 44.0f, occurrence++));
  adjacentSource.sourceRevision                       = occurrence;
  adjacentSource.hasValidReplacementSource            = true;
  adjacentImpl.GetOrCreateReplacementSourceSnapshot() = adjacentSource;
  adjacentController->Relayout(Size(260.0f, 100.0f));
  const Text::ReplacementPlacement& secondAdjacent =
    adjacentImpl.GetReplacementRenderState().placements[1u];
  Text::CursorInfo sharedBoundary;
  adjacentImpl.GetCursorPosition(5u, sharedBoundary);
  DALI_TEST_CHECK(sharedBoundary.hasPrimaryCaretGeometry);
  DALI_TEST_EQUALS(sharedBoundary.primaryPosition.x,
                   secondAdjacent.position.x,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);

  Text::ControllerPtr     replacementOnlyController = createEditableController();
  Text::Controller::Impl& replacementOnlyImpl =
    Text::Controller::Impl::GetImplementation(*replacementOnlyController.Get());
  replacementOnlyController->SetText("icon");
  Text::ReplacementSourceSnapshot replacementOnlySource;
  replacementOnlySource.runs.PushBack(Candidate(0u, 4u, 96.0f, 128.0f, 532u));
  replacementOnlySource.sourceRevision                       = 32u;
  replacementOnlySource.hasValidReplacementSource            = true;
  replacementOnlyImpl.GetOrCreateReplacementSourceSnapshot() = replacementOnlySource;
  replacementOnlyController->Relayout(Size(220.0f, 180.0f));

  Text::CursorInfo replacementOnlyBefore;
  Text::CursorInfo replacementOnlyAfter;
  replacementOnlyImpl.GetCursorPosition(0u, replacementOnlyBefore);
  replacementOnlyImpl.GetCursorPosition(4u, replacementOnlyAfter);
  DALI_TEST_CHECK(replacementOnlyBefore.hasPrimaryCaretGeometry);
  DALI_TEST_CHECK(replacementOnlyAfter.hasPrimaryCaretGeometry);
  DALI_TEST_CHECK(replacementOnlyAfter.primaryCaretHeight > 0.0f);
  DALI_TEST_CHECK(replacementOnlyAfter.primaryCaretHeight < replacementOnlyAfter.lineHeight);

  Text::ControllerPtr     ordinaryController = createEditableController();
  Text::Controller::Impl& ordinaryImpl =
    Text::Controller::Impl::GetImplementation(*ordinaryController.Get());
  ordinaryController->SetText("ordinary editable text");
  ordinaryController->Relayout(Size(220.0f, 80.0f));
  Text::CursorInfo ordinaryCursor;
  ordinaryImpl.GetCursorPosition(8u, ordinaryCursor);
  DALI_TEST_CHECK(!ordinaryCursor.hasPrimaryCaretGeometry);
  DALI_TEST_CHECK(!ordinaryCursor.hasSecondaryCaretGeometry);
  DALI_TEST_CHECK(ordinaryCursor.primaryCursorHeight > 0.0f);

  View  owner         = View::New();
  Actor contentParent = Actor::New();
  Actor highlight     = Actor::New();
  Actor textActor     = Actor::New();
  Actor cursorLayer   = Actor::New();
  contentParent.Add(highlight);
  contentParent.Add(textActor);
  contentParent.Add(cursorLayer);
  const auto getChildOrder = [&contentParent](Actor child)
  {
    for(uint32_t index = 0u; index < contentParent.GetChildCount(); ++index)
    {
      if(contentParent.GetChildAt(index) == child)
      {
        return index;
      }
    }
    return std::numeric_limits<uint32_t>::max();
  };

  {
    Dali::Ui::Internal::Text::EditableInlineReplacementData data(owner);
    data.PlaceVisualLayer(contentParent, textActor, cursorLayer, Vector2(200.0f, 160.0f));
    DALI_TEST_CHECK(data.visualLayer.GetParent() == contentParent);
    DALI_TEST_CHECK(getChildOrder(highlight) < getChildOrder(data.visualLayer));
    DALI_TEST_CHECK(getChildOrder(data.visualLayer) < getChildOrder(textActor));
    DALI_TEST_CHECK(getChildOrder(data.visualLayer) < getChildOrder(cursorLayer));
  }
  DALI_TEST_EQUALS(contentParent.GetChildCount(), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliReplacementEditableVisualLayerRecreationP(void)
{
  UiTestApplication application;

  struct NoopRelayoutContainer : public RelayoutContainer
  {
    void Add(const Actor&, const Vector2&) override
    {
    }
  } relayoutContainer;

  auto buildImageText = [](const char* source, const Vector2& reservedSize)
  {
    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("AiconB\nsecond line");
    DALI_TEST_CHECK(builder.SetSpan(
      Text::ImageSpan::New(Text::ImageAttributes(source, reservedSize)), 1u, 5u));
    return builder.Build();
  };

  const Text::StyledText styledTexts[] = {
    buildImageText("unused-a.png", Vector2(32.0f, 24.0f)),
    buildImageText("unused-a.png", Vector2(48.0f, 30.0f)),
    buildImageText("unused-b.png", Vector2(32.0f, 24.0f)),
    buildImageText("unused-b.png", Vector2(48.0f, 30.0f))};

  Text::StyledTextBuilder coloredBuilder = Text::StyledTextBuilder::New("colored text");
  DALI_TEST_CHECK(coloredBuilder.SetSpan(Text::ForegroundColorSpan::New(UiColor(Color::RED)), 0u, 7u));
  const Text::StyledText coloredText = coloredBuilder.Build();

  auto countInlineVisuals = [](Ui::View layer)
  {
    uint32_t count = 0u;
    auto&    viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(layer));
    for(uint32_t slot = 0u; slot < 16u; ++slot)
    {
      const std::string name = "__dali_ui_inline_replacement_" + std::to_string(slot);
      const Property::Index index = layer.GetPropertyIndex(Dali::String(name.c_str()));
      if(index != Property::INVALID_INDEX && viewData.GetVisual(index))
      {
        ++count;
      }
    }
    return count;
  };

  auto checkLayer = [&application, &countInlineVisuals](
                      Dali::Ui::Internal::Text::EditableInlineReplacementData* data,
                      const Vector2&                                            expectedSize)
  {
    DALI_TEST_CHECK(data);
    DALI_TEST_CHECK(data->visualLayer.GetParent());
    DALI_TEST_EQUALS(data->visualLayer.GetRequestedWidth(), expectedSize.x, TEST_LOCATION);
    DALI_TEST_EQUALS(data->visualLayer.GetRequestedHeight(), expectedSize.y, TEST_LOCATION);
    DALI_TEST_EQUALS(data->visualLayer.GetProperty<Vector3>(Actor::Property::SIZE).GetVectorXY(),
                     expectedSize,
                     0.01f,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(countInlineVisuals(data->visualLayer), 1u, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(data->visualLayer.GetCurrentProperty<Vector3>(Actor::Property::SIZE).GetVectorXY(),
                     expectedSize,
                     0.01f,
                     TEST_LOCATION);
  };

  struct LayerMeasureCounter
  {
    MeasuredSize OnMeasure(Ui::View layer, float, float)
    {
      ++count;
      return MeasuredSize(layer.GetRequestedWidth(), layer.GetRequestedHeight());
    }

    uint32_t count{0u};
  };

  auto exerciseControl = [&](auto control, auto& impl, const Vector2& smallSize, const Vector2& largeSize)
  {
    control.SetPadding(Extents(0u, 0u, 0u, 0u));
    const Vector2 lifecycleSizes[] = {smallSize, smallSize, largeSize, largeSize, smallSize};

    for(uint32_t cycle = 0u; cycle < 5u; ++cycle)
    {
      Vector2 currentSize = lifecycleSizes[cycle];
      control.SetStyledText(styledTexts[cycle % 4u]);
      impl.OnRelayout(currentSize, relayoutContainer);

      auto* data = Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl);
      checkLayer(data, currentSize);

      if(cycle == 0u)
      {
        auto* const initialData  = data;
        Ui::View    initialLayer = data->visualLayer;
        const Property::Index visualIndex =
          initialLayer.GetPropertyIndex("__dali_ui_inline_replacement_0");
        DALI_TEST_CHECK(visualIndex != Property::INVALID_INDEX);
        auto& initialViewData =
          Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(initialLayer));
        Ui::Integration::Visual::Base initialVisual = initialViewData.GetVisual(visualIndex);
        DALI_TEST_CHECK(initialVisual);

        LayerMeasureCounter measureCounter;
        initialLayer.SetMeasureCallback(
          MeasureCallback::New(&measureCounter, &LayerMeasureCounter::OnMeasure));
        initialLayer.Measure(1000.0f, 1000.0f);
        DALI_TEST_EQUALS(measureCounter.count, 1u, TEST_LOCATION);

        for(uint32_t repeat = 0u; repeat < 10u; ++repeat)
        {
          impl.OnRelayout(smallSize, relayoutContainer);
          data = Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl);
          DALI_TEST_CHECK(data == initialData);
          DALI_TEST_CHECK(data->visualLayer == initialLayer);
          checkLayer(data, smallSize);
          initialLayer.Measure(1000.0f, 1000.0f);
          DALI_TEST_EQUALS(measureCounter.count, 1u, TEST_LOCATION);
          DALI_TEST_CHECK(initialViewData.GetVisual(visualIndex) == initialVisual);
        }

        impl.OnRelayout(largeSize, relayoutContainer);
        data        = Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl);
        currentSize = largeSize;
        checkLayer(data, currentSize);
        initialLayer.Measure(1000.0f, 1000.0f);
        DALI_TEST_EQUALS(measureCounter.count, 2u, TEST_LOCATION);
        DALI_TEST_CHECK(initialViewData.GetVisual(visualIndex) == initialVisual);

        impl.OnRelayout(largeSize, relayoutContainer);
        checkLayer(Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl), currentSize);
        initialLayer.Measure(1000.0f, 1000.0f);
        DALI_TEST_EQUALS(measureCounter.count, 2u, TEST_LOCATION);
        DALI_TEST_CHECK(initialViewData.GetVisual(visualIndex) == initialVisual);
        initialLayer.SetMeasureCallback({});

        for(uint32_t variant = 1u; variant < 4u; ++variant)
        {
          control.SetStyledText(styledTexts[variant]);
          impl.OnRelayout(currentSize, relayoutContainer);
          data = Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl);
          DALI_TEST_CHECK(data == initialData);
          checkLayer(data, currentSize);
        }
      }

      data = Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl);
      Ui::View oldLayer = data->visualLayer;
      control.SetStyledText(styledTexts[cycle % 4u]);
      impl.OnRelayout(currentSize, relayoutContainer);
      data = Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl);
      DALI_TEST_CHECK(data);
      DALI_TEST_CHECK(data->visualLayer == oldLayer);
      checkLayer(data, currentSize);

      control.SetText("plain text");
      impl.OnRelayout(currentSize, relayoutContainer);
      DALI_TEST_CHECK(!Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl));
      DALI_TEST_CHECK(!oldLayer.GetParent());
      DALI_TEST_EQUALS(countInlineVisuals(oldLayer), 0u, TEST_LOCATION);
    }

    control.SetStyledText(coloredText);
    impl.OnRelayout(smallSize, relayoutContainer);
    DALI_TEST_CHECK(!Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl));
    control.SetText("plain text");
    impl.OnRelayout(smallSize, relayoutContainer);
    DALI_TEST_CHECK(!Dali::Ui::Internal::Text::GetEditableInlineReplacementData(impl));
  };

  InputField field = InputField::New();
  auto& fieldImpl = static_cast<Dali::Ui::Integration::InputFieldImpl&>(field.GetImplementation());
  exerciseControl(field, fieldImpl, Vector2(240.0f, 64.0f), Vector2(300.0f, 72.0f));

  InputEditor editor = InputEditor::New();
  auto& editorImpl = static_cast<Dali::Ui::Integration::InputEditorImpl&>(editor.GetImplementation());
  exerciseControl(editor, editorImpl, Vector2(240.0f, 120.0f), Vector2(300.0f, 160.0f));

  END_TEST;
}

int UtcDaliInlineReplacementManagerDescriptorAndOwnershipP(void)
{
  UiTestApplication application;

  Text::ReplacementSourceSnapshot source;
  source.sourceRevision            = 5u;
  Text::ReplacementRunSnapshot run = Candidate(1u, 4u, 24.0f, 18.0f, 321u);
  run.type                         = Text::ReplacementType::IMAGE;
  run.occurrenceIdentity           = 11u;
  run.image.source                 = "missing-inline-manager-test.png";
  source.runs.PushBack(run);

  Vector<Text::ReplacementPlacement> placements;
  Text::ReplacementPlacement         placement;
  placement.logicalCharacterRange = run.logicalCharacterRange;
  placement.sourceRunIndex        = 0u;
  placement.occurrenceIdentity    = run.occurrenceIdentity;
  placement.position              = Vector2(4.0f, 6.0f);
  placement.size                  = Vector2(24.0f, 18.0f);
  placement.visible               = true;
  placements.PushBack(placement);

  View                                                firstOwner  = View::New();
  View                                                secondOwner = View::New();
  Dali::Ui::Internal::Text::InlineReplacementViewHost firstHost(
    firstOwner,
    Ui::Integration::DepthIndex::CONTENT + 1);
  Dali::Ui::Internal::Text::InlineReplacementViewHost secondHost(
    secondOwner,
    Ui::Integration::DepthIndex::CONTENT + 7);
  Dali::Ui::Internal::Text::InlineReplacementManager firstManager;
  Dali::Ui::Internal::Text::InlineReplacementManager secondManager;

  const Property::Index firstVisualIndex  = firstHost.AllocateVisualSlot();
  const Property::Index secondVisualIndex = secondHost.AllocateVisualSlot();
  DALI_TEST_CHECK(firstVisualIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(secondVisualIndex != Property::INVALID_INDEX);
  firstHost.ReleaseVisualSlot(firstVisualIndex);
  secondHost.ReleaseVisualSlot(secondVisualIndex);
  auto& firstViewData  = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(firstOwner));
  auto& secondViewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(secondOwner));

  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2(3.0f, 2.0f),
                                      Vector2(94.0f, 36.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      5u));
  DALI_TEST_EQUALS(firstOwner.GetChildCount(), 0u, TEST_LOCATION);

  Ui::Integration::Visual::Base inlineVisual = firstViewData.GetVisual(firstVisualIndex);
  DALI_TEST_CHECK(inlineVisual);
  auto& inlineVisualImpl = Ui::GetImplementation(inlineVisual);
  DALI_TEST_CHECK(!inlineVisualImpl.IsFittingModeRequired());
  DALI_TEST_CHECK(!inlineVisualImpl.IsResourceReadyRelayoutRequired());
  DALI_TEST_EQUALS(inlineVisual.GetDepthIndex(), Ui::Integration::DepthIndex::CONTENT + 1, TEST_LOCATION);

  int  expectedDesiredWidth  = 24;
  int  expectedDesiredHeight = 18;
  auto getTransform          = [&inlineVisual, &expectedDesiredWidth, &expectedDesiredHeight](Vector2& size, Vector2& offset)
  {
    Property::Map visualMap;
    inlineVisual.CreatePropertyMap(visualMap);

    int fittingMode = -1;
    DALI_TEST_CHECK(visualMap.Find(Ui::ImageVisualPropertyIndex::FITTING_MODE)->Get(fittingMode));
    DALI_TEST_EQUALS(fittingMode,
                     static_cast<int>(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO),
                     TEST_LOCATION);

    int  desiredWidth          = 0;
    int  desiredHeight         = 0;
    bool orientationCorrection = false;
    DALI_TEST_CHECK(visualMap.Find(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH)->Get(desiredWidth));
    DALI_TEST_CHECK(visualMap.Find(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT)->Get(desiredHeight));
    DALI_TEST_CHECK(visualMap.Find(Ui::ImageVisualPropertyIndex::ORIENTATION_CORRECTION)->Get(orientationCorrection));
    DALI_TEST_EQUALS(desiredWidth, expectedDesiredWidth, TEST_LOCATION);
    DALI_TEST_EQUALS(desiredHeight, expectedDesiredHeight, TEST_LOCATION);
    DALI_TEST_CHECK(orientationCorrection);

    Property::Map transform;
    DALI_TEST_CHECK(visualMap.Find(Ui::VisualBasePropertyIndex::TRANSFORM)->Get(transform));
    DALI_TEST_CHECK(transform.Find(Ui::Visual::Transform::Property::SIZE)->Get(size));
    DALI_TEST_CHECK(transform.Find(Ui::Visual::Transform::Property::OFFSET)->Get(offset));
  };

  auto getPixelArea = [&inlineVisual]()
  {
    Property::Map visualMap;
    inlineVisual.CreatePropertyMap(visualMap);
    Vector4 pixelArea;
    DALI_TEST_CHECK(visualMap.Find(Ui::ImageVisualPropertyIndex::PIXEL_AREA)->Get(pixelArea));
    return pixelArea;
  };

  auto getOpacity = [&inlineVisual]()
  {
    Property::Map visualMap;
    inlineVisual.CreatePropertyMap(visualMap);
    float opacity = 1.0f;
    DALI_TEST_CHECK(visualMap.Find(Ui::VisualBasePropertyIndex::OPACITY)->Get(opacity));
    return opacity;
  };

  Vector2 inlineSize;
  Vector2 inlineOffset;
  getTransform(inlineSize, inlineOffset);
  DALI_TEST_EQUALS(inlineSize, Vector2(24.0f, 18.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(inlineOffset, Vector2(7.0f, 8.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(getOpacity(), 0.0f, TEST_LOCATION);

  Ui::Integration::Visual::Base originalVisual = inlineVisual;

  // A pending image owns its final hidden geometry but cannot draw a stretched
  // reserved-box frame. Failure keeps the synthetic advance, unregisters the
  // failed visual and does not request text relayout.
  inlineVisualImpl.ResourceReady(Ui::Visual::ResourceStatus::FAILED);
  firstManager.Refresh();
  DALI_TEST_CHECK(!firstViewData.GetVisual(firstVisualIndex));

  DALI_TEST_CHECK(secondManager.Update(secondHost,
                                       source,
                                       placements,
                                       Vector2::ZERO,
                                       Vector2(80.0f, 30.0f),
                                       Vector2(80.0f, 30.0f),
                                       1.0f,
                                       5u));
  Ui::Integration::Visual::Base secondVisual = secondViewData.GetVisual(secondVisualIndex);
  DALI_TEST_CHECK(secondVisual);
  DALI_TEST_EQUALS(secondVisual.GetDepthIndex(), Ui::Integration::DepthIndex::CONTENT + 7, TEST_LOCATION);

  // A stale request is rejected without disturbing either control-local entry.
  DALI_TEST_CHECK(!firstManager.Update(firstHost,
                                       source,
                                       placements,
                                       Vector2::ZERO,
                                       Vector2(100.0f, 40.0f),
                                       Vector2(100.0f, 40.0f),
                                       1.0f,
                                       4u));
  DALI_TEST_CHECK(!firstViewData.GetVisual(firstVisualIndex));

  // Hash equality cannot hide a source change: source string is correctness identity.
  source.runs[0u].image.source = "other-missing-inline-manager-test.png";
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(100.0f, 40.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      5u));
  inlineVisual = firstViewData.GetVisual(firstVisualIndex);
  DALI_TEST_CHECK(inlineVisual != originalVisual);
  DALI_TEST_EQUALS(getOpacity(), 0.0f, TEST_LOCATION);

  // Completion from the discarded source cannot reveal the current visual.
  Ui::GetImplementation(originalVisual).ResourceReady(Ui::Visual::ResourceStatus::READY);
  firstManager.Refresh();
  DALI_TEST_EQUALS(getOpacity(), 0.0f, TEST_LOCATION);

  // Commit sampling/transform first, then reveal in the same manager refresh.
  Ui::GetImplementation(inlineVisual).ResourceReady(Ui::Visual::ResourceStatus::READY);
  firstManager.Refresh();
  DALI_TEST_EQUALS(getOpacity(), 1.0f, TEST_LOCATION);
  getTransform(inlineSize, inlineOffset);
  DALI_TEST_EQUALS(inlineSize, Vector2(24.0f, 18.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(inlineOffset, Vector2(4.0f, 6.0f), TEST_LOCATION);

  // Geometry/alignment-only updates reuse the existing runtime visual.
  const Ui::Integration::Visual::Base sourceChangedVisual = inlineVisual;
  placements[0u].position                                 = Vector2(13.0f, 9.0f);
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(100.0f, 40.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      5u));
  inlineVisual = firstViewData.GetVisual(firstVisualIndex);
  DALI_TEST_CHECK(inlineVisual == sourceChangedVisual);
  getTransform(inlineSize, inlineOffset);
  DALI_TEST_EQUALS(inlineOffset, Vector2(13.0f, 9.0f), TEST_LOCATION);

  // A new authored snapshot with the same occurrence and descriptor reuses
  // the existing visual while committing its new placement.
  source.sourceRevision      = 6u;
  placements[0u].position.x = 17.0f;
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(100.0f, 40.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      6u));
  inlineVisual = firstViewData.GetVisual(firstVisualIndex);
  DALI_TEST_CHECK(inlineVisual == sourceChangedVisual);
  getTransform(inlineSize, inlineOffset);
  DALI_TEST_EQUALS(inlineOffset, Vector2(17.0f, 9.0f), TEST_LOCATION);

  // The registered visual has no child-actor clip. Its quad and sampled pixel
  // area must be cropped explicitly to the content box.
  placements[0u].position = Vector2(-8.0f, 9.0f);
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(100.0f, 40.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      6u));
  inlineVisual = firstViewData.GetVisual(firstVisualIndex);
  getTransform(inlineSize, inlineOffset);
  DALI_TEST_EQUALS(inlineOffset, Vector2(0.0f, 9.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(inlineSize, Vector2(16.0f, 18.0f), TEST_LOCATION);
  const Vector4 clippedPixelArea = getPixelArea();
  DALI_TEST_EQUALS(clippedPixelArea.x, 1.0f / 3.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(clippedPixelArea.y, 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(clippedPixelArea.z, 2.0f / 3.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(clippedPixelArea.w, 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Decode-size identity changes recreate the visual while reusing the owner-local slot.
  source.runs[0u].metrics.width  = 30.0f;
  source.runs[0u].metrics.height = 20.0f;
  expectedDesiredWidth           = 30;
  expectedDesiredHeight          = 20;
  placements[0u].position        = Vector2(13.0f, 9.0f);
  placements[0u].size            = Vector2(30.0f, 20.0f);
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(100.0f, 40.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      6u));
  inlineVisual = firstViewData.GetVisual(firstVisualIndex);
  DALI_TEST_CHECK(inlineVisual != sourceChangedVisual);
  getTransform(inlineSize, inlineOffset);
  DALI_TEST_EQUALS(inlineSize, Vector2(30.0f, 20.0f), TEST_LOCATION);

  // Effective scale changes runtime sampling and final geometry without
  // changing the authored replacement metrics.
  expectedDesiredWidth  = 60;
  expectedDesiredHeight = 40;
  placements[0u].size   = Vector2(60.0f, 40.0f);
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(140.0f, 80.0f),
                                      Vector2(140.0f, 80.0f),
                                      2.0f,
                                      6u));
  inlineVisual = firstViewData.GetVisual(firstVisualIndex);
  getTransform(inlineSize, inlineOffset);
  DALI_TEST_EQUALS(inlineSize, Vector2(60.0f, 40.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(source.runs[0u].metrics.width, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(source.runs[0u].metrics.height, 20.0f, TEST_LOCATION);

  placements[0u].visible = false;
  placements[0u].elided  = true;
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(100.0f, 40.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      6u));
  DALI_TEST_CHECK(!firstViewData.GetVisual(firstVisualIndex));
  DALI_TEST_CHECK(secondViewData.GetVisual(secondVisualIndex));

  placements[0u].visible = true;
  placements[0u].elided  = false;
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(100.0f, 40.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      6u));
  DALI_TEST_CHECK(firstViewData.GetVisual(firstVisualIndex));

  // JSON would otherwise bypass the factory's static-only option and create
  // an AnimatedVectorImageVisual. v1 keeps the reserved box but no runtime visual.
  source.runs[0u].image.source = "animated-inline-manager-test.json";
  DALI_TEST_CHECK(firstManager.Update(firstHost,
                                      source,
                                      placements,
                                      Vector2::ZERO,
                                      Vector2(100.0f, 40.0f),
                                      Vector2(100.0f, 40.0f),
                                      1.0f,
                                      6u));
  DALI_TEST_CHECK(!firstViewData.GetVisual(firstVisualIndex));
  firstManager.Clear();
  DALI_TEST_CHECK(!firstViewData.GetVisual(firstVisualIndex));

  // Repeated create/clear cycles reuse the owner-local slot. Custom property
  // count reaches a high-water mark after the first slot and does not grow.
  source.runs[0u].image.source = "missing-inline-manager-high-water.png";
  const uint32_t propertyHighWater = firstOwner.GetPropertyCount();
  for(uint32_t cycle = 0u; cycle < 32u; ++cycle)
  {
    DALI_TEST_CHECK(firstManager.Update(firstHost,
                                       source,
                                       placements,
                                       Vector2::ZERO,
                                       Vector2(100.0f, 40.0f),
                                       Vector2(100.0f, 40.0f),
                                       1.0f,
                                       6u));
    Ui::Integration::Visual::Base discardedVisual = firstViewData.GetVisual(firstVisualIndex);
    DALI_TEST_CHECK(discardedVisual);
    firstManager.Clear();
    DALI_TEST_CHECK(!firstViewData.GetVisual(firstVisualIndex));
    Ui::GetImplementation(discardedVisual).ResourceReady(Ui::Visual::ResourceStatus::READY);
    firstManager.Refresh();
    DALI_TEST_CHECK(!firstViewData.GetVisual(firstVisualIndex));
    DALI_TEST_EQUALS(firstOwner.GetPropertyCount(), propertyHighWater, TEST_LOCATION);

    const Property::Index reusedIndex = firstHost.AllocateVisualSlot();
    DALI_TEST_EQUALS(reusedIndex, firstVisualIndex, TEST_LOCATION);
    firstHost.ReleaseVisualSlot(reusedIndex);
  }

  // Owner teardown and the later clear path must both be idempotent.
  secondManager.PrepareOwnerDestruction();
  secondManager.PrepareOwnerDestruction();
  DALI_TEST_CHECK(secondViewData.GetVisual(secondVisualIndex));
  secondOwner.Reset();
  Ui::GetImplementation(secondVisual).ResourceReady(Ui::Visual::ResourceStatus::READY);
  secondManager.Refresh();
  secondManager.Clear();
  secondManager.Clear();
  END_TEST;
}

int UtcDaliReplacementProjectionBidiLayoutsP(void)
{
  UiTestApplication                   application;
  Text::ReplacementLayoutTestServices services = MakeLayoutServices();

  struct BidiCase
  {
    const char*          text;
    Text::CharacterIndex start;
    Text::Length         length;
  };
  const BidiCase cases[] = {
    {"abICONcd", 2u, 4u},                                          // LTR + Latin replacement
    {"x\xE2\x80\x8F\xD7\x90\xD7\x91ICON\xD7\x92\xD7\x93", 4u, 4u}, // explicit RTL + Hebrew
    {"A\xE2\x80\x8F\xD7\x90\xD7\x91ICON\xD7\x92\xD7\x93"
     "B",
     4u, 4u}, // LTR paragraph around RTL
    {"x\xE2\x80\x8F\xD7\x90"
     "ABICONCD"
     "\xD7\x91",
     5u, 4u}, // RTL paragraph around LTR
    {"Aab\xD7\x90\xD7\x91"
     "cdB",
     1u, 6u},               // replacement underlying text contains mixed bidi classes
    {"A,(icon).B", 3u, 4u}, // neutral punctuation
  };

  for(uint32_t caseIndex = 0u; caseIndex < sizeof(cases) / sizeof(cases[0u]); ++caseIndex)
  {
    const BidiCase&                      testCase = cases[caseIndex];
    Vector<Text::Character>              text     = Utf32(testCase.text);
    Vector<Text::ReplacementRunSnapshot> candidates;
    candidates.PushBack(Candidate(testCase.start, testCase.length, 28.0f, 20.0f, caseIndex + 1u));
    Text::ReplacementProjection projection =
      Text::ReplacementProjection::Build(text, candidates);

    Text::ReplacementLayoutTestOptions options;
    options.contentSize = Vector2(400.0f, 80.0f);
    Text::ReplacementRenderState result;
    DALI_TEST_CHECK(Text::LayoutReplacementForTest(projection, services, options, result));
    DALI_TEST_EQUALS(result.placements.Count(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(result.placements[0u].visible);
    DALI_TEST_CHECK(std::isfinite(result.placements[0u].position.x));
    DALI_TEST_EQUALS(result.placements[0u].logicalCharacterRange.characterIndex, testCase.start, TEST_LOCATION);
    DALI_TEST_EQUALS(result.placements[0u].logicalCharacterRange.numberOfCharacters, testCase.length, TEST_LOCATION);
    const Text::CharacterIndex projectedReplacementIndex =
      projection.LogicalCharacterToProjected(result.placements[0u].logicalCharacterRange.characterIndex);
    DALI_TEST_EQUALS(projection.FindByProjectedCharacter(projectedReplacementIndex)->sourceRunIndex,
                     0u, TEST_LOCATION);
    // The internal UTC harness interposes a Latin-only script classifier and no-op bidi reorder. This test checks the
    // explicit replacement lookup and boundary/placement invariants across bidi-shaped inputs. Production Hebrew and
    // Bidi maps are asserted by the separate dali-replacement-real-layout-diagnostic executable.
    const bool                 rtl     = result.placements[0u].lineDirection;
    const Text::CharacterIndex leftHit = projection.HitTestLogicalBoundary(
      projectedReplacementIndex, 1.0f, 28.0f, rtl);
    const Text::CharacterIndex rightHit = projection.HitTestLogicalBoundary(
      projectedReplacementIndex, 27.0f, 28.0f, rtl);
    DALI_TEST_CHECK((leftHit == testCase.start || leftHit == testCase.start + testCase.length));
    DALI_TEST_CHECK((rightHit == testCase.start || rightHit == testCase.start + testCase.length));
    DALI_TEST_CHECK(leftHit != rightHit);
    CheckLaidOutBoundariesAreAtomic(projection, result);
  }

  // Multiple adjacent replacements in an RTL paragraph remain two visual layout units.
  Vector<Text::Character> adjacentText = Utf32(
    "x\xE2\x80\x8F\xD7\x90"
    "abcd"
    "\xD7\x91");
  Vector<Text::ReplacementRunSnapshot> adjacentCandidates;
  adjacentCandidates.PushBack(Candidate(3u, 2u, 18.0f, 18.0f, 70u));
  adjacentCandidates.PushBack(Candidate(5u, 2u, 19.0f, 18.0f, 71u));
  Text::ReplacementProjection adjacent =
    Text::ReplacementProjection::Build(adjacentText, adjacentCandidates);
  Text::ReplacementLayoutTestOptions adjacentOptions;
  adjacentOptions.contentSize = Vector2(300.0f, 80.0f);
  Text::ReplacementRenderState adjacentResult;
  DALI_TEST_CHECK(Text::LayoutReplacementForTest(adjacent, services, adjacentOptions, adjacentResult));
  DALI_TEST_EQUALS(adjacentResult.placements.Count(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(adjacentResult.placements[0u].visible);
  DALI_TEST_CHECK(adjacentResult.placements[1u].visible);
  DALI_TEST_CHECK(adjacentResult.placements[0u].syntheticGlyphIndex != adjacentResult.placements[1u].syntheticGlyphIndex);
  DALI_TEST_CHECK(adjacentResult.placements[0u].position.x != adjacentResult.placements[1u].position.x);
  CheckLaidOutBoundariesAreAtomic(adjacent, adjacentResult);

  END_TEST;
}

int UtcDaliReplacementProjectionEllipsisAtomicP(void)
{
  UiTestApplication                   application;
  Text::ReplacementLayoutTestServices services = MakeLayoutServices();

  struct EllipsisCase
  {
    const char*          text;
    Text::CharacterIndex firstReplacement;
  };
  const EllipsisCase cases[] = {
    {"A\uFFFCx\uFFFCx\uFFFCx\uFFFCx\uFFFCZ", 1u},
    {"x\u200Fא\uFFFCx\uFFFCx\uFFFCx\uFFFCx\uFFFCב", 3u},
    {"A\u200Fא\uFFFCx\uFFFCx\uFFFCx\uFFFCx\uFFFCבZ", 3u},
  };
  constexpr float replacementWidths[]  = {8.0f, 24.0f, 48.0f, 80.0f, 32.0f};
  constexpr float replacementHeights[] = {8.0f, 24.0f, 32.0f, 48.0f, 20.0f};

  // Replacement content deliberately supports only the END policy. Existing START/MIDDLE engine defects are outside
  // the acceptance result and must not be attributed to replacement projection.
  for(uint32_t caseIndex = 0u; caseIndex < sizeof(cases) / sizeof(cases[0u]); ++caseIndex)
  {
    const EllipsisCase&                  testCase = cases[caseIndex];
    Vector<Text::Character>              text     = Utf32(testCase.text);
    Vector<Text::ReplacementRunSnapshot> candidates;
    for(uint32_t replacementIndex = 0u; replacementIndex < 5u; ++replacementIndex)
    {
      candidates.PushBack(Candidate(testCase.firstReplacement + replacementIndex * 2u, 1u,
                                    replacementWidths[replacementIndex], replacementHeights[replacementIndex],
                                    400u + caseIndex * 10u + replacementIndex));
    }
    Text::ReplacementProjection projection =
      Text::ReplacementProjection::Build(text, candidates);

    Text::ReplacementLayoutTestOptions options;
    options.contentSize      = Vector2(62.0f, 40.0f);
    options.elideText        = true;
    options.ellipsisPosition = Text::EllipsisPosition::END;
    Text::ReplacementRenderState result;
    DALI_TEST_CHECK(Text::LayoutReplacementForTest(projection, services, options, result));
    DALI_TEST_CHECK(result.finalElision.textElided);
    DALI_TEST_EQUALS(result.placements.Count(), 5u, TEST_LOCATION);

    uint32_t elidedCount = 0u;
    for(const Text::ReplacementPlacement& placement : result.placements)
    {
      DALI_TEST_CHECK(placement.visible != placement.elided);
      elidedCount += placement.elided ? 1u : 0u;
      DALI_TEST_EQUALS(placement.logicalCharacterRange.numberOfCharacters, 1u, TEST_LOCATION);
      DALI_TEST_CHECK(IsExactLogicalBoundary(projection, placement.logicalCharacterRange.characterIndex));
      DALI_TEST_CHECK(IsExactLogicalBoundary(
        projection, placement.logicalCharacterRange.characterIndex + placement.logicalCharacterRange.numberOfCharacters));
    }
    DALI_TEST_CHECK(elidedCount > 0u);
    CheckLaidOutBoundariesAreAtomic(projection, result);
  }

  // Sweep across the threshold where END ellipsis lands directly on a wide
  // synthetic replacement. The replacement has fontId 0, but its non-zero
  // advance must be removed from the fit width and its nearest text font must
  // be used to replace that exact glyph with ellipsis. Otherwise the renderer
  // walks backward and leaves a replacement-sized blank before the dots.
  Vector<Text::Character>              boundaryText = Utf32("Long prefix words\uFFFCtrailing text");
  Vector<Text::ReplacementRunSnapshot> boundaryCandidates;
  boundaryCandidates.PushBack(Candidate(17u, 1u, 70.0f, 28.0f, 499u));
  Text::ReplacementProjection boundaryProjection =
    Text::ReplacementProjection::Build(boundaryText, boundaryCandidates);

  bool sawVisibleAtThreshold = false;
  bool sawElidedAtThreshold  = false;
  for(float width = 80.0f; width <= 360.0f; width += 2.0f)
  {
    Text::ReplacementLayoutTestOptions boundaryOptions;
    boundaryOptions.contentSize         = Vector2(width, 80.0f);
    boundaryOptions.lineWrapMode        = Text::LineWrapMode::CHARACTER;
    boundaryOptions.elideText           = true;
    boundaryOptions.ellipsisPosition    = Text::EllipsisPosition::END;
    boundaryOptions.horizontalAlignment = Text::Alignment::CENTER;

    Text::ReplacementRenderState boundaryResult;
    DALI_TEST_CHECK(Text::LayoutReplacementForTest(boundaryProjection,
                                                   services,
                                                   boundaryOptions,
                                                   boundaryResult));
    DALI_TEST_EQUALS(boundaryResult.placements.Count(), 1u, TEST_LOCATION);
    const Text::ReplacementPlacement& boundaryPlacement = boundaryResult.placements[0u];
    const Text::FinalElisionResult&   finalElision      = boundaryResult.finalElision;
    DALI_TEST_EQUALS(boundaryPlacement.visible,
                     finalElision.IsOriginalGlyphVisible(boundaryPlacement.syntheticGlyphIndex), TEST_LOCATION);
    DALI_TEST_CHECK(boundaryPlacement.visible != boundaryPlacement.elided);
    if(finalElision.textElided && boundaryPlacement.visible)
    {
      Text::GlyphIndex finalGlyphIndex = 0u;
      DALI_TEST_CHECK(finalElision.FindFinalGlyphIndex(boundaryPlacement.syntheticGlyphIndex,
                                                       finalGlyphIndex));
      DALI_TEST_EQUALS(FindSourceGlyphIndex(finalElision, finalGlyphIndex),
                       boundaryPlacement.syntheticGlyphIndex,
                       TEST_LOCATION);
      Vector2 finalGlyphPosition;
      DALI_TEST_CHECK(finalElision.GetFinalGlyphPosition(boundaryPlacement.syntheticGlyphIndex,
                                                         finalGlyphPosition));
      Text::FinalGlyphGeometry finalGeometry;
      DALI_TEST_CHECK(Text::GetFinalSourceGlyphGeometry(*boundaryResult.processingModel,
                                                        finalElision,
                                                        boundaryPlacement.syntheticGlyphIndex,
                                                        finalGeometry));
      DALI_TEST_EQUALS(finalGlyphPosition.x,
                       finalGeometry.contentLocalPenPosition.x,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(boundaryPlacement.position.x,
                       finalGeometry.contentLocalPenPosition.x,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
    }
    sawVisibleAtThreshold |= boundaryPlacement.visible;
    sawElidedAtThreshold |= finalElision.textElided && boundaryPlacement.elided;
    boundaryResult.Clear(services.bidirectionalSupport);
  }
  DALI_TEST_CHECK(sawVisibleAtThreshold);
  DALI_TEST_CHECK(sawElidedAtThreshold);

  END_TEST;
}

namespace
{
constexpr uint32_t FINAL_EQUIVALENCE_SHARD_COUNT = 4u;

void CheckEndEllipsisPlannerFinalEquivalence(uint32_t shardIndex)
{
  DALI_TEST_CHECK(shardIndex < FINAL_EQUIVALENCE_SHARD_COUNT);

  UiTestApplication                   application;
  Text::ReplacementLayoutTestServices services = MakeLayoutServices();

  struct PlannerCase
  {
    const char*                 text;
    std::vector<Vector2>        sizes;
    Dali::LayoutDirection::Type direction;
    bool                        multiline;
    float                       characterSpacing;
    Text::Alignment             horizontalAlignment{Text::Alignment::START};
    Text::Alignment             verticalAlignment{Text::Alignment::START};
    float                       lineSpacing{0.0f};
    float                       relativeLineSize{1.0f};
  };
  const PlannerCase cases[] = {
    {"ordinary prefix \uFFFC followed by trailing words", {Vector2(24.0f, 24.0f)}, Dali::LayoutDirection::LEFT_TO_RIGHT, false, 0.0f},
    {"adjacent \uFFFC\uFFFC replacements followed by trailing words", {Vector2(8.0f, 40.0f), Vector2(16.0f, 70.0f)}, Dali::LayoutDirection::LEFT_TO_RIGHT, false, 4.0f},
    {"small \uFFFC then oversized \uFFFC then small \uFFFC and trailing words",
     {Vector2(8.0f, 8.0f), Vector2(210.0f, 120.0f), Vector2(16.0f, 16.0f)},
     Dali::LayoutDirection::LEFT_TO_RIGHT,
     false,
     -2.0f},
    {"אבג mixed LTR \uFFFC ثم العربية \uFFFC trailing words",
     {Vector2(40.0f, 30.0f), Vector2(80.0f, 48.0f)},
     Dali::LayoutDirection::RIGHT_TO_LEFT,
     false,
     3.0f},
    {"LTR אבגדה \uFFFC العربية Latin tail \uFFFC סוף",
     {Vector2(34.0f, 22.0f), Vector2(52.0f, 36.0f)},
     Dali::LayoutDirection::LEFT_TO_RIGHT,
     false,
     -1.0f},
    {"multiline prefix with a small \uFFFC image and enough ordinary text before an oversized \uFFFC image "
     "followed by several wrapping lines and one final \uFFFC replacement",
     {Vector2(24.0f, 24.0f), Vector2(210.0f, 120.0f), Vector2(40.0f, 40.0f)},
     Dali::LayoutDirection::LEFT_TO_RIGHT,
     true,
     2.0f},
  };

  uint32_t comparedCount         = 0u;
  uint32_t verticalFallbackCount = 0u;
  uint32_t splitBidiCount        = 0u;
  for(const PlannerCase& testCase : cases)
  {
    Vector<Text::Character>              text = Utf32(testCase.text);
    Vector<Text::ReplacementRunSnapshot> candidates;
    uint32_t                             replacementIndex = 0u;
    for(Text::CharacterIndex characterIndex = 0u; characterIndex < text.Count(); ++characterIndex)
    {
      if(text[characterIndex] == Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER)
      {
        DALI_TEST_CHECK(replacementIndex < testCase.sizes.size());
        candidates.PushBack(Candidate(characterIndex,
                                      1u,
                                      testCase.sizes[replacementIndex].width,
                                      testCase.sizes[replacementIndex].height,
                                      9000u + replacementIndex));
        ++replacementIndex;
      }
    }
    DALI_TEST_EQUALS(replacementIndex, static_cast<uint32_t>(testCase.sizes.size()), TEST_LOCATION);
    const Text::ReplacementProjection projection = Text::ReplacementProjection::Build(text, candidates);
    DALI_TEST_CHECK(projection.HasReplacements());

    const float minimumWidth = 20.0f;
    const float maximumWidth = testCase.multiline ? 260.0f : 500.0f;
    uint32_t    widthIndex   = 0u;
    for(float width = minimumWidth; width <= maximumWidth; width += 2.0f)
    {
      if((widthIndex++ % FINAL_EQUIVALENCE_SHARD_COUNT) != shardIndex)
      {
        continue;
      }

      const float minimumHeight = testCase.multiline ? 24.0f : 160.0f;
      const float maximumHeight = testCase.multiline ? 220.0f : minimumHeight;
      for(float height = minimumHeight; height <= maximumHeight; height += testCase.multiline ? 2.0f : 400.0f)
      {
        Text::ReplacementLayoutTestOptions options;
        options.contentSize         = Size(width, height);
        options.layoutType          = testCase.multiline ? Text::Layout::Engine::MULTI_LINE_BOX
                                                         : Text::Layout::Engine::SINGLE_LINE_BOX;
        options.lineWrapMode        = Text::LineWrapMode::CHARACTER;
        options.elideText           = true;
        options.ellipsisPosition    = Text::EllipsisPosition::END;
        options.layoutDirection     = testCase.direction;
        options.fontPointSize       = 20u * 64u;
        options.fontPixelSize       = 20.0f * 4.0f / 3.0f;
        options.characterSpacing    = testCase.characterSpacing;
        options.horizontalAlignment = testCase.horizontalAlignment;
        options.verticalAlignment   = testCase.verticalAlignment;
        options.defaultLineSpacing  = testCase.lineSpacing;
        options.relativeLineSize    = testCase.relativeLineSize;

        const auto compareModel = [&](const Text::Model& model, const Text::FinalElisionResult& finalElision)
        {
          if(!finalElision.textElided || finalElision.ellipsisLineIndex >= model.mVisualModel->mLines.Count())
          {
            return;
          }

          const Text::LineRun&        line = model.mVisualModel->mLines[finalElision.ellipsisLineIndex];
          const Text::EndEllipsisPlan planner =
            ResolveFinalHostLinePlan(model, line, services.fontClient);
          const std::vector<Text::GlyphIndex> actual =
            FindActuallyRemovedHostLineReplacements(model, finalElision, line);
          const Text::GlyphIndex invalid = Text::EndEllipsisPlan::INVALID_GLYPH_INDEX;
          ++comparedCount;
          splitBidiCount += line.isSplitToTwoHalves ? 1u : 0u;
          if(actual.empty())
          {
            DALI_TEST_EQUALS(planner.firstRemovedReplacementGlyphIndex, invalid, TEST_LOCATION);
            DALI_TEST_EQUALS(planner.lastRemovedReplacementGlyphIndex, invalid, TEST_LOCATION);
          }
          else
          {
            DALI_TEST_EQUALS(planner.firstRemovedReplacementGlyphIndex, actual.front(), TEST_LOCATION);
            DALI_TEST_EQUALS(planner.lastRemovedReplacementGlyphIndex, actual.back(), TEST_LOCATION);
            for(const Text::GlyphIndex glyphIndex : actual)
            {
              DALI_TEST_CHECK(glyphIndex >= planner.firstRemovedReplacementGlyphIndex);
              DALI_TEST_CHECK(glyphIndex <= planner.lastRemovedReplacementGlyphIndex);
            }
          }

          const bool usesVerticalFallback =
            model.mVisualModel->mLines.Count() == 1u &&
            Text::GetLineHeight(line, true) > model.mVisualModel->mControlSize.height;
          verticalFallbackCount += usesVerticalFallback ? 1u : 0u;
          if(!usesVerticalFallback)
          {
            DALI_TEST_EQUALS(planner.resolved, finalElision.applied, TEST_LOCATION);
            if(planner.resolved)
            {
              DALI_TEST_CHECK(finalElision.ellipsisFinalGlyphIndex < finalElision.glyphs.Count());
              DALI_TEST_EQUALS(planner.ellipsisGlyphIndex, finalElision.endIndex, TEST_LOCATION);
              DALI_TEST_EQUALS(planner.ellipsisGlyph.index,
                               finalElision.glyphs[finalElision.ellipsisFinalGlyphIndex].index,
                               TEST_LOCATION);
              DALI_TEST_EQUALS(planner.ellipsisGlyph.fontId,
                               finalElision.glyphs[finalElision.ellipsisFinalGlyphIndex].fontId,
                               TEST_LOCATION);
              DALI_TEST_EQUALS(planner.ellipsisPosition.x,
                               finalElision.viewGlyphPositions[finalElision.ellipsisFinalGlyphIndex].x,
                               Math::MACHINE_EPSILON_1000,
                               TEST_LOCATION);
            }
          }
        };

        Text::ReplacementRenderState state;
        DALI_TEST_CHECK(Text::LayoutReplacementForTest(projection, services, options, state));
        compareModel(*state.processingModel, state.finalElision);
        state.Clear(services.bidirectionalSupport);
      }
    }
  }

  DALI_TEST_CHECK(comparedCount > 0u);
  // END ellipsis never uses LineRun::isSplitToTwoHalves. That representation
  // is reserved for single-line MIDDLE ellipsis by LayoutEngine.
  DALI_TEST_EQUALS(splitBidiCount, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(verticalFallbackCount > 0u);
}
} // unnamed namespace

int UtcDaliEndEllipsisPlannerFinalEquivalence01P(void)
{
  CheckEndEllipsisPlannerFinalEquivalence(0u);
  END_TEST;
}

int UtcDaliEndEllipsisPlannerFinalEquivalence02P(void)
{
  CheckEndEllipsisPlannerFinalEquivalence(1u);
  END_TEST;
}

int UtcDaliEndEllipsisPlannerFinalEquivalence03P(void)
{
  CheckEndEllipsisPlannerFinalEquivalence(2u);
  END_TEST;
}

int UtcDaliEndEllipsisPlannerFinalEquivalence04P(void)
{
  CheckEndEllipsisPlannerFinalEquivalence(3u);
  END_TEST;
}

int UtcDaliEndEllipsisPlannerInvariantP(void)
{
  Vector<Text::GlyphInfo>      glyphs;
  Vector<Vector2>              positions;
  Vector<Text::Character>      text;
  Vector<Text::CharacterIndex> glyphToCharacterMap;
  glyphs.Resize(4u);
  positions.Resize(4u);
  text.Resize(4u);
  glyphToCharacterMap.Resize(4u);
  for(Text::GlyphIndex index = 0u; index < glyphs.Count(); ++index)
  {
    glyphs[index].fontId       = 1u;
    glyphs[index].index        = 100u + index;
    glyphs[index].width        = 10.0f;
    glyphs[index].height       = 12.0f;
    glyphs[index].xBearing     = 0.0f;
    glyphs[index].yBearing     = 9.0f;
    glyphs[index].advance      = 10.0f;
    positions[index]           = Vector2(10.0f * index, 0.0f);
    text[index]                = static_cast<Text::Character>('a' + index);
    glyphToCharacterMap[index] = index;
  }

  const Vector<Text::GlyphInfo>          glyphSnapshot    = glyphs;
  const Vector<Vector2>                  positionSnapshot = positions;
  Vector<Text::CharacterSpacingGlyphRun> characterSpacingRuns;
  FixedEllipsisMetrics                   fixed;
  fixed.glyph.fontId   = 2u;
  fixed.glyph.index    = 500u;
  fixed.glyph.width    = 15.0f;
  fixed.glyph.height   = 12.0f;
  fixed.glyph.xBearing = 0.0f;
  fixed.glyph.yBearing = 9.0f;
  fixed.glyph.advance  = 15.0f;

  Text::EndEllipsisInputView input;
  input.glyphs                  = glyphs.Begin();
  input.glyphPositions          = positions.Begin();
  input.text                    = text.Begin();
  input.glyphToCharacterMap     = glyphToCharacterMap.Begin();
  input.characterSpacingRuns    = &characterSpacingRuns;
  input.numberOfGlyphs          = glyphs.Count();
  input.glyphPositionStartIndex = 0u;
  input.numberOfGlyphPositions  = positions.Count();
  input.numberOfCharacters      = text.Count();
  input.startIndex              = 3u;
  input.lineWidth               = 40.0f;
  input.metricsContext          = &fixed;
  input.resolveMetrics          = ResolveFixedEllipsisMetrics;

  const Text::EndEllipsisPlan first  = Text::ResolveEndEllipsisPlan(input);
  const Text::EndEllipsisPlan second = Text::ResolveEndEllipsisPlan(input);
  DALI_TEST_CHECK(first.resolved);
  DALI_TEST_EQUALS(first.ellipsisGlyphIndex, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(first.numberOfRemovedGlyphs, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(first.ellipsisGlyph.index, fixed.glyph.index, TEST_LOCATION);
  DALI_TEST_EQUALS(first.ellipsisGlyph.fontId, fixed.glyph.fontId, TEST_LOCATION);
  DALI_TEST_EQUALS(first.ellipsisPosition, Vector2(20.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(second.resolved, first.resolved, TEST_LOCATION);
  DALI_TEST_EQUALS(second.ellipsisGlyphIndex, first.ellipsisGlyphIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(second.numberOfRemovedGlyphs, first.numberOfRemovedGlyphs, TEST_LOCATION);
  DALI_TEST_EQUALS(second.ellipsisPosition, first.ellipsisPosition, TEST_LOCATION);
  DALI_TEST_CHECK(fixed.resolveCount > 0u);

  for(Text::GlyphIndex index = 0u; index < glyphs.Count(); ++index)
  {
    DALI_TEST_EQUALS(glyphs[index].index, glyphSnapshot[index].index, TEST_LOCATION);
    DALI_TEST_EQUALS(glyphs[index].fontId, glyphSnapshot[index].fontId, TEST_LOCATION);
    DALI_TEST_EQUALS(glyphs[index].width, glyphSnapshot[index].width, TEST_LOCATION);
    DALI_TEST_EQUALS(glyphs[index].advance, glyphSnapshot[index].advance, TEST_LOCATION);
    DALI_TEST_EQUALS(positions[index], positionSnapshot[index], TEST_LOCATION);
  }

  Vector<Vector2> mixedPositions = positions;
  mixedPositions[1u].x           = 85.0f;
  mixedPositions[2u].x           = 80.0f;
  mixedPositions[3u].x           = 30.0f;
  FixedEllipsisMetrics mixedMetrics;
  mixedMetrics.glyph                      = fixed.glyph;
  mixedMetrics.glyph.xBearing             = 2.0f;
  input.glyphPositions                    = mixedPositions.Begin();
  input.metricsContext                    = &mixedMetrics;
  const Text::EndEllipsisPlan mixedRewind = Text::ResolveEndEllipsisPlan(input);
  DALI_TEST_CHECK(mixedRewind.resolved);
  DALI_TEST_EQUALS(mixedRewind.ellipsisGlyphIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(mixedRewind.numberOfRemovedGlyphs, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(mixedRewind.ellipsisPosition, Vector2(81.0f, 0.0f), TEST_LOCATION);

  input.glyphPositions = positions.Begin();
  input.metricsContext = &fixed;

  glyphs[3u].fontId                       = 0u;
  glyphs[3u].index                        = Text::SYNTHETIC_REPLACEMENT_GLYPH_ID;
  glyphs[3u].width                        = 30.0f;
  glyphs[3u].advance                      = 30.0f;
  const Text::EndEllipsisPlan replacement = Text::ResolveEndEllipsisPlan(input);
  DALI_TEST_CHECK(replacement.resolved);
  DALI_TEST_EQUALS(replacement.ellipsisGlyphIndex, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(replacement.numberOfRemovedGlyphs, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(replacement.firstRemovedReplacementGlyphIndex, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(replacement.lastRemovedReplacementGlyphIndex, 3u, TEST_LOCATION);

  glyphs                         = glyphSnapshot;
  positions                      = positionSnapshot;
  glyphs[2u].fontId              = 0u;
  glyphs[2u].index               = Text::SYNTHETIC_REPLACEMENT_GLYPH_ID;
  glyphs[2u].width               = 5.0f;
  glyphs[2u].advance             = 5.0f;
  glyphs[3u].fontId              = 0u;
  glyphs[3u].index               = Text::SYNTHETIC_REPLACEMENT_GLYPH_ID;
  glyphs[3u].width               = 5.0f;
  glyphs[3u].advance             = 5.0f;
  positions[3u].x               = 25.0f;
  input.glyphs                   = glyphs.Begin();
  input.glyphPositions           = positions.Begin();
  const Text::EndEllipsisPlan multipleReplacements = Text::ResolveEndEllipsisPlan(input);
  DALI_TEST_CHECK(multipleReplacements.resolved);
  DALI_TEST_EQUALS(multipleReplacements.ellipsisGlyphIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleReplacements.firstRemovedReplacementGlyphIndex, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleReplacements.lastRemovedReplacementGlyphIndex, 3u, TEST_LOCATION);

  for(Text::GlyphInfo& glyph : glyphs)
  {
    glyph.fontId = 0u;
    glyph.index  = 1u;
  }
  const Text::EndEllipsisPlan missingMetrics = Text::ResolveEndEllipsisPlan(input);
  DALI_TEST_CHECK(!missingMetrics.resolved);
  DALI_TEST_EQUALS(missingMetrics.ellipsisGlyphIndex,
                   Text::EndEllipsisPlan::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(missingMetrics.firstRemovedReplacementGlyphIndex,
                   Text::EndEllipsisPlan::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(missingMetrics.lastRemovedReplacementGlyphIndex,
                   Text::EndEllipsisPlan::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(missingMetrics.numberOfRemovedGlyphs, glyphs.Count(), TEST_LOCATION);

  Text::EndEllipsisInputView  invalidInput;
  const Text::EndEllipsisPlan invalid = Text::ResolveEndEllipsisPlan(invalidInput);
  DALI_TEST_CHECK(!invalid.resolved);
  DALI_TEST_EQUALS(invalid.ellipsisGlyphIndex,
                   Text::EndEllipsisPlan::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliReplacementVerticalEndEllipsisLifecycleP(void)
{
  UiTestApplication                   application;
  Text::ReplacementLayoutTestServices services = MakeLayoutServices();

  const std::string utf8 =
    "An oversized replacement follows wrapped introductory prose and tests vertical END ellipsis. "
    "More words place \uFFFC near a constrained line before many trailing sentences continue. "
    "The large reserved box must be fully visible only when its whole line participates in the visible layout. "
    "Otherwise the renderer must choose a text ellipsis boundary without flashing, cropping or retaining the large image. "
    "Repeated trailing words add stable overflow for wide and narrow resize verification.";
  Vector<Text::Character> text             = Utf32(utf8);
  Text::CharacterIndex    replacementIndex = 0u;
  while(replacementIndex < text.Count() && text[replacementIndex] != Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER)
  {
    ++replacementIndex;
  }
  DALI_TEST_CHECK(replacementIndex < text.Count());

  Vector<Text::ReplacementRunSnapshot> candidates;
  candidates.PushBack(Candidate(replacementIndex, 1u, 210.0f, 120.0f, 2700u));
  const Text::ReplacementProjection projection = Text::ReplacementProjection::Build(text, candidates);
  DALI_TEST_CHECK(projection.HasReplacements());

  auto layout = [&](float width, float height, uint64_t generation)
  {
    Text::ReplacementLayoutTestOptions options;
    options.contentSize      = Size(width, height);
    options.layoutType       = Text::Layout::Engine::MULTI_LINE_BOX;
    options.lineWrapMode     = Text::LineWrapMode::WORD;
    options.elideText        = true;
    options.ellipsisPosition = Text::EllipsisPosition::END;
    options.fontPointSize    = 28u * 64u;
    options.fontPixelSize    = 28.0f * 4.0f / 3.0f;
    options.sourceRevision   = 270u;
    options.layoutGeneration = generation;
    Text::ReplacementRenderState result;
    DALI_TEST_CHECK(Text::LayoutReplacementForTest(projection, services, options, result));
    DALI_TEST_EQUALS(result.placements.Count(), 1u, TEST_LOCATION);
    CheckFinalElisionContract(result);
    return result;
  };

  // The sample's vertical threshold is deterministic: more height can reveal
  // additional END-prefix glyphs and the image, but cannot hide either again.
  bool     sawImageVisible           = false;
  bool     sawImageElided            = false;
  uint32_t previousVisibleGlyphCount = 0u;
  bool     previousImageVisible      = false;
  uint64_t generation                = 1u;
  for(float height = 80.0f; height <= 320.0f; height += 2.0f)
  {
    Text::ReplacementRenderState    result            = layout(400.0f, height, generation++);
    const Text::FinalElisionResult& finalElision      = result.finalElision;
    const uint32_t                  visibleGlyphCount = finalElision.textElided
                                                          ? CountVisibleOriginalGlyphs(finalElision)
                                                          : result.processingModel->mVisualModel->mGlyphs.Count();
    DALI_TEST_CHECK(visibleGlyphCount >= previousVisibleGlyphCount);
    DALI_TEST_CHECK(!previousImageVisible || result.placements[0u].visible);
    previousVisibleGlyphCount = visibleGlyphCount;
    previousImageVisible      = result.placements[0u].visible;
    sawImageVisible |= result.placements[0u].visible;
    sawImageElided |= result.placements[0u].elided;
    result.Clear(services.bidirectionalSupport);
  }
  DALI_TEST_CHECK(sawImageVisible);
  DALI_TEST_CHECK(sawImageElided);

  // Width changes exercise line-count thresholds and must retain a single
  // drawable ellipsis unit even when horizontal and vertical pressure meet.
  // Visible text count may move at word-wrap thresholds, but a visible image
  // cannot become hidden again as width increases.
  uint32_t previousLineCount         = 0u;
  bool     previousWidthImageVisible = false;
  bool     sawLineCountChange        = false;
  for(float width = 120.0f; width <= 600.0f; width += 2.0f)
  {
    Text::ReplacementRenderState result    = layout(width, 367.0f, generation++);
    const uint32_t               lineCount = result.processingModel->mVisualModel->mLines.Count();
    DALI_TEST_CHECK(!previousWidthImageVisible || result.placements[0u].visible);
    sawLineCountChange |= previousLineCount != 0u && previousLineCount != lineCount;
    previousWidthImageVisible = result.placements[0u].visible;
    previousLineCount         = lineCount;
    result.Clear(services.bidirectionalSupport);
  }
  DALI_TEST_CHECK(sawLineCountChange);

  // Reuse one controller across narrow/wide/narrow generations. The second
  // resolve of an unchanged generation is an idempotent cache hit, and the
  // renderer-facing ViewModel consumes that same immutable sequence.
  Text::ReplacementSourceSnapshot source;
  source.runs                        = candidates;
  source.sourceRevision              = 271u;
  source.hasValidReplacementSource   = true;
  Text::ControllerPtr     controller = Text::Controller::New();
  Text::Controller::Impl& impl       = Text::Controller::Impl::GetImplementation(*controller.Get());
  controller->SetText(utf8);
  controller->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetLineWrapMode(Text::LineWrapMode::WORD);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  impl.GetOrCreateReplacementSourceSnapshot() = source;

  controller->Relayout(Size(400.0f, 90.0f));
  const uint64_t narrowGeneration = impl.GetReplacementRenderState().finalElision.layoutGeneration;
  const bool     narrowVisible    = impl.GetReplacementRenderState().placements[0u].visible;
  CheckFinalElisionContract(impl.GetReplacementRenderState());

  controller->Relayout(Size(400.0f, 320.0f));
  DALI_TEST_CHECK(
    impl.GetReplacementRenderState().finalElision.layoutGeneration > narrowGeneration);
  DALI_TEST_CHECK(!narrowVisible && impl.GetReplacementRenderState().placements[0u].visible);
  CheckFinalElisionContract(impl.GetReplacementRenderState());

  Text::FinalElisionResult& repeatedResult     = impl.GetOrCreateReplacementRenderState().finalElision;
  const uint64_t            repeatedGeneration = repeatedResult.layoutGeneration;
  const uint32_t            repeatedGlyphCount = repeatedResult.glyphs.Count();
  const Text::GlyphInfo*    repeatedGlyphData  = repeatedResult.glyphs.Begin();
  impl.mView.ResolveFinalElision(impl.GetFontClient(), repeatedResult, repeatedGeneration);
  DALI_TEST_EQUALS(repeatedResult.glyphs.Count(), repeatedGlyphCount, TEST_LOCATION);
  DALI_TEST_CHECK(repeatedResult.glyphs.Begin() == repeatedGlyphData);

  Text::ViewModel rendererView(impl.GetReplacementRenderState().processingModel.Get());
  rendererView.SetFinalElisionResult(&repeatedResult);
  rendererView.ElideGlyphs(impl.GetFontClient());
  const uint32_t expectedRendererGlyphCount = repeatedResult.textElided
                                                ? repeatedResult.glyphs.Count()
                                                : impl.GetReplacementRenderState().processingModel->mVisualModel->mGlyphs.Count();
  DALI_TEST_EQUALS(rendererView.GetNumberOfGlyphs(), expectedRendererGlyphCount, TEST_LOCATION);

  controller->Relayout(Size(400.0f, 90.0f));
  DALI_TEST_CHECK(
    impl.GetReplacementRenderState().finalElision.layoutGeneration > repeatedGeneration);
  DALI_TEST_EQUALS(impl.GetReplacementRenderState().placements[0u].visible, narrowVisible, TEST_LOCATION);
  CheckFinalElisionContract(impl.GetReplacementRenderState());

  // Async renderScale may execute several measurement layouts, but the final
  // render owns exactly one result and has the same logical image decision.
  struct AsyncSummary
  {
    bool            visible{false};
    bool            textElided{false};
    bool            ellipsisApplied{false};
    uint32_t        ellipsisUnits{0u};
    uint32_t        lineCount{0u};
    Text::LineIndex ellipsisLineIndex{Text::FinalElisionResult::INVALID_LINE_INDEX};
  };
  auto renderAsync = [&](float renderScale, const Size& size, uint64_t requestGeneration)
  {
    uint32_t horizontalDpi = 0u;
    uint32_t verticalDpi   = 0u;
    TextAbstraction::FontClient::Get().GetDpi(horizontalDpi, verticalDpi);
    Text::AsyncTextParameters parameters;
    parameters.text                        = utf8;
    parameters.fontSize                    = 18.0f * 72.0f / static_cast<float>(horizontalDpi);
    parameters.textWidth                   = size.width;
    parameters.textHeight                  = size.height;
    parameters.isMultiLine                 = true;
    parameters.lineWrapMode                = Text::LineWrapMode::WORD;
    parameters.ellipsis                    = true;
    parameters.ellipsisPosition            = Text::EllipsisPosition::END;
    parameters.renderScale                 = renderScale;
    parameters.replacementSourceSnapshot   = source;
    parameters.replacementLayoutGeneration = requestGeneration;

    Text::AsyncTextLoader loader            = Text::AsyncTextLoader::New();
    bool                  cachedNaturalSize = false;
    Size                  naturalSize       = Size::ZERO;
    if(renderScale > 1.0f)
    {
      naturalSize = loader.SetupRenderScale(parameters, cachedNaturalSize);
    }
    const Text::AsyncTextRenderInfo renderInfo = loader.RenderText(parameters, cachedNaturalSize, naturalSize);
    const Text::ReplacementRenderState* state = Text::GetImplementation(loader).GetReplacementRenderState();
    DALI_TEST_CHECK(state);
    CheckFinalElisionContract(*state);
    DALI_TEST_EQUALS(renderInfo.replacementPlacements.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(renderInfo.replacementPlacements[0u].visible, state->placements[0u].visible, TEST_LOCATION);
    const Text::FinalElisionResult& finalElision = state->finalElision;
    return AsyncSummary{state->placements[0u].visible,
                        finalElision.textElided,
                        finalElision.applied,
                        finalElision.ellipsisUnitCount,
                        static_cast<uint32_t>(state->processingModel->mVisualModel->mLines.Count()),
                        finalElision.ellipsisLineIndex};
  };

  const AsyncSummary scaleOneNarrow = renderAsync(1.0f, Size(400.0f, 90.0f), 500u);
  const AsyncSummary scaleTwoNarrow = renderAsync(2.0f, Size(400.0f, 90.0f), 501u);
  DALI_TEST_EQUALS(scaleTwoNarrow.visible, scaleOneNarrow.visible, TEST_LOCATION);
  DALI_TEST_EQUALS(scaleTwoNarrow.textElided, scaleOneNarrow.textElided, TEST_LOCATION);
  DALI_TEST_EQUALS(scaleTwoNarrow.ellipsisApplied, scaleOneNarrow.ellipsisApplied, TEST_LOCATION);
  DALI_TEST_EQUALS(scaleTwoNarrow.ellipsisUnits, scaleOneNarrow.ellipsisUnits, TEST_LOCATION);
  DALI_TEST_EQUALS(scaleTwoNarrow.ellipsisLineIndex, scaleOneNarrow.ellipsisLineIndex, TEST_LOCATION);
  const AsyncSummary scaleOneWide = renderAsync(1.0f, Size(400.0f, 320.0f), 502u);
  const AsyncSummary scaleTwoWide = renderAsync(2.0f, Size(400.0f, 320.0f), 503u);
  DALI_TEST_EQUALS(scaleTwoWide.visible, scaleOneWide.visible, TEST_LOCATION);
  DALI_TEST_EQUALS(scaleTwoWide.textElided, scaleOneWide.textElided, TEST_LOCATION);
  DALI_TEST_EQUALS(scaleTwoWide.ellipsisApplied, scaleOneWide.ellipsisApplied, TEST_LOCATION);
  DALI_TEST_EQUALS(scaleTwoWide.ellipsisUnits, scaleOneWide.ellipsisUnits, TEST_LOCATION);
  DALI_TEST_EQUALS(scaleTwoWide.ellipsisLineIndex, scaleOneWide.ellipsisLineIndex, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrdinaryTextSkipsReplacementStateP(void)
{
  UiTestApplication application;

  Text::ControllerPtr     controller = Text::Controller::New();
  Text::Controller::Impl& impl       = Text::Controller::Impl::GetImplementation(*controller.Get());
  DALI_TEST_CHECK(!impl.HasReplacementData());
  DALI_TEST_CHECK(!controller->HasValidReplacementSource());
  DALI_TEST_CHECK(controller->GetFinalElisionResult() == nullptr);

  controller->SetText("Ordinary multiline text remains on the legacy layout and ellipsis path without replacement state");
  controller->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetLineWrapMode(Text::LineWrapMode::WORD);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(100.0f, 35.0f));

  DALI_TEST_CHECK(controller->GetRenderTextModel() == controller->GetLogicalTextModel());
  DALI_TEST_CHECK(!impl.HasReplacementData());
  DALI_TEST_CHECK(!controller->HasValidReplacementSource());
  DALI_TEST_CHECK(controller->GetFinalElisionResult() == nullptr);

  Text::AsyncTextParameters parameters;
  parameters.text             = "Ordinary async text also skips replacement projection and final-elision storage";
  parameters.textWidth        = 100.0f;
  parameters.textHeight       = 35.0f;
  parameters.isMultiLine      = true;
  parameters.lineWrapMode     = Text::LineWrapMode::WORD;
  parameters.ellipsis         = true;
  parameters.ellipsisPosition = Text::EllipsisPosition::END;
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  loader.RenderText(parameters, false, Size::ZERO);
  DALI_TEST_CHECK(Text::GetImplementation(loader).GetReplacementRenderState() == nullptr);

  END_TEST;
}

int UtcDaliReplacementUnsupportedEllipsisPoliciesP(void)
{
  UiTestApplication application;

  Text::ReplacementSourceSnapshot source;
  source.runs.PushBack(Candidate(1u, 1u, 28.0f, 20.0f, 1u));
  source.sourceRevision            = 71u;
  source.hasValidReplacementSource = true;

  const Text::EllipsisPosition::Type unsupported[] = {
    Text::EllipsisPosition::START,
    Text::EllipsisPosition::MIDDLE};
  for(const Text::EllipsisPosition::Type position : unsupported)
  {
    Text::ControllerPtr     controller = Text::Controller::New();
    Text::Controller::Impl& impl       = Text::Controller::Impl::GetImplementation(*controller.Get());
    controller->SetText("A\uFFFCB");
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(position);
    impl.GetOrCreateReplacementSourceSnapshot() = source;
    controller->Relayout(Size(32.0f, 40.0f));
    DALI_TEST_CHECK(impl.GetReplacementRenderState().attempted);
    DALI_TEST_CHECK(impl.GetReplacementRenderState().processingModel);
    DALI_TEST_CHECK(controller->GetLogicalTextModel() != controller->GetRenderTextModel());
    DALI_TEST_CHECK(!impl.GetReplacementRenderState().processingModel->mElideEnabled);
    DALI_TEST_CHECK(!impl.GetReplacementRenderState().finalElision.textElided);
    DALI_TEST_EQUALS(impl.GetReplacementRenderState().placements.Count(), 1u, TEST_LOCATION);

    Text::AsyncTextParameters parameters;
    parameters.text                        = "A\uFFFCB";
    parameters.textWidth                   = 32.0f;
    parameters.textHeight                  = 40.0f;
    parameters.ellipsis                    = true;
    parameters.ellipsisPosition            = position;
    parameters.replacementSourceSnapshot   = source;
    parameters.replacementLayoutGeneration = 9u;
    Text::AsyncTextLoader loader           = Text::AsyncTextLoader::New();
    loader.GetHeightForWidth(parameters);
    const Text::ReplacementRenderState* async = Text::GetImplementation(loader).GetReplacementRenderState();
    DALI_TEST_CHECK(async);
    DALI_TEST_CHECK(async->attempted);
    DALI_TEST_CHECK(async->processingModel);
    DALI_TEST_CHECK(!async->processingModel->mElideEnabled);
    DALI_TEST_CHECK(!async->finalElision.textElided);
    DALI_TEST_EQUALS(async->placements.Count(), 1u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliReplacementProductionSyncAsyncParityP(void)
{
  UiTestApplication application;

  Text::ReplacementSourceSnapshot source;
  source.runs.PushBack(Candidate(0u, 2u, 28.0f, 20.0f, 700u));
  source.sourceRevision            = 41u;
  source.hasValidReplacementSource = true;

  Text::ControllerPtr     controller     = Text::Controller::New();
  Text::Controller::Impl& controllerImpl = Text::Controller::Impl::GetImplementation(*controller.Get());
  controller->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  controller->SetRelativeLineSize(1.0f);
  controller->SetUiScale(1.5f);
  controller->SetText("ab");
  DALI_TEST_CHECK(!controllerImpl.mReplacementData);
  controllerImpl.GetOrCreateReplacementSourceSnapshot() = source;
  DALI_TEST_CHECK(controllerImpl.mReplacementData);
  controller->Relayout(Size(160.0f, 60.0f));

  const Text::ReplacementRenderState& sync = controllerImpl.GetReplacementRenderState();
  DALI_TEST_CHECK(sync.attempted);
  DALI_TEST_EQUALS(sync.sourceRevision, 41u, TEST_LOCATION);
  DALI_TEST_EQUALS(sync.projection.GetReplacementRuns().Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSyntheticGlyphs(sync), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(sync.placements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(sync.placements[0u].size, Vector2(42.0f, 30.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(source.runs[0u].metrics.width, 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(source.runs[0u].metrics.height, 20.0f, TEST_LOCATION);

  uint32_t horizontalDpi = 0u;
  uint32_t verticalDpi   = 0u;
  TextAbstraction::FontClient::Get().GetDpi(horizontalDpi, verticalDpi);

  Text::AsyncTextParameters parameters;
  parameters.text                        = "ab";
  parameters.fontSize                    = 18.0f * 72.0f / static_cast<float>(horizontalDpi);
  parameters.textWidth                   = 160.0f;
  parameters.textHeight                  = 60.0f;
  parameters.ellipsis                    = false;
  parameters.replacementSourceSnapshot   = source;
  parameters.replacementLayoutGeneration = 77u;
  parameters.effectiveTextScale          = 1.5f;

  Text::AsyncTextLoader               asyncLoader = Text::AsyncTextLoader::New();
  const Text::AsyncTextRenderInfo     asyncInfo   = asyncLoader.GetHeightForWidth(parameters);
  const Text::ReplacementRenderState* async =
    Text::GetImplementation(asyncLoader).GetReplacementRenderState();

  DALI_TEST_CHECK(async);
  DALI_TEST_CHECK(async->attempted);
  DALI_TEST_EQUALS(async->sourceRevision, sync.sourceRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(async->layoutGeneration, 77u, TEST_LOCATION);
  DALI_TEST_EQUALS(async->projection.GetMode(), sync.projection.GetMode(), TEST_LOCATION);
  DALI_TEST_EQUALS(async->projection.GetProcessingCharacterCount(),
                   sync.projection.GetProcessingCharacterCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(CountSyntheticGlyphs(*async), CountSyntheticGlyphs(sync), TEST_LOCATION);
  DALI_TEST_EQUALS(async->processingModel->mVisualModel->mLines.Count(),
                   sync.processingModel->mVisualModel->mLines.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements.Count(), sync.placements.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements[0u].logicalCharacterRange.characterIndex,
                   sync.placements[0u].logicalCharacterRange.characterIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements[0u].logicalCharacterRange.numberOfCharacters,
                   sync.placements[0u].logicalCharacterRange.numberOfCharacters, TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements[0u].size, sync.placements[0u].size, TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements[0u].position.x, sync.placements[0u].position.x,
                   Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements[0u].position.y, sync.placements[0u].position.y,
                   Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements[0u].lineIndex, sync.placements[0u].lineIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements[0u].visible, sync.placements[0u].visible, TEST_LOCATION);
  DALI_TEST_EQUALS(async->placements[0u].elided, sync.placements[0u].elided, TEST_LOCATION);
  const Text::FinalElisionResult& syncFinal  = sync.finalElision;
  const Text::FinalElisionResult& asyncFinal = async->finalElision;
  DALI_TEST_EQUALS(asyncFinal.textElided, syncFinal.textElided, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncFinal.applied, syncFinal.applied, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncFinal.ellipsisUnitCount, syncFinal.ellipsisUnitCount, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncFinal.ellipsisLineIndex, syncFinal.ellipsisLineIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.replacementSourceRevision, 41u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.replacementLayoutGeneration, 77u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.replacementPlacements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.lineCount,
                   static_cast<int>(async->processingModel->GetNumberOfLines()),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.isTextDirectionRTL,
                   async->processingModel->mVisualModel->mLines[0u].direction,
                   TEST_LOCATION);

  // Natural-size requests also replace the final placement state from their final Layout() call.
  controller->GetNaturalSize(false);
  const Size syncNaturalLayoutSize          = sync.layoutSize;
  parameters.replacementLayoutGeneration    = 78u;
  const Text::AsyncTextRenderInfo newerInfo = asyncLoader.GetNaturalSize(parameters);
  DALI_TEST_EQUALS(async->layoutSize.width, syncNaturalLayoutSize.width,
                   Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(async->layoutSize.height, syncNaturalLayoutSize.height,
                   Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(newerInfo.replacementPlacements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(newerInfo.replacementLayoutGeneration, 78u, TEST_LOCATION);
  DALI_TEST_EQUALS(newerInfo.lineCount,
                   static_cast<int>(async->processingModel->GetNumberOfLines()),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(newerInfo.isTextDirectionRTL,
                   async->processingModel->mVisualModel->mLines[0u].direction,
                   TEST_LOCATION);

  // Sync and async removal both clear the newer generation without invoking projection.
  controllerImpl.InvalidateReplacementRenderState();
  controllerImpl.ClearReplacementData();
  controller->Relayout(Size(161.0f, 60.0f));
  const Text::ReplacementRenderState& removedSync = controllerImpl.GetReplacementRenderState();
  DALI_TEST_CHECK(!removedSync.attempted);
  DALI_TEST_EQUALS(removedSync.placements.Count(), 0u, TEST_LOCATION);

  parameters.replacementSourceSnapshot.runs.Clear();
  parameters.replacementSourceSnapshot.hasValidReplacementSource = false;
  const Text::AsyncTextRenderInfo removedInfo = asyncLoader.GetNaturalSize(parameters);
  DALI_TEST_EQUALS(removedInfo.replacementPlacements.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(removedInfo.replacementSourceRevision, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(Text::GetImplementation(asyncLoader).GetReplacementRenderState() == nullptr);

  controller->SetText("plain");
  DALI_TEST_CHECK(!controllerImpl.mReplacementData);

  END_TEST;
}

int UtcDaliReplacementAsyncRenderScalePlacementP(void)
{
  UiTestApplication application;

  uint32_t horizontalDpi = 0u;
  uint32_t verticalDpi   = 0u;
  TextAbstraction::FontClient::Get().GetDpi(horizontalDpi, verticalDpi);

  Text::ReplacementSourceSnapshot source;
  source.runs.PushBack(Candidate(0u, 2u, 28.0f, 20.0f, 750u));
  source.sourceRevision            = 51u;
  source.hasValidReplacementSource = true;

  Text::AsyncTextParameters parameters;
  parameters.text                        = "ab";
  parameters.fontSize                    = 18.0f * 72.0f / static_cast<float>(horizontalDpi);
  parameters.textWidth                   = 160.0f;
  parameters.textHeight                  = 60.0f;
  parameters.renderScale                 = 2.0f;
  parameters.effectiveTextScale          = 1.5f;
  parameters.replacementSourceSnapshot   = source;
  parameters.replacementLayoutGeneration = 81u;

  Text::AsyncTextLoader               loader            = Text::AsyncTextLoader::New();
  bool                                cachedNaturalSize = false;
  const Size                          naturalSize       = loader.SetupRenderScale(parameters, cachedNaturalSize);
  const Text::AsyncTextRenderInfo     renderInfo        = loader.RenderText(parameters, cachedNaturalSize, naturalSize);
  const Text::ReplacementRenderState* processing =
    Text::GetImplementation(loader).GetReplacementRenderState();

  DALI_TEST_CHECK(processing);
  DALI_TEST_CHECK(processing->processingModel);
  DALI_TEST_EQUALS(processing->placements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(processing->placements[0u].size, Vector2(84.0f, 60.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderInfo.replacementPlacements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderInfo.replacementPlacements[0u].size, Vector2(42.0f, 30.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(source.runs[0u].metrics.width, 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(source.runs[0u].metrics.height, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderInfo.replacementPlacements[0u].position.x,
                   processing->placements[0u].position.x * 0.5f,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(renderInfo.replacementPlacements[0u].position.y,
                   processing->placements[0u].position.y * 0.5f,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(renderInfo.replacementSourceRevision, 51u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderInfo.replacementLayoutGeneration, 81u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliReplacementProductionParityMatrixP(void)
{
  UiTestApplication application;

  uint32_t horizontalDpi = 0u;
  uint32_t verticalDpi   = 0u;
  TextAbstraction::FontClient::Get().GetDpi(horizontalDpi, verticalDpi);
  const float asyncPointSize = 18.0f * 72.0f / static_cast<float>(horizontalDpi);

  auto checkCase = [asyncPointSize](const std::string&                          utf8,
                                    const Vector<Text::ReplacementRunSnapshot>& sourceRuns,
                                    const Size&                                 size,
                                    bool                                        multiline,
                                    bool                                        ellipsis,
                                    uint32_t                                    expectedAccepted,
                                    uint32_t                                    expectedSuppressed,
                                    Text::Alignment                             horizontalAlignment = Text::Alignment::START,
                                    Text::Alignment                             verticalAlignment   = Text::Alignment::START)
  {
    Text::ReplacementSourceSnapshot source;
    source.runs                      = sourceRuns;
    source.sourceRevision            = 99u;
    source.hasValidReplacementSource = !source.runs.Empty();

    Text::ControllerPtr     controller     = Text::Controller::New();
    Text::Controller::Impl& controllerImpl = Text::Controller::Impl::GetImplementation(*controller.Get());
    controller->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(multiline);
    controller->SetLineWrapMode(Text::LineWrapMode::CHARACTER);
    controller->SetRelativeLineSize(1.0f);
    controller->SetTextElideEnabled(ellipsis);
    controller->SetEllipsisPosition(Text::EllipsisPosition::END);
    controller->SetHorizontalAlignment(horizontalAlignment);
    controller->SetVerticalAlignment(verticalAlignment);
    controller->SetVerticalLineAlignment(Text::Alignment::START);
    controller->SetText(utf8);
    controllerImpl.GetOrCreateReplacementSourceSnapshot() = source;
    controller->Relayout(size);

    Text::AsyncTextParameters parameters;
    parameters.text                        = utf8;
    parameters.fontSize                    = asyncPointSize;
    parameters.textWidth                   = size.width;
    parameters.textHeight                  = size.height;
    parameters.isMultiLine                 = multiline;
    parameters.lineWrapMode                = Text::LineWrapMode::CHARACTER;
    parameters.ellipsis                    = ellipsis;
    parameters.ellipsisPosition            = Text::EllipsisPosition::END;
    parameters.horizontalAlignment         = horizontalAlignment;
    parameters.verticalAlignment           = verticalAlignment;
    parameters.verticalLineAlignment       = Text::Alignment::START;
    parameters.replacementSourceSnapshot   = source;
    parameters.replacementLayoutGeneration = 123u;

    Text::AsyncTextLoader               asyncLoader = Text::AsyncTextLoader::New();
    const Text::AsyncTextRenderInfo     asyncInfo   = asyncLoader.RenderText(parameters, false, Size::ZERO);
    const Text::ReplacementRenderState& sync        = controllerImpl.GetReplacementRenderState();
    const Text::ReplacementRenderState* async =
      Text::GetImplementation(asyncLoader).GetReplacementRenderState();

    DALI_TEST_EQUALS(sync.attempted, !sourceRuns.Empty(), TEST_LOCATION);
    DALI_TEST_EQUALS(async != nullptr, !sourceRuns.Empty(), TEST_LOCATION);
    if(sourceRuns.Empty())
    {
      DALI_TEST_EQUALS(asyncInfo.replacementPlacements.Count(), 0u, TEST_LOCATION);
      return;
    }
    DALI_TEST_CHECK(async);

    DALI_TEST_EQUALS(async->projection.GetReplacementRuns().Count(),
                     sync.projection.GetReplacementRuns().Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(sync.projection.GetReplacementRuns().Count(), expectedAccepted, TEST_LOCATION);
    DALI_TEST_EQUALS(sourceRuns.Count() - sync.projection.GetReplacementRuns().Count(),
                     expectedSuppressed, TEST_LOCATION);
    DALI_TEST_EQUALS(static_cast<uint32_t>(async->projection.GetMode()),
                     static_cast<uint32_t>(sync.projection.GetMode()), TEST_LOCATION);
    if(sync.projection.HasReplacements())
    {
      DALI_TEST_EQUALS(async->projection.GetProcessingCharacterCount(),
                       sync.projection.GetProcessingCharacterCount(), TEST_LOCATION);
      for(uint32_t index = 0u; index < sync.projection.GetProcessingCharacterCount(); ++index)
      {
        DALI_TEST_EQUALS(async->projection.GetProcessingText()[index],
                         sync.projection.GetProcessingText()[index], TEST_LOCATION);
      }
      DALI_TEST_EQUALS(CountSyntheticGlyphs(*async), CountSyntheticGlyphs(sync), TEST_LOCATION);
      DALI_TEST_EQUALS(async->processingModel->mVisualModel->mLines.Count(),
                       sync.processingModel->mVisualModel->mLines.Count(), TEST_LOCATION);
      DALI_TEST_EQUALS(static_cast<uint32_t>(async->processingModel->GetVerticalLineAlignment()),
                       static_cast<uint32_t>(sync.processingModel->GetVerticalLineAlignment()),
                       TEST_LOCATION);
      DALI_TEST_EQUALS(async->finalElision.textElided,
                       sync.finalElision.textElided,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(async->layoutSize.width, sync.layoutSize.width,
                       Math::MACHINE_EPSILON_1000, TEST_LOCATION);
      DALI_TEST_EQUALS(async->layoutSize.height, sync.layoutSize.height,
                       Math::MACHINE_EPSILON_1000, TEST_LOCATION);
      DALI_TEST_EQUALS(async->processingModel->mVisualModel->mControlSize,
                       sync.processingModel->mVisualModel->mControlSize,
                       TEST_LOCATION);
      for(uint32_t lineIndex = 0u;
          lineIndex < sync.processingModel->mVisualModel->mLines.Count();
          ++lineIndex)
      {
        const Text::LineRun& syncLine  = sync.processingModel->mVisualModel->mLines[lineIndex];
        const Text::LineRun& asyncLine = async->processingModel->mVisualModel->mLines[lineIndex];
        DALI_TEST_EQUALS(asyncLine.ascender, syncLine.ascender, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
        DALI_TEST_EQUALS(asyncLine.descender, syncLine.descender, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
        DALI_TEST_EQUALS(asyncLine.lineSpacing, syncLine.lineSpacing, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
      }
      for(const Text::ProjectedReplacementRun& replacement : sync.projection.GetReplacementRuns())
      {
        const Text::ReplacementRunSnapshot& authored = sourceRuns[replacement.sourceRunIndex];
        DALI_TEST_EQUALS(replacement.logicalCharacterRange.characterIndex,
                         authored.logicalCharacterRange.characterIndex, TEST_LOCATION);
        DALI_TEST_EQUALS(replacement.logicalCharacterRange.numberOfCharacters,
                         authored.logicalCharacterRange.numberOfCharacters, TEST_LOCATION);
        DALI_TEST_EQUALS(sync.projection.GetProcessingText()[replacement.projectedCharacterIndex],
                         Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER, TEST_LOCATION);
        for(Text::CharacterIndex logicalIndex = replacement.logicalCharacterRange.characterIndex;
            logicalIndex < replacement.logicalCharacterRange.characterIndex +
                             replacement.logicalCharacterRange.numberOfCharacters;
            ++logicalIndex)
        {
          DALI_TEST_EQUALS(sync.projection.LogicalCharacterToProjected(logicalIndex),
                           replacement.projectedCharacterIndex, TEST_LOCATION);
        }
      }
    }

    DALI_TEST_EQUALS(async->placements.Count(), sync.placements.Count(), TEST_LOCATION);
    for(uint32_t index = 0u; index < sync.placements.Count(); ++index)
    {
      const Text::ReplacementPlacement& lhs = sync.placements[index];
      const Text::ReplacementPlacement& rhs = async->placements[index];
      DALI_TEST_EQUALS(rhs.logicalCharacterRange.characterIndex,
                       lhs.logicalCharacterRange.characterIndex, TEST_LOCATION);
      DALI_TEST_EQUALS(rhs.logicalCharacterRange.numberOfCharacters,
                       lhs.logicalCharacterRange.numberOfCharacters, TEST_LOCATION);
      DALI_TEST_EQUALS(rhs.lineIndex, lhs.lineIndex, TEST_LOCATION);
      if(lhs.visible && rhs.visible)
      {
        const Text::LineRun& syncLine  = sync.processingModel->mVisualModel->mLines[lhs.lineIndex];
        const Text::LineRun& asyncLine = async->processingModel->mVisualModel->mLines[rhs.lineIndex];
        DALI_TEST_EQUALS(asyncLine.ascender, syncLine.ascender, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
        DALI_TEST_EQUALS(asyncLine.descender, syncLine.descender, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
        DALI_TEST_EQUALS(asyncLine.lineSpacing, syncLine.lineSpacing, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
        Text::FinalGlyphGeometry syncGeometry;
        Text::FinalGlyphGeometry asyncGeometry;
        DALI_TEST_CHECK(Text::GetFinalSourceGlyphGeometry(*sync.processingModel,
                                                          sync.finalElision,
                                                          lhs.syntheticGlyphIndex,
                                                          syncGeometry));
        DALI_TEST_CHECK(Text::GetFinalSourceGlyphGeometry(*async->processingModel,
                                                          async->finalElision,
                                                          rhs.syntheticGlyphIndex,
                                                          asyncGeometry));
        DALI_TEST_EQUALS(asyncGeometry.contentLocalPenPosition.y,
                         syncGeometry.contentLocalPenPosition.y,
                         Math::MACHINE_EPSILON_1000,
                         TEST_LOCATION);
        DALI_TEST_EQUALS(asyncGeometry.baseline,
                         syncGeometry.baseline,
                         Math::MACHINE_EPSILON_1000,
                         TEST_LOCATION);
      }
      DALI_TEST_EQUALS(rhs.position.x, lhs.position.x, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
      DALI_TEST_EQUALS(rhs.position.y, lhs.position.y, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
      DALI_TEST_EQUALS(rhs.size, lhs.size, TEST_LOCATION);
      DALI_TEST_EQUALS(rhs.visible, lhs.visible, TEST_LOCATION);
      DALI_TEST_EQUALS(rhs.elided, lhs.elided, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(asyncInfo.replacementPlacements.Count(), async->placements.Count(), TEST_LOCATION);
  };

  Vector<Text::ReplacementRunSnapshot> noRuns;
  checkCase("plain", noRuns, Size(160.0f, 60.0f), false, false, 0u, 0u);

  auto runSingle = [&checkCase](const std::string&   utf8,
                                Text::CharacterIndex start,
                                Text::Length         length,
                                const Size&          size,
                                bool                 multiline = false,
                                bool                 ellipsis  = false)
  {
    Vector<Text::ReplacementRunSnapshot> runs;
    runs.PushBack(Candidate(start, length, 28.0f, 20.0f, 1u));
    checkCase(utf8, runs, size, multiline, ellipsis, 1u, 0u);
  };

  runSingle("A\uFFFCB", 1u, 1u, Size(160.0f, 60.0f));
  runSingle("AxB", 1u, 1u, Size(160.0f, 60.0f));
  runSingle("AiconB", 1u, 4u, Size(160.0f, 60.0f));
  runSingle("\xD7\x90\xD7\x91ICON\xD7\x92", 2u, 4u, Size(180.0f, 60.0f));
  runSingle("A\xD7\x90\xD7\x91ICON\xD7\x92Z", 3u, 4u, Size(180.0f, 60.0f));
  runSingle(
    "\xD7\x90"
    "ABICONCD"
    "\xD7\x91",
    3u, 4u, Size(180.0f, 60.0f));
  runSingle("AAa b cBB", 2u, 5u, Size(44.0f, 160.0f), true, false);
  runSingle("xy", 0u, 2u, Size(20.0f, 80.0f), true, false);

  // Exact UTF-32 ranges remain valid even when they cut through a combining, emoji or shaping sequence.
  runSingle(
    "Xa\xCC\x81"
    "Y",
    1u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "Xa\xCC\x81"
    "Y",
    2u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "Xa\xCC\x81"
    "Y",
    1u, 2u, Size(160.0f, 60.0f));
  runSingle(
    "X\xE2\x9D\xA4\xEF\xB8\x8F"
    "Y",
    1u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "X\xE2\x9D\xA4\xEF\xB8\x8F"
    "Y",
    2u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "X\xE2\x9D\xA4\xEF\xB8\x8F"
    "Y",
    1u, 2u, Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"
    "Y",
    1u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"
    "Y",
    2u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"
    "Y",
    1u, 2u, Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
    "Y",
    1u, 1u,
    Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
    "Y",
    2u, 1u,
    Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
    "Y",
    3u, 1u,
    Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
    "Y",
    1u, 2u,
    Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9"
    "Y",
    1u, 3u,
    Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"
    "Y",
    1u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"
    "Y",
    2u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "X\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"
    "Y",
    1u, 2u, Size(160.0f, 60.0f));
  runSingle("XfiY", 1u, 1u, Size(160.0f, 60.0f));
  runSingle("XfiY", 1u, 2u, Size(160.0f, 60.0f));
  runSingle("XffiY", 2u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "X\xD9\x84\xD8\xA7"
    "Y",
    1u, 1u, Size(160.0f, 60.0f));
  runSingle(
    "X\xD9\x84\xD8\xA7"
    "Y",
    2u, 1u, Size(160.0f, 60.0f));

  Vector<Text::ReplacementRunSnapshot> adjacentRuns;
  adjacentRuns.PushBack(Candidate(1u, 2u, 20.0f, 18.0f, 10u));
  adjacentRuns.PushBack(Candidate(3u, 2u, 21.0f, 18.0f, 11u));
  checkCase("AabcdE", adjacentRuns, Size(45.0f, 120.0f), true, false, 2u, 0u);

  Vector<Text::ReplacementRunSnapshot> overlapRuns;
  overlapRuns.PushBack(Candidate(1u, 3u, 20.0f, 18.0f, 20u));
  overlapRuns.PushBack(Candidate(2u, 3u, 20.0f, 18.0f, 21u));
  checkCase("abcdef", overlapRuns, Size(160.0f, 60.0f), false, false, 1u, 1u);

  Vector<Text::ReplacementRunSnapshot> newlineRuns;
  newlineRuns.PushBack(Candidate(1u, 3u, 20.0f, 18.0f, 30u));
  checkCase("a\nbc", newlineRuns, Size(160.0f, 60.0f), true, false, 0u, 1u);

  const char* ellipsisTexts[] = {
    "AabcdefghijZ",
    "\xD7\x90"
    "abcdefghij"
    "\xD7\x91",
    "A\xD7\x90"
    "abcdefghij"
    "\xD7\x91Z"};
  const Text::CharacterIndex starts[] = {1u, 1u, 2u};
  for(uint32_t caseIndex = 0u; caseIndex < 3u; ++caseIndex)
  {
    Vector<Text::ReplacementRunSnapshot> runs;
    for(uint32_t index = 0u; index < 5u; ++index)
    {
      runs.PushBack(Candidate(starts[caseIndex] + index * 2u, 2u, 20.0f, 18.0f, 40u + index));
    }
    checkCase(ellipsisTexts[caseIndex], runs, Size(62.0f, 40.0f), false, true, 5u, 0u);
  }

  const std::string complexText =
    "One \uFFFC two ordinary words then \uFFFC followed by visible trailing text and \uFFFC final overflow words";
  const Vector<Text::Character>        complexCharacters = Utf32(complexText);
  Vector<Text::ReplacementRunSnapshot> complexRuns;
  for(Text::CharacterIndex index = 0u; index < complexCharacters.Count(); ++index)
  {
    if(complexCharacters[index] != Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER)
    {
      continue;
    }
    const uint32_t replacementIndex = complexRuns.Count();
    const float    widths[]         = {18.0f, 76.0f, 150.0f};
    const float    heights[]        = {16.0f, 44.0f, 88.0f};
    complexRuns.PushBack(Candidate(index,
                                   1u,
                                   widths[replacementIndex],
                                   heights[replacementIndex],
                                   900u + replacementIndex));
  }
  DALI_TEST_EQUALS(complexRuns.Count(), 3u, TEST_LOCATION);
  const Text::Alignment alignments[] = {
    Text::Alignment::START,
    Text::Alignment::CENTER,
    Text::Alignment::END};
  for(const Text::Alignment horizontalAlignment : alignments)
  {
    for(const Text::Alignment verticalAlignment : alignments)
    {
      checkCase(complexText,
                complexRuns,
                Size(210.0f, 90.0f),
                true,
                true,
                3u,
                0u,
                horizontalAlignment,
                verticalAlignment);
    }
  }

  END_TEST;
}
