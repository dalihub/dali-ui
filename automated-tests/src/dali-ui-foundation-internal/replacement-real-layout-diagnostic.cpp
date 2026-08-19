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
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>
#include "replacement-layout-test-adapter.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Vector<Text::Character> Characters(std::initializer_list<Text::Character> values)
{
  Vector<Text::Character> text;
  text.Reserve(values.size());
  for(const Text::Character value : values)
  {
    text.PushBack(value);
  }
  return text;
}

Vector<Text::Character> Utf32(const std::string& utf8)
{
  const auto* bytes = reinterpret_cast<const uint8_t*>(utf8.data());
  Vector<Text::Character> text;
  text.Resize(Text::GetNumberOfUtf8Characters(bytes, utf8.size()));
  const uint32_t converted = Text::Utf8ToUtf32(bytes, utf8.size(), text.Begin());
  text.Resize(converted);
  return text;
}

Text::ReplacementRunSnapshot Candidate(Text::CharacterIndex start, Text::Length length, uint32_t id,
                                       float width = 20.0f)
{
  Text::ReplacementRunSnapshot candidate;
  candidate.logicalCharacterRange = Text::CharacterRun{start, length};
  candidate.metrics.width         = width;
  candidate.metrics.height        = 18.0f;
  candidate.occurrenceIdentity    = id;
  return candidate;
}

