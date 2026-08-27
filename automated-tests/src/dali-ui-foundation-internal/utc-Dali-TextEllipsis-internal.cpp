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
#include <cstring>
#include <limits>
#include <utility>

// INTERNAL INCLUDES
#include <dali/integration-api/pixel-data-integ.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-resolver.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-start-geometry.h>
#include <dali-ui-foundation/internal/text/rendering/styles/underline-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
float CalculateMarqueeAnchorControlX(float textureX, float viewportOrigin, float initialDelta)
{
  return textureX - viewportOrigin - initialDelta;
}

struct VisualGeometrySnapshot
{
  Vector<Text::GlyphInfo> glyphs;
  Vector<Vector2>         positions;
  Vector<Text::LineRun>   lines;
  Size                    layoutSize{Size::ZERO};
};

VisualGeometrySnapshot SnapshotVisualGeometry(const Text::VisualModel& visual)
{
  return {visual.mGlyphs, visual.mGlyphPositions, visual.mLines, visual.GetLayoutSize()};
}

bool IsSameVisualGeometry(const VisualGeometrySnapshot& snapshot, const Text::VisualModel& visual)
{
  if(snapshot.glyphs.Count() != visual.mGlyphs.Count() ||
     snapshot.positions.Count() != visual.mGlyphPositions.Count() ||
     snapshot.lines.Count() != visual.mLines.Count() ||
     snapshot.layoutSize != visual.GetLayoutSize())
  {
    return false;
  }
  for(Text::GlyphIndex index = 0u; index < snapshot.glyphs.Count(); ++index)
  {
    const Text::GlyphInfo& left  = snapshot.glyphs[index];
    const Text::GlyphInfo& right = visual.mGlyphs[index];
    if(left.fontId != right.fontId || left.index != right.index || left.width != right.width ||
       left.height != right.height || left.xBearing != right.xBearing || left.yBearing != right.yBearing ||
       left.advance != right.advance || left.scaleFactor != right.scaleFactor ||
       left.isItalicRequired != right.isItalicRequired || left.isBoldRequired != right.isBoldRequired ||
       left.isShaped != right.isShaped || snapshot.positions[index] != visual.mGlyphPositions[index])
    {
      return false;
    }
  }
  for(Text::LineIndex index = 0u; index < snapshot.lines.Count(); ++index)
  {
    const Text::LineRun& left  = snapshot.lines[index];
    const Text::LineRun& right = visual.mLines[index];
    if(left.glyphRun.glyphIndex != right.glyphRun.glyphIndex ||
       left.glyphRun.numberOfGlyphs != right.glyphRun.numberOfGlyphs ||
       left.characterRun.characterIndex != right.characterRun.characterIndex ||
       left.characterRun.numberOfCharacters != right.characterRun.numberOfCharacters ||
       left.width != right.width || left.ascender != right.ascender || left.descender != right.descender ||
       left.extraLength != right.extraLength || left.alignmentOffset != right.alignmentOffset ||
       left.lineSpacing != right.lineSpacing || left.direction != right.direction || left.ellipsis != right.ellipsis ||
       left.isSplitToTwoHalves != right.isSplitToTwoHalves ||
       left.glyphRunSecondHalf.glyphIndex != right.glyphRunSecondHalf.glyphIndex ||
       left.glyphRunSecondHalf.numberOfGlyphs != right.glyphRunSecondHalf.numberOfGlyphs ||
       left.characterRunForSecondHalfLine.characterIndex != right.characterRunForSecondHalfLine.characterIndex ||
       left.characterRunForSecondHalfLine.numberOfCharacters != right.characterRunForSecondHalfLine.numberOfCharacters)
    {
      return false;
    }
  }
  return true;
}

Text::CharacterIndex GetRetainedCharacterEnd(const Text::FinalElisionResult& result)
{
  if(result.ellipsisLineIndex >= result.lines.Count())
  {
    return 0u;
  }
  const Text::LineRun& line = result.lines[result.ellipsisLineIndex];
  return line.characterRun.characterIndex + line.characterRun.numberOfCharacters;
}

uint32_t CountGeneratedGlyphs(const Text::FinalElisionResult& result)
{
  uint32_t count = 0u;
  for(Text::GlyphIndex sourceGlyph : result.finalToSourceGlyphIndices)
  {
    count += sourceGlyph == Text::FinalElisionResult::INVALID_GLYPH_INDEX ? 1u : 0u;
  }
  return count;
}

bool HasGeneratedEllipsis(const Text::FinalElisionResult& result)
{
  return result.HasAuthoritativeLayout() && result.ellipsisFinalGlyphIndex < result.glyphs.Count() &&
         result.ellipsisLineIndex < result.lines.Count() &&
         result.finalToSourceGlyphIndices.Count() == result.glyphs.Count() &&
         result.finalToSourceGlyphIndices[result.ellipsisFinalGlyphIndex] ==
           Text::FinalElisionResult::INVALID_GLYPH_INDEX &&
         CountGeneratedGlyphs(result) == 1u;
}

void CheckGeneratedEllipsis(const Text::FinalElisionResult& result, float controlWidth)
{
  DALI_TEST_CHECK(HasGeneratedEllipsis(result));
  DALI_TEST_CHECK(result.applied);
  DALI_TEST_EQUALS(result.ellipsisUnitCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.ellipsisOmissionReason,
                   Text::FinalElisionResult::EllipsisOmissionReason::NONE,
                   TEST_LOCATION);
  DALI_TEST_CHECK(result.lines[result.ellipsisLineIndex].width <= controlWidth + 0.01f);
  DALI_TEST_EQUALS(result.glyphs.Count(), result.lineLocalGlyphPositions.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(result.glyphs.Count(), result.viewGlyphPositions.Count(), TEST_LOCATION);
}

void CheckViewGlyphAccess(const Text::ControllerPtr& controller,
                          const Text::FinalElisionResult& result)
{
  const Text::Length glyphCount = controller->GetView().GetNumberOfGlyphs();
  DALI_TEST_EQUALS(glyphCount, result.glyphs.Count(), TEST_LOCATION);
  DALI_TEST_CHECK(glyphCount > 0u);

  Vector<Text::GlyphInfo> glyphs;
  Vector<Vector2>         positions;
  glyphs.Resize(glyphCount);
  positions.Resize(glyphCount);
  float minimumLineOffset = 0.0f;
  DALI_TEST_EQUALS(controller->GetView().GetGlyphs(glyphs.Begin(),
                                                   positions.Begin(),
                                                   minimumLineOffset,
                                                   0u,
                                                   glyphCount),
                   glyphCount,
                   TEST_LOCATION);
}

void CheckEndReplaceContract(const Text::ControllerPtr& controller,
                             const Text::FinalElisionResult& result,
                             float controlWidth)
{
  CheckGeneratedEllipsis(result, controlWidth);
  const Text::Controller::Impl& impl    = Text::Controller::Impl::GetImplementation(*controller.Get());
  const Text::VisualModel&      source  = *impl.mModel->mVisualModel;
  const Text::LogicalModel&     logical = *impl.mModel->mLogicalModel;
  const Text::LineRun&          line    = result.lines[result.ellipsisLineIndex];
  const Text::CharacterIndex    retainedEnd = GetRetainedCharacterEnd(result);
  DALI_TEST_CHECK(retainedEnd < logical.mText.Count());
  DALI_TEST_CHECK(retainedEnd < source.mCharactersToGlyph.Count());

  const Text::GlyphIndex firstRemovedGlyph = source.mCharactersToGlyph[retainedEnd];
  DALI_TEST_CHECK(firstRemovedGlyph < source.mGlyphs.Count());
  DALI_TEST_CHECK(firstRemovedGlyph < result.sourceToFinalGlyphIndices.Count());
  DALI_TEST_EQUALS(result.sourceToFinalGlyphIndices[firstRemovedGlyph],
                   Text::FinalElisionResult::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);

  const float removedPen = source.mGlyphPositions[firstRemovedGlyph].x -
                           source.mGlyphs[firstRemovedGlyph].xBearing;
  const Text::GlyphIndex ellipsisIndex = result.ellipsisFinalGlyphIndex;
  const Text::GlyphInfo& ellipsis      = result.glyphs[ellipsisIndex];
  const float ellipsisPen = result.lineLocalGlyphPositions[ellipsisIndex].x - ellipsis.xBearing;
  const Text::GlyphIndex ellipsisStyleGlyph = result.finalToStyleGlyphIndices[ellipsisIndex];
  DALI_TEST_CHECK(ellipsisStyleGlyph < source.mGlyphs.Count());
  const float ellipsisSpacing = Text::GetGlyphCharacterSpacing(ellipsisStyleGlyph,
                                                               source.GetCharacterSpacingGlyphRuns(),
                                                               source.GetCharacterSpacing());
  const float ellipsisAdvance = Text::GetCalculatedAdvance(0x2026u, ellipsisSpacing, ellipsis.advance);

  bool  hasLeftRetained   = false;
  bool  hasRightRetained  = false;
  float retainedLeftEdge  = -std::numeric_limits<float>::max();
  float retainedRightEdge = std::numeric_limits<float>::max();
  const Text::GlyphIndex lineEnd = line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs;
  for(Text::GlyphIndex finalGlyph = line.glyphRun.glyphIndex; finalGlyph < lineEnd; ++finalGlyph)
  {
    const Text::GlyphInfo& glyph    = result.glyphs[finalGlyph];
    const Vector2&         position = result.lineLocalGlyphPositions[finalGlyph];
    DALI_TEST_CHECK(position.x >= -0.01f);
    DALI_TEST_CHECK(position.x + std::max(0.0f, glyph.width) <= line.width + 0.01f);
    if(finalGlyph == ellipsisIndex)
    {
      continue;
    }

    const Text::GlyphIndex sourceGlyph = result.finalToSourceGlyphIndices[finalGlyph];
    DALI_TEST_CHECK(sourceGlyph < source.mGlyphs.Count());
    DALI_TEST_CHECK(source.mGlyphsToCharacters[sourceGlyph] < retainedEnd);
    const float sourcePen = source.mGlyphPositions[sourceGlyph].x - source.mGlyphs[sourceGlyph].xBearing;
    const float finalPen  = position.x - glyph.xBearing;
    const Text::CharacterIndex sourceCharacter = source.mGlyphsToCharacters[sourceGlyph];
    const float sourceSpacing = Text::GetGlyphCharacterSpacing(sourceGlyph,
                                                               source.GetCharacterSpacingGlyphRuns(),
                                                               source.GetCharacterSpacing());
    const float sourceAdvance = Text::GetCalculatedAdvance(logical.mText[sourceCharacter],
                                                           sourceSpacing,
                                                           glyph.advance);
    if(sourcePen < removedPen - 0.01f)
    {
      hasLeftRetained  = true;
      retainedLeftEdge = std::max(retainedLeftEdge, finalPen + sourceAdvance);
    }
    else if(sourcePen > removedPen + 0.01f)
    {
      hasRightRetained  = true;
      retainedRightEdge = std::min(retainedRightEdge, finalPen);
    }
  }
  DALI_TEST_CHECK(hasLeftRetained || hasRightRetained);
  if(hasLeftRetained)
  {
    DALI_TEST_EQUALS(retainedLeftEdge, ellipsisPen, 0.02f, TEST_LOCATION);
  }
  if(hasRightRetained)
  {
    DALI_TEST_EQUALS(retainedRightEdge, ellipsisPen + ellipsisAdvance, 0.02f, TEST_LOCATION);
  }
}

void CheckSameFinalOutput(const Text::FinalElisionResult& left, const Text::FinalElisionResult& right)
{
  DALI_TEST_EQUALS(GetRetainedCharacterEnd(left), GetRetainedCharacterEnd(right), TEST_LOCATION);
  DALI_TEST_EQUALS(left.glyphs.Count(), right.glyphs.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(left.finalToSourceGlyphIndices.Count(), right.finalToSourceGlyphIndices.Count(), TEST_LOCATION);
  for(Text::GlyphIndex index = 0u; index < left.glyphs.Count(); ++index)
  {
    DALI_TEST_EQUALS(left.glyphs[index].fontId, right.glyphs[index].fontId, TEST_LOCATION);
    DALI_TEST_EQUALS(left.glyphs[index].index, right.glyphs[index].index, TEST_LOCATION);
    DALI_TEST_EQUALS(left.finalToSourceGlyphIndices[index], right.finalToSourceGlyphIndices[index], TEST_LOCATION);
  }
}

bool HaveSamePixels(const PixelData& left, const PixelData& right)
{
  if(!left || !right || left.GetWidth() != right.GetWidth() || left.GetHeight() != right.GetHeight() ||
     left.GetPixelFormat() != right.GetPixelFormat() || left.GetStrideBytes() != right.GetStrideBytes())
  {
    return false;
  }
  const Dali::Integration::PixelDataBuffer leftBuffer  = Dali::Integration::GetPixelDataBuffer(left);
  const Dali::Integration::PixelDataBuffer rightBuffer = Dali::Integration::GetPixelDataBuffer(right);
  const size_t byteCount = static_cast<size_t>(left.GetStrideBytes()) * left.GetHeight();
  return leftBuffer.buffer && rightBuffer.buffer && memcmp(leftBuffer.buffer, rightBuffer.buffer, byteCount) == 0;
}

Text::ControllerPtr MakeEndController(const std::string& text, float width, float height, bool multiline = false)
{
  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText(text);
  controller->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(multiline);
  controller->SetLineWrapMode(Text::LineWrapMode::WORD);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(width, height));
  return controller;
}

Text::AsyncTextParameters MakeAsyncEndParameters(const std::string& text, float width, float height, bool multiline = false)
{
  uint32_t horizontalDpi = 0u;
  uint32_t verticalDpi   = 0u;
  TextAbstraction::FontClient::Get().GetDpi(horizontalDpi, verticalDpi);
  Text::AsyncTextParameters parameters;
  parameters.text             = text;
  parameters.fontSize         = 18.0f * 72.0f / static_cast<float>(horizontalDpi);
  parameters.textWidth        = width;
  parameters.textHeight       = height;
  parameters.isMultiLine      = multiline;
  parameters.lineWrapMode     = Text::LineWrapMode::WORD;
  parameters.ellipsis         = true;
  parameters.ellipsisPosition = Text::EllipsisPosition::END;
  return parameters;
}
} // unnamed namespace

