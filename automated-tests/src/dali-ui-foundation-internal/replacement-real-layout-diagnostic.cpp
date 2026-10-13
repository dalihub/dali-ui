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
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
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
  const auto*             bytes = reinterpret_cast<const uint8_t*>(utf8.data());
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
  Text::ModelPtr source        = Text::Model::New();
  source->mLogicalModel->mText = Utf32(utf8);
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

void CheckOrdinaryMultilineDirectionCases(Text::ReplacementLayoutTestServices& services)
{
  struct Case
  {
    const char* name;
    std::string text;
    Text::LineWrapMode wrapMode;
    float width;
  };
  const std::initializer_list<Case> cases = {
    {"hebrew_words", "אבגדה וזחטי כלמנס עפצקר שתאבג וזחטי כלמנס", Text::LineWrapMode::WORD, 75.0f},
    {"arabic_characters", "العربية مع English وأرقام 123 وأحرف إضافية", Text::LineWrapMode::CHARACTER, 65.0f},
    {"mixed_paragraphs", "אבגדה English 123\nالعربية second line 456", Text::LineWrapMode::MIXED, 90.0f}
  };

  for(const Case& testCase : cases)
  {
    Text::ModelPtr source = Text::Model::New();
    source->mLogicalModel->mText = Utf32(testCase.text);
    Text::ReplacementLayoutTestOptions options;
    options.contentSize = Size(testCase.width, 250.0f);
    options.layoutType = Text::Layout::Engine::MULTI_LINE_BOX;
    options.lineWrapMode = testCase.wrapMode;

    Text::ModelPtr result;
    Require(Text::LayoutOrdinaryForTest(*source, options, result),
            std::string(testCase.name) + ": multiline layout failed");
    const Text::LogicalModel& logical = *result->mLogicalModel;
    const Text::VisualModel& visual = *result->mVisualModel;
    Require(visual.mLines.Count() > 1u, std::string(testCase.name) + ": expected wrapped lines");
    Require(!logical.mBidirectionalParagraphInfo.Empty(),
            std::string(testCase.name) + ": expected RTL paragraph");
    Require(!logical.mBidirectionalLineInfo.Empty(),
            std::string(testCase.name) + ": expected bidi line maps");
    for(const Text::LineRun& line : visual.mLines)
    {
      Require(line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs <= visual.mGlyphs.Count(),
              std::string(testCase.name) + ": line exceeds glyph buffer");
    }
    std::cout << "REAL_MULTILINE_DIRECTION case=" << testCase.name
              << " lines=" << visual.mLines.Count()
              << " bidi_lines=" << logical.mBidirectionalLineInfo.Count() << std::endl;
    ReleaseBidi(services, result);
  }
}

void CheckMarqueeTransitionCase(const char*           name,
                                const std::string&    utf8,
                                Text::Alignment       alignment,
                                LayoutDirection::Type layoutDirection,
                                float                 controlWidth,
                                bool                  expectedRightToLeft)
{
  Text::ModelPtr source        = Text::Model::New();
  source->mLogicalModel->mText = Utf32(utf8);

  Text::ReplacementLayoutTestOptions options;
  options.contentSize          = Size(controlWidth, 40.0f);
  options.elideText            = true;
  options.ellipsisPosition     = Text::EllipsisPosition::END;
  options.horizontalAlignment  = alignment;
  options.layoutDirection      = layoutDirection;
  options.matchLayoutDirection = true;

  const Text::OrdinaryMarqueeTransitionTrace trace =
    Text::TraceOrdinaryMarqueeTransitionForTest(*source, options);
  Require(trace.valid, std::string(name) + ": transition trace invalid");
  Require(trace.directionRightToLeft == expectedRightToLeft,
          std::string(name) + ": unexpected resolved text direction");
  Require(std::fabs(trace.sourceToTextureMaximumTranslation - trace.sourceToTextureMinimumTranslation) < 0.01f,
          std::string(name) + ": source-to-texture mapping is not rigid");

  // Delta solving and first-frame coordinate arithmetic are covered by
  // UtcDaliEndEllipsisMarqueeStartAnchorP. Here the real bidi/shaping output
  // must preserve a rigid mapping between static and marquee layouts.
  std::cout << "REAL_MARQUEE_TRANSITION case=" << name
            << " source_texture_spread="
            << trace.sourceToTextureMaximumTranslation - trace.sourceToTextureMinimumTranslation
            << std::endl;
}