void Require(bool condition, const std::string& message)
{
  if(!condition)
  {
    throw std::runtime_error(message);
  }
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

uint32_t CountGeneratedFinalGlyphs(const Text::FinalElisionResult& result)
{
  uint32_t visibleSourceGlyphs = 0u;
  for(const Text::GlyphIndex finalGlyphIndex : result.sourceToFinalGlyphIndices)
  {
    visibleSourceGlyphs += finalGlyphIndex != Text::FinalElisionResult::INVALID_GLYPH_INDEX ? 1u : 0u;
  }
  return result.glyphs.Count() - visibleSourceGlyphs;
}

bool IsGeneratedEllipsisDrawable(const Text::ReplacementRenderState& state)
{
  const Text::FinalElisionResult& result = state.finalElision;
  if(!result.applied || result.ellipsisFinalGlyphIndex >= result.glyphs.Count())
  {
    return false;
  }

  const TextAbstraction::GlyphInfo& glyph = result.glyphs[result.ellipsisFinalGlyphIndex];
  const Vector2& position = result.viewGlyphPositions[result.ellipsisFinalGlyphIndex];
  const Size& control = state.processingModel->mVisualModel->mControlSize;
  return position.x + glyph.width > 0.0f && position.x < control.width &&
         position.y + glyph.height > 0.0f && position.y < control.height;
}

void ReleaseBidi(Text::ReplacementLayoutTestServices& services, Text::ReplacementRenderState& result)
{
  if(result.processingModel)
  {
    Text::LogicalModel& logicalModel = *result.processingModel->mLogicalModel;
    logicalModel.ClearBidirectionalParagraphInfo(services.bidirectionalSupport);
    logicalModel.mBidirectionalParagraphInfo.Clear();
  }
}

void ReleaseBidi(Text::ReplacementLayoutTestServices& services, const Text::ModelPtr& model)
{
  if(model)
  {
    Text::LogicalModel& logicalModel = *model->mLogicalModel;
    logicalModel.ClearBidirectionalParagraphInfo(services.bidirectionalSupport);
    logicalModel.mBidirectionalParagraphInfo.Clear();
  }
}

void CheckOrdinaryLineDirectionCase(const char*                          name,
                                    const std::string&                   utf8,
                                    bool                                 expectBidi,
                                    bool                                 expectRightToLeft,
                                    bool                                 expectNonIdentity,
                                    bool                                 elideText,
                                    float                                width,
                                    Text::ReplacementLayoutTestServices& services)
{
  Text::ModelPtr source              = Text::Model::New();
  source->mLogicalModel->mText       = Utf32(utf8);
  Text::ReplacementLayoutTestOptions options;
  options.contentSize = Size(width, 80.0f);
  options.elideText   = elideText;

  Text::ModelPtr result;
  Require(Text::LayoutOrdinaryForTest(*source, options, result),
          std::string(name) + ": ordinary layout failed");
  const Text::LogicalModel& logical = *result->mLogicalModel;
  const Text::VisualModel&  visual  = *result->mVisualModel;
  Require(visual.mLines.Count() == 1u, std::string(name) + ": expected one source line");
  Require(expectBidi == !logical.mBidirectionalParagraphInfo.Empty(),
          std::string(name) + ": unexpected bidi paragraph state");
  Require(visual.mLines[0u].direction == expectRightToLeft,
          std::string(name) + ": LineRun direction disagrees with the paragraph/line direction");
  Require(visual.mLines[0u].ellipsis == elideText,
          std::string(name) + ": unexpected source ellipsis state");

  if(expectBidi)
  {
    Require(!logical.mBidirectionalLineInfo.Empty(), std::string(name) + ": bidi line map was not produced");
    const Text::BidirectionalLineInfoRun& bidiLine = logical.mBidirectionalLineInfo[0u];
    Require(bidiLine.direction == expectRightToLeft,
            std::string(name) + ": BiDi line direction disagrees with expected paragraph direction");
    Require(bidiLine.isIdentity == !expectNonIdentity,
            std::string(name) + ": unexpected visual-to-logical map identity");
  }

  std::cout << "REAL_LINE_DIRECTION case=" << name
            << " bidi=" << expectBidi
            << " non_identity=" << expectNonIdentity
            << " rtl_line=" << visual.mLines[0u].direction
            << " ellipsis=" << elideText << std::endl;
  ReleaseBidi(services, result);
}

void CheckLayoutCase(const char* name, Vector<Text::Character>& text, Text::CharacterIndex start,
                     Text::Length length, bool expectBidi, Text::ReplacementLayoutTestServices& services)
{
  Vector<Text::ReplacementRunSnapshot> candidates;
  candidates.PushBack(Candidate(start, length, 1u, 28.0f));
  Text::ReplacementProjection projection = Text::ReplacementProjection::Build(text, candidates);

  Text::ReplacementLayoutTestOptions options;
  options.contentSize = Vector2(400.0f, 80.0f);
  Text::ReplacementRenderState result;
  Require(Text::LayoutReplacementForTest(projection, services, options, result),
          std::string(name) + ": projection layout was not entered");
  Require(CountSyntheticGlyphs(result) == 1u, std::string(name) + ": replacement was not one glyph");
  Require(result.placements.Count() == 1u && result.placements[0u].visible,
          std::string(name) + ": replacement placement is missing");
  Require(result.placements[0u].logicalCharacterRange.characterIndex == start &&
            result.placements[0u].logicalCharacterRange.numberOfCharacters == length,
          std::string(name) + ": original logical range was not retained");
  Require(std::isfinite(result.placements[0u].position.x), std::string(name) + ": placement is not finite");
  Require(projection.FindByLogicalCharacter(start + length / 2u) != nullptr,
          std::string(name) + ": interior logical lookup lost the replacement");
  Require(projection.FindByLogicalCharacter(start + length / 2u)->projectedCharacterIndex ==
            projection.LogicalCharacterToProjected(result.placements[0u].logicalCharacterRange.characterIndex),
          std::string(name) + ": interior logical index maps to another visual unit");

  const Text::LogicalModel& logicalModel = *result.processingModel->mLogicalModel;
  Require(logicalModel.mBidirectionalParagraphInfo.Empty() != expectBidi,
          std::string(name) + ": unexpected bidi paragraph state");
  if(expectBidi)
  {
    Require(!logicalModel.mBidirectionalLineInfo.Empty(), std::string(name) + ": bidi line map was not produced");
  }

  std::cout << "REAL_REPLACEMENT_LAYOUT case=" << name
            << " bidi=" << expectBidi
            << " logical_length=" << length
            << " projected_index="
            << projection.LogicalCharacterToProjected(result.placements[0u].logicalCharacterRange.characterIndex)
            << " x=" << result.placements[0u].position.x
            << " rtl_line=" << result.placements[0u].lineDirection << std::endl;
  ReleaseBidi(services, result);
}

void CheckEndEllipsisCase(const char* name, Vector<Text::Character>& text, Text::CharacterIndex firstReplacement,
                          bool expectBidi, Text::ReplacementLayoutTestServices& services)
{
  Vector<Text::ReplacementRunSnapshot> candidates;
  constexpr float widths[] = {8.0f, 24.0f, 48.0f, 80.0f, 32.0f};
  for(uint32_t index = 0u; index < 5u; ++index)
  {
    candidates.PushBack(Candidate(firstReplacement + index * 2u, 1u, 100u + index, widths[index]));
  }
  Text::ReplacementProjection projection = Text::ReplacementProjection::Build(text, candidates);

  Text::ReplacementLayoutTestOptions options;
  options.contentSize      = Vector2(90.0f, 40.0f);
  options.elideText        = true;
  options.ellipsisPosition = Text::EllipsisPosition::END;
  Text::ReplacementRenderState result;
  Require(Text::LayoutReplacementForTest(projection, services, options, result),
          std::string(name) + ": END ellipsis layout was not entered");
  Require(result.finalElision.textElided,
          std::string(name) + ": END ellipsis was not produced");
  Require(result.placements.Count() == 5u, std::string(name) + ": placement count changed");

  uint32_t visibleCount = 0u;
  uint32_t elidedCount  = 0u;
  for(const Text::ReplacementPlacement& placement : result.placements)
  {
    Require(placement.visible != placement.elided, std::string(name) + ": replacement is partially classified");
    Require(placement.logicalCharacterRange.numberOfCharacters == 1u,
            std::string(name) + ": ellipsis split the logical replacement range");
    visibleCount += placement.visible ? 1u : 0u;
    elidedCount += placement.elided ? 1u : 0u;
  }
  Require(visibleCount > 0u && elidedCount > 0u,
          std::string(name) + ": test did not cover both visible and elided replacements");

  const Text::FinalElisionResult& finalElision = result.finalElision;
  Require(finalElision.resolved,
          std::string(name) + ": final elision was not resolved");
  Require(finalElision.applied && finalElision.ellipsisUnitCount == 1u &&
            finalElision.ellipsisOmissionReason == Text::FinalElisionResult::EllipsisOmissionReason::NONE,
          std::string(name) + ": shared final-elision state disagrees with the generated ellipsis");
  Require(CountGeneratedFinalGlyphs(finalElision) == 1u,
          std::string(name) + ": END ellipsis does not own exactly one semantic unit");
  const Text::LogicalModel& logicalModel = *result.processingModel->mLogicalModel;
  Require(logicalModel.mBidirectionalParagraphInfo.Empty() != expectBidi,
          std::string(name) + ": unexpected END ellipsis bidi state");
  std::cout << "REAL_REPLACEMENT_END_ELLIPSIS case=" << name
            << " visible=" << visibleCount
            << " elided=" << elidedCount
            << " bidi=" << expectBidi << std::endl;
  ReleaseBidi(services, result);
}

void CheckOversizedVerticalSweep(Text::ReplacementLayoutTestServices& services)
{
  const std::string utf8 =
    "An oversized replacement follows wrapped introductory prose and tests vertical END ellipsis. "
    "More words place \uFFFC near a constrained line before many trailing sentences continue. "
    "The large reserved box must be fully visible only when its whole line participates in the visible layout. "
    "Otherwise the renderer must choose a text ellipsis boundary without flashing, cropping or retaining the large image. "
    "Repeated trailing words add stable overflow for wide and narrow resize verification.";
  Vector<Text::Character> text = Utf32(utf8);
  Text::CharacterIndex replacementIndex = 0u;
  while(replacementIndex < text.Count() && text[replacementIndex] != Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER)
  {
    ++replacementIndex;
  }
  Require(replacementIndex < text.Count(), "oversized: U+FFFC marker is missing");

  Vector<Text::ReplacementRunSnapshot> candidates;
  candidates.PushBack(Candidate(replacementIndex, 1u, 2700u, 210.0f));
  candidates[0u].metrics.height = 120.0f;
  const Text::ReplacementProjection projection = Text::ReplacementProjection::Build(text, candidates);

  bool     sawVisible              = false;
  bool     sawElided               = false;
  bool     previousVisible         = false;
  uint32_t heightEllipsisLayouts   = 0u;
  uint32_t widthEllipsisLayouts    = 0u;
  float    firstVisibleHeight      = 0.0f;
  float    firstVisibleWidth       = 0.0f;
  for(float height = 40.0f; height <= 400.0f; height += 2.0f)
  {
    Text::ReplacementLayoutTestOptions options;
    options.contentSize      = Size(400.0f, height);
    options.layoutType       = Text::Layout::Engine::MULTI_LINE_BOX;
    options.lineWrapMode     = Text::LineWrapMode::WORD;
    options.elideText        = true;
    options.ellipsisPosition = Text::EllipsisPosition::END;
    options.fontPointSize    = 28u * 64u;
    options.fontPixelSize    = 28.0f * 4.0f / 3.0f;
    options.sourceRevision   = 270u;
    options.layoutGeneration = static_cast<uint64_t>(height);
    Text::ReplacementRenderState result;
    Require(Text::LayoutReplacementForTest(projection, services, options, result),
            "oversized: projected layout was not entered");
    Require(result.placements.Count() == 1u, "oversized: placement count changed");
    Require(!previousVisible || result.placements[0u].visible,
            "oversized: more height hid a previously visible replacement");
    previousVisible = result.placements[0u].visible;
    sawVisible |= result.placements[0u].visible;
    sawElided  |= result.placements[0u].elided;
    if(firstVisibleHeight == 0.0f && result.placements[0u].visible)
    {
      firstVisibleHeight = height;
    }

    const Text::FinalElisionResult& finalElision = result.finalElision;
    Require(finalElision.resolved,
            "oversized: final result was not resolved");
    Require(CountGeneratedFinalGlyphs(finalElision) <= 1u,
            "oversized: duplicate semantic ellipsis unit");
    if(finalElision.textElided)
    {
      ++heightEllipsisLayouts;
      Require(finalElision.applied && finalElision.ellipsisUnitCount == 1u &&
                CountGeneratedFinalGlyphs(finalElision) == 1u,
              "oversized: elided layout has no authoritative ellipsis unit");
      Require(IsGeneratedEllipsisDrawable(result),
              "oversized: generated ellipsis is outside the control");
    }
    ReleaseBidi(services, result);
  }

  previousVisible = false;
  for(float width = 120.0f; width <= 600.0f; width += 2.0f)
  {
    Text::ReplacementLayoutTestOptions options;
    options.contentSize      = Size(width, 367.0f);
    options.layoutType       = Text::Layout::Engine::MULTI_LINE_BOX;
    options.lineWrapMode     = Text::LineWrapMode::WORD;
    options.elideText        = true;
    options.ellipsisPosition = Text::EllipsisPosition::END;
    options.fontPointSize    = 28u * 64u;
    options.fontPixelSize    = 28.0f * 4.0f / 3.0f;
    options.sourceRevision   = 271u;
    options.layoutGeneration = static_cast<uint64_t>(width + 1000.0f);
    Text::ReplacementRenderState result;
    Require(Text::LayoutReplacementForTest(projection, services, options, result),
            "oversized width: projected layout was not entered");
    Require(result.placements.Count() == 1u, "oversized width: placement count changed");
    Require(!previousVisible || result.placements[0u].visible,
            "oversized width: more width hid a previously visible replacement");
    previousVisible = result.placements[0u].visible;
    sawVisible |= result.placements[0u].visible;
    sawElided  |= result.placements[0u].elided;
    if(firstVisibleWidth == 0.0f && result.placements[0u].visible)
    {
      firstVisibleWidth = width;
    }

    const Text::FinalElisionResult& finalElision = result.finalElision;
    if(finalElision.textElided)
    {
      ++widthEllipsisLayouts;
      Require(finalElision.applied && finalElision.ellipsisUnitCount == 1u &&
                CountGeneratedFinalGlyphs(finalElision) == 1u,
              "oversized width: elided layout has no authoritative ellipsis unit");
      Require(IsGeneratedEllipsisDrawable(result),
              "oversized width: generated ellipsis is outside the control");
    }
    ReleaseBidi(services, result);
  }

  Require(sawVisible && sawElided && heightEllipsisLayouts > 0u && widthEllipsisLayouts > 0u,
          "oversized: sweep did not cross visible/elided and ellipsis thresholds");
  std::cout << "REAL_REPLACEMENT_OVERSIZED_SWEEP height=40..400/2 width=120..600/2"
            << " first_visible_height=" << firstVisibleHeight
            << " first_visible_width=" << firstVisibleWidth
            << " height_ellipsis_layouts=" << heightEllipsisLayouts
            << " width_ellipsis_layouts=" << widthEllipsisLayouts
            << " visible_and_elided=1" << std::endl;
}

} // unnamed namespace

