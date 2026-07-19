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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/internal/text/replacement/editable-inline-replacement-runtime.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <stdlib.h>
#include <iostream>
#include <limits>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

using Dali::Integration::ToStdString;

namespace
{
const char* const PROPERTY_NAME_TEXT                                 = "text";
const char* const PROPERTY_NAME_FONT_FAMILY                          = "fontFamily";
const char* const PROPERTY_NAME_FONT_SIZE                            = "fontSize";
const char* const PROPERTY_NAME_TEXT_COLOR                           = "textColor";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT                 = "horizontalAlignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT                   = "verticalAlignment";
const char* const PROPERTY_NAME_OVERFLOW_MODE                        = "overflowMode";
const char* const PROPERTY_NAME_PLACEHOLDER                          = "placeholder";
const char* const PROPERTY_NAME_PLACEHOLDER_COLOR                    = "placeholderColor";
const char* const PROPERTY_NAME_SHOW_PLACEHOLDER_ON_FOCUS            = "showPlaceholderOnFocus";
const char* const PROPERTY_NAME_CURSOR_WIDTH                         = "cursorWidth";
const char* const PROPERTY_NAME_CURSOR_COLOR                         = "cursorColor";
const char* const PROPERTY_NAME_CURSOR_BLINK_ENABLED                 = "cursorBlinkEnabled";
const char* const PROPERTY_NAME_CURSOR_BLINK_INTERVAL                = "cursorBlinkInterval";
const char* const PROPERTY_NAME_CURSOR_POSITION                      = "cursorPosition";
const char* const PROPERTY_NAME_SELECTION_ENABLED                    = "selectionEnabled";
const char* const PROPERTY_NAME_SELECTION_COLOR                      = "selectionColor";
const char* const PROPERTY_NAME_SELECTED_TEXT                        = "selectedText";
const char* const PROPERTY_NAME_SELECTED_TEXT_START                  = "selectedTextStart";
const char* const PROPERTY_NAME_SELECTED_TEXT_END                    = "selectedTextEnd";
const char* const PROPERTY_NAME_TEXT_HANDLE_ENABLED                  = "textHandleEnabled";
const char* const PROPERTY_NAME_TEXT_HANDLE_COLOR                    = "textHandleColor";
const char* const PROPERTY_NAME_CURSOR_HANDLE_IMAGE                  = "cursorHandleImage";
const char* const PROPERTY_NAME_CURSOR_HANDLE_PRESSED_IMAGE          = "cursorHandlePressedImage";
const char* const PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT          = "selectionHandleImageLeft";
const char* const PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT         = "selectionHandleImageRight";
const char* const PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT  = "selectionHandlePressedImageLeft";
const char* const PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = "selectionHandlePressedImageRight";
const char* const PROPERTY_NAME_MAXIMUM_LENGTH                       = "maximumLength";
const char* const PROPERTY_NAME_PASSWORD_MODE                        = "passwordMode";
const char* const PROPERTY_NAME_PASSWORD_MASK_CHARACTER              = "passwordMaskCharacter";
const char* const PROPERTY_NAME_PASSWORD_REVEAL_DURATION             = "passwordRevealDuration";
const char* const PROPERTY_NAME_EDITABLE                             = "editable";
const char* const PROPERTY_NAME_LAYOUT_DIRECTION_MODE                = "layoutDirectionMode";
const char* const PROPERTY_NAME_FONT_WEIGHT                          = "fontWeight";
const char* const PROPERTY_NAME_FONT_WIDTH                           = "fontWidth";
const char* const PROPERTY_NAME_FONT_SLANT                           = "fontSlant";
const char* const PROPERTY_NAME_TEXT_BACKGROUND_COLOR                = "textBackgroundColor";
const char* const PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE              = "minimumFontSizeScale";
const char* const PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE              = "maximumFontSizeScale";
const char* const PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED       = "systemFontSizeScaleEnabled";
const char* const PROPERTY_NAME_TYPING_TEXT_COLOR                    = "typingTextColor";
const char* const PROPERTY_NAME_TYPING_FONT_FAMILY                   = "typingFontFamily";
const char* const PROPERTY_NAME_TYPING_FONT_SIZE                     = "typingFontSize";
const char* const PROPERTY_NAME_TYPING_FONT_WEIGHT                   = "typingFontWeight";
const char* const PROPERTY_NAME_TYPING_FONT_WIDTH                    = "typingFontWidth";
const char* const PROPERTY_NAME_TYPING_FONT_SLANT                    = "typingFontSlant";

constexpr const char* TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME             = "uTextGradientStartOffset";
constexpr const char* PLACEHOLDER_TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME = "uPlaceholderTextGradientStartOffset";
constexpr float       EPSILON                                              = Math::MACHINE_EPSILON_1000;

Gradient::Linear MakeInputFieldGradient(const Vector4& first, const Vector4& second)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(first)), Gradient::StopNode(1.0f, UiColor(second))});
  return gradient;
}

Gradient::Radial MakeInputFieldRadialGradient()
{
  Gradient::Radial gradient(Vector2(12.0f, 18.0f), 24.0f);
  gradient.SetUnits(Gradient::Units::USER_SPACE);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REPEAT);
  gradient.SetStartOffset(0.125f);
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Vector4(1.0f, 0.0f, 0.0f, 0.0f))),
                         Gradient::StopNode(0.5f, UiColor(Color::GREEN)),
                         Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
  return gradient;
}

Gradient::Conic MakeInputFieldConicGradient()
{
  Gradient::Conic gradient(Vector2(0.25f, 0.75f), Radian(0.75f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  gradient.SetStartOffset(-0.25f);
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::CYAN)),
                         Gradient::StopNode(1.0f, UiColor(Vector4(1.0f, 1.0f, 0.0f, 0.35f)))});
  return gradient;
}

void ExpectInputFieldGradient(const Gradient::Base& gradient, const Vector4& first, const Vector4& second)
{
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  const auto linear = Gradient::Linear::DownCast(gradient);
  DALI_TEST_CHECK(linear.GetType() == Gradient::Type::LINEAR);
  DALI_TEST_EQUALS(linear.GetStartPosition(), Vector2(-0.5f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), Vector2(0.5f, 0.0f), TEST_LOCATION);
  const auto stops = linear.GetStopNodes();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[0u].GetColor().GetRgba(), first, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[1u].GetColor().GetRgba(), second, TEST_LOCATION);
}

