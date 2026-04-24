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
const char* const PROPERTY_NAME_TEXT_COLOR                     = "textColor";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT           = "horizontalAlignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT             = "verticalAlignment";
const char* const PROPERTY_NAME_OVERFLOW_MODE                  = "overflowMode";
const char* const PROPERTY_NAME_PLACEHOLDER                    = "placeholder";
const char* const PROPERTY_NAME_PLACEHOLDER_COLOR              = "placeholderColor";
const char* const PROPERTY_NAME_SHOW_PLACEHOLDER_ON_FOCUS      = "showPlaceholderOnFocus";
const char* const PROPERTY_NAME_CURSOR_WIDTH                   = "cursorWidth";
const char* const PROPERTY_NAME_CURSOR_COLOR                   = "cursorColor";
const char* const PROPERTY_NAME_CURSOR_BLINK_ENABLED           = "cursorBlinkEnabled";
const char* const PROPERTY_NAME_CURSOR_BLINK_INTERVAL          = "cursorBlinkInterval";
const char* const PROPERTY_NAME_CURSOR_POSITION                = "cursorPosition";
const char* const PROPERTY_NAME_SELECTION_ENABLED              = "selectionEnabled";
const char* const PROPERTY_NAME_SELECTION_COLOR                = "selectionColor";
const char* const PROPERTY_NAME_SELECTED_TEXT                  = "selectedText";
const char* const PROPERTY_NAME_SELECTED_TEXT_START            = "selectedTextStart";
const char* const PROPERTY_NAME_SELECTED_TEXT_END              = "selectedTextEnd";
const char* const PROPERTY_NAME_MAXIMUM_LENGTH                 = "maximumLength";
const char* const PROPERTY_NAME_EDITABLE                       = "editable";
const char* const PROPERTY_NAME_LAYOUT_DIRECTION_MODE          = "layoutDirectionMode";
const char* const PROPERTY_NAME_FONT_WEIGHT                    = "fontWeight";
const char* const PROPERTY_NAME_FONT_WIDTH                     = "fontWidth";
const char* const PROPERTY_NAME_FONT_SLANT                     = "fontSlant";
const char* const PROPERTY_NAME_TEXT_BACKGROUND_COLOR          = "textBackgroundColor";
const char* const PROPERTY_NAME_FONT_SIZE_SCALE                = "fontSizeScale";
const char* const PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE        = "minimumFontSizeScale";
const char* const PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE        = "maximumFontSizeScale";
const char* const PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED = "systemFontSizeScaleEnabled";

} // namespace

void utc_dali_input_field_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_input_field_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInputFieldConstructorP(void)
{
  UiTestApplication application;
  InputField inputField;
  DALI_TEST_CHECK(!inputField);
  END_TEST;
}

int UtcDaliInputFieldNewP(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);
  END_TEST;
}

int UtcDaliInputFieldCopyConstructorP(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  InputField copy(inputField);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(inputField == copy);
  END_TEST;
}

int UtcDaliInputFieldMoveConstructor(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_EQUALS(1, inputField.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  InputField moved = std::move(inputField);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!inputField);
  END_TEST;
}

int UtcDaliInputFieldAssignmentOperatorP(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  InputField copy;
  copy = inputField;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(inputField == copy);
  END_TEST;
}

int UtcDaliInputFieldMoveAssignment(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_EQUALS(1, inputField.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  InputField moved;
  moved = std::move(inputField);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!inputField);
  END_TEST;
}

int UtcDaliInputFieldDownCastP(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  BaseHandle object(inputField);
  InputField inputField2 = InputField::DownCast(object);
  InputField inputField3 = DownCast<InputField>(object);
  DALI_TEST_CHECK(inputField2);
  DALI_TEST_CHECK(inputField3);
  END_TEST;
}

int UtcDaliInputFieldDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitializedObject;
  InputField inputField1 = InputField::DownCast(unInitializedObject);
  InputField inputField2 = DownCast<InputField>(unInitializedObject);
  DALI_TEST_CHECK(!inputField1);
  DALI_TEST_CHECK(!inputField2);
  END_TEST;
}

// Setter, Getter

