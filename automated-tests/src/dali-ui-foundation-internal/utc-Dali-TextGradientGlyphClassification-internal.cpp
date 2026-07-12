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

#include <dali-ui-foundation/internal/text/gradient-glyph-classification.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;

namespace
{

namespace UiText         = Dali::Ui::Text;
namespace UiTextInternal = Dali::Ui::Text::Internal;

UiText::GlyphInfo MakeGlyph(UiText::FontId fontId = 1u, UiText::GlyphIndex glyphIndex = 1u)
{
  UiText::GlyphInfo glyph;
  glyph.fontId = fontId;
  glyph.index  = glyphIndex;
  return glyph;
}

void ExpectClassification(const UiTextInternal::GradientGlyphInfo& classification,
                          bool                                      usesGradientFill,
                          bool                                      isColorGlyph,
                          bool                                      hasExplicitColor,
                          bool                                      isEmbeddedItem = false)
{
  DALI_TEST_EQUALS(classification.usesGradientFill, usesGradientFill, TEST_LOCATION);
  DALI_TEST_EQUALS(classification.isColorGlyph, isColorGlyph, TEST_LOCATION);
  DALI_TEST_EQUALS(classification.hasExplicitColor, hasExplicitColor, TEST_LOCATION);
  DALI_TEST_EQUALS(classification.isEmbeddedItem, isEmbeddedItem, TEST_LOCATION);
}

} // namespace

void utc_dali_text_gradient_glyph_classification_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_gradient_glyph_classification_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGradientGlyphClassificationDefaultColorNullIndicesP(void)
{
  UiTestApplication application;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  const UiText::GlyphInfo     glyph      = MakeGlyph();

  const auto classification = UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, nullptr, 0u);

  ExpectClassification(classification, true, false, false);
  END_TEST;
}

int UtcDaliTextGradientGlyphClassificationDefaultColorZeroIndexP(void)
{
  UiTestApplication application;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  const UiText::GlyphInfo     glyph      = MakeGlyph();
  const UiText::ColorIndex    colorIndices[] = {0u};

  const auto classification = UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, colorIndices, 0u);

  ExpectClassification(classification, true, false, false);
  END_TEST;
}

int UtcDaliTextGradientGlyphClassificationExplicitColorIndexP(void)
{
  UiTestApplication application;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  const UiText::GlyphInfo     glyph      = MakeGlyph();
  const UiText::ColorIndex    colorIndices[] = {1u};

  const auto classification = UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, colorIndices, 0u);

  ExpectClassification(classification, false, false, true);
  END_TEST;
}

int UtcDaliTextGradientGlyphClassificationMixedColorIndicesP(void)
{
  UiTestApplication application;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  const UiText::GlyphInfo     glyph      = MakeGlyph();
  const UiText::ColorIndex    colorIndices[] = {0u, 1u, 0u, 2u};

  ExpectClassification(UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, colorIndices, 0u), true, false, false);
  ExpectClassification(UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, colorIndices, 1u), false, false, true);
  ExpectClassification(UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, colorIndices, 2u), true, false, false);
  ExpectClassification(UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, colorIndices, 3u), false, false, true);
  END_TEST;
}

int UtcDaliTextGradientGlyphClassificationColorGlyphDefaultFixtureSmokeP(void)
{
  UiTestApplication application;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  const UiText::GlyphInfo     glyph      = MakeGlyph();

  const auto classification = UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, nullptr, 0u);

  ExpectClassification(classification, true, false, false);
  END_TEST;
}

int UtcDaliTextGradientGlyphClassificationExplicitColorWithDefaultFixtureSmokeP(void)
{
  UiTestApplication application;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  const UiText::GlyphInfo     glyph      = MakeGlyph();
  const UiText::ColorIndex    colorIndices[] = {2u};

  const auto classification = UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, colorIndices, 0u);

  ExpectClassification(classification, false, false, true);
  END_TEST;
}

int UtcDaliTextGradientGlyphClassificationEmbeddedItemExcludedP(void)
{
  UiTestApplication application;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  Pixel::Format pixelFormat = Pixel::A8;
  const TextAbstraction::FontClient::EmbeddedItemDescription description = {
    "", 16u, 12u, TextAbstraction::ColorBlendingMode::NONE};
  const UiText::GlyphIndex embeddedIndex = fontClient.CreateEmbeddedItem(description, pixelFormat);
  DALI_TEST_CHECK(embeddedIndex != 0u);

  const UiText::GlyphInfo glyph = MakeGlyph(0u, embeddedIndex);

  const auto classification = UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, nullptr, 0u);

  ExpectClassification(classification, false, false, false, true);
  END_TEST;
}

int UtcDaliTextGradientGlyphClassificationZeroFontAndZeroIndexIsNotEmbeddedP(void)
{
  UiTestApplication application;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  const UiText::GlyphInfo     glyph      = MakeGlyph(0u, 0u);

  const auto classification = UiTextInternal::ClassifyGradientGlyph(fontClient, glyph, nullptr, 0u);

  ExpectClassification(classification, true, false, false, false);
  END_TEST;
}