void ExpectInputFieldRadialGradient(const Gradient::Base& gradient)
{
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::RADIAL, TEST_LOCATION);
  const auto radial = Gradient::Radial::DownCast(gradient);
  DALI_TEST_EQUALS(radial.GetUnits(), Gradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetSpreadMethod(), Gradient::SpreadMethod::REPEAT, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetStartOffset(), 0.125f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetCenter(), Vector2(12.0f, 18.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetRadius(), 24.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  const auto stops = radial.GetStopNodes();
  DALI_TEST_EQUALS(stops.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[0u].GetColor().GetRgba(), Vector4(1.0f, 0.0f, 0.0f, 0.0f), TEST_LOCATION);
}

void ExpectInputFieldConicGradient(const Gradient::Base& gradient)
{
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::CONIC, TEST_LOCATION);
  const auto conic = Gradient::Conic::DownCast(gradient);
  DALI_TEST_EQUALS(conic.GetUnits(), Gradient::Units::OBJECT_BOUNDING_BOX, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetSpreadMethod(), Gradient::SpreadMethod::REFLECT, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartOffset(), -0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetCenter(), Vector2(0.25f, 0.75f), TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartAngle(), Radian(0.75f), TEST_LOCATION);
  const auto stops = conic.GetStopNodes();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[1u].GetColor().GetRgba(), Vector4(1.0f, 1.0f, 0.0f, 0.35f), TEST_LOCATION);
}

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
  DALI_TEST_EQUALS(inputField.GetTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  inputField.SetTextColor(color2);
  DALI_TEST_EQUALS(inputField.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTextStyleNoneP(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  DALI_TEST_CHECK(inputField.GetTextUnderline() == Text::Underline::None());
  Text::Underline underline;
  inputField.SetTextUnderline(underline);
  DALI_TEST_CHECK(inputField.GetTextUnderline() != Text::Underline::None());
  inputField.SetTextUnderline(Text::Underline::None());
  DALI_TEST_CHECK(inputField.GetTextUnderline() == Text::Underline::None());

  DALI_TEST_CHECK(inputField.GetTextShadow() == Text::Shadow::None());
  Text::Shadow zeroOffsetShadow;
  zeroOffsetShadow.SetOffset(Vector2::ZERO);
  inputField.SetTextShadow(zeroOffsetShadow);
  DALI_TEST_CHECK(inputField.GetTextShadow() != Text::Shadow::None());
  inputField.SetTextShadow(Text::Shadow::None());
  DALI_TEST_CHECK(inputField.GetTextShadow() == Text::Shadow::None());

  DALI_TEST_CHECK(inputField.GetTextOutline() == Text::Outline::None());
  Text::Outline zeroWidthOutline;
  zeroWidthOutline.SetWidth(0.0f);
  inputField.SetTextOutline(zeroWidthOutline);
  DALI_TEST_CHECK(inputField.GetTextOutline() != Text::Outline::None());
  inputField.SetTextOutline(Text::Outline::None());
  DALI_TEST_CHECK(inputField.GetTextOutline() == Text::Outline::None());

  DALI_TEST_CHECK(inputField.GetTextLineThrough() == Text::LineThrough::None());
  Text::LineThrough lineThrough;
  inputField.SetTextLineThrough(lineThrough);
  DALI_TEST_CHECK(inputField.GetTextLineThrough() != Text::LineThrough::None());
  inputField.SetTextLineThrough(Text::LineThrough::None());
  DALI_TEST_CHECK(inputField.GetTextLineThrough() == Text::LineThrough::None());

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

  inputField.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(inputField.GetTextOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  inputField.SetTextOverflowMode(Text::OverflowMode::CLIP);
  DALI_TEST_EQUALS(inputField.GetTextOverflowMode(), Text::OverflowMode::CLIP, TEST_LOCATION);

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
  DALI_TEST_EQUALS(inputField.GetPlaceholderColor().GetRgba(), Color::GRAY, TEST_LOCATION);

  UiColor color2(Color::BLUE);
  inputField.SetPlaceholderColor(color2);
  DALI_TEST_EQUALS(inputField.GetPlaceholderColor().GetRgba(), Color::BLUE, TEST_LOCATION);

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
  DALI_TEST_EQUALS(inputField.GetCursorColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  inputField.SetCursorColor(color2);
  DALI_TEST_EQUALS(inputField.GetCursorColor().GetRgba(), Color::RED, TEST_LOCATION);

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

  // Test setter
  inputField.SetSelectionEnabled(false);

  END_TEST;
}

int UtcDaliInputFieldSelectionColor(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  UiColor color(Color::CYAN);
  inputField.SetSelectionColor(color);
  DALI_TEST_EQUALS(inputField.GetSelectionColor().GetRgba(), Color::CYAN, TEST_LOCATION);

  UiColor color2(Color::MAGENTA);
  inputField.SetSelectionColor(color2);
  DALI_TEST_EQUALS(inputField.GetSelectionColor().GetRgba(), Color::MAGENTA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTextHandle(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Test SetTextHandleEnabled
  inputField.SetTextHandleEnabled(true);
  DALI_TEST_EQUALS(inputField.IsTextHandleEnabled(), true, TEST_LOCATION);

  inputField.SetTextHandleEnabled(false);
  DALI_TEST_EQUALS(inputField.IsTextHandleEnabled(), false, TEST_LOCATION);

  // Test setter for SetTextHandleEnabled
  inputField.SetTextHandleEnabled(true);

  // Test SetTextHandleColor
  UiColor handleColor(Color::BLUE);
  inputField.SetTextHandleColor(handleColor);
  DALI_TEST_EQUALS(inputField.GetTextHandleColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor handleColor2(Color::RED);
  inputField.SetTextHandleColor(handleColor2);
  DALI_TEST_EQUALS(inputField.GetTextHandleColor().GetRgba(), Color::RED, TEST_LOCATION);

  // Test setter for SetTextHandleColor
  inputField.SetTextHandleColor(Color::GREEN);

  END_TEST;
}

int UtcDaliInputFieldHandleImages(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Test cursor handle image
  inputField.SetCursorHandleImage("cursor-handle.png");
  DALI_TEST_EQUALS(inputField.GetCursorHandleImage(), std::string("cursor-handle.png"), TEST_LOCATION);

  inputField.SetCursorHandleImage("cursor-handle-2.png");
  DALI_TEST_EQUALS(inputField.GetCursorHandleImage(), std::string("cursor-handle-2.png"), TEST_LOCATION);

  // Test cursor handle pressed image
  inputField.SetCursorHandlePressedImage("cursor-handle-pressed.png");
  DALI_TEST_EQUALS(inputField.GetCursorHandlePressedImage(), std::string("cursor-handle-pressed.png"), TEST_LOCATION);

  // Test selection handle images
  inputField.SetSelectionHandleImageLeft("selection-left.png");
  DALI_TEST_EQUALS(inputField.GetSelectionHandleImageLeft(), std::string("selection-left.png"), TEST_LOCATION);

  inputField.SetSelectionHandleImageRight("selection-right.png");
  DALI_TEST_EQUALS(inputField.GetSelectionHandleImageRight(), std::string("selection-right.png"), TEST_LOCATION);

  inputField.SetSelectionHandlePressedImageLeft("selection-left-pressed.png");
  DALI_TEST_EQUALS(inputField.GetSelectionHandlePressedImageLeft(), std::string("selection-left-pressed.png"), TEST_LOCATION);

  inputField.SetSelectionHandlePressedImageRight("selection-right-pressed.png");
  DALI_TEST_EQUALS(inputField.GetSelectionHandlePressedImageRight(), std::string("selection-right-pressed.png"), TEST_LOCATION);

  // Test setter for a couple of setters
  inputField.SetCursorHandleImage("new-cursor.png");

  inputField.SetSelectionHandleImageLeft("new-left.png");

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


// Password API tests
int UtcDaliInputFieldPasswordMode(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Default should be NONE
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::NONE, TEST_LOCATION);

  // Test setter
  inputField.SetPasswordMode(Text::PasswordMode::HIDE_ALL);

  inputField.SetPasswordMode(Text::PasswordMode::HIDE_ALL);
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::HIDE_ALL, TEST_LOCATION);

  inputField.SetPasswordMode(Text::PasswordMode::REVEAL_LAST_CHARACTER);
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::REVEAL_LAST_CHARACTER, TEST_LOCATION);

  inputField.SetPasswordMode(Text::PasswordMode::NONE);
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldPasswordMaskCharacter(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Test with '*' (U+002A)
  inputField.SetPasswordMaskCharacter(0x2A);
  DALI_TEST_EQUALS(inputField.GetPasswordMaskCharacter(), 0x2Au, TEST_LOCATION);

  // Test with '•' (U+2022)
  inputField.SetPasswordMaskCharacter(0x2022);
  DALI_TEST_EQUALS(inputField.GetPasswordMaskCharacter(), 0x2022u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldPasswordRevealDuration(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Test with 0
  inputField.SetPasswordRevealDuration(0u);
  DALI_TEST_EQUALS(inputField.GetPasswordRevealDuration(), 0u, TEST_LOCATION);

  // Test with 1000ms
  inputField.SetPasswordRevealDuration(1000u);
  DALI_TEST_EQUALS(inputField.GetPasswordRevealDuration(), 1000u, TEST_LOCATION);

  // Test with 500ms
  inputField.SetPasswordRevealDuration(500u);
  DALI_TEST_EQUALS(inputField.GetPasswordRevealDuration(), 500u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldInputFilter(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  DALI_TEST_CHECK(inputField.GetInputFilter() == Text::InputFilter::None());

  Text::InputFilter inputFilter;
  inputFilter.SetAllowPattern("[\\d]");
  inputFilter.SetDenyPattern("[0-5]");
  inputField.SetInputFilter(inputFilter);

  Text::InputFilter currentInputFilter = inputField.GetInputFilter();
  DALI_TEST_EQUALS(currentInputFilter.GetAllowPattern(), Dali::String("[\\d]"), TEST_LOCATION);
  DALI_TEST_EQUALS(currentInputFilter.GetDenyPattern(), Dali::String("[0-5]"), TEST_LOCATION);
  DALI_TEST_CHECK(currentInputFilter == inputFilter);
  DALI_TEST_CHECK(currentInputFilter != Text::InputFilter::None());

  inputField.SetInputFilter(Text::InputFilter::None());
  DALI_TEST_CHECK(inputField.GetInputFilter() == Text::InputFilter::None());

  END_TEST;
}

int UtcDaliInputFieldPasswordProperties(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Check Property Indices are correct
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_PASSWORD_MODE) == InputField::Property::PASSWORD_MODE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_PASSWORD_MASK_CHARACTER) == InputField::Property::PASSWORD_MASK_CHARACTER);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_PASSWORD_REVEAL_DURATION) == InputField::Property::PASSWORD_REVEAL_DURATION);

  // PASSWORD_MODE property
  inputField.SetProperty(InputField::Property::PASSWORD_MODE, static_cast<int>(Text::PasswordMode::HIDE_ALL));
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::HIDE_ALL, TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::PASSWORD_MODE), static_cast<int>(Text::PasswordMode::HIDE_ALL), TEST_LOCATION);

  inputField.SetProperty(InputField::Property::PASSWORD_MODE, static_cast<int>(Text::PasswordMode::REVEAL_LAST_CHARACTER));
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::REVEAL_LAST_CHARACTER, TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::PASSWORD_MODE), static_cast<int>(Text::PasswordMode::REVEAL_LAST_CHARACTER), TEST_LOCATION);

  // PASSWORD_MODE property with string values
  inputField.SetProperty(InputField::Property::PASSWORD_MODE, "NONE");
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::NONE, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::PASSWORD_MODE, "HIDE_ALL");
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::HIDE_ALL, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::PASSWORD_MODE, "REVEAL_LAST_CHARACTER");
  DALI_TEST_EQUALS(inputField.GetPasswordMode(), Text::PasswordMode::REVEAL_LAST_CHARACTER, TEST_LOCATION);

  // PASSWORD_MASK_CHARACTER property
  inputField.SetProperty(InputField::Property::PASSWORD_MASK_CHARACTER, 0x2022);
  DALI_TEST_EQUALS(inputField.GetPasswordMaskCharacter(), 0x2022u, TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::PASSWORD_MASK_CHARACTER), 0x2022, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::PASSWORD_MASK_CHARACTER, 0x2A);
  DALI_TEST_EQUALS(inputField.GetPasswordMaskCharacter(), 0x2Au, TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::PASSWORD_MASK_CHARACTER), 0x2A, TEST_LOCATION);

  // PASSWORD_REVEAL_DURATION property
  inputField.SetProperty(InputField::Property::PASSWORD_REVEAL_DURATION, 1000);
  DALI_TEST_EQUALS(inputField.GetPasswordRevealDuration(), 1000u, TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::PASSWORD_REVEAL_DURATION), 1000, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::PASSWORD_REVEAL_DURATION, 500);
  DALI_TEST_EQUALS(inputField.GetPasswordRevealDuration(), 500u, TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetProperty<int>(InputField::Property::PASSWORD_REVEAL_DURATION), 500, TEST_LOCATION);

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

  // Test setter
  inputField.SetEditable(false);

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


