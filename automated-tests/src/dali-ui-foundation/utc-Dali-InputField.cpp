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
const char* const PROPERTY_NAME_TEXT                  = "text";
const char* const PROPERTY_NAME_FONT_FAMILY           = "fontFamily";
const char* const PROPERTY_NAME_FONT_SIZE             = "fontSize";
const char* const PROPERTY_NAME_TEXT_COLOR            = "textColor";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT  = "horizontalAlignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT    = "verticalAlignment";
const char* const PROPERTY_NAME_OVERFLOW_MODE         = "overflowMode";
const char* const PROPERTY_NAME_PLACEHOLDER           = "placeholder";
const char* const PROPERTY_NAME_PLACEHOLDER_COLOR     = "placeholderColor";
const char* const PROPERTY_NAME_CURSOR_WIDTH          = "cursorWidth";
const char* const PROPERTY_NAME_CURSOR_COLOR          = "cursorColor";
const char* const PROPERTY_NAME_SELECTION_COLOR       = "selectionColor";
const char* const PROPERTY_NAME_MAXIMUM_LENGTH        = "maximumLength";
const char* const PROPERTY_NAME_LAYOUT_DIRECTION_MODE = "layoutDirectionMode";
const char* const PROPERTY_NAME_FONT_WEIGHT           = "fontWeight";
const char* const PROPERTY_NAME_FONT_WIDTH            = "fontWidth";
const char* const PROPERTY_NAME_FONT_SLANT            = "fontSlant";

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
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_WIDTH) == InputField::Property::CURSOR_WIDTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_COLOR) == InputField::Property::CURSOR_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTION_COLOR) == InputField::Property::SELECTION_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_LENGTH) == InputField::Property::MAXIMUM_LENGTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_LAYOUT_DIRECTION_MODE) == InputField::Property::LAYOUT_DIRECTION_MODE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_WEIGHT) == InputField::Property::FONT_WEIGHT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_WIDTH) == InputField::Property::FONT_WIDTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_SLANT) == InputField::Property::FONT_SLANT);

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

  // CURSOR_WIDTH
  inputField.SetProperty(InputField::Property::CURSOR_WIDTH, 2);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::CURSOR_WIDTH), 2, TEST_LOCATION);

  // CURSOR_COLOR
  inputField.SetProperty(InputField::Property::CURSOR_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::CURSOR_COLOR), Color::BLUE, TEST_LOCATION);

  // SELECTION_COLOR
  inputField.SetProperty(InputField::Property::SELECTION_COLOR, Color::CYAN);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::SELECTION_COLOR), Color::CYAN, TEST_LOCATION);

  // MAXIMUM_LENGTH
  inputField.SetProperty(InputField::Property::MAXIMUM_LENGTH, 50);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::MAXIMUM_LENGTH), 50, TEST_LOCATION);

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

  END_TEST;
}
