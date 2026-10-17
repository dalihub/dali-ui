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

#include <dali-ui-foundation/internal/text/bidirectional-line-info-run.h>
#include <dali-ui-foundation/internal/text/bidirectional-paragraph-info-run.h>
#include <dali-ui-foundation/internal/text/bidirectional-support.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/layouts/layout-parameters.h>
#include <dali-ui-foundation/internal/text/script-run.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-text-abstraction.h>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Text;

void utc_dali_bidirectional_support_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_bidirectional_support_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
#if defined(__cpp_char8_t) && (__cpp_char8_t >= 201811L)
inline const char* u8s(const char8_t* s)
{
  return reinterpret_cast<const char*>(s);
}
#else
inline const char* u8s(const char* s)
{
  return s;
}
#endif

struct ReorderRequest
{
  TextAbstraction::BidiInfoIndex paragraph;
  CharacterIndex                 offset;
  Length                         length;
};
std::vector<ReorderRequest> reorderRequests;

/**
 * @brief Records paragraph-relative requests and returns a reversed range map.
 * The deterministic map exposes missing translation into line-relative indices.
 */
void ReverseParagraph(TextAbstraction::BidiInfoIndex paragraph, CharacterIndex offset, Length length, CharacterIndex* map)
{
  reorderRequests.push_back({paragraph, offset, length});
  // A deterministic nonidentity map makes missing line-relative offsets visible.
  for(CharacterIndex i = 0u; i < length; ++i)
  {
    map[i] = length - 1u - i;
  }
}

/** @brief Restores the previous mock callback when the synchronous test ends. */
struct ScopedReorderOracle
{
  ScopedReorderOracle()
  : previous(TextAbstraction::SetTestReorderCallback(ReverseParagraph))
  {
    reorderRequests.clear();
  }
  ~ScopedReorderOracle()
  {
    TextAbstraction::SetTestReorderCallback(previous);
  }
  ScopedReorderOracle(const ScopedReorderOracle&)                            = delete;
  ScopedReorderOracle&                 operator=(const ScopedReorderOracle&) = delete;
  TextAbstraction::TestReorderCallback previous;
};

/**
 * @brief Shapes a model with the existing font mock before injecting bidi metadata.
 * The script mock classifies every character as Latin; each test supplies the
 * paragraph ranges and directions needed to exercise the real layout engine.
 *
 * @param[in] text UTF-8 source text.
 * @return A controller whose model is ready for the test's paragraph metadata.
 */
ControllerPtr CreateBidiLayoutController(const char* text)
{
  auto controller = Controller::New();
  controller->SetText(text);
  controller->SetMultiLineEnabled(false);
  controller->Relayout(Size(500.0f, 200.0f));
  auto model = Controller::Impl::GetImplementation(*controller.Get()).mModel;
  model->mLogicalModel->mCharacterDirections.Resize(model->mLogicalModel->mText.Count(), true);
  model->mVisualModel->mLines.Clear();
  model->mRemoveFrontInset = true;
  model->mRemoveBackInset  = true;
  return controller;
}

/**
 * @brief Lays out the prepared source model without rebuilding its bidi metadata.
 *
 * @param[in,out] controller The controller owning the prepared model and engine.
 * @param[in] width The available width in the mock's coordinate system.
 * @param[in] multiline Whether the text may wrap onto multiple lines.
 * @return Whether layout updated the model.
 */
bool LayoutBidiModel(Controller& controller, float width, bool multiline)
{
  auto  model      = Controller::Impl::GetImplementation(controller).mModel;
  auto  fontClient = TextAbstraction::FontClient::Get();
  auto  bidi       = TextAbstraction::BidirectionalSupport::Get();
  auto& engine     = controller.GetLayoutEngine();
  engine.SetLayout(multiline ? Layout::Engine::MULTI_LINE_BOX : Layout::Engine::SINGLE_LINE_BOX);
  Layout::Parameters parameters(Size(width, 200.0f), model, fontClient, bidi);
  parameters.numberOfGlyphs         = static_cast<Length>(model->mVisualModel->mGlyphs.Count());
  parameters.estimatedNumberOfLines = parameters.numberOfGlyphs;
  Size size;
  bool marquee = false;
  return engine.LayoutText(parameters, size, false, marquee, false, false);
}
} //namespace