void CheckMarqueeTransitions()
{
  const std::string rtl =
    "\xD7\xA9\xD7\x9C\xD7\x95\xD7\x9D \xD7\xA2\xD7\x95\xD7\x9C\xD7\x9D, \xD7\xA0\xD7\xA2\xD7\x99\xD7\x9D \xD7\x9E\xD7\x90\xD7\x95\xD7\x93,\xD7\x95\xD7\x9E\xD7\xA7\xD7\x95\xD7\x95\xD7\x94 \xD7\xA9\xD7\x99\xD7\x94\xD7\x99\xD7\x94 \xD7\x9C\xD7\xA0\xD7\x95 \xD7\xA9\xD7\x99\xD7\x97\xD7\x94 \xD7\xA0\xD7\xA2\xD7\x99\xD7\x9E\xD7\x94 \xD7\x95\xD7\x98\xD7\x95\xD7\x91\xD7\x94 \xD7\x99\xD7\x97\xD7\x93";

  CheckMarqueeTransitionCase("rtl_start", rtl, Text::Alignment::START,
                             LayoutDirection::RIGHT_TO_LEFT, 150.0f, true);
  CheckMarqueeTransitionCase("mixed_rigid", "English \xD7\x90\xD7\x91\xD7\x92 trailing words force END ellipsis",
                             Text::Alignment::CENTER, LayoutDirection::LEFT_TO_RIGHT,
                             90.0f, false);
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
  constexpr float                      widths[] = {8.0f, 24.0f, 48.0f, 80.0f, 32.0f};
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
    visibleCount += placement.visible ? 1u : 0u;
    elidedCount += placement.elided ? 1u : 0u;
  }
  Require(visibleCount > 0u && elidedCount > 0u,
          std::string(name) + ": test did not cover both visible and elided replacements");

  const Text::LogicalModel& logicalModel = *result.processingModel->mLogicalModel;
  Require(logicalModel.mBidirectionalParagraphInfo.Empty() != expectBidi,
          std::string(name) + ": unexpected END ellipsis bidi state");
  std::cout << "REAL_REPLACEMENT_END_ELLIPSIS case=" << name
            << " visible=" << visibleCount
            << " elided=" << elidedCount
            << " bidi=" << expectBidi << std::endl;
  ReleaseBidi(services, result);
}

void CheckAsyncRenderScaleAutoLineHeight()
{
  struct Summary
  {
    std::vector<float> lineHeights;
    float              layoutHeight{0.0f};
    Text::LineIndex    replacementLine{0u};
    uint32_t           fontCount{0u};
    bool               replacementVisible{false};
  };

  const std::pair<const char*, std::string> cases[] = {
    {"mixed_fallback_emoji",
     "Latin first line\n한글 둘째 줄\nمرحبا third 😀\n\uFFFC image line\ntrailing fifth line\ntrailing sixth line"},
    {"same_line_mixed",
     "Latin 😀 مرحبا before \uFFFC 한국어 after replacement on one mixed line\ntrailing second line\ntrailing third line"},
  };

  uint32_t horizontalDpi = 0u;
  uint32_t verticalDpi   = 0u;
  TextAbstraction::FontClient::Get().GetDpi(horizontalDpi, verticalDpi);

  for(const auto& testCase : cases)
  {
    const Vector<Text::Character> text             = Utf32(testCase.second);
    Text::CharacterIndex          replacementIndex = 0u;
    while(replacementIndex < text.Count() &&
          text[replacementIndex] != Text::ReplacementProjection::OBJECT_REPLACEMENT_CHARACTER)
    {
      ++replacementIndex;
    }
    Require(replacementIndex < text.Count(), "render-scale: U+FFFC marker is missing");

    const auto render = [&](float scale, float textHeight)
    {
      Text::AsyncTextParameters parameters;
      parameters.text             = testCase.second;
      parameters.fontSize         = 28.337f * 72.0f / static_cast<float>(horizontalDpi);
      parameters.textWidth        = 492.0f;
      parameters.textHeight       = textHeight;
      parameters.originWidth      = parameters.textWidth;
      parameters.originHeight     = parameters.textHeight;
      parameters.renderScale      = scale;
      parameters.isMultiLine      = true;
      parameters.ellipsis         = true;
      parameters.ellipsisPosition = Text::EllipsisPosition::END;
      parameters.relativeLineSize = -1.0f;
      parameters.lineWrapMode     = Text::LineWrapMode::WORD;
      parameters.maxTextureSize   = 4096;
      parameters.replacementSourceSnapshot.runs.PushBack(Candidate(replacementIndex, 1u, 2710u, 210.0f));
      parameters.replacementSourceSnapshot.runs[0u].metrics.height   = 120.0f;
      parameters.replacementSourceSnapshot.hasValidReplacementSource = true;

      Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
      bool                  cached = false;
      Size                  naturalSize;
      if(scale > 1.0f)
      {
        naturalSize = loader.SetupRenderScale(parameters, cached);
      }
      loader.RenderText(parameters, cached, naturalSize);
      const Text::ReplacementRenderState* state = Text::GetImplementation(loader).GetReplacementRenderState();
      Require(state && state->processingModel && state->placements.Count() == 1u,
              "render-scale: final replacement state is missing");
      const Vector<Text::LineRun>& lines = state->processingModel->mVisualModel->mLines;
      Require(!lines.Empty(), "render-scale: no final lines were produced");

      Summary summary;
      summary.layoutHeight       = state->layoutSize.height / scale;
      summary.replacementVisible = state->placements[0u].visible;
      summary.replacementLine    = state->placements[0u].lineIndex;
      std::set<Text::FontId> fontIds;
      for(const Text::GlyphInfo& glyph : state->processingModel->mVisualModel->mGlyphs)
      {
        if(glyph.fontId != 0u)
        {
          fontIds.insert(glyph.fontId);
        }
      }
      summary.fontCount = static_cast<uint32_t>(fontIds.size());
      for(const Text::LineRun& line : lines)
      {
        summary.lineHeights.push_back(Text::GetLineHeight(line, false) / scale);
      }
      return summary;
    };

    const Summary unconstrained = render(1.0f, 2000.0f);
    Require(unconstrained.replacementVisible, "render-scale: unconstrained replacement is hidden");
    float boundaryHeight = 0.0f;
    for(Text::LineIndex index = 0u; index <= unconstrained.replacementLine; ++index)
    {
      boundaryHeight += unconstrained.lineHeights[index];
    }
    boundaryHeight = std::ceil(boundaryHeight);
    while(boundaryHeight > 1.0f && render(1.0f, boundaryHeight - 1.0f).replacementVisible)
    {
      boundaryHeight -= 1.0f;
    }
    while(!render(1.0f, boundaryHeight).replacementVisible)
    {
      boundaryHeight += 1.0f;
    }

    const Summary logical      = render(1.0f, boundaryHeight);
    const Summary logicalBelow = render(1.0f, boundaryHeight - 1.0f);
    Require(logical.replacementVisible && !logicalBelow.replacementVisible,
            "render-scale: scale-1 boundary is not exact");
    Require(logical.fontCount > 1u, "render-scale: mixed case did not resolve multiple font ids");

    for(float scale : {1.25f, 1.5f, 2.0f})
    {
      const Summary scaled = render(scale, boundaryHeight);
      Require(scaled.replacementVisible,
              "render-scale: raster scale changed the oversized replacement boundary");
      Require(scaled.lineHeights.size() == logical.lineHeights.size(),
              "render-scale: raster scale changed the retained line count");
      for(std::size_t index = 0u; index < logical.lineHeights.size(); ++index)
      {
        Require(std::fabs(scaled.lineHeights[index] - logical.lineHeights[index]) < 0.01f,
                "render-scale: AUTO line height changed in logical coordinates");
      }
      std::cout << "REAL_REPLACEMENT_RENDER_SCALE case=" << testCase.first
                << " scale=" << scale
                << " boundary_height=" << boundaryHeight
                << " logical_lines=" << logical.lineHeights.size()
                << " scaled_lines=" << scaled.lineHeights.size()
                << " logical_fonts=" << logical.fontCount
                << " scaled_fonts=" << scaled.fontCount
                << " logical_replacement_visible=" << logical.replacementVisible
                << " scaled_replacement_visible=" << scaled.replacementVisible << std::endl;
    }
  }
}

} // unnamed namespace