int UtcDaliInputFieldSetStyledText(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetStyledText(Text::StyledText::FromMarkup("<color value='red'>Hello</color>"));
  DALI_TEST_EQUALS(inputField.GetText(), "Hello", TEST_LOCATION);

  inputField.SetStyledText(Text::StyledText::FromMarkup("<annotation style='muted'>Muted</annotation>"));
  DALI_TEST_EQUALS(inputField.GetText(), "Muted", TEST_LOCATION);

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup("<annotation style='muted'>Hello</annotation>");
  const uint32_t annotationCount = builder.GetAnnotationCount();
  for(uint32_t annotationIndex = 0u; annotationIndex < annotationCount; ++annotationIndex)
  {
    const Text::AnnotationSpan annotation = builder.GetAnnotationAt(annotationIndex);
    if(annotation.GetKey() == "style" && annotation.GetValue() == "muted")
    {
      builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0x808080)), builder.GetAnnotationStartIndexAt(annotationIndex), builder.GetAnnotationEndIndexAt(annotationIndex));
    }
  }
  inputField.SetStyledText(builder.Build());
  DALI_TEST_EQUALS(inputField.GetText(), "Hello", TEST_LOCATION);

  Text::StyledTextBuilder replacementBuilder = Text::StyledTextBuilder::New("AiconB");
  DALI_TEST_CHECK(replacementBuilder.SetSpan(
    Text::ImageSpan::New(Text::ImageAttributes("unused.png", Vector2(24.0f, 18.0f))), 1u, 5u));
  inputField.SetStyledText(replacementBuilder.Build());

  inputField.SetCursorPosition(3u);
  DALI_TEST_EQUALS(inputField.GetCursorPosition(), 1u, TEST_LOCATION);
  inputField.SetCursorPosition(5u);
  DALI_TEST_EQUALS(inputField.GetCursorPosition(), 5u, TEST_LOCATION);

  uint32_t selectionStart = 3u;
  uint32_t selectionEnd   = 6u;
  auto&    inputFieldImpl = static_cast<Dali::Ui::Integration::InputFieldImpl&>(inputField.GetImplementation());
  inputFieldImpl.SetTextSelectionRange(&selectionStart, &selectionEnd);
  DALI_TEST_EQUALS(inputField.GetSelectedTextStart(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetSelectedTextEnd(), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetSelectedText(), "iconB", TEST_LOCATION);

  inputField.SetText("<color value='red'>Hello</color>");
  DALI_TEST_EQUALS(inputField.GetText(), "<color value='red'>Hello</color>", TEST_LOCATION);

  const auto buildReplacementText = [](float verticalOffset)
  {
    Text::StyledTextBuilder runtimeBuilder = Text::StyledTextBuilder::New("AiconB");
    Text::ImageAttributes   attributes("unused.png", Vector2(8.0f, 8.0f));
    attributes.SetVerticalOffset(verticalOffset);
    DALI_TEST_CHECK(runtimeBuilder.SetSpan(Text::ImageSpan::New(attributes), 1u, 5u));
    return runtimeBuilder.Build();
  };
  const auto getVisualOffset = [](Ui::Integration::Visual::Base visual)
  {
    Property::Map visualMap;
    Property::Map transform;
    Vector2       offset;
    visual.CreatePropertyMap(visualMap);
    DALI_TEST_CHECK(visualMap.Find(Ui::VisualBasePropertyIndex::TRANSFORM)->Get(transform));
    DALI_TEST_CHECK(transform.Find(Ui::Visual::Transform::Property::OFFSET)->Get(offset));
    return offset;
  };

  InputField runtimeField = InputField::New();
  runtimeField.SetVerticalTextAlignment(Text::Alignment::START);
  auto& runtimeFieldImpl = static_cast<Dali::Ui::Integration::InputFieldImpl&>(runtimeField.GetImplementation());
  struct TestRelayoutContainer : RelayoutContainer
  {
    void Add(const Actor&, const Vector2&) override
    {
    }
  } relayoutContainer;

  runtimeField.SetStyledText(buildReplacementText(0.0f));
  runtimeFieldImpl.OnRelayout(Vector2(320.0f, 80.0f), relayoutContainer);
  auto* runtime = Dali::Ui::Internal::Text::GetEditableInlineReplacementRuntime(runtimeField);
  DALI_TEST_CHECK(runtime);
  DALI_TEST_CHECK(runtime->visualLayer.GetParent());
  const Property::Index visualIndex = runtime->visualLayer.GetPropertyIndex("__dali_ui_inline_replacement_0");
  DALI_TEST_CHECK(visualIndex != Property::INVALID_INDEX);
  auto& visualData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(runtime->visualLayer));
  Ui::Integration::Visual::Base originalVisual = visualData.GetVisual(visualIndex);
  DALI_TEST_CHECK(originalVisual);
  const Vector2 originalOffset = getVisualOffset(originalVisual);

  runtimeField.SetStyledText(buildReplacementText(-8.0f));
  DALI_TEST_CHECK(Dali::Ui::Internal::Text::GetEditableInlineReplacementRuntime(runtimeField) == runtime);
  DALI_TEST_CHECK(visualData.GetVisual(visualIndex) == originalVisual);
  runtimeFieldImpl.OnRelayout(Vector2(320.0f, 80.0f), relayoutContainer);
  DALI_TEST_CHECK(Dali::Ui::Internal::Text::GetEditableInlineReplacementRuntime(runtimeField) == runtime);
  DALI_TEST_CHECK(visualData.GetVisual(visualIndex) == originalVisual);
  DALI_TEST_CHECK(getVisualOffset(originalVisual).y != originalOffset.y);

  runtimeField.SetText("plain");
  DALI_TEST_CHECK(!Dali::Ui::Internal::Text::GetEditableInlineReplacementRuntime(runtimeField));

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
  DALI_TEST_EQUALS(inputField.GetTextBackgroundColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  UiColor color2(Color::GREEN);
  inputField.SetTextBackgroundColor(color2);
  DALI_TEST_EQUALS(inputField.GetTextBackgroundColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  // Clear text background color
  inputField.ClearTextBackgroundColor();
  DALI_TEST_EQUALS(inputField.GetTextBackgroundColor().GetRgba(), Color::TRANSPARENT, TEST_LOCATION);

  // Set again after clear
  inputField.SetTextBackgroundColor(Color::BLUE);
  DALI_TEST_EQUALS(inputField.GetTextBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);

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

int UtcDaliInputFieldTypingTextColor(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  UiColor color(Color::BLUE);
  inputField.SetTypingTextColor(color);
  DALI_TEST_EQUALS(inputField.GetTypingTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  inputField.SetTypingTextColor(color2);
  DALI_TEST_EQUALS(inputField.GetTypingTextColor().GetRgba(), Color::RED, TEST_LOCATION);

  // Test setter
  inputField.SetTypingTextColor(Color::GREEN);
  DALI_TEST_EQUALS(inputField.GetTypingTextColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTypingFontFamily(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetTypingFontFamily("Arial");
  DALI_TEST_EQUALS(inputField.GetTypingFontFamily(), std::string("Arial"), TEST_LOCATION);

  inputField.SetTypingFontFamily("Roboto");
  DALI_TEST_EQUALS(inputField.GetTypingFontFamily(), std::string("Roboto"), TEST_LOCATION);

  // Test setter
  inputField.SetTypingFontFamily("DejaVu Sans");
  DALI_TEST_EQUALS(inputField.GetTypingFontFamily(), std::string("DejaVu Sans"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTypingFontSize(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetTypingFontSize(20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputField.GetTypingFontSize(), 20.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputField.SetTypingFontSize(32.5f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputField.GetTypingFontSize(), 32.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test setter
  inputField.SetTypingFontSize(28.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputField.GetTypingFontSize(), 28.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTypingFontWeight(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetTypingFontWeight(Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(inputField.GetTypingFontWeight(), Text::FontWeight::BOLD, TEST_LOCATION);

  inputField.SetTypingFontWeight(Text::FontWeight::LIGHT);
  DALI_TEST_EQUALS(inputField.GetTypingFontWeight(), Text::FontWeight::LIGHT, TEST_LOCATION);

  // Test setter
  inputField.SetTypingFontWeight(Text::FontWeight::MEDIUM);
  DALI_TEST_EQUALS(inputField.GetTypingFontWeight(), Text::FontWeight::MEDIUM, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTypingFontWidth(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetTypingFontWidth(Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(inputField.GetTypingFontWidth(), Text::FontWidth::EXPANDED, TEST_LOCATION);

  inputField.SetTypingFontWidth(Text::FontWidth::CONDENSED);
  DALI_TEST_EQUALS(inputField.GetTypingFontWidth(), Text::FontWidth::CONDENSED, TEST_LOCATION);

  // Test setter
  inputField.SetTypingFontWidth(Text::FontWidth::NORMAL);
  DALI_TEST_EQUALS(inputField.GetTypingFontWidth(), Text::FontWidth::NORMAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldTypingFontSlant(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  inputField.SetTypingFontSlant(Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(inputField.GetTypingFontSlant(), Text::FontSlant::ITALIC, TEST_LOCATION);

  inputField.SetTypingFontSlant(Text::FontSlant::OBLIQUE);
  DALI_TEST_EQUALS(inputField.GetTypingFontSlant(), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  // Test setter
  inputField.SetTypingFontSlant(Text::FontSlant::NORMAL);
  DALI_TEST_EQUALS(inputField.GetTypingFontSlant(), Text::FontSlant::NORMAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldAdjustedFontSizeScale(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Test clamping to minimum from default scale 1.0
  inputField.SetMinimumFontSizeScale(1.2f);
  inputField.SetMaximumFontSizeScale(2.0f);
  inputField.SetSystemFontSizeScaleEnabled(false);
  DALI_TEST_EQUALS(inputField.GetAdjustedFontSizeScale(), 1.2f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test clamping to maximum from default scale 1.0
  inputField.SetMinimumFontSizeScale(0.1f);
  inputField.SetMaximumFontSizeScale(0.8f);
  DALI_TEST_EQUALS(inputField.GetAdjustedFontSizeScale(), 0.8f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test normal range with default scale 1.0
  inputField.SetMinimumFontSizeScale(0.5f);
  inputField.SetMaximumFontSizeScale(2.0f);
  DALI_TEST_EQUALS(inputField.GetAdjustedFontSizeScale(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputFieldFontVariation(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Set via axis API
  Dali::Vector<Text::FontVariation::Axis> axes;
  axes.PushBack(Text::FontVariation::Axis("wght", 700.0f));
  axes.PushBack(Text::FontVariation::Axis("wdth", 90.0f));

  inputField.SetFontVariation(axes);

  Dali::Vector<Text::FontVariation::Axis> result = inputField.GetFontVariation();

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

  // Empty string is invalid and should not clear or change the font variation.
  inputField.SetFontVariation("");

  result = inputField.GetFontVariation();

  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetValue(), 500.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetValue(), 80.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Clear via None()
  inputField.SetFontVariation(Text::FontVariation::None());

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
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TEXT_HANDLE_ENABLED) == InputField::Property::TEXT_HANDLE_ENABLED);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TEXT_HANDLE_COLOR) == InputField::Property::TEXT_HANDLE_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_HANDLE_IMAGE) == InputField::Property::CURSOR_HANDLE_IMAGE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_CURSOR_HANDLE_PRESSED_IMAGE) == InputField::Property::CURSOR_HANDLE_PRESSED_IMAGE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT) == InputField::Property::SELECTION_HANDLE_IMAGE_LEFT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT) == InputField::Property::SELECTION_HANDLE_IMAGE_RIGHT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT) == InputField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT) == InputField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_LENGTH) == InputField::Property::MAXIMUM_LENGTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_EDITABLE) == InputField::Property::EDITABLE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_LAYOUT_DIRECTION_MODE) == InputField::Property::LAYOUT_DIRECTION_MODE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_WEIGHT) == InputField::Property::FONT_WEIGHT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_WIDTH) == InputField::Property::FONT_WIDTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_FONT_SLANT) == InputField::Property::FONT_SLANT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TEXT_BACKGROUND_COLOR) == InputField::Property::TEXT_BACKGROUND_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE) == InputField::Property::MINIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE) == InputField::Property::MAXIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED) == InputField::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TYPING_TEXT_COLOR) == InputField::Property::TYPING_TEXT_COLOR);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_FAMILY) == InputField::Property::TYPING_FONT_FAMILY);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_SIZE) == InputField::Property::TYPING_FONT_SIZE);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_WEIGHT) == InputField::Property::TYPING_FONT_WEIGHT);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_WIDTH) == InputField::Property::TYPING_FONT_WIDTH);
  DALI_TEST_CHECK(inputField.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_SLANT) == InputField::Property::TYPING_FONT_SLANT);

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

  // TEXT_HANDLE_ENABLED
  inputField.SetProperty(InputField::Property::TEXT_HANDLE_ENABLED, true);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::TEXT_HANDLE_ENABLED), true, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::TEXT_HANDLE_ENABLED, false);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::TEXT_HANDLE_ENABLED), false, TEST_LOCATION);

  // TEXT_HANDLE_COLOR
  inputField.SetProperty(InputField::Property::TEXT_HANDLE_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::TEXT_HANDLE_COLOR), Color::BLUE, TEST_LOCATION);

  // CURSOR_HANDLE_IMAGE
  inputField.SetProperty(InputField::Property::CURSOR_HANDLE_IMAGE, "cursor-handle.png");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::CURSOR_HANDLE_IMAGE), std::string("cursor-handle.png"), TEST_LOCATION);

  // CURSOR_HANDLE_PRESSED_IMAGE
  inputField.SetProperty(InputField::Property::CURSOR_HANDLE_PRESSED_IMAGE, "cursor-handle-pressed.png");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::CURSOR_HANDLE_PRESSED_IMAGE), std::string("cursor-handle-pressed.png"), TEST_LOCATION);

  // SELECTION_HANDLE_IMAGE_LEFT
  inputField.SetProperty(InputField::Property::SELECTION_HANDLE_IMAGE_LEFT, "selection-left.png");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::SELECTION_HANDLE_IMAGE_LEFT), std::string("selection-left.png"), TEST_LOCATION);

  // SELECTION_HANDLE_IMAGE_RIGHT
  inputField.SetProperty(InputField::Property::SELECTION_HANDLE_IMAGE_RIGHT, "selection-right.png");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::SELECTION_HANDLE_IMAGE_RIGHT), std::string("selection-right.png"), TEST_LOCATION);

  // SELECTION_HANDLE_PRESSED_IMAGE_LEFT
  inputField.SetProperty(InputField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, "selection-left-pressed.png");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT), std::string("selection-left-pressed.png"), TEST_LOCATION);

  // SELECTION_HANDLE_PRESSED_IMAGE_RIGHT
  inputField.SetProperty(InputField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, "selection-right-pressed.png");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT), std::string("selection-right-pressed.png"), TEST_LOCATION);

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

  // MINIMUM_FONT_SIZE_SCALE
  inputField.SetProperty(InputField::Property::MINIMUM_FONT_SIZE_SCALE, 0.5f);
  DALI_TEST_EQUALS(inputField.GetProperty<float>(InputField::Property::MINIMUM_FONT_SIZE_SCALE), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // MAXIMUM_FONT_SIZE_SCALE
  inputField.SetProperty(InputField::Property::MAXIMUM_FONT_SIZE_SCALE, 2.0f);
  DALI_TEST_EQUALS(inputField.GetProperty<float>(InputField::Property::MAXIMUM_FONT_SIZE_SCALE), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // SYSTEM_FONT_SIZE_SCALE_ENABLED
  inputField.SetProperty(InputField::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED, true);
  DALI_TEST_EQUALS(inputField.GetProperty<bool>(InputField::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED), true, TEST_LOCATION);

  // TYPING_TEXT_COLOR
  inputField.SetProperty(InputField::Property::TYPING_TEXT_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputField.GetProperty<Vector4>(InputField::Property::TYPING_TEXT_COLOR), Color::BLUE, TEST_LOCATION);

  // TYPING_FONT_FAMILY
  inputField.SetProperty(InputField::Property::TYPING_FONT_FAMILY, "Arial");
  DALI_TEST_EQUALS(inputField.GetProperty<Dali::String>(InputField::Property::TYPING_FONT_FAMILY), std::string("Arial"), TEST_LOCATION);

  // TYPING_FONT_SIZE
  inputField.SetProperty(InputField::Property::TYPING_FONT_SIZE, 20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputField.GetProperty<float>(InputField::Property::TYPING_FONT_SIZE), 20.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // TYPING_FONT_WEIGHT
  inputField.SetProperty(InputField::Property::TYPING_FONT_WEIGHT, Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontWeight>(InputField::Property::TYPING_FONT_WEIGHT), Text::FontWeight::BOLD, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::TYPING_FONT_WEIGHT, "LIGHT");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontWeight>(InputField::Property::TYPING_FONT_WEIGHT), Text::FontWeight::LIGHT, TEST_LOCATION);

  // TYPING_FONT_WIDTH
  inputField.SetProperty(InputField::Property::TYPING_FONT_WIDTH, Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontWidth>(InputField::Property::TYPING_FONT_WIDTH), Text::FontWidth::EXPANDED, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::TYPING_FONT_WIDTH, "CONDENSED");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontWidth>(InputField::Property::TYPING_FONT_WIDTH), Text::FontWidth::CONDENSED, TEST_LOCATION);

  // TYPING_FONT_SLANT
  inputField.SetProperty(InputField::Property::TYPING_FONT_SLANT, Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontSlant>(InputField::Property::TYPING_FONT_SLANT), Text::FontSlant::ITALIC, TEST_LOCATION);

  inputField.SetProperty(InputField::Property::TYPING_FONT_SLANT, "OBLIQUE");
  DALI_TEST_EQUALS(inputField.GetProperty<Text::FontSlant>(InputField::Property::TYPING_FONT_SLANT), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  END_TEST;
}

// Localization test helpers for InputField
namespace
{

bool InputFieldLocalizationOverride(StringView resourceId, StringView domain, Dali::String& outString)
{
  const std::string rid = ToStdString(resourceId);
  const std::string dom = ToStdString(domain);

  if(rid == "IDS_INPUT_PLACEHOLDER")
  {
    if(dom == "domainA")
    {
      outString = "Placeholder A";
    }
    else if(dom == "domainB")
    {
      outString = "Placeholder B";
    }
    else
    {
      outString = "Placeholder Default";
    }
    return true;
  }

  return false;
}

void CleanupInputFieldLocalization(InputField& inputField)
{
  inputField.ClearTranslatablePlaceholder();
  UiLocalizationManager::Get().ClearLocalizedStringOverride();
  UiLocalizationManager::Get().SetBypassEnabled(false);
  UiLocalizationManager::Get().SetDefaultDomain("");
}

} // anonymous namespace

int UtcDaliInputFieldSetTranslatablePlaceholderDefaultDomainP(void)
{
  TestApplication application;
  InputField inputField = InputField::New();
  application.GetScene().Add(inputField);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputFieldLocalizationOverride);

  inputField.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER");
  DALI_TEST_EQUALS(inputField.GetTranslatablePlaceholder(), "IDS_INPUT_PLACEHOLDER", TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  CleanupInputFieldLocalization(inputField);
  END_TEST;
}

int UtcDaliInputFieldSetTranslatablePlaceholderExplicitDomainP(void)
{
  TestApplication application;
  InputField inputField = InputField::New();
  application.GetScene().Add(inputField);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputFieldLocalizationOverride);

  inputField.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER", "domainA");
  DALI_TEST_EQUALS(inputField.GetTranslatablePlaceholder(), "IDS_INPUT_PLACEHOLDER", TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder A", TEST_LOCATION);

  // Change default domain - explicit domain binding should remain
  locManager.SetDefaultDomain("domainB");
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder A", TEST_LOCATION);

  CleanupInputFieldLocalization(inputField);
  END_TEST;
}

int UtcDaliInputFieldSetTranslatablePlaceholderDefaultDomainAfterExplicitDomainP(void)
{
  TestApplication application;
  InputField inputField = InputField::New();
  application.GetScene().Add(inputField);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputFieldLocalizationOverride);

  // Set default domain to domainB
  locManager.SetDefaultDomain("domainB");

  // First, set with explicit domainA
  inputField.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER", "domainA");
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder A", TEST_LOCATION);

  // Now call SetTranslatablePlaceholder(resourceId) without domain.
  // This should use default domain (domainB), NOT reuse the previous explicit domainA.
  inputField.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER");
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder B", TEST_LOCATION);

  CleanupInputFieldLocalization(inputField);
  END_TEST;
}