int UtcDaliInputFieldText(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetText("Hello world");
  DALI_TEST_EQUALS(inputField.GetText(), std::string("Hello world"), TEST_LOCATION);

  inputField.SetText("Updated text");
  DALI_TEST_EQUALS(inputField.GetText(), std::string("Updated text"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldFontFamily(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetFontFamily("Arial");
  DALI_TEST_EQUALS(inputField.GetFontFamily(), std::string("Arial"), TEST_LOCATION);

  inputField.SetFontFamily("Roboto");
  DALI_TEST_EQUALS(inputField.GetFontFamily(), std::string("Roboto"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldFontSize(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetFontSize(20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputField.GetFontSize(), 20.0f, TEST_LOCATION);

  inputField.SetFontSize(32.5f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputField.GetFontSize(), 32.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTextColor(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  UiColor color(Color::BLUE);
  inputField.SetTextColor(color);
  DALI_TEST_EQUALS(inputField.GetTextColor().Resolve(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  inputField.SetTextColor(color2);
  DALI_TEST_EQUALS(inputField.GetTextColor().Resolve(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldHorizontalTextAlignment(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(inputField.GetHorizontalTextAlignment(), Text::Alignment::CENTER, TEST_LOCATION);

  inputField.SetHorizontalTextAlignment(Text::Alignment::END);
  DALI_TEST_EQUALS(inputField.GetHorizontalTextAlignment(), Text::Alignment::END, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldVerticalTextAlignment(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetVerticalTextAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(inputField.GetVerticalTextAlignment(), Text::Alignment::CENTER, TEST_LOCATION);

  inputField.SetVerticalTextAlignment(Text::Alignment::END);
  DALI_TEST_EQUALS(inputField.GetVerticalTextAlignment(), Text::Alignment::END, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldOverflowMode(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetOverflowMode(Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(inputField.GetOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  inputField.SetOverflowMode(Text::OverflowMode::CLIP);
  DALI_TEST_EQUALS(inputField.GetOverflowMode(), Text::OverflowMode::CLIP, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldPlaceholder(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetPlaceholder("Enter text");
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), std::string("Enter text"), TEST_LOCATION);

  inputField.SetPlaceholder("Type here");
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), std::string("Type here"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldPlaceholderColor(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  UiColor color(Color::GRAY);
  inputField.SetPlaceholderColor(color);
  DALI_TEST_EQUALS(inputField.GetPlaceholderColor().Resolve(), Color::GRAY, TEST_LOCATION);

  UiColor color2(Color::BLUE);
  inputField.SetPlaceholderColor(color2);
  DALI_TEST_EQUALS(inputField.GetPlaceholderColor().Resolve(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldShowPlaceholderOnFocus(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetShowPlaceholderOnFocus(true);
  DALI_TEST_EQUALS(inputField.IsPlaceholderShownOnFocus(), true, TEST_LOCATION);

  inputField.SetShowPlaceholderOnFocus(false);
  DALI_TEST_EQUALS(inputField.IsPlaceholderShownOnFocus(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldCursorWidth(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetCursorWidth(2);
  DALI_TEST_EQUALS(inputField.GetCursorWidth(), 2, TEST_LOCATION);

  inputField.SetCursorWidth(4);
  DALI_TEST_EQUALS(inputField.GetCursorWidth(), 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldCursorColor(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  UiColor color(Color::BLUE);
  inputField.SetCursorColor(color);
  DALI_TEST_EQUALS(inputField.GetCursorColor().Resolve(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  inputField.SetCursorColor(color2);
  DALI_TEST_EQUALS(inputField.GetCursorColor().Resolve(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldCursorBlinkEnabled(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetCursorBlinkEnabled(true);
  DALI_TEST_EQUALS(inputField.IsCursorBlinkEnabled(), true, TEST_LOCATION);

  inputField.SetCursorBlinkEnabled(false);
  DALI_TEST_EQUALS(inputField.IsCursorBlinkEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldCursorBlinkInterval(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetCursorBlinkInterval(0.5f);
  DALI_TEST_EQUALS(inputField.GetCursorBlinkInterval(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputField.SetCursorBlinkInterval(1.0f);
  DALI_TEST_EQUALS(inputField.GetCursorBlinkInterval(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldCursorPosition(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Empty text: cursor position should be clamped to 0.
  inputField.SetCursorPosition(5u);
  DALI_TEST_EQUALS(inputField.GetCursorPosition(), 0u, TEST_LOCATION);

  Dali::String text = "Hello world";
  inputField.SetText(text);

  // Clamp to the end when the requested position exceeds text length.
  inputField.SetCursorPosition(50u);
  DALI_TEST_EQUALS(inputField.GetCursorPosition(), text.Size(), TEST_LOCATION);

  inputField.SetCursorPosition(text.Size());
  DALI_TEST_EQUALS(inputField.GetCursorPosition(), text.Size(), TEST_LOCATION);

  inputField.SetCursorPosition(5u);
  DALI_TEST_EQUALS(inputField.GetCursorPosition(), 5u, TEST_LOCATION);

  inputField.SetCursorPosition(0u);
  DALI_TEST_EQUALS(inputField.GetCursorPosition(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldSelectionEnabled(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Default should be true
  DALI_TEST_EQUALS(inputField.IsSelectionEnabled(), true, TEST_LOCATION);

  inputField.SetSelectionEnabled(false);
  DALI_TEST_EQUALS(inputField.IsSelectionEnabled(), false, TEST_LOCATION);

  inputField.SetSelectionEnabled(true);
  DALI_TEST_EQUALS(inputField.IsSelectionEnabled(), true, TEST_LOCATION);

  // Test chaining
  InputField& ref = inputField.SetSelectionEnabled(false);
  DALI_TEST_CHECK(&ref == &inputField);

  END_TEST;
}

int UtcDaliInputFieldSelectionColor(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  UiColor color(Color::CYAN);
  inputField.SetSelectionColor(color);
  DALI_TEST_EQUALS(inputField.GetSelectionColor().Resolve(), Color::CYAN, TEST_LOCATION);

  UiColor color2(Color::MAGENTA);
  inputField.SetSelectionColor(color2);
  DALI_TEST_EQUALS(inputField.GetSelectionColor().Resolve(), Color::MAGENTA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldMaximumLength(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetMaximumLength(10);
  DALI_TEST_EQUALS(inputField.GetMaximumLength(), 10, TEST_LOCATION);

  inputField.SetMaximumLength(100);
  DALI_TEST_EQUALS(inputField.GetMaximumLength(), 100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldEditable(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Default should be true
  DALI_TEST_CHECK(inputField.IsEditable());

  inputField.SetEditable(false);
  DALI_TEST_EQUALS(inputField.IsEditable(), false, TEST_LOCATION);

  inputField.SetEditable(true);
  DALI_TEST_EQUALS(inputField.IsEditable(), true, TEST_LOCATION);

  // Test chaining
  InputField& ref = inputField.SetEditable(false);
  DALI_TEST_CHECK(&ref == &inputField);

  END_TEST;
}

int UtcDaliInputFieldLayoutDirectionMode(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  DALI_TEST_EQUALS(inputField.GetLayoutDirectionMode(), Text::LayoutDirectionMode::LOCALE, TEST_LOCATION);

  inputField.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  DALI_TEST_EQUALS(inputField.GetLayoutDirectionMode(), Text::LayoutDirectionMode::CONTENTS, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldFontWeight(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetFontWeight(Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(inputField.GetFontWeight(), Text::FontWeight::BOLD, TEST_LOCATION);

  inputField.SetFontWeight(Text::FontWeight::LIGHT);
  DALI_TEST_EQUALS(inputField.GetFontWeight(), Text::FontWeight::LIGHT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldFontWidth(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetFontWidth(Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(inputField.GetFontWidth(), Text::FontWidth::EXPANDED, TEST_LOCATION);

  inputField.SetFontWidth(Text::FontWidth::CONDENSED);
  DALI_TEST_EQUALS(inputField.GetFontWidth(), Text::FontWidth::CONDENSED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldFontSlant(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetFontSlant(Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(inputField.GetFontSlant(), Text::FontSlant::ITALIC, TEST_LOCATION);

  inputField.SetFontSlant(Text::FontSlant::OBLIQUE);
  DALI_TEST_EQUALS(inputField.GetFontSlant(), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTextBackgroundColor(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  UiColor color(Color::YELLOW);
  inputField.SetTextBackgroundColor(color);
  DALI_TEST_EQUALS(inputField.GetTextBackgroundColor().Resolve(), Color::YELLOW, TEST_LOCATION);

  UiColor color2(Color::GREEN);
  inputField.SetTextBackgroundColor(color2);
  DALI_TEST_EQUALS(inputField.GetTextBackgroundColor().Resolve(), Color::GREEN, TEST_LOCATION);

  // Clear text background color
  inputField.ClearTextBackgroundColor();
  DALI_TEST_EQUALS(inputField.GetTextBackgroundColor().Resolve(), Color::TRANSPARENT, TEST_LOCATION);

  // Set again after clear
  inputField.SetTextBackgroundColor(Color::BLUE);
  DALI_TEST_EQUALS(inputField.GetTextBackgroundColor().Resolve(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldFontSizeScale(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetFontSizeScale(1.5f);
  DALI_TEST_EQUALS(inputField.GetFontSizeScale(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputField.SetFontSizeScale(2.0f);
  DALI_TEST_EQUALS(inputField.GetFontSizeScale(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldMinimumFontSizeScale(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetMinimumFontSizeScale(0.5f);
  DALI_TEST_EQUALS(inputField.GetMinimumFontSizeScale(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputField.SetMinimumFontSizeScale(0.8f);
  DALI_TEST_EQUALS(inputField.GetMinimumFontSizeScale(), 0.8f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldMaximumFontSizeScale(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetMaximumFontSizeScale(2.0f);
  DALI_TEST_EQUALS(inputField.GetMaximumFontSizeScale(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputField.SetMaximumFontSizeScale(1.5f);
  DALI_TEST_EQUALS(inputField.GetMaximumFontSizeScale(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldSystemFontSizeScaleEnabled(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetSystemFontSizeScaleEnabled(true);
  DALI_TEST_EQUALS(inputField.IsSystemFontSizeScaleEnabled(), true, TEST_LOCATION);

  inputField.SetSystemFontSizeScaleEnabled(false);
  DALI_TEST_EQUALS(inputField.IsSystemFontSizeScaleEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldAdjustedFontSizeScale(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Test clamping to minimum
  inputField.SetFontSizeScale(0.5f);
  inputField.SetMinimumFontSizeScale(1.0f);
  inputField.SetMaximumFontSizeScale(2.0f);
  inputField.SetSystemFontSizeScaleEnabled(false);
  DALI_TEST_EQUALS(inputField.GetAdjustedFontSizeScale(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test clamping to maximum
  inputField.SetFontSizeScale(3.0f);
  DALI_TEST_EQUALS(inputField.GetAdjustedFontSizeScale(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test normal range
  inputField.SetFontSizeScale(1.5f);
  DALI_TEST_EQUALS(inputField.GetAdjustedFontSizeScale(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldFontVariation(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Set via axis API
  Dali::Vector<Text::FontVariationAxis> axes;
  axes.PushBack(Text::FontVariationAxis("wght", 700.0f));
  axes.PushBack(Text::FontVariationAxis("wdth", 90.0f));

  inputField.SetFontVariation(axes);

  Dali::Vector<Text::FontVariationAxis> result = inputField.GetFontVariation();

  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetValue(), 700.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetValue(), 90.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Set via string API
  inputField.SetFontVariation("wght=500,wdth=80");

  result = inputField.GetFontVariation();

  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetValue(), 500.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetValue(), 80.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Clear
  inputField.ClearFontVariation();

  result = inputField.GetFontVariation();
  DALI_TEST_EQUALS(result.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

// Property
int UtcDaliInputFieldGetProperty(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Check Property Indices are correct
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TEXT) == InputField::Property::TEXT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_FAMILY) == InputField::Property::FONT_FAMILY);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_SIZE) == InputField::Property::FONT_SIZE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TEXT_COLOR) == InputField::Property::TEXT_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_HORIZONTAL_ALIGNMENT) == InputField::Property::HORIZONTAL_ALIGNMENT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_VERTICAL_ALIGNMENT) == InputField::Property::VERTICAL_ALIGNMENT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_OVERFLOW_MODE) == InputField::Property::OVERFLOW_MODE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_PLACEHOLDER) == InputField::Property::PLACEHOLDER);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_PLACEHOLDER_COLOR) == InputField::Property::PLACEHOLDER_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SHOW_PLACEHOLDER_ON_FOCUS) == InputField::Property::SHOW_PLACEHOLDER_ON_FOCUS);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_WIDTH) == InputField::Property::CURSOR_WIDTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_COLOR) == InputField::Property::CURSOR_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_BLINK_ENABLED) == InputField::Property::CURSOR_BLINK_ENABLED);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_BLINK_INTERVAL) == InputField::Property::CURSOR_BLINK_INTERVAL);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_POSITION) == InputField::Property::CURSOR_POSITION);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTION_ENABLED) == InputField::Property::SELECTION_ENABLED);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTION_COLOR) == InputField::Property::SELECTION_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTED_TEXT) == InputField::Property::SELECTED_TEXT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTED_TEXT_START) == InputField::Property::SELECTED_TEXT_START);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTED_TEXT_END) == InputField::Property::SELECTED_TEXT_END);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_LENGTH) == InputField::Property::MAXIMUM_LENGTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_EDITABLE) == InputField::Property::EDITABLE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_LAYOUT_DIRECTION_MODE) == InputField::Property::LAYOUT_DIRECTION_MODE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_WEIGHT) == InputField::Property::FONT_WEIGHT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_WIDTH) == InputField::Property::FONT_WIDTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_SLANT) == InputField::Property::FONT_SLANT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TEXT_BACKGROUND_COLOR) == InputField::Property::TEXT_BACKGROUND_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_SIZE_SCALE) == InputField::Property::FONT_SIZE_SCALE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE) == InputField::Property::MINIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE) == InputField::Property::MAXIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED) == InputField::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED);

  END_TEST;
}

int UtcDaliInputFieldSetProperty(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // TEXT
  inputField.SetProperty(InputField::Property::TEXT, "Hello world");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::TEXT), std::string("Hello world"), TEST_LOCATION);

  // FONT_FAMILY
  inputField.SetProperty(InputField::Property::FONT_FAMILY, "Arial");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::FONT_FAMILY), std::string("Arial"), TEST_LOCATION);

  // FONT_SIZE
  inputField.SetProperty(InputField::Property::FONT_SIZE, 20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputField.GetProperty<float>(InputField::Property::FONT_SIZE), 20.0f, TEST_LOCATION);

  // TEXT_COLOR
  inputField.SetProperty(InputField::Property::TEXT_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::TEXT_COLOR), Color::BLUE, TEST_LOCATION);

  // HORIZONTAL_ALIGNMENT
  inputField.SetProperty(InputField::Property::HORIZONTAL_ALIGNMENT, Text::Alignment::CENTER);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::Alignment>(InputField::Property::HORIZONTAL_ALIGNMENT), Text::Alignment::CENTER, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::HORIZONTAL_ALIGNMENT, "END");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::Alignment>(InputField::Property::HORIZONTAL_ALIGNMENT), Text::Alignment::END, TEST_LOCATION);

  // VERTICAL_ALIGNMENT
  inputField.SetProperty(InputField::Property::VERTICAL_ALIGNMENT, Text::Alignment::CENTER);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::Alignment>(InputField::Property::VERTICAL_ALIGNMENT), Text::Alignment::CENTER, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::VERTICAL_ALIGNMENT, "END");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::Alignment>(InputField::Property::VERTICAL_ALIGNMENT), Text::Alignment::END, TEST_LOCATION);

  // OVERFLOW_MODE
  inputField.SetProperty(InputField::Property::OVERFLOW_MODE, Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::OverflowMode>(InputField::Property::OVERFLOW_MODE), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::OVERFLOW_MODE, "CLIP");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::OverflowMode>(InputField::Property::OVERFLOW_MODE), Text::OverflowMode::CLIP, TEST_LOCATION);

  // PLACEHOLDER
  inputField.SetProperty(InputField::Property::PLACEHOLDER, "Enter text");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::PLACEHOLDER), std::string("Enter text"), TEST_LOCATION);

  // PLACEHOLDER_COLOR
  inputField.SetProperty(InputField::Property::PLACEHOLDER_COLOR, Color::GRAY);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::PLACEHOLDER_COLOR), Color::GRAY, TEST_LOCATION);

  // SHOW_PLACEHOLDER_ON_FOCUS
  inputField.SetProperty(InputField::Property::SHOW_PLACEHOLDER_ON_FOCUS, true);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::SHOW_PLACEHOLDER_ON_FOCUS), true, TEST_LOCATION);

  // CURSOR_WIDTH
  inputField.SetProperty(InputField::Property::CURSOR_WIDTH, 2);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::CURSOR_WIDTH), 2, TEST_LOCATION);

  // CURSOR_COLOR
  inputField.SetProperty(InputField::Property::CURSOR_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::CURSOR_COLOR), Color::BLUE, TEST_LOCATION);

  // CURSOR_BLINK_ENABLED
  inputField.SetProperty(InputField::Property::CURSOR_BLINK_ENABLED, true);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::CURSOR_BLINK_ENABLED), true, TEST_LOCATION);

  // CURSOR_BLINK_INTERVAL
  inputField.SetProperty(InputField::Property::CURSOR_BLINK_INTERVAL, 0.5f);
  DALI_TEST_EQUALS(inputField.GetProperty<float>(InputField::Property::CURSOR_BLINK_INTERVAL), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // CURSOR_POSITION
  inputField.SetProperty(InputField::Property::CURSOR_POSITION, 5);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::CURSOR_POSITION), 5, TEST_LOCATION);

  // SELECTION_ENABLED
  inputField.SetProperty(InputField::Property::SELECTION_ENABLED, false);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::SELECTION_ENABLED), false, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::SELECTION_ENABLED, true);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::SELECTION_ENABLED), true, TEST_LOCATION);

  // SELECTION_COLOR
  inputField.SetProperty(InputField::Property::SELECTION_COLOR, Color::CYAN);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::SELECTION_COLOR), Color::CYAN, TEST_LOCATION);

  // SELECTED_TEXT (read-only)
  // Get selected text returns empty string if no selection
  DALI_TEST_CHECK(inputField.GetProperty<Dali::String>(InputField::Property::SELECTED_TEXT).Size() >= 0u);

  // SELECTED_TEXT_START (read-only)
  DALI_TEST_CHECK(inputField.GetProperty<int>(InputField::Property::SELECTED_TEXT_START) >= 0);

  // SELECTED_TEXT_END (read-only)
  DALI_TEST_CHECK(inputField.GetProperty<int>(InputField::Property::SELECTED_TEXT_END) >= 0);

  // MAXIMUM_LENGTH
  inputField.SetProperty(InputField::Property::MAXIMUM_LENGTH, 50);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::MAXIMUM_LENGTH), 50, TEST_LOCATION);

  // EDITABLE
  inputField.SetProperty(InputField::Property::EDITABLE, false);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::EDITABLE), false, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::EDITABLE, true);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::EDITABLE), true, TEST_LOCATION);

  // LAYOUT_DIRECTION_MODE
  inputField.SetProperty(InputField::Property::LAYOUT_DIRECTION_MODE, Text::LayoutDirectionMode::LOCALE);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::LayoutDirectionMode>(InputField::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::LOCALE, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::LAYOUT_DIRECTION_MODE, "CONTENTS");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::LayoutDirectionMode>(InputField::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::CONTENTS, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::LAYOUT_DIRECTION_MODE, "INHERIT");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::LayoutDirectionMode>(InputField::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::INHERIT, TEST_LOCATION);

  // FONT_WEIGHT
  inputField.SetProperty(InputField::Property::FONT_WEIGHT, Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontWeight>(InputField::Property::FONT_WEIGHT), Text::FontWeight::BOLD, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::FONT_WEIGHT, "LIGHT");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontWeight>(InputField::Property::FONT_WEIGHT), Text::FontWeight::LIGHT, TEST_LOCATION);

  // FONT_WIDTH
  inputField.SetProperty(InputField::Property::FONT_WIDTH, Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontWidth>(InputField::Property::FONT_WIDTH), Text::FontWidth::EXPANDED, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::FONT_WIDTH, "CONDENSED");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontWidth>(InputField::Property::FONT_WIDTH), Text::FontWidth::CONDENSED, TEST_LOCATION);

  // FONT_SLANT
  inputField.SetProperty(InputField::Property::FONT_SLANT, Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontSlant>(InputField::Property::FONT_SLANT), Text::FontSlant::ITALIC, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::FONT_SLANT, "OBLIQUE");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontSlant>(InputField::Property::FONT_SLANT), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  // TEXT_BACKGROUND_COLOR
  inputField.SetProperty(InputField::Property::TEXT_BACKGROUND_COLOR, Color::YELLOW);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::TEXT_BACKGROUND_COLOR), Color::YELLOW, TEST_LOCATION);

  // FONT_SIZE_SCALE
  inputField.SetProperty(InputField::Property::FONT_SIZE_SCALE, 1.5f);
  DALI_TEST_EQUALS(inputField.GetProperty<float>(InputField::Property::FONT_SIZE_SCALE), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // MINIMUM_FONT_SIZE_SCALE
  inputField.SetProperty(InputField::Property::MINIMUM_FONT_SIZE_SCALE, 0.5f);
  DALI_TEST_EQUALS(inputField.GetProperty<float>(InputField::Property::MINIMUM_FONT_SIZE_SCALE), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // MAXIMUM_FONT_SIZE_SCALE
  inputField.SetProperty(InputField::Property::MAXIMUM_FONT_SIZE_SCALE, 2.0f);
  DALI_TEST_EQUALS(inputField.GetProperty<float>(InputField::Property::MAXIMUM_FONT_SIZE_SCALE), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // SYSTEM_FONT_SIZE_SCALE_ENABLED
  inputField.SetProperty(InputField::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED, true);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED), true, TEST_LOCATION);

  END_TEST;
}
