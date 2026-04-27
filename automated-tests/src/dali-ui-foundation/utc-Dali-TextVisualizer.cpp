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
 *
 */

#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/internal/text/text-visualizer/atlas-renderer-bridge.h>
#include <dali-ui-foundation/internal/text/text-visualizer/atlas-view-adapter.h>
#include <dali-ui-foundation/internal/text/text-visualizer/exclusion-layout-cache.h>
#include <dali-ui-foundation/internal/text/text-visualizer/layout-engine.h>
#include <dali-ui-foundation/internal/text/text-visualizer/prepared-text.h>
#include <dali-ui-foundation/internal/text/text-visualizer/rendering/text-visualizer-glyph-renderer.h>
#include <dali-ui-foundation/internal/text/text-visualizer/text-preparer.h>
#include <dali-ui-foundation/internal/text/text-visualizer/text-visualizer-view-interface.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{
const char* const PROPERTY_NAME_TEXT        = "text";
const char* const PROPERTY_NAME_FONT_FAMILY = "fontFamily";
const char* const PROPERTY_NAME_FONT_SIZE   = "fontSize";
const char* const PROPERTY_NAME_TEXT_COLOR  = "textColor";
const char* const PROPERTY_NAME_LINE_HEIGHT = "lineHeight";

Dali::Ui::Internal::TextVisualizer::PreparedText CreatePreparedText(const char* text, float fontSize)
{
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text     = Dali::String(text);
  input.fontSize = fontSize;
  return Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);
}

void CheckGlyphMappingConsistency(const Dali::Ui::Internal::TextVisualizer::PreparedText& preparedText)
{
  const uint32_t glyphCount     = preparedText.GetGlyphCount();
  const uint32_t characterCount = preparedText.GetCharacterCount();

  DALI_TEST_EQUALS(preparedText.GetGlyphs().Count(), glyphCount, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetGlyphToCharacterMap().Count(), glyphCount, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetCharactersPerGlyph().Count(), glyphCount, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetCharacterToGlyphTable().Count(), characterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetGlyphsPerCharacterTable().Count(), characterCount, TEST_LOCATION);
}

bool HasAnyValidGlyphMetric(const Dali::Ui::Internal::TextVisualizer::PreparedText& preparedText)
{
  const Dali::Vector<Dali::Ui::Text::GlyphInfo>& glyphs = preparedText.GetGlyphs();

  for(Dali::Vector<Dali::Ui::Text::GlyphInfo>::ConstIterator it = glyphs.Begin(), endIt = glyphs.End(); it != endIt; ++it)
  {
    if((it->advance > 0.0f) || (it->width > 0.0f) || (it->height > 0.0f))
    {
      return true;
    }
  }

  return false;
}

float GetGlyphPlacementAdvanceForTest(const Dali::Ui::Text::GlyphInfo& glyph)
{
  if(glyph.advance > 0.0f)
  {
    return glyph.advance;
  }

  return glyph.width > 0.0f ? glyph.width : 0.0f;
}

float GetGlyphRangeAdvanceForTest(const Dali::Ui::Internal::TextVisualizer::PreparedText& preparedText,
                                  uint32_t                                                glyphStart,
                                  uint32_t                                                glyphEnd)
{
  const Dali::Vector<Dali::Ui::Text::GlyphInfo>& glyphs = preparedText.GetGlyphs();
  float                                          advance = 0.0f;

  for(uint32_t glyphIndex = glyphStart; glyphIndex < glyphEnd && glyphIndex < glyphs.Count(); ++glyphIndex)
  {
    advance += GetGlyphPlacementAdvanceForTest(glyphs[glyphIndex]);
  }

  return advance;
}

uint32_t CountRenderableGlyphPlacementsForTest(const Dali::Ui::Internal::TextVisualizer::PreparedText& preparedText,
                                               const Dali::Ui::Internal::TextVisualizer::LayoutResult&  layoutResult)
{
  uint32_t renderableGlyphCount = 0u;

  const Dali::Vector<Dali::Ui::Text::GlyphInfo>& glyphs = preparedText.GetGlyphs();
  for(Dali::Vector<Dali::Ui::Internal::TextVisualizer::GlyphPlacement>::ConstIterator it = layoutResult.glyphPlacements.Begin(), endIt = layoutResult.glyphPlacements.End();
      it != endIt; ++it)
  {
    if(it->glyphIndex < glyphs.Count())
    {
      const Dali::Ui::Text::GlyphInfo& glyph = glyphs[it->glyphIndex];
      if((glyph.width > 0.0f) || (glyph.height > 0.0f))
      {
        ++renderableGlyphCount;
      }
    }
  }

  return renderableGlyphCount;
}

uint32_t FindFirstAllowedLineBreakGlyphEndForTest(const Dali::Ui::Internal::TextVisualizer::PreparedText& preparedText)
{
  const Dali::Vector<Dali::Ui::Text::CharacterIndex>& glyphToCharacterMap = preparedText.GetGlyphToCharacterMap();
  const Dali::Vector<Dali::Ui::Text::Length>&         charactersPerGlyph  = preparedText.GetCharactersPerGlyph();
  const Dali::Vector<Dali::Ui::Text::LineBreakInfo>&  lineBreakInfo       = preparedText.GetLineBreakInfo();

  for(uint32_t glyphIndex = 0u; glyphIndex < preparedText.GetGlyphCount() && glyphIndex < glyphToCharacterMap.Count(); ++glyphIndex)
  {
    const uint32_t characterStart = glyphToCharacterMap[glyphIndex];
    const uint32_t characterSpan  = glyphIndex < charactersPerGlyph.Count() ? charactersPerGlyph[glyphIndex] : 0u;
    if(characterSpan == 0u)
    {
      continue;
    }

    const uint32_t characterLast = characterStart + characterSpan - 1u;
    if(characterLast < lineBreakInfo.Count() &&
       ((lineBreakInfo[characterLast] == TextAbstraction::LINE_ALLOW_BREAK) ||
        (lineBreakInfo[characterLast] == TextAbstraction::LINE_MUST_BREAK)))
    {
      return glyphIndex + 1u;
    }
  }

  return 0u;
}

void CheckGlyphPlacementsOutsideExclusion(const Dali::Ui::Internal::TextVisualizer::LayoutResult& result,
                                          const Rect<float>&                                      exclusionRegion)
{
  const float regionLeft   = exclusionRegion.x;
  const float regionRight  = exclusionRegion.x + exclusionRegion.width;
  const float regionTop    = exclusionRegion.y;
  const float regionBottom = exclusionRegion.y + exclusionRegion.height;
  const float tolerance    = 0.5f;

  for(Dali::Vector<Dali::Ui::Internal::TextVisualizer::GlyphPlacement>::ConstIterator it = result.glyphPlacements.Begin(),
                                                                                      endIt = result.glyphPlacements.End();
      it != endIt; ++it)
  {
    const bool overlapsVerticalBand = (it->y < regionBottom - tolerance) && ((it->y + std::max(it->height, 1.0f)) > regionTop + tolerance);
    const bool insideBlockedX       = (it->x >= regionLeft - tolerance) && (it->x < regionRight - tolerance);
    DALI_TEST_CHECK(!(overlapsVerticalBand && insideBlockedX));
  }
}

void CheckGlyphPositionsOutsideExclusion(const Dali::Vector<Vector2>& positions,
                                         const Rect<float>&          exclusionRegion)
{
  const float regionLeft   = exclusionRegion.x;
  const float regionRight  = exclusionRegion.x + exclusionRegion.width;
  const float regionTop    = exclusionRegion.y;
  const float regionBottom = exclusionRegion.y + exclusionRegion.height;
  // Renderer glyph positions include glyph bearings, so allow a little more
  // tolerance than layout placement checks at exclusion interval edges.
  const float tolerance    = 3.0f;

  for(Dali::Vector<Vector2>::ConstIterator it = positions.Begin(), endIt = positions.End(); it != endIt; ++it)
  {
    const bool overlapsVerticalBand = (it->y >= regionTop + tolerance) && (it->y < regionBottom - tolerance);
    const bool insideBlockedX       = (it->x >= regionLeft - tolerance) && (it->x < regionRight - tolerance);
    DALI_TEST_CHECK(!(overlapsVerticalBand && insideBlockedX));
  }
}

void CheckGlyphPositionsFinite(const Dali::Vector<Vector2>& positions)
{
  for(Dali::Vector<Vector2>::ConstIterator it = positions.Begin(), endIt = positions.End(); it != endIt; ++it)
  {
    DALI_TEST_CHECK(std::isfinite(it->x));
    DALI_TEST_CHECK(std::isfinite(it->y));
  }
}
} // namespace

void utc_dali_text_visualizer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_visualizer_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextVisualizerConstructorP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer;
  DALI_TEST_CHECK(!textVisualizer);
  END_TEST;
}

int UtcDaliTextVisualizerNewP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);
  END_TEST;
}

int UtcDaliTextVisualizerDownCastP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  BaseHandle        object(textVisualizer);
  TextVisualizer    textVisualizer2 = TextVisualizer::DownCast(object);
  TextVisualizer    textVisualizer3 = DownCast<TextVisualizer>(object);
  DALI_TEST_CHECK(textVisualizer2);
  DALI_TEST_CHECK(textVisualizer3);
  END_TEST;
}