// Keep only integration checks whose inputs depend on real font selection,
// shaping or bidi reordering. Projection, ellipsis ownership, size sweeps and
// marquee delta arithmetic belong to the mock-backed internal UTCs.
void RunDiagnostics()
{
  Text::ReplacementLayoutTestServices services{
    TextAbstraction::Segmentation::Get(),
    TextAbstraction::BidirectionalSupport::Get(),
    TextAbstraction::Shaping::Get(),
    TextAbstraction::FontClient::Get(),
    Text::MultilanguageSupport::Get()};

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
  CheckOrdinaryMultilineDirectionCases(services);
  CheckMarqueeTransitions();

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

  Vector<Text::Character> rtlEllipsis =
    Characters({0x05D0u, 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 0x05D1u});
  CheckEndEllipsisCase("rtl", rtlEllipsis, 1u, true, services);

  Vector<Text::Character> mixedEllipsis =
    Characters({'A', 0x05D0u, 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 'x', 0xFFFCu, 0x05D1u, 'Z'});
  CheckEndEllipsisCase("ltr_rtl", mixedEllipsis, 2u, true, services);

  // Size sweeps and ellipsis ownership are covered with deterministic metrics
  // by UtcDaliReplacementVerticalEndEllipsisLifecycleP.
  CheckAsyncRenderScaleAutoLineHeight();
}

class DiagnosticRunner : public ConnectionTracker
{
public:
  DiagnosticRunner(Application& application, bool renderScaleOnly)
  : mApplication(application),
    mRenderScaleOnly(renderScaleOnly)
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
      if(mRenderScaleOnly)
      {
        CheckAsyncRenderScaleAutoLineHeight();
      }
      else
      {
        RunDiagnostics();
      }
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
  bool         mRenderScaleOnly{false};
};

int main(int argc, char** argv)
{
  const bool       renderScaleOnly = argc > 1 && std::string(argv[1]) == "--render-scale-only";
  Application      application     = Application::New(&argc, &argv);
  DiagnosticRunner runner(application, renderScaleOnly);
  application.MainLoop();
  return runner.GetExitStatus();
}