void RunDiagnostics()
{
  Text::ReplacementLayoutTestServices services{
    TextAbstraction::Segmentation::Get(),
    TextAbstraction::BidirectionalSupport::Get(),
    TextAbstraction::Shaping::Get(),
    TextAbstraction::FontClient::Get(),
    Text::MultilanguageSupport::Get()};

  CheckOrdinaryLineDirectionCase("pure_ltr",
                                 "Pure LTR text",
                                 false,
                                 false,
                                 false,
                                 false,
                                 400.0f,
                                 services);
  CheckOrdinaryLineDirectionCase("pure_rtl",
                                 "\xD7\x90\xD7\x91\xD7\x92\xD7\x93",
                                 true,
                                 true,
                                 true,
                                 false,
                                 400.0f,
                                 services);
  CheckOrdinaryLineDirectionCase("mixed_ltr",
                                 "English \xD7\x90\xD7\x91\xD7\x92 trailing",
                                 true,
                                 false,
                                 true,
                                 false,
                                 400.0f,
                                 services);
  CheckOrdinaryLineDirectionCase("mixed_rtl",
                                 "\xD7\x90\xD7\x91\xD7\x92 English \xD7\x93",
                                 true,
                                 true,
                                 true,
                                 false,
                                 400.0f,
                                 services);
  CheckOrdinaryLineDirectionCase("mixed_ltr_end_ellipsis",
                                 "English \xD7\x90\xD7\x91\xD7\x92 trailing words force END ellipsis",
                                 true,
                                 false,
                                 true,
                                 true,
                                 100.0f,
                                 services);

  Vector<Text::Character> ltr = Characters({'a', 'b', 'I', 'C', 'O', 'N', 'c', 'd'});
  CheckLayoutCase("ltr", ltr, 2u, 4u, false, services);

  Vector<Text::Character> canonical = Characters({'A', 0xFFFCu, 'B'});
  CheckLayoutCase("canonical_ufffc", canonical, 1u, 1u, false, services);

  Vector<Text::Character> rtl = Characters({0x05D0u, 0x05D1u, 'I', 'C', 'O', 'N', 0x05D2u, 0x05D3u});
  CheckLayoutCase("rtl", rtl, 2u, 4u, true, services);

  Vector<Text::Character> ltrRtl =
    Characters({'A', 0x05D0u, 0x05D1u, 'I', 'C', 'O', 'N', 0x05D2u, 0x05D3u, 'B'});
  CheckLayoutCase("ltr_rtl", ltrRtl, 3u, 4u, true, services);

  Vector<Text::Character> rtlLtr =
    Characters({0x05D0u, 'A', 'B', 'I', 'C', 'O', 'N', 'C', 'D', 0x05D1u});
  CheckLayoutCase("rtl_ltr", rtlLtr, 3u, 4u, true, services);

  Vector<Text::Character> combiningBase = Characters({'X', 'a', 0x0301u, 'Y'});
  CheckLayoutCase("combining_base_only", combiningBase, 1u, 1u, false, services);

  Vector<Text::Character> combiningMark = Characters({'X', 'a', 0x0301u, 'Y'});
  CheckLayoutCase("combining_mark_only", combiningMark, 2u, 1u, false, services);

  Vector<Text::Character> variationSelector = Characters({'X', 0x2764u, 0xFE0Fu, 'Y'});
  CheckLayoutCase("variation_selector_only", variationSelector, 2u, 1u, false, services);

  Vector<Text::Character> emojiModifier = Characters({'X', 0x1F44Du, 0x1F3FDu, 'Y'});
  CheckLayoutCase("emoji_modifier_only", emojiModifier, 2u, 1u, false, services);

  Vector<Text::Character> zwj = Characters({'X', 0x1F469u, 0x200Du, 0x1F469u, 'Y'});
  CheckLayoutCase("emoji_zwj_only", zwj, 2u, 1u, false, services);

  Vector<Text::Character> regionalIndicator = Characters({'X', 0x1F1F0u, 0x1F1F7u, 'Y'});
  CheckLayoutCase("regional_indicator_first", regionalIndicator, 1u, 1u, false, services);

  Vector<Text::Character> latinLigature = Characters({'X', 'f', 'i', 'Y'});
  CheckLayoutCase("latin_ligature_first", latinLigature, 1u, 1u, false, services);

  Vector<Text::Character> arabicLigature = Characters({'X', 0x0644u, 0x0627u, 'Y'});
  CheckLayoutCase("arabic_ligature_lam", arabicLigature, 1u, 1u, true, services);

  Vector<Text::Character> ltrEllipsis =
    Characters({'A', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'Z'});
  CheckEndEllipsisCase("ltr", ltrEllipsis, 1u, false, services);

  Vector<Text::Character> rtlEllipsis =
    Characters({0x05D0u, 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 0x05D1u});
  CheckEndEllipsisCase("rtl", rtlEllipsis, 1u, true, services);

  Vector<Text::Character> mixedEllipsis =
    Characters({'A', 0x05D0u, 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 0x05D1u, 'Z'});
  CheckEndEllipsisCase("ltr_rtl", mixedEllipsis, 2u, true, services);

  CheckOversizedVerticalSweep(services);
}

class DiagnosticRunner : public ConnectionTracker
{
public:
  explicit DiagnosticRunner(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &DiagnosticRunner::OnInit);
  }

  int GetExitStatus() const
  {
    return mExitStatus;
  }

private:
  void OnInit(Application)
  {
    try
    {
      RunDiagnostics();
    }
    catch(const std::exception& exception)
    {
      std::cerr << "REAL_REPLACEMENT_FAILURE " << exception.what() << std::endl;
      mExitStatus = 1;
    }
    // InitSignal is emitted before MainLoop finishes its post-initialize
    // phase. Defer Quit to the first loop tick so the request is not lost.
    mQuitTimer = Timer::New(1u);
    mQuitTimer.TickSignal().Connect(this, &DiagnosticRunner::OnQuitTimer);
    mQuitTimer.Start();
  }

  bool OnQuitTimer()
  {
    mApplication.Quit();
    return false;
  }

private:
  Application& mApplication;
  Timer        mQuitTimer;
  int          mExitStatus{0};
};

int main(int argc, char** argv)
{
  Application      application = Application::New(&argc, &argv);
  DiagnosticRunner runner(application);
  application.MainLoop();
  return runner.GetExitStatus();
}
