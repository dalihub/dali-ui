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

// INTERNAL INCLUDES
#include "replacement-layout-test-adapter.h"
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-start-geometry.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-processing-source.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace Dali::Ui::Text
{
namespace
{
ReplacementSourceSnapshot MakeSourceSnapshot(const ReplacementProjection& projection, uint64_t sourceRevision)
{
  ReplacementSourceSnapshot snapshot;
  snapshot.sourceRevision = sourceRevision;
  snapshot.runs.Reserve(projection.GetReplacementRuns().Count());
  for(const ProjectedReplacementRun& projected : projection.GetReplacementRuns())
  {
    ReplacementRunSnapshot run;
    run.logicalCharacterRange = projected.logicalCharacterRange;
    run.metrics               = projected.metrics;
    run.occurrenceIdentity    = projected.occurrenceIdentity;
    snapshot.runs.PushBack(run);
  }
  snapshot.hasValidReplacementSource = !snapshot.runs.Empty();
  return snapshot;
}

void CopyAuthoredRuns(const Model& source, Model& target)
{
  const LogicalModel& sourceLogical            = *source.mLogicalModel;
  LogicalModel&       targetLogical            = *target.mLogicalModel;
  targetLogical.mFontDescriptionRuns           = sourceLogical.mFontDescriptionRuns;
  targetLogical.mColorRuns                     = sourceLogical.mColorRuns;
  targetLogical.mBackgroundColorRuns           = sourceLogical.mBackgroundColorRuns;
  targetLogical.mUnderlinedCharacterRuns       = sourceLogical.mUnderlinedCharacterRuns;
  targetLogical.mStrikethroughCharacterRuns    = sourceLogical.mStrikethroughCharacterRuns;
  targetLogical.mCharacterSpacingCharacterRuns = sourceLogical.mCharacterSpacingCharacterRuns;
  targetLogical.mVariationsMap                 = sourceLogical.mVariationsMap;
}

ControllerPtr CreateController(const Model&                        originalModel,
                               const ReplacementLayoutTestOptions& options)
{
  std::string              utf8;
  const Vector<Character>& logicalText = originalModel.mLogicalModel->mText;
  Utf32ToUtf8(logicalText.Begin(), logicalText.Count(), utf8);

  ControllerPtr     controller = Controller::New();
  Controller::Impl& impl       = Controller::Impl::GetImplementation(*controller.Get());
  controller->SetText(utf8);
  CopyTextProcessingProperties(originalModel, *impl.mModel);
  CopyAuthoredRuns(originalModel, *impl.mModel);

  const uint32_t pointsPerUnit = impl.GetFontClient().GetNumberOfPointsPerOneUnitOfPointSize();
  const float    pointSize     = pointsPerUnit == 0u
                                   ? 0.0f
                                   : static_cast<float>(options.fontPointSize) / static_cast<float>(pointsPerUnit);
  controller->SetDefaultFontSize(pointSize, Controller::POINT_SIZE);
  controller->SetCharacterSpacing(options.characterSpacing);
  controller->SetMultiLineEnabled(options.layoutType == Layout::Engine::MULTI_LINE_BOX);
  controller->SetLineWrapMode(options.lineWrapMode);
  controller->SetTextElideEnabled(options.elideText);
  controller->SetEllipsisPosition(options.ellipsisPosition);
  controller->SetHorizontalAlignment(options.horizontalAlignment);
  controller->SetVerticalAlignment(options.verticalAlignment);
  controller->SetLayoutDirection(options.layoutDirection);
  controller->SetLayoutDirectionMode(options.matchLayoutDirection ? LayoutDirectionMode::LOCALE
                                                                  : LayoutDirectionMode::CONTENTS);
  impl.mLayoutEngine.SetDefaultLineSize(options.defaultLineSize);
  impl.mLayoutEngine.SetDefaultLineSpacing(options.defaultLineSpacing);
  impl.mLayoutEngine.SetRelativeLineSize(options.relativeLineSize);
  impl.mLayoutEngine.SetFontPixelSize(options.fontPixelSize);
  impl.mIsMarqueeMaxTextureExceeded = options.marqueeMaxTextureExceeded;
  return controller;
}

bool RunController(const Model&                        originalModel,
                   const ReplacementSourceSnapshot&    sourceSnapshot,
                   const ReplacementLayoutTestOptions& options,
                   ReplacementRenderState&             result)
{
  ControllerPtr     controller                = CreateController(originalModel, options);
  Controller::Impl& impl                      = Controller::Impl::GetImplementation(*controller.Get());
  impl.GetOrCreateReplacementSourceSnapshot() = sourceSnapshot;
  impl.mReplacementData->layoutGeneration     = options.layoutGeneration == 0u ? 0u : options.layoutGeneration - 1u;

  controller->Relayout(options.contentSize);
  result = std::move(impl.GetOrCreateReplacementRenderState());
  return result.processingModel && result.projection.HasReplacements();
}
} // unnamed namespace

bool LayoutReplacementForTest(const Model&                 originalModel,
                              const ReplacementProjection& projection,
                              ReplacementLayoutTestServices&,
                              const ReplacementLayoutTestOptions& options,
                              ReplacementRenderState&             result)
{
  return projection.HasReplacements() &&
         RunController(originalModel, MakeSourceSnapshot(projection, options.sourceRevision), options, result);
}

bool LayoutReplacementForTest(const ReplacementProjection&        projection,
                              ReplacementLayoutTestServices&      services,
                              const ReplacementLayoutTestOptions& options,
                              ReplacementRenderState&             result)
{
  ModelPtr originalModel              = Model::New();
  originalModel->mLogicalModel->mText = projection.GetLogicalText();
  return LayoutReplacementForTest(*originalModel, projection, services, options, result);
}

bool LayoutOrdinaryForTest(const Model&                        originalModel,
                           const ReplacementLayoutTestOptions& options,
                           ModelPtr&                           result)
{
  ControllerPtr     controller = CreateController(originalModel, options);
  Controller::Impl& impl       = Controller::Impl::GetImplementation(*controller.Get());
  controller->Relayout(options.contentSize);
  result = impl.mModel;
  return static_cast<bool>(result);
}

OrdinaryMarqueeTransitionTrace TraceOrdinaryMarqueeTransitionForTest(
  const Model&                        originalModel,
  const ReplacementLayoutTestOptions& options)
{
  struct SharedGlyph
  {
    CharacterIndex character{0u};
    Length         glyphOccurrence{0u};
    GlyphInfo      glyph;
    float          sourceLayoutX{0.0f};
  };

  OrdinaryMarqueeTransitionTrace trace;
  ControllerPtr                  controller = CreateController(originalModel, options);
  Controller::Impl&              impl       = Controller::Impl::GetImplementation(*controller.Get());
  controller->Relayout(options.contentSize, options.layoutDirection);

  const FinalElisionResult* final       = controller->GetFinalElisionResult();
  const VisualModel&        source      = *impl.mModel->mVisualModel;
  const MarqueeStartAnchor  startAnchor = controller->GetMarqueeStartAnchor();
  if(!final || !startAnchor.valid || final->finalToSourceGlyphIndices.Count() != final->glyphs.Count())
  {
    return trace;
  }

  std::vector<SharedGlyph> shared;
  bool                     anchorCaptured = false;
  for(GlyphIndex finalGlyph = 0u; finalGlyph < final->glyphs.Count(); ++finalGlyph)
  {
    const GlyphIndex sourceGlyph = final->finalToSourceGlyphIndices[finalGlyph];
    if(finalGlyph == final->ellipsisFinalGlyphIndex || sourceGlyph >= source.mGlyphs.Count() ||
       sourceGlyph >= source.mGlyphsToCharacters.Count() ||
       source.mGlyphs[sourceGlyph].width <= 0.01f || source.mGlyphs[sourceGlyph].height <= 0.01f)
    {
      continue;
    }

    const CharacterIndex character       = source.mGlyphsToCharacters[sourceGlyph];
    Length               glyphOccurrence = 0u;
    for(GlyphIndex candidate = 0u; candidate < sourceGlyph; ++candidate)
    {
      if(source.mGlyphsToCharacters[candidate] == character)
      {
        ++glyphOccurrence;
      }
    }

    shared.push_back({character,
                      glyphOccurrence,
                      source.mGlyphs[sourceGlyph],
                      source.mGlyphPositions[sourceGlyph].x});
    if(character == startAnchor.characterIndex && glyphOccurrence == startAnchor.glyphOccurrence)
    {
      trace.anchorCharacter   = character;
      trace.staticSourceGlyph = sourceGlyph;
      trace.staticControlX    = startAnchor.staticControlX;
      anchorCaptured          = true;
    }
  }
  if(shared.empty() || !anchorCaptured)
  {
    return trace;
  }

  controller->SetMarqueeEnabled(true, false, MarqueeOrientation::HORIZONTAL);
  controller->Relayout(options.contentSize, options.layoutDirection);
  trace.naturalContentWidth = controller->GetNaturalSize(false).width;
  trace.directionRightToLeft = controller->GetMarqueeTextDirection();

  TypesetterPtr                typesetter = Typesetter::New(controller->GetRenderTextModel());
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  typesetter->SetFontClient(fontClient);
  typesetter->SetFinalElisionResult(controller->GetFinalElisionResult());
  typesetter->Render(Size(trace.naturalContentWidth + 20.0f, options.contentSize.height),
                     controller->GetTextDirection(),
                     Typesetter::RENDER_TEXT_AND_STYLES,
                     true,
                     Pixel::RGBA8888);
  const MarqueeTextureAnchor renderedTextureAnchor =
    typesetter->ResolveMarqueeTextureAnchor(startAnchor);

  const VisualModel& natural = *impl.mModel->mVisualModel;
  float  minimumTranslation = std::numeric_limits<float>::max();
  float  maximumTranslation = std::numeric_limits<float>::lowest();
  size_t resolvedCount      = 0u;
  for(const SharedGlyph& candidate : shared)
  {
    GlyphIndex naturalGlyph = FinalElisionResult::INVALID_GLYPH_INDEX;
    Length     occurrence   = 0u;
    for(GlyphIndex glyph = 0u; glyph < natural.mGlyphsToCharacters.Count(); ++glyph)
    {
      if(natural.mGlyphsToCharacters[glyph] == candidate.character &&
         occurrence++ == candidate.glyphOccurrence)
      {
        naturalGlyph = glyph;
        break;
      }
    }
    if(naturalGlyph == FinalElisionResult::INVALID_GLYPH_INDEX ||
       naturalGlyph >= natural.mGlyphs.Count() || naturalGlyph >= natural.mGlyphPositions.Count())
    {
      continue;
    }
    const GlyphInfo& glyph = natural.mGlyphs[naturalGlyph];
    if(glyph.fontId != candidate.glyph.fontId || glyph.index != candidate.glyph.index)
    {
      continue;
    }

    const float translation = natural.mGlyphPositions[naturalGlyph].x - candidate.sourceLayoutX;
    minimumTranslation      = std::min(minimumTranslation, translation);
    maximumTranslation      = std::max(maximumTranslation, translation);
    if(candidate.character == startAnchor.characterIndex &&
       candidate.glyphOccurrence == startAnchor.glyphOccurrence)
    {
      trace.marqueeTextureGlyph = naturalGlyph;
      trace.marqueeTextureX     = natural.mGlyphPositions[naturalGlyph].x;
    }
    ++resolvedCount;
  }

  if(resolvedCount == shared.size() && renderedTextureAnchor.valid &&
     std::fabs(renderedTextureAnchor.textureX - trace.marqueeTextureX) <= 0.01f)
  {
    trace.marqueeTextureX              = renderedTextureAnchor.textureX;
    trace.sourceToTextureMinimumTranslation = minimumTranslation;
    trace.sourceToTextureMaximumTranslation = maximumTranslation;
    trace.valid                             = true;
  }
  return trace;
}

bool LayoutReplacementSourceForTest(const Model&                     originalModel,
                                    const ReplacementSourceSnapshot& sourceSnapshot,
                                    ReplacementLayoutTestServices&,
                                    const ReplacementLayoutTestOptions& options,
                                    ReplacementRenderState&             result)
{
  return !sourceSnapshot.runs.Empty() && RunController(originalModel, sourceSnapshot, options, result);
}

} // namespace Dali::Ui::Text