int UtcDaliTextVisualizerText(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("Hello world");
  DALI_TEST_EQUALS(textVisualizer.GetText(), std::string("Hello world"), TEST_LOCATION);

  textVisualizer.SetText("Updated text");
  DALI_TEST_EQUALS(textVisualizer.GetText(), std::string("Updated text"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerFontFamily(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetFontFamily("Arial");
  DALI_TEST_EQUALS(textVisualizer.GetFontFamily(), std::string("Arial"), TEST_LOCATION);

  textVisualizer.SetFontFamily("Roboto");
  DALI_TEST_EQUALS(textVisualizer.GetFontFamily(), std::string("Roboto"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerFontSize(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetFontSize(20.0f);
  DALI_TEST_EQUALS(textVisualizer.GetFontSize(), 20.0f, TEST_LOCATION);

  textVisualizer.SetFontSize(32.5f);
  DALI_TEST_EQUALS(textVisualizer.GetFontSize(), 32.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerTextColor(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  UiColor color(Color::BLUE);
  textVisualizer.SetTextColor(color);
  DALI_TEST_EQUALS(textVisualizer.GetTextColor().Resolve(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  textVisualizer.SetTextColor(color2);
  DALI_TEST_EQUALS(textVisualizer.GetTextColor().Resolve(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLineHeight(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), Text::LINE_HEIGHT_AUTO, TEST_LOCATION);

  textVisualizer.SetLineHeight(1.5f);
  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), 1.5f, TEST_LOCATION);

  textVisualizer.SetLineHeight(0.0f);
  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), Text::LINE_HEIGHT_AUTO, TEST_LOCATION);

  textVisualizer.ClearLineHeight();
  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), Text::LINE_HEIGHT_AUTO, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerSetProperty(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetProperty(TextVisualizer::Property::TEXT, "Property text");
  DALI_TEST_EQUALS(textVisualizer.GetText(), std::string("Property text"), TEST_LOCATION);

  textVisualizer.SetProperty(TextVisualizer::Property::FONT_FAMILY, "Sans");
  DALI_TEST_EQUALS(textVisualizer.GetFontFamily(), std::string("Sans"), TEST_LOCATION);

  textVisualizer.SetProperty(TextVisualizer::Property::FONT_SIZE, 18.0f);
  DALI_TEST_EQUALS(textVisualizer.GetFontSize(), 18.0f, TEST_LOCATION);

  textVisualizer.SetProperty(TextVisualizer::Property::TEXT_COLOR, Color::GREEN);
  DALI_TEST_EQUALS(textVisualizer.GetTextColor().Resolve(), Color::GREEN, TEST_LOCATION);

  textVisualizer.SetProperty(TextVisualizer::Property::LINE_HEIGHT, 1.4f);
  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), 1.4f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGetProperty(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  DALI_TEST_CHECK(textVisualizer.GetPropertyIndex(PROPERTY_NAME_TEXT) == TextVisualizer::Property::TEXT);
  DALI_TEST_CHECK(textVisualizer.GetPropertyIndex(PROPERTY_NAME_FONT_FAMILY) == TextVisualizer::Property::FONT_FAMILY);
  DALI_TEST_CHECK(textVisualizer.GetPropertyIndex(PROPERTY_NAME_FONT_SIZE) == TextVisualizer::Property::FONT_SIZE);
  DALI_TEST_CHECK(textVisualizer.GetPropertyIndex(PROPERTY_NAME_TEXT_COLOR) == TextVisualizer::Property::TEXT_COLOR);
  DALI_TEST_CHECK(textVisualizer.GetPropertyIndex(PROPERTY_NAME_LINE_HEIGHT) == TextVisualizer::Property::LINE_HEIGHT);

  textVisualizer.SetText("Property value");
  textVisualizer.SetFontFamily("Serif");
  textVisualizer.SetFontSize(24.0f);
  textVisualizer.SetTextColor(UiColor(Color::MAGENTA));
  textVisualizer.SetLineHeight(1.3f);

  DALI_TEST_EQUALS(textVisualizer.GetProperty<Dali::String>(TextVisualizer::Property::TEXT), Dali::String("Property value"), TEST_LOCATION);
  DALI_TEST_EQUALS(textVisualizer.GetProperty<Dali::String>(TextVisualizer::Property::FONT_FAMILY), Dali::String("Serif"), TEST_LOCATION);
  DALI_TEST_EQUALS(textVisualizer.GetProperty<float>(TextVisualizer::Property::FONT_SIZE), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(textVisualizer.GetProperty<Vector4>(TextVisualizer::Property::TEXT_COLOR), Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_EQUALS(textVisualizer.GetProperty<float>(TextVisualizer::Property::LINE_HEIGHT), 1.3f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLineHeightPropertyAutoP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();

  textVisualizer.SetProperty(TextVisualizer::Property::LINE_HEIGHT, -1.0f);
  DALI_TEST_EQUALS(textVisualizer.GetProperty<float>(TextVisualizer::Property::LINE_HEIGHT), Text::LINE_HEIGHT_AUTO, TEST_LOCATION);

  textVisualizer.SetProperty(TextVisualizer::Property::LINE_HEIGHT, -3.0f);
  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), Text::LINE_HEIGHT_AUTO, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPrepareP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.Prepare();
  textVisualizer.Prepare();

  END_TEST;
}

int UtcDaliTextVisualizerExclusionRegions(void)
{
  UiTestApplication         application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> regions;
  DALI_TEST_CHECK(textVisualizer);

  regions.PushBack(Rect<float>(0.0f, 0.0f, 100.0f, 30.0f));
  regions.PushBack(Rect<float>(50.0f, 40.0f, 80.0f, 20.0f));

  textVisualizer.SetExclusionRegions(regions);

  const Dali::Vector<Rect<float>> storedRegions = textVisualizer.GetExclusionRegions();
  DALI_TEST_EQUALS(storedRegions.Count(), regions.Count(), TEST_LOCATION);
  DALI_TEST_CHECK(storedRegions[0] == regions[0]);
  DALI_TEST_CHECK(storedRegions[1] == regions[1]);

  END_TEST;
}

int UtcDaliTextVisualizerClearExclusionRegions(void)
{
  UiTestApplication         application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> regions;
  DALI_TEST_CHECK(textVisualizer);

  regions.PushBack(Rect<float>(10.0f, 20.0f, 30.0f, 40.0f));
  textVisualizer.SetExclusionRegions(regions);
  DALI_TEST_EQUALS(textVisualizer.GetExclusionRegions().Count(), 1u, TEST_LOCATION);

  textVisualizer.ClearExclusionRegions();
  DALI_TEST_EQUALS(textVisualizer.GetExclusionRegions().Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerExclusionRegionsSameCountUpdateP(void)
{
  UiTestApplication         application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> regionsA;
  Dali::Vector<Rect<float>> regionsB;
  DALI_TEST_CHECK(textVisualizer);

  regionsA.PushBack(Rect<float>(0.0f, 0.0f, 100.0f, 30.0f));
  regionsA.PushBack(Rect<float>(50.0f, 40.0f, 80.0f, 20.0f));
  regionsB.PushBack(Rect<float>(10.0f, 20.0f, 90.0f, 35.0f));
  regionsB.PushBack(Rect<float>(70.0f, 45.0f, 60.0f, 25.0f));

  textVisualizer.SetExclusionRegions(regionsA);
  textVisualizer.SetExclusionRegions(regionsB);

  const Dali::Vector<Rect<float>> storedRegions = textVisualizer.GetExclusionRegions();
  DALI_TEST_EQUALS(storedRegions.Count(), regionsB.Count(), TEST_LOCATION);
  DALI_TEST_CHECK(storedRegions[0] == regionsB[0]);
  DALI_TEST_CHECK(storedRegions[1] == regionsB[1]);

  END_TEST;
}

int UtcDaliTextVisualizerExclusionRegionsDifferentCountUpdateP(void)
{
  UiTestApplication         application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> regionsA;
  Dali::Vector<Rect<float>> regionsB;
  DALI_TEST_CHECK(textVisualizer);

  regionsA.PushBack(Rect<float>(0.0f, 0.0f, 100.0f, 30.0f));
  regionsB.PushBack(Rect<float>(10.0f, 20.0f, 90.0f, 35.0f));
  regionsB.PushBack(Rect<float>(70.0f, 45.0f, 60.0f, 25.0f));
  regionsB.PushBack(Rect<float>(140.0f, 80.0f, 40.0f, 30.0f));

  textVisualizer.SetExclusionRegions(regionsA);
  textVisualizer.SetExclusionRegions(regionsB);

  const Dali::Vector<Rect<float>> storedRegions = textVisualizer.GetExclusionRegions();
  DALI_TEST_EQUALS(storedRegions.Count(), regionsB.Count(), TEST_LOCATION);
  DALI_TEST_CHECK(storedRegions[0] == regionsB[0]);
  DALI_TEST_CHECK(storedRegions[1] == regionsB[1]);
  DALI_TEST_CHECK(storedRegions[2] == regionsB[2]);

  END_TEST;
}

int UtcDaliTextVisualizerExclusionRegionsSameValuesNoOpP(void)
{
  UiTestApplication         application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> regions;
  DALI_TEST_CHECK(textVisualizer);

  regions.PushBack(Rect<float>(0.0f, 0.0f, 100.0f, 30.0f));
  regions.PushBack(Rect<float>(50.0f, 40.0f, 80.0f, 20.0f));

  textVisualizer.SetExclusionRegions(regions);
  textVisualizer.SetExclusionRegions(regions);

  const Dali::Vector<Rect<float>> storedRegions = textVisualizer.GetExclusionRegions();
  DALI_TEST_EQUALS(storedRegions.Count(), regions.Count(), TEST_LOCATION);
  DALI_TEST_CHECK(storedRegions[0] == regions[0]);
  DALI_TEST_CHECK(storedRegions[1] == regions[1]);

  END_TEST;
}

int UtcDaliTextVisualizerPrepareAfterStateChangesP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("Prepared text");
  textVisualizer.SetFontFamily("Roboto");
  textVisualizer.SetFontSize(28.0f);
  textVisualizer.Prepare();

  DALI_TEST_EQUALS(textVisualizer.GetText(), std::string("Prepared text"), TEST_LOCATION);
  DALI_TEST_EQUALS(textVisualizer.GetFontFamily(), std::string("Roboto"), TEST_LOCATION);
  DALI_TEST_EQUALS(textVisualizer.GetFontSize(), 28.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerExclusionRegionsWithoutPrepareP(void)
{
  UiTestApplication         application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> regions;
  DALI_TEST_CHECK(textVisualizer);

  regions.PushBack(Rect<float>(5.0f, 5.0f, 25.0f, 15.0f));
  textVisualizer.SetExclusionRegions(regions);
  textVisualizer.SetExclusionRegions(regions);
  textVisualizer.ClearExclusionRegions();

  DALI_TEST_EQUALS(textVisualizer.GetExclusionRegions().Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPrepareEmptyTextP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("");
  textVisualizer.Prepare();

  END_TEST;
}

int UtcDaliTextVisualizerPreparedAsciiCharacterCountP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "abc";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetCharacterCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetClusterCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetLineBreakCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetParagraphCount(), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedKoreanCharacterCountP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "가나다";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetCharacterCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetClusterCount(), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedEmojiCharacterCountP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "😀";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_CHECK(preparedText.GetCharacterCount() > 0u);
  DALI_TEST_CHECK(preparedText.GetCharacterCount() < input.text.Size());
  DALI_TEST_EQUALS(preparedText.GetClusterCount(), preparedText.GetCharacterCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedMixedTextP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "A가😀";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetCharacterCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetCharacters().Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetClusterCount(), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedEmptyTextP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_CHECK(preparedText.Empty());
  DALI_TEST_EQUALS(preparedText.GetCharacterCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetClusterCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetScriptRunCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetFontRunCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetGlyphCount(), 0u, TEST_LOCATION);
  CheckGlyphMappingConsistency(preparedText);

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 100.0f, 12.0f, exclusionRegions, layoutResult);
  DALI_TEST_CHECK(layoutResult.Empty());

  END_TEST;
}

int UtcDaliTextVisualizerPreparedAsciiLineMetricsP(void)
{
  UiTestApplication application;
  const auto        preparedText = CreatePreparedText("abc", 20.0f);

  DALI_TEST_CHECK(preparedText.IsPrepared());

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(preparedText.HasLineMetrics());
    DALI_TEST_CHECK(preparedText.GetLineMetrics().naturalLineHeight > 0.0f);
    DALI_TEST_CHECK(preparedText.GetLineMetrics().baselineOffset >= 0.0f);
  }
  else
  {
    DALI_TEST_CHECK(!preparedText.HasLineMetrics());
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedKoreanLineMetricsP(void)
{
  UiTestApplication application;
  const auto        preparedText = CreatePreparedText("가나다", 20.0f);

  DALI_TEST_CHECK(preparedText.IsPrepared());

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(preparedText.HasLineMetrics());
    DALI_TEST_CHECK(preparedText.GetLineMetrics().naturalLineHeight > 0.0f);
    DALI_TEST_CHECK(preparedText.GetLineMetrics().baselineOffset >= 0.0f);
  }
  else
  {
    DALI_TEST_CHECK(!preparedText.HasLineMetrics());
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedEmptyLineMetricsP(void)
{
  UiTestApplication application;
  const auto        preparedText = CreatePreparedText("", 20.0f);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_CHECK(!preparedText.HasLineMetrics());
  DALI_TEST_EQUALS(preparedText.GetLineMetrics().naturalLineHeight, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedLineMetricsClearP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("abc", 20.0f);

  preparedText.Clear();

  DALI_TEST_CHECK(!preparedText.HasLineMetrics());
  DALI_TEST_EQUALS(preparedText.GetLineMetrics().naturalLineHeight, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(preparedText.GetLineMetrics().baselineOffset, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerTextPreparerFontSizePixelSmokeP(void)
{
  UiTestApplication application;
  const auto        preparedText = CreatePreparedText("abc", 20.0f);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetFontSize(), 20.0f, TEST_LOCATION);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(preparedText.HasGlyphMetrics());
    DALI_TEST_CHECK(preparedText.GetLineMetrics().naturalLineHeight > 0.0f);
  }

  END_TEST;
}

int UtcDaliTextVisualizerTextPreparerFontSizeAffectsGlyphMetricsP(void)
{
  UiTestApplication application;
  const auto        smallPreparedText = CreatePreparedText("abc", 10.0f);
  const auto        largePreparedText = CreatePreparedText("abc", 30.0f);

  DALI_TEST_CHECK(smallPreparedText.IsPrepared());
  DALI_TEST_CHECK(largePreparedText.IsPrepared());
  DALI_TEST_EQUALS(smallPreparedText.GetFontSize(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(largePreparedText.GetFontSize(), 30.0f, TEST_LOCATION);

  if(smallPreparedText.HasGlyphMetrics() && largePreparedText.HasGlyphMetrics())
  {
    const bool totalAdvanceGrew = largePreparedText.GetTotalGlyphAdvance() > smallPreparedText.GetTotalGlyphAdvance();
    const bool lineHeightGrew   = largePreparedText.GetLineMetrics().naturalLineHeight > smallPreparedText.GetLineMetrics().naturalLineHeight;
    DALI_TEST_CHECK(totalAdvanceGrew || lineHeightGrew);
  }

  END_TEST;
}

int UtcDaliTextVisualizerTextPreparerDefaultFontSizeFallbackP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text     = "abc";
  input.fontSize = 0.0f;

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetFontSize(), 0.0f, TEST_LOCATION);

  if(preparedText.GetGlyphCount() > 0u)
  {
    CheckGlyphMappingConsistency(preparedText);
  }

  END_TEST;
}

int UtcDaliTextVisualizerTextLineMetricsDefaultP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextLineMetrics metrics;
  Dali::Ui::Internal::TextVisualizer::TextLine        line;

  DALI_TEST_CHECK(!metrics.valid);
  DALI_TEST_EQUALS(metrics.naturalLineHeight, 0.0f, TEST_LOCATION);
  DALI_TEST_CHECK(!line.metrics.valid);

  line.metrics.valid             = true;
  line.metrics.naturalLineHeight = 10.0f;
  line.Clear();
  DALI_TEST_CHECK(!line.metrics.valid);
  DALI_TEST_EQUALS(line.metrics.naturalLineHeight, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutGlyphsUsesLineMetricsHeightP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("abc", 20.0f);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 300.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.HasLineMetrics() && (preparedText.GetGlyphCount() > 0u))
  {
    DALI_TEST_CHECK(!layoutResult.Empty());
    DALI_TEST_CHECK(layoutResult.height >= preparedText.GetLineMetrics().naturalLineHeight);
  }

  END_TEST;
}

int UtcDaliTextVisualizerLayoutGlyphsFillsTextLineMetricsP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("abc", 20.0f);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 300.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u && layoutResult.GetLineCount() > 0u)
  {
    const Dali::Ui::Internal::TextVisualizer::TextLine& line = layoutResult.lines[0u];
    DALI_TEST_CHECK(line.metrics.valid);
    DALI_TEST_CHECK(line.metrics.naturalLineHeight > 0.0f);
    DALI_TEST_CHECK(line.metrics.baselineOffset >= 0.0f);
  }

  END_TEST;
}

int UtcDaliTextVisualizerLayoutGlyphsExplicitLineHeightOverrideP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("abc", 20.0f);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  constexpr float                                  explicitLineHeight = 50.0f;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 300.0f, explicitLineHeight, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(!layoutResult.Empty());
    DALI_TEST_EQUALS(layoutResult.height, explicitLineHeight, TEST_LOCATION);
    DALI_TEST_EQUALS(layoutResult.lines[0u].height, explicitLineHeight, TEST_LOCATION);
    DALI_TEST_CHECK(layoutResult.lines[0u].metrics.valid);
  }

  END_TEST;
}

int UtcDaliTextVisualizerLayoutGlyphsRelativeLineHeightFormulaP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("abc", 20.0f);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  constexpr float                                  fontSize              = 20.0f;
  constexpr float                                  relativeLineHeight    = 1.5f;
  constexpr float                                  calculatedLineHeight  = fontSize * relativeLineHeight;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 300.0f, calculatedLineHeight, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(!layoutResult.Empty());
    DALI_TEST_EQUALS(layoutResult.height, calculatedLineHeight, TEST_LOCATION);
    DALI_TEST_EQUALS(layoutResult.lines[0u].height, calculatedLineHeight, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerLineHeightChangeSmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("Line height smoke line height smoke line height smoke");
  textVisualizer.SetFontSize(20.0f);
  textVisualizer.SetRequestedWidth(180.0f);
  textVisualizer.SetRequestedHeight(240.0f);
  textVisualizer.SetLineHeight(3.0f);
  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), 3.0f, TEST_LOCATION);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  MeasuredSize measured = textVisualizer.Measure(180.0f, 0.0f);
  DALI_TEST_CHECK(measured.GetWidth() > 0.0f);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);

  textVisualizer.SetLineHeight(Text::LINE_HEIGHT_AUTO);
  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), Text::LINE_HEIGHT_AUTO, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  measured = textVisualizer.Measure(180.0f, 0.0f);
  DALI_TEST_CHECK(measured.GetWidth() > 0.0f);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterRendererGlyphPositionUsesMetricsBaselineP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("abc", 20.0f);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter adapter;
  Vector2                                          position;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 300.0f, 0.0f, exclusionRegions, layoutResult);
  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(adapter.GetRendererGlyphPosition(0u, position));
    DALI_TEST_CHECK(std::isfinite(position.x));
    DALI_TEST_CHECK(std::isfinite(position.y));
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterRendererGlyphPositionFallbackWithoutMetricsP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("abc", 20.0f);
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedTextWithoutMetrics(preparedText);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter adapter;
  Vector2                                          position;

  preparedTextWithoutMetrics.SetLineMetrics(Dali::Ui::Internal::TextVisualizer::PreparedText::LineMetrics{});
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedTextWithoutMetrics, 300.0f, 0.0f, exclusionRegions, layoutResult);
  adapter.SetPreparedText(&preparedTextWithoutMetrics);
  adapter.SetLayoutResult(&layoutResult);

  if(preparedTextWithoutMetrics.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(adapter.GetRendererGlyphPosition(0u, position));
    DALI_TEST_CHECK(std::isfinite(position.x));
    DALI_TEST_CHECK(std::isfinite(position.y));
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterLineMetricsCacheBuildsP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("line metrics cache smoke", 20.0f);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter adapter;
  Vector2                                          position;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 160.0f, 0.0f, exclusionRegions, layoutResult);
  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  if(preparedText.GetGlyphCount() > 0u && !layoutResult.glyphPlacements.Empty())
  {
    DALI_TEST_CHECK(adapter.GetLineMetricsCacheCount() > 0u);
    DALI_TEST_CHECK(adapter.GetRendererGlyphPosition(0u, position));
    DALI_TEST_CHECK(std::isfinite(position.x));
    DALI_TEST_CHECK(std::isfinite(position.y));
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterLineMetricsCacheClearP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("line metrics clear", 20.0f);
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter adapter;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 160.0f, 0.0f, exclusionRegions, layoutResult);
  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  if(preparedText.GetGlyphCount() > 0u && !layoutResult.glyphPlacements.Empty())
  {
    DALI_TEST_CHECK(adapter.GetLineMetricsCacheCount() > 0u);
  }

  adapter.Clear();
  DALI_TEST_EQUALS(adapter.GetLineMetricsCacheCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterLineMetricsCacheEmptyLayoutP(void)
{
  UiTestApplication                                application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText = CreatePreparedText("line metrics empty", 20.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter adapter;
  Vector2                                          position;

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  DALI_TEST_EQUALS(adapter.GetLineMetricsCacheCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!adapter.GetRendererGlyphPosition(0u, position));

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterRendererGlyphPositionCacheBuildsP(void)
{
  UiTestApplication                                     application;
  const auto                                            preparedText = CreatePreparedText("position cache", 20.0f);
  Dali::Vector<Rect<float>>                             exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult      layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter  adapter;
  Vector2                                               position;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 180.0f, 0.0f, exclusionRegions, layoutResult);
  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  if(preparedText.GetGlyphCount() > 0u && adapter.HasRenderableGlyphs())
  {
    DALI_TEST_EQUALS(adapter.GetRendererGlyphPositionCacheCount(), layoutResult.glyphPlacements.Count(), TEST_LOCATION);
    DALI_TEST_CHECK(adapter.GetRendererGlyphPosition(0u, position));
    DALI_TEST_CHECK(std::isfinite(position.x));
    DALI_TEST_CHECK(std::isfinite(position.y));
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterRendererGlyphPositionCacheClearP(void)
{
  UiTestApplication                                     application;
  const auto                                            preparedText = CreatePreparedText("position cache clear", 20.0f);
  Dali::Vector<Rect<float>>                             exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult      layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter  adapter;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 180.0f, 0.0f, exclusionRegions, layoutResult);
  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  if(preparedText.GetGlyphCount() > 0u && adapter.HasRenderableGlyphs())
  {
    DALI_TEST_EQUALS(adapter.GetRendererGlyphPositionCacheCount(), layoutResult.glyphPlacements.Count(), TEST_LOCATION);
  }

  adapter.Clear();
  DALI_TEST_EQUALS(adapter.GetRendererGlyphPositionCacheCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedLineBreakInfoSmokeP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "abc\ndef";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_CHECK(preparedText.GetLineBreakCount() > 0u);
  DALI_TEST_CHECK(preparedText.GetParagraphCount() > 0u);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedAsciiScriptAndFontDataP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text     = "abc";
  input.fontSize = 10.0f;

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetCharacterCount(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(preparedText.GetScriptRunCount() > 0u);
  DALI_TEST_EQUALS(preparedText.GetScriptRuns().Count(), preparedText.GetScriptRunCount(), TEST_LOCATION);

  if(preparedText.GetScriptRunCount() > 0u)
  {
    DALI_TEST_CHECK(preparedText.GetScriptRuns()[0].characterRun.numberOfCharacters > 0u);
  }

  DALI_TEST_EQUALS(preparedText.GetFontRuns().Count(), preparedText.GetFontRunCount(), TEST_LOCATION);
  if(preparedText.GetFontRunCount() > 0u)
  {
    DALI_TEST_CHECK(preparedText.GetFontRuns()[0].characterRun.numberOfCharacters > 0u);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedKoreanScriptAndFontDataP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text     = "가나다";
  input.fontSize = 10.0f;

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetCharacterCount(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(preparedText.GetScriptRunCount() > 0u);

  if(preparedText.GetFontRunCount() > 0u)
  {
    DALI_TEST_CHECK(preparedText.GetFontRuns()[0].characterRun.numberOfCharacters > 0u);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedEmojiScriptAndFontDataP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "😀";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_CHECK(preparedText.GetScriptRunCount() > 0u);
  DALI_TEST_EQUALS(preparedText.GetScriptRuns().Count(), preparedText.GetScriptRunCount(), TEST_LOCATION);

  if(preparedText.GetFontRunCount() > 0u)
  {
    DALI_TEST_CHECK(preparedText.GetFontRuns()[0].characterRun.numberOfCharacters > 0u);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedMixedScriptDataP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "abc가나다";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_CHECK(preparedText.GetScriptRunCount() > 0u);
  DALI_TEST_EQUALS(preparedText.GetScriptRuns().Count(), preparedText.GetScriptRunCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedAsciiShapedGlyphDataP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text     = "abc";
  input.fontSize = 10.0f;

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetCharacterCount(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(preparedText.GetGlyphCount() > 0u);
  CheckGlyphMappingConsistency(preparedText);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedKoreanShapedGlyphDataP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text     = "가나다";
  input.fontSize = 10.0f;

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_EQUALS(preparedText.GetCharacterCount(), 3u, TEST_LOCATION);

  if(preparedText.GetGlyphCount() > 0u)
  {
    CheckGlyphMappingConsistency(preparedText);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedEmojiShapedGlyphDataP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "😀";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());

  if(preparedText.GetGlyphCount() > 0u)
  {
    CheckGlyphMappingConsistency(preparedText);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedMixedShapedGlyphDataP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "A가😀";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());

  if(preparedText.GetGlyphCount() > 0u)
  {
    CheckGlyphMappingConsistency(preparedText);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedAsciiGlyphMetricsP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text     = "abc";
  input.fontSize = 10.0f;

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());
  DALI_TEST_CHECK(preparedText.HasGlyphData());
  DALI_TEST_CHECK(preparedText.GetGlyphCount() > 0u);
  CheckGlyphMappingConsistency(preparedText);
  DALI_TEST_CHECK(HasAnyValidGlyphMetric(preparedText));
  DALI_TEST_CHECK(preparedText.HasGlyphMetrics());
  DALI_TEST_CHECK(preparedText.GetTotalGlyphAdvance() >= 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerPreparedKoreanGlyphMetricsP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text     = "가나다";
  input.fontSize = 10.0f;

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());

  if(preparedText.GetGlyphCount() > 0u)
  {
    CheckGlyphMappingConsistency(preparedText);
    DALI_TEST_CHECK(HasAnyValidGlyphMetric(preparedText));
    DALI_TEST_CHECK(preparedText.HasGlyphMetrics());
    DALI_TEST_CHECK(preparedText.GetTotalGlyphAdvance() >= 0.0f);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedEmojiGlyphMetricsP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextPreparer::Input input;
  input.text = "😀";

  const auto preparedText = Dali::Ui::Internal::TextVisualizer::TextPreparer::Prepare(input);

  DALI_TEST_CHECK(preparedText.IsPrepared());

  if(preparedText.GetGlyphCount() > 0u)
  {
    CheckGlyphMappingConsistency(preparedText);
    DALI_TEST_CHECK(HasAnyValidGlyphMetric(preparedText));
    DALI_TEST_CHECK(preparedText.HasGlyphMetrics());
    DALI_TEST_CHECK(preparedText.GetTotalGlyphAdvance() >= 0.0f);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedGlyphLayoutDataAsciiP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abc", 18.0f);

  if(preparedText.GetGlyphCount() > 0u)
  {
    const auto& glyphLayoutData = preparedText.GetGlyphLayoutData();
    DALI_TEST_CHECK(preparedText.HasGlyphLayoutData());
    DALI_TEST_EQUALS(glyphLayoutData.advances.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(glyphLayoutData.widths.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(glyphLayoutData.prefixAdvances.Count(), preparedText.GetGlyphCount() + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(glyphLayoutData.characterStarts.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(glyphLayoutData.characterEnds.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(glyphLayoutData.breakAllowedAfterGlyph.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(glyphLayoutData.breakMandatoryAfterGlyph.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_CHECK(std::fabs(glyphLayoutData.prefixAdvances[0u]) < 0.001f);
    DALI_TEST_CHECK(std::fabs(glyphLayoutData.prefixAdvances[preparedText.GetGlyphCount()] - preparedText.GetTotalGlyphAdvance()) < 0.001f);
  }

  END_TEST;
}

int UtcDaliTextVisualizerPreparedGlyphLayoutDataBreakInfoP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("hello world", 18.0f);

  if(preparedText.GetGlyphCount() > 0u)
  {
    const auto& glyphLayoutData = preparedText.GetGlyphLayoutData();
    bool        hasAllowedBreak = false;

    DALI_TEST_CHECK(preparedText.HasGlyphLayoutData());

    for(uint32_t glyphIndex = 0u; glyphIndex < glyphLayoutData.breakAllowedAfterGlyph.Count(); ++glyphIndex)
    {
      hasAllowedBreak = hasAllowedBreak || (glyphLayoutData.breakAllowedAfterGlyph[glyphIndex] != 0u);
    }

    DALI_TEST_CHECK(hasAllowedBreak);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutBasicAsciiP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_CHECK(layoutResult.GetLineCount() >= 1u);
    DALI_TEST_CHECK(layoutResult.height > 0.0f);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutUsesPreparedGlyphLayoutDataSmokeP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("hello world", 18.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  DALI_TEST_CHECK(preparedText.HasGlyphLayoutData());

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutFallbackWithoutPreparedGlyphLayoutDataP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("hello world", 18.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  preparedText.ClearGlyphLayoutData();
  DALI_TEST_CHECK(!preparedText.HasGlyphLayoutData());

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutWrapsByWidthP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghij", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult narrowLayoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutResult wideLayoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 25.0f, 0.0f, exclusionRegions, narrowLayoutResult);
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 100.0f, 0.0f, exclusionRegions, wideLayoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(narrowLayoutResult.GetLineCount() >= wideLayoutResult.GetLineCount());
    DALI_TEST_EQUALS(narrowLayoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(wideLayoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutWithExclusionP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghij", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(10.0f, 0.0f, 25.0f, 20.0f));

  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 120.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    CheckGlyphPlacementsOutsideExclusion(layoutResult, exclusionRegions[0]);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutWordWrapSpacesP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("hello world", 18.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  const uint32_t firstBreakGlyphEnd = FindFirstAllowedLineBreakGlyphEndForTest(preparedText);
  if(preparedText.GetGlyphCount() > 0u && firstBreakGlyphEnd > 0u && firstBreakGlyphEnd < preparedText.GetGlyphCount())
  {
    const float firstBreakAdvance = GetGlyphRangeAdvanceForTest(preparedText, 0u, firstBreakGlyphEnd);
    const float totalAdvance      = preparedText.GetTotalGlyphAdvance();
    if(firstBreakAdvance > 0.0f && firstBreakAdvance < totalAdvance)
    {
      Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, firstBreakAdvance + 1.0f, 0.0f, exclusionRegions, layoutResult);

      DALI_TEST_CHECK(layoutResult.GetLineCount() >= 2u);
      DALI_TEST_CHECK(layoutResult.lines[0u].fragments.Count() > 0u);
      DALI_TEST_EQUALS(layoutResult.lines[0u].fragments[0u].glyphEnd, firstBreakGlyphEnd, TEST_LOCATION);
      DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutLongWordFallbackP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("supercalifragilistic", 18.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 8.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(layoutResult.GetLineCount() >= 1u);
    DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutWordWrapWideWidthP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("one two", 18.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_EQUALS(layoutResult.GetLineCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutWordWrapWithExclusionSmokeP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("hello world dynamic text", 16.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(55.0f, 0.0f, 45.0f, 24.0f));
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 180.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(!layoutResult.Empty());
    DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    CheckGlyphPlacementsOutsideExclusion(layoutResult, exclusionRegions[0u]);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutWordWrapMixedTextSmokeP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("가나다 abc 😀 test", 16.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 90.0f, 0.0f, exclusionRegions, layoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_CHECK(!layoutResult.Empty());
    DALI_TEST_EQUALS(layoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutWordWrapSameResultP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("hello world dynamic text", 16.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult firstLayoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutResult secondLayoutResult;

  exclusionRegions.PushBack(Rect<float>(55.0f, 0.0f, 45.0f, 24.0f));

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 180.0f, 0.0f, exclusionRegions, firstLayoutResult);
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 180.0f, 0.0f, exclusionRegions, secondLayoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_EQUALS(firstLayoutResult.glyphPlacements.Count(), preparedText.GetGlyphCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(firstLayoutResult.CalculateSignature(), secondLayoutResult.CalculateSignature(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerLayoutResultSignatureSameForSameLayoutP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghij", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(10.0f, 0.0f, 25.0f, 20.0f));

  Dali::Ui::Internal::TextVisualizer::LayoutResult firstLayoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutResult secondLayoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 120.0f, 0.0f, exclusionRegions, firstLayoutResult);
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 120.0f, 0.0f, exclusionRegions, secondLayoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_EQUALS(firstLayoutResult.CalculateSignature(), secondLayoutResult.CalculateSignature(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerLayoutResultSignatureChangesForMovedExclusionP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghijabcdefghij", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegionsA;
  Dali::Vector<Rect<float>> exclusionRegionsB;
  Dali::Ui::Internal::TextVisualizer::LayoutResult firstLayoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutResult secondLayoutResult;

  exclusionRegionsA.PushBack(Rect<float>(0.0f, 0.0f, 55.0f, 24.0f));
  exclusionRegionsB.PushBack(Rect<float>(70.0f, 0.0f, 55.0f, 24.0f));

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 140.0f, 0.0f, exclusionRegionsA, firstLayoutResult);
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 140.0f, 0.0f, exclusionRegionsB, secondLayoutResult);

  if(preparedText.GetGlyphCount() > 0u && !firstLayoutResult.Empty() && !secondLayoutResult.Empty())
  {
    DALI_TEST_CHECK(firstLayoutResult.CalculateSignature() != secondLayoutResult.CalculateSignature());
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutEmptyTextP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText;
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 100.0f, 0.0f, exclusionRegions, layoutResult);

  DALI_TEST_CHECK(layoutResult.Empty());

  END_TEST;
}

int UtcDaliTextVisualizerGlyphLayoutFallbackToPlaceholderP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText;
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  preparedText.SetPrepared(true);
  preparedText.SetFontSize(10.0f);
  Dali::Vector<Dali::Ui::Text::Character> characters;
  characters.Resize(6u);
  preparedText.SetCharacters(characters);
  preparedText.SetClusterCount(6u);

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 30.0f, 0.0f, exclusionRegions, layoutResult);

  DALI_TEST_CHECK(!layoutResult.Empty());
  DALI_TEST_EQUALS(layoutResult.clusterPlacements.Count(), 6u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterEmptyStateP(void)
{
  UiTestApplication                                             application;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter adapter;

  DALI_TEST_CHECK(!adapter.HasRenderableGlyphs());
  DALI_TEST_CHECK(!adapter.HasValidGlyphPlacementIndices());
  DALI_TEST_EQUALS(adapter.GetGlyphCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetGlyphPlacementCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetGlyphs().Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetGlyphPlacements().Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetRendererGlyphPositionCacheCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterWithGlyphLayoutP(void)
{
  UiTestApplication                                     application;
  const auto                                            preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                             exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult      layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter  adapter;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  if(preparedText.GetGlyphCount() > 0u && !layoutResult.glyphPlacements.Empty())
  {
    DALI_TEST_CHECK(adapter.HasValidGlyphPlacementIndices());
    DALI_TEST_CHECK(adapter.HasRenderableGlyphs());
  }
  else
  {
    DALI_TEST_CHECK(!adapter.HasRenderableGlyphs());
  }

  DALI_TEST_EQUALS(adapter.GetGlyphCount(), preparedText.GetGlyphCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetGlyphPlacementCount(), layoutResult.glyphPlacements.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetGlyphs().Count(), preparedText.GetGlyphs().Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetGlyphPlacements().Count(), layoutResult.glyphPlacements.Count(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterClearP(void)
{
  UiTestApplication                                     application;
  const auto                                            preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                             exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult      layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter  adapter;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.Clear();

  DALI_TEST_CHECK(!adapter.HasRenderableGlyphs());
  DALI_TEST_CHECK(!adapter.HasValidGlyphPlacementIndices());
  DALI_TEST_EQUALS(adapter.GetGlyphCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetGlyphPlacementCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(adapter.GetRendererGlyphPositionCacheCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterInvalidPlacementIndexP(void)
{
  UiTestApplication                                     application;
  const auto                                            preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult      layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter  adapter;
  Dali::Ui::Internal::TextVisualizer::GlyphPlacement    placement;

  placement.glyphIndex = preparedText.GetGlyphCount() + 1u;
  placement.x          = 0.0f;
  placement.y          = 0.0f;
  placement.width      = 10.0f;
  placement.height     = 10.0f;
  placement.advance    = 10.0f;
  layoutResult.glyphPlacements.PushBack(placement);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  DALI_TEST_CHECK(!adapter.HasValidGlyphPlacementIndices());
  DALI_TEST_CHECK(!adapter.HasRenderableGlyphs());
  DALI_TEST_EQUALS(adapter.GetGlyphPlacementCount(), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererEmptyStateP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  DALI_TEST_CHECK(!renderer.HasRenderHost());
  DALI_TEST_CHECK(!renderer.HasOutputActor());
  DALI_TEST_CHECK(!renderer.IsAttached());
  DALI_TEST_CHECK(!renderer.HasMeshRecords());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());
  DALI_TEST_CHECK(!renderer.GetRenderHost());
  DALI_TEST_CHECK(!renderer.GetOutputActor());
  DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGlyphCacheSignature(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(renderer.GetLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::NONE), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureEmptyInputCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureNoRenderableGlyphsCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureNoPositionCacheCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureSignatureCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureNoGlyphManagerCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureGlyphPlacementCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureGlyphInfoCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureGlyphPositionCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureCacheMissCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureEmptyMeshCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureNoOutputActorCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureNoTextureSetCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererRenderHostSetterP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;
  Actor                                                          host = Actor::New();

  renderer.SetRenderHost(host);
  DALI_TEST_CHECK(renderer.HasRenderHost());
  DALI_TEST_CHECK(renderer.GetRenderHost() == host);

  renderer.Clear();
  DALI_TEST_CHECK(!renderer.HasRenderHost());
  DALI_TEST_CHECK(!renderer.GetRenderHost());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererEnsureOutputActorP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  renderer.EnsureOutputActor();

  DALI_TEST_CHECK(renderer.HasOutputActor());
  DALI_TEST_CHECK(renderer.GetOutputActor());
  DALI_TEST_CHECK(!renderer.IsAttached());
  DALI_TEST_CHECK(!renderer.GetOutputActor().GetParent());

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererAttachWithoutHostP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  renderer.EnsureOutputActor();

  DALI_TEST_CHECK(!renderer.AttachOutputToHost());
  DALI_TEST_CHECK(!renderer.IsAttached());
  DALI_TEST_CHECK(renderer.HasOutputActor());
  DALI_TEST_CHECK(!renderer.GetOutputActor().GetParent());

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererAttachDetachP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;
  Actor                                                          host = Actor::New();

  renderer.SetRenderHost(host);

  DALI_TEST_CHECK(renderer.AttachOutputToHost());
  DALI_TEST_CHECK(renderer.IsAttached());
  DALI_TEST_CHECK(renderer.HasOutputActor());
  DALI_TEST_CHECK(renderer.GetOutputActor().GetParent() == host);
  DALI_TEST_EQUALS(host.GetChildCount(), 1u, TEST_LOCATION);

  renderer.DetachOutputFromHost();

  DALI_TEST_CHECK(!renderer.IsAttached());
  DALI_TEST_CHECK(renderer.HasOutputActor());
  DALI_TEST_CHECK(!renderer.GetOutputActor().GetParent());
  DALI_TEST_EQUALS(host.GetChildCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererClearP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;
  Actor                                                          host = Actor::New();

  renderer.SetRenderHost(host);
  DALI_TEST_CHECK(renderer.AttachOutputToHost());

  renderer.Clear();

  DALI_TEST_CHECK(!renderer.HasRenderHost());
  DALI_TEST_CHECK(!renderer.HasOutputActor());
  DALI_TEST_CHECK(!renderer.IsAttached());
  DALI_TEST_CHECK(!renderer.GetRenderHost());
  DALI_TEST_CHECK(!renderer.GetOutputActor());
  DALI_TEST_EQUALS(host.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());
  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(renderer.GetLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::NONE), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureEmptyInputCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureCacheMissCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererMeshEmptyStateP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  DALI_TEST_CHECK(!renderer.HasMeshRecords());
  DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererGeometryOnlyInitialStateP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererClearMeshesP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  renderer.EnsureOutputActor();
  renderer.ClearMeshes();

  DALI_TEST_CHECK(renderer.HasOutputActor());
  DALI_TEST_CHECK(!renderer.HasMeshRecords());
  DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererClearClearsMeshesP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;
  Actor                                                          host = Actor::New();

  renderer.SetRenderHost(host);
  DALI_TEST_CHECK(renderer.AttachOutputToHost());

  renderer.Clear();

  DALI_TEST_CHECK(!renderer.HasRenderHost());
  DALI_TEST_CHECK(!renderer.HasOutputActor());
  DALI_TEST_CHECK(!renderer.IsAttached());
  DALI_TEST_CHECK(!renderer.HasMeshRecords());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());
  DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererGlyphCacheEmptyStateP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
  DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());
  DALI_TEST_EQUALS(renderer.GetGlyphCacheSignature(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererRenderEmptyP(void)
{
  UiTestApplication                                               application;
  Dali::Ui::Internal::TextVisualizer::PreparedText                preparedText;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter            adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  DALI_TEST_CHECK(!renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE));
  DALI_TEST_CHECK(!renderer.HasMeshRecords());
  DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
  DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());
  DALI_TEST_EQUALS(static_cast<uint32_t>(renderer.GetLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::EMPTY_INPUT), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureEmptyInputCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureCacheMissCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererRenderWithoutCachedGlyphsFailsSafelyP(void)
{
  UiTestApplication                                               application;
  const auto                                                      preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                       exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter            adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));

  const uint32_t renderableGlyphCount = CountRenderableGlyphPlacementsForTest(preparedText, layoutResult);
  const bool renderResult = renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE);
  DALI_TEST_CHECK(renderResult == renderer.HasMeshRecords());

  if(renderResult)
  {
    DALI_TEST_CHECK(renderer.HasOutputActor());
    DALI_TEST_CHECK(renderer.GetOutputActor().GetChildCount() > 0u);
    DALI_TEST_EQUALS(renderer.GetOutputActor().GetProperty<Vector3>(Actor::Property::SIZE), Vector3(layoutResult.width, layoutResult.height, 0.0f), TEST_LOCATION);
    DALI_TEST_CHECK(renderer.GetMeshRecordCount() > 0u);
    DALI_TEST_CHECK(renderer.HasMeshTopologySignature());
    DALI_TEST_CHECK(renderer.GetMeshTopologySignature() != 0u);
    DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.HasGlyphCacheEntries());
    DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), renderableGlyphCount, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.HasGlyphCacheSignature());
    DALI_TEST_CHECK(renderer.GetGlyphCacheSignature() != 0u);
    DALI_TEST_EQUALS(renderer.GetFailureCacheMissCount(), 0u, TEST_LOCATION);
  }
  else
  {
    DALI_TEST_CHECK(!renderer.HasMeshRecords());
    DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
    DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
    DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());
    DALI_TEST_CHECK(renderer.GetFailureCacheMissCount() > 0u ||
                    renderer.GetFailureEmptyMeshCount() > 0u ||
                    renderer.GetFailureNoGlyphManagerCount() > 0u ||
                    renderer.GetFailureSignatureCount() > 0u);
    if(renderer.GetFailureCacheMissCount() > 0u)
    {
      DALI_TEST_EQUALS(static_cast<uint32_t>(renderer.GetLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::GLYPH_CACHE_MISS), TEST_LOCATION);
      DALI_TEST_CHECK(renderer.GetLastFailedFontId() != 0u);
    }
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererSkipsNonRenderableGlyphCacheMissP(void)
{
  UiTestApplication                                               application;
  const auto                                                      preparedText = CreatePreparedText("a b", 10.0f);
  Dali::Vector<Rect<float>>                                       exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter            adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));

  const uint32_t renderableGlyphCount = CountRenderableGlyphPlacementsForTest(preparedText, layoutResult);
  const bool     renderResult         = renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE);
  DALI_TEST_CHECK(renderResult == renderer.HasMeshRecords());

  if(renderResult)
  {
    DALI_TEST_CHECK(renderer.HasGlyphCacheEntries());
    DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), renderableGlyphCount, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.GetGlyphCacheEntryCount() <= adapter.GetGlyphPlacementCount());
    DALI_TEST_EQUALS(renderer.GetFailureCacheMissCount(), 0u, TEST_LOCATION);
  }
  else if(renderer.GetFailureCacheMissCount() > 0u)
  {
    DALI_TEST_EQUALS(static_cast<uint32_t>(renderer.GetLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::GLYPH_CACHE_MISS), TEST_LOCATION);
    DALI_TEST_CHECK((renderer.GetLastFailedGlyphWidth() > 0.0f) || (renderer.GetLastFailedGlyphHeight() > 0.0f));
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererRenderAfterAtlasRendererWarmupSmokeP(void)
{
  UiTestApplication                                               application;
  const auto                                                      preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                       exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter            adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge         bridge;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;
  Actor                                                          renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  renderHost.SetProperty(Actor::Property::SIZE, Vector3(layoutResult.width, layoutResult.height, 0.0f));
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
    bridge.AttachRendererToHost();
  }

  const bool renderResult = renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE);
  DALI_TEST_CHECK(renderResult == renderer.HasMeshRecords());

  const uint32_t renderableGlyphCount = CountRenderableGlyphPlacementsForTest(preparedText, layoutResult);
  if(renderResult)
  {
    DALI_TEST_CHECK(renderer.HasOutputActor());
    DALI_TEST_CHECK(renderer.GetOutputActor().GetChildCount() > 0u);
    DALI_TEST_EQUALS(renderer.GetOutputActor().GetProperty<Vector3>(Actor::Property::SIZE), Vector3(layoutResult.width, layoutResult.height, 0.0f), TEST_LOCATION);
    DALI_TEST_CHECK(renderer.GetMeshRecordCount() > 0u);
    DALI_TEST_CHECK(renderer.HasMeshTopologySignature());
    DALI_TEST_CHECK(renderer.GetMeshTopologySignature() != 0u);
    DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.HasGlyphCacheEntries());
    DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), renderableGlyphCount, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.HasGlyphCacheSignature());
    DALI_TEST_CHECK(renderer.GetGlyphCacheSignature() != 0u);
  }
  else
  {
    DALI_TEST_CHECK(!renderer.HasMeshRecords());
    DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
    DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
    DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererUsesLayoutSizeForOutputActorP(void)
{
  UiTestApplication                                               application;
  const auto                                                      preparedText = CreatePreparedText("bounds", 10.0f);
  Dali::Vector<Rect<float>>                                       exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter            adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge         bridge;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;
  Actor                                                          renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 40.0f, 0.0f, exclusionRegions, layoutResult);

  const Vector2 controlSize(layoutResult.width * 0.5f, std::max(1.0f, layoutResult.height * 0.5f));
  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(controlSize);
  bridge.SetAdapter(&adapter);
  renderHost.SetProperty(Actor::Property::SIZE, Vector3(controlSize.x, controlSize.y, 0.0f));
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    bridge.UpdateRenderData();
    bridge.AttachRendererToHost();
  }

  const bool renderResult = renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE);
  DALI_TEST_CHECK(renderResult == renderer.HasMeshRecords());

  if(renderResult)
  {
    const Vector3 expectedSize(layoutResult.width, layoutResult.height, 0.0f);
    DALI_TEST_EQUALS(renderer.GetOutputActor().GetProperty<Vector3>(Actor::Property::SIZE), expectedSize, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.GetOutputActor().GetChildCount() > 0u);
    DALI_TEST_EQUALS(renderer.GetOutputActor().GetChildAt(0u).GetProperty<Vector3>(Actor::Property::SIZE), expectedSize, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererClearAfterRenderSmokeP(void)
{
  UiTestApplication                                               application;
  const auto                                                      preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                       exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter            adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));

  renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE);
  renderer.Clear();

  DALI_TEST_CHECK(!renderer.HasRenderHost());
  DALI_TEST_CHECK(!renderer.HasOutputActor());
  DALI_TEST_CHECK(!renderer.IsAttached());
  DALI_TEST_CHECK(!renderer.HasMeshRecords());
  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
  DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());
  DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(renderer.GetLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::NONE), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureEmptyInputCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFailureCacheMissCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererRepeatedRenderSameGlyphSequenceSmokeP(void)
{
  UiTestApplication                                               application;
  const auto                                                      preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                       exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter            adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));

  const bool     firstRenderResult    = renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE);
  const uint32_t firstCacheEntryCount = renderer.GetGlyphCacheEntryCount();
  const uint64_t firstSignature       = renderer.GetGlyphCacheSignature();
  const uint64_t firstTopologySignature = renderer.GetMeshTopologySignature();
  const uint32_t firstMeshRecordCount   = renderer.GetMeshRecordCount();

  const bool secondRenderResult = renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE);

  if(firstRenderResult && secondRenderResult)
  {
    DALI_TEST_CHECK(renderer.HasGlyphCacheEntries());
    DALI_TEST_EQUALS(renderer.GetOutputActor().GetProperty<Vector3>(Actor::Property::SIZE), Vector3(layoutResult.width, layoutResult.height, 0.0f), TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), firstCacheEntryCount, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.HasGlyphCacheSignature());
    DALI_TEST_EQUALS(renderer.GetGlyphCacheSignature(), firstSignature, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.HasMeshTopologySignature());
    DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), firstTopologySignature, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), firstMeshRecordCount, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.GetGeometryOnlyUpdateCount() >= 1u);
  }
  else if(!secondRenderResult)
  {
    DALI_TEST_CHECK(!renderer.HasMeshRecords());
  }

  renderer.Clear();
  DALI_TEST_CHECK(!renderer.HasGlyphCacheEntries());
  DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer.HasGlyphCacheSignature());
  DALI_TEST_CHECK(!renderer.HasMeshTopologySignature());
  DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerGlyphRendererRepeatedRenderAfterWarmupGeometryOnlySmokeP(void)
{
  UiTestApplication                                               application;
  const auto                                                      preparedText = CreatePreparedText("geometry", 10.0f);
  Dali::Vector<Rect<float>>                                       exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter            adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge         bridge;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer renderer;
  Actor                                                          renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 40.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  renderHost.SetProperty(Actor::Property::SIZE, Vector3(layoutResult.width, layoutResult.height, 0.0f));
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    bridge.UpdateRenderData();
    bridge.AttachRendererToHost();
  }

  const bool firstRenderResult = renderer.Render(preparedText, layoutResult, adapter, Vector4::ONE);
  const uint64_t firstTopologySignature = renderer.GetMeshTopologySignature();
  const uint32_t firstMeshRecordCount   = renderer.GetMeshRecordCount();
  const uint32_t firstCacheEntryCount   = renderer.GetGlyphCacheEntryCount();

  const bool secondRenderResult = renderer.Render(preparedText, layoutResult, adapter, Vector4(0.8f, 0.9f, 1.0f, 1.0f));

  if(firstRenderResult && secondRenderResult)
  {
    DALI_TEST_CHECK(renderer.HasMeshRecords());
    DALI_TEST_EQUALS(renderer.GetOutputActor().GetProperty<Vector3>(Actor::Property::SIZE), Vector3(layoutResult.width, layoutResult.height, 0.0f), TEST_LOCATION);
    DALI_TEST_CHECK(renderer.HasMeshTopologySignature());
    DALI_TEST_EQUALS(renderer.GetMeshTopologySignature(), firstTopologySignature, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetMeshRecordCount(), firstMeshRecordCount, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetGlyphCacheEntryCount(), firstCacheEntryCount, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetFullMeshRebuildCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetGeometryOnlyUpdateCount(), 1u, TEST_LOCATION);
  }
  else
  {
    DALI_TEST_CHECK(!renderer.HasMeshRecords());
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeEmptyStateP(void)
{
  UiTestApplication                                                 application;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge bridge;

  DALI_TEST_CHECK(!bridge.HasRenderableGlyphs());
  DALI_TEST_CHECK(!bridge.IsRendererCreated());
  DALI_TEST_CHECK(!bridge.HasTextControlActor());
  DALI_TEST_CHECK(!bridge.HasRendererOutput());
  DALI_TEST_EQUALS(bridge.GetRendererOutputChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRendererOutputRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRendererOutputDescendantCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRendererOutputTotalRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!bridge.HasRendererOutputRenderableDescendant());
  DALI_TEST_EQUALS(bridge.GetFirstRendererOutputChildSize(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_CHECK(!bridge.IsFirstRendererOutputChildVisible());
  DALI_TEST_EQUALS(bridge.GetViewInterfaceGetGlyphsCallCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLastRequestedGlyphCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLastReturnedGlyphCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLastGlyphStartIndex(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRenderCallCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetAttachCallCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderAttemptCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderSuccessCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderFallbackCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightMeshTopologySignature(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!bridge.HasLightweightMeshTopologySignature());
  DALI_TEST_EQUALS(static_cast<uint32_t>(bridge.GetLightweightLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::NONE), TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureCacheMissCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureEmptyMeshCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureGlyphPlacementCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureGlyphInfoCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureGlyphPositionCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureEmptyInputCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureNoRenderableGlyphsCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureNoPositionCacheCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureSignatureCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureNoGlyphManagerCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureNoOutputActorCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureNoTextureSetCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRendererOutputSize(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRenderHostSize(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_CHECK(!bridge.IsRendererOutputVisible());
  DALI_TEST_CHECK(!bridge.IsRenderReady());

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeWithEmptyAdapterP(void)
{
  UiTestApplication                                                application;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;

  bridge.SetAdapter(&adapter);

  DALI_TEST_CHECK(!bridge.HasRenderableGlyphs());
  DALI_TEST_CHECK(!bridge.IsRendererCreated());
  DALI_TEST_EQUALS(bridge.GetLightweightRenderAttemptCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderSuccessCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderFallbackCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!bridge.HasLightweightMeshTopologySignature());
  DALI_TEST_EQUALS(static_cast<uint32_t>(bridge.GetLightweightLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::NONE), TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureCacheMissCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureEmptyMeshCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeWithRenderableAdapterP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);

  if(preparedText.GetGlyphCount() > 0u && !layoutResult.glyphPlacements.Empty())
  {
    DALI_TEST_CHECK(bridge.HasRenderableGlyphs());
  }
  else
  {
    DALI_TEST_CHECK(!bridge.HasRenderableGlyphs());
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeClearP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);
  bridge.EnsureRenderer();
  bridge.Clear();

  DALI_TEST_CHECK(!bridge.HasRenderableGlyphs());
  DALI_TEST_CHECK(!bridge.IsRendererCreated());
  DALI_TEST_EQUALS(bridge.GetLightweightRenderAttemptCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderSuccessCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderFallbackCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightMeshTopologySignature(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!bridge.HasLightweightMeshTopologySignature());
  DALI_TEST_EQUALS(static_cast<uint32_t>(bridge.GetLightweightLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::NONE), TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureCacheMissCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureEmptyMeshCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeEnsureRendererP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);
  bridge.EnsureRenderer();

  if(bridge.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.IsRendererCreated());
  }
  else
  {
    DALI_TEST_CHECK(!bridge.IsRendererCreated());
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasViewAdapterRenderableHelpersP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Text::GlyphInfo                                        glyphInfo;
  Dali::Ui::Internal::TextVisualizer::GlyphPlacement               placement;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(adapter.GetRenderableGlyphCount() > 0u);
    DALI_TEST_CHECK(adapter.GetGlyphPlacement(0u, placement));
    DALI_TEST_CHECK(adapter.GetGlyphInfo(placement.glyphIndex, glyphInfo));
  }
  else
  {
    DALI_TEST_EQUALS(adapter.GetRenderableGlyphCount(), 0u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeUpdateRenderDataEmptyP(void)
{
  UiTestApplication                                                 application;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge bridge;

  DALI_TEST_CHECK(!bridge.UpdateRenderData());
  DALI_TEST_CHECK(!bridge.IsRendererCreated());
  DALI_TEST_CHECK(!bridge.HasValidatedRenderData());
  DALI_TEST_EQUALS(bridge.GetValidatedGlyphCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeUpdateRenderDataWithEmptyAdapterP(void)
{
  UiTestApplication                                                application;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;

  bridge.SetAdapter(&adapter);

  DALI_TEST_CHECK(!bridge.UpdateRenderData());
  DALI_TEST_CHECK(!bridge.IsRendererCreated());
  DALI_TEST_CHECK(!bridge.HasValidatedRenderData());
  DALI_TEST_EQUALS(bridge.GetValidatedGlyphCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeUpdateRenderDataWithRenderableAdapterP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
    DALI_TEST_CHECK(bridge.IsRendererCreated());
    DALI_TEST_CHECK(bridge.HasValidatedRenderData());
    DALI_TEST_EQUALS(bridge.GetValidatedGlyphCount(), adapter.GetRenderableGlyphCount(), TEST_LOCATION);
    DALI_TEST_EQUALS(bridge.GetViewInterfaceGetGlyphsCallCount(), 0u, TEST_LOCATION);
  }
  else
  {
    DALI_TEST_CHECK(!bridge.UpdateRenderData());
    DALI_TEST_CHECK(!bridge.HasValidatedRenderData());
    DALI_TEST_EQUALS(bridge.GetValidatedGlyphCount(), 0u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeUpdateRenderDataInvalidPlacementP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Dali::Ui::Internal::TextVisualizer::GlyphPlacement               placement;

  placement.glyphIndex = preparedText.GetGlyphCount() + 10u;
  placement.x          = 0.0f;
  placement.y          = 0.0f;
  placement.width      = 10.0f;
  placement.height     = 10.0f;
  placement.advance    = 10.0f;
  layoutResult.glyphPlacements.PushBack(placement);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);

  DALI_TEST_CHECK(!bridge.UpdateRenderData());
  DALI_TEST_CHECK(!bridge.IsRendererCreated());
  DALI_TEST_CHECK(!bridge.HasValidatedRenderData());
  DALI_TEST_EQUALS(bridge.GetValidatedGlyphCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeClearAfterUpdateP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  bridge.Clear();

  DALI_TEST_CHECK(!bridge.IsRendererCreated());
  DALI_TEST_CHECK(!bridge.HasRenderableGlyphs());
  DALI_TEST_CHECK(!bridge.HasValidatedRenderData());
  DALI_TEST_EQUALS(bridge.GetValidatedGlyphCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeRenderHostSetterP(void)
{
  UiTestApplication                                                application;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  DALI_TEST_CHECK(!bridge.HasRenderHost());
  bridge.SetRenderHost(renderHost);
  DALI_TEST_CHECK(bridge.HasRenderHost());
  DALI_TEST_CHECK(bridge.GetRenderHost() == renderHost);

  bridge.Clear();
  DALI_TEST_CHECK(!bridge.HasRenderHost());
  DALI_TEST_CHECK(!bridge.GetRenderHost());

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeTextControlActorSetterP(void)
{
  UiTestApplication                                       application;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge bridge;
  Actor                                                   textControlActor = Actor::New();

  DALI_TEST_CHECK(!bridge.HasTextControlActor());
  bridge.SetTextControlActor(textControlActor);
  DALI_TEST_CHECK(bridge.HasTextControlActor());
  DALI_TEST_CHECK(bridge.GetTextControlActor() == textControlActor);

  bridge.Clear();
  DALI_TEST_CHECK(!bridge.HasTextControlActor());
  DALI_TEST_CHECK(!bridge.GetTextControlActor());

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeAttachWithoutHostP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  DALI_TEST_CHECK(!bridge.AttachRendererToHost());
  DALI_TEST_CHECK(!bridge.IsRendererAttached());
  DALI_TEST_CHECK(!bridge.GetRendererOutput());

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeAttachWithHostP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);
  renderHost.SetProperty(Actor::Property::SIZE, Vector3(layoutResult.width, layoutResult.height, 0.0f));
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  const bool attached = bridge.AttachRendererToHost();
  DALI_TEST_CHECK(attached == bridge.IsRendererAttached());
  DALI_TEST_EQUALS(bridge.GetLightweightRenderAttemptCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderSuccessCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightRenderFallbackCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightGeometryOnlyUpdateCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFullMeshRebuildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightGlyphCacheEntryCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightMeshRecordCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!bridge.HasLightweightMeshTopologySignature());
  DALI_TEST_EQUALS(static_cast<uint32_t>(bridge.GetLightweightLastFailureReason()), static_cast<uint32_t>(Dali::Ui::Internal::TextVisualizer::TextVisualizerGlyphRenderer::RenderFailureReason::NONE), TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureCacheMissCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetLightweightFailureEmptyMeshCount(), 0u, TEST_LOCATION);

  if(attached)
  {
    DALI_TEST_CHECK(bridge.HasRendererOutput());
    DALI_TEST_CHECK(bridge.IsRendererOutputParentedToHost());
    DALI_TEST_CHECK(bridge.IsRendererOutputVisible());
    DALI_TEST_CHECK(bridge.GetRendererOutputChildCount() > 0u || bridge.GetRendererOutputRendererCount() > 0u);
    DALI_TEST_CHECK(bridge.IsRenderReady());
    DALI_TEST_CHECK(bridge.GetRendererOutput());
    DALI_TEST_CHECK(bridge.GetRendererOutput().GetParent() == renderHost);
  }
  else
  {
    DALI_TEST_CHECK(!bridge.IsRenderReady());
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeOutputParentedToHostP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  renderHost.SetProperty(Actor::Property::SIZE, Vector3(layoutResult.width, layoutResult.height, 0.0f));
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  const bool attached = bridge.AttachRendererToHost();
  if(attached)
  {
    DALI_TEST_CHECK(bridge.IsRendererOutputParentedToHost());
  }
  else
  {
    DALI_TEST_CHECK(!bridge.IsRendererOutputParentedToHost());
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeOutputDiagnosticsP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  renderHost.SetProperty(Actor::Property::SIZE, Vector3(layoutResult.width, layoutResult.height, 0.0f));
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  const bool attached = bridge.AttachRendererToHost();
  if(attached)
  {
    DALI_TEST_CHECK(bridge.IsRendererOutputParentedToHost());
    DALI_TEST_CHECK(bridge.IsRendererOutputVisible());
    DALI_TEST_CHECK(bridge.GetRenderHostSize() == Vector3(layoutResult.width, layoutResult.height, 0.0f));
    DALI_TEST_CHECK(bridge.GetRendererOutputSize().x >= 0.0f);
    DALI_TEST_CHECK(bridge.GetRendererOutputSize().y >= 0.0f);
    DALI_TEST_CHECK(bridge.GetRendererOutputChildCount() > 0u || bridge.GetRendererOutputRendererCount() > 0u);
    DALI_TEST_CHECK(bridge.IsRenderReady());
  }
  else
  {
    DALI_TEST_EQUALS(bridge.GetRendererOutputChildCount(), 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(bridge.GetRendererOutputRendererCount(), 0u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeRenderDiagnosticsAfterAttachP(void)
{
  UiTestApplication                                       application;
  const auto                                              preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                               exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult        layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter    adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge bridge;
  Actor                                                   renderHost        = Actor::New();
  Actor                                                   textControlActor  = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  renderHost.SetProperty(Actor::Property::SIZE, Vector3(layoutResult.width, layoutResult.height, 0.0f));
  textControlActor.SetProperty(Actor::Property::SIZE, Vector3(layoutResult.width, layoutResult.height, 0.0f));
  bridge.SetRenderHost(renderHost);
  bridge.SetTextControlActor(textControlActor);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  const bool attached = bridge.AttachRendererToHost();

  DALI_TEST_CHECK(bridge.GetRendererOutputDescendantCount() >= bridge.GetRendererOutputChildCount());
  DALI_TEST_CHECK(bridge.GetRendererOutputTotalRendererCount() >= bridge.GetRendererOutputRendererCount());

  if(attached)
  {
    DALI_TEST_CHECK(bridge.GetViewInterfaceGetGlyphsCallCount() > 0u);
    DALI_TEST_EQUALS(bridge.GetLastGlyphStartIndex(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(bridge.GetLastRequestedGlyphCount() > 0u);
    DALI_TEST_CHECK(bridge.GetLastReturnedGlyphCount() > 0u);
    DALI_TEST_CHECK(bridge.GetLastReturnedGlyphCount() <= bridge.GetLastRequestedGlyphCount());
    DALI_TEST_CHECK(bridge.HasRendererOutput());
    DALI_TEST_CHECK(bridge.IsRenderReady());
    DALI_TEST_CHECK(bridge.GetFirstRendererOutputChildSize().x >= 0.0f);
    DALI_TEST_CHECK(bridge.GetFirstRendererOutputChildSize().y >= 0.0f);
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeAttachDuplicateP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  const bool firstAttach  = bridge.AttachRendererToHost();
  const uint32_t firstRenderCallCount = bridge.GetRenderCallCount();
  const uint32_t firstAttachCallCount = bridge.GetAttachCallCount();
  const uint32_t firstHostChildCount  = renderHost.GetChildCount();
  const bool secondAttach = bridge.AttachRendererToHost();

  DALI_TEST_CHECK(firstAttach == secondAttach);
  DALI_TEST_CHECK(secondAttach == bridge.IsRendererAttached());
  DALI_TEST_CHECK(bridge.GetRenderCallCount() > firstRenderCallCount);

  if(secondAttach)
  {
    DALI_TEST_CHECK(bridge.IsRendererOutputParentedToHost());
    DALI_TEST_CHECK(bridge.GetRendererOutput());
    DALI_TEST_CHECK(renderHost.GetChildCount() > 0u);
    DALI_TEST_CHECK(bridge.GetAttachCallCount() >= firstAttachCallCount);
    DALI_TEST_CHECK(renderHost.GetChildCount() <= firstHostChildCount + 1u);
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeDetachClearP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  bridge.AttachRendererToHost();
  bridge.DetachRendererFromHost();
  DALI_TEST_CHECK(!bridge.HasRendererOutput());
  DALI_TEST_CHECK(!bridge.IsRenderReady());
  DALI_TEST_CHECK(!bridge.IsRendererAttached());
  DALI_TEST_CHECK(!bridge.GetRendererOutput());

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeClearAfterRenderCallP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  bridge.AttachRendererToHost();
  bridge.Clear();

  DALI_TEST_CHECK(!bridge.HasRendererOutput());
  DALI_TEST_EQUALS(bridge.GetRendererOutputChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRendererOutputRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRendererOutputSize(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(bridge.GetRenderHostSize(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_CHECK(!bridge.IsRendererOutputVisible());
  DALI_TEST_CHECK(!bridge.IsRenderReady());
  DALI_TEST_CHECK(!bridge.IsRendererAttached());
  DALI_TEST_CHECK(!bridge.HasRenderHost());
  DALI_TEST_CHECK(!bridge.IsRendererCreated());

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeRenderReadyAfterRenderCallP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  const bool attached = bridge.AttachRendererToHost();
  if(attached)
  {
    DALI_TEST_CHECK(bridge.HasRendererOutput());
    DALI_TEST_CHECK(bridge.IsRenderReady());
  }
  else
  {
    DALI_TEST_CHECK(!bridge.IsRenderReady());
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeRenderSuccessConditionP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  renderHost.SetProperty(Actor::Property::SIZE, Vector3(layoutResult.width, layoutResult.height, 0.0f));
  bridge.SetRenderHost(renderHost);

  const bool updateRenderDataResult = adapter.HasRenderableGlyphs() && bridge.UpdateRenderData();
  const bool attached               = bridge.AttachRendererToHost();

  if(updateRenderDataResult && attached)
  {
    DALI_TEST_CHECK(bridge.IsRenderReady());
    DALI_TEST_CHECK(bridge.GetLastReturnedGlyphCount() > 0u);
    DALI_TEST_CHECK(bridge.GetLastReturnedGlyphCount() <= bridge.GetLastRequestedGlyphCount());
    DALI_TEST_CHECK(bridge.GetRendererOutput());
    DALI_TEST_CHECK(bridge.IsRendererOutputParentedToHost());
  }
  else
  {
    DALI_TEST_CHECK(!bridge.IsRenderReady());
  }

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeSetRenderHostEmptyClearsReadinessP(void)
{
  UiTestApplication                                                application;
  const auto                                                       preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Vector<Rect<float>>                                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge          bridge;
  Actor                                                            renderHost = Actor::New();

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  bridge.SetAdapter(&adapter);
  bridge.SetRenderHost(renderHost);

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  bridge.AttachRendererToHost();
  bridge.SetRenderHost(Actor());

  DALI_TEST_CHECK(!bridge.HasRenderHost());
  DALI_TEST_CHECK(!bridge.HasRendererOutput());
  DALI_TEST_CHECK(!bridge.IsRendererAttached());
  DALI_TEST_CHECK(!bridge.IsRenderReady());

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceEmptyStateP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface viewInterface;
  DALI_TEST_CHECK(!viewInterface.HasAdapter());
  DALI_TEST_EQUALS(viewInterface.GetNumberOfGlyphs(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetControlSize(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetLayoutSize(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetTextColor(), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_CHECK(nullptr == viewInterface.GetTextBuffer());
  DALI_TEST_EQUALS(viewInterface.GetGlyphsToCharacters().Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetDiagnostics().getGlyphsCallCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastRequestedGlyphCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastReturnedGlyphCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastGlyphStartIndex, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceTextColorDefaultP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface viewInterface;

  DALI_TEST_EQUALS(viewInterface.GetTextColor(), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceWithRenderableAdapterP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult       layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter   adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface viewInterface;
  Dali::Vector<Rect<float>> exclusionRegions;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  adapter.SetTextColor(Color::BLUE);
  viewInterface.SetAdapter(&adapter);

  DALI_TEST_CHECK(viewInterface.HasAdapter());
  DALI_TEST_EQUALS(viewInterface.GetLayoutSize(), Vector2(layoutResult.width, layoutResult.height), TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetControlSize(), Vector2(layoutResult.width, layoutResult.height), TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetTextColor(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetNumberOfGlyphs(), adapter.GetRenderableGlyphCount(), TEST_LOCATION);

  if(adapter.HasRenderableGlyphs())
  {
    const uint32_t glyphCount = adapter.GetRenderableGlyphCount();
    Dali::Vector<Dali::Ui::Text::GlyphInfo> glyphs;
    Dali::Vector<Vector2>                   positions;
    float                                   minLineOffset = -1.0f;
    glyphs.Resize(glyphCount);
    positions.Resize(glyphCount);

    DALI_TEST_EQUALS(adapter.GetRendererGlyphPositionCacheCount(), glyphCount, TEST_LOCATION);
    DALI_TEST_EQUALS(viewInterface.GetGlyphs(glyphs.Begin(), positions.Begin(), minLineOffset, 0u, glyphCount), glyphCount, TEST_LOCATION);
    DALI_TEST_EQUALS(minLineOffset, 0.0f, TEST_LOCATION);
    CheckGlyphPositionsFinite(positions);
  }

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceGlyphPositionConsistencyP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult             layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter         adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface viewInterface;
  Dali::Vector<Rect<float>> exclusionRegions;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  viewInterface.SetAdapter(&adapter);

  const uint32_t glyphCount = viewInterface.GetNumberOfGlyphs();
  if(glyphCount > 0u)
  {
    Dali::Vector<Dali::Ui::Text::GlyphInfo> glyphs;
    Dali::Vector<Vector2>                   positions;
    float                                   minLineOffset = -1.0f;
    glyphs.Resize(glyphCount);
    positions.Resize(glyphCount);

    DALI_TEST_EQUALS(viewInterface.GetGlyphs(glyphs.Begin(), positions.Begin(), minLineOffset, 0u, glyphCount), glyphCount, TEST_LOCATION);
    DALI_TEST_EQUALS(positions.Count(), glyphCount, TEST_LOCATION);
    DALI_TEST_EQUALS(glyphs.Count(), glyphCount, TEST_LOCATION);
    DALI_TEST_EQUALS(minLineOffset, 0.0f, TEST_LOCATION);
    CheckGlyphPositionsFinite(positions);
  }

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceGlyphDiagnosticsP(void)
{
  UiTestApplication                                                application;
  auto                                                             preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult                 layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter             adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface  viewInterface;
  Dali::Vector<Rect<float>>                                        exclusionRegions;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  viewInterface.SetAdapter(&adapter);

  const uint32_t glyphCount = viewInterface.GetNumberOfGlyphs();
  if(glyphCount > 0u)
  {
    Dali::Vector<Dali::Ui::Text::GlyphInfo> glyphs;
    Dali::Vector<Vector2>                   positions;
    float                                   minLineOffset = -1.0f;
    glyphs.Resize(glyphCount);
    positions.Resize(glyphCount);

    const uint32_t returnedGlyphCount = viewInterface.GetGlyphs(glyphs.Begin(), positions.Begin(), minLineOffset, 0u, glyphCount);

    DALI_TEST_EQUALS(viewInterface.GetDiagnostics().getGlyphsCallCount, 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastGlyphStartIndex, 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastRequestedGlyphCount, glyphCount, TEST_LOCATION);
    DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastReturnedGlyphCount, returnedGlyphCount, TEST_LOCATION);
  }

  viewInterface.Clear();
  DALI_TEST_EQUALS(viewInterface.GetDiagnostics().getGlyphsCallCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastRequestedGlyphCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastReturnedGlyphCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(viewInterface.GetDiagnostics().lastGlyphStartIndex, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceGlyphPositionWithExclusionP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghij", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult             layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter         adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface viewInterface;
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(10.0f, 0.0f, 25.0f, 20.0f));

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 120.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetControlSize(Vector2(layoutResult.width, layoutResult.height));
  viewInterface.SetAdapter(&adapter);

  const uint32_t glyphCount = viewInterface.GetNumberOfGlyphs();
  if(glyphCount > 0u)
  {
    Dali::Vector<Dali::Ui::Text::GlyphInfo> glyphs;
    Dali::Vector<Vector2>                   positions;
    float                                   minLineOffset = -1.0f;
    glyphs.Resize(glyphCount);
    positions.Resize(glyphCount);

    DALI_TEST_EQUALS(viewInterface.GetGlyphs(glyphs.Begin(), positions.Begin(), minLineOffset, 0u, glyphCount), glyphCount, TEST_LOCATION);
    CheckGlyphPositionsFinite(positions);
    CheckGlyphPositionsOutsideExclusion(positions, exclusionRegions[0]);
  }

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceTextColorPassThroughP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult             layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter         adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface viewInterface;
  Dali::Vector<Rect<float>> exclusionRegions;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetTextColor(Color::BLUE);
  viewInterface.SetAdapter(&adapter);

  DALI_TEST_EQUALS(viewInterface.GetTextColor(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceTextColorImmediateAdapterUpdateP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult              layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter          adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface viewInterface;
  Dali::Vector<Rect<float>> exclusionRegions;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  adapter.SetTextColor(Color::BLUE);
  viewInterface.SetAdapter(&adapter);

  DALI_TEST_EQUALS(viewInterface.GetTextColor(), Color::BLUE, TEST_LOCATION);

  adapter.SetTextColor(Color::GREEN);
  DALI_TEST_EQUALS(viewInterface.GetTextColor(), Color::GREEN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerViewInterfaceClearP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult       layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter   adapter;
  Dali::Ui::Internal::TextVisualizer::TextVisualizerViewInterface viewInterface;
  Dali::Vector<Rect<float>> exclusionRegions;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  viewInterface.SetAdapter(&adapter);
  viewInterface.Clear();

  DALI_TEST_CHECK(!viewInterface.HasAdapter());
  DALI_TEST_EQUALS(viewInterface.GetNumberOfGlyphs(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(nullptr == viewInterface.GetTextBuffer());

  END_TEST;
}

int UtcDaliTextVisualizerAtlasRendererBridgeOwnsViewInterfaceSmokeP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abc", 10.0f);
  Dali::Ui::Internal::TextVisualizer::LayoutResult       layoutResult;
  Dali::Ui::Internal::TextVisualizer::AtlasViewAdapter   adapter;
  Dali::Ui::Internal::TextVisualizer::AtlasRendererBridge bridge;
  Dali::Vector<Rect<float>> exclusionRegions;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, preparedText.GetTotalGlyphAdvance() + 20.0f, 0.0f, exclusionRegions, layoutResult);

  adapter.SetPreparedText(&preparedText);
  adapter.SetLayoutResult(&layoutResult);
  bridge.SetAdapter(&adapter);

  DALI_TEST_CHECK(bridge.HasViewInterfaceAdapter());

  if(adapter.HasRenderableGlyphs())
  {
    DALI_TEST_CHECK(bridge.UpdateRenderData());
  }

  DALI_TEST_CHECK(!bridge.IsRendererAttached());
  bridge.Clear();
  DALI_TEST_CHECK(!bridge.HasViewInterfaceAdapter());

  END_TEST;
}

int UtcDaliTextVisualizerRenderHostEmptySmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextVisualizerMeasureNonZeroWithExplicitWidthP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("TextVisualizer explicit width measurement");
  textVisualizer.SetFontSize(18.0f);

  MeasuredSize measured = textVisualizer.Measure(240.0f, 0.0f);

  DALI_TEST_CHECK(measured.GetWidth() > 0.0f);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);
  DALI_TEST_CHECK(textVisualizer.GetMeasuredSize().GetWidth() > 0.0f);
  DALI_TEST_CHECK(textVisualizer.GetMeasuredSize().GetHeight() > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerMeasureWrapContentNaturalSizeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("TextVisualizer wrap content natural size");
  textVisualizer.SetFontSize(18.0f);
  textVisualizer.SetRequestedWidth(WRAP_CONTENT);
  textVisualizer.SetRequestedHeight(WRAP_CONTENT);

  const MeasuredSize measured = textVisualizer.Measure(0.0f, 0.0f);
  DALI_TEST_CHECK(measured.GetWidth() > 0.0f);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerMeasureFixedWidthWrapHeightP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("TextVisualizer fixed width wrap height measure should wrap this text across several lines.");
  textVisualizer.SetFontSize(18.0f);
  textVisualizer.SetRequestedWidth(160.0f);
  textVisualizer.SetRequestedHeight(WRAP_CONTENT);

  const MeasuredSize measured = textVisualizer.Measure(160.0f, 0.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 160.0f, TEST_LOCATION);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerMeasureThenRelayoutWithExclusionSmokeP(void)
{
  UiTestApplication          application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> exclusionRegions;
  DALI_TEST_CHECK(textVisualizer);

  exclusionRegions.PushBack(Rect<float>(40.0f, 0.0f, 80.0f, 60.0f));

  textVisualizer.SetText("TextVisualizer measure result should stay valid for the following relayout with matching width and exclusions.");
  textVisualizer.SetFontSize(18.0f);
  textVisualizer.SetRequestedWidth(220.0f);
  textVisualizer.SetRequestedHeight(WRAP_CONTENT);
  textVisualizer.SetExclusionRegions(exclusionRegions);

  const MeasuredSize measured = textVisualizer.Measure(220.0f, 0.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 220.0f, TEST_LOCATION);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  const Vector3 size = textVisualizer.GetProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(size.x, 220.0f, TEST_LOCATION);
  DALI_TEST_CHECK(size.y > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerMeasuredLayoutCacheInvalidationSmokeP(void)
{
  UiTestApplication          application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> exclusionRegions;
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("Changing exclusion and line height after measure should relayout safely.");
  textVisualizer.SetFontSize(18.0f);
  textVisualizer.SetRequestedWidth(180.0f);
  textVisualizer.SetRequestedHeight(WRAP_CONTENT);

  const float initialHeight = textVisualizer.Measure(180.0f, 0.0f).GetHeight();
  DALI_TEST_CHECK(initialHeight > 0.0f);

  exclusionRegions.PushBack(Rect<float>(0.0f, 0.0f, 90.0f, 80.0f));
  textVisualizer.SetExclusionRegions(exclusionRegions);
  textVisualizer.SetLineHeight(1.6f);

  const float updatedHeight = textVisualizer.Measure(180.0f, 0.0f).GetHeight();
  DALI_TEST_CHECK(updatedHeight > 0.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextVisualizerMeasureFixedSizeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("TextVisualizer fixed size measure");
  textVisualizer.SetFontSize(18.0f);
  textVisualizer.SetRequestedWidth(220.0f);
  textVisualizer.SetRequestedHeight(96.0f);

  const MeasuredSize measured = textVisualizer.Measure(0.0f, 0.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 220.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.GetHeight(), 96.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerMeasureMatchParentP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("TextVisualizer match parent measure");
  textVisualizer.SetFontSize(18.0f);
  textVisualizer.SetRequestedWidth(MATCH_PARENT);
  textVisualizer.SetRequestedHeight(MATCH_PARENT);

  const MeasuredSize measured = textVisualizer.Measure(320.0f, 180.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 320.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.GetHeight(), 180.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerMeasureEmptyTextP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("");
  textVisualizer.SetRequestedWidth(WRAP_CONTENT);
  textVisualizer.SetRequestedHeight(WRAP_CONTENT);

  MeasuredSize measured = textVisualizer.Measure(0.0f, 0.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.GetHeight(), 0.0f, TEST_LOCATION);

  textVisualizer.SetRequestedWidth(120.0f);
  textVisualizer.SetRequestedHeight(80.0f);

  measured = textVisualizer.Measure(0.0f, 0.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.GetHeight(), 80.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerRelayoutWithExplicitSizeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("TextVisualizer explicit relayout size");
  textVisualizer.SetFontSize(18.0f);
  textVisualizer.SetRequestedWidth(240.0f);
  textVisualizer.SetRequestedHeight(180.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  const Vector3 size = textVisualizer.GetProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_CHECK(size.x > 0.0f);
  DALI_TEST_CHECK(size.y > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerRenderHostCreatedWithTextP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("abc");
  textVisualizer.SetFontSize(10.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextVisualizerRenderHostStencilLikeSmokeP(void)
{
  UiTestApplication          application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> exclusionRegions;
  DALI_TEST_CHECK(textVisualizer);

  exclusionRegions.PushBack(Rect<float>(40.0f, 10.0f, 80.0f, 40.0f));

  textVisualizer.SetText("TextVisualizer render host clipping smoke with exclusion region.");
  textVisualizer.SetFontSize(14.0f);
  textVisualizer.SetRequestedWidth(240.0f);
  textVisualizer.SetRequestedHeight(160.0f);
  textVisualizer.SetExclusionRegions(exclusionRegions);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  const Vector3 size = textVisualizer.GetProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_CHECK(size.x > 0.0f);
  DALI_TEST_CHECK(size.y > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerRenderCallSmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("abc");
  textVisualizer.SetFontSize(10.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextVisualizerRenderDirtyClearsAfterSuccessfulRenderSmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("TextVisualizer render dirty clear smoke.");
  textVisualizer.SetFontSize(12.0f);
  textVisualizer.SetRequestedWidth(240.0f);
  textVisualizer.SetRequestedHeight(120.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  MeasuredSize measured = textVisualizer.Measure(240.0f, 0.0f);
  DALI_TEST_CHECK(measured.GetWidth() > 0.0f);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutUnchangedRenderSkipSmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> exclusionRegionsA;
  Dali::Vector<Rect<float>> exclusionRegionsB;
  DALI_TEST_CHECK(textVisualizer);

  exclusionRegionsA.PushBack(Rect<float>(0.0f, 220.0f, 20.0f, 20.0f));
  exclusionRegionsB.PushBack(Rect<float>(40.0f, 220.0f, 20.0f, 20.0f));

  textVisualizer.SetText("TextVisualizer unchanged layout render skip smoke.");
  textVisualizer.SetFontSize(12.0f);
  textVisualizer.SetRequestedWidth(240.0f);
  textVisualizer.SetRequestedHeight(120.0f);
  textVisualizer.SetExclusionRegions(exclusionRegionsA);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  textVisualizer.SetExclusionRegions(exclusionRegionsB);
  application.SendNotification();
  application.Render();

  MeasuredSize measured = textVisualizer.Measure(240.0f, 0.0f);
  DALI_TEST_CHECK(measured.GetWidth() > 0.0f);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerTextColorRenderSmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("abc");
  textVisualizer.SetFontSize(10.0f);
  textVisualizer.SetTextColor(UiColor(Color::RED));

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textVisualizer.GetTextColor().Resolve(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerTextColorChangeWithoutLayoutDirtySmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("abc");
  textVisualizer.SetFontSize(10.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  textVisualizer.SetTextColor(UiColor(Color::GREEN));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textVisualizer.GetTextColor().Resolve(), Color::GREEN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLineHeightChangeReenablesRenderSmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("Line height dirty render smoke line height dirty render smoke");
  textVisualizer.SetFontSize(18.0f);
  textVisualizer.SetRequestedWidth(220.0f);
  textVisualizer.SetRequestedHeight(180.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  textVisualizer.SetLineHeight(1.5f);
  application.SendNotification();
  application.Render();

  MeasuredSize measured = textVisualizer.Measure(220.0f, 0.0f);
  DALI_TEST_EQUALS(textVisualizer.GetLineHeight(), 1.5f, TEST_LOCATION);
  DALI_TEST_CHECK(measured.GetWidth() > 0.0f);
  DALI_TEST_CHECK(measured.GetHeight() > 0.0f);

  END_TEST;
}

int UtcDaliTextVisualizerRenderHostSurvivesRelayoutP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("abcdefghij");
  textVisualizer.SetFontSize(10.0f);
  textVisualizer.SetRequestedWidth(25.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  textVisualizer.SetRequestedWidth(60.0f);
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextVisualizerExclusionRelayoutTriggersRenderUpdateSmokeP(void)
{
  UiTestApplication         application;
  TextVisualizer            textVisualizer = TextVisualizer::New();
  Dali::Vector<Rect<float>> exclusionRegionsA;
  Dali::Vector<Rect<float>> exclusionRegionsB;
  DALI_TEST_CHECK(textVisualizer);

  exclusionRegionsA.PushBack(Rect<float>(20.0f, 20.0f, 80.0f, 40.0f));
  exclusionRegionsB.PushBack(Rect<float>(120.0f, 90.0f, 60.0f, 60.0f));

  textVisualizer.SetText("TextVisualizer exclusion relayout render update smoke.");
  textVisualizer.SetFontSize(14.0f);
  textVisualizer.SetRequestedWidth(240.0f);
  textVisualizer.SetRequestedHeight(180.0f);
  textVisualizer.SetExclusionRegions(exclusionRegionsA);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  textVisualizer.SetExclusionRegions(exclusionRegionsB);
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextVisualizerRenderHostClearPathP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("abc");
  textVisualizer.SetFontSize(10.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  textVisualizer.SetText("");
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextVisualizerPrepareSimpleAsciiTextP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("Simple ASCII text");
  textVisualizer.Prepare();
  textVisualizer.Prepare();

  END_TEST;
}

int UtcDaliTextVisualizerPrepareKoreanTextP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("한글 테스트 문장");
  textVisualizer.Prepare();

  END_TEST;
}

int UtcDaliTextVisualizerPrepareEmojiTextP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("Emoji 😀👨‍👩‍👧‍👦");
  textVisualizer.Prepare();

  END_TEST;
}

int UtcDaliTextVisualizerExclusionLayoutCacheEmptyP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::ExclusionLayoutCache cache;

  DALI_TEST_CHECK(cache.Empty());
  DALI_TEST_EQUALS(cache.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(cache.GetSortedRegions().size()), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerExclusionLayoutCacheSetRegionsSortedP(void)
{
  UiTestApplication application;
  Dali::Vector<Rect<float>> regions;
  regions.PushBack(Rect<float>(0.0f, 30.0f, 10.0f, -20.0f));
  regions.PushBack(Rect<float>(0.0f, 0.0f, 10.0f, 5.0f));
  regions.PushBack(Rect<float>(0.0f, 20.0f, 10.0f, 5.0f));

  Dali::Ui::Internal::TextVisualizer::ExclusionLayoutCache cache;
  const uint64_t                                           initialVersion = cache.GetVersion();
  cache.SetRegions(regions);

  const Dali::Ui::Internal::TextVisualizer::SortedExclusionRegions& sortedRegions = cache.GetSortedRegions();
  DALI_TEST_CHECK(!cache.Empty());
  DALI_TEST_EQUALS(cache.Count(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(cache.GetVersion() > initialVersion);
  DALI_TEST_EQUALS(sortedRegions[0u].top, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(sortedRegions[0u].bottom, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(sortedRegions[1u].top, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(sortedRegions[1u].bottom, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(sortedRegions[2u].top, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(sortedRegions[2u].bottom, 25.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerExclusionLayoutCacheClearP(void)
{
  UiTestApplication application;
  Dali::Vector<Rect<float>> regions;
  regions.PushBack(Rect<float>(0.0f, 0.0f, 10.0f, 10.0f));

  Dali::Ui::Internal::TextVisualizer::ExclusionLayoutCache cache;
  cache.SetRegions(regions);
  const uint64_t setVersion = cache.GetVersion();

  cache.Clear();

  DALI_TEST_CHECK(cache.Empty());
  DALI_TEST_EQUALS(cache.Count(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(cache.GetVersion() > setVersion);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutIntervalsSortedCacheSameAsVectorPathP(void)
{
  UiTestApplication application;
  Dali::Vector<Rect<float>> regions;
  regions.PushBack(Rect<float>(45.0f, 0.0f, 20.0f, 20.0f));
  regions.PushBack(Rect<float>(10.0f, 0.0f, 15.0f, 20.0f));

  Dali::Ui::Internal::TextVisualizer::ExclusionLayoutCache cache;
  cache.SetRegions(regions);

  const Dali::Vector<Dali::Ui::Internal::TextVisualizer::AvailableInterval> vectorIntervals =
    Dali::Ui::Internal::TextVisualizer::LayoutEngine::BuildAvailableIntervals(100.0f, 0.0f, 20.0f, regions);
  const Dali::Vector<Dali::Ui::Internal::TextVisualizer::AvailableInterval> cacheIntervals =
    Dali::Ui::Internal::TextVisualizer::LayoutEngine::BuildAvailableIntervals(100.0f, 0.0f, 20.0f, cache);

  DALI_TEST_EQUALS(cacheIntervals.Count(), vectorIntervals.Count(), TEST_LOCATION);
  for(uint32_t index = 0u; index < vectorIntervals.Count(); ++index)
  {
    DALI_TEST_EQUALS(cacheIntervals[index].x, vectorIntervals[index].x, TEST_LOCATION);
    DALI_TEST_EQUALS(cacheIntervals[index].width, vectorIntervals[index].width, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerLayoutGlyphsSortedCacheSameAsVectorPathP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("hello world dynamic text", 16.0f);
  Dali::Vector<Rect<float>> regions;
  regions.PushBack(Rect<float>(55.0f, 0.0f, 45.0f, 24.0f));
  regions.PushBack(Rect<float>(0.0f, 32.0f, 35.0f, 24.0f));

  Dali::Ui::Internal::TextVisualizer::ExclusionLayoutCache cache;
  cache.SetRegions(regions);

  Dali::Ui::Internal::TextVisualizer::LayoutResult vectorLayoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutResult cacheLayoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 180.0f, 0.0f, regions, vectorLayoutResult);
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(preparedText, 180.0f, 0.0f, cache, cacheLayoutResult);

  if(preparedText.GetGlyphCount() > 0u)
  {
    DALI_TEST_EQUALS(cacheLayoutResult.glyphPlacements.Count(), vectorLayoutResult.glyphPlacements.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(cacheLayoutResult.CalculateSignature(), vectorLayoutResult.CalculateSignature(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextVisualizerLayoutPlaceholderSortedCacheSameAsVectorPathP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghij", 10.0f);
  Dali::Vector<Rect<float>> regions;
  regions.PushBack(Rect<float>(20.0f, 0.0f, 20.0f, 12.0f));
  regions.PushBack(Rect<float>(0.0f, 12.0f, 15.0f, 12.0f));

  Dali::Ui::Internal::TextVisualizer::ExclusionLayoutCache cache;
  cache.SetRegions(regions);

  Dali::Ui::Internal::TextVisualizer::LayoutResult vectorLayoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutResult cacheLayoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 100.0f, 12.0f, regions, vectorLayoutResult);
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 100.0f, 12.0f, cache, cacheLayoutResult);

  DALI_TEST_EQUALS(cacheLayoutResult.clusterPlacements.Count(), vectorLayoutResult.clusterPlacements.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(cacheLayoutResult.CalculateSignature(), vectorLayoutResult.CalculateSignature(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutIntervalsWithoutExclusionP(void)
{
  UiTestApplication         application;
  Dali::Vector<Rect<float>> exclusionRegions;

  const auto availableIntervals = Dali::Ui::Internal::TextVisualizer::LayoutEngine::BuildAvailableIntervals(100.0f, 0.0f, 20.0f, exclusionRegions);

  DALI_TEST_EQUALS(availableIntervals.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].width, 100.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutIntervalsSingleExclusionP(void)
{
  UiTestApplication         application;
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(20.0f, 0.0f, 20.0f, 20.0f));

  const auto availableIntervals = Dali::Ui::Internal::TextVisualizer::LayoutEngine::BuildAvailableIntervals(100.0f, 0.0f, 20.0f, exclusionRegions);

  DALI_TEST_EQUALS(availableIntervals.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].width, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[1].x, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[1].width, 60.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutIntervalsWithoutVerticalOverlapP(void)
{
  UiTestApplication         application;
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(20.0f, 30.0f, 20.0f, 10.0f));

  const auto availableIntervals = Dali::Ui::Internal::TextVisualizer::LayoutEngine::BuildAvailableIntervals(100.0f, 0.0f, 20.0f, exclusionRegions);

  DALI_TEST_EQUALS(availableIntervals.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].width, 100.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutIntervalsMergeBlockedRangesP(void)
{
  UiTestApplication         application;
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(20.0f, 0.0f, 20.0f, 20.0f));
  exclusionRegions.PushBack(Rect<float>(30.0f, 0.0f, 30.0f, 20.0f));

  const auto availableIntervals = Dali::Ui::Internal::TextVisualizer::LayoutEngine::BuildAvailableIntervals(100.0f, 0.0f, 20.0f, exclusionRegions);

  DALI_TEST_EQUALS(availableIntervals.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].width, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[1].x, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[1].width, 40.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutIntervalsClampOutsideWidthP(void)
{
  UiTestApplication         application;
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(-10.0f, 0.0f, 30.0f, 20.0f));
  exclusionRegions.PushBack(Rect<float>(80.0f, 0.0f, 60.0f, 20.0f));

  const auto availableIntervals = Dali::Ui::Internal::TextVisualizer::LayoutEngine::BuildAvailableIntervals(100.0f, 0.0f, 20.0f, exclusionRegions);

  DALI_TEST_EQUALS(availableIntervals.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].x, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(availableIntervals[0].width, 60.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerLayoutIntervalsFullyBlockedP(void)
{
  UiTestApplication         application;
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(0.0f, 0.0f, 100.0f, 20.0f));

  const auto availableIntervals = Dali::Ui::Internal::TextVisualizer::LayoutEngine::BuildAvailableIntervals(100.0f, 0.0f, 20.0f, exclusionRegions);

  DALI_TEST_EQUALS(availableIntervals.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPlaceholderLayoutBasicP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghij", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 20.0f, 12.0f, exclusionRegions, layoutResult);

  DALI_TEST_EQUALS(layoutResult.GetLineCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments[0].clusterStart, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments[0].clusterEnd, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[2].fragments[0].clusterEnd, 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.clusterPlacements.Count(), 10u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPlaceholderLayoutWithExclusionP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghij", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(20.0f, 0.0f, 20.0f, 12.0f));

  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 100.0f, 12.0f, exclusionRegions, layoutResult);

  DALI_TEST_EQUALS(layoutResult.GetLineCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments[0].clusterStart, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments[0].clusterEnd, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments[1].clusterStart, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments[1].clusterEnd, 10u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPlaceholderLayoutEmptyTextP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextVisualizer::PreparedText preparedText;
  Dali::Vector<Rect<float>>                        exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 100.0f, 12.0f, exclusionRegions, layoutResult);

  DALI_TEST_CHECK(layoutResult.Empty());

  END_TEST;
}

int UtcDaliTextVisualizerPlaceholderLayoutWidthChangeP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdefghij", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  Dali::Ui::Internal::TextVisualizer::LayoutResult narrowLayoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutResult wideLayoutResult;

  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 25.0f, 12.0f, exclusionRegions, narrowLayoutResult);
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 50.0f, 12.0f, exclusionRegions, wideLayoutResult);

  DALI_TEST_CHECK(narrowLayoutResult.GetLineCount() > wideLayoutResult.GetLineCount());

  END_TEST;
}

int UtcDaliTextVisualizerPlaceholderLayoutFullBlockedLineP(void)
{
  UiTestApplication application;
  auto              preparedText = CreatePreparedText("abcdef", 10.0f);
  Dali::Vector<Rect<float>> exclusionRegions;
  exclusionRegions.PushBack(Rect<float>(0.0f, 0.0f, 30.0f, 12.0f));

  Dali::Ui::Internal::TextVisualizer::LayoutResult layoutResult;
  Dali::Ui::Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(preparedText, 30.0f, 12.0f, exclusionRegions, layoutResult);

  DALI_TEST_EQUALS(layoutResult.GetLineCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].y, 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layoutResult.lines[0].fragments[0].clusterEnd, 6u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualizerPlaceholderRelayoutSmokeP(void)
{
  UiTestApplication application;
  TextVisualizer    textVisualizer = TextVisualizer::New();
  DALI_TEST_CHECK(textVisualizer);

  textVisualizer.SetText("abcdefghijabcdefghijabcdefghij");
  textVisualizer.SetFontSize(24.0f);
  textVisualizer.Prepare();
  textVisualizer.SetRequestedWidth(60.0f);

  application.GetScene().Add(textVisualizer);
  application.SendNotification();
  application.Render();

  const float narrowHeight = textVisualizer.Measure(60.0f, 0.0f).GetHeight();

  textVisualizer.SetRequestedWidth(180.0f);
  application.SendNotification();
  application.Render();

  const float wideHeight = textVisualizer.Measure(180.0f, 0.0f).GetHeight();
  DALI_TEST_CHECK(narrowHeight > 0.0f);
  DALI_TEST_CHECK(wideHeight > 0.0f);
  DALI_TEST_CHECK(narrowHeight >= wideHeight);

  END_TEST;
}
