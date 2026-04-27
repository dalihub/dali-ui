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

#include <stdlib.h>
#include <iostream>
#include <limits>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

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
const char* const PROPERTY_NAME_MARKUP_ENABLED                 = "markupEnabled";
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
const char* const PROPERTY_NAME_FONT_SIZE_SCALE                = "fontSizeScale";
const char* const PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE        = "minimumFontSizeScale";
const char* const PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE        = "maximumFontSizeScale";
const char* const PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED = "systemFontSizeScaleEnabled";
const char* const PROPERTY_NAME_CUTOUT_ENABLED                 = "cutoutEnabled";
const char* const PROPERTY_NAME_ASYNC_RENDERING                = "asyncRendering";
const char* const PROPERTY_NAME_RENDER_SCALE                   = "renderScale";

// Animatable
const char* const PROPERTY_NAME_TEXT_COLOR        = "textColor";
const char* const PROPERTY_NAME_PIXEL_SNAP_FACTOR = "pixelSnapFactor";
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
  Label label;
  DALI_TEST_CHECK(!label);
  END_TEST;
}

int UtcDaliLabelNewP(void)
{
  UiTestApplication application;
  Label label = Label::New();
  DALI_TEST_CHECK(label);
  END_TEST;
}

int UtcDaliLabelNewWithTextP(void)
{
  UiTestApplication application;
  Label label = Label::New("Hello world");
  DALI_TEST_CHECK(label);
  END_TEST;
}

int UtcDaliLabelCopyConstructorP(void)
{
  UiTestApplication application;
  Label label = Label::New();
  Label copy(label);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(label == copy);
  END_TEST;
}

int UtcDaliLabelMoveConstructor(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
  Label copy;
  copy = label;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(label == copy);
  END_TEST;
}

int UtcDaliLabelMoveAssignment(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
  BaseHandle object(label);
  Label label2 = Label::DownCast(object);
  Label label3 = DownCast<Label>(object);
  DALI_TEST_CHECK(label2);
  DALI_TEST_CHECK(label3);
  END_TEST;
}

int UtcDaliLabelDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitializedObject;
  Label label1 = Label::DownCast(unInitializedObject);
  Label label2 = DownCast<Label>(unInitializedObject);
  DALI_TEST_CHECK(!label1);
  DALI_TEST_CHECK(!label2);
  END_TEST;
}

// Setter, Getter