int UtcDaliEndEllipsisInactiveP(void)
{
  UiTestApplication application;

  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText("Short text");
  controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(false);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(400.0f, 44.0f));

  DALI_TEST_CHECK(controller->GetFinalElisionResult() == nullptr);
  DALI_TEST_EQUALS(controller->GetView().GetNumberOfGlyphs(),
                   Text::Controller::Impl::GetImplementation(*controller.Get()).mModel->mVisualModel->mGlyphs.Count(),
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliEndEllipsisMarqueeStartAnchorP(void)
{
  UiTestApplication application;

  auto resolve = [](const std::string&          text,
                    Text::Alignment             alignment,
                    Dali::LayoutDirection::Type layoutDirection)
  {
    Text::ControllerPtr controller = MakeEndController(text, 120.0f, 40.0f);
    controller->SetHorizontalAlignment(alignment);
    controller->SetLayoutDirection(layoutDirection);
    controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
    controller->Relayout(Size(120.0f, 40.0f));
    return std::make_pair(controller, controller->GetMarqueeStartAnchor());
  };

  const std::string ltrText = "A long left to right line that requires END ellipsis before marquee starts";
  const std::string rtlText =
    "\xD7\xA2\xD7\x91\xD7\xA8\xD7\x99\xD7\xAA \xD7\x90\xD7\xA8\xD7\x95\xD7\x9B\xD7\x94 \xD7\x9E\xD7\x90\xD7\x95\xD7\x93 \xD7\x9C\xD7\x91\xD7\x93\xD7\x99\xD7\xA7\xD7\xAA \xD7\xA8\xD7\xA6\xD7\xA3";

  const auto ltrStart  = resolve(ltrText, Text::Alignment::START, Dali::LayoutDirection::LEFT_TO_RIGHT);
  const auto ltrCenter = resolve(ltrText, Text::Alignment::CENTER, Dali::LayoutDirection::LEFT_TO_RIGHT);
  const auto ltrEnd    = resolve(ltrText, Text::Alignment::END, Dali::LayoutDirection::LEFT_TO_RIGHT);
  const auto rtlStart  = resolve(rtlText, Text::Alignment::START, Dali::LayoutDirection::RIGHT_TO_LEFT);
  const auto rtlCenter = resolve(rtlText, Text::Alignment::CENTER, Dali::LayoutDirection::RIGHT_TO_LEFT);
  const auto rtlEnd    = resolve(rtlText, Text::Alignment::END, Dali::LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_CHECK(ltrStart.second.valid);
  DALI_TEST_CHECK(ltrCenter.second.valid);
  DALI_TEST_CHECK(ltrEnd.second.valid);
  DALI_TEST_CHECK(rtlStart.second.valid);
  DALI_TEST_CHECK(rtlCenter.second.valid);
  DALI_TEST_CHECK(rtlEnd.second.valid);
  DALI_TEST_CHECK(!Text::ResolveMarqueeFittingStartGeometry(ltrCenter.first->GetRenderTextModel()).valid);
  DALI_TEST_CHECK(!Text::ResolveMarqueeFittingStartGeometry(rtlCenter.first->GetRenderTextModel()).valid);

  // The production transform must solve the same first-frame invariant for
  // every direction/alignment combination without direction-specific signs.
  const auto checkTransition = [](const Text::MarqueeStartAnchor& anchor,
                                  bool                             direction,
                                  Text::Alignment                  alignment,
                                  float                            expectedDelta)
  {
    constexpr float controlWidth = 120.0f;
    constexpr float textureWidth = 360.0f;
    constexpr float wrapGap      = 20.0f;
    const float horizontalAlignment =
      Text::ResolveHorizontalMarqueeAlignment(true, direction, alignment);
    const float viewportOrigin =
      Text::ResolveLegacyHorizontalMarqueeViewportOrigin(horizontalAlignment,
                                                         textureWidth,
                                                         controlWidth,
                                                         wrapGap);
    const Text::MarqueeTextureAnchor textureAnchor{
      anchor.staticControlX + viewportOrigin + expectedDelta,
      true};
    const Text::MarqueeInitialDelta initialDelta =
      Text::ResolveMarqueeInitialDelta(anchor,
                                       textureAnchor,
                                       horizontalAlignment,
                                       textureWidth,
                                       controlWidth,
                                       wrapGap);
    DALI_TEST_CHECK(initialDelta.valid);
    DALI_TEST_EQUALS(initialDelta.value, expectedDelta, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(CalculateMarqueeAnchorControlX(textureAnchor.textureX,
                                                    viewportOrigin,
                                                    initialDelta.value),
                     anchor.staticControlX,
                     0.001f,
                     TEST_LOCATION);
  };

  checkTransition(ltrStart.second, false, Text::Alignment::START, 2.0f);
  checkTransition(ltrCenter.second, false, Text::Alignment::CENTER, -3.0f);
  checkTransition(ltrEnd.second, false, Text::Alignment::END, 4.0f);
  checkTransition(rtlStart.second, true, Text::Alignment::START, -2.0f);
  checkTransition(rtlCenter.second, true, Text::Alignment::CENTER, 3.0f);
  checkTransition(rtlEnd.second, true, Text::Alignment::END, -4.0f);

  Text::ControllerPtr mixedEquivalent = MakeEndController("English אבג trailing words force END ellipsis", 100.0f, 44.0f);
  mixedEquivalent->SetHorizontalAlignment(Text::Alignment::CENTER);
  mixedEquivalent->SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);
  mixedEquivalent->SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  mixedEquivalent->Relayout(Size(100.0f, 44.0f), Dali::LayoutDirection::LEFT_TO_RIGHT);
  DALI_TEST_CHECK(mixedEquivalent->GetMarqueeStartAnchor().valid);

  Text::ControllerPtr mixedNonRigid = MakeEndController("אבג 123 abcdef", 97.0f, 44.0f);
  mixedNonRigid->SetHorizontalAlignment(Text::Alignment::CENTER);
  mixedNonRigid->SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);
  mixedNonRigid->SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  mixedNonRigid->Relayout(Size(97.0f, 44.0f), Dali::LayoutDirection::LEFT_TO_RIGHT);
  DALI_TEST_CHECK(!mixedNonRigid->GetMarqueeStartAnchor().valid);

  // A retained run that requires another translation must conservatively keep legacy marquee behavior.
  Text::FinalElisionResult nonRigid = *ltrCenter.first->GetFinalElisionResult();
  Text::GlyphIndex         movedGlyph = Text::FinalElisionResult::INVALID_GLYPH_INDEX;
  for(Text::GlyphIndex finalGlyph = 0u; finalGlyph < nonRigid.glyphs.Count(); ++finalGlyph)
  {
    if(finalGlyph != nonRigid.ellipsisFinalGlyphIndex &&
       nonRigid.finalToSourceGlyphIndices[finalGlyph] != Text::FinalElisionResult::INVALID_GLYPH_INDEX &&
       nonRigid.glyphs[finalGlyph].width > 0.01f && nonRigid.glyphs[finalGlyph].height > 0.01f)
    {
      movedGlyph = finalGlyph;
      break;
    }
  }
  DALI_TEST_CHECK(movedGlyph != Text::FinalElisionResult::INVALID_GLYPH_INDEX);
  nonRigid.viewGlyphPositions[movedGlyph].x += 36.0f;
  const Text::Controller::Impl& ltrImpl =
    Text::Controller::Impl::GetImplementation(*ltrCenter.first.Get());
  DALI_TEST_CHECK(!Text::ResolveMarqueeStartAnchor(&nonRigid,
                                                   ltrImpl.mModel->mVisualModel.Get())
                     .valid);

  // Protect the complete synchronous production geometry chain: capture from
  // the final static END layout, render the actual natural-width marquee
  // texture, resolve the same retained glyph, and solve the first-frame delta.
  const auto checkActualSyncTransition = [](Text::ControllerPtr              controller,
                                            const Text::MarqueeStartAnchor& staticAnchor,
                                            Text::Alignment                  alignment,
                                            Dali::LayoutDirection::Type      layoutDirection)
  {
    constexpr float controlWidth  = 120.0f;
    constexpr float controlHeight = 40.0f;
    constexpr float wrapGap       = 20.0f;

    controller->SetMarqueeEnabled(true, false, Text::MarqueeOrientation::HORIZONTAL);
    controller->Relayout(Size(controlWidth, controlHeight), layoutDirection);
    const float naturalWidth = controller->GetNaturalSize(false).width;

    Text::TypesetterPtr         typesetter = Text::Typesetter::New(controller->GetRenderTextModel());
    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    typesetter->SetFontClient(fontClient);
    typesetter->SetFinalElisionResult(controller->GetFinalElisionResult());
    typesetter->Render(Size(naturalWidth + wrapGap, controlHeight),
                       controller->GetTextDirection(),
                       Text::Typesetter::RENDER_TEXT_AND_STYLES,
                       true,
                       Pixel::RGBA8888);

    const Text::MarqueeTextureAnchor textureAnchor =
      typesetter->ResolveMarqueeTextureAnchor(staticAnchor);
    DALI_TEST_CHECK(textureAnchor.valid);

    const float horizontalAlignment =
      Text::ResolveHorizontalMarqueeAlignment(true,
                                              controller->GetMarqueeTextDirection(),
                                              alignment);
    const Text::MarqueeInitialDelta initialDelta =
      Text::ResolveMarqueeInitialDelta(staticAnchor,
                                       textureAnchor,
                                       horizontalAlignment,
                                       naturalWidth + wrapGap,
                                       controlWidth,
                                       wrapGap);
    DALI_TEST_CHECK(initialDelta.valid);
    const float viewportOrigin =
      Text::ResolveLegacyHorizontalMarqueeViewportOrigin(horizontalAlignment,
                                                         naturalWidth + wrapGap,
                                                         controlWidth,
                                                         wrapGap);
    DALI_TEST_EQUALS(CalculateMarqueeAnchorControlX(textureAnchor.textureX,
                                                    viewportOrigin,
                                                    initialDelta.value),
                     staticAnchor.staticControlX,
                     0.01f,
                     TEST_LOCATION);
  };

  checkActualSyncTransition(ltrCenter.first,
                            ltrCenter.second,
                            Text::Alignment::CENTER,
                            Dali::LayoutDirection::LEFT_TO_RIGHT);
  checkActualSyncTransition(rtlEnd.first,
                            rtlEnd.second,
                            Text::Alignment::END,
                            Dali::LayoutDirection::RIGHT_TO_LEFT);

  Text::AsyncTextParameters asyncParameters = MakeAsyncEndParameters(ltrText, 120.0f, 40.0f);
  asyncParameters.horizontalAlignment       = Text::Alignment::END;
  Text::AsyncTextLoader     loader           = Text::AsyncTextLoader::New();
  Text::AsyncTextRenderInfo asyncInfo        = loader.RenderText(asyncParameters, false, Size::ZERO);
  DALI_TEST_CHECK(asyncInfo.isMarqueeStartAnchorResolved);
  DALI_TEST_CHECK(asyncInfo.marqueeStartAnchor.valid);
  DALI_TEST_CHECK(std::isfinite(asyncInfo.marqueeStartAnchor.staticControlX));
  DALI_TEST_CHECK(asyncInfo.isMarqueeFittingStartGeometryResolved);
  DALI_TEST_CHECK(!asyncInfo.marqueeFittingStartGeometry.valid);

  Text::AsyncTextParameters scaledParameters = MakeAsyncEndParameters(ltrText, 120.0f, 40.0f);
  scaledParameters.horizontalAlignment       = Text::Alignment::END;
  scaledParameters.renderScale               = 2.0f;
  bool       cachedNaturalSize               = false;
  const Size scaledNaturalSize               = loader.SetupRenderScale(scaledParameters, cachedNaturalSize);
  Text::AsyncTextRenderInfo scaledInfo =
    loader.RenderText(scaledParameters, cachedNaturalSize, scaledNaturalSize);
  DALI_TEST_CHECK(scaledInfo.isMarqueeStartAnchorResolved);
  DALI_TEST_CHECK(scaledInfo.marqueeStartAnchor.valid);
  DALI_TEST_CHECK(std::isfinite(scaledInfo.marqueeStartAnchor.staticControlX));
  DALI_TEST_EQUALS(scaledInfo.marqueeStartAnchor.staticControlX,
                   asyncInfo.marqueeStartAnchor.staticControlX,
                   0.05f,
                   TEST_LOCATION);

  const auto checkAsyncTransition = [&](const Text::AsyncTextRenderInfo& staticInfo,
                                        float                            renderScale)
  {
    Text::AsyncTextParameters marqueeParameters = MakeAsyncEndParameters(ltrText, 120.0f, 40.0f);
    marqueeParameters.horizontalAlignment       = Text::Alignment::END;
    marqueeParameters.renderScale               = renderScale;
    marqueeParameters.maxTextureSize             = 2048;
    marqueeParameters.marqueeGap                 = 20;
    marqueeParameters.isMarqueeEnabled           = true;
    marqueeParameters.marqueeOrientation         = Text::MarqueeOrientation::HORIZONTAL;
    marqueeParameters.marqueeStartAnchor         = staticInfo.marqueeStartAnchor;

    Text::AsyncTextLoader marqueeLoader = Text::AsyncTextLoader::New();
    bool                  cached        = false;
    Size                  naturalSize   = Size::ZERO;
    if(renderScale > 1.0f)
    {
      naturalSize = marqueeLoader.SetupRenderScale(marqueeParameters, cached);
    }
    const Text::AsyncTextRenderInfo marqueeInfo =
      marqueeLoader.RenderMarquee(marqueeParameters, cached, naturalSize);
    DALI_TEST_CHECK(marqueeInfo.isMarqueeContentOverflow);
    DALI_TEST_CHECK(marqueeInfo.marqueeTextureAnchor.valid);

    const float horizontalAlignment =
      Text::ResolveHorizontalMarqueeAlignment(true,
                                              marqueeInfo.isTextDirectionRTL,
                                              Text::Alignment::END);
    const Text::MarqueeInitialDelta initialDelta =
      Text::ResolveMarqueeInitialDelta(staticInfo.marqueeStartAnchor,
                                       marqueeInfo.marqueeTextureAnchor,
                                       horizontalAlignment,
                                       marqueeInfo.size.width,
                                       marqueeInfo.controlSize.width,
                                       marqueeInfo.marqueeWrapGap);
    DALI_TEST_CHECK(initialDelta.valid);
    const float viewportOrigin =
      Text::ResolveLegacyHorizontalMarqueeViewportOrigin(horizontalAlignment,
                                                         marqueeInfo.size.width,
                                                         marqueeInfo.controlSize.width,
                                                         marqueeInfo.marqueeWrapGap);
    DALI_TEST_EQUALS(CalculateMarqueeAnchorControlX(marqueeInfo.marqueeTextureAnchor.textureX,
                                                    viewportOrigin,
                                                    initialDelta.value),
                     staticInfo.marqueeStartAnchor.staticControlX,
                     0.01f,
                     TEST_LOCATION);
  };

  checkAsyncTransition(asyncInfo, 1.0f);
  checkAsyncTransition(scaledInfo, 2.0f);

  Text::AsyncTextParameters fittingParameters = MakeAsyncEndParameters("Short text", 400.0f, 40.0f);
  Text::AsyncTextRenderInfo fittingInfo        = loader.RenderText(fittingParameters, false, Size::ZERO);
  DALI_TEST_CHECK(fittingInfo.isMarqueeStartAnchorResolved);
  DALI_TEST_CHECK(!fittingInfo.marqueeStartAnchor.valid);
  DALI_TEST_CHECK(fittingInfo.isMarqueeFittingStartGeometryResolved);
  DALI_TEST_CHECK(fittingInfo.marqueeFittingStartGeometry.valid);

  END_TEST;
}

int UtcDaliFittingMarqueeStartGeometryP(void)
{
  UiTestApplication application;

  const auto configure = [](const Text::ControllerPtr&  controller,
                            Text::Alignment             alignment,
                            Dali::LayoutDirection::Type layoutDirection,
                            const Size&                 size)
  {
    controller->SetHorizontalAlignment(alignment);
    controller->SetLayoutDirection(layoutDirection);
    controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
    controller->Relayout(size, layoutDirection);
  };

  const auto checkSyncTransition = [&](const std::string&          text,
                                       Text::Alignment             alignment,
                                       Dali::LayoutDirection::Type layoutDirection)
  {
    constexpr float height  = 40.0f;
    constexpr float wrapGap = 20.0f;

    Text::ControllerPtr naturalController = MakeEndController(text, 1000.0f, height);
    configure(naturalController, alignment, layoutDirection, Size(1000.0f, height));
    const float naturalWidth = naturalController->GetNaturalSize().width;
    const float controlWidth = naturalWidth + 21.0f; // Odd fitting remainder exercises half-pixel CENTER.

    Text::ControllerPtr staticController = MakeEndController(text, controlWidth, height);
    configure(staticController, alignment, layoutDirection, Size(controlWidth, height));
    const Text::ModelInterface*             staticModel = staticController->GetRenderTextModel();
    const Text::MarqueeFittingStartGeometry staticGeometry =
      Text::ResolveMarqueeFittingStartGeometry(staticModel);
    DALI_TEST_CHECK(staticGeometry.valid);
    DALI_TEST_EQUALS(staticModel->GetNumberOfLines(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(staticGeometry.staticTranslation,
                     static_cast<float>(static_cast<int32_t>(staticModel->GetLines()[0].alignmentOffset)),
                     0.001f,
                     TEST_LOCATION);

    const Text::Length staticGlyphCount = staticModel->GetNumberOfGlyphs();
    DALI_TEST_CHECK(staticGlyphCount > 0u);
    Vector<Vector2> staticPositions;
    staticPositions.Resize(staticGlyphCount);
    std::copy(staticModel->GetLayout(),
              staticModel->GetLayout() + staticGlyphCount,
              staticPositions.Begin());

    Text::ControllerPtr marqueeController = MakeEndController(text, naturalWidth, height);
    configure(marqueeController, alignment, layoutDirection, Size(naturalWidth, height));
    marqueeController->SetMarqueeEnabled(true, false, Text::MarqueeOrientation::HORIZONTAL);
    marqueeController->Relayout(Size(naturalWidth, height), layoutDirection);
    const Text::ModelInterface* marqueeModel = marqueeController->GetRenderTextModel();
    DALI_TEST_EQUALS(marqueeModel->GetNumberOfGlyphs(), staticGlyphCount, TEST_LOCATION);

    // Fitting keeps source topology rigid; only the static line translation is
    // absent from the natural-width marquee texture.
    const Vector2* marqueePositions = marqueeModel->GetLayout();
    DALI_TEST_CHECK(marqueePositions);
    for(Text::GlyphIndex glyphIndex = 0u; glyphIndex < staticGlyphCount; ++glyphIndex)
    {
      DALI_TEST_EQUALS(marqueePositions[glyphIndex].x,
                       staticPositions[glyphIndex].x,
                       0.01f,
                       TEST_LOCATION);
    }

    const float textureWidth = naturalWidth + wrapGap;
    const bool  direction    = layoutDirection == Dali::LayoutDirection::RIGHT_TO_LEFT;
    const float horizontalAlignment =
      Text::ResolveHorizontalMarqueeAlignment(false, direction, alignment);
    const Text::MarqueeInitialDelta initialDelta =
      Text::ResolveMarqueeFittingInitialDelta(staticGeometry,
                                              horizontalAlignment,
                                              textureWidth,
                                              controlWidth,
                                              wrapGap);
    DALI_TEST_CHECK(initialDelta.valid);
    const float viewportOrigin =
      Text::ResolveLegacyHorizontalMarqueeViewportOrigin(horizontalAlignment,
                                                         textureWidth,
                                                         controlWidth,
                                                         wrapGap);
    const float staticControlX  = staticPositions[0u].x + staticGeometry.staticTranslation;
    const float marqueeControlX = marqueePositions[0u].x - viewportOrigin - initialDelta.value;
    DALI_TEST_EQUALS(marqueeControlX, staticControlX, 0.01f, TEST_LOCATION);
  };

  const std::string ltrText = "Fitting marquee text";
  const std::string rtlText = "\xD7\x98\xD7\xA7\xD7\xA1\xD7\x98 \xD7\x9E\xD7\xAA\xD7\x90\xD7\x99\xD7\x9D \xD7\x9C\xD7\x9E\xD7\xA8\xD7\xA7\xD7\x99";
  for(Text::Alignment alignment : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END})
  {
    checkSyncTransition(ltrText, alignment, Dali::LayoutDirection::LEFT_TO_RIGHT);
    checkSyncTransition(rtlText, alignment, Dali::LayoutDirection::RIGHT_TO_LEFT);
  }
  checkSyncTransition("English \xD7\xA2\xD7\x91\xD7\xA8\xD7\x99\xD7\xAA mixed fitting text",
                      Text::Alignment::CENTER,
                      Dali::LayoutDirection::RIGHT_TO_LEFT);

  const auto checkAsyncTransition = [](float renderScale)
  {
    Text::AsyncTextParameters staticParameters = MakeAsyncEndParameters("Async fitting marquee", 401.0f, 40.0f);
    staticParameters.horizontalAlignment       = Text::Alignment::CENTER;
    staticParameters.renderScale               = renderScale;

    Text::AsyncTextLoader staticLoader      = Text::AsyncTextLoader::New();
    bool                  staticCached      = false;
    Size                  staticNaturalSize = Size::ZERO;
    if(renderScale > 1.0f)
    {
      staticNaturalSize = staticLoader.SetupRenderScale(staticParameters, staticCached);
    }
    const Text::AsyncTextRenderInfo staticInfo =
      staticLoader.RenderText(staticParameters, staticCached, staticNaturalSize);
    DALI_TEST_CHECK(staticInfo.isMarqueeFittingStartGeometryResolved);
    DALI_TEST_CHECK(staticInfo.marqueeFittingStartGeometry.valid);

    Text::AsyncTextParameters marqueeParameters = MakeAsyncEndParameters("Async fitting marquee", 401.0f, 40.0f);
    marqueeParameters.horizontalAlignment       = Text::Alignment::CENTER;
    marqueeParameters.renderScale               = renderScale;
    marqueeParameters.maxTextureSize            = 2048;
    marqueeParameters.marqueeGap                = 20;
    marqueeParameters.isMarqueeEnabled          = true;
    marqueeParameters.marqueeOrientation        = Text::MarqueeOrientation::HORIZONTAL;

    Text::AsyncTextLoader marqueeLoader      = Text::AsyncTextLoader::New();
    bool                  marqueeCached      = false;
    Size                  marqueeNaturalSize = Size::ZERO;
    if(renderScale > 1.0f)
    {
      marqueeNaturalSize = marqueeLoader.SetupRenderScale(marqueeParameters, marqueeCached);
    }
    const Text::AsyncTextRenderInfo marqueeInfo =
      marqueeLoader.RenderMarquee(marqueeParameters, marqueeCached, marqueeNaturalSize);
    DALI_TEST_CHECK(!marqueeInfo.isMarqueeContentOverflow);

    const float horizontalAlignment =
      Text::ResolveHorizontalMarqueeAlignment(false,
                                              marqueeInfo.isTextDirectionRTL,
                                              Text::Alignment::CENTER);
    const Text::MarqueeInitialDelta initialDelta =
      Text::ResolveMarqueeFittingInitialDelta(staticInfo.marqueeFittingStartGeometry,
                                              horizontalAlignment,
                                              marqueeInfo.size.width,
                                              marqueeInfo.controlSize.width,
                                              marqueeInfo.marqueeWrapGap);
    DALI_TEST_CHECK(initialDelta.valid);
    const float viewportOrigin =
      Text::ResolveLegacyHorizontalMarqueeViewportOrigin(horizontalAlignment,
                                                         marqueeInfo.size.width,
                                                         marqueeInfo.controlSize.width,
                                                         marqueeInfo.marqueeWrapGap);
    DALI_TEST_EQUALS(-viewportOrigin - initialDelta.value,
                     staticInfo.marqueeFittingStartGeometry.staticTranslation,
                     0.01f,
                     TEST_LOCATION);
  };

  checkAsyncTransition(1.0f);
  checkAsyncTransition(2.0f);

  // Multiline geometry is ambiguous and must retain the legacy zero-delta fallback.
  Text::ControllerPtr multiline = MakeEndController("first line second line", 70.0f, 100.0f, true);
  DALI_TEST_CHECK(!Text::ResolveMarqueeFittingStartGeometry(multiline->GetRenderTextModel()).valid);

  END_TEST;
}

int UtcDaliEndEllipsisOwnershipAndParityP(void)
{
  UiTestApplication application;

  const std::string       text       = "Multiline text now shares one final END ellipsis result";
  Text::ControllerPtr     controller = MakeEndController(text, 100.0f, 35.0f);
  Text::Controller::Impl& impl       = Text::Controller::Impl::GetImplementation(*controller.Get());
  DALI_TEST_CHECK(!impl.HasReplacementData());
  DALI_TEST_CHECK(!controller->HasValidReplacementSource());
  DALI_TEST_CHECK(controller->GetRenderTextModel() == controller->GetLogicalTextModel());
  const Text::FinalElisionResult* syncFinal = controller->GetFinalElisionResult();
  DALI_TEST_CHECK(syncFinal);
  CheckGeneratedEllipsis(*syncFinal, 100.0f);

  Vector<Text::GlyphInfo> viewGlyphs;
  Vector<Vector2>         viewPositions;
  const Text::Length      finalGlyphCount = static_cast<Text::Length>(syncFinal->glyphs.Count());
  viewGlyphs.Resize(finalGlyphCount);
  viewPositions.Resize(finalGlyphCount);
  float viewMinimumLineOffset = 0.0f;
  const Text::Length viewCount = controller->GetView().GetGlyphs(viewGlyphs.Begin(),
                                                                 viewPositions.Begin(),
                                                                 viewMinimumLineOffset,
                                                                 0u,
                                                                 finalGlyphCount);
  DALI_TEST_EQUALS(viewCount, finalGlyphCount, TEST_LOCATION);
  for(Text::GlyphIndex index = 0u; index < viewCount; ++index)
  {
    DALI_TEST_EQUALS(viewGlyphs[index].fontId, syncFinal->glyphs[index].fontId, TEST_LOCATION);
    DALI_TEST_EQUALS(viewGlyphs[index].index, syncFinal->glyphs[index].index, TEST_LOCATION);
    DALI_TEST_EQUALS(viewPositions[index], syncFinal->viewGlyphPositions[index], TEST_LOCATION);
  }

  Text::AsyncTextParameters parameters = MakeAsyncEndParameters(text, 100.0f, 35.0f);
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  const Text::AsyncTextRenderInfo asyncInfo = loader.RenderText(parameters, false, Size::ZERO);
  DALI_TEST_EQUALS(asyncInfo.lineCount, static_cast<int>(syncFinal->lines.Count()), TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.isTextDirectionRTL, syncFinal->lines[0u].direction, TEST_LOCATION);

  Text::TypesetterPtr syncTypesetter = Text::Typesetter::New(controller->GetRenderTextModel());
  syncTypesetter->SetFinalElisionResult(syncFinal);
  const PixelData syncPixels = syncTypesetter->Render(asyncInfo.size,
                                                      syncFinal->lines[0u].direction
                                                        ? Text::Direction::RIGHT_TO_LEFT
                                                        : Text::Direction::LEFT_TO_RIGHT,
                                                      Text::Typesetter::RENDER_NO_STYLES,
                                                      false,
                                                      Pixel::L8);
  DALI_TEST_CHECK(HaveSamePixels(syncPixels, asyncInfo.textPixelData));

  END_TEST;
}

int UtcDaliEndEllipsisOmissionP(void)
{
  UiTestApplication application;

  Text::ControllerPtr noVisibleLine = MakeEndController("No visible line can own an ellipsis", 1.0f, 24.0f, true);
  Text::Controller::Impl& noVisibleLineImpl =
    Text::Controller::Impl::GetImplementation(*noVisibleLine.Get());
  const Text::FinalElisionResult* noVisibleLineResult = noVisibleLine->GetFinalElisionResult();
  DALI_TEST_CHECK(noVisibleLineResult);
  DALI_TEST_CHECK(noVisibleLineImpl.mModel->mVisualModel->mLines.Empty());
  DALI_TEST_CHECK(noVisibleLineResult->HasAuthoritativeLayout());
  DALI_TEST_CHECK(!noVisibleLineResult->applied);
  DALI_TEST_EQUALS(noVisibleLineResult->ellipsisUnitCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(noVisibleLineResult->ellipsisOmissionReason,
                   Text::FinalElisionResult::EllipsisOmissionReason::NO_VISIBLE_LINE,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(noVisibleLineResult->ellipsisFinalGlyphIndex,
                   Text::FinalElisionResult::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(CountGeneratedGlyphs(*noVisibleLineResult), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(noVisibleLine->GetView().GetNumberOfGlyphs(),
                   noVisibleLineResult->glyphs.Count(),
                   TEST_LOCATION);

  Text::AsyncTextParameters parameters = MakeAsyncEndParameters("No visible line can own an ellipsis",
                                                                1.0f,
                                                                24.0f,
                                                                true);
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  const Text::AsyncTextRenderInfo asyncInfo = loader.RenderText(parameters, false, Size::ZERO);
  DALI_TEST_EQUALS(asyncInfo.lineCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.replacementPlacements.Count(), 0u, TEST_LOCATION);

  Text::ControllerPtr cannotFit = Text::Controller::New();
  cannotFit->SetText("i i i i i");
  cannotFit->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  cannotFit->SetCharacterSpacing(-5.0f);
  cannotFit->SetMultiLineEnabled(true);
  cannotFit->SetLineWrapMode(Text::LineWrapMode::WORD);
  cannotFit->SetTextElideEnabled(true);
  cannotFit->SetEllipsisPosition(Text::EllipsisPosition::END);
  cannotFit->Relayout(Size(2.0f, 35.0f));
  Text::Controller::Impl& cannotFitImpl =
    Text::Controller::Impl::GetImplementation(*cannotFit.Get());
  const Text::VisualModel& cannotFitSource = *cannotFitImpl.mModel->mVisualModel;
  DALI_TEST_CHECK(!cannotFitSource.mLines.Empty());
  const auto ellipsisLine = std::find_if(cannotFitSource.mLines.Begin(),
                                         cannotFitSource.mLines.End(),
                                         [](const Text::LineRun& line) { return line.ellipsis; });
  DALI_TEST_CHECK(ellipsisLine != cannotFitSource.mLines.End());
  DALI_TEST_CHECK(ellipsisLine->glyphRun.numberOfGlyphs > 0u);
  DALI_TEST_CHECK(ellipsisLine->characterRun.numberOfCharacters > 0u);
  const Text::FinalElisionResult* cannotFitResult = cannotFit->GetFinalElisionResult();
  DALI_TEST_CHECK(cannotFitResult);
  DALI_TEST_CHECK(cannotFitResult->HasAuthoritativeLayout());
  DALI_TEST_CHECK(!cannotFitResult->applied);
  DALI_TEST_EQUALS(cannotFitResult->ellipsisUnitCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(cannotFitResult->ellipsisOmissionReason,
                   Text::FinalElisionResult::EllipsisOmissionReason::ELLIPSIS_CANNOT_FIT,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(cannotFitResult->ellipsisFinalGlyphIndex,
                   Text::FinalElisionResult::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(CountGeneratedGlyphs(*cannotFitResult), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(cannotFit->GetView().GetNumberOfGlyphs(), cannotFitResult->glyphs.Count(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliEndEllipsisPlaceholderOwnershipP(void)
{
  UiTestApplication application;

  // Placeholder ellipsis is an effective controller state independent of the
  // stored text-elide flag. It must still publish the authoritative END result.
  Text::ControllerPtr placeholderController = Text::Controller::New();
  Text::DecoratorPtr  decorator             = Text::Decorator::New(*placeholderController, *placeholderController);
  Dali::InputMethodContext inputMethodContext;
  placeholderController->EnableTextInput(decorator, inputMethodContext);
  placeholderController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE,
                                            "Placeholder text must use one authoritative END ellipsis producer");
  placeholderController->SetPlaceholderTextElideEnabled(true);
  placeholderController->SetTextElideEnabled(false);
  placeholderController->SetEllipsisPosition(Text::EllipsisPosition::END);
  placeholderController->SetMultiLineEnabled(false);
  placeholderController->Relayout(Size(100.0f, 35.0f));

  Text::Controller::Impl& placeholderImpl =
    Text::Controller::Impl::GetImplementation(*placeholderController.Get());
  DALI_TEST_CHECK(placeholderImpl.IsShowingPlaceholderText());
  DALI_TEST_CHECK(!placeholderImpl.mModel->mElideEnabled);
  DALI_TEST_CHECK(placeholderImpl.mEventData->mIsPlaceholderElideEnabled);
  const Text::FinalElisionResult* placeholderFinal = placeholderController->GetFinalElisionResult();
  DALI_TEST_CHECK(placeholderFinal);

  // Publishing the authoritative owner must preserve the standalone View
  // fallback output for this effective-placeholder state.
  placeholderImpl.mView.SetFinalElisionResult(nullptr);
  const Text::Length standaloneCapacity = placeholderImpl.mView.GetNumberOfGlyphs();
  Vector<Text::GlyphInfo> standaloneGlyphs;
  Vector<Vector2>         standalonePositions;
  standaloneGlyphs.Resize(standaloneCapacity);
  standalonePositions.Resize(standaloneCapacity);
  float standaloneMinimumLineOffset = 0.0f;
  const Text::Length standaloneCount = placeholderImpl.mView.GetGlyphs(standaloneGlyphs.Begin(),
                                                                       standalonePositions.Begin(),
                                                                       standaloneMinimumLineOffset,
                                                                       0u,
                                                                       standaloneCapacity);
  placeholderImpl.mView.SetFinalElisionResult(placeholderFinal);

  DALI_TEST_EQUALS(standaloneCount, placeholderFinal->glyphs.Count(), TEST_LOCATION);
  for(Text::GlyphIndex index = 0u; index < standaloneCount; ++index)
  {
    DALI_TEST_EQUALS(standaloneGlyphs[index].fontId, placeholderFinal->glyphs[index].fontId, TEST_LOCATION);
    DALI_TEST_EQUALS(standaloneGlyphs[index].index, placeholderFinal->glyphs[index].index, TEST_LOCATION);
    DALI_TEST_EQUALS(standalonePositions[index].x,
                     placeholderFinal->viewGlyphPositions[index].x,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(standalonePositions[index].y,
                     placeholderFinal->viewGlyphPositions[index].y,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliEndEllipsisFinalShapeAndClusterP(void)
{
  UiTestApplication application;

  struct Case
  {
    const char* text;
    float       width;
    float       characterSpacing;
  };
  const Case cases[] = {
    {"cafe\xCC\x81 cafe\xCC\x81 cafe\xCC\x81", 105.0f, 1.0f},
    {"بببببببببببببببب", 90.0f, 0.0f},
  };

  for(const Case& testCase : cases)
  {
    Text::ControllerPtr controller = Text::Controller::New();
    controller->SetText(testCase.text);
    controller->SetDefaultFontSize(22.0f, Text::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(false);
    controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
    controller->SetCharacterSpacing(testCase.characterSpacing);
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(Text::EllipsisPosition::END);
    controller->SetHorizontalAlignment(Text::Alignment::CENTER);
    controller->Relayout(Size(testCase.width, 50.0f));

    Text::Controller::Impl&         impl  = Text::Controller::Impl::GetImplementation(*controller.Get());
    const Text::FinalElisionResult* final = controller->GetFinalElisionResult();
    DALI_TEST_CHECK(final);
    CheckGeneratedEllipsis(*final, testCase.width);
    const Text::LineRun& finalLine = final->lines[final->ellipsisLineIndex];

    const Text::VisualModel&   sourceVisual = *impl.mModel->mVisualModel;
    const Text::CharacterIndex keptEnd      = finalLine.characterRun.characterIndex +
                                         finalLine.characterRun.numberOfCharacters;
    DALI_TEST_CHECK(keptEnd < impl.mModel->mLogicalModel->mText.Count());
    if(keptEnd > 0u)
    {
      DALI_TEST_CHECK(sourceVisual.mCharactersToGlyph[keptEnd - 1u] !=
                      sourceVisual.mCharactersToGlyph[keptEnd]);
    }
    const Text::GlyphIndex firstRemovedGlyph = sourceVisual.mCharactersToGlyph[keptEnd];
    DALI_TEST_CHECK(firstRemovedGlyph < final->sourceToFinalGlyphIndices.Count());
    DALI_TEST_EQUALS(final->sourceToFinalGlyphIndices[firstRemovedGlyph],
                     Text::FinalElisionResult::INVALID_GLYPH_INDEX,
                     TEST_LOCATION);

    // END projects source-shaped glyphs. U+2026 is generated once,
    // but never changes the authored glyph identities or their joining
    // context by re-shaping a shortened paragraph.
    for(Text::GlyphIndex finalGlyph = finalLine.glyphRun.glyphIndex;
        finalGlyph < finalLine.glyphRun.glyphIndex + finalLine.glyphRun.numberOfGlyphs;
        ++finalGlyph)
    {
      if(finalGlyph == final->ellipsisFinalGlyphIndex)
      {
        continue;
      }
      const Text::GlyphIndex sourceGlyph = final->finalToSourceGlyphIndices[finalGlyph];
      DALI_TEST_CHECK(sourceGlyph < sourceVisual.mGlyphs.Count());
      DALI_TEST_EQUALS(final->glyphs[finalGlyph].fontId, sourceVisual.mGlyphs[sourceGlyph].fontId, TEST_LOCATION);
      DALI_TEST_EQUALS(final->glyphs[finalGlyph].index, sourceVisual.mGlyphs[sourceGlyph].index, TEST_LOCATION);
      DALI_TEST_EQUALS(final->glyphs[finalGlyph].advance,
                       sourceVisual.mGlyphs[sourceGlyph].advance,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
    }
    const Vector2& ellipsisLocal = final->lineLocalGlyphPositions[final->ellipsisFinalGlyphIndex];
    const Vector2& ellipsisView  = final->viewGlyphPositions[final->ellipsisFinalGlyphIndex];
    DALI_TEST_EQUALS(ellipsisView.x - ellipsisLocal.x,
                     finalLine.alignmentOffset,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliEndEllipsisRepresentativeBoundariesP(void)
{
  UiTestApplication application;

  struct Probe
  {
    const char*                 text;
    float                       width;
    Dali::LayoutDirection::Type direction;
  };
  const Probe probes[] = {
    {"alpha beta gamma delta", 100.0f, LayoutDirection::LEFT_TO_RIGHT},
    {"עברית חושפת מילים", 100.0f, LayoutDirection::RIGHT_TO_LEFT},
    {"אבג 123 abcdef", 97.0f, LayoutDirection::LEFT_TO_RIGHT},  // #050
    {"abc 123 אבגדה", 80.0f, LayoutDirection::RIGHT_TO_LEFT}, // #055
  };

  for(const Probe& probe : probes)
  {
    const Dali::LayoutDirection::Type forcedDirection = probe.direction;
    const float                       width           = probe.width;
    Text::ControllerPtr               controller      = Text::Controller::New();
    controller->SetText(probe.text);
    controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(false);
    controller->SetLayoutDirection(forcedDirection);
    controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(Text::EllipsisPosition::END);
    controller->Relayout(Size(width, 44.0f), forcedDirection);

    const Text::FinalElisionResult* final = controller->GetFinalElisionResult();
    DALI_TEST_CHECK(final);
    CheckEndReplaceContract(controller, *final, width);
  }
  END_TEST;
}

int UtcDaliEndEllipsisHistoricalPhysicalTopologyP(void)
{
  UiTestApplication application;

  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText("אבג 123 abcdef"); // #050
  controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(false);
  controller->SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
  controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(97.0f, 44.0f), LayoutDirection::LEFT_TO_RIGHT);

  const Text::FinalElisionResult* baselinePtr = controller->GetFinalElisionResult();
  DALI_TEST_CHECK(baselinePtr);
  CheckEndReplaceContract(controller, *baselinePtr, 97.0f);
  const Text::FinalElisionResult baseline = *baselinePtr;

  Text::Controller::Impl& impl   = Text::Controller::Impl::GetImplementation(*controller.Get());
  Text::VisualModel&      source = *impl.mModel->mVisualModel;
  const Text::LineRun&    line   = source.mLines[baseline.ellipsisLineIndex];
  const Text::CharacterIndex retainedEnd = GetRetainedCharacterEnd(baseline);
  const Text::GlyphIndex removedGlyph = source.mCharactersToGlyph[retainedEnd];
  const float removedPen = source.mGlyphPositions[removedGlyph].x - source.mGlyphs[removedGlyph].xBearing;
  const float baselineEllipsisPen = baseline.lineLocalGlyphPositions[baseline.ellipsisFinalGlyphIndex].x -
                                    baseline.glyphs[baseline.ellipsisFinalGlyphIndex].xBearing;

  Text::GlyphIndex partnerGlyph = Text::FinalElisionResult::INVALID_GLYPH_INDEX;
  float            partnerPen   = removedPen;
  float            greatestDistance{0.0f};
  const Text::GlyphIndex lineEnd = line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs;
  for(Text::GlyphIndex glyph = line.glyphRun.glyphIndex; glyph < lineEnd; ++glyph)
  {
    if(glyph == removedGlyph || glyph >= source.mCharactersPerGlyph.Count() ||
       source.mCharactersPerGlyph[glyph] != 1u ||
       glyph >= baseline.sourceToFinalGlyphIndices.Count() ||
       baseline.sourceToFinalGlyphIndices[glyph] == Text::FinalElisionResult::INVALID_GLYPH_INDEX)
    {
      continue;
    }
    const float pen      = source.mGlyphPositions[glyph].x - source.mGlyphs[glyph].xBearing;
    const float distance = std::fabs(pen - removedPen);
    if(distance > greatestDistance)
    {
      greatestDistance = distance;
      partnerGlyph     = glyph;
      partnerPen       = pen;
    }
  }
  DALI_TEST_CHECK(partnerGlyph < source.mGlyphs.Count());
  DALI_TEST_CHECK(greatestDistance > 0.01f);

  source.mGlyphPositions[removedGlyph].x = partnerPen + source.mGlyphs[removedGlyph].xBearing;
  source.mGlyphPositions[partnerGlyph].x = removedPen + source.mGlyphs[partnerGlyph].xBearing;

  Text::FinalElisionResult reordered;
  DALI_TEST_CHECK(Text::ResolveEndEllipsis(*impl.mModel,
                                           Size(97.0f, 44.0f),
                                           impl.GetFontClient(),
                                           reordered));
  Text::FinalizeEndEllipsisGeometry(*impl.mModel,
                                    Size(97.0f, 44.0f),
                                    LayoutDirection::LEFT_TO_RIGHT,
                                    true,
                                    impl.mLayoutEngine,
                                    reordered);
  CheckEndReplaceContract(controller, reordered, 97.0f);
  DALI_TEST_EQUALS(GetRetainedCharacterEnd(reordered), retainedEnd, TEST_LOCATION);
  const float reorderedEllipsisPen = reordered.lineLocalGlyphPositions[reordered.ellipsisFinalGlyphIndex].x -
                                     reordered.glyphs[reordered.ellipsisFinalGlyphIndex].xBearing;
  DALI_TEST_CHECK(std::fabs(reorderedEllipsisPen - baselineEllipsisPen) > 0.01f);

  END_TEST;
}

int UtcDaliEndEllipsisAlignmentAndWidthBoundaryP(void)
{
  UiTestApplication application;

  struct Case
  {
    const char* text;
    float       characterSpacing;
  };
  const Case cases[] = {
    {"LTR office cafe\xCC\x81 tail sequence", 1.5f},
    {"עברית العربية 123 English tail", -0.5f},
  };
  const Text::Alignment alignments[] = {
    Text::Alignment::START,
    Text::Alignment::CENTER,
    Text::Alignment::END,
  };
  constexpr float width = 120.0f;

  for(const Case& testCase : cases)
  {
    for(Text::Alignment alignment : alignments)
    {
      Text::ControllerPtr controller = Text::Controller::New();
      controller->SetText(testCase.text);
      controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
      controller->SetMultiLineEnabled(false);
      controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
      controller->SetCharacterSpacing(testCase.characterSpacing);
      controller->SetTextElideEnabled(true);
      controller->SetEllipsisPosition(Text::EllipsisPosition::END);
      controller->SetHorizontalAlignment(alignment);
      controller->Relayout(Size(width, 44.0f));

      const Text::FinalElisionResult* final = controller->GetFinalElisionResult();
      DALI_TEST_CHECK(final);
      CheckGeneratedEllipsis(*final, width);
      DALI_TEST_EQUALS(final->lines.Count(), 1u, TEST_LOCATION);
      const Text::LineRun& line = final->lines[0u];

      const bool lineIsRtl      = line.direction;
      float      expectedOffset = 0.0f;
      if(alignment == Text::Alignment::START)
      {
        expectedOffset = lineIsRtl ? width - (line.width + line.extraLength) : 0.0f;
      }
      else if(alignment == Text::Alignment::CENTER)
      {
        expectedOffset = std::floor(0.5f * (width - line.width) - (lineIsRtl ? line.extraLength : 0.0f));
      }
      else
      {
        expectedOffset = lineIsRtl ? -line.extraLength : width - line.width;
      }
      DALI_TEST_EQUALS(line.alignmentOffset,
                       expectedOffset,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);

      for(Text::GlyphIndex glyph = 0u; glyph < final->glyphs.Count(); ++glyph)
      {
        DALI_TEST_EQUALS(final->viewGlyphPositions[glyph].x - final->lineLocalGlyphPositions[glyph].x,
                         expectedOffset,
                         Math::MACHINE_EPSILON_1000,
                         TEST_LOCATION);
      }
      DALI_TEST_EQUALS(final->finalToSourceGlyphIndices[final->ellipsisFinalGlyphIndex],
                       Text::FinalElisionResult::INVALID_GLYPH_INDEX,
                       TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliEndEllipsisFinalStyleDomainP(void)
{
  UiTestApplication application;

  const std::string       plainText       = "Styled END ellipsis keeps authored color ownership";
  const uint32_t          plainTextLength = static_cast<uint32_t>(plainText.size());
  Text::StyledTextBuilder builder         = Text::StyledTextBuilder::New(Dali::String(plainText.c_str()));
  DALI_TEST_CHECK(builder.SetSpan(Text::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED)),
                                  0u,
                                  16u));
  DALI_TEST_CHECK(builder.SetSpan(Text::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::BLUE)),
                                  16u,
                                  plainTextLength));
  Text::Underline underline;
  underline.SetColor(Dali::Ui::UiColor(Color::GREEN));
  underline.SetThickness(2.0f);
  DALI_TEST_CHECK(builder.SetSpan(Text::UnderlineSpan::New(underline), 16u, plainTextLength));
  DALI_TEST_CHECK(builder.SetSpan(Text::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::YELLOW)),
                                  16u,
                                  plainTextLength));

  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetStyledText(builder.Build());
  controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(false);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(130.0f, 44.0f));

  Text::Controller::Impl&         impl   = Text::Controller::Impl::GetImplementation(*controller.Get());
  const Text::VisualModel&        source = *impl.mModel->mVisualModel;
  const Text::FinalElisionResult* final  = controller->GetFinalElisionResult();
  DALI_TEST_CHECK(final);
  CheckGeneratedEllipsis(*final, 130.0f);
  DALI_TEST_EQUALS(final->colorIndices.Count(), final->glyphs.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(final->backgroundColorIndices.Count(), final->glyphs.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(final->finalToStyleGlyphIndices.Count(), final->glyphs.Count(), TEST_LOCATION);
  for(Text::GlyphIndex glyph = 0u; glyph < final->glyphs.Count(); ++glyph)
  {
    const Text::GlyphIndex styleGlyph = final->finalToStyleGlyphIndices[glyph];
    DALI_TEST_CHECK(styleGlyph < source.mGlyphs.Count());
    DALI_TEST_EQUALS(final->colorIndices[glyph], source.mColorIndices[styleGlyph], TEST_LOCATION);
    DALI_TEST_EQUALS(final->backgroundColorIndices[glyph],
                     source.mBackgroundColorIndices[styleGlyph],
                     TEST_LOCATION);
  }
  DALI_TEST_EQUALS(final->finalToSourceGlyphIndices[final->ellipsisFinalGlyphIndex],
                   Text::FinalElisionResult::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);
  const Text::GlyphIndex ellipsisStyleGlyph = final->finalToStyleGlyphIndices[final->ellipsisFinalGlyphIndex];
  DALI_TEST_CHECK(ellipsisStyleGlyph < source.mGlyphs.Count());

  // The retained prefix ends in the RED run, while the source layout's END
  // insertion boundary is in the first removed BLUE run. END keeps
  // that established insertion-point inheritance for every style consumer.
  const Text::CharacterIndex retainedEnd = final->lines[final->ellipsisLineIndex].characterRun.characterIndex +
                                           final->lines[final->ellipsisLineIndex].characterRun.numberOfCharacters;
  const Text::CharacterIndex ellipsisStyleCharacter = source.mGlyphsToCharacters[ellipsisStyleGlyph];
  DALI_TEST_CHECK(retainedEnd <= 16u);
  DALI_TEST_CHECK(ellipsisStyleCharacter >= 16u);
  DALI_TEST_EQUALS(final->colorIndices[final->ellipsisFinalGlyphIndex],
                   source.mColorIndices[ellipsisStyleGlyph],
                   TEST_LOCATION);
  DALI_TEST_EQUALS(final->backgroundColorIndices[final->ellipsisFinalGlyphIndex],
                   source.mBackgroundColorIndices[ellipsisStyleGlyph],
                   TEST_LOCATION);
  Vector<Text::UnderlinedGlyphRun>::ConstIterator underlineRun = source.mUnderlineRuns.End();
  DALI_TEST_CHECK(Text::IsGlyphUnderlined(ellipsisStyleGlyph, source.mUnderlineRuns, underlineRun));

  Text::TypesetterPtr typesetter = Text::Typesetter::New(controller->GetRenderTextModel());
  typesetter->SetFinalElisionResult(final);
  const Text::GlyphIndex* consumerStyleMap = typesetter->GetViewModel()->GetFinalGlyphStyleSourceIndices();
  DALI_TEST_CHECK(consumerStyleMap);
  DALI_TEST_EQUALS(consumerStyleMap[final->ellipsisFinalGlyphIndex], ellipsisStyleGlyph, TEST_LOCATION);
  const PixelData overlayPixels = typesetter->Render(Size(130.0f, 44.0f),
                                                      final->lines[0u].direction
                                                        ? Text::Direction::RIGHT_TO_LEFT
                                                        : Text::Direction::LEFT_TO_RIGHT,
                                                      Text::Typesetter::RENDER_OVERLAY_STYLE,
                                                      false,
                                                      Pixel::RGBA8888);
  DALI_TEST_CHECK(overlayPixels);
  if(retainedEnd > 0u)
  {
    const Text::GlyphIndex retainedStyleGlyph = source.mCharactersToGlyph[retainedEnd - 1u];
    DALI_TEST_CHECK(retainedStyleGlyph < source.mColorIndices.Count());
    DALI_TEST_CHECK(source.mColorIndices[retainedStyleGlyph] != source.mColorIndices[ellipsisStyleGlyph]);
  }

  END_TEST;
}

int UtcDaliEndEllipsisSourceImmutableAndStateP(void)
{
  UiTestApplication application;

  constexpr float     WIDTH      = 132.0f;
  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText("Immutable source geometry العربية office cafe\xCC\x81 trailing text");
  controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(false);
  controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(WIDTH, 44.0f));

  Text::Controller::Impl&          impl       = Text::Controller::Impl::GetImplementation(*controller.Get());
  const Text::VisualModel&         source     = *impl.mModel->mVisualModel;
  const VisualGeometrySnapshot     before     = SnapshotVisualGeometry(source);
  const Text::Layout::Engine::Type modeBefore = impl.mLayoutEngine.GetLayout();
  Text::FinalElisionResult         resolved;
  DALI_TEST_CHECK(Text::ResolveEndEllipsis(*impl.mModel,
                                           Size(WIDTH, 44.0f),
                                           impl.GetFontClient(),
                                           resolved));
  DALI_TEST_CHECK(IsSameVisualGeometry(before, source));
  DALI_TEST_EQUALS(impl.mLayoutEngine.GetLayout(), modeBefore, TEST_LOCATION);

  Text::BoundedParagraphRun boundedParagraph;
  boundedParagraph.characterRun               = {0u, static_cast<Text::Length>(impl.mModel->mLogicalModel->mText.Count())};
  boundedParagraph.horizontalAlignment        = Text::Alignment::CENTER;
  boundedParagraph.horizontalAlignmentDefined = true;
  impl.mModel->mLogicalModel->mBoundedParagraphRuns.PushBack(boundedParagraph);
  Text::FinalizeEndEllipsisGeometry(*impl.mModel,
                                    Size(WIDTH, 44.0f),
                                    LayoutDirection::LEFT_TO_RIGHT,
                                    false,
                                    impl.mLayoutEngine,
                                    resolved);
  DALI_TEST_CHECK(IsSameVisualGeometry(before, source));
  DALI_TEST_CHECK(resolved.HasAuthoritativeLayout());
  DALI_TEST_CHECK(HasGeneratedEllipsis(resolved));
  DALI_TEST_CHECK(resolved.layoutSize.height > 0.0f);
  const Text::LineRun& boundedFinalLine      = resolved.lines[resolved.ellipsisLineIndex];
  const float          expectedBoundedOffset = std::floor(0.5f * (WIDTH - boundedFinalLine.width) -
                                                          (boundedFinalLine.direction ? boundedFinalLine.extraLength : 0.0f));
  DALI_TEST_EQUALS(boundedFinalLine.alignmentOffset,
                   expectedBoundedOffset,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.glyphs.Count(), resolved.lineLocalGlyphPositions.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.glyphs.Count(), resolved.viewGlyphPositions.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.glyphs.Count(), resolved.finalToSourceGlyphIndices.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.glyphs.Count(), resolved.finalToStyleGlyphIndices.Count(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliEndEllipsisAppendContractP(void)
{
  UiTestApplication application;

  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText("first visible line with horizontal slack\n"
                      "second line\nthird line\nfourth line\nfifth line\nsixth line\n"
                      "seventh line\neighth line\nninth line\ntenth line");
  controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetLineWrapMode(Text::LineWrapMode::WORD);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(800.0f, 24.0f), LayoutDirection::LEFT_TO_RIGHT);

  const Text::FinalElisionResult* final = controller->GetFinalElisionResult();
  DALI_TEST_CHECK(final);
  CheckGeneratedEllipsis(*final, 800.0f);
  Text::Controller::Impl& impl       = Text::Controller::Impl::GetImplementation(*controller.Get());
  const Text::VisualModel& source     = *impl.mModel->mVisualModel;
  const Text::LineRun&     sourceLine = source.mLines[final->ellipsisLineIndex];
  const Text::LogicalModel& logical = *impl.mModel->mLogicalModel;
  const Text::CharacterIndex representedEnd = std::min<Text::CharacterIndex>(
    sourceLine.characterRun.characterIndex + sourceLine.characterRun.numberOfCharacters,
    static_cast<Text::CharacterIndex>(logical.mText.Count()));
  Text::CharacterIndex semanticEnd = sourceLine.characterRun.characterIndex;
  while(semanticEnd < representedEnd && !TextAbstraction::IsNewParagraph(logical.mText[semanticEnd]))
  {
    ++semanticEnd;
  }
  DALI_TEST_CHECK(semanticEnd < logical.mText.Count());

  const Text::GlyphIndex sourceLineEnd = sourceLine.glyphRun.glyphIndex + sourceLine.glyphRun.numberOfGlyphs;
  for(Text::GlyphIndex sourceGlyph = sourceLine.glyphRun.glyphIndex;
      sourceGlyph < sourceLineEnd;
      ++sourceGlyph)
  {
    if(source.mGlyphsToCharacters[sourceGlyph] >= semanticEnd)
    {
      continue;
    }
    DALI_TEST_CHECK(sourceGlyph < final->sourceToFinalGlyphIndices.Count());
    DALI_TEST_CHECK(final->sourceToFinalGlyphIndices[sourceGlyph] !=
                    Text::FinalElisionResult::INVALID_GLYPH_INDEX);
  }
  DALI_TEST_EQUALS(GetRetainedCharacterEnd(*final), semanticEnd, TEST_LOCATION);

  END_TEST;
}

int UtcDaliEndEllipsisHistoricalMixedLineMembershipP(void)
{
  UiTestApplication application;

  const char* text =
    "Character reveal keeps office ligatures and cafe\xCC\x81 combining sequences atomic.\n"
    "한국어 순차 표시와 한글 자모를 확인합니다.\n"
    "العربية تكشف الكلمات — mixed עברית 123.\n"
    "Emoji: 👩‍💻 👍🏽 👨‍👩‍👧‍👦";

  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText(text);
  controller->SetDefaultFontSize(28.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetLineWrapMode(Text::LineWrapMode::WORD);
  controller->SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(206.0f, 96.0f), LayoutDirection::RIGHT_TO_LEFT);

  const Text::Controller::Impl&   impl   = Text::Controller::Impl::GetImplementation(*controller.Get());
  const Text::FinalElisionResult* final  = controller->GetFinalElisionResult();
  DALI_TEST_CHECK(final);
  CheckGeneratedEllipsis(*final, 206.0f);
  const Text::LineRun& finalLine = final->lines[final->ellipsisLineIndex];
  const Text::CharacterIndex retainedEnd = finalLine.characterRun.characterIndex +
                                           finalLine.characterRun.numberOfCharacters;

  const Text::VisualModel& source = *impl.mModel->mVisualModel;
  for(Text::GlyphIndex finalGlyph = finalLine.glyphRun.glyphIndex;
      finalGlyph < finalLine.glyphRun.glyphIndex + finalLine.glyphRun.numberOfGlyphs;
      ++finalGlyph)
  {
    if(finalGlyph == final->ellipsisFinalGlyphIndex)
    {
      continue;
    }
    const Text::GlyphIndex sourceGlyph = final->finalToSourceGlyphIndices[finalGlyph];
    DALI_TEST_CHECK(sourceGlyph < source.mGlyphs.Count());
    DALI_TEST_CHECK(source.mGlyphsToCharacters[sourceGlyph] < retainedEnd);
  }

  END_TEST;
}

int UtcDaliEndEllipsisLifecycleP(void)
{
  UiTestApplication application;

  constexpr float     ACTIVE_WIDTH   = 112.0f;
  constexpr float     CONTROL_HEIGHT = 44.0f;
  const std::string   longText       = "Lifecycle updates must never retain stale END ellipsis geometry";
  Text::ControllerPtr controller     = MakeEndController(longText, ACTIVE_WIDTH, CONTROL_HEIGHT);
  Text::Controller::Impl& impl       = Text::Controller::Impl::GetImplementation(*controller.Get());
  LayoutDirection::Type  direction  = LayoutDirection::LEFT_TO_RIGHT;

  const Text::FinalElisionResult* initial = controller->GetFinalElisionResult();
  DALI_TEST_CHECK(initial);
  CheckGeneratedEllipsis(*initial, ACTIVE_WIDTH);
  const float initialAlignment = initial->lines[initial->ellipsisLineIndex].alignmentOffset;

  // Horizontal alignment must produce a valid END ellipsis result with
  // updated final-domain placement.
  controller->SetHorizontalAlignment(Text::Alignment::END);
  DALI_TEST_CHECK(controller->GetFinalElisionResult());
  controller->Relayout(Size(ACTIVE_WIDTH, CONTROL_HEIGHT));
  const Text::FinalElisionResult* aligned = controller->GetFinalElisionResult();
  DALI_TEST_CHECK(aligned);
  CheckGeneratedEllipsis(*aligned, ACTIVE_WIDTH);
  DALI_TEST_CHECK(aligned->lines[aligned->ellipsisLineIndex].alignmentOffset > initialAlignment);

  controller->Relayout(Size(800.0f, CONTROL_HEIGHT));
  DALI_TEST_CHECK(controller->GetFinalElisionResult() == nullptr);

  controller->Relayout(Size(ACTIVE_WIDTH, CONTROL_HEIGHT));
  const auto checkInvalidation = [&](const auto& change)
  {
    DALI_TEST_CHECK(controller->GetFinalElisionResult());
    change();
    DALI_TEST_CHECK(controller->GetFinalElisionResult() == nullptr);
    controller->Relayout(Size(ACTIVE_WIDTH, CONTROL_HEIGHT), direction);
    DALI_TEST_CHECK(controller->GetFinalElisionResult());
  };
  checkInvalidation([&] { controller->SetCharacterSpacing(1.0f); });
  checkInvalidation([&] { controller->SetDefaultFontSize(19.0f, Text::Controller::PIXEL_SIZE); });
  checkInvalidation([&] { impl.ClearFontData(); });
  checkInvalidation([&]
  {
    direction = LayoutDirection::RIGHT_TO_LEFT;
    controller->SetLayoutDirection(direction);
  });
  checkInvalidation([&] { controller->SetLineWrapMode(Text::LineWrapMode::CHARACTER); });
  checkInvalidation([&]
  {
    controller->SetMultiLineEnabled(true);
    controller->SetMultiLineEnabled(false);
  });
  checkInvalidation([&]
  {
    controller->SetMarqueeEnabled(true);
    controller->SetMarqueeEnabled(false);
  });
  checkInvalidation([&]
  {
    controller->SetText("A different long source also invalidates final ownership immediately");
  });

  DALI_TEST_CHECK(controller->GetFinalElisionResult());
  controller->SetEllipsisPosition(Text::EllipsisPosition::START);
  DALI_TEST_CHECK(controller->GetFinalElisionResult() == nullptr);
  controller->Relayout(Size(ACTIVE_WIDTH, CONTROL_HEIGHT), direction);
  DALI_TEST_CHECK(controller->GetFinalElisionResult() == nullptr);

  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->Relayout(Size(ACTIVE_WIDTH, CONTROL_HEIGHT), direction);
  DALI_TEST_CHECK(controller->GetFinalElisionResult());
  controller->SetTextElideEnabled(false);
  DALI_TEST_CHECK(controller->GetFinalElisionResult() == nullptr);
  DALI_TEST_EQUALS(controller->GetView().GetNumberOfGlyphs(),
                   impl.mModel->mVisualModel->mGlyphs.Count(),
                   TEST_LOCATION);

  // Reusing one async loader must produce the same inactive output as a fresh
  // loader after an active END render.
  Text::AsyncTextParameters activeParameters =
    MakeAsyncEndParameters(longText, ACTIVE_WIDTH, CONTROL_HEIGHT);
  Text::AsyncTextLoader reusedLoader = Text::AsyncTextLoader::New();
  const Text::AsyncTextRenderInfo activeInfo =
    reusedLoader.RenderText(activeParameters, false, Size::ZERO);
  DALI_TEST_CHECK(activeInfo.lineCount > 0);

  Text::AsyncTextParameters inactiveParameters =
    MakeAsyncEndParameters("Short", 400.0f, CONTROL_HEIGHT);
  const Text::AsyncTextRenderInfo reusedInactive =
    reusedLoader.RenderText(inactiveParameters, false, Size::ZERO);
  Text::AsyncTextLoader freshLoader = Text::AsyncTextLoader::New();
  const Text::AsyncTextRenderInfo freshInactive =
    freshLoader.RenderText(inactiveParameters, false, Size::ZERO);
  DALI_TEST_EQUALS(reusedInactive.lineCount, freshInactive.lineCount, TEST_LOCATION);
  DALI_TEST_EQUALS(reusedInactive.size, freshInactive.size, TEST_LOCATION);
  DALI_TEST_CHECK(HaveSamePixels(reusedInactive.textPixelData, freshInactive.textPixelData));

  // One Controller must switch final-result ownership cleanly across
  // non-replacement END -> ImageSpan END -> non-replacement END.
  constexpr float OWNERSHIP_WIDTH = 112.0f;
  const std::string ownershipText =
    "Ownership transitions keep one final END producer across replacement updates";
  Text::ControllerPtr ownershipController =
    MakeEndController(ownershipText, OWNERSHIP_WIDTH, CONTROL_HEIGHT);
  Text::Controller::Impl& ownershipImpl =
    Text::Controller::Impl::GetImplementation(*ownershipController.Get());

  DALI_TEST_CHECK(!ownershipController->HasValidReplacementSource());
  const Text::FinalElisionResult* plainFinal = ownershipController->GetFinalElisionResult();
  DALI_TEST_CHECK(plainFinal);
  CheckGeneratedEllipsis(*plainFinal, OWNERSHIP_WIDTH);
  CheckViewGlyphAccess(ownershipController, *plainFinal);

  Text::StyledTextBuilder imageBuilder = Text::StyledTextBuilder::New(Dali::String(ownershipText.c_str()));
  DALI_TEST_CHECK(imageBuilder.SetSpan(
    Text::ImageSpan::New(Text::ImageAttributes("unused-ellipsis-lifecycle.png", Vector2(40.0f, 30.0f))),
    10u,
    21u));
  ownershipController->SetStyledText(imageBuilder.Build());
  ownershipController->Relayout(Size(OWNERSHIP_WIDTH, CONTROL_HEIGHT));

  DALI_TEST_CHECK(ownershipController->HasValidReplacementSource());
  const Text::ReplacementRenderState& replacementState = ownershipImpl.GetReplacementRenderState();
  DALI_TEST_CHECK(replacementState.processingModel);
  DALI_TEST_EQUALS(replacementState.placements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(replacementState.placements[0u].visible != replacementState.placements[0u].elided);
  const Text::FinalElisionResult* replacementFinal = ownershipController->GetFinalElisionResult();
  DALI_TEST_CHECK(replacementFinal == &replacementState.finalElision);
  DALI_TEST_CHECK(replacementFinal->resolved);
  DALI_TEST_CHECK(replacementFinal->textElided);
  DALI_TEST_CHECK(replacementFinal->applied);
  DALI_TEST_EQUALS(replacementFinal->ellipsisUnitCount, 1u, TEST_LOCATION);
  CheckViewGlyphAccess(ownershipController, *replacementFinal);

  ownershipController->SetText(ownershipText);
  ownershipController->Relayout(Size(OWNERSHIP_WIDTH, CONTROL_HEIGHT));
  DALI_TEST_CHECK(!ownershipController->HasValidReplacementSource());
  DALI_TEST_CHECK(!ownershipImpl.mReplacementData);
  const Text::FinalElisionResult* restoredPlainFinal = ownershipController->GetFinalElisionResult();
  DALI_TEST_CHECK(restoredPlainFinal);
  CheckGeneratedEllipsis(*restoredPlainFinal, OWNERSHIP_WIDTH);
  CheckViewGlyphAccess(ownershipController, *restoredPlainFinal);

  END_TEST;
}

int UtcDaliEndEllipsisMetamorphicLayoutP(void)
{
  UiTestApplication application;

  const char* text = "prefix العربية 123 mixed עברית office cafe\xCC\x81 trailing sequence";
  auto makeController = [&]()
  {
    Text::ControllerPtr controller = Text::Controller::New();
    controller->SetText(text);
    controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(false);
    controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(Text::EllipsisPosition::END);
    return controller;
  };

  auto compare = [](const Text::FinalElisionResult& lhs, const Text::FinalElisionResult& rhs)
  {
    CheckSameFinalOutput(lhs, rhs);
  };

  Text::ControllerPtr persistent = makeController();
  persistent->Relayout(Size(96.0f, 44.0f));
  const Text::FinalElisionResult first = *persistent->GetFinalElisionResult();
  persistent->Relayout(Size(128.0f, 44.0f));
  const Text::FinalElisionResult resized = *persistent->GetFinalElisionResult();
  persistent->Relayout(Size(128.0f, 44.0f));
  compare(resized, *persistent->GetFinalElisionResult());

  Text::ControllerPtr fresh = makeController();
  fresh->Relayout(Size(128.0f, 44.0f));
  compare(resized, *fresh->GetFinalElisionResult());

  Text::CharacterIndex previousRetainedEnd = first.lines[first.ellipsisLineIndex].characterRun.characterIndex +
                                             first.lines[first.ellipsisLineIndex].characterRun.numberOfCharacters;
  for(float width : {104.0f, 112.0f, 120.0f, 128.0f, 136.0f})
  {
    Text::ControllerPtr probe = makeController();
    probe->Relayout(Size(width, 44.0f));
    const Text::FinalElisionResult* final = probe->GetFinalElisionResult();
    DALI_TEST_CHECK(final);
    CheckGeneratedEllipsis(*final, width);
    const Text::LineRun& line = final->lines[final->ellipsisLineIndex];
    const Text::CharacterIndex retainedEnd = line.characterRun.characterIndex + line.characterRun.numberOfCharacters;
    DALI_TEST_CHECK(retainedEnd >= previousRetainedEnd);
    previousRetainedEnd = retainedEnd;
  }

  END_TEST;
}

int UtcDaliEndEllipsisMaximalWidthUtilizationP(void)
{
  UiTestApplication application;
  struct Probe
  {
    const char*                 text;
    float                       width;
    Dali::LayoutDirection::Type direction;
    Text::CharacterIndex        expectedRetainedEnd;
    float                       expectedFinalWidth;
  };
  const Probe probes[] = {
    {"The quick brown fox jumps over the lazy dog and keeps going",
     303.0f,
     LayoutDirection::LEFT_TO_RIGHT,
     39u,
     300.0f},
    {"עברית חושפת מילים וממשיכה לאורך השורה לבדיקה",
     250.0f,
     LayoutDirection::RIGHT_TO_LEFT,
     32u,
     247.5f},
  };
  for(const Probe& probe : probes)
  {
    Text::ControllerPtr controller = Text::Controller::New();
    controller->SetText(probe.text);
    controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(false);
    controller->SetLayoutDirection(probe.direction);
    controller->SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(Text::EllipsisPosition::END);
    controller->Relayout(Size(probe.width, 44.0f), probe.direction);
    const Text::FinalElisionResult* final = controller->GetFinalElisionResult();
    DALI_TEST_CHECK(final);
    CheckEndReplaceContract(controller, *final, probe.width);
    DALI_TEST_EQUALS(GetRetainedCharacterEnd(*final), probe.expectedRetainedEnd, TEST_LOCATION);
    DALI_TEST_EQUALS(final->lines[final->ellipsisLineIndex].width,
                     probe.expectedFinalWidth,
                     0.01f,
                     TEST_LOCATION);
  }
  END_TEST;
}
