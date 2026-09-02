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

// EXTERNAL INCLUDES
#include <dali.h>
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h>
#include <dali/integration-api/string-utils.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

using Dali::Integration::ToStdString;

namespace
{
const char* const PROPERTY_NAME_TEXT                           = "text";
const char* const PROPERTY_NAME_FONT_FAMILY                    = "fontFamily";
const char* const PROPERTY_NAME_FONT_SIZE                      = "fontSize";
const char* const PROPERTY_NAME_MULTI_LINE                     = "multiLine";
const char* const PROPERTY_NAME_LINE_WRAP_MODE                 = "lineWrapMode";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT           = "horizontalAlignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT             = "verticalAlignment";
const char* const PROPERTY_NAME_OVERFLOW_MODE                  = "overflowMode";
const char* const PROPERTY_NAME_LINE_HEIGHT                    = "lineHeight";
const char* const PROPERTY_NAME_LINE_HEIGHT_MODE               = "lineHeightMode";
const char* const PROPERTY_NAME_LAYOUT_DIRECTION_MODE          = "layoutDirectionMode";
const char* const PROPERTY_NAME_ANCHOR_COLOR                   = "anchorColor";
const char* const PROPERTY_NAME_ANCHOR_CLICKED_COLOR           = "anchorClickedColor";
const char* const PROPERTY_NAME_MARQUEE_TRIGGER_POLICY         = "marqueeTriggerPolicy";
const char* const PROPERTY_NAME_MARQUEE_SPEED                  = "marqueeSpeed";
const char* const PROPERTY_NAME_MARQUEE_LOOP_COUNT             = "marqueeLoopCount";
const char* const PROPERTY_NAME_MARQUEE_LOOP_DELAY             = "marqueeLoopDelay";
const char* const PROPERTY_NAME_MARQUEE_GAP                    = "marqueeGap";
const char* const PROPERTY_NAME_MARQUEE_ORIENTATION            = "marqueeOrientation";
const char* const PROPERTY_NAME_MARQUEE_STOP_MODE              = "marqueeStopMode";
const char* const PROPERTY_NAME_FONT_WEIGHT                    = "fontWeight";
const char* const PROPERTY_NAME_FONT_WIDTH                     = "fontWidth";
const char* const PROPERTY_NAME_FONT_SLANT                     = "fontSlant";
const char* const PROPERTY_NAME_TEXT_BACKGROUND_COLOR          = "textBackgroundColor";
const char* const PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE        = "minimumFontSizeScale";
const char* const PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE        = "maximumFontSizeScale";
const char* const PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED = "systemFontSizeScaleEnabled";
const char* const PROPERTY_NAME_CUTOUT_ENABLED                 = "cutoutEnabled";
const char* const PROPERTY_NAME_ASYNC_RENDERING                = "asyncRendering";
const char* const PROPERTY_NAME_RENDER_SCALE                   = "renderScale";

// Animatable
const char* const PROPERTY_NAME_TEXT_COLOR        = "textColor";
const char* const PROPERTY_NAME_PIXEL_SNAP_FACTOR = "pixelSnapFactor";

constexpr int ASYNC_TEXT_THREAD_TIMEOUT = 5;

bool  gAsyncNaturalSizeComputed    = false;
float gAsyncNaturalSizeWidth       = 0.0f;
float gAsyncNaturalSizeHeight      = 0.0f;
bool  gAsyncHeightForWidthComputed = false;
float gAsyncHeightForWidthWidth    = 0.0f;
float gAsyncHeightForWidthHeight   = 0.0f;
bool  gAsyncRenderFinished         = false;
float gAsyncRenderWidth            = 0.0f;
float gAsyncRenderHeight           = 0.0f;

void OnAsyncNaturalSizeComputed(View, float width, float height)
{
  gAsyncNaturalSizeComputed = true;
  gAsyncNaturalSizeWidth    = width;
  gAsyncNaturalSizeHeight   = height;
}

void OnAsyncHeightForWidthComputed(View, float width, float height)
{
  gAsyncHeightForWidthComputed = true;
  gAsyncHeightForWidthWidth    = width;
  gAsyncHeightForWidthHeight   = height;
}

void OnAsyncRenderFinished(View, float width, float height)
{
  gAsyncRenderFinished = true;
  gAsyncRenderWidth    = width;
  gAsyncRenderHeight   = height;
}

bool HasValidTextTexture(Actor actor)
{
  for(uint32_t rendererIndex = 0u; rendererIndex < actor.GetRendererCount(); ++rendererIndex)
  {
    TextureSet textures = actor.GetRendererAt(rendererIndex).GetTextures();
    if(!textures || textures.GetTextureCount() == 0u)
    {
      continue;
    }

    Texture texture = textures.GetTexture(0u);
    if(texture && texture.GetWidth() > 0u && texture.GetHeight() > 0u)
    {
      return true;
    }
  }
  return false;
}

void CheckEmbossRendererProperties(Actor actor, const Vector2& direction, float strength, const Vector4& lightColor, const Vector4& shadowColor)
{
  bool foundEmbossRenderer = false;
  for(uint32_t rendererIndex = 0u; rendererIndex < actor.GetRendererCount(); ++rendererIndex)
  {
    Renderer              renderer       = actor.GetRendererAt(rendererIndex);
    const Property::Index directionIndex = renderer.GetPropertyIndex("uEmbossDirection");
    if(directionIndex == Property::INVALID_INDEX)
    {
      continue;
    }

    foundEmbossRenderer = true;
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(directionIndex), direction, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetProperty<float>(renderer.GetPropertyIndex("uEmbossStrength")), strength, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("uEmbossLightColor")), lightColor, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("uEmbossShadowColor")), shadowColor, TEST_LOCATION);
  }
  DALI_TEST_CHECK(foundEmbossRenderer);
}

bool WaitForValidTextTexture(UiTestApplication& application, Label label)
{
  constexpr uint32_t MAX_TRIGGER_COUNT = 4u;
  for(uint32_t trigger = 0u; trigger < MAX_TRIGGER_COUNT && !HasValidTextTexture(label); ++trigger)
  {
    if(!Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT))
    {
      return false;
    }
    application.SendNotification();
    application.Render();
  }
  return HasValidTextTexture(label);
}

bool WaitForAsyncNaturalSize(UiTestApplication& application)
{
  constexpr uint32_t MAX_TRIGGER_COUNT = 4u;
  for(uint32_t trigger = 0u; trigger < MAX_TRIGGER_COUNT && !gAsyncNaturalSizeComputed; ++trigger)
  {
    if(!Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT))
    {
      return false;
    }
    application.SendNotification();
    application.Render();
  }
  return gAsyncNaturalSizeComputed;
}

bool WaitForAsyncHeightForWidth(UiTestApplication& application)
{
  constexpr uint32_t MAX_TRIGGER_COUNT = 4u;
  for(uint32_t trigger = 0u; trigger < MAX_TRIGGER_COUNT && !gAsyncHeightForWidthComputed; ++trigger)
  {
    if(!Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT))
    {
      return false;
    }
    application.SendNotification();
    application.Render();
  }
  return gAsyncHeightForWidthComputed;
}

bool WaitForAsyncRender(UiTestApplication& application)
{
  constexpr uint32_t MAX_TRIGGER_COUNT = 4u;
  for(uint32_t trigger = 0u; trigger < MAX_TRIGGER_COUNT && !gAsyncRenderFinished; ++trigger)
  {
    if(!Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT))
    {
      return false;
    }
    application.SendNotification();
    application.Render();
  }
  return gAsyncRenderFinished;
}
} // namespace

void utc_dali_label_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_label_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLabelConstructorP(void)
{
  UiTestApplication application;
  Label             label;
  DALI_TEST_CHECK(!label);
  END_TEST;
}

int UtcDaliLabelNewP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);
  END_TEST;
}

int UtcDaliLabelNewWithTextP(void)
{
  UiTestApplication application;
  Label             label = Label::New("Hello world");
  DALI_TEST_CHECK(label);
  END_TEST;
}