int UtcDaliLabelText(void)
{
  UiTestApplication application;
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetText("Hello world");
  DALI_TEST_EQUALS(label.GetText(), std::string("Hello world"), TEST_LOCATION);

  label.SetText("Updated text");
  DALI_TEST_EQUALS(label.GetText(), std::string("Updated text"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelFontFamily(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetOverflowMode(Text::OverflowMode::CLIP);
  DALI_TEST_EQUALS(label.GetOverflowMode(), Text::OverflowMode::CLIP, TEST_LOCATION);

  label.SetOverflowMode(Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(label.GetOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelLineHeight(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  DALI_TEST_EQUALS(label.GetLayoutDirectionMode(), Text::LayoutDirectionMode::LOCALE, TEST_LOCATION);

  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  DALI_TEST_EQUALS(label.GetLayoutDirectionMode(), Text::LayoutDirectionMode::CONTENTS, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarkupEnabled(void)
{
  UiTestApplication application;
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarkupEnabled(true);
  DALI_TEST_EQUALS(label.IsMarkupEnabled(), true, TEST_LOCATION);

  label.SetMarkupEnabled(false);
  DALI_TEST_EQUALS(label.IsMarkupEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelAnchorColor(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  UiColor color(Color::GREEN);
  label.SetAnchorClickedColor(color);
  DALI_TEST_EQUALS(label.GetAnchorClickedColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  UiColor color2(Color::YELLOW);
  label.SetAnchorClickedColor(color2);
  DALI_TEST_EQUALS(label.GetAnchorClickedColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeTriggerPolicy(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeLoopCount(3);
  DALI_TEST_EQUALS(label.GetMarqueeLoopCount(), 3, TEST_LOCATION);

  label.SetMarqueeLoopCount(5);
  DALI_TEST_EQUALS(label.GetMarqueeLoopCount(), 5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMarqueeLoopDelay(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetMarqueeOrientation(Text::MarqueeOrientation::VERTICAL);
  DALI_TEST_EQUALS(label.GetMarqueeOrientation(), Text::MarqueeOrientation::VERTICAL, TEST_LOCATION);

  label.SetMarqueeOrientation(Text::MarqueeOrientation::HORIZONTAL);
  DALI_TEST_EQUALS(label.GetMarqueeOrientation(), Text::MarqueeOrientation::HORIZONTAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextColor(void)
{
  UiTestApplication application;
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  UiColor color(Color::BLUE);
  label.SetTextColor(color);
  DALI_TEST_EQUALS(label.GetTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  label.SetTextColor(color2);
  DALI_TEST_EQUALS(label.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelFontWeight(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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

int UtcDaliLabelFontSizeScale(void)
{
  UiTestApplication application;
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetFontSizeScale(1.5f);
  DALI_TEST_EQUALS(label.GetFontSizeScale(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  label.SetFontSizeScale(2.0f);
  DALI_TEST_EQUALS(label.GetFontSizeScale(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelMinimumFontSizeScale(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  // Test clamping to minimum
  label.SetFontSizeScale(0.5f);
  label.SetMinimumFontSizeScale(1.0f);
  label.SetMaximumFontSizeScale(2.0f);
  label.SetSystemFontSizeScaleEnabled(false);
  DALI_TEST_EQUALS(label.GetAdjustedFontSizeScale(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test clamping to maximum
  label.SetFontSizeScale(3.0f);
  DALI_TEST_EQUALS(label.GetAdjustedFontSizeScale(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test normal range
  label.SetFontSizeScale(1.5f);
  DALI_TEST_EQUALS(label.GetAdjustedFontSizeScale(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelCutoutEnabled(void)
{
  UiTestApplication application;
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  label.SetCutoutEnabled(true);
  DALI_TEST_EQUALS(label.IsCutoutEnabled(), true, TEST_LOCATION);

  label.SetCutoutEnabled(false);
  DALI_TEST_EQUALS(label.IsCutoutEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelFontVariation(void)
{
  UiTestApplication application;
  Label label = Label::New();
  DALI_TEST_CHECK(label);

  // Set via axis API
  Dali::Vector<Text::FontVariationAxis> axes;
  axes.PushBack(Text::FontVariationAxis("wght", 700.0f));
  axes.PushBack(Text::FontVariationAxis("wdth", 90.0f));

  label.SetFontVariation(axes);

  Dali::Vector<Text::FontVariationAxis> result = label.GetFontVariation();

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

  // Clear
  label.ClearFontVariation();

  result = label.GetFontVariation();
  DALI_TEST_EQUALS(result.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelAsyncRendering(void)
{
  UiTestApplication application;
  Label label = Label::New();
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
  Label label = Label::New();
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
  Label label = Label::New();
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
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MARKUP_ENABLED) == Label::Property::MARKUP_ENABLED);
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
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_FONT_SIZE_SCALE) == Label::Property::FONT_SIZE_SCALE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE) == Label::Property::MINIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE) == Label::Property::MAXIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED) == Label::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_CUTOUT_ENABLED) == Label::Property::CUTOUT_ENABLED);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_ASYNC_RENDERING) == Label::Property::ASYNC_RENDERING);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_RENDER_SCALE) == Label::Property::RENDER_SCALE);

  // Animatable
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_TEXT_COLOR) == Label::Property::TEXT_COLOR);
  DALI_TEST_CHECK(label.GetPropertyIndex(PROPERTY_NAME_PIXEL_SNAP_FACTOR) == Label::Property::PIXEL_SNAP_FACTOR);

  END_TEST;
}

int UtcDaliLabelSetProperty(void)
{
  UiTestApplication application;
  Label label = Label::New();
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

  // MARKUP_ENABLED
  label.SetProperty(Label::Property::MARKUP_ENABLED, true);
  DALI_TEST_EQUALS(label.GetProperty<bool>(Label::Property::MARKUP_ENABLED), true, TEST_LOCATION);

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

  // FONT_SIZE_SCALE
  label.SetProperty(Label::Property::FONT_SIZE_SCALE, 1.5f);
  DALI_TEST_EQUALS(label.GetProperty<float>(Label::Property::FONT_SIZE_SCALE), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

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

  // PIXEL_SNAP_FACTOR
  label.SetProperty(Label::Property::PIXEL_SNAP_FACTOR, 0.5f);
  DALI_TEST_EQUALS(label.GetProperty<float>(Label::Property::PIXEL_SNAP_FACTOR), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Animatable
  // TEXT_COLOR
  label.SetProperty(Label::Property::TEXT_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(label.GetProperty<Vector4>(Label::Property::TEXT_COLOR), Color::BLUE, TEST_LOCATION);

  END_TEST;
}