int UtcDaliInputFieldClearTranslatablePlaceholderP(void)
{
  TestApplication application;
  InputField inputField = InputField::New();
  application.GetScene().Add(inputField);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputFieldLocalizationOverride);

  inputField.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER");
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  inputField.ClearTranslatablePlaceholder();
  // Current placeholder value is maintained after clear
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);
  DALI_TEST_EQUALS(inputField.GetTranslatablePlaceholder(), Dali::String(), TEST_LOCATION);

  // RefreshBindings should not change the placeholder after clear
  locManager.RefreshBindings();
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  CleanupInputFieldLocalization(inputField);
  END_TEST;
}

int UtcDaliInputFieldGetInputMethodContextP(void)
{
  UiTestApplication application;
  InputField inputField = InputField::New();
  DALI_TEST_CHECK(inputField);

  // Get the InputMethodContext - it should not be empty after control is created
  InputMethodContext context = inputField.GetInputMethodContext();
  DALI_TEST_CHECK(context);

  // Calling GetInputMethodContext() twice should return the same context handle
  InputMethodContext context2 = inputField.GetInputMethodContext();
  DALI_TEST_CHECK(context2);
  DALI_TEST_CHECK(context == context2);

  END_TEST;
}

int UtcDaliInputFieldSetPlaceholderDoesNotClearTranslatablePlaceholderP(void)
{
  TestApplication application;
  InputField inputField = InputField::New();
  application.GetScene().Add(inputField);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputFieldLocalizationOverride);

  inputField.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER");
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  inputField.SetPlaceholder("Manual Placeholder");
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Manual Placeholder", TEST_LOCATION);
  // TranslatablePlaceholder binding is still active
  DALI_TEST_EQUALS(inputField.GetTranslatablePlaceholder(), "IDS_INPUT_PLACEHOLDER", TEST_LOCATION);

  // RefreshBindings overwrites Placeholder with localized string
  locManager.RefreshBindings();
  DALI_TEST_EQUALS(inputField.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  CleanupInputFieldLocalization(inputField);
  END_TEST;
}