int UtcDaliLabelGetNaturalSizeP(void)
{
  UiTestApplication application;
  const Label       label = Label::New("Natural size");

  const Vector3 naturalSize = label.GetNaturalSize();
  DALI_TEST_CHECK(naturalSize.width > 0.0f);
  DALI_TEST_CHECK(naturalSize.height > 0.0f);
  DALI_TEST_EQUALS(naturalSize.depth, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelGetHeightForWidthP(void)
{
  UiTestApplication application;
  Label             label = Label::New("Height for width");

  DALI_TEST_CHECK(label.GetHeightForWidth(100.0f) > 0.0f);
  END_TEST;
}

int UtcDaliLabelMaximumLinesPublicApiP(void)
{
  UiTestApplication application;
  Label             label = Label::New("A\nB\nC");

  DALI_TEST_EQUALS(label.GetMaximumLines(), Text::MAXIMUM_LINES_UNLIMITED, TEST_LOCATION);

  label.SetMaximumLines(1);
  DALI_TEST_EQUALS(label.GetMaximumLines(), 1, TEST_LOCATION);
  label.SetMaximumLines(3);
  DALI_TEST_EQUALS(label.GetMaximumLines(), 3, TEST_LOCATION);
  label.SetMaximumLines(Text::MAXIMUM_LINES_UNLIMITED);
  DALI_TEST_EQUALS(label.GetMaximumLines(), Text::MAXIMUM_LINES_UNLIMITED, TEST_LOCATION);
  label.SetMaximumLines(-1);
  DALI_TEST_EQUALS(label.GetMaximumLines(), Text::MAXIMUM_LINES_UNLIMITED, TEST_LOCATION);

  DALI_TEST_CHECK(!label.IsMultiLine());
  label.SetMaximumLines(3);
  DALI_TEST_CHECK(!label.IsMultiLine());
  label.SetMultiLine(true);
  label.SetMaximumLines(1);
  DALI_TEST_CHECK(label.IsMultiLine());
  DALI_TEST_EQUALS(label.GetMaximumLines(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMaximumLinesMeasurementP(void)
{
  UiTestApplication application;
  constexpr float   WIDTH        = 500.0f;
  constexpr float   SIZE_EPSILON = 0.01f;
  const char* const FIVE_LINES   = "A\nB\nC\nD\nE";

  auto makeLabel = [](const char* text, int maximumLines)
  {
    Label label = Label::New(text);
    label.SetMultiLine(true);
    label.SetTextOverflowMode(Text::OverflowMode::CLIP);
    label.SetMaximumLines(maximumLines);
    return label;
  };

  Label         threeLineReference = makeLabel("A\nB\nC", Text::MAXIMUM_LINES_UNLIMITED);
  const Vector3 threeLineNatural   = threeLineReference.GetNaturalSize();
  const float   threeLineHfw       = threeLineReference.GetHeightForWidth(WIDTH);

  Label         legacyUnlimited = makeLabel(FIVE_LINES, Text::MAXIMUM_LINES_UNLIMITED);
  const Vector3 legacyNatural   = legacyUnlimited.GetNaturalSize();
  const float   legacyHfw       = legacyUnlimited.GetHeightForWidth(WIDTH);
  const int     legacyLineCount = legacyUnlimited.GetLineCount(WIDTH);
  DALI_TEST_EQUALS(legacyLineCount, 5, TEST_LOCATION);

  Label explicitUnlimited = makeLabel(FIVE_LINES, Text::MAXIMUM_LINES_UNLIMITED);
  explicitUnlimited.SetMaximumLines(Text::MAXIMUM_LINES_UNLIMITED);
  DALI_TEST_EQUALS(explicitUnlimited.GetNaturalSize(), legacyNatural, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitUnlimited.GetHeightForWidth(WIDTH), legacyHfw, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitUnlimited.GetLineCount(WIDTH), legacyLineCount, TEST_LOCATION);

  Label capped = makeLabel(FIVE_LINES, 3);
  DALI_TEST_EQUALS(capped.GetNaturalSize().height, threeLineNatural.height, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(capped.GetHeightForWidth(WIDTH), threeLineHfw, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(capped.GetLineCount(WIDTH), 3, TEST_LOCATION);

  Label equalLimit  = makeLabel(FIVE_LINES, 5);
  Label largerLimit = makeLabel(FIVE_LINES, 10);
  DALI_TEST_EQUALS(equalLimit.GetNaturalSize(), legacyNatural, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(largerLimit.GetNaturalSize(), legacyNatural, SIZE_EPSILON, TEST_LOCATION);

  Label         hiddenLongLine    = makeLabel("A\nB\nC\nTHIS_IS_A_VERY_VERY_VERY_LONG_HIDDEN_FOURTH_LINE", 3);
  const Vector3 hiddenLongNatural = hiddenLongLine.GetNaturalSize();
  DALI_TEST_EQUALS(hiddenLongNatural.width, threeLineNatural.width, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(hiddenLongNatural.height, threeLineNatural.height, SIZE_EPSILON, TEST_LOCATION);

  Label ellipsisReference = makeLabel("A\nB\nC\nD", 3);
  ellipsisReference.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  Label hiddenLongEllipsis =
    makeLabel("A\nB\nC\nTHIS_IS_A_VERY_VERY_VERY_LONG_HIDDEN_FOURTH_LINE", 3);
  hiddenLongEllipsis.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(hiddenLongEllipsis.GetNaturalSize(),
                   ellipsisReference.GetNaturalSize(),
                   SIZE_EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(hiddenLongEllipsis.GetHeightForWidth(WIDTH),
                   ellipsisReference.GetHeightForWidth(WIDTH),
                   SIZE_EPSILON,
                   TEST_LOCATION);

  Label styledReference = Label::New();
  styledReference.SetStyledText(Text::StyledText::FromMarkup("<b>A</b>\nB\n<i>C</i>"));
  styledReference.SetMultiLine(true);
  styledReference.SetTextOverflowMode(Text::OverflowMode::CLIP);
  const Vector3 styledReferenceNatural = styledReference.GetNaturalSize();
  const float   styledReferenceHfw     = styledReference.GetHeightForWidth(WIDTH);

  Label styled = Label::New();
  styled.SetStyledText(Text::StyledText::FromMarkup("<b>A</b>\nB\n<i>C</i>\nD\nE"));
  styled.SetMultiLine(true);
  styled.SetTextOverflowMode(Text::OverflowMode::CLIP);
  styled.SetMaximumLines(3);
  DALI_TEST_EQUALS(styled.GetNaturalSize(), styledReferenceNatural, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(styled.GetHeightForWidth(WIDTH), styledReferenceHfw, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(styled.GetLineCount(WIDTH), 3, TEST_LOCATION);

  Label paddedReference = makeLabel("A\nB\nC", Text::MAXIMUM_LINES_UNLIMITED);
  paddedReference.SetPadding(Insets(11.0f, 13.0f, 7.0f, 17.0f));
  Label paddedCapped = makeLabel(FIVE_LINES, 3);
  paddedCapped.SetPadding(Insets(11.0f, 13.0f, 7.0f, 17.0f));
  DALI_TEST_EQUALS(paddedCapped.GetNaturalSize(), paddedReference.GetNaturalSize(), SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(paddedCapped.GetHeightForWidth(WIDTH),
                   paddedReference.GetHeightForWidth(WIDTH),
                   SIZE_EPSILON,
                   TEST_LOCATION);

  Label wrapReference = makeLabel("A\nB\nC", Text::MAXIMUM_LINES_UNLIMITED);
  wrapReference.SetRequestedWidth(WRAP_CONTENT);
  wrapReference.SetRequestedHeight(WRAP_CONTENT);
  Label wrapCapped = makeLabel("A\nB\nC\nTHIS_IS_A_VERY_VERY_VERY_LONG_HIDDEN_FOURTH_LINE", 3);
  wrapCapped.SetRequestedWidth(WRAP_CONTENT);
  wrapCapped.SetRequestedHeight(WRAP_CONTENT);
  const MeasuredSize wrapReferenceSize = wrapReference.Measure(1000.0f, 1000.0f);
  const MeasuredSize wrapCappedSize    = wrapCapped.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(wrapCappedSize.GetWidth(), wrapReferenceSize.GetWidth(), SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(wrapCappedSize.GetHeight(), wrapReferenceSize.GetHeight(), SIZE_EPSILON, TEST_LOCATION);

  // LineCount follows the same capped HFW layout without changing its semantic state.
  const float cappedHfwBefore = capped.GetHeightForWidth(WIDTH);
  DALI_TEST_EQUALS(capped.GetLineCount(WIDTH), 3, TEST_LOCATION);
  const float cappedHfwAfter = capped.GetHeightForWidth(WIDTH);
  DALI_TEST_EQUALS(cappedHfwBefore, cappedHfwAfter, SIZE_EPSILON, TEST_LOCATION);

  Label       reverseOrder           = makeLabel(FIVE_LINES, 3);
  const int   reverseLineCountBefore = reverseOrder.GetLineCount(WIDTH);
  const float reverseHfw             = reverseOrder.GetHeightForWidth(WIDTH);
  const int   reverseLineCountAfter  = reverseOrder.GetLineCount(WIDTH);
  DALI_TEST_EQUALS(reverseLineCountBefore, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(reverseLineCountAfter, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(reverseHfw, threeLineHfw, SIZE_EPSILON, TEST_LOCATION);

  // Changing MaximumLines invalidates both NaturalSize and HFW caches.
  Label invalidation = makeLabel(FIVE_LINES, Text::MAXIMUM_LINES_UNLIMITED);
  DALI_TEST_EQUALS(invalidation.GetNaturalSize(), legacyNatural, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(invalidation.GetHeightForWidth(WIDTH), legacyHfw, SIZE_EPSILON, TEST_LOCATION);
  invalidation.SetMaximumLines(3);
  DALI_TEST_EQUALS(invalidation.GetNaturalSize().height, threeLineNatural.height, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(invalidation.GetHeightForWidth(WIDTH), threeLineHfw, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(invalidation.GetLineCount(WIDTH), 3, TEST_LOCATION);
  invalidation.SetMaximumLines(Text::MAXIMUM_LINES_UNLIMITED);
  DALI_TEST_EQUALS(invalidation.GetNaturalSize(), legacyNatural, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(invalidation.GetHeightForWidth(WIDTH), legacyHfw, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(invalidation.GetLineCount(WIDTH), 5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMaximumLinesMeasurementSequenceP(void)
{
  UiTestApplication application;
  constexpr float   WIDTH      = 500.0f;
  constexpr float   HEIGHT     = 500.0f;
  const char* const FIVE_LINES = "A\nB\nC\nD\nE";

  auto render = [&](Label label)
  {
    application.GetScene().Add(label);
    application.SendNotification();
    application.Render();
  };

  auto verifySequence = [&](int maximumNumberOfLines, int sequence)
  {
    Label label = Label::New(FIVE_LINES);
    label.SetMultiLine(true);
    label.SetTextOverflowMode(Text::OverflowMode::CLIP);
    label.SetRequestedWidth(WIDTH);
    label.SetRequestedHeight(HEIGHT);
    label.SetMaximumLines(maximumNumberOfLines);

    switch(sequence)
    {
      case 0: // NaturalSize -> HFW -> LineCount -> Render
      {
        label.GetNaturalSize();
        label.GetHeightForWidth(WIDTH);
        label.GetLineCount(WIDTH);
        render(label);
        break;
      }
      case 1: // Render -> LineCount
      {
        render(label);
        label.GetLineCount(WIDTH);
        break;
      }
      case 2: // Render -> HFW -> LineCount
      {
        render(label);
        label.GetHeightForWidth(WIDTH);
        label.GetLineCount(WIDTH);
        break;
      }
      case 3: // LineCount -> Render
      {
        label.GetLineCount(WIDTH);
        render(label);
        break;
      }
      case 4: // HFW -> Render -> LineCount
      {
        label.GetHeightForWidth(WIDTH);
        render(label);
        label.GetLineCount(WIDTH);
        break;
      }
    }

    const int expectedLineCount = maximumNumberOfLines == Text::MAXIMUM_LINES_UNLIMITED ? 5 : 3;
    DALI_TEST_EQUALS(label.GetLineCount(), expectedLineCount, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetLineCount(WIDTH), expectedLineCount, TEST_LOCATION);
    application.GetScene().Remove(label);
  };

  for(int sequence = 0; sequence < 5; ++sequence)
  {
    verifySequence(Text::MAXIMUM_LINES_UNLIMITED, sequence);
    verifySequence(3, sequence);
  }

  END_TEST;
}

int UtcDaliLabelMaximumLinesSoftWrapAndNewlineP(void)
{
  UiTestApplication application;
  constexpr float   WRAP_WIDTH = 90.0f;

  struct WrapCase
  {
    Text::LineWrapMode mode;
    const char*        text;
  };
  const WrapCase wrapCases[] = {
    {Text::LineWrapMode::WORD,
     "one two three four five six seven eight nine ten eleven twelve thirteen fourteen"},
    {Text::LineWrapMode::CHARACTER,
     "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"},
  };

  for(const WrapCase& wrapCase : wrapCases)
  {
    auto makeLabel = [&]()
    {
      Label label = Label::New(wrapCase.text);
      label.SetMultiLine(true);
      label.SetLineWrapMode(wrapCase.mode);
      label.SetTextOverflowMode(Text::OverflowMode::CLIP);
      return label;
    };

    Label     unlimited         = makeLabel();
    const int requiredLineCount = unlimited.GetLineCount(WRAP_WIDTH);
    DALI_TEST_CHECK(requiredLineCount >= 4);

    for(int limit : {requiredLineCount - 1, requiredLineCount, requiredLineCount + 1})
    {
      Label capped = makeLabel();
      capped.SetMaximumLines(limit);
      DALI_TEST_EQUALS(capped.GetLineCount(WRAP_WIDTH), std::min(requiredLineCount, limit), TEST_LOCATION);
    }
  }

  auto makeNewlineLabel = [](const char* text)
  {
    Label label = Label::New(text);
    label.SetMultiLine(true);
    label.SetTextOverflowMode(Text::OverflowMode::CLIP);
    label.SetMaximumLines(2);
    return label;
  };

  Label exact            = makeNewlineLabel("A\nB");
  Label trailingNewline  = makeNewlineLabel("A\nB\n");
  Label explicitOverflow = makeNewlineLabel("A\nB\nC");
  DALI_TEST_EQUALS(exact.GetLineCount(500.0f), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(trailingNewline.GetLineCount(500.0f), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitOverflow.GetLineCount(500.0f), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(trailingNewline.GetNaturalSize().height, exact.GetNaturalSize().height, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitOverflow.GetNaturalSize().height, exact.GetNaturalSize().height, 0.01f, TEST_LOCATION);

  // Every character classified as a paragraph separator must obey the same
  // exact-N, N+1 and trailing-empty-line MaximumLines contract as LF.
  const std::string paragraphSeparators[] = {
    "\n",
    "\v",
    "\f",
    "\r",
    "\xC2\x85",       // NEL
    "\xE2\x80\xA8", // LINE SEPARATOR
    "\xE2\x80\xA9", // PARAGRAPH SEPARATOR
  };
  for(const std::string& separator : paragraphSeparators)
  {
    const std::string exactText    = std::string("A") + separator + "B";
    const std::string overflowText = exactText + separator + "C";
    const std::string trailingText = exactText + separator;
    Label             exactSeparator = makeNewlineLabel(exactText.c_str());
    Label             overflowSeparator = makeNewlineLabel(overflowText.c_str());
    Label             trailingSeparator = makeNewlineLabel(trailingText.c_str());
    DALI_TEST_EQUALS(exactSeparator.GetLineCount(500.0f), 2, TEST_LOCATION);
    DALI_TEST_EQUALS(overflowSeparator.GetLineCount(500.0f), 2, TEST_LOCATION);
    DALI_TEST_EQUALS(trailingSeparator.GetLineCount(500.0f), 2, TEST_LOCATION);
    DALI_TEST_EQUALS(overflowSeparator.GetNaturalSize().height,
                     exactSeparator.GetNaturalSize().height,
                     0.01f,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(trailingSeparator.GetNaturalSize().height,
                     exactSeparator.GetNaturalSize().height,
                     0.01f,
                     TEST_LOCATION);
  }

  // The second retained line may be empty. A later authored line must not leak
  // into measurement merely because the retained boundary contains no glyphs.
  Label interiorEmptyReference = makeNewlineLabel("A\n");
  Label interiorEmptyOverflow  = makeNewlineLabel("A\n\nB");
  DALI_TEST_EQUALS(interiorEmptyOverflow.GetLineCount(500.0f), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(interiorEmptyOverflow.GetNaturalSize(),
                   interiorEmptyReference.GetNaturalSize(),
                   0.01f,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(interiorEmptyOverflow.GetHeightForWidth(500.0f),
                   interiorEmptyReference.GetHeightForWidth(500.0f),
                   0.01f,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMaximumLinesAsyncAndStaleResultP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Ensure FontClient is initialized before worker text work starts.
  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  (void)fontClient;

  constexpr float WIDTH        = 500.0f;
  constexpr float HEIGHT       = 500.0f;
  constexpr float SIZE_EPSILON = 1.0f;

  Label label = Label::New("A\nB\nC\nD\nE");
  label.SetMultiLine(true);
  label.SetTextOverflowMode(Text::OverflowMode::CLIP);
  label.SetRequestedWidth(WIDTH);
  label.SetRequestedHeight(HEIGHT);
  label.SetMaximumLines(5);
  label.SetAsyncRendering(true);
  label.AsyncRenderFinishedSignal().Connect(&OnAsyncRenderFinished);
  label.AsyncNaturalSizeComputedSignal().Connect(&OnAsyncNaturalSizeComputed);
  label.AsyncHeightForWidthComputedSignal().Connect(&OnAsyncHeightForWidthComputed);

  gAsyncRenderFinished = false;
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(); // Request A snapshots MaximumLines=5.

  // Request B supersedes A. Completion from A must not publish five lines.
  label.SetMaximumLines(2);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForAsyncRender(application));
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetLineCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetLineCount(WIDTH), 2, TEST_LOCATION);

  const Vector3 syncNatural = label.GetNaturalSize();
  gAsyncNaturalSizeComputed = false;
  label.RequestAsyncNaturalSize();
  DALI_TEST_CHECK(WaitForAsyncNaturalSize(application));
  DALI_TEST_EQUALS(gAsyncNaturalSizeWidth, syncNatural.width, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gAsyncNaturalSizeHeight, syncNatural.height, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 2, TEST_LOCATION);

  const float syncHeightForWidth = label.GetHeightForWidth(WIDTH);
  gAsyncHeightForWidthComputed   = false;
  label.RequestAsyncHeightForWidth(WIDTH);
  DALI_TEST_CHECK(WaitForAsyncHeightForWidth(application));
  DALI_TEST_EQUALS(gAsyncHeightForWidthWidth, WIDTH, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gAsyncHeightForWidthHeight, syncHeightForWidth, SIZE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 2, TEST_LOCATION);

  // A value-only stale check cannot distinguish 5 -> 2 -> 5. The old
  // natural-size result must still be rejected by its MaximumLines revision.
  label.SetMaximumLines(5);
  gAsyncNaturalSizeComputed = false;
  label.RequestAsyncNaturalSize();
  label.SetMaximumLines(2);
  label.SetMaximumLines(5);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!gAsyncNaturalSizeComputed);

  // Rejecting the stale completion must not leave the request lifecycle stuck.
  label.RequestAsyncNaturalSize();
  DALI_TEST_CHECK(WaitForAsyncNaturalSize(application));
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 5, TEST_LOCATION);

  // HFW owns an independent request lifecycle and must also reject a true
  // same-value ABA completion, not only a value-different stale result.
  label.SetMaximumLines(5);
  gAsyncHeightForWidthComputed = false;
  label.RequestAsyncHeightForWidth(WIDTH);
  label.SetMaximumLines(2);
  label.SetMaximumLines(5);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!gAsyncHeightForWidthComputed);

  label.RequestAsyncHeightForWidth(WIDTH);
  DALI_TEST_CHECK(WaitForAsyncHeightForWidth(application));
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 5, TEST_LOCATION);

  // Render requests use a third revision owner. Start request A, return to the
  // same MaximumLines value without publishing an intermediate frame, and verify
  // that A cannot emit completion or replace the latest renderer.
  gAsyncRenderFinished = false;
  label.SetRequestedWidth(WIDTH - 1.0f); // Force request A while MaximumLines remains 5.
  application.SendNotification();
  application.Render(); // Request A snapshots MaximumLines=5.
  label.SetMaximumLines(2);
  label.SetMaximumLines(5);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!gAsyncRenderFinished);
  DALI_TEST_CHECK(WaitForAsyncRender(application));
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 5, TEST_LOCATION);

  gAsyncRenderFinished = false;
  const int rapidLimits[] = {5, 2, 5, 1, Text::MAXIMUM_LINES_UNLIMITED};
  for(uint32_t index = 0u; index < sizeof(rapidLimits) / sizeof(rapidLimits[0]); ++index)
  {
    label.SetMaximumLines(rapidLimits[index]);
    label.SetRequestedWidth(WIDTH - static_cast<float>(index * 10u));
    application.SendNotification();
    application.Render();
  }
  constexpr uint32_t RAPID_UPDATE_MAX_TRIGGER_COUNT = 12u;
  for(uint32_t trigger = 0u; trigger < RAPID_UPDATE_MAX_TRIGGER_COUNT && !gAsyncRenderFinished; ++trigger)
  {
    if(!Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT))
    {
      break;
    }
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(gAsyncRenderFinished);
  DALI_TEST_EQUALS(label.GetMaximumLines(), Text::MAXIMUM_LINES_UNLIMITED, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 5, TEST_LOCATION);

  label.SetAsyncRendering(false);
  label.SetMaximumLines(3);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(label.GetLineCount(WIDTH), 3, TEST_LOCATION);

  label.SetAsyncRendering(true);
  gAsyncRenderFinished = false;
  label.SetMaximumLines(2);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForAsyncRender(application));
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 2, TEST_LOCATION);

  label.SetAsyncRendering(false);
  label.SetMaximumLines(Text::MAXIMUM_LINES_UNLIMITED);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(label.GetLineCount(WIDTH), 5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMaximumLinesEdgeCasesP(void)
{
  UiTestApplication application;
  constexpr float   WIDTH        = 240.0f;
  constexpr float   HEIGHT       = 120.0f;
  constexpr float   SIZE_EPSILON = 0.01f;

  auto makeLabel = [](const char* text, int maximumNumberOfLines)
  {
    Label label = Label::New(text);
    label.SetMultiLine(true);
    label.SetTextOverflowMode(Text::OverflowMode::CLIP);
    label.SetMaximumLines(maximumNumberOfLines);
    return label;
  };

  Label singleLineReference = Label::New("A\nB\nC");
  singleLineReference.SetMultiLine(false);
  singleLineReference.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  const Vector3 singleLineNatural = singleLineReference.GetNaturalSize();
  const float   singleLineHfw     = singleLineReference.GetHeightForWidth(WIDTH);
  for(int maximumNumberOfLines : {1, 3})
  {
    Label singleLine = Label::New("A\nB\nC");
    singleLine.SetMultiLine(false);
    singleLine.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
    singleLine.SetMaximumLines(maximumNumberOfLines);
    DALI_TEST_CHECK(!singleLine.IsMultiLine());
    DALI_TEST_EQUALS(singleLine.GetNaturalSize(), singleLineNatural, SIZE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(singleLine.GetHeightForWidth(WIDTH), singleLineHfw, SIZE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(singleLine.GetLineCount(WIDTH), 1, TEST_LOCATION);
  }

  for(const char* text : {"", "A", "\n", "A\n"})
  {
    for(int maximumNumberOfLines : {Text::MAXIMUM_LINES_UNLIMITED, 1, 2, 3})
    {
      Label label = makeLabel(text, maximumNumberOfLines);
      const Vector3 natural = label.GetNaturalSize();
      DALI_TEST_CHECK(std::isfinite(natural.width));
      DALI_TEST_CHECK(std::isfinite(natural.height));
      DALI_TEST_CHECK(natural.width >= 0.0f);
      DALI_TEST_CHECK(natural.height >= 0.0f);
      for(float width : {0.0f, Math::MACHINE_EPSILON_1000, WIDTH})
      {
        const float hfw       = label.GetHeightForWidth(width);
        const int   lineCount = label.GetLineCount(width);
        DALI_TEST_CHECK(std::isfinite(hfw));
        DALI_TEST_CHECK(hfw >= 0.0f);
        DALI_TEST_CHECK(lineCount >= 0);
        if(maximumNumberOfLines > Text::MAXIMUM_LINES_UNLIMITED)
        {
          DALI_TEST_CHECK(lineCount <= maximumNumberOfLines);
        }
      }
    }
  }

  for(float width : {0.0f, Math::MACHINE_EPSILON_1000})
  {
    for(float height : {0.0f, Math::MACHINE_EPSILON_1000})
    {
      for(int maximumNumberOfLines : {Text::MAXIMUM_LINES_UNLIMITED, 1, 3})
      {
        for(Text::OverflowMode overflow : {Text::OverflowMode::CLIP, Text::OverflowMode::ELLIPSIS})
        {
          Label label = makeLabel("A\nB\nC\nD", maximumNumberOfLines);
          label.SetTextOverflowMode(overflow);
          label.SetRequestedWidth(width);
          label.SetRequestedHeight(height);
          application.GetScene().Add(label);
        }
      }
    }
  }
  Label zeroAreaFit = makeLabel("A long text-fit value that wraps across several lines", 3);
  zeroAreaFit.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  zeroAreaFit.SetTextFit(Text::Fit::Range(8.0f, 40.0f, 4.0f));
  zeroAreaFit.SetRequestedWidth(Math::MACHINE_EPSILON_1000);
  zeroAreaFit.SetRequestedHeight(0.0f);
  application.GetScene().Add(zeroAreaFit);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(zeroAreaFit.GetLineCount(Math::MACHINE_EPSILON_1000) <= 3);

  const Insets paddings[] = {
    Insets(),
    Insets(8.0f, 8.0f, 8.0f, 8.0f),
    Insets(3.0f, 17.0f, 5.0f, 11.0f),
  };
  for(const Insets& padding : paddings)
  {
    Label reference = makeLabel("A\nB\nC", Text::MAXIMUM_LINES_UNLIMITED);
    Label capped    = makeLabel("A\nB\nC\nTHIS_IS_A_VERY_LONG_HIDDEN_LINE", 3);
    reference.SetPadding(padding);
    capped.SetPadding(padding);
    DALI_TEST_EQUALS(capped.GetNaturalSize(), reference.GetNaturalSize(), SIZE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(capped.GetHeightForWidth(WIDTH),
                     reference.GetHeightForWidth(WIDTH),
                     SIZE_EPSILON,
                     TEST_LOCATION);
  }

  struct MeasureCase
  {
    float requestedWidth;
    float requestedHeight;
  };
  const MeasureCase measureCases[] = {
    {WRAP_CONTENT, WRAP_CONTENT},
    {WIDTH, WRAP_CONTENT},
    {MATCH_PARENT, WRAP_CONTENT},
  };
  for(const MeasureCase& measureCase : measureCases)
  {
    Label reference = makeLabel("A\nB\nC", Text::MAXIMUM_LINES_UNLIMITED);
    Label capped    = makeLabel("A\nB\nC\nTHIS_IS_A_VERY_LONG_HIDDEN_LINE", 3);
    reference.SetRequestedWidth(measureCase.requestedWidth);
    reference.SetRequestedHeight(measureCase.requestedHeight);
    capped.SetRequestedWidth(measureCase.requestedWidth);
    capped.SetRequestedHeight(measureCase.requestedHeight);
    const MeasuredSize referenceSize = reference.Measure(WIDTH, HEIGHT);
    const MeasuredSize cappedSize    = capped.Measure(WIDTH, HEIGHT);
    DALI_TEST_EQUALS(cappedSize.GetWidth(), referenceSize.GetWidth(), SIZE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(cappedSize.GetHeight(), referenceSize.GetHeight(), SIZE_EPSILON, TEST_LOCATION);
  }

  Label wrapFit = makeLabel("A\nB\nC\nTHIS_IS_A_VERY_LONG_HIDDEN_LINE", 3);
  wrapFit.SetRequestedWidth(WRAP_CONTENT);
  wrapFit.SetRequestedHeight(WRAP_CONTENT);
  wrapFit.SetTextFit(Text::Fit::Range(8.0f, 40.0f, 4.0f));
  const MeasuredSize wrapFitSize = wrapFit.Measure(WIDTH, HEIGHT);
  DALI_TEST_CHECK(std::isfinite(wrapFitSize.GetWidth()));
  DALI_TEST_CHECK(std::isfinite(wrapFitSize.GetHeight()));
  DALI_TEST_CHECK(wrapFitSize.GetWidth() <= WIDTH);
  DALI_TEST_CHECK(wrapFitSize.GetHeight() <= HEIGHT);

  END_TEST;
}

int UtcDaliLabelMaximumLinesTextFitP(void)
{
  UiTestApplication application;
  constexpr float   WIDTH  = 200.0f;
  constexpr float   HEIGHT = 1000.0f;
  const char* const TEXT   = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

  auto makeLabel = [&](int maximumNumberOfLines)
  {
    Label label = Label::New(TEXT);
    label.SetRequestedWidth(WIDTH);
    label.SetRequestedHeight(HEIGHT);
    label.SetMultiLine(true);
    label.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
    label.SetMaximumLines(maximumNumberOfLines);
    application.GetScene().Add(label);
    return label;
  };

  Label rangeUnlimited = makeLabel(Text::MAXIMUM_LINES_UNLIMITED);
  rangeUnlimited.SetTextFit(Text::Fit::Range(8.0f, 40.0f, 4.0f));
  Label rangeCapped = makeLabel(2);
  rangeCapped.SetTextFit(Text::Fit::Range(8.0f, 40.0f, 4.0f));

  Dali::Vector<Text::Fit::Candidate> candidates;
  candidates.PushBack(Text::Fit::Candidate(8.0f, 10.0f));
  candidates.PushBack(Text::Fit::Candidate(16.0f, 18.0f));
  candidates.PushBack(Text::Fit::Candidate(24.0f, 26.0f));
  candidates.PushBack(Text::Fit::Candidate(32.0f, 34.0f));
  candidates.PushBack(Text::Fit::Candidate(40.0f, 42.0f));

  Label candidatesUnlimited = makeLabel(Text::MAXIMUM_LINES_UNLIMITED);
  candidatesUnlimited.SetTextFit(candidates);
  Label candidatesCapped = makeLabel(2);
  candidatesCapped.SetTextFit(candidates);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(rangeUnlimited.GetLineCount(WIDTH) > 2);
  DALI_TEST_CHECK(rangeCapped.GetLineCount(WIDTH) <= 2);
  DALI_TEST_CHECK(candidatesUnlimited.GetLineCount(WIDTH) > 2);
  DALI_TEST_CHECK(candidatesCapped.GetLineCount(WIDTH) <= 2);

  const Vector3 rangeUnlimitedNatural = rangeUnlimited.GetNaturalSize();
  const float   rangeUnlimitedHfw     = rangeUnlimited.GetHeightForWidth(WIDTH);
  const Vector3 rangeCappedNatural    = rangeCapped.GetNaturalSize();
  const float   rangeCappedHfw        = rangeCapped.GetHeightForWidth(WIDTH);
  rangeUnlimited.SetMaximumLines(2);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(rangeUnlimited.GetLineCount(WIDTH) <= 2);
  DALI_TEST_EQUALS(rangeUnlimited.GetNaturalSize(), rangeCappedNatural, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(rangeUnlimited.GetHeightForWidth(WIDTH), rangeCappedHfw, 0.01f, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rangeUnlimited.GetNaturalSize(), rangeCappedNatural, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(rangeUnlimited.GetHeightForWidth(WIDTH), rangeCappedHfw, 0.01f, TEST_LOCATION);
  rangeUnlimited.SetMaximumLines(Text::MAXIMUM_LINES_UNLIMITED);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(rangeUnlimited.GetLineCount(WIDTH) > 2);
  DALI_TEST_EQUALS(rangeUnlimited.GetNaturalSize(), rangeUnlimitedNatural, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(rangeUnlimited.GetHeightForWidth(WIDTH), rangeUnlimitedHfw, 0.01f, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rangeUnlimited.GetNaturalSize(), rangeUnlimitedNatural, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(rangeUnlimited.GetHeightForWidth(WIDTH), rangeUnlimitedHfw, 0.01f, TEST_LOCATION);

  const Vector3 candidatesUnlimitedNatural = candidatesUnlimited.GetNaturalSize();
  const float   candidatesUnlimitedHfw     = candidatesUnlimited.GetHeightForWidth(WIDTH);
  const Vector3 candidatesCappedNatural    = candidatesCapped.GetNaturalSize();
  const float   candidatesCappedHfw        = candidatesCapped.GetHeightForWidth(WIDTH);
  candidatesUnlimited.SetMaximumLines(2);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(candidatesUnlimited.GetLineCount(WIDTH) <= 2);
  DALI_TEST_EQUALS(candidatesUnlimited.GetNaturalSize(), candidatesCappedNatural, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(candidatesUnlimited.GetHeightForWidth(WIDTH), candidatesCappedHfw, 0.01f, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(candidatesUnlimited.GetNaturalSize(), candidatesCappedNatural, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(candidatesUnlimited.GetHeightForWidth(WIDTH), candidatesCappedHfw, 0.01f, TEST_LOCATION);
  candidatesUnlimited.SetMaximumLines(Text::MAXIMUM_LINES_UNLIMITED);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(candidatesUnlimited.GetLineCount(WIDTH) > 2);
  DALI_TEST_EQUALS(candidatesUnlimited.GetNaturalSize(), candidatesUnlimitedNatural, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(candidatesUnlimited.GetHeightForWidth(WIDTH), candidatesUnlimitedHfw, 0.01f, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(candidatesUnlimited.GetNaturalSize(), candidatesUnlimitedNatural, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(candidatesUnlimited.GetHeightForWidth(WIDTH), candidatesUnlimitedHfw, 0.01f, TEST_LOCATION);

  auto verifyAsyncTransition = [&](Label          label,
                                   const Vector3& cappedNatural,
                                   float          cappedHfw,
                                   const Vector3& unlimitedNatural,
                                   float          unlimitedHfw)
  {
    label.AsyncRenderFinishedSignal().Connect(&OnAsyncRenderFinished);
    label.SetAsyncRendering(true);

    gAsyncRenderFinished = false;
    label.SetMaximumLines(2);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(WaitForAsyncRender(application));
    DALI_TEST_CHECK(label.GetAsyncLineCount() <= 2);
    DALI_TEST_CHECK(label.GetLineCount(WIDTH) <= 2);
    DALI_TEST_EQUALS(label.GetNaturalSize(), cappedNatural, 1.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetHeightForWidth(WIDTH), cappedHfw, 1.0f, TEST_LOCATION);

    gAsyncRenderFinished = false;
    label.SetMaximumLines(Text::MAXIMUM_LINES_UNLIMITED);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(WaitForAsyncRender(application));
    DALI_TEST_CHECK(label.GetAsyncLineCount() > 2);
    DALI_TEST_CHECK(label.GetLineCount(WIDTH) > 2);
    DALI_TEST_EQUALS(label.GetNaturalSize(), unlimitedNatural, 1.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetHeightForWidth(WIDTH), unlimitedHfw, 1.0f, TEST_LOCATION);

    label.SetAsyncRendering(false);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(label.GetLineCount(WIDTH) > 2);
  };

  verifyAsyncTransition(rangeUnlimited,
                        rangeCappedNatural,
                        rangeCappedHfw,
                        rangeUnlimitedNatural,
                        rangeUnlimitedHfw);
  verifyAsyncTransition(candidatesUnlimited,
                        candidatesCappedNatural,
                        candidatesCappedHfw,
                        candidatesUnlimitedNatural,
                        candidatesUnlimitedHfw);

  // MaximumLines is inert while multiline is disabled. Preserve the legacy
  // single-line TextFit result for both APIs, including MaximumLines=1 where a
  // careless line-cap check could reject every fitting candidate.
  auto makeSingleLineFit = [&](int maximumNumberOfLines, bool useCandidates)
  {
    Label label = Label::New(TEXT);
    label.SetRequestedWidth(WIDTH);
    label.SetRequestedHeight(120.0f);
    label.SetMultiLine(false);
    label.SetMaximumLines(maximumNumberOfLines);
    if(useCandidates)
    {
      label.SetTextFit(candidates);
    }
    else
    {
      label.SetTextFit(Text::Fit::Range(8.0f, 40.0f, 4.0f));
    }
    application.GetScene().Add(label);
    return label;
  };

  for(bool useCandidates : {false, true})
  {
    Label legacySingleLine = makeSingleLineFit(Text::MAXIMUM_LINES_UNLIMITED, useCandidates);
    Label maxOneSingleLine = makeSingleLineFit(1, useCandidates);
    Label maxThreeSingleLine = makeSingleLineFit(3, useCandidates);
    application.SendNotification();
    application.Render();
    const Vector3 legacyNatural = legacySingleLine.GetNaturalSize();
    const float   legacyHfw     = legacySingleLine.GetHeightForWidth(WIDTH);
    for(Label constrained : {maxOneSingleLine, maxThreeSingleLine})
    {
      DALI_TEST_EQUALS(constrained.GetLineCount(WIDTH), 1, TEST_LOCATION);
      DALI_TEST_EQUALS(constrained.GetNaturalSize(), legacyNatural, 0.01f, TEST_LOCATION);
      DALI_TEST_EQUALS(constrained.GetHeightForWidth(WIDTH), legacyHfw, 0.01f, TEST_LOCATION);
    }
  }

  auto makeImpossibleLabel = [&]()
  {
    Label label = Label::New("A\nB\nC");
    label.SetRequestedWidth(WIDTH);
    label.SetRequestedHeight(HEIGHT);
    label.SetMultiLine(true);
    label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
    label.SetMaximumLines(2);
    application.GetScene().Add(label);
    return label;
  };

  Label impossibleRange = makeImpossibleLabel();
  impossibleRange.SetTextFit(Text::Fit::Range(8.0f, 40.0f, 4.0f));
  Label impossibleCandidates = makeImpossibleLabel();
  impossibleCandidates.SetTextFit(candidates);
  application.SendNotification();
  application.Render();

  for(Label label : {impossibleRange, impossibleCandidates})
  {
    DALI_TEST_EQUALS(label.GetLineCount(WIDTH), 2, TEST_LOCATION);
    const Vector3 natural = label.GetNaturalSize();
    const float   hfw     = label.GetHeightForWidth(WIDTH);
    DALI_TEST_CHECK(std::isfinite(natural.width));
    DALI_TEST_CHECK(std::isfinite(natural.height));
    DALI_TEST_CHECK(std::isfinite(hfw));
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(label.GetLineCount(WIDTH), 2, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetNaturalSize(), natural, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetHeightForWidth(WIDTH), hfw, 0.01f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliLabelCopyConstructorP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  Label             copy(label);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(label == copy);
  END_TEST;
}

int UtcDaliLabelMoveConstructor(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_EQUALS(1, label.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Label moved = std::move(label);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!label);
  END_TEST;
}

int UtcDaliLabelAssignmentOperatorP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  Label             copy;
  copy = label;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(label == copy);
  END_TEST;
}

int UtcDaliLabelMoveAssignment(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_EQUALS(1, label.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Label moved;
  moved = std::move(label);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!label);
  END_TEST;
}

int UtcDaliLabelDownCastP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  BaseHandle        object(label);
  Label             label2 = Label::DownCast(object);
  Label             label3 = DownCast<Label>(object);
  DALI_TEST_CHECK(label2);
  DALI_TEST_CHECK(label3);
  END_TEST;
}

int UtcDaliLabelDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitializedObject;
  Label             label1 = Label::DownCast(unInitializedObject);
  Label             label2 = DownCast<Label>(unInitializedObject);
  DALI_TEST_CHECK(!label1);
  DALI_TEST_CHECK(!label2);
  END_TEST;
}

int UtcDaliLabelInvokeMethod(void)
{
  UiTestApplication application;
  Label             label = Label::New();

  InvokeArguments textArguments;
  textArguments.PushBack(Any(String("Invoked label")));

  InvokeResult result;
  DALI_TEST_CHECK(label.InvokeMethod("SetText", textArguments, result));
  DALI_TEST_CHECK(label.InvokeMethod("GetText", InvokeArguments(), result));

  const String* text = AnyCast<String>(&result);
  DALI_TEST_CHECK(text);
  DALI_TEST_EQUALS(*text, String("Invoked label"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextRevealPublicApiP(void)
{
  UiTestApplication application;
  Label             label = Label::New("Hello reveal world");
  label.SetProperty(Actor::Property::SIZE, Vector3(320.0f, 80.0f, 0.0f));

  DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), Property::INVALID_INDEX, TEST_LOCATION);
  label.SetTextReveal(Text::Reveal::None());
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), Property::INVALID_INDEX, TEST_LOCATION);

  Text::Reveal reveal;
  DALI_TEST_EQUALS(reveal.GetSequence(), Text::Reveal::Sequence::WHOLE_TEXT, TEST_LOCATION);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.0f, 0.0001f, TEST_LOCATION);
  reveal.SetUnit(Text::Reveal::Unit::WORD);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.5f);
  reveal.SetFadeDurationRatio(0.0f);
  label.SetTextReveal(reveal);
  Text::Reveal nearlyEqualReveal(reveal);
  nearlyEqualReveal.SetFadeDurationRatio(std::nextafter(0.0f, 1.0f));
  label.SetTextReveal(nearlyEqualReveal);
  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  DALI_TEST_CHECK(label.GetTextReveal().GetFadeDurationRatio() == 0.0f);
  DALI_TEST_EQUALS(label.GetTextReveal().GetSequence(), Text::Reveal::Sequence::PER_LINE, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextReveal().GetSequenceStaggerRatio(), 0.5f, 0.0001f, TEST_LOCATION);
  label.SetTextRevealProgress(-2.0f);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.0f, 0.0001f, TEST_LOCATION);
  label.SetTextRevealProgress(2.0f);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 1.0f, 0.0001f, TEST_LOCATION);
  label.SetTextRevealProgress(std::numeric_limits<float>::quiet_NaN());
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.0f, 0.0001f, TEST_LOCATION);
  label.SetTextRevealProgress(std::numeric_limits<float>::infinity());
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 1.0f, 0.0001f, TEST_LOCATION);
  label.SetTextRevealProgress(-std::numeric_limits<float>::infinity());
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.0f, 0.0001f, TEST_LOCATION);
  label.SetTextRevealProgress(0.4f);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  const Property::Index progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  DALI_TEST_CHECK(progressIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetPropertyIndex("uTextRevealFadeDuration") != Property::INVALID_INDEX);

  reveal.SetUnit(Text::Reveal::Unit::LINE);
  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetTextReveal().GetUnit(), Text::Reveal::Unit::LINE, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);

  label.SetText("Changed text keeps normalized progress");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);

  label.SetTextReveal(Text::Reveal::None());
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
  // The renderer handle may retain a dormant registered uniform, but the base shader and
  // one-texture set prove that the reveal feature and its metadata are no longer active.
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);

  // Previously enabled Labels must return to the ordinary elision path. The
  // progress property remains stable, but no reveal metadata texture is built.
  label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  label.SetRequestedWidth(120.0f);
  label.SetText("A much longer ordinary text update after Reveal None");
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);

  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() >= 2u);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextRevealForegroundOnlyStyleResourcesP(void)
{
  UiTestApplication application;
  Label             label = Label::New("Foreground reveal leaves decoration\nresources intact");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(128.0f);
  label.SetMultiLine(true);

  Text::Shadow shadow;
  shadow.SetOffset(Vector2(2.0f, 2.0f));
  Text::Outline outline;
  outline.SetWidth(2.0f);
  Text::Underline   underline;
  Text::LineThrough lineThrough;
  label.SetTextShadow(shadow);
  label.SetTextOutline(outline);
  label.SetTextUnderline(underline);
  label.SetTextLineThrough(lineThrough);
  label.SetTextBackgroundColor(UiColor(Color::MAGENTA));
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::LINE);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.5f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.0f);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  Renderer   renderer = label.GetRendererAt(0u);
  TextureSet textures = renderer.GetTextures();
  DALI_TEST_CHECK(textures.GetTextureCount() >= 3u);
  std::vector<Texture> resources;
  resources.reserve(textures.GetTextureCount());
  for(uint32_t index = 0u; index < textures.GetTextureCount(); ++index)
  {
    resources.push_back(textures.GetTexture(index));
  }

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  gl.ResetTextureCallStack();
  for(uint32_t update = 0u; update < 1000u; ++update)
  {
    label.SetTextRevealProgress(static_cast<float>(update % 101u) * 0.01f);
  }
  for(float progress : {0.0f, 0.5f, 1.0f})
  {
    label.SetTextRevealProgress(progress);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
    const Property::Index rendererProgressIndex = renderer.GetPropertyIndex("uTextRevealProgress");
    DALI_TEST_CHECK(rendererProgressIndex != Property::INVALID_INDEX);
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(rendererProgressIndex), progress, 0.0001f, TEST_LOCATION);
    TextureSet current = label.GetRendererAt(0u).GetTextures();
    DALI_TEST_EQUALS(current.GetTextureCount(), resources.size(), TEST_LOCATION);
    for(uint32_t index = 0u; index < current.GetTextureCount(); ++index)
    {
      DALI_TEST_CHECK(current.GetTexture(index) == resources[index]);
    }
    DALI_TEST_CHECK(label.GetTextShadow() == shadow);
    DALI_TEST_CHECK(label.GetTextOutline() == outline);
    DALI_TEST_CHECK(label.GetTextUnderline() == underline);
    DALI_TEST_CHECK(label.GetTextLineThrough() == lineThrough);
    DALI_TEST_EQUALS(label.GetTextBackgroundColor().GetRgba(), Color::MAGENTA, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextRevealEndEllipsisIntegrationSmokeP(void)
{
  UiTestApplication application;

  for(Text::Reveal::Unit unit : {Text::Reveal::Unit::CHARACTER,
                                 Text::Reveal::Unit::WORD,
                                 Text::Reveal::Unit::LINE,
                                 Text::Reveal::Unit::PIXEL})
  {
    Label label = Label::New("Alpha Beta Gamma Delta Epsilon Zeta Eta Theta Iota Kappa Lambda");
    label.SetProperty(Actor::Property::SIZE, Vector3(72.0f, 36.0f, 0.0f));
    label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);

    Text::Reveal reveal;
    reveal.SetUnit(unit);
    label.SetTextReveal(reveal);
    label.SetTextRevealProgress(0.0f);

    application.GetScene().Add(label);
    application.SendNotification();
    application.Render(16);
    application.SendNotification();
    application.Render(16);

    // This public smoke covers Label END-ellipsis configuration together with
    // Reveal renderer and metadata integration. Final ellipsis identity and
    // timing correctness are verified by the internal Reveal UTC.
    DALI_TEST_EQUALS(label.GetTextOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    Renderer              renderer      = label.GetRendererAt(0u);
    const Property::Index progressIndex = renderer.GetPropertyIndex("uTextRevealProgress");
    DALI_TEST_CHECK(progressIndex != Property::INVALID_INDEX);
    DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealFadeDuration") != Property::INVALID_INDEX);

    TextureSet textures = renderer.GetTextures();
    DALI_TEST_CHECK(textures.GetTextureCount() >= 2u);
    Texture textTexture     = textures.GetTexture(0u);
    Texture metadataTexture = textures.GetTexture(textures.GetTextureCount() - 1u);
    DALI_TEST_CHECK(textTexture && metadataTexture);
    DALI_TEST_EQUALS(metadataTexture.GetWidth(), textTexture.GetWidth(), TEST_LOCATION);
    DALI_TEST_EQUALS(metadataTexture.GetHeight(), textTexture.GetHeight(), TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(progressIndex), 0.0f, 0.0001f, TEST_LOCATION);

    label.SetTextRevealProgress(1.0f);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(progressIndex), 1.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_CHECK(renderer.GetTextures().GetTexture(renderer.GetTextures().GetTextureCount() - 1u) == metadataTexture);

    label.Unparent();
  }
  END_TEST;
}

int UtcDaliLabelTextRevealRendererPropertyLifecycleP(void)
{
  UiTestApplication application;
  Label             label = Label::New("ABCDE");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(84.0f);

  Text::Reveal reveal;
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.37f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  Renderer              renderer           = label.GetRendererAt(0u);
  const Property::Index labelProgressIndex = label.GetPropertyIndex("uTextRevealProgress");
  const Property::Index progressIndex      = renderer.GetPropertyIndex("uTextRevealProgress");
  const Property::Index fadeIndex          = renderer.GetPropertyIndex("uTextRevealFadeDuration");
  const uint32_t        propertyCount      = renderer.GetPropertyCount();
  DALI_TEST_CHECK(labelProgressIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(progressIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(fadeIndex != Property::INVALID_INDEX);
  const float initialAutoFade = renderer.GetCurrentProperty<float>(fadeIndex);
  DALI_TEST_CHECK(initialAutoFade > 0.0f && initialAutoFade <= 1.0f);

  const float ratios[] = {
    0.0f,
    0.2f,
    1.0f,
    Text::Reveal::AUTO_FADE_DURATION_RATIO,
    0.05f,
    0.15f,
    0.3f,
    1.0f};
  for(uint32_t iteration = 0u; iteration < sizeof(ratios) / sizeof(ratios[0]); ++iteration)
  {
    reveal.SetFadeDurationRatio(ratios[iteration]);
    label.SetTextReveal(reveal);
    label.SetText((iteration & 1u) ? "VWXYZ" : "ABCDE");
    Text::Outline outline;
    outline.SetWidth((iteration & 2u) ? 2.0f : 1.0f);
    label.SetTextOutline(outline);
    application.SendNotification();
    application.Render(16);
    application.SendNotification();
    application.Render(16);

    DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
    DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), labelProgressIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextRevealFadeDuration"), fadeIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetPropertyCount(), propertyCount, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetTextReveal().GetFadeDurationRatio(), ratios[iteration], 0.0001f, TEST_LOCATION);
    const float fadeDuration = renderer.GetCurrentProperty<float>(fadeIndex);
    if(ratios[iteration] == Text::Reveal::AUTO_FADE_DURATION_RATIO)
    {
      DALI_TEST_CHECK(fadeDuration > 0.0f && fadeDuration <= 1.0f);
    }
    else
    {
      DALI_TEST_EQUALS(fadeDuration, ratios[iteration], 0.0001f, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.37f, 0.0001f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliLabelTextRevealProgressGetterCurrentP(void)
{
  UiTestApplication application;
  Label             label = Label::New("Current reveal progress");
  label.SetRequestedWidth(320.0f);
  label.SetRequestedHeight(80.0f);
  label.SetTextReveal(Text::Reveal());
  label.SetTextRevealProgress(0.0f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);

  const Property::Index progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  DALI_TEST_CHECK(progressIndex != Property::INVALID_INDEX);
  auto CheckRenderedProgress = [&]()
  {
    const float sceneValue = label.GetCurrentProperty<float>(progressIndex);
    const float expected   = std::isnan(sceneValue) ? 0.0f : std::max(0.0f, std::min(1.0f, sceneValue));
    DALI_TEST_EQUALS(label.GetTextRevealProgress(), expected, 0.0001f, TEST_LOCATION);
  };

  // Scheduling alone changes the event-side target, not the rendered value.
  Animation animation = Animation::New(1.0f);
  label.Animate(animation).TextRevealProgress(1.0f, Duration(1.0f));
  CheckRenderedProgress();
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.0f, 0.0001f, TEST_LOCATION);

  animation.Play();
  application.SendNotification();
  application.Render(250);
  CheckRenderedProgress();
  const float current = label.GetTextRevealProgress();
  DALI_TEST_CHECK(current > 0.0f && current < 1.0f);
  application.Render(800);

  // A direct setter while an animation is active must never make the public
  // getter diverge from the value used by the renderer.
  label.SetTextRevealProgress(0.0f);
  application.SendNotification();
  application.Render(16);
  Animation active = Animation::New(1.0f);
  label.Animate(active).TextRevealProgress(1.0f, Duration(1.0f));
  active.Play();
  application.SendNotification();
  application.Render(200);
  CheckRenderedProgress();
  DALI_TEST_CHECK(label.GetTextRevealProgress() > 0.0f && label.GetTextRevealProgress() < 1.0f);
  label.SetTextRevealProgress(0.25f);
  CheckRenderedProgress();
  application.SendNotification();
  application.Render(16);
  CheckRenderedProgress();
  application.Render(900);

  // Raw scene progress may overshoot, while the typed Reveal presentation is
  // clamped to its public [0, 1] range.
  label.SetProperty(progressIndex, 2.0f);
  application.SendNotification();
  application.Render(16);
  CheckRenderedProgress();
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(progressIndex), 2.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 1.0f, 0.0001f, TEST_LOCATION);

  label.SetProperty(progressIndex, -1.0f);
  application.SendNotification();
  application.Render(16);
  CheckRenderedProgress();
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(progressIndex), -1.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.0f, 0.0001f, TEST_LOCATION);

  // The generated Reveal bridge must forward true AnimateBy semantics instead
  // of converting the configured delta to an AnimateTo target.
  Label relativeLabel = Label::New("Reveal AnimateBy bridge");
  relativeLabel.SetRequestedWidth(320.0f);
  relativeLabel.SetRequestedHeight(80.0f);
  relativeLabel.SetTextReveal(Text::Reveal());
  relativeLabel.SetTextRevealProgress(0.2f);
  application.GetScene().Add(relativeLabel);
  application.SendNotification();
  application.Render(16);
  Animation configuredBeforeSet = Animation::New(0.1f);
  relativeLabel.Animate(configuredBeforeSet).TextRevealProgressBy(0.2f, Duration(0.1f));
  relativeLabel.SetTextRevealProgress(0.7f);
  configuredBeforeSet.Play();
  application.SendNotification();
  application.Render(120);
  const Property::Index relativeProgressIndex = relativeLabel.GetPropertyIndex("uTextRevealProgress");
  DALI_TEST_CHECK(relativeProgressIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(relativeLabel.GetCurrentProperty<float>(relativeProgressIndex), 0.9f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(relativeLabel.GetTextRevealProgress(), 0.9f, 0.0001f, TEST_LOCATION);

  // Animation targets use the same Reveal normalization as direct setters.
  Animation nanTarget = Animation::New(0.1f);
  relativeLabel.Animate(nanTarget).TextRevealProgress(std::numeric_limits<float>::quiet_NaN(), Duration(0.1f));
  nanTarget.Play();
  application.SendNotification();
  application.Render(120);
  DALI_TEST_EQUALS(relativeLabel.GetCurrentProperty<float>(relativeProgressIndex), 0.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(relativeLabel.GetTextRevealProgress(), 0.0f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextRevealAsyncP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  (void)fontClient;

  Label label = Label::New("Async CHARACTER reveal e\u0301 office العربية 😀");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  label.SetAsyncRendering(true);
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::CHARACTER);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.25f);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(WaitForValidTextTexture(application, label));

  DALI_TEST_CHECK(label.GetRendererCount() > 0u);
  Renderer renderer = label.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealFadeDuration") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(renderer.GetTextures().GetTextureCount() >= 2u);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.25f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextRevealAsyncRenderScaleP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  (void)fontClient;

  for(float renderScale : {1.25f, 1.5f})
  {
    Label label = Label::New("Async Reveal metadata follows render scale");
    label.SetRequestedWidth(420.0f);
    label.SetRequestedHeight(96.0f);
    label.SetAsyncRendering(true);
    label.SetRenderScale(renderScale);
    label.SetTextReveal(Text::Reveal());
    label.SetTextRevealProgress(0.4f);

    application.GetScene().Add(label);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_CHECK(WaitForValidTextTexture(application, label));

    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    Renderer renderer = label.GetRendererAt(0u);
    DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
    DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealFadeDuration") != Property::INVALID_INDEX);

    TextureSet textures = renderer.GetTextures();
    DALI_TEST_CHECK(textures.GetTextureCount() >= 2u);
    Texture textTexture     = textures.GetTexture(0u);
    Texture metadataTexture = textures.GetTexture(textures.GetTextureCount() - 1u);
    DALI_TEST_CHECK(textTexture && metadataTexture);
    DALI_TEST_CHECK(textTexture.GetWidth() > 0u && textTexture.GetHeight() > 0u);
    DALI_TEST_EQUALS(metadataTexture.GetWidth(), textTexture.GetWidth(), TEST_LOCATION);
    DALI_TEST_EQUALS(metadataTexture.GetHeight(), textTexture.GetHeight(), TEST_LOCATION);
    label.Unparent();
  }
  END_TEST;
}

int UtcDaliLabelTextRevealCutoutUnsupportedP(void)
{
  UiTestApplication application;
  Label             label = Label::New("Cutout retains authored reveal");
  label.SetProperty(Actor::Property::SIZE, Vector3(360.0f, 80.0f, 0.0f));
  Text::Reveal reveal;
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.5f);
  label.SetTextCutoutEnabled(true);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetPropertyIndex("uTextRevealProgress"),
                   Property::INVALID_INDEX, TEST_LOCATION);

  label.SetTextCutoutEnabled(false);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.5f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextRevealMarqueeUnsupportedP(void)
{
  UiTestApplication application;
  Label             label = Label::New(
    "A long marquee line keeps scrolling while its authored reveal configuration remains available.");
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(48.0f);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(0);

  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::WORD);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.35f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() >= 2u);

  label.StartMarquee();
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(label.IsMarqueeRunning());
  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.35f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);

  label.StopMarquee();
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(!label.IsMarqueeRunning());
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() >= 2u);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.35f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextRevealHeightTilingP(void)
{
  UiTestApplication application;
  const uint32_t    maxTextureSize = static_cast<uint32_t>(Dali::GetMaxTextureSize());
  DALI_TEST_CHECK(maxTextureSize > 0u);

  std::string    text;
  const uint32_t lineCount = maxTextureSize / 4u + 64u;
  text.reserve(lineCount * 2u);
  for(uint32_t line = 0u; line < lineCount; ++line)
  {
    text += "X\n";
  }

  Label label = Label::New(Dali::String(text.c_str()));
  label.SetMultiLine(true);
  label.SetFontSize(32.0f);
  label.SetRequestedWidth(96.0f);
  label.SetRequestedHeight(static_cast<float>(maxTextureSize + 64u));
  label.SetTextReveal(Text::Reveal());
  label.SetTextRevealProgress(0.5f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.GetRendererCount() >= 2u);
  for(uint32_t rendererIndex = 0u; rendererIndex < label.GetRendererCount(); ++rendererIndex)
  {
    Renderer renderer = label.GetRendererAt(rendererIndex);
    DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
    TextureSet textures = renderer.GetTextures();
    DALI_TEST_CHECK(textures.GetTextureCount() >= 2u);
    Texture metadata = textures.GetTexture(textures.GetTextureCount() - 1u);
    DALI_TEST_CHECK(metadata);
    DALI_TEST_CHECK(metadata.GetHeight() > 0u);
    DALI_TEST_CHECK(metadata.GetHeight() <= maxTextureSize);
  }
  END_TEST;
}

int UtcDaliLabelTextRevealAsyncHeightTilingP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  (void)fontClient;

  const uint32_t maxTextureSize = static_cast<uint32_t>(Dali::GetMaxTextureSize());
  std::string    text;
  const uint32_t lineCount = maxTextureSize / 4u + 64u;
  text.reserve(lineCount * 2u);
  for(uint32_t line = 0u; line < lineCount; ++line)
  {
    text += "Y\n";
  }

  Label label = Label::New(Dali::String(text.c_str()));
  label.SetMultiLine(true);
  label.SetFontSize(32.0f);
  label.SetRequestedWidth(96.0f);
  label.SetRequestedHeight(static_cast<float>(maxTextureSize + 64u));
  label.SetAsyncRendering(true);
  label.SetRenderScale(1.25f);
  label.SetTextReveal(Text::Reveal());
  label.SetTextRevealProgress(0.75f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(WaitForValidTextTexture(application, label));

  DALI_TEST_CHECK(label.GetRendererCount() >= 2u);
  for(uint32_t rendererIndex = 0u; rendererIndex < label.GetRendererCount(); ++rendererIndex)
  {
    Renderer renderer = label.GetRendererAt(rendererIndex);
    DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
    TextureSet textures = renderer.GetTextures();
    DALI_TEST_CHECK(textures.GetTextureCount() >= 2u);
    Texture textTexture = textures.GetTexture(0u);
    Texture metadata    = textures.GetTexture(textures.GetTextureCount() - 1u);
    DALI_TEST_CHECK(textTexture && metadata);
    DALI_TEST_EQUALS(metadata.GetWidth(), textTexture.GetWidth(), TEST_LOCATION);
    DALI_TEST_EQUALS(metadata.GetHeight(), textTexture.GetHeight(), TEST_LOCATION);
    DALI_TEST_CHECK(metadata.GetHeight() > 0u);
    DALI_TEST_CHECK(metadata.GetHeight() <= maxTextureSize);
  }
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.75f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextRevealAnimationConfigurationRatioP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  (void)fontClient;

  Label label = Label::New("Animation keeps running while CHARACTER changes to async WORD reveal");
  label.SetRequestedWidth(320.0f);
  label.SetRequestedHeight(80.0f);

  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::CHARACTER);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.0f);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);

  Animation animation = Animation::New(2.0f);
  label.Animate(animation).TextRevealProgress(1.0f, Duration(2.0f));
  animation.Play();
  application.SendNotification();
  application.Render(100);

  const Property::Index progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  DALI_TEST_CHECK(progressIndex != Property::INVALID_INDEX);

  label.SetAsyncRendering(true);
  reveal.SetUnit(Text::Reveal::Unit::WORD);
  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render(100);

  // Keep the original Animation target alive while repeatedly changing every
  // configuration dimension that may replace the renderer or its metadata.
  const float ratios[] = {
    Text::Reveal::AUTO_FADE_DURATION_RATIO,
    0.0f,
    0.05f,
    0.15f,
    0.3f,
    1.0f};
  for(uint32_t iteration = 0u; iteration < 8u; ++iteration)
  {
    reveal.SetUnit((iteration & 1u) ? Text::Reveal::Unit::CHARACTER : Text::Reveal::Unit::WORD);
    reveal.SetFadeDurationRatio(ratios[iteration % (sizeof(ratios) / sizeof(ratios[0]))]);
    if(iteration == 3u)
    {
      label.SetTextReveal(Text::Reveal::None());
    }
    else
    {
      label.SetTextReveal(reveal);
    }
    label.SetAsyncRendering((iteration & 1u) != 0u);
    label.SetText((iteration & 1u) ? "Reverse mixed אבג ratio text" : "Forward العربية ratio text");
    label.SetRequestedWidth(300.0f + static_cast<float>(iteration));
    application.SendNotification();
    application.Render(32);
    DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
    if(iteration != 3u)
    {
      DALI_TEST_EQUALS(label.GetTextReveal().GetFadeDurationRatio(),
                       ratios[iteration % (sizeof(ratios) / sizeof(ratios[0]))],
                       0.0001f,
                       TEST_LOCATION);
    }
  }

  reveal.SetUnit(Text::Reveal::Unit::WORD);
  label.SetTextReveal(reveal);
  label.SetAsyncRendering(true);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render(1800);

  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 1.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererCount() > 0u);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() >= 2u);

  Animation reverse = Animation::New(1.0f);
  label.Animate(reverse).TextRevealProgress(0.0f, Duration(1.0f));
  reverse.Play();
  application.SendNotification();
  application.Render(200);
  label.SetAsyncRendering(false);
  reveal.SetUnit(Text::Reveal::Unit::CHARACTER);
  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render(300);
  DALI_TEST_CHECK(label.GetTextRevealProgress() > 0.0f && label.GetTextRevealProgress() < 1.0f);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextRevealLifecycleStressP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  (void)Dali::TextAbstraction::FontClient::Get();

  const char* const texts[] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ repeated reveal lifecycle text",
    "office cafe\u0301 العربية אבג 😀 mixed-script reveal lifecycle text",
    "A long END ellipsis corpus that repeatedly changes layout while Reveal animation remains active"};
  const float ratios[] = {
    Text::Reveal::AUTO_FADE_DURATION_RATIO,
    0.0f,
    0.1f,
    0.25f,
    1.0f};
  const float progressValues[] = {0.1f, 0.8f, 0.0f, 1.0f};
  const Text::Reveal::Unit units[] = {
    Text::Reveal::Unit::CHARACTER,
    Text::Reveal::Unit::WORD,
    Text::Reveal::Unit::LINE};

  Label label = Label::New(texts[1]);
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(88.0f);
  label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);

  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::CHARACTER);
  reveal.SetFadeDurationRatio(Text::Reveal::AUTO_FADE_DURATION_RATIO);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.0f);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  const Property::Index progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  DALI_TEST_CHECK(progressIndex != Property::INVALID_INDEX);

  auto CheckCommonState = [&]()
  {
    DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
    const float progress = label.GetTextRevealProgress();
    DALI_TEST_CHECK(std::isfinite(progress));
    DALI_TEST_CHECK(progress >= 0.0f && progress <= 1.0f);
    const bool connected = label.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE);
    DALI_TEST_CHECK(!connected || label.GetRendererCount() > 0u);
  };

  auto HasRevealRenderer = [&]()
  {
    for(uint32_t rendererIndex = 0u; rendererIndex < label.GetRendererCount(); ++rendererIndex)
    {
      Renderer renderer = label.GetRendererAt(rendererIndex);
      if(renderer.GetPropertyIndex("uTextRevealProgress") == Property::INVALID_INDEX ||
         renderer.GetPropertyIndex("uTextRevealFadeDuration") == Property::INVALID_INDEX)
      {
        continue;
      }

      TextureSet textures = renderer.GetTextures();
      if(textures && textures.GetTextureCount() >= 2u)
      {
        Texture metadata = textures.GetTexture(textures.GetTextureCount() - 1u);
        if(metadata && metadata.GetWidth() > 0u && metadata.GetHeight() > 0u)
        {
          return true;
        }
      }
    }
    return false;
  };

  // Checkpoint A: the initial supported synchronous configuration owns a
  // Reveal renderer and metadata texture.
  CheckCommonState();
  DALI_TEST_CHECK(HasRevealRenderer());

  Animation animation = Animation::New(4.0f);
  label.Animate(animation).TextRevealProgress(1.0f, Duration(4.0f));
  animation.Play();
  application.SendNotification();
  application.Render(160);
  CheckCommonState();
  DALI_TEST_CHECK(label.GetTextRevealProgress() > 0.0f && label.GetTextRevealProgress() < 1.0f);

  bool      revealEnabled = true;
  bool      cutoutEnabled = false;
  Animation secondAnimation;
  for(uint32_t iteration = 0u; iteration < 24u; ++iteration)
  {
    if(iteration % 4u == 0u)
    {
      label.SetTextRevealProgress(progressValues[(iteration / 4u) % 4u]);
    }
    if(iteration % 5u == 0u)
    {
      reveal.SetFadeDurationRatio(ratios[(iteration / 5u) % (sizeof(ratios) / sizeof(ratios[0]))]);
      if(revealEnabled)
      {
        label.SetTextReveal(reveal);
      }
    }

    switch(iteration % 8u)
    {
      case 0u:
      {
        label.SetTextReveal(Text::Reveal::None());
        revealEnabled = false;
        break;
      }
      case 1u:
      {
        reveal.SetUnit(units[(iteration / 8u) % (sizeof(units) / sizeof(units[0]))]);
        label.SetTextReveal(reveal);
        revealEnabled = true;
        break;
      }
      case 2u:
      {
        label.SetAsyncRendering(true);
        label.SetText(texts[(iteration / 8u + 1u) % (sizeof(texts) / sizeof(texts[0]))]);
        break;
      }
      case 3u:
      {
        label.SetAsyncRendering(false);
        reveal.SetUnit(units[(iteration / 8u + 1u) % (sizeof(units) / sizeof(units[0]))]);
        label.SetTextReveal(reveal);
        revealEnabled = true;
        break;
      }
      case 4u:
      {
        label.SetText(texts[(iteration / 4u) % (sizeof(texts) / sizeof(texts[0]))]);
        break;
      }
      case 5u:
      {
        label.SetRequestedWidth((iteration & 8u) ? 150.0f : 420.0f);
        label.SetRequestedHeight((iteration & 8u) ? 72.0f : 96.0f);
        break;
      }
      case 6u:
      {
        label.SetTextColor(UiColor((iteration & 8u) ? Color::BLUE : Color::RED));
        if(iteration & 8u)
        {
          label.SetTextOutline(Text::Outline::None());
        }
        else
        {
          Text::Outline outline;
          outline.SetWidth((iteration & 16u) ? 2.0f : 1.0f);
          label.SetTextOutline(outline);
        }
        break;
      }
      case 7u:
      {
        cutoutEnabled = !cutoutEnabled;
        label.SetTextCutoutEnabled(cutoutEnabled);
        break;
      }
    }

    application.SendNotification();
    application.Render(24u + (iteration % 3u) * 8u);
    CheckCommonState();
    DALI_TEST_CHECK(revealEnabled ? label.GetTextReveal() == reveal
                                  : label.GetTextReveal() == Text::Reveal::None());

    if(iteration == 0u)
    {
      // Checkpoint B: disabling Reveal keeps the source property stable.
      DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
    }
    else if(iteration == 11u)
    {
      animation.Stop();
      application.SendNotification();
      application.Render(16);
      CheckCommonState();

      secondAnimation = Animation::New(2.0f);
      label.Animate(secondAnimation).TextRevealProgress(0.6f, Duration(2.0f));
      secondAnimation.Play();
    }
    else if(iteration == 15u)
    {
      const Text::Reveal authoredReveal = label.GetTextReveal();
      label.Unparent();
      application.SendNotification();
      application.Render(16);
      CheckCommonState();
      DALI_TEST_CHECK(label.GetTextReveal() == authoredReveal);

      application.GetScene().Add(label);
      application.SendNotification();
      application.Render(16);
      CheckCommonState();
      DALI_TEST_CHECK(label.GetTextReveal() == authoredReveal);
    }
  }

  // A pending asynchronous update must not restore an authored Reveal that
  // was disabled before the result was published.
  label.SetAsyncRendering(true);
  label.SetTextReveal(reveal);
  label.SetText(texts[2]);
  application.SendNotification();
  application.Render(16);
  label.SetTextReveal(Text::Reveal::None());
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render(32);
  CheckCommonState();
  DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);

  // Checkpoint C: async publication can still converge to a supported Reveal
  // renderer after the high-churn phase.
  label.SetTextCutoutEnabled(false);
  label.SetTextOutline(Text::Outline::None());
  label.SetAsyncRendering(true);
  reveal.SetUnit(Text::Reveal::Unit::WORD);
  reveal.SetFadeDurationRatio(0.1f);
  label.SetTextReveal(reveal);
  label.SetText(texts[1]);
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(88.0f);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render(32);
  CheckCommonState();
  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  DALI_TEST_CHECK(HasRevealRenderer());

  for(uint32_t frame = 0u; frame < 3u; ++frame)
  {
    application.SendNotification();
    application.Render(32);
    CheckCommonState();
    DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  }

  if(secondAnimation)
  {
    secondAnimation.Stop();
  }
  application.SendNotification();
  application.Render(16);

  // Checkpoint D: return to a deterministic synchronous steady state and
  // verify the final authored configuration, source property and renderer.
  label.SetAsyncRendering(false);
  label.SetTextCutoutEnabled(false);
  reveal.SetUnit(Text::Reveal::Unit::CHARACTER);
  reveal.SetFadeDurationRatio(0.25f);
  label.SetTextReveal(reveal);
  label.SetText("Final normalized Reveal lifecycle state");
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(88.0f);
  label.SetTextRevealProgress(0.4f);
  application.SendNotification();
  application.Render(32);
  application.SendNotification();
  application.Render(32);

  CheckCommonState();
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  DALI_TEST_CHECK(HasRevealRenderer());
  END_TEST;
}

int UtcDaliLabelAsyncSameSizePaddingChangePublishesResourceP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  Label label = Label::New(
    "Asynchronous padding changes must republish text when fixed outer geometry keeps the Label size unchanged");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(240.0f);
  label.SetFontSize(20.0f);
  label.SetMultiLine(true);
  label.SetAsyncRendering(true);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(WaitForValidTextTexture(application, label));

  const Vector3  fixedSize           = label.GetCurrentSize();
  const uint32_t initialTextureWidth = label.GetRendererAt(0u).GetTextures().GetTexture(0u).GetWidth();

  label.SetPadding(Insets(100.0f, 110.0f, 7.0f, 11.0f));
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(label.GetCurrentSize(), fixedSize, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() > 0u);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTexture(0u).GetWidth() < initialTextureWidth);

  END_TEST;
}

int UtcDaliAnimationSpecDownCastP(void)
{
  UiTestApplication application;

  ViewAnimationSpec viewSpec = View::NewAnimationSpec();
  BaseHandle        viewHandle(viewSpec);
  ViewAnimationSpec viewDownCast = ViewAnimationSpec::DownCast(viewHandle);
  DALI_TEST_CHECK(viewDownCast);

  LabelAnimationSpec labelSpec = Label::NewAnimationSpec();
  BaseHandle         labelHandle(labelSpec);
  LabelAnimationSpec labelDownCast = LabelAnimationSpec::DownCast(labelHandle);
  DALI_TEST_CHECK(labelDownCast);

  LabelAnimationSpec invalidLabelDownCast = LabelAnimationSpec::DownCast(viewHandle);
  DALI_TEST_CHECK(!invalidLabelDownCast);

  END_TEST;
}

int UtcDaliLabelPixelSnapFactorAnimationOnDemandP(void)
{
  UiTestApplication application;

  Label setterLabel = Label::New();
  application.GetScene().Add(setterLabel);

  // PixelSnapFactor is registered on demand, not as a default Label property.
  DALI_TEST_EQUALS(setterLabel.GetPropertyIndex(PROPERTY_NAME_PIXEL_SNAP_FACTOR), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(setterLabel.GetPixelSnapFactor(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  setterLabel.SetPixelSnapFactor(0.75f);
  const Property::Index setterPixelSnapFactorIndex = setterLabel.GetPropertyIndex(PROPERTY_NAME_PIXEL_SNAP_FACTOR);
  DALI_TEST_CHECK(setterPixelSnapFactorIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(setterLabel.GetPixelSnapFactor(), 0.75f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  setterLabel.SetPixelSnapFactor(0.0f);
  DALI_TEST_EQUALS(setterLabel.GetPropertyIndex(PROPERTY_NAME_PIXEL_SNAP_FACTOR), setterPixelSnapFactorIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(setterLabel.GetPixelSnapFactor(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Label label = Label::New();
  application.GetScene().Add(label);

  // Animation also registers the backing property on demand.
  DALI_TEST_EQUALS(label.GetPropertyIndex(PROPERTY_NAME_PIXEL_SNAP_FACTOR), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPixelSnapFactor(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Animation bridgeAnimation = Animation::New(0.0f);
  label.Animate(bridgeAnimation)
    .PixelSnapFactor(1.0f, Duration(0.1f));

  const Property::Index pixelSnapFactorIndex = label.GetPropertyIndex(PROPERTY_NAME_PIXEL_SNAP_FACTOR);
  DALI_TEST_CHECK(pixelSnapFactorIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(label.GetProperty<float>(pixelSnapFactorIndex), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(bridgeAnimation.GetDuration(), 0.1f, TEST_LOCATION);

  bridgeAnimation.Play();
  application.SendNotification();
  application.Render(0);
  application.Render(100);
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(pixelSnapFactorIndex), 1.0f, 0.01f, TEST_LOCATION);

  Animation byAnimation = Animation::New(0.0f);
  label.Animate(byAnimation)
    .PixelSnapFactorBy(-0.5f, Duration(0.1f));
  DALI_TEST_EQUALS(label.GetPropertyIndex(PROPERTY_NAME_PIXEL_SNAP_FACTOR), pixelSnapFactorIndex, TEST_LOCATION);

  byAnimation.Play();
  application.SendNotification();
  application.Render(0);
  application.Render(100);
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(pixelSnapFactorIndex), 0.5f, 0.01f, TEST_LOCATION);

  LabelAnimationSpec spec = Label::NewAnimationSpec();
  spec.PixelSnapFactor(0.25f, Duration(0.1f));

  Animation specAnimation = Animation::New(0.0f);
  spec.ApplyTo(specAnimation, label);
  DALI_TEST_EQUALS(label.GetPropertyIndex(PROPERTY_NAME_PIXEL_SNAP_FACTOR), pixelSnapFactorIndex, TEST_LOCATION);

  specAnimation.Play();
  application.SendNotification();
  application.Render(0);
  application.Render(100);
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(pixelSnapFactorIndex), 0.25f, 0.01f, TEST_LOCATION);

  END_TEST;
}

// Setter, Getter

int UtcDaliLabelText(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetText("Hello world");
  DALI_TEST_EQUALS(label.GetText(), std::string("Hello world"), TEST_LOCATION);

  label.SetText("Updated text");
  DALI_TEST_EQUALS(label.GetText(), std::string("Updated text"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelSetStyledTextPlainTextReflectionP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  Text::StyledText styledText = Text::StyledText::New("Styled plain text");
  label.SetStyledText(styledText);

  DALI_TEST_EQUALS(label.GetText(), "Styled plain text", TEST_LOCATION);
  DALI_TEST_CHECK(label.GetStyledText());
  DALI_TEST_EQUALS(label.GetStyledText().GetText(), "Styled plain text", TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelSetStyledTextBackgroundColorSpanReflectionP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  Text::StyledTextBuilder   builder = Text::StyledTextBuilder::New("Styled background");
  Text::BackgroundColorSpan span    = Text::BackgroundColorSpan::New(UiColor(Color::CYAN));
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 6u));

  label.SetStyledText(builder.Build());

  Text::StyledText styledText = label.GetStyledText();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(label.GetText(), "Styled background", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::BackgroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor().GetRgba(), Color::CYAN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelSetStyledTextUnderlineSpanReflectionP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  Text::Underline underline;
  underline.SetColor(UiColor(Color::GREEN));
  underline.SetThickness(2.0f);
  underline.SetType(Text::Underline::Type::DASHED);
  underline.SetDashLength(3.0f);
  underline.SetDashGap(1.5f);

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("Styled underline");
  Text::UnderlineSpan     span    = Text::UnderlineSpan::New(underline);
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 6u));

  label.SetStyledText(builder.Build());

  Text::StyledText styledText = label.GetStyledText();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(label.GetText(), "Styled underline", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_CHECK(Text::UnderlineSpan::DownCast(styledText.GetSpanAt(0u)).GetUnderline() == underline);

  END_TEST;
}

int UtcDaliLabelSetStyledTextLineThroughSpanReflectionP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  Text::LineThrough lineThrough;
  lineThrough.SetColor(UiColor(Color::RED));
  lineThrough.SetThickness(2.0f);

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("Styled line-through");
  Text::LineThroughSpan   span    = Text::LineThroughSpan::New(lineThrough);
  DALI_TEST_CHECK(builder.SetSpan(span, 7u, 19u));

  label.SetStyledText(builder.Build());

  Text::StyledText styledText = label.GetStyledText();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(label.GetText(), "Styled line-through", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(0u), 7u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(0u), 19u, TEST_LOCATION);
  DALI_TEST_CHECK(Text::LineThroughSpan::DownCast(styledText.GetSpanAt(0u)).GetLineThrough() == lineThrough);

  END_TEST;
}

int UtcDaliLabelSetStyledTextFontSpanReflectionP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  Text::FontAttributes attributes;
  attributes.SetFamily("");
  attributes.SetSize(32.0f);
  attributes.SetWeight(Text::FontWeight::BOLD);
  attributes.SetWidth(Text::FontWidth::CONDENSED);
  attributes.SetSlant(Text::FontSlant::ITALIC);

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("Styled font span");
  Text::FontSpan          span    = Text::FontSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 7u, 11u));

  label.SetStyledText(builder.Build());

  Text::StyledText styledText = label.GetStyledText();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(label.GetText(), "Styled font span", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(0u), 7u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(0u), 11u, TEST_LOCATION);

  Text::FontAttributes roundTripAttributes = Text::FontSpan::DownCast(styledText.GetSpanAt(0u)).GetFontAttributes();
  DALI_TEST_CHECK(roundTripAttributes == attributes);
  DALI_TEST_CHECK(roundTripAttributes.Has(Text::FontAttributes::Attribute::FAMILY));
  DALI_TEST_CHECK(roundTripAttributes.Has(Text::FontAttributes::Attribute::SIZE));
  DALI_TEST_CHECK(roundTripAttributes.Has(Text::FontAttributes::Attribute::WEIGHT));
  DALI_TEST_CHECK(roundTripAttributes.Has(Text::FontAttributes::Attribute::WIDTH));
  DALI_TEST_CHECK(roundTripAttributes.Has(Text::FontAttributes::Attribute::SLANT));

  END_TEST;
}

int UtcDaliLabelSetTextClearsStyledTextSourceP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetStyledText(Text::StyledText::New("Styled source"));
  DALI_TEST_CHECK(label.GetStyledText());

  label.SetText("Plain source");

  DALI_TEST_EQUALS(label.GetText(), "Plain source", TEST_LOCATION);
  DALI_TEST_CHECK(!label.GetStyledText());

  END_TEST;
}

int UtcDaliLabelStyledTextLiteralMarkupP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetStyledText(Text::StyledText::New("<color value='red'>ABC</color>"));

  DALI_TEST_EQUALS(label.GetText(), "<color value='red'>ABC</color>", TEST_LOCATION);
  DALI_TEST_CHECK(label.GetStyledText());
  DALI_TEST_EQUALS(label.GetStyledText().GetText(), "<color value='red'>ABC</color>", TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelAsyncStyledTextLiteralMarkupP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Ensure FontClient is initialized before async text work starts.
  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  (void)fontClient;

  Label label = Label::New();
  DALI_TEST_CHECK(label);
  application.GetScene().Add(label);

  gAsyncNaturalSizeComputed = false;
  gAsyncNaturalSizeWidth    = 0.0f;
  gAsyncNaturalSizeHeight   = 0.0f;

  label.SetFontSize(16.0f);
  label.SetStyledText(Text::StyledText::New("<b>Hi</b>"));
  label.AsyncNaturalSizeComputedSignal().Connect(&OnAsyncNaturalSizeComputed);

  application.SendNotification();
  application.Render();

  label.SetAsyncRendering(true);
  label.RequestAsyncNaturalSize();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  label.SetAsyncRendering(false);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(label.GetText(), "<b>Hi</b>", TEST_LOCATION);
  DALI_TEST_CHECK(label.GetStyledText());
  DALI_TEST_EQUALS(label.GetStyledText().GetText(), "<b>Hi</b>", TEST_LOCATION);
  DALI_TEST_CHECK(gAsyncNaturalSizeComputed);
  DALI_TEST_CHECK(gAsyncNaturalSizeWidth > 0.0f);
  DALI_TEST_CHECK(gAsyncNaturalSizeHeight > 0.0f);

  END_TEST;
}

int UtcDaliLabelSetStyledTextEmptyHandleClearsSourceP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetStyledText(Text::StyledText::New("Styled source"));
  DALI_TEST_CHECK(label.GetStyledText());

  label.SetStyledText(Text::StyledText());

  DALI_TEST_EQUALS(label.GetText(), "", TEST_LOCATION);
  DALI_TEST_CHECK(!label.GetStyledText());

  END_TEST;
}

int UtcDaliLabelFontFamily(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetFontFamily("Arial");
  DALI_TEST_EQUALS(label.GetFontFamily(), std::string("Arial"), TEST_LOCATION);

  label.SetFontFamily("Roboto");
  DALI_TEST_EQUALS(label.GetFontFamily(), std::string("Roboto"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelFontSize(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetFontSize(20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(label.GetFontSize(), 20.0f, TEST_LOCATION);

  label.SetFontSize(32.5f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(label.GetFontSize(), 32.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMultiLine(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMultiLine(true);
  DALI_TEST_EQUALS(label.IsMultiLine(), true, TEST_LOCATION);

  label.SetMultiLine(false);
  DALI_TEST_EQUALS(label.IsMultiLine(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelLineWrapMode(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetLineWrapMode(Text::LineWrapMode::WORD);
  DALI_TEST_EQUALS(label.GetLineWrapMode(), Text::LineWrapMode::WORD, TEST_LOCATION);

  label.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  DALI_TEST_EQUALS(label.GetLineWrapMode(), Text::LineWrapMode::CHARACTER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelHorizontalTextAlignment(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(label.GetHorizontalTextAlignment(), Text::Alignment::CENTER, TEST_LOCATION);

  label.SetHorizontalTextAlignment(Text::Alignment::END);
  DALI_TEST_EQUALS(label.GetHorizontalTextAlignment(), Text::Alignment::END, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelVerticalTextAlignment(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(label.GetVerticalTextAlignment(), Text::Alignment::CENTER, TEST_LOCATION);

  label.SetVerticalTextAlignment(Text::Alignment::END);
  DALI_TEST_EQUALS(label.GetVerticalTextAlignment(), Text::Alignment::END, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelOverflowMode(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetTextOverflowMode(Text::OverflowMode::CLIP);
  DALI_TEST_EQUALS(label.GetTextOverflowMode(), Text::OverflowMode::CLIP, TEST_LOCATION);

  label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(label.GetTextOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelLineHeight(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetLineHeight(1.5f);
  DALI_TEST_EQUALS(label.GetLineHeight(), 1.5f, TEST_LOCATION);

  label.SetLineHeight(2.0f);
  DALI_TEST_EQUALS(label.GetLineHeight(), 2.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelLineHeightMode(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
  DALI_TEST_EQUALS(label.GetLineHeightMode(), Text::LineHeightMode::ABSOLUTE, TEST_LOCATION);

  label.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
  DALI_TEST_EQUALS(label.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelLayoutDirectionMode(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  DALI_TEST_EQUALS(label.GetLayoutDirectionMode(), Text::LayoutDirectionMode::LOCALE, TEST_LOCATION);

  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  DALI_TEST_EQUALS(label.GetLayoutDirectionMode(), Text::LayoutDirectionMode::CONTENTS, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelAnchorColor(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  UiColor color(Color::BLUE);
  label.SetAnchorColor(color);
  DALI_TEST_EQUALS(label.GetAnchorColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  label.SetAnchorColor(color2);
  DALI_TEST_EQUALS(label.GetAnchorColor().GetRgba(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelAnchorClickedColor(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  UiColor color(Color::GREEN);
  label.SetAnchorClickedColor(color);
  DALI_TEST_EQUALS(label.GetAnchorClickedColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  UiColor color2(Color::YELLOW);
  label.SetAnchorClickedColor(color2);
  DALI_TEST_EQUALS(label.GetAnchorClickedColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelAnchorAccessibilitySignalsP(void)
{
  UiTestApplication application;

  auto& enabledSignal  = Dali::Integration::Accessibility::Bridge::EnabledSignal();
  auto& disabledSignal = Dali::Integration::Accessibility::Bridge::DisabledSignal();

  const std::size_t enabledCount  = enabledSignal.GetConnectionCount();
  const std::size_t disabledCount = disabledSignal.GetConnectionCount();

  {
    Label label = Label::New();
    DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount, TEST_LOCATION);
    DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount, TEST_LOCATION);
    DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 0u, TEST_LOCATION);

    enabledSignal.Emit();
    disabledSignal.Emit();

    const Text::StyledText anchorText = Text::StyledText::FromMarkup("<a href='docs'>link</a>");
    label.SetStyledText(anchorText);
    DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 1u, TEST_LOCATION);

    label.SetStyledText(anchorText);
    DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 1u, TEST_LOCATION);
    enabledSignal.Emit();
    disabledSignal.Emit();

    label.SetText("plain");
    DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount, TEST_LOCATION);
    DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount, TEST_LOCATION);
    DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 0u, TEST_LOCATION);
    enabledSignal.Emit();
    disabledSignal.Emit();

    label.SetAsyncRendering(true);
    label.SetStyledText(anchorText);
    DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 1u, TEST_LOCATION);

    label.SetText("plain");
    DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount, TEST_LOCATION);
    DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount, TEST_LOCATION);
    DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 0u, TEST_LOCATION);

    label.SetStyledText(anchorText);
    DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 1u, TEST_LOCATION);

    label.SetAsyncRendering(false);
    DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 1u, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(enabledSignal.GetConnectionCount(), enabledCount, TEST_LOCATION);
  DALI_TEST_EQUALS(disabledSignal.GetConnectionCount(), disabledCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeTriggerPolicy(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
  DALI_TEST_EQUALS(label.GetMarqueeTriggerPolicy(), Text::MarqueeTriggerPolicy::ON_OVERFLOW, TEST_LOCATION);

  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  DALI_TEST_EQUALS(label.GetMarqueeTriggerPolicy(), Text::MarqueeTriggerPolicy::MANUAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeSpeed(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeSpeed(100);
  DALI_TEST_EQUALS(label.GetMarqueeSpeed(), 100, TEST_LOCATION);

  label.SetMarqueeSpeed(200);
  DALI_TEST_EQUALS(label.GetMarqueeSpeed(), 200, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeLoopCount(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeLoopCount(3);
  DALI_TEST_EQUALS(label.GetMarqueeLoopCount(), 3, TEST_LOCATION);

  label.SetMarqueeLoopCount(5);
  DALI_TEST_EQUALS(label.GetMarqueeLoopCount(), 5, TEST_LOCATION);

  label.SetMarqueeLoopCount(Text::MARQUEE_LOOP_COUNT_INFINITE);
  DALI_TEST_EQUALS(label.GetMarqueeLoopCount(), Text::MARQUEE_LOOP_COUNT_INFINITE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeLoopDelay(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeLoopDelay(1.5f);
  DALI_TEST_EQUALS(label.GetMarqueeLoopDelay(), 1.5f, TEST_LOCATION);

  label.SetMarqueeLoopDelay(2.0f);
  DALI_TEST_EQUALS(label.GetMarqueeLoopDelay(), 2.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeGap(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeGap(50);
  DALI_TEST_EQUALS(label.GetMarqueeGap(), 50, TEST_LOCATION);

  label.SetMarqueeGap(100);
  DALI_TEST_EQUALS(label.GetMarqueeGap(), 100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeStopMode(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeStopMode(Text::MarqueeStopMode::FINISH_LOOP);
  DALI_TEST_EQUALS(label.GetMarqueeStopMode(), Text::MarqueeStopMode::FINISH_LOOP, TEST_LOCATION);

  label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  DALI_TEST_EQUALS(label.GetMarqueeStopMode(), Text::MarqueeStopMode::IMMEDIATE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeOrientation(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeOrientation(Text::MarqueeOrientation::VERTICAL);
  DALI_TEST_EQUALS(label.GetMarqueeOrientation(), Text::MarqueeOrientation::VERTICAL, TEST_LOCATION);

  label.SetMarqueeOrientation(Text::MarqueeOrientation::HORIZONTAL);
  DALI_TEST_EQUALS(label.GetMarqueeOrientation(), Text::MarqueeOrientation::HORIZONTAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelSyncMarqueeRestartsAfterUiScaleChangeP(void)
{
  UiTestApplication application;
  UiScaleManager::Get().SetScale(1.0f);

  Label label = Label::New("This is a long single-line marquee text that should keep scrolling after UI scale changes.");
  label.SetAsyncRendering(false);
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(40.0f);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(0);
  label.SetMarqueeStopMode(Text::MarqueeStopMode::FINISH_LOOP);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();

  label.StartMarquee();
  application.SendNotification();
  application.Render();
  application.Render(16);

  DALI_TEST_CHECK(label.IsMarqueeRunning());

  UiScaleManager::Get().SetScale(1.2f);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.IsMarqueeRunning());

  UiScaleManager::Get().SetScale(1.0f);
  END_TEST;
}

int UtcDaliLabelSyncMarqueeDoesNotRestartAfterNaturalFinishOnResizeP(void)
{
  UiTestApplication application;

  Label label = Label::New("This is a long single-line marquee text that should finish once and stay stopped after resize.");
  label.SetAsyncRendering(false);
  label.SetRequestedWidth(40.0f);
  label.SetRequestedHeight(40.0f);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(1);
  label.SetMarqueeLoopDelay(0.0f);
  label.SetMarqueeSpeed(1000);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();

  label.StartMarquee();
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.IsMarqueeRunning());

  application.Render(1000);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(!label.IsMarqueeRunning());

  label.SetRequestedWidth(60.0f);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(!label.IsMarqueeRunning());

  END_TEST;
}

int UtcDaliLabelTextColor(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  UiColor color(Color::BLUE);
  label.SetTextColor(color);
  DALI_TEST_EQUALS(label.GetTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  label.SetTextColor(color2);
  DALI_TEST_EQUALS(label.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextStyleNoneP(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  DALI_TEST_CHECK(label.GetTextUnderline() == Text::Underline::None());
  Text::Underline underline;
  label.SetTextUnderline(underline);
  DALI_TEST_CHECK(label.GetTextUnderline() != Text::Underline::None());
  label.SetTextUnderline(Text::Underline::None());
  DALI_TEST_CHECK(label.GetTextUnderline() == Text::Underline::None());

  DALI_TEST_CHECK(label.GetTextShadow() == Text::Shadow::None());
  Text::Shadow zeroOffsetShadow;
  zeroOffsetShadow.SetOffset(Vector2::ZERO);
  label.SetTextShadow(zeroOffsetShadow);
  DALI_TEST_CHECK(label.GetTextShadow() != Text::Shadow::None());
  label.SetTextShadow(Text::Shadow::None());
  DALI_TEST_CHECK(label.GetTextShadow() == Text::Shadow::None());

  DALI_TEST_CHECK(label.GetTextOutline() == Text::Outline::None());
  Text::Outline zeroWidthOutline;
  zeroWidthOutline.SetWidth(0.0f);
  label.SetTextOutline(zeroWidthOutline);
  DALI_TEST_CHECK(label.GetTextOutline() != Text::Outline::None());
  label.SetTextOutline(Text::Outline::None());
  DALI_TEST_CHECK(label.GetTextOutline() == Text::Outline::None());

  DALI_TEST_CHECK(label.GetTextLineThrough() == Text::LineThrough::None());
  Text::LineThrough lineThrough;
  label.SetTextLineThrough(lineThrough);
  DALI_TEST_CHECK(label.GetTextLineThrough() != Text::LineThrough::None());
  label.SetTextLineThrough(Text::LineThrough::None());
  DALI_TEST_CHECK(label.GetTextLineThrough() == Text::LineThrough::None());

  DALI_TEST_CHECK(label.GetTextBevel() == Text::Bevel::None());
  Text::Bevel bevel;
  label.SetTextBevel(bevel);
  DALI_TEST_CHECK(label.GetTextBevel() != Text::Bevel::None());
  label.SetTextBevel(Text::Bevel::None());
  DALI_TEST_CHECK(label.GetTextBevel() == Text::Bevel::None());

  END_TEST;
}

int UtcDaliLabelTextBevelSyncAsyncP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  const Vector2 direction(-0.75f, 0.25f);
  const float   strength = 2.5f;
  const Vector4 lightColor(0.8f, 0.7f, 0.6f, 0.5f);
  const Vector4 shadowColor(0.1f, 0.2f, 0.3f, 0.4f);

  Text::Bevel bevel;
  bevel.SetDirection(direction);
  bevel.SetIntensity(strength);
  bevel.SetLightColor(UiColor(lightColor));
  bevel.SetShadowColor(UiColor(shadowColor));

  Label label = Label::New("Emboss state must survive synchronous and asynchronous rendering");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  label.SetTextBevel(bevel);
  DALI_TEST_CHECK(label.GetTextBevel() == bevel);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(HasValidTextTexture(label));
  CheckEmbossRendererProperties(label, direction, strength, lightColor, shadowColor);

  label.AsyncRenderFinishedSignal().Connect(&OnAsyncRenderFinished);
  label.SetAsyncRendering(true);
  gAsyncRenderFinished = false;
  label.SetText("Emboss values are copied into each asynchronous request");
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForAsyncRender(application));
  DALI_TEST_CHECK(label.GetTextBevel() == bevel);
  CheckEmbossRendererProperties(label, direction, strength, lightColor, shadowColor);

  END_TEST;
}

int UtcDaliLabelFontWeight(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetFontWeight(Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(label.GetFontWeight(), Text::FontWeight::BOLD, TEST_LOCATION);

  label.SetFontWeight(Text::FontWeight::LIGHT);
  DALI_TEST_EQUALS(label.GetFontWeight(), Text::FontWeight::LIGHT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelFontWidth(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetFontWidth(Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(label.GetFontWidth(), Text::FontWidth::EXPANDED, TEST_LOCATION);

  label.SetFontWidth(Text::FontWidth::CONDENSED);
  DALI_TEST_EQUALS(label.GetFontWidth(), Text::FontWidth::CONDENSED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelFontSlant(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetFontSlant(Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(label.GetFontSlant(), Text::FontSlant::ITALIC, TEST_LOCATION);

  label.SetFontSlant(Text::FontSlant::OBLIQUE);
  DALI_TEST_EQUALS(label.GetFontSlant(), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextBackgroundColor(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  UiColor color(Color::YELLOW);
  label.SetTextBackgroundColor(color);
  DALI_TEST_EQUALS(label.GetTextBackgroundColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  UiColor color2(Color::GREEN);
  label.SetTextBackgroundColor(color2);
  DALI_TEST_EQUALS(label.GetTextBackgroundColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  // Clear text background color
  label.ClearTextBackgroundColor();
  DALI_TEST_EQUALS(label.GetTextBackgroundColor().GetRgba(), Color::TRANSPARENT, TEST_LOCATION);

  // Set again after clear
  label.SetTextBackgroundColor(Color::BLUE);
  DALI_TEST_EQUALS(label.GetTextBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMinimumFontSizeScale(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMinimumFontSizeScale(0.5f);
  DALI_TEST_EQUALS(label.GetMinimumFontSizeScale(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  label.SetMinimumFontSizeScale(0.8f);
  DALI_TEST_EQUALS(label.GetMinimumFontSizeScale(), 0.8f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMaximumFontSizeScale(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMaximumFontSizeScale(2.0f);
  DALI_TEST_EQUALS(label.GetMaximumFontSizeScale(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  label.SetMaximumFontSizeScale(1.5f);
  DALI_TEST_EQUALS(label.GetMaximumFontSizeScale(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelSystemFontSizeScaleEnabled(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetSystemFontSizeScaleEnabled(true);
  DALI_TEST_EQUALS(label.IsSystemFontSizeScaleEnabled(), true, TEST_LOCATION);

  label.SetSystemFontSizeScaleEnabled(false);
  DALI_TEST_EQUALS(label.IsSystemFontSizeScaleEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelAdjustedFontSizeScale(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  // Test clamping to minimum from default scale 1.0
  label.SetMinimumFontSizeScale(1.2f);
  label.SetMaximumFontSizeScale(2.0f);
  label.SetSystemFontSizeScaleEnabled(false);
  DALI_TEST_EQUALS(label.GetAdjustedFontSizeScale(), 1.2f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test clamping to maximum from default scale 1.0
  label.SetMinimumFontSizeScale(0.1f);
  label.SetMaximumFontSizeScale(0.8f);
  DALI_TEST_EQUALS(label.GetAdjustedFontSizeScale(), 0.8f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test normal range with default scale 1.0
  label.SetMinimumFontSizeScale(0.5f);
  label.SetMaximumFontSizeScale(2.0f);
  DALI_TEST_EQUALS(label.GetAdjustedFontSizeScale(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelCutoutEnabled(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetTextCutoutEnabled(true);
  DALI_TEST_EQUALS(label.IsTextCutoutEnabled(), true, TEST_LOCATION);

  label.SetTextCutoutEnabled(false);
  DALI_TEST_EQUALS(label.IsTextCutoutEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelCutoutSyncAsyncLifecycleP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  const Vector4 backgroundColor(0.15f, 0.25f, 0.35f, 0.8f);
  Label         label = Label::New("Cutout must survive synchronous and asynchronous rendering transitions");
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(88.0f);
  label.SetBackgroundColor(UiColor(backgroundColor));
  label.SetTextCutoutEnabled(true);
  DALI_TEST_CHECK(label.IsTextCutoutEnabled());

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(HasValidTextTexture(label));

  label.AsyncRenderFinishedSignal().Connect(&OnAsyncRenderFinished);
  label.SetAsyncRendering(true);
  gAsyncRenderFinished = false;
  label.SetText("The first asynchronous request keeps cutout enabled");
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForAsyncRender(application));
  DALI_TEST_CHECK(label.IsTextCutoutEnabled());
  DALI_TEST_CHECK(HasValidTextTexture(label));

  gAsyncRenderFinished = false;
  label.SetTextCutoutEnabled(false);
  label.SetText("The next request resets the reused worker to ordinary text");
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForAsyncRender(application));
  DALI_TEST_CHECK(!label.IsTextCutoutEnabled());
  DALI_TEST_CHECK(HasValidTextTexture(label));

  gAsyncRenderFinished = false;
  label.SetTextCutoutEnabled(true);
  label.SetText("The final request enables cutout again without stale state");
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForAsyncRender(application));
  DALI_TEST_CHECK(label.IsTextCutoutEnabled());
  DALI_TEST_CHECK(HasValidTextTexture(label));
  DALI_TEST_EQUALS(label.GetBackgroundColor().GetRgba(), backgroundColor, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextFit(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  Text::Fit fit = label.GetTextFit();
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::NONE);

  Text::Fit::Range range(10.0f, 40.0f, 2.0f);
  label.SetTextFit(range);

  fit = label.GetTextFit();
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::RANGE);
  DALI_TEST_EQUALS(fit.GetRange().GetMinimumFontSize(), 10.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(fit.GetRange().GetMaximumFontSize(), 40.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(fit.GetRange().GetFontSizeStep(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Dali::Vector<Text::Fit::Candidate> candidates;
  candidates.PushBack(Text::Fit::Candidate(16.0f, 32.0f));
  candidates.PushBack(Text::Fit::Candidate(24.0f, 48.0f));
  label.SetTextFit(candidates);

  fit = label.GetTextFit();
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::CANDIDATES);
  DALI_TEST_EQUALS(fit.GetCandidates().Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(fit.GetCandidates()[0].GetFontSize(), 16.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(fit.GetCandidates()[0].GetLineHeight(), 32.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(fit.GetCandidates()[1].GetFontSize(), 24.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(fit.GetCandidates()[1].GetLineHeight(), 48.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  label.SetTextFit(Text::Fit::None());
  fit = label.GetTextFit();
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::NONE);

  label.SetTextFit(Text::Fit::FromRange(range));
  fit = label.GetTextFit();
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::RANGE);

  Dali::Vector<Text::Fit::Candidate> emptyCandidates;
  label.SetTextFit(emptyCandidates);

  fit = label.GetTextFit();
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::NONE);

  END_TEST;
}

int UtcDaliLabelTextFitRenderingP(void)
{
  UiTestApplication application;
  Label             label = Label::New("TextFit should preserve its state across synchronous and asynchronous rendering");
  DALI_TEST_CHECK(label);

  label.SetMultiLine(true);
  label.SetRequestedWidth(180.0f);
  label.SetRequestedHeight(80.0f);

  const Text::Fit::Range range(10.0f, 36.0f, 2.0f);
  label.SetTextFit(range);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(HasValidTextTexture(label));

  label.AsyncRenderFinishedSignal().Connect(&OnAsyncRenderFinished);
  label.SetAsyncRendering(true);

  gAsyncRenderFinished = false;
  gAsyncRenderWidth    = 0.0f;
  gAsyncRenderHeight   = 0.0f;
  label.SetRequestedWidth(160.0f);
  label.SetRequestedHeight(70.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForAsyncRender(application));
  DALI_TEST_CHECK(gAsyncRenderWidth > 0.0f);
  DALI_TEST_CHECK(gAsyncRenderHeight > 0.0f);

  Text::Fit fit = label.GetTextFit();
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::RANGE);
  DALI_TEST_EQUALS(fit.GetRange().GetMinimumFontSize(), 10.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(fit.GetRange().GetMaximumFontSize(), 36.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(fit.GetRange().GetFontSizeStep(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Dali::Vector<Text::Fit::Candidate> candidates;
  candidates.PushBack(Text::Fit::Candidate(12.0f, 16.0f));
  candidates.PushBack(Text::Fit::Candidate(24.0f, 30.0f));

  gAsyncRenderFinished = false;
  label.SetTextFit(candidates);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForAsyncRender(application));

  fit = label.GetTextFit();
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::CANDIDATES);
  DALI_TEST_EQUALS(fit.GetCandidates().Count(), 2u, TEST_LOCATION);

  label.SetTextFit(Text::Fit::None());
  DALI_TEST_CHECK(label.GetTextFit().GetType() == Text::Fit::Type::NONE);

  label.SetAsyncRendering(false);
  label.SetTextFit(range);
  label.SetRequestedWidth(140.0f);
  label.SetRequestedHeight(60.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(HasValidTextTexture(label));
  DALI_TEST_CHECK(label.GetTextFit().GetType() == Text::Fit::Type::RANGE);

  END_TEST;
}

int UtcDaliLabelFontVariation(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  // Set via axis API
  Dali::Vector<Text::FontVariation::Axis> axes;
  axes.PushBack(Text::FontVariation::Axis("wght", 700.0f));
  axes.PushBack(Text::FontVariation::Axis("wdth", 90.0f));

  label.SetFontVariation(axes);

  Dali::Vector<Text::FontVariation::Axis> result = label.GetFontVariation();

  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetValue(), 700.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetValue(), 90.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Set via string API
  label.SetFontVariation("wght=500,wdth=80");

  result = label.GetFontVariation();

  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetValue(), 500.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetValue(), 80.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Empty string is invalid and should not clear or change the font variation.
  label.SetFontVariation("");

  result = label.GetFontVariation();

  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetValue(), 500.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetValue(), 80.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Clear via None()
  label.SetFontVariation(Text::FontVariation::None());

  result = label.GetFontVariation();
  DALI_TEST_EQUALS(result.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelAsyncRendering(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetAsyncRendering(true);
  DALI_TEST_EQUALS(label.IsAsyncRendering(), true, TEST_LOCATION);

  label.SetAsyncRendering(false);
  DALI_TEST_EQUALS(label.IsAsyncRendering(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelRenderScale(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetRenderScale(2.0f);
  DALI_TEST_EQUALS(label.GetRenderScale(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  label.SetRenderScale(1.5f);
  DALI_TEST_EQUALS(label.GetRenderScale(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

// Property
int UtcDaliLabelGetProperty(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  // Check Property Indices are correct
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_TEXT) == Label::Property::TEXT);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_FONT_FAMILY) == Label::Property::FONT_FAMILY);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_FONT_SIZE) == Label::Property::FONT_SIZE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MULTI_LINE) == Label::Property::MULTI_LINE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_LINE_WRAP_MODE) == Label::Property::LINE_WRAP_MODE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_HORIZONTAL_ALIGNMENT) == Label::Property::HORIZONTAL_ALIGNMENT);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_VERTICAL_ALIGNMENT) == Label::Property::VERTICAL_ALIGNMENT);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_OVERFLOW_MODE) == Label::Property::OVERFLOW_MODE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_LINE_HEIGHT) == Label::Property::LINE_HEIGHT);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_LINE_HEIGHT_MODE) == Label::Property::LINE_HEIGHT_MODE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_LAYOUT_DIRECTION_MODE) == Label::Property::LAYOUT_DIRECTION_MODE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_ANCHOR_COLOR) == Label::Property::ANCHOR_COLOR);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_ANCHOR_CLICKED_COLOR) == Label::Property::ANCHOR_CLICKED_COLOR);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MARQUEE_TRIGGER_POLICY) == Label::Property::MARQUEE_TRIGGER_POLICY);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MARQUEE_SPEED) == Label::Property::MARQUEE_SPEED);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MARQUEE_LOOP_COUNT) == Label::Property::MARQUEE_LOOP_COUNT);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MARQUEE_LOOP_DELAY) == Label::Property::MARQUEE_LOOP_DELAY);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MARQUEE_GAP) == Label::Property::MARQUEE_GAP);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MARQUEE_ORIENTATION) == Label::Property::MARQUEE_ORIENTATION);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MARQUEE_STOP_MODE) == Label::Property::MARQUEE_STOP_MODE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_FONT_WEIGHT) == Label::Property::FONT_WEIGHT);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_FONT_WIDTH) == Label::Property::FONT_WIDTH);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_FONT_SLANT) == Label::Property::FONT_SLANT);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_TEXT_BACKGROUND_COLOR) == Label::Property::TEXT_BACKGROUND_COLOR);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE) == Label::Property::MINIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE) == Label::Property::MAXIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED) == Label::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_CUTOUT_ENABLED) == Label::Property::CUTOUT_ENABLED);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_ASYNC_RENDERING) == Label::Property::ASYNC_RENDERING);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_RENDER_SCALE) == Label::Property::RENDER_SCALE);

  // Animatable
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_TEXT_COLOR) == Label::Property::TEXT_COLOR);

  END_TEST;
}

int UtcDaliLabelSetProperty(void)
{
  UiTestApplication application;
  Label             label = Label::New();
  DALI_TEST_CHECK(label);

  // TEXT
  label.SetProperty(Label::Property::TEXT, "Hello world");
  DALI_TEST_EQUALS(label.GetProperty<Dali::String>(Label::Property::TEXT), std::string("Hello world"), TEST_LOCATION);

  // FONT_FAMILY
  label.SetProperty(Label::Property::FONT_FAMILY, "Arial");
  DALI_TEST_EQUALS(label.GetProperty<Dali::String>(Label::Property::FONT_FAMILY), std::string("Arial"), TEST_LOCATION);

  // FONT_SIZE
  label.SetProperty(Label::Property::FONT_SIZE, 20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(label.GetProperty<float>(Label::Property::FONT_SIZE), 20.0f, TEST_LOCATION);

  // MULTI_LINE
  label.SetProperty(Label::Property::MULTI_LINE, true);
  DALI_TEST_EQUALS(label.GetProperty<bool>(Label::Property::MULTI_LINE), true, TEST_LOCATION);

  // LINE_WRAP_MODE
  label.SetProperty(Label::Property::LINE_WRAP_MODE, Text::LineWrapMode::WORD);
  DALI_TEST_EQUALS(label.GetProperty<Text::LineWrapMode>(Label::Property::LINE_WRAP_MODE), Text::LineWrapMode::WORD, TEST_LOCATION);

  label.SetProperty(Label::Property::LINE_WRAP_MODE, "CHARACTER");
  DALI_TEST_EQUALS(label.GetProperty<Text::LineWrapMode>(Label::Property::LINE_WRAP_MODE), Text::LineWrapMode::CHARACTER, TEST_LOCATION);

  // HORIZONTAL_ALIGNMENT
  label.SetProperty(Label::Property::HORIZONTAL_ALIGNMENT, Text::Alignment::CENTER);
  DALI_TEST_EQUALS(label.GetProperty<Text::Alignment>(Label::Property::HORIZONTAL_ALIGNMENT), Text::Alignment::CENTER, TEST_LOCATION);

  label.SetProperty(Label::Property::HORIZONTAL_ALIGNMENT, "END");
  DALI_TEST_EQUALS(label.GetProperty<Text::Alignment>(Label::Property::HORIZONTAL_ALIGNMENT), Text::Alignment::END, TEST_LOCATION);

  // VERTICAL_ALIGNMENT
  label.SetProperty(Label::Property::VERTICAL_ALIGNMENT, Text::Alignment::CENTER);
  DALI_TEST_EQUALS(label.GetProperty<Text::Alignment>(Label::Property::VERTICAL_ALIGNMENT), Text::Alignment::CENTER, TEST_LOCATION);

  label.SetProperty(Label::Property::VERTICAL_ALIGNMENT, "END");
  DALI_TEST_EQUALS(label.GetProperty<Text::Alignment>(Label::Property::VERTICAL_ALIGNMENT), Text::Alignment::END, TEST_LOCATION);

  // OVERFLOW_MODE
  label.SetProperty(Label::Property::OVERFLOW_MODE, Text::OverflowMode::CLIP);
  DALI_TEST_EQUALS(label.GetProperty<Text::OverflowMode>(Label::Property::OVERFLOW_MODE), Text::OverflowMode::CLIP, TEST_LOCATION);

  label.SetProperty(Label::Property::OVERFLOW_MODE, "ELLIPSIS");
  DALI_TEST_EQUALS(label.GetProperty<Text::OverflowMode>(Label::Property::OVERFLOW_MODE), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  // LINE_HEIGHT
  label.SetProperty(Label::Property::LINE_HEIGHT, 1.5f);
  DALI_TEST_EQUALS(label.GetProperty<float>(Label::Property::LINE_HEIGHT), 1.5f, TEST_LOCATION);

  // LINE_HEIGHT_MODE
  label.SetProperty(Label::Property::LINE_HEIGHT_MODE, Text::LineHeightMode::ABSOLUTE);
  DALI_TEST_EQUALS(label.GetProperty<Text::LineHeightMode>(Label::Property::LINE_HEIGHT_MODE), Text::LineHeightMode::ABSOLUTE, TEST_LOCATION);

  label.SetProperty(Label::Property::LINE_HEIGHT_MODE, "RELATIVE");
  DALI_TEST_EQUALS(label.GetProperty<Text::LineHeightMode>(Label::Property::LINE_HEIGHT_MODE), Text::LineHeightMode::RELATIVE, TEST_LOCATION);

  // LAYOUT_DIRECTION_MODE
  label.SetProperty(Label::Property::LAYOUT_DIRECTION_MODE, Text::LayoutDirectionMode::LOCALE);
  DALI_TEST_EQUALS(label.GetProperty<Text::LayoutDirectionMode>(Label::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::LOCALE, TEST_LOCATION);

  label.SetProperty(Label::Property::LAYOUT_DIRECTION_MODE, "CONTENTS");
  DALI_TEST_EQUALS(label.GetProperty<Text::LayoutDirectionMode>(Label::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::CONTENTS, TEST_LOCATION);

  label.SetProperty(Label::Property::LAYOUT_DIRECTION_MODE, "INHERIT");
  DALI_TEST_EQUALS(label.GetProperty<Text::LayoutDirectionMode>(Label::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::INHERIT, TEST_LOCATION);

  // ANCHOR_COLOR
  label.SetProperty(Label::Property::ANCHOR_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(label.GetProperty<Vector4>(Label::Property::ANCHOR_COLOR), Color::BLUE, TEST_LOCATION);

  // ANCHOR_CLICKED_COLOR
  label.SetProperty(Label::Property::ANCHOR_CLICKED_COLOR, Color::RED);
  DALI_TEST_EQUALS(label.GetProperty<Vector4>(Label::Property::ANCHOR_CLICKED_COLOR), Color::RED, TEST_LOCATION);

  // MARQUEE_TRIGGER_POLICY
  label.SetProperty(Label::Property::MARQUEE_TRIGGER_POLICY, Text::MarqueeTriggerPolicy::ON_OVERFLOW);
  DALI_TEST_EQUALS(label.GetProperty<Text::MarqueeTriggerPolicy>(Label::Property::MARQUEE_TRIGGER_POLICY), Text::MarqueeTriggerPolicy::ON_OVERFLOW, TEST_LOCATION);

  label.SetProperty(Label::Property::MARQUEE_TRIGGER_POLICY, "MANUAL");
  DALI_TEST_EQUALS(label.GetProperty<Text::MarqueeTriggerPolicy>(Label::Property::MARQUEE_TRIGGER_POLICY), Text::MarqueeTriggerPolicy::MANUAL, TEST_LOCATION);

  // MARQUEE_SPEED
  label.SetProperty(Label::Property::MARQUEE_SPEED, 100);
  DALI_TEST_EQUALS(label.GetProperty<int>(Label::Property::MARQUEE_SPEED), 100, TEST_LOCATION);

  // MARQUEE_LOOP_COUNT
  label.SetProperty(Label::Property::MARQUEE_LOOP_COUNT, 3);
  DALI_TEST_EQUALS(label.GetProperty<int>(Label::Property::MARQUEE_LOOP_COUNT), 3, TEST_LOCATION);

  // MARQUEE_LOOP_DELAY
  label.SetProperty(Label::Property::MARQUEE_LOOP_DELAY, 1.5f);
  DALI_TEST_EQUALS(label.GetProperty<float>(Label::Property::MARQUEE_LOOP_DELAY), 1.5f, TEST_LOCATION);

  // MARQUEE_GAP
  label.SetProperty(Label::Property::MARQUEE_GAP, 50);
  DALI_TEST_EQUALS(label.GetProperty<int>(Label::Property::MARQUEE_GAP), 50, TEST_LOCATION);

  // MARQUEE_STOP_MODE
  label.SetProperty(Label::Property::MARQUEE_STOP_MODE, Text::MarqueeStopMode::FINISH_LOOP);
  DALI_TEST_EQUALS(label.GetProperty<Text::MarqueeStopMode>(Label::Property::MARQUEE_STOP_MODE), Text::MarqueeStopMode::FINISH_LOOP, TEST_LOCATION);

  label.SetProperty(Label::Property::MARQUEE_STOP_MODE, "IMMEDIATE");
  DALI_TEST_EQUALS(label.GetProperty<Text::MarqueeStopMode>(Label::Property::MARQUEE_STOP_MODE), Text::MarqueeStopMode::IMMEDIATE, TEST_LOCATION);

  // MARQUEE_ORIENTATION
  label.SetProperty(Label::Property::MARQUEE_ORIENTATION, Text::MarqueeOrientation::VERTICAL);
  DALI_TEST_EQUALS(label.GetProperty<Text::MarqueeOrientation>(Label::Property::MARQUEE_ORIENTATION), Text::MarqueeOrientation::VERTICAL, TEST_LOCATION);

  label.SetProperty(Label::Property::MARQUEE_ORIENTATION, "HORIZONTAL");
  DALI_TEST_EQUALS(label.GetProperty<Text::MarqueeOrientation>(Label::Property::MARQUEE_ORIENTATION), Text::MarqueeOrientation::HORIZONTAL, TEST_LOCATION);

  // FONT_WEIGHT
  label.SetProperty(Label::Property::FONT_WEIGHT, Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(label.GetProperty<Text::FontWeight>(Label::Property::FONT_WEIGHT), Text::FontWeight::BOLD, TEST_LOCATION);

  label.SetProperty(Label::Property::FONT_WEIGHT, "LIGHT");
  DALI_TEST_EQUALS(label.GetProperty<Text::FontWeight>(Label::Property::FONT_WEIGHT), Text::FontWeight::LIGHT, TEST_LOCATION);

  // FONT_WIDTH
  label.SetProperty(Label::Property::FONT_WIDTH, Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(label.GetProperty<Text::FontWidth>(Label::Property::FONT_WIDTH), Text::FontWidth::EXPANDED, TEST_LOCATION);

  label.SetProperty(Label::Property::FONT_WIDTH, "CONDENSED");
  DALI_TEST_EQUALS(label.GetProperty<Text::FontWidth>(Label::Property::FONT_WIDTH), Text::FontWidth::CONDENSED, TEST_LOCATION);

  // FONT_SLANT
  label.SetProperty(Label::Property::FONT_SLANT, Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(label.GetProperty<Text::FontSlant>(Label::Property::FONT_SLANT), Text::FontSlant::ITALIC, TEST_LOCATION);

  label.SetProperty(Label::Property::FONT_SLANT, "OBLIQUE");
  DALI_TEST_EQUALS(label.GetProperty<Text::FontSlant>(Label::Property::FONT_SLANT), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  // TEXT_BACKGROUND_COLOR
  label.SetProperty(Label::Property::TEXT_BACKGROUND_COLOR, Color::YELLOW);
  DALI_TEST_EQUALS(label.GetProperty<Vector4>(Label::Property::TEXT_BACKGROUND_COLOR), Color::YELLOW, TEST_LOCATION);

  // MINIMUM_FONT_SIZE_SCALE
  label.SetProperty(Label::Property::MINIMUM_FONT_SIZE_SCALE, 0.5f);
  DALI_TEST_EQUALS(label.GetProperty<float>(Label::Property::MINIMUM_FONT_SIZE_SCALE), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // MAXIMUM_FONT_SIZE_SCALE
  label.SetProperty(Label::Property::MAXIMUM_FONT_SIZE_SCALE, 2.0f);
  DALI_TEST_EQUALS(label.GetProperty<float>(Label::Property::MAXIMUM_FONT_SIZE_SCALE), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // SYSTEM_FONT_SIZE_SCALE_ENABLED
  label.SetProperty(Label::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED, true);
  DALI_TEST_EQUALS(label.GetProperty<bool>(Label::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED), true, TEST_LOCATION);

  // CUTOUT_ENABLED
  label.SetProperty(Label::Property::CUTOUT_ENABLED, true);
  DALI_TEST_EQUALS(label.GetProperty<bool>(Label::Property::CUTOUT_ENABLED), true, TEST_LOCATION);

  label.SetProperty(Label::Property::CUTOUT_ENABLED, false);
  DALI_TEST_EQUALS(label.GetProperty<bool>(Label::Property::CUTOUT_ENABLED), false, TEST_LOCATION);

  // ASYNC_RENDERING
  label.SetProperty(Label::Property::ASYNC_RENDERING, true);
  DALI_TEST_EQUALS(label.GetProperty<bool>(Label::Property::ASYNC_RENDERING), true, TEST_LOCATION);

  label.SetProperty(Label::Property::ASYNC_RENDERING, false);
  DALI_TEST_EQUALS(label.GetProperty<bool>(Label::Property::ASYNC_RENDERING), false, TEST_LOCATION);

  // RENDER_SCALE
  label.SetProperty(Label::Property::RENDER_SCALE, 2.0f);
  DALI_TEST_EQUALS(label.GetProperty<float>(Label::Property::RENDER_SCALE), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Animatable
  // TEXT_COLOR
  label.SetProperty(Label::Property::TEXT_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(label.GetProperty<Vector4>(Label::Property::TEXT_COLOR), Color::BLUE, TEST_LOCATION);

  // Clear localization state
  label.ClearTranslatableText();
  UiLocalizationManager::Get().ClearLocalizedStringOverride();
  UiLocalizationManager::Get().SetBypassEnabled(false);
  UiLocalizationManager::Get().SetDefaultDomain("");

  END_TEST;
}

// Localization test helpers
namespace
{

bool LabelLocalizationOverride(StringView resourceId, StringView domain, Dali::String& outString)
{
  const std::string rid = ToStdString(resourceId);
  const std::string dom = ToStdString(domain);

  if(rid == "IDS_TITLE")
  {
    if(dom == "domainA")
    {
      outString = "Title A";
    }
    else if(dom == "domainB")
    {
      outString = "Title B";
    }
    else
    {
      outString = "Title Default";
    }
    return true;
  }

  return false;
}

void CleanupLocalization(Label& label)
{
  label.ClearTranslatableText();
  UiLocalizationManager::Get().ClearLocalizedStringOverride();
  UiLocalizationManager::Get().SetBypassEnabled(false);
  UiLocalizationManager::Get().SetDefaultDomain("");
}

} // anonymous namespace

int UtcDaliLabelTranslatableTextP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  label.SetTranslatableText("IDS_TITLE");
  DALI_TEST_EQUALS(label.GetTranslatableText(), "IDS_TITLE", TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelSetTranslatableTextDefaultDomainAfterExplicitDomainP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  // Set default domain to domainB
  locManager.SetDefaultDomain("domainB");

  // First, set with explicit domainA
  label.SetTranslatableText("IDS_TITLE", "domainA");
  DALI_TEST_EQUALS(label.GetText(), "Title A", TEST_LOCATION);

  // Now call SetTranslatableText(resourceId) without domain.
  // This should use default domain (domainB), NOT reuse the previous explicit domainA.
  label.SetTranslatableText("IDS_TITLE");
  DALI_TEST_EQUALS(label.GetText(), "Title B", TEST_LOCATION);

  // Verify that RefreshBindings still uses default domain
  locManager.RefreshBindings();
  DALI_TEST_EQUALS(label.GetText(), "Title B", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelTranslatableTextWithDomainP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  label.SetTranslatableText("IDS_TITLE", "domainA");
  DALI_TEST_EQUALS(label.GetText(), "Title A", TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTranslatableText(), "IDS_TITLE", TEST_LOCATION);

  label.SetTranslatableText("IDS_TITLE", "domainB");
  DALI_TEST_EQUALS(label.GetText(), "Title B", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelTranslatableTextWithDomainChainingP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  // Test fluent API with domain overload
  label.SetTranslatableText("IDS_TITLE", "domainA");
  DALI_TEST_EQUALS(label.GetTranslatableText(), "IDS_TITLE", TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetText(), "Title A", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelClearTranslatableTextP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  label.SetTranslatableText("IDS_TITLE");
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);

  label.ClearTranslatableText();
  // Current Text value is maintained after clear
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTranslatableText(), Dali::String(), TEST_LOCATION);

  // RefreshBindings should not change the text after clear
  locManager.RefreshBindings();
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelSetTextDoesNotClearTranslatableTextP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  label.SetTranslatableText("IDS_TITLE");
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);

  label.SetText("Manual Text");
  DALI_TEST_EQUALS(label.GetText(), "Manual Text", TEST_LOCATION);
  // TranslatableText binding is still active
  DALI_TEST_EQUALS(label.GetTranslatableText(), "IDS_TITLE", TEST_LOCATION);

  // RefreshBindings overwrites Text with localized string
  locManager.RefreshBindings();
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelLocalizationBypassP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  label.SetTranslatableText("IDS_TITLE");
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);

  locManager.SetBypassEnabled(true);
  // Bypass returns resourceId directly
  DALI_TEST_EQUALS(label.GetText(), "IDS_TITLE", TEST_LOCATION);

  locManager.SetBypassEnabled(false);
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelDefaultDomainChangeP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  // No explicit domain - uses default domain
  label.SetTranslatableText("IDS_TITLE");
  DALI_TEST_EQUALS(label.GetText(), "Title Default", TEST_LOCATION);

  locManager.SetDefaultDomain("domainA");
  DALI_TEST_EQUALS(label.GetText(), "Title A", TEST_LOCATION);

  locManager.SetDefaultDomain("domainB");
  DALI_TEST_EQUALS(label.GetText(), "Title B", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelExplicitDomainUnaffectedByDefaultDomainP(void)
{
  TestApplication application;
  Label           label = Label::New();
  application.GetScene().Add(label);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&LabelLocalizationOverride);

  label.SetTranslatableText("IDS_TITLE", "domainA");
  DALI_TEST_EQUALS(label.GetText(), "Title A", TEST_LOCATION);

  locManager.SetDefaultDomain("domainB");
  // Explicit domain should still be domainA
  DALI_TEST_EQUALS(label.GetText(), "Title A", TEST_LOCATION);

  CleanupLocalization(label);
  END_TEST;
}

int UtcDaliLabelImageSpanAuthoritativeNaturalSizeAndFallbackP(void)
{
  UiTestApplication application;

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("A[icon]B");
  Text::ImageSpan         image   = Text::ImageSpan::New(
    Text::ImageAttributes("/path/that/does/not/exist.png", Vector2(180.0f, 32.0f)));
  DALI_TEST_CHECK(builder.SetSpan(image, 1u, 7u));

  Label label = Label::New();
  DALI_TEST_EQUALS(label.ResourceReadySignal().GetConnectionCount(), 0u, TEST_LOCATION);
  label.SetFontSize(16.0f);
  label.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 80.0f));
  application.GetScene().Add(label);
  label.SetStyledText(builder.Build());
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(label.ResourceReadySignal().GetConnectionCount() > 0u);
  DALI_TEST_EQUALS(label.GetText(), "A[icon]B", TEST_LOCATION);
  DALI_TEST_CHECK(label.GetStyledText());

  const Vector3 replacementNaturalSize = label.GetNaturalSize();
  DALI_TEST_CHECK(replacementNaturalSize.width >= 180.0f);
  DALI_TEST_CHECK(replacementNaturalSize.height >= 32.0f);

  Text::StyledTextBuilder invalidBuilder = Text::StyledTextBuilder::New("A[icon]B");
  Text::ImageAttributes   invalidAttributes("", Vector2(180.0f, 32.0f));
  DALI_TEST_CHECK(invalidBuilder.SetSpan(Text::ImageSpan::New(invalidAttributes), 1u, 7u));
  label.SetStyledText(invalidBuilder.Build());
  const Vector3 fallbackNaturalSize = label.GetNaturalSize();
  DALI_TEST_CHECK(fallbackNaturalSize.width < replacementNaturalSize.width);
  DALI_TEST_EQUALS(label.GetText(), "A[icon]B", TEST_LOCATION);
  DALI_TEST_EQUALS(label.ResourceReadySignal().GetConnectionCount(), 0u, TEST_LOCATION);

  const Text::StyledText replacementText = builder.Build();
  for(uint32_t iteration = 0u; iteration < 8u; ++iteration)
  {
    label.SetStyledText(replacementText);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(label.ResourceReadySignal().GetConnectionCount() > 0u);

    label.SetText("plain");
    DALI_TEST_EQUALS(label.ResourceReadySignal().GetConnectionCount(), 0u, TEST_LOCATION);
  }

  Label transient = Label::New();
  transient.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 80.0f));
  application.GetScene().Add(transient);
  transient.SetStyledText(replacementText);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(transient.ResourceReadySignal().GetConnectionCount() > 0u);
  application.GetScene().Remove(transient);
  transient.Reset();
  application.SendNotification();
  application.Render();

  Label cleared = Label::New();
  cleared.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 80.0f));
  application.GetScene().Add(cleared);
  cleared.SetStyledText(replacementText);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(cleared.ResourceReadySignal().GetConnectionCount() > 0u);
  cleared.SetText("");
  DALI_TEST_EQUALS(cleared.ResourceReadySignal().GetConnectionCount(), 0u, TEST_LOCATION);
  application.GetScene().Remove(cleared);
  cleared.Reset();

  Label firstShared  = Label::New();
  Label secondShared = Label::New();
  firstShared.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 80.0f));
  secondShared.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 80.0f));
  application.GetScene().Add(firstShared);
  application.GetScene().Add(secondShared);
  firstShared.SetStyledText(replacementText);
  secondShared.SetStyledText(replacementText);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(firstShared.ResourceReadySignal().GetConnectionCount() > 0u);
  DALI_TEST_CHECK(secondShared.ResourceReadySignal().GetConnectionCount() > 0u);
  application.GetScene().Remove(firstShared);
  firstShared.Reset();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(secondShared.ResourceReadySignal().GetConnectionCount() > 0u);
  application.GetScene().Remove(secondShared);
  secondShared.Reset();

  Label ordinary = Label::New("ordinary");
  application.GetScene().Add(ordinary);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(ordinary.ResourceReadySignal().GetConnectionCount(), 0u, TEST_LOCATION);
  application.GetScene().Remove(ordinary);
  ordinary.Reset();

  END_TEST;
}

int UtcDaliLabelImageSpanAsyncNaturalSizeParityP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  (void)fontClient;

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("before [icon] after");
  DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(
                                    Text::ImageAttributes("missing-image.png", Vector2(160.0f, 30.0f))),
                                  7u,
                                  13u));

  Label label = Label::New();
  label.SetFontSize(16.0f);
  label.SetStyledText(builder.Build());
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();

  const Vector3 syncSize    = label.GetNaturalSize();
  gAsyncNaturalSizeComputed = false;
  gAsyncNaturalSizeWidth    = 0.0f;
  gAsyncNaturalSizeHeight   = 0.0f;
  label.AsyncNaturalSizeComputedSignal().Connect(&OnAsyncNaturalSizeComputed);
  label.SetAsyncRendering(true);
  label.RequestAsyncNaturalSize();
  DALI_TEST_CHECK(WaitForAsyncNaturalSize(application));
  DALI_TEST_EQUALS(gAsyncNaturalSizeWidth, syncSize.width, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gAsyncNaturalSizeHeight, syncSize.height, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetText(), "before [icon] after", TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelImageSpanBlocksMarqueeAndDoesNotAutoRestartP(void)
{
  UiTestApplication application;

  Label label = Label::New("A long plain text that can marquee before an inline replacement is applied.");
  label.SetRequestedWidth(80.0f);
  label.SetRequestedHeight(40.0f);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(0);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();

  label.StartMarquee();
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(label.IsMarqueeRunning());

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("A [icon] replacement");
  DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(
                                    Text::ImageAttributes("missing-image.png", Vector2(24.0f, 24.0f))),
                                  2u,
                                  8u));
  label.SetStyledText(builder.Build());
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(!label.IsMarqueeRunning());

  label.StartMarquee();
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(!label.IsMarqueeRunning());

  label.SetText("Plain text again; removal alone must not restart marquee.");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(!label.IsMarqueeRunning());

  END_TEST;
}

int UtcDaliLabelOrdinaryAsyncMarqueeKeepsPublishedTextureP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  (void)fontClient;

  Label label = Label::New(
    "Ordinary asynchronous marquee text must keep its published renderer and texture for every animation frame.");
  label.SetRequestedWidth(160.0f);
  label.SetRequestedHeight(48.0f);
  label.SetAsyncRendering(true);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(0);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(WaitForValidTextTexture(application, label));

  label.StartMarquee();
  application.SendNotification();
  application.Render(16);
  constexpr uint32_t MAX_TRIGGER_COUNT = 4u;
  for(uint32_t trigger = 0u; trigger < MAX_TRIGGER_COUNT && !label.IsMarqueeRunning(); ++trigger)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
    application.SendNotification();
    application.Render(16);
  }
  DALI_TEST_CHECK(label.IsMarqueeRunning());

  for(uint32_t frame = 0u; frame < 32u; ++frame)
  {
    application.Render(16);
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    DALI_TEST_CHECK(HasValidTextTexture(label));
    DALI_TEST_CHECK(label.IsMarqueeRunning());
  }

  label.StopMarquee();
  DALI_TEST_CHECK(!label.IsMarqueeRunning());
  DALI_TEST_CHECK(HasValidTextTexture(label));

  END_TEST;
}

int UtcDaliLabelTextRevealPixelP(void)
{
  UiTestApplication application;
  Label label = Label::New("ABC אבג DEF — 안녕하세요 DALi UI — A🦋B 🌈✨");
  label.SetRequestedWidth(520.0f);
  label.SetRequestedHeight(180.0f);
  label.SetMultiLine(true);
  label.SetTextFit(Text::Fit::Range(12.0f, 28.0f, 2.0f));

  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::PIXEL);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  reveal.SetFadeDurationRatio(0.25f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.0f);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetTextReveal().GetUnit(), Text::Reveal::Unit::PIXEL, TEST_LOCATION);
  DALI_TEST_CHECK(WaitForValidTextTexture(application, label));
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);

  auto CheckRevealRenderer = [&]()
  {
    Renderer renderer = label.GetRendererAt(0u);
    DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
    DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealFadeDuration") != Property::INVALID_INDEX);
    TextureSet textures = renderer.GetTextures();
    DALI_TEST_CHECK(textures.GetTextureCount() >= 2u);
    Texture textTexture = textures.GetTexture(0u);
    Texture metadata    = textures.GetTexture(textures.GetTextureCount() - 1u);
    DALI_TEST_CHECK(textTexture && metadata);
    DALI_TEST_EQUALS(metadata.GetWidth(), textTexture.GetWidth(), TEST_LOCATION);
    DALI_TEST_EQUALS(metadata.GetHeight(), textTexture.GetHeight(), TEST_LOCATION);
  };
  CheckRevealRenderer();

  label.SetAsyncRendering(true);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(WaitForValidTextTexture(application, label));
  CheckRevealRenderer();

  uint32_t previousTextureWidth = 0u;
  for(float renderScale : {1.25f, 1.5f})
  {
    label.SetRenderScale(renderScale);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
    application.SendNotification();
    application.Render(16);
    DALI_TEST_CHECK(WaitForValidTextTexture(application, label));
    CheckRevealRenderer();
    const uint32_t textureWidth = label.GetRendererAt(0u).GetTextures().GetTexture(0u).GetWidth();
    DALI_TEST_CHECK(textureWidth >= previousTextureWidth);
    previousTextureWidth = textureWidth;
  }

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  gl.ResetTextureCallStack();
  Renderer renderer = label.GetRendererAt(0u);
  for(uint32_t update = 0u; update < 1000u; ++update)
  {
    label.SetTextRevealProgress(static_cast<float>(update % 101u) * 0.01f);
  }
  label.SetTextRevealProgress(0.5f);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);

  END_TEST;
}
