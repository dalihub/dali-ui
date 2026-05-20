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

#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>
#include <dali-ui-foundation/public-api/visuals/border-visual.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>
#include <dali-ui-foundation/public-api/visuals/text-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_text_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextVisualCreateAndOwner(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  // Initially, the visual is not attached to any view.
  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::INVALID, TEST_LOCATION);

  View view = View::New();
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), true, TEST_LOCATION);

  DALI_TEST_EQUALS(visual.GetOwner(), view, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualAt(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 0u), visual, TEST_LOCATION);

  visual.Detach();

  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::INVALID, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualGetVisualType(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  // Upcast the handle
  VisualBase visualBase = visual;

  DALI_TEST_EQUALS(visualBase.GetVisualType(), Visual::TEXT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualDownCast(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  // Upcast the handle
  BaseHandle baseHandle = visual;

  // Downcast check
  DALI_TEST_CHECK(VisualBase::DownCast(baseHandle));
  DALI_TEST_CHECK(TextVisual::DownCast(baseHandle));

  // Do not convert to other type of visual
  DALI_TEST_CHECK(!AnimatedImageVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!BorderVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!ColorVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!GradientVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!ImageVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!LottieAnimationVisual::DownCast(baseHandle));

  END_TEST;
}

int UtcDaliTextVisualText(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetText("Hello World");
  DALI_TEST_EQUALS(visual.GetText(), "Hello World", TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualFontFamily(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetFontFamily("Arial");
  DALI_TEST_EQUALS(visual.GetFontFamily(), "Arial", TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualFontSize(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetFontSize(20.0f);
  DALI_TEST_EQUALS(visual.GetFontSize(), 20.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualFontWeight(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetFontWeight(Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(visual.GetFontWeight(), Text::FontWeight::BOLD, TEST_LOCATION);

  visual.SetFontWeight(Text::FontWeight::LIGHT);
  DALI_TEST_EQUALS(visual.GetFontWeight(), Text::FontWeight::LIGHT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualFontWidth(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetFontWidth(Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(visual.GetFontWidth(), Text::FontWidth::EXPANDED, TEST_LOCATION);

  visual.SetFontWidth(Text::FontWidth::CONDENSED);
  DALI_TEST_EQUALS(visual.GetFontWidth(), Text::FontWidth::CONDENSED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualFontSlant(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetFontSlant(Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(visual.GetFontSlant(), Text::FontSlant::ITALIC, TEST_LOCATION);

  visual.SetFontSlant(Text::FontSlant::OBLIQUE);
  DALI_TEST_EQUALS(visual.GetFontSlant(), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualMultiLine(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  DALI_TEST_EQUALS(visual.IsMultiLine(), false, TEST_LOCATION);

  visual.SetMultiLine(true);
  DALI_TEST_EQUALS(visual.IsMultiLine(), true, TEST_LOCATION);

  visual.SetMultiLine(false);
  DALI_TEST_EQUALS(visual.IsMultiLine(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualLineWrapMode(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  DALI_TEST_EQUALS(visual.GetLineWrapMode(), Text::LineWrapMode::CHARACTER, TEST_LOCATION);

  visual.SetLineWrapMode(Text::LineWrapMode::WORD);
  DALI_TEST_EQUALS(visual.GetLineWrapMode(), Text::LineWrapMode::WORD, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualHorizontalAlignment(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetHorizontalAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(visual.GetHorizontalAlignment(), Text::Alignment::CENTER, TEST_LOCATION);

  visual.SetHorizontalAlignment(Text::Alignment::END);
  DALI_TEST_EQUALS(visual.GetHorizontalAlignment(), Text::Alignment::END, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualVerticalAlignment(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetVerticalAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(visual.GetVerticalAlignment(), Text::Alignment::CENTER, TEST_LOCATION);

  visual.SetVerticalAlignment(Text::Alignment::END);
  DALI_TEST_EQUALS(visual.GetVerticalAlignment(), Text::Alignment::END, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualOverflowMode(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetOverflowMode(Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(visual.GetOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  visual.SetOverflowMode(Text::OverflowMode::CLIP);
  DALI_TEST_EQUALS(visual.GetOverflowMode(), Text::OverflowMode::CLIP, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualLineHeight(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetLineHeight(1.5f);
  DALI_TEST_EQUALS(visual.GetLineHeight(), 1.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualLineHeightMode(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
  DALI_TEST_EQUALS(visual.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualTextColor(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  visual.SetTextColor(UiColor(1.0f, 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(visual.GetTextColor().GetRgba(), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualMarkupEnabled(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  DALI_TEST_EQUALS(visual.IsMarkupEnabled(), false, TEST_LOCATION);

  visual.SetMarkupEnabled(true);
  DALI_TEST_EQUALS(visual.IsMarkupEnabled(), true, TEST_LOCATION);

  visual.SetMarkupEnabled(false);
  DALI_TEST_EQUALS(visual.IsMarkupEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualCutoutEnabled(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  DALI_TEST_EQUALS(visual.IsCutoutEnabled(), false, TEST_LOCATION);

  visual.SetCutoutEnabled(true);
  DALI_TEST_EQUALS(visual.IsCutoutEnabled(), true, TEST_LOCATION);

  visual.SetCutoutEnabled(false);
  DALI_TEST_EQUALS(visual.IsCutoutEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualSetGetPropertyValue(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  // TEXT
  visual.SetText("Hello");
  DALI_TEST_EQUALS(visual.GetText(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Dali::String>(TextVisual::Property::TEXT), "Hello", TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::TEXT, "World");
  DALI_TEST_EQUALS(visual.GetText(), "World", TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Dali::String>(TextVisual::Property::TEXT), "World", TEST_LOCATION);

  // FONT_FAMILY
  visual.SetFontFamily("Arial");
  DALI_TEST_EQUALS(visual.GetFontFamily(), "Arial", TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Dali::String>(TextVisual::Property::FONT_FAMILY), "Arial", TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::FONT_FAMILY, "Courier");
  DALI_TEST_EQUALS(visual.GetFontFamily(), "Courier", TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Dali::String>(TextVisual::Property::FONT_FAMILY), "Courier", TEST_LOCATION);

  // FONT_SIZE
  visual.SetFontSize(20.0f);
  DALI_TEST_EQUALS(visual.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(TextVisual::Property::FONT_SIZE), 20.0f, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::FONT_SIZE, 30.0f);
  DALI_TEST_EQUALS(visual.GetFontSize(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(TextVisual::Property::FONT_SIZE), 30.0f, TEST_LOCATION);

  // FONT_WEIGHT
  visual.SetFontWeight(Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(visual.GetFontWeight(), Text::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::FontWeight>(TextVisual::Property::FONT_WEIGHT), Text::FontWeight::BOLD, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::FONT_WEIGHT, Text::FontWeight::LIGHT);
  DALI_TEST_EQUALS(visual.GetFontWeight(), Text::FontWeight::LIGHT, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::FontWeight>(TextVisual::Property::FONT_WEIGHT), Text::FontWeight::LIGHT, TEST_LOCATION);

  // FONT_WIDTH
  visual.SetFontWidth(Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(visual.GetFontWidth(), Text::FontWidth::EXPANDED, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::FontWidth>(TextVisual::Property::FONT_WIDTH), Text::FontWidth::EXPANDED, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::FONT_WIDTH, Text::FontWidth::CONDENSED);
  DALI_TEST_EQUALS(visual.GetFontWidth(), Text::FontWidth::CONDENSED, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::FontWidth>(TextVisual::Property::FONT_WIDTH), Text::FontWidth::CONDENSED, TEST_LOCATION);

  // FONT_SLANT
  visual.SetFontSlant(Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(visual.GetFontSlant(), Text::FontSlant::ITALIC, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::FontSlant>(TextVisual::Property::FONT_SLANT), Text::FontSlant::ITALIC, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::FONT_SLANT, Text::FontSlant::OBLIQUE);
  DALI_TEST_EQUALS(visual.GetFontSlant(), Text::FontSlant::OBLIQUE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::FontSlant>(TextVisual::Property::FONT_SLANT), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  // MULTI_LINE
  visual.SetMultiLine(true);
  DALI_TEST_EQUALS(visual.IsMultiLine(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<bool>(TextVisual::Property::MULTI_LINE), true, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::MULTI_LINE, false);
  DALI_TEST_EQUALS(visual.IsMultiLine(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<bool>(TextVisual::Property::MULTI_LINE), false, TEST_LOCATION);

  // LINE_WRAP_MODE
  visual.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  DALI_TEST_EQUALS(visual.GetLineWrapMode(), Text::LineWrapMode::CHARACTER, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::LineWrapMode>(TextVisual::Property::LINE_WRAP_MODE), Text::LineWrapMode::CHARACTER, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::LINE_WRAP_MODE, Text::LineWrapMode::WORD);
  DALI_TEST_EQUALS(visual.GetLineWrapMode(), Text::LineWrapMode::WORD, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::LineWrapMode>(TextVisual::Property::LINE_WRAP_MODE), Text::LineWrapMode::WORD, TEST_LOCATION);

  // HORIZONTAL_ALIGNMENT
  visual.SetHorizontalAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(visual.GetHorizontalAlignment(), Text::Alignment::CENTER, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::Alignment>(TextVisual::Property::HORIZONTAL_ALIGNMENT), Text::Alignment::CENTER, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::HORIZONTAL_ALIGNMENT, Text::Alignment::END);
  DALI_TEST_EQUALS(visual.GetHorizontalAlignment(), Text::Alignment::END, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::Alignment>(TextVisual::Property::HORIZONTAL_ALIGNMENT), Text::Alignment::END, TEST_LOCATION);

  // VERTICAL_ALIGNMENT
  visual.SetVerticalAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(visual.GetVerticalAlignment(), Text::Alignment::CENTER, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::Alignment>(TextVisual::Property::VERTICAL_ALIGNMENT), Text::Alignment::CENTER, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::VERTICAL_ALIGNMENT, Text::Alignment::END);
  DALI_TEST_EQUALS(visual.GetVerticalAlignment(), Text::Alignment::END, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::Alignment>(TextVisual::Property::VERTICAL_ALIGNMENT), Text::Alignment::END, TEST_LOCATION);

  // OVERFLOW_MODE
  visual.SetOverflowMode(Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(visual.GetOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::OverflowMode>(TextVisual::Property::OVERFLOW_MODE), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  // LINE_HEIGHT
  visual.SetLineHeight(1.5f);
  DALI_TEST_EQUALS(visual.GetLineHeight(), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(TextVisual::Property::LINE_HEIGHT), 1.5f, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::LINE_HEIGHT, 2.0f);
  DALI_TEST_EQUALS(visual.GetLineHeight(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(TextVisual::Property::LINE_HEIGHT), 2.0f, TEST_LOCATION);

  // LINE_HEIGHT_MODE
  visual.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
  DALI_TEST_EQUALS(visual.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Text::LineHeightMode>(TextVisual::Property::LINE_HEIGHT_MODE), Text::LineHeightMode::RELATIVE, TEST_LOCATION);

  // TEXT_COLOR
  visual.SetTextColor(UiColor(1.0f, 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(visual.GetTextColor().GetRgba(), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Vector4>(TextVisual::Property::TEXT_COLOR), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::TEXT_COLOR, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(visual.GetTextColor().GetRgba(), Vector4(0.0f, 1.0f, 0.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Vector4>(TextVisual::Property::TEXT_COLOR), Vector4(0.0f, 1.0f, 0.0f, 1.0f), TEST_LOCATION);

  // MARKUP_ENABLED
  visual.SetMarkupEnabled(true);
  DALI_TEST_EQUALS(visual.IsMarkupEnabled(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<bool>(TextVisual::Property::MARKUP_ENABLED), true, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::MARKUP_ENABLED, false);
  DALI_TEST_EQUALS(visual.IsMarkupEnabled(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<bool>(TextVisual::Property::MARKUP_ENABLED), false, TEST_LOCATION);

  // CUTOUT_ENABLED
  visual.SetCutoutEnabled(true);
  DALI_TEST_EQUALS(visual.IsCutoutEnabled(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<bool>(TextVisual::Property::CUTOUT_ENABLED), true, TEST_LOCATION);

  visual.SetProperty(TextVisual::Property::CUTOUT_ENABLED, false);
  DALI_TEST_EQUALS(visual.IsCutoutEnabled(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<bool>(TextVisual::Property::CUTOUT_ENABLED), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualChaining(void)
{
  UiTestApplication application;

  TextVisual visual = TextVisual::New();

  // Test that setter methods return *this for chaining
  visual.SetText("Hello")
        .SetFontFamily("Arial")
        .SetFontSize(20.0f)
        .SetFontWeight(Text::FontWeight::BOLD)
        .SetFontWidth(Text::FontWidth::NORMAL)
        .SetFontSlant(Text::FontSlant::NORMAL)
        .SetMultiLine(true)
        .SetLineWrapMode(Text::LineWrapMode::WORD)
        .SetHorizontalAlignment(Text::Alignment::CENTER)
        .SetVerticalAlignment(Text::Alignment::CENTER)
        .SetOverflowMode(Text::OverflowMode::ELLIPSIS)
        .SetLineHeight(1.5f)
        .SetLineHeightMode(Text::LineHeightMode::RELATIVE)
        .SetTextColor(UiColor(1.0f, 0.0f, 0.0f, 1.0f))
        .SetMarkupEnabled(true)
        .SetCutoutEnabled(false);

  DALI_TEST_EQUALS(visual.GetText(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetFontFamily(), "Arial", TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetFontWeight(), Text::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetFontWidth(), Text::FontWidth::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetFontSlant(), Text::FontSlant::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsMultiLine(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetLineWrapMode(), Text::LineWrapMode::WORD, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetHorizontalAlignment(), Text::Alignment::CENTER, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetVerticalAlignment(), Text::Alignment::CENTER, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetLineHeight(), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetTextColor().GetRgba(), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsMarkupEnabled(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCutoutEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualInheritedSetters(void)
{
  UiTestApplication application;

  // Test that inherited setters from VisualBase work.
  TextVisual visual = TextVisual::New();

  visual.SetName("TextVisual");
  DALI_TEST_EQUALS(visual.GetName(), "TextVisual", TEST_LOCATION);

  visual.SetColor(UiColor(0.1f, 0.2f, 0.3f, 0.4f));
  DALI_TEST_EQUALS(visual.GetColor().GetRgba(), UiColor(0.1f, 0.2f, 0.3f, 0.4f).GetRgba(), TEST_LOCATION);

  visual.SetOffsetX(12.0f);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 12.0f, TEST_LOCATION);

  visual.SetOffsetY(34.0f);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 34.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualInvalidHandle(void)
{
  UiTestApplication application;

  // Empty TextVisual handle.
  TextVisual empty;

  auto TestAssertFunction = [&](std::function<void(void)> func){
    try
    {
      func();
      tet_result(TET_FAIL);
    }
    catch(DaliException& e)
    {
      tet_result(TET_PASS);
    }
  };

  // Inherit
  TestAssertFunction([&](){empty.SetName("ShouldBeCrash");});
  TestAssertFunction([&](){empty.SetOffsetX(1.0f);});
  TestAssertFunction([&](){empty.SetOffsetY(1.0f);});
  TestAssertFunction([&](){empty.SetWidth(100.0f);});
  TestAssertFunction([&](){empty.SetHeight(100.0f);});
  TestAssertFunction([&](){empty.SetProportionFlags(Visual::Transform::ProportionFlags::ALL);});
  TestAssertFunction([&](){empty.SetExtraWidth(10.0f);});
  TestAssertFunction([&](){empty.SetExtraHeight(10.0f);});
  TestAssertFunction([&](){empty.SetOrigin(Align::CENTER_BEGIN);});
  TestAssertFunction([&](){empty.SetPivot(Align::CENTER_BEGIN);});
  TestAssertFunction([&](){empty.SetSiblingOrder(0u);});
  TestAssertFunction([&](){empty.SetProperty(Property::INVALID_INDEX, Property::Value());});

  TestAssertFunction([&](){empty.GetOwner();});
  TestAssertFunction([&](){empty.GetContainerRangeType();});
  TestAssertFunction([&](){empty.GetName();});
  TestAssertFunction([&](){empty.GetOffsetX();});
  TestAssertFunction([&](){empty.GetOffsetY();});
  TestAssertFunction([&](){empty.GetWidth();});
  TestAssertFunction([&](){empty.GetHeight();});
  TestAssertFunction([&](){empty.GetProportionFlags();});
  TestAssertFunction([&](){empty.GetExtraWidth();});
  TestAssertFunction([&](){empty.GetExtraHeight();});
  TestAssertFunction([&](){empty.GetOrigin();});
  TestAssertFunction([&](){empty.GetPivot();});
  TestAssertFunction([&](){empty.GetSiblingOrder();});
  TestAssertFunction([&](){empty.GetProperty(Property::INVALID_INDEX);});

  // TextVisual specific
  TestAssertFunction([&](){empty.SetText("Hello");});
  TestAssertFunction([&](){empty.SetFontFamily("Arial");});
  TestAssertFunction([&](){empty.SetFontSize(20.0f);});
  TestAssertFunction([&](){empty.SetFontWeight(Text::FontWeight::BOLD);});
  TestAssertFunction([&](){empty.SetFontWidth(Text::FontWidth::EXPANDED);});
  TestAssertFunction([&](){empty.SetFontSlant(Text::FontSlant::ITALIC);});
  TestAssertFunction([&](){empty.SetMultiLine(true);});
  TestAssertFunction([&](){empty.SetLineWrapMode(Text::LineWrapMode::CHARACTER);});
  TestAssertFunction([&](){empty.SetHorizontalAlignment(Text::Alignment::CENTER);});
  TestAssertFunction([&](){empty.SetVerticalAlignment(Text::Alignment::CENTER);});
  TestAssertFunction([&](){empty.SetOverflowMode(Text::OverflowMode::ELLIPSIS);});
  TestAssertFunction([&](){empty.SetLineHeight(1.5f);});
  TestAssertFunction([&](){empty.SetLineHeightMode(Text::LineHeightMode::RELATIVE);});
  TestAssertFunction([&](){empty.SetTextColor(UiColor(Vector4::ONE));});
  TestAssertFunction([&](){empty.SetMarkupEnabled(true);});
  TestAssertFunction([&](){empty.SetCutoutEnabled(true);});

  TestAssertFunction([&](){empty.GetText();});
  TestAssertFunction([&](){empty.GetFontFamily();});
  TestAssertFunction([&](){empty.GetFontSize();});
  TestAssertFunction([&](){empty.GetFontWeight();});
  TestAssertFunction([&](){empty.GetFontWidth();});
  TestAssertFunction([&](){empty.GetFontSlant();});
  TestAssertFunction([&](){empty.IsMultiLine();});
  TestAssertFunction([&](){empty.GetLineWrapMode();});
  TestAssertFunction([&](){empty.GetHorizontalAlignment();});
  TestAssertFunction([&](){empty.GetVerticalAlignment();});
  TestAssertFunction([&](){empty.GetOverflowMode();});
  TestAssertFunction([&](){empty.GetLineHeight();});
  TestAssertFunction([&](){empty.GetLineHeightMode();});
  TestAssertFunction([&](){empty.GetTextColor();});
  TestAssertFunction([&](){empty.IsMarkupEnabled();});
  TestAssertFunction([&](){empty.IsCutoutEnabled();});

  END_TEST;
}