int UtcDaliBidirectionalMixedParagraphAndLineRangesP(void)
{
  UiTestApplication application;
  ScopedReorderOracle                   oracle;
  TextAbstraction::BidirectionalSupport bidi = TextAbstraction::BidirectionalSupport::Get();

  Vector<Character> text;
  for(Character character : {Character('a'), Character('b'), Character(' '),
                             Character(0x05D0), Character('('), Character(0x05D1),
                             Character(')'), Character('\n'), Character('c')})
  {
    text.PushBack(character);
  }

  Vector<ScriptRun> scripts;
  scripts.PushBack({{0u, 3u}, TextAbstraction::LATIN, false});
  scripts.PushBack({{3u, 5u}, TextAbstraction::HEBREW, true});
  scripts.PushBack({{8u, 1u}, TextAbstraction::LATIN, false});

  Vector<LineBreakInfo> lineBreaks;
  lineBreaks.Resize(text.Count());
  for(uint32_t index = 0u; index < text.Count(); ++index)
  {
    lineBreaks[index] = TextAbstraction::LINE_NO_BREAK;
  }
  lineBreaks[7u] = TextAbstraction::LINE_MUST_BREAK;
  lineBreaks[8u] = TextAbstraction::LINE_MUST_BREAK;

  const Length characterCount = static_cast<Length>(text.Count());
  Vector<BidirectionalParagraphInfoRun> paragraphs;
  Vector<BidirectionalLineInfoRun> lines;
  SetBidirectionalInfo(bidi, text, scripts, lineBreaks, 0u, characterCount, paragraphs, lines);
  DALI_TEST_EQUALS(paragraphs.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(paragraphs[0u].characterRun.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(paragraphs[0u].characterRun.numberOfCharacters, 8u, TEST_LOCATION);

  Vector<CharacterDirection> directions;
  // The direction mock is a no-op; initialize its output before inspection.
  directions.Resize(characterCount, false);
  GetCharactersDirection(bidi, paragraphs, characterCount, 0u, characterCount, directions);
  DALI_TEST_CHECK(!directions[0u]);
  DALI_TEST_EQUALS(paragraphs[0u].direction, true, TEST_LOCATION);
  DALI_TEST_EQUALS(directions.Count(), text.Count(), TEST_LOCATION);

  ReorderLine(bidi, paragraphs[0u], paragraphs, lines, 0u, 0u, 8u, paragraphs[0u].direction);
  DALI_TEST_EQUALS(lines.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(lines[0u].visualToLogicalMap != nullptr);
  DALI_TEST_CHECK(!lines[0u].isIdentity);

  ReorderLine(bidi, paragraphs[0u], paragraphs, lines, 1u, 4u, 4u, paragraphs[0u].direction);
  DALI_TEST_EQUALS(lines.Count(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(lines[1u].visualToLogicalMap != nullptr);

  // The scoped oracle writes a deterministic map; real-font probes additionally
  // validate Unicode bidi ordering and rendered positions.
  DALI_TEST_EQUALS(lines[0u].characterRun.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[0u].characterRun.numberOfCharacters, 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1u].characterRun.characterIndex, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1u].characterRun.numberOfCharacters, 4u, TEST_LOCATION);

  Vector<Character> mirrored;
  DALI_TEST_CHECK(GetMirroredText(bidi, text, directions, paragraphs, 0u, characterCount, mirrored));
  DALI_TEST_EQUALS(mirrored.Count(), text.Count(), TEST_LOCATION);

  for(auto& line : lines)
  {
    free(line.visualToLogicalMap);
  }
  bidi.DestroyInfo(paragraphs[0u].bidirectionalInfoIndex);

  END_TEST;
}

int UtcDaliBidirectionalSpanningParagraphRangesP(void)
{
  UiTestApplication                     application;
  ScopedReorderOracle                   oracle;
  auto                                  bidi = TextAbstraction::BidirectionalSupport::Get();
  Vector<BidirectionalParagraphInfoRun> paragraphs;
  BidirectionalParagraphInfoRun         first;
  first.characterRun           = {2u, 3u};
  first.bidirectionalInfoIndex = 10u;
  first.direction              = true;
  paragraphs.PushBack(first);
  BidirectionalParagraphInfoRun second;
  second.characterRun           = {7u, 4u};
  second.bidirectionalInfoIndex = 20u;
  second.direction              = true;
  paragraphs.PushBack(second);

  struct Case
  {
    CharacterIndex              start;
    Length                      length;
    std::vector<CharacterIndex> expected;
    std::vector<ReorderRequest> requests;
  };
  // Leading/intermediate/trailing LTR gaps, partial paragraph intersections,
  // a stale first-paragraph hint, ordinary contained ranges, and empty lines.
  const Case cases[] = {
    {0u, 13u, {0u, 1u, 4u, 3u, 2u, 5u, 6u, 10u, 9u, 8u, 7u, 11u, 12u}, {{10u, 0u, 3u}, {20u, 0u, 4u}}},
    {3u, 6u, {1u, 0u, 2u, 3u, 5u, 4u}, {{10u, 1u, 2u}, {20u, 0u, 2u}}},
    {8u, 3u, {2u, 1u, 0u}, {{20u, 1u, 3u}}},
    {3u, 2u, {1u, 0u}, {{10u, 1u, 2u}}},
    {5u, 2u, {0u, 1u}, {}},
    {2u, 0u, {}, {}}};
  for(const auto& test : cases)
  {
    reorderRequests.clear();
    Vector<BidirectionalLineInfoRun> lines;
    ReorderLine(bidi, paragraphs[0u], paragraphs, lines, 0u, test.start, test.length, false);
    DALI_TEST_EQUALS(lines.Count(), 1u, TEST_LOCATION);
    const auto& line = lines[0u];
    DALI_TEST_EQUALS(line.characterRun.characterIndex, test.start, TEST_LOCATION);
    DALI_TEST_EQUALS(line.characterRun.numberOfCharacters, test.length, TEST_LOCATION);
    DALI_TEST_EQUALS(line.direction, false, TEST_LOCATION);
    DALI_TEST_EQUALS(reorderRequests.size(), test.requests.size(), TEST_LOCATION);
    for(size_t i = 0u; i < test.requests.size(); ++i)
    {
      DALI_TEST_EQUALS(reorderRequests[i].paragraph, test.requests[i].paragraph, TEST_LOCATION);
      DALI_TEST_EQUALS(reorderRequests[i].offset, test.requests[i].offset, TEST_LOCATION);
      DALI_TEST_EQUALS(reorderRequests[i].length, test.requests[i].length, TEST_LOCATION);
    }
    bool identity = true;
    for(CharacterIndex i = 0u; i < test.length; ++i)
    {
      DALI_TEST_EQUALS(line.visualToLogicalMap[i], test.expected[i], TEST_LOCATION);
      identity = identity && test.expected[i] == i;
    }
    DALI_TEST_EQUALS(line.isIdentity, identity, TEST_LOCATION);
    free(line.visualToLogicalMap);
  }
  END_TEST;
}

int UtcDaliBidirectionalLayoutParagraphDispatchP(void)
{
  UiTestApplication             application;
  ScopedReorderOracle           oracle;
  auto                          controller = CreateBidiLayoutController(u8s(u8"ab\n\u05D0\u05D1\n\u05D2\u05D3"));
  auto                          model      = Controller::Impl::GetImplementation(*controller.Get()).mModel;
  auto&                         logical    = *model->mLogicalModel;
  BidirectionalParagraphInfoRun paragraph;
  paragraph.characterRun           = {3u, 3u};
  paragraph.bidirectionalInfoIndex = 10u;
  paragraph.direction              = true;
  logical.mBidirectionalParagraphInfo.PushBack(paragraph);
  paragraph.characterRun           = {6u, 2u};
  paragraph.bidirectionalInfoIndex = 20u;
  logical.mBidirectionalParagraphInfo.PushBack(paragraph);
  for(CharacterIndex i = 0u; i < 3u; ++i)
  {
    logical.mCharacterDirections[i] = false;
  }

  DALI_TEST_CHECK(LayoutBidiModel(*controller, 500.0f, false));
  DALI_TEST_EQUALS(model->mVisualModel->mLines.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(logical.mBidirectionalLineInfo.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests.size(), size_t{2u}, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[0u].paragraph, 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[0u].offset, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[0u].length, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[1u].paragraph, 20u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[1u].offset, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[1u].length, 2u, TEST_LOCATION);
  const auto& line = logical.mBidirectionalLineInfo[0u];
  DALI_TEST_EQUALS(line.characterRun.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(line.characterRun.numberOfCharacters, 8u, TEST_LOCATION);
  DALI_TEST_CHECK(!line.direction && !line.isIdentity);
  const std::array<CharacterIndex, 8u> expected{{0u, 1u, 2u, 5u, 4u, 3u, 7u, 6u}};
  for(CharacterIndex i = 0u; i < line.characterRun.numberOfCharacters; ++i)
  {
    DALI_TEST_EQUALS(line.visualToLogicalMap[i], expected[i], TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliBidirectionalLayoutShortenedMapP(void)
{
  UiTestApplication   application;
  ScopedReorderOracle oracle;
  auto                controller = CreateBidiLayoutController(u8s(u8"\u05D0\u05D1\u05D2\u05D3"));
  auto                model      = Controller::Impl::GetImplementation(*controller.Get()).mModel;
  auto&               logical    = *model->mLogicalModel;
  auto&               visual     = *model->mVisualModel;
  DALI_TEST_EQUALS(visual.mGlyphs.Count(), 4u, TEST_LOCATION);
  BidirectionalParagraphInfoRun paragraph;
  paragraph.characterRun           = {0u, 4u};
  paragraph.bidirectionalInfoIndex = 10u;
  paragraph.direction              = true;
  logical.mBidirectionalParagraphInfo.PushBack(paragraph);
  model->mLineWrapMode = LineWrapMode::CHARACTER;
  // Synthetic overhang makes the reversed candidate wider than its logical
  // order. Layout must shorten it twice and replace the map after each change.
  for(auto& glyph : visual.mGlyphs)
  {
    glyph.advance  = 10.0f;
    glyph.width    = 5.0f;
    glyph.xBearing = 0.0f;
  }
  visual.mGlyphs[0u].width = 22.0f;
  DALI_TEST_CHECK(LayoutBidiModel(*controller, 35.0f, true));
  const std::array<ReorderRequest, 4u> expected{{{10u, 0u, 4u}, {10u, 0u, 3u}, {10u, 0u, 2u}, {10u, 2u, 2u}}};
  DALI_TEST_EQUALS(reorderRequests.size(), expected.size(), TEST_LOCATION);
  for(size_t i = 0u; i < expected.size(); ++i)
  {
    DALI_TEST_EQUALS(reorderRequests[i].paragraph, expected[i].paragraph, TEST_LOCATION);
    DALI_TEST_EQUALS(reorderRequests[i].offset, expected[i].offset, TEST_LOCATION);
    DALI_TEST_EQUALS(reorderRequests[i].length, expected[i].length, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(visual.mLines.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(logical.mBidirectionalLineInfo.Count(), 2u, TEST_LOCATION);
  for(LineIndex i = 0u; i < 2u; ++i)
  {
    const auto& line = logical.mBidirectionalLineInfo[i];
    DALI_TEST_EQUALS(line.characterRun.characterIndex, 2u * i, TEST_LOCATION);
    DALI_TEST_EQUALS(line.characterRun.numberOfCharacters, 2u, TEST_LOCATION);
    DALI_TEST_CHECK(line.direction && !line.isIdentity);
    DALI_TEST_EQUALS(line.visualToLogicalMap[0u], 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(line.visualToLogicalMap[1u], 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(visual.mLines[i].characterRun.numberOfCharacters, 2u, TEST_LOCATION);
  }

  // Clear only the second line as a partial-layout caller would. Relayout must
  // preserve the live prefix map and positions while rebuilding the suffix.
  const auto*                   prefixMap = logical.mBidirectionalLineInfo[0u].visualToLogicalMap;
  const std::array<Vector2, 2u> prefixPositions{{visual.mGlyphPositions[0u], visual.mGlyphPositions[1u]}};
  free(logical.mBidirectionalLineInfo[1u].visualToLogicalMap);
  logical.mBidirectionalLineInfo.Resize(1u);
  visual.mLines.Resize(1u);
  visual.mGlyphPositions.Resize(2u);
  auto               fontClient = TextAbstraction::FontClient::Get();
  auto               bidi       = TextAbstraction::BidirectionalSupport::Get();
  Layout::Parameters parameters(Size(60.0f, 200.0f), model, fontClient, bidi);
  parameters.startGlyphIndex        = 2u;
  parameters.numberOfGlyphs         = 2u;
  parameters.startLineIndex         = 1u;
  parameters.estimatedNumberOfLines = 1u;
  reorderRequests.clear();
  Size size;
  bool marquee = false;
  DALI_TEST_CHECK(controller->GetLayoutEngine().LayoutText(parameters, size, false, marquee, false, false));
  DALI_TEST_EQUALS(reorderRequests.size(), size_t{1u}, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[0u].paragraph, 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[0u].offset, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[0u].length, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(logical.mBidirectionalLineInfo.Count(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(logical.mBidirectionalLineInfo[0u].visualToLogicalMap == prefixMap);
  DALI_TEST_EQUALS(logical.mBidirectionalLineInfo[1u].visualToLogicalMap[0u], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(logical.mBidirectionalLineInfo[1u].visualToLogicalMap[1u], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.mGlyphPositions.Count(), 4u, TEST_LOCATION);
  for(GlyphIndex i = 0u; i < 2u; ++i)
  {
    DALI_TEST_EQUALS(visual.mGlyphPositions[i], prefixPositions[i], TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliBidirectionalLayoutTrailingWhitespaceP(void)
{
  UiTestApplication             application;
  ScopedReorderOracle           oracle;
  auto                          controller = CreateBidiLayoutController(u8s(u8"\u05D0\u05D1\u05D2 "));
  auto                          model      = Controller::Impl::GetImplementation(*controller.Get()).mModel;
  BidirectionalParagraphInfoRun paragraph;
  paragraph.characterRun           = {0u, 4u};
  paragraph.bidirectionalInfoIndex = 10u;
  paragraph.direction              = true;
  model->mLogicalModel->mBidirectionalParagraphInfo.PushBack(paragraph);
  DALI_TEST_CHECK(LayoutBidiModel(*controller, 500.0f, false));
  DALI_TEST_EQUALS(reorderRequests.size(), size_t{1u}, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[0u].offset, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(reorderRequests[0u].length, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->mLogicalModel->mBidirectionalLineInfo.Count(), 1u, TEST_LOCATION);
  const auto& line = model->mLogicalModel->mBidirectionalLineInfo[0u];
  for(CharacterIndex i = 0u; i < 4u; ++i)
  {
    DALI_TEST_EQUALS(line.visualToLogicalMap[i], 3u - i, TEST_LOCATION);
  }
  const auto& visual = *model->mVisualModel;
  DALI_TEST_EQUALS(visual.mLines.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(visual.mLines[0u].direction);
  DALI_TEST_EQUALS(visual.mLines[0u].extraLength, visual.mGlyphs.Back().advance, TEST_LOCATION);
  END_TEST;
}

int UtcDaliBidirectionalLayoutEndEllipsisP(void)
{
  UiTestApplication   application;
  ScopedReorderOracle oracle;
  for(bool multiline : {false, true})
  {
    auto                          controller = CreateBidiLayoutController(u8s(u8"\u05D0\u05D1 \u05D2\u05D3 \u05D4\u05D5"));
    auto                          model      = Controller::Impl::GetImplementation(*controller.Get()).mModel;
    auto&                         logical    = *model->mLogicalModel;
    auto&                         visual     = *model->mVisualModel;
    BidirectionalParagraphInfoRun paragraph;
    paragraph.characterRun           = {0u, 8u};
    paragraph.bidirectionalInfoIndex = 10u;
    paragraph.direction              = true;
    logical.mBidirectionalParagraphInfo.PushBack(paragraph);
    auto  fontClient = TextAbstraction::FontClient::Get();
    auto  bidi       = TextAbstraction::BidirectionalSupport::Get();
    auto& engine     = controller->GetLayoutEngine();
    engine.SetLayout(multiline ? Layout::Engine::MULTI_LINE_BOX : Layout::Engine::SINGLE_LINE_BOX);
    // Fit one mock word and its trailing space, but not the next word.
    Layout::Parameters parameters(Size(3.5f * visual.mGlyphs[0u].advance, 200.0f), model, fontClient, bidi);
    parameters.numberOfGlyphs         = static_cast<Length>(visual.mGlyphs.Count());
    parameters.estimatedNumberOfLines = parameters.numberOfGlyphs;
    parameters.maximumNumberOfLines   = 1u;
    Size size;
    bool marquee = false;
    reorderRequests.clear();
    DALI_TEST_CHECK(engine.LayoutText(parameters, size, true, marquee, false, false));
    DALI_TEST_EQUALS(visual.mLines.Count(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(visual.mLines[0u].ellipsis && visual.mLines[0u].direction);
    DALI_TEST_EQUALS(visual.mLines[0u].characterRun.characterIndex, 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(visual.mLines[0u].characterRun.numberOfCharacters, 3u, TEST_LOCATION);
    DALI_TEST_CHECK(!reorderRequests.empty());
    // The last reorder belongs to the retained END candidate, not the full
    // source line or the next paragraph/line examined while detecting overflow.
    const auto& request = reorderRequests.back();
    DALI_TEST_EQUALS(request.paragraph, 10u, TEST_LOCATION);
    DALI_TEST_EQUALS(request.offset, 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(request.length, 3u, TEST_LOCATION);
    DALI_TEST_CHECK(!logical.mBidirectionalLineInfo.Empty());
    const auto& line = logical.mBidirectionalLineInfo[0u];
    DALI_TEST_EQUALS(line.characterRun.characterIndex, 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(line.characterRun.numberOfCharacters, 3u, TEST_LOCATION);
    DALI_TEST_CHECK(!line.isIdentity);
    for(CharacterIndex i = 0u; i < 3u; ++i)
    {
      DALI_TEST_EQUALS(line.visualToLogicalMap[i], 2u - i, TEST_LOCATION);
    }
  }
  END_TEST;
}