int UtcDaliInputFieldTextGradientDefaultsP(void)
{
  UiTestApplication application;
  InputField       field = InputField::New();

  DALI_TEST_EQUALS(field.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetPlaceholderTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInputFieldTextGradientSetGetCopyAndBoundsP(void)
{
  UiTestApplication application;
  InputField       field = InputField::New();

  Gradient::Linear source = MakeInputFieldGradient(Color::RED, Color::BLUE);
  field.SetTextGradient(source);
  field.SetTextGradient(source); // Identical authored value is a public no-op.
  ExpectInputFieldGradient(field.GetTextGradient(), Color::RED, Color::BLUE);

  source.SetStartAndEndPosition(Vector2(10.0f, 20.0f), Vector2(30.0f, 40.0f));
  source.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::GREEN)),
                       Gradient::StopNode(1.0f, UiColor(Color::YELLOW))});
  ExpectInputFieldGradient(field.GetTextGradient(), Color::RED, Color::BLUE);

  field.SetTextGradientBoundsMode(Text::GradientBoundsMode::VIEW_BOUND);
  DALI_TEST_EQUALS(field.GetTextGradientBoundsMode(), Text::GradientBoundsMode::VIEW_BOUND, TEST_LOCATION);
  field.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  DALI_TEST_EQUALS(field.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInputFieldTextGradientRadialConicAuthoredP(void)
{
  UiTestApplication application;
  InputField       field = InputField::New();

  field.SetTextGradient(MakeInputFieldRadialGradient());
  ExpectInputFieldRadialGradient(field.GetTextGradient());

  field.SetPlaceholderTextGradient(MakeInputFieldConicGradient());
  ExpectInputFieldConicGradient(field.GetPlaceholderTextGradient());
  END_TEST;
}

int UtcDaliInputFieldTextGradientNormalPlaceholderIndependentP(void)
{
  UiTestApplication application;
  InputField       field = InputField::New();

  field.SetTextGradient(MakeInputFieldGradient(Color::RED, Color::BLUE));
  field.SetPlaceholderTextGradient(MakeInputFieldGradient(Color::GREEN, Color::YELLOW));
  ExpectInputFieldGradient(field.GetTextGradient(), Color::RED, Color::BLUE);
  ExpectInputFieldGradient(field.GetPlaceholderTextGradient(), Color::GREEN, Color::YELLOW);

  field.SetTextGradient(Gradient::Base::None());
  DALI_TEST_EQUALS(field.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  ExpectInputFieldGradient(field.GetPlaceholderTextGradient(), Color::GREEN, Color::YELLOW);

  field.SetPlaceholderTextGradient(Gradient::Base::None());
  DALI_TEST_EQUALS(field.GetPlaceholderTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInputFieldTextGradientColorFallbackP(void)
{
  UiTestApplication application;
  InputField       field = InputField::New();

  field.SetTextColor(UiColor(Color::RED));
  field.SetTextGradient(MakeInputFieldGradient(Color::GREEN, Color::YELLOW));
  field.SetTextColor(UiColor(Color::BLUE));
  DALI_TEST_EQUALS(field.GetTextGradient().GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  field.SetTextGradient(Gradient::Base::None());
  DALI_TEST_EQUALS(field.GetTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  field.SetPlaceholderColor(UiColor(Color::CYAN));
  field.SetPlaceholderTextGradient(MakeInputFieldGradient(Color::RED, Color::BLUE));
  field.SetPlaceholderColor(UiColor(Color::MAGENTA));
  DALI_TEST_EQUALS(field.GetPlaceholderTextGradient().GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  field.SetPlaceholderTextGradient(Gradient::Base::None());
  DALI_TEST_EQUALS(field.GetPlaceholderColor().GetRgba(), Color::MAGENTA, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInputFieldTextGradientRejectInvalidP(void)
{
  UiTestApplication application;
  InputField       field = InputField::New();

  Gradient::Linear oneStop(Vector2::ZERO, Vector2::ONE);
  oneStop.SetStopNodes({Gradient::StopNode(0.5f, UiColor(Color::RED))});

  field.SetTextGradient(MakeInputFieldGradient(Color::RED, Color::BLUE));
  field.SetTextGradient(oneStop);
  DALI_TEST_EQUALS(field.GetTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);

  field.SetPlaceholderTextGradient(MakeInputFieldGradient(Color::GREEN, Color::YELLOW));
  field.SetPlaceholderTextGradient(oneStop);
  DALI_TEST_EQUALS(field.GetPlaceholderTextGradient().GetType(), Gradient::Type::NONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInputFieldTextGradientControlLifecycleP(void)
{
  TestApplication application;

  for(uint32_t iteration = 0u; iteration < 6u; ++iteration)
  {
    InputField field = InputField::New();
    DALI_TEST_CHECK(field);
    field.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 60.0f));
    field.SetPlaceholder("placeholder gradient");
    field.SetTextGradient(MakeInputFieldGradient(Color::RED, Color::BLUE));
    field.SetPlaceholderTextGradient(MakeInputFieldGradient(Color::GREEN, Color::YELLOW));
    field.SetTextGradientBoundsMode(Text::GradientBoundsMode::VIEW_BOUND);

    application.GetScene().Add(field);
    field.SetText("normal text that can scroll horizontally");
    field.SetTextGradient(MakeInputFieldGradient(Color::YELLOW, Color::CYAN));
    field.SetText("");
    field.SetTextGradient(Gradient::Base::None());
    field.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
    application.GetScene().Remove(field);
    field.Reset();
  }
  END_TEST;
}

int UtcDaliInputFieldTextGradientStartOffsetAnimationP(void)
{
  TestApplication application;
  InputField      field = InputField::New();

  Animation noGradientAnimation = Animation::New(0.1f);
  field.Animate(noGradientAnimation)
    .TextGradientStartOffset(0.75f, Duration(0.1f))
    .TextGradientStartOffsetBy(0.1f, Duration(0.1f))
    .PlaceholderTextGradientStartOffset(0.5f, Duration(0.1f))
    .PlaceholderTextGradientStartOffsetBy(0.1f, Duration(0.1f));
  DALI_TEST_EQUALS(field.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetPropertyIndex(PLACEHOLDER_TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  InputFieldAnimationSpec noGradientSpec = InputField::NewAnimationSpec();
  noGradientSpec.TextGradientStartOffset(0.25f, Duration(0.1f))
    .PlaceholderTextGradientStartOffset(0.35f, Duration(0.1f));
  noGradientSpec.ApplyTo(noGradientAnimation, field);
  DALI_TEST_EQUALS(field.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetPropertyIndex(PLACEHOLDER_TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  field.SetTextGradient(MakeInputFieldRadialGradient());
  field.SetPlaceholderTextGradient(MakeInputFieldConicGradient());

  Animation animation = Animation::New(0.1f);
  field.Animate(animation)
    .TextGradientStartOffset(0.75f, Duration(0.1f))
    .PlaceholderTextGradientStartOffset(0.5f, Duration(0.1f));

  const Property::Index normalIndex = field.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  const Property::Index placeholderIndex = field.GetPropertyIndex(PLACEHOLDER_TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(normalIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(placeholderIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(normalIndex != placeholderIndex);
  DALI_TEST_EQUALS(field.GetProperty<float>(normalIndex), 0.125f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetProperty<float>(placeholderIndex), -0.25f, EPSILON, TEST_LOCATION);

  InputFieldAnimationSpec spec = InputField::NewAnimationSpec();
  spec.TextGradientStartOffsetBy(0.05f, Duration(0.1f))
    .PlaceholderTextGradientStartOffsetBy(0.05f, Duration(0.1f));
  Animation specAnimation = Animation::New(0.1f);
  spec.ApplyTo(specAnimation, field);
  DALI_TEST_EQUALS(field.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), normalIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetPropertyIndex(PLACEHOLDER_TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), placeholderIndex, TEST_LOCATION);

  field.SetTextGradient(Gradient::Base::None());
  field.SetPlaceholderTextGradient(Gradient::Base::None());

  Animation afterClearAnimation = Animation::New(0.1f);
  field.Animate(afterClearAnimation)
    .TextGradientStartOffset(0.2f, Duration(0.1f))
    .PlaceholderTextGradientStartOffset(0.3f, Duration(0.1f));
  DALI_TEST_EQUALS(field.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), normalIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetPropertyIndex(PLACEHOLDER_TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), placeholderIndex, TEST_LOCATION);

  END_TEST;
}
