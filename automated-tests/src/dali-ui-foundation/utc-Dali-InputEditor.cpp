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

using Dali::Integration::ToStdString;

namespace
{
const char* const PROPERTY_NAME_TEXT                                 = "text";
const char* const PROPERTY_NAME_FONT_FAMILY                          = "fontFamily";
const char* const PROPERTY_NAME_FONT_SIZE                            = "fontSize";
const char* const PROPERTY_NAME_TEXT_COLOR                           = "textColor";
const char* const PROPERTY_NAME_LINE_WRAP_MODE                       = "lineWrapMode";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT                 = "horizontalAlignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT                   = "verticalAlignment";
const char* const PROPERTY_NAME_OVERFLOW_MODE                        = "overflowMode";
const char* const PROPERTY_NAME_LINE_HEIGHT                          = "lineHeight";
const char* const PROPERTY_NAME_LINE_HEIGHT_MODE                     = "lineHeightMode";
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
const char* const PROPERTY_NAME_EDITABLE                             = "editable";
const char* const PROPERTY_NAME_LAYOUT_DIRECTION_MODE                = "layoutDirectionMode";
const char* const PROPERTY_NAME_MARKUP_ENABLED                       = "markupEnabled";
const char* const PROPERTY_NAME_FONT_WEIGHT                          = "fontWeight";
const char* const PROPERTY_NAME_FONT_WIDTH                           = "fontWidth";
const char* const PROPERTY_NAME_FONT_SLANT                           = "fontSlant";
const char* const PROPERTY_NAME_TEXT_BACKGROUND_COLOR                = "textBackgroundColor";
const char* const PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE              = "minimumFontSizeScale";
const char* const PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE              = "maximumFontSizeScale";
const char* const PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED       = "systemFontSizeScaleEnabled";
const char* const PROPERTY_NAME_AUTO_GROW_ENABLED                    = "autoGrowEnabled";
const char* const PROPERTY_NAME_TYPING_TEXT_COLOR                    = "typingTextColor";
const char* const PROPERTY_NAME_TYPING_FONT_FAMILY                   = "typingFontFamily";
const char* const PROPERTY_NAME_TYPING_FONT_SIZE                     = "typingFontSize";
const char* const PROPERTY_NAME_TYPING_FONT_WEIGHT                   = "typingFontWeight";
const char* const PROPERTY_NAME_TYPING_FONT_WIDTH                    = "typingFontWidth";
const char* const PROPERTY_NAME_TYPING_FONT_SLANT                    = "typingFontSlant";

} // namespace

void utc_dali_input_editor_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_input_editor_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInputEditorConstructorP(void)
{
  UiTestApplication application;
  InputEditor inputEditor;
  DALI_TEST_CHECK(!inputEditor);
  END_TEST;
}

int UtcDaliInputEditorNewP(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);
  END_TEST;
}

int UtcDaliInputEditorCopyConstructorP(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  InputEditor copy(inputEditor);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(inputEditor == copy);
  END_TEST;
}

int UtcDaliInputEditorMoveConstructor(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_EQUALS(1, inputEditor.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  InputEditor moved = std::move(inputEditor);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!inputEditor);
  END_TEST;
}

int UtcDaliInputEditorAssignmentOperatorP(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  InputEditor copy;
  copy = inputEditor;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(inputEditor == copy);
  END_TEST;
}

int UtcDaliInputEditorMoveAssignment(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_EQUALS(1, inputEditor.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  InputEditor moved;
  moved = std::move(inputEditor);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!inputEditor);
  END_TEST;
}

int UtcDaliInputEditorDownCastP(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  BaseHandle object(inputEditor);
  InputEditor inputEditor2 = InputEditor::DownCast(object);
  InputEditor inputEditor3 = DownCast<InputEditor>(object);
  DALI_TEST_CHECK(inputEditor2);
  DALI_TEST_CHECK(inputEditor3);
  END_TEST;
}

int UtcDaliInputEditorDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitializedObject;
  InputEditor inputEditor1 = InputEditor::DownCast(unInitializedObject);
  InputEditor inputEditor2 = DownCast<InputEditor>(unInitializedObject);
  DALI_TEST_CHECK(!inputEditor1);
  DALI_TEST_CHECK(!inputEditor2);
  END_TEST;
}

// Setter, Getter

int UtcDaliInputEditorText(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetText("Hello world");
  DALI_TEST_EQUALS(inputEditor.GetText(), std::string("Hello world"), TEST_LOCATION);

  inputEditor.SetText("Updated text");
  DALI_TEST_EQUALS(inputEditor.GetText(), std::string("Updated text"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorFontFamily(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetFontFamily("Arial");
  DALI_TEST_EQUALS(inputEditor.GetFontFamily(), std::string("Arial"), TEST_LOCATION);

  inputEditor.SetFontFamily("Roboto");
  DALI_TEST_EQUALS(inputEditor.GetFontFamily(), std::string("Roboto"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorFontSize(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetFontSize(20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputEditor.GetFontSize(), 20.0f, TEST_LOCATION);

  inputEditor.SetFontSize(32.5f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputEditor.GetFontSize(), 32.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTextColor(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  UiColor color(Color::BLUE);
  inputEditor.SetTextColor(color);
  DALI_TEST_EQUALS(inputEditor.GetTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  inputEditor.SetTextColor(color2);
  DALI_TEST_EQUALS(inputEditor.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorLineWrapMode(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetLineWrapMode(Text::LineWrapMode::WORD);
  DALI_TEST_EQUALS(inputEditor.GetLineWrapMode(), Text::LineWrapMode::WORD, TEST_LOCATION);

  inputEditor.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  DALI_TEST_EQUALS(inputEditor.GetLineWrapMode(), Text::LineWrapMode::CHARACTER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorHorizontalTextAlignment(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(inputEditor.GetHorizontalTextAlignment(), Text::Alignment::CENTER, TEST_LOCATION);

  inputEditor.SetHorizontalTextAlignment(Text::Alignment::END);
  DALI_TEST_EQUALS(inputEditor.GetHorizontalTextAlignment(), Text::Alignment::END, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorVerticalTextAlignment(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetVerticalTextAlignment(Text::Alignment::CENTER);
  DALI_TEST_EQUALS(inputEditor.GetVerticalTextAlignment(), Text::Alignment::CENTER, TEST_LOCATION);

  inputEditor.SetVerticalTextAlignment(Text::Alignment::END);
  DALI_TEST_EQUALS(inputEditor.GetVerticalTextAlignment(), Text::Alignment::END, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorOverflowMode(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(inputEditor.GetTextOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  inputEditor.SetTextOverflowMode(Text::OverflowMode::CLIP);
  DALI_TEST_EQUALS(inputEditor.GetTextOverflowMode(), Text::OverflowMode::CLIP, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorLineHeight(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetLineHeight(1.5f);
  DALI_TEST_EQUALS(inputEditor.GetLineHeight(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputEditor.SetLineHeight(24.0f);
  DALI_TEST_EQUALS(inputEditor.GetLineHeight(), 24.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorLineHeightMode(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
  DALI_TEST_EQUALS(inputEditor.GetLineHeightMode(), Text::LineHeightMode::RELATIVE, TEST_LOCATION);

  inputEditor.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
  DALI_TEST_EQUALS(inputEditor.GetLineHeightMode(), Text::LineHeightMode::ABSOLUTE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorPlaceholder(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetPlaceholder("Enter text");
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), std::string("Enter text"), TEST_LOCATION);

  inputEditor.SetPlaceholder("Type here");
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), std::string("Type here"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorPlaceholderColor(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  UiColor color(Color::GRAY);
  inputEditor.SetPlaceholderColor(color);
  DALI_TEST_EQUALS(inputEditor.GetPlaceholderColor().GetRgba(), Color::GRAY, TEST_LOCATION);

  UiColor color2(Color::BLUE);
  inputEditor.SetPlaceholderColor(color2);
  DALI_TEST_EQUALS(inputEditor.GetPlaceholderColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorShowPlaceholderOnFocus(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetShowPlaceholderOnFocus(true);
  DALI_TEST_EQUALS(inputEditor.IsPlaceholderShownOnFocus(), true, TEST_LOCATION);

  inputEditor.SetShowPlaceholderOnFocus(false);
  DALI_TEST_EQUALS(inputEditor.IsPlaceholderShownOnFocus(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorCursorWidth(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetCursorWidth(2);
  DALI_TEST_EQUALS(inputEditor.GetCursorWidth(), 2, TEST_LOCATION);

  inputEditor.SetCursorWidth(4);
  DALI_TEST_EQUALS(inputEditor.GetCursorWidth(), 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorCursorColor(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  UiColor color(Color::BLUE);
  inputEditor.SetCursorColor(color);
  DALI_TEST_EQUALS(inputEditor.GetCursorColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  inputEditor.SetCursorColor(color2);
  DALI_TEST_EQUALS(inputEditor.GetCursorColor().GetRgba(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorCursorBlinkEnabled(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetCursorBlinkEnabled(true);
  DALI_TEST_EQUALS(inputEditor.IsCursorBlinkEnabled(), true, TEST_LOCATION);

  inputEditor.SetCursorBlinkEnabled(false);
  DALI_TEST_EQUALS(inputEditor.IsCursorBlinkEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorCursorBlinkInterval(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetCursorBlinkInterval(0.5f);
  DALI_TEST_EQUALS(inputEditor.GetCursorBlinkInterval(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputEditor.SetCursorBlinkInterval(1.0f);
  DALI_TEST_EQUALS(inputEditor.GetCursorBlinkInterval(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorCursorPosition(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Empty text: cursor position should be clamped to 0.
  inputEditor.SetCursorPosition(5u);
  DALI_TEST_EQUALS(inputEditor.GetCursorPosition(), 0u, TEST_LOCATION);

  Dali::String text = "Hello world";
  inputEditor.SetText(text);

  // Clamp to the end when the requested position exceeds text length.
  inputEditor.SetCursorPosition(50u);
  DALI_TEST_EQUALS(inputEditor.GetCursorPosition(), text.Size(), TEST_LOCATION);

  inputEditor.SetCursorPosition(text.Size());
  DALI_TEST_EQUALS(inputEditor.GetCursorPosition(), text.Size(), TEST_LOCATION);

  inputEditor.SetCursorPosition(5u);
  DALI_TEST_EQUALS(inputEditor.GetCursorPosition(), 5u, TEST_LOCATION);

  inputEditor.SetCursorPosition(0u);
  DALI_TEST_EQUALS(inputEditor.GetCursorPosition(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorSelectionEnabled(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Default should be true
  DALI_TEST_EQUALS(inputEditor.IsSelectionEnabled(), true, TEST_LOCATION);

  inputEditor.SetSelectionEnabled(false);
  DALI_TEST_EQUALS(inputEditor.IsSelectionEnabled(), false, TEST_LOCATION);

  inputEditor.SetSelectionEnabled(true);
  DALI_TEST_EQUALS(inputEditor.IsSelectionEnabled(), true, TEST_LOCATION);

  // Test setter
  inputEditor.SetSelectionEnabled(false);

  END_TEST;
}

int UtcDaliInputEditorSelectionColor(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  UiColor color(Color::CYAN);
  inputEditor.SetSelectionColor(color);
  DALI_TEST_EQUALS(inputEditor.GetSelectionColor().GetRgba(), Color::CYAN, TEST_LOCATION);

  UiColor color2(Color::MAGENTA);
  inputEditor.SetSelectionColor(color2);
  DALI_TEST_EQUALS(inputEditor.GetSelectionColor().GetRgba(), Color::MAGENTA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTextHandle(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Test SetTextHandleEnabled
  inputEditor.SetTextHandleEnabled(true);
  DALI_TEST_EQUALS(inputEditor.IsTextHandleEnabled(), true, TEST_LOCATION);

  inputEditor.SetTextHandleEnabled(false);
  DALI_TEST_EQUALS(inputEditor.IsTextHandleEnabled(), false, TEST_LOCATION);

  // Test setter for SetTextHandleEnabled
  inputEditor.SetTextHandleEnabled(true);

  // Test SetTextHandleColor
  UiColor handleColor(Color::BLUE);
  inputEditor.SetTextHandleColor(handleColor);
  DALI_TEST_EQUALS(inputEditor.GetTextHandleColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor handleColor2(Color::RED);
  inputEditor.SetTextHandleColor(handleColor2);
  DALI_TEST_EQUALS(inputEditor.GetTextHandleColor().GetRgba(), Color::RED, TEST_LOCATION);

  // Test setter for SetTextHandleColor
  inputEditor.SetTextHandleColor(Color::GREEN);

  END_TEST;
}

int UtcDaliInputEditorHandleImages(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Test cursor handle image
  inputEditor.SetCursorHandleImage("cursor-handle.png");
  DALI_TEST_EQUALS(inputEditor.GetCursorHandleImage(), std::string("cursor-handle.png"), TEST_LOCATION);

  inputEditor.SetCursorHandleImage("cursor-handle-2.png");
  DALI_TEST_EQUALS(inputEditor.GetCursorHandleImage(), std::string("cursor-handle-2.png"), TEST_LOCATION);

  // Test cursor handle pressed image
  inputEditor.SetCursorHandlePressedImage("cursor-handle-pressed.png");
  DALI_TEST_EQUALS(inputEditor.GetCursorHandlePressedImage(), std::string("cursor-handle-pressed.png"), TEST_LOCATION);

  // Test selection handle images
  inputEditor.SetSelectionHandleImageLeft("selection-left.png");
  DALI_TEST_EQUALS(inputEditor.GetSelectionHandleImageLeft(), std::string("selection-left.png"), TEST_LOCATION);

  inputEditor.SetSelectionHandleImageRight("selection-right.png");
  DALI_TEST_EQUALS(inputEditor.GetSelectionHandleImageRight(), std::string("selection-right.png"), TEST_LOCATION);

  inputEditor.SetSelectionHandlePressedImageLeft("selection-left-pressed.png");
  DALI_TEST_EQUALS(inputEditor.GetSelectionHandlePressedImageLeft(), std::string("selection-left-pressed.png"), TEST_LOCATION);

  inputEditor.SetSelectionHandlePressedImageRight("selection-right-pressed.png");
  DALI_TEST_EQUALS(inputEditor.GetSelectionHandlePressedImageRight(), std::string("selection-right-pressed.png"), TEST_LOCATION);

  // Test setter for a couple of setters
  inputEditor.SetCursorHandleImage("new-cursor.png");

  inputEditor.SetSelectionHandleImageLeft("new-left.png");

  END_TEST;
}

int UtcDaliInputEditorMaximumLength(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetMaximumLength(10);
  DALI_TEST_EQUALS(inputEditor.GetMaximumLength(), 10, TEST_LOCATION);

  inputEditor.SetMaximumLength(100);
  DALI_TEST_EQUALS(inputEditor.GetMaximumLength(), 100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorEditable(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Default should be true
  DALI_TEST_CHECK(inputEditor.IsEditable());

  inputEditor.SetEditable(false);
  DALI_TEST_EQUALS(inputEditor.IsEditable(), false, TEST_LOCATION);

  inputEditor.SetEditable(true);
  DALI_TEST_EQUALS(inputEditor.IsEditable(), true, TEST_LOCATION);

  // Test setter
  inputEditor.SetEditable(false);

  END_TEST;
}

int UtcDaliInputEditorLayoutDirectionMode(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  DALI_TEST_EQUALS(inputEditor.GetLayoutDirectionMode(), Text::LayoutDirectionMode::LOCALE, TEST_LOCATION);

  inputEditor.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  DALI_TEST_EQUALS(inputEditor.GetLayoutDirectionMode(), Text::LayoutDirectionMode::CONTENTS, TEST_LOCATION);

  END_TEST;
}


int UtcDaliInputEditorMarkupEnabled(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Default value should be false
  DALI_TEST_EQUALS(inputEditor.IsMarkupEnabled(), false, TEST_LOCATION);

  // Test SetMarkupEnabled with true
  inputEditor.SetMarkupEnabled(true);
  DALI_TEST_EQUALS(inputEditor.IsMarkupEnabled(), true, TEST_LOCATION);

  // Test SetMarkupEnabled with false
  inputEditor.SetMarkupEnabled(false);
  DALI_TEST_EQUALS(inputEditor.IsMarkupEnabled(), false, TEST_LOCATION);

  // Test setter
  inputEditor.SetMarkupEnabled(true);

  END_TEST;
}

int UtcDaliInputEditorFontWeight(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetFontWeight(Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(inputEditor.GetFontWeight(), Text::FontWeight::BOLD, TEST_LOCATION);

  inputEditor.SetFontWeight(Text::FontWeight::LIGHT);
  DALI_TEST_EQUALS(inputEditor.GetFontWeight(), Text::FontWeight::LIGHT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorFontWidth(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetFontWidth(Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(inputEditor.GetFontWidth(), Text::FontWidth::EXPANDED, TEST_LOCATION);

  inputEditor.SetFontWidth(Text::FontWidth::CONDENSED);
  DALI_TEST_EQUALS(inputEditor.GetFontWidth(), Text::FontWidth::CONDENSED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorFontSlant(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetFontSlant(Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(inputEditor.GetFontSlant(), Text::FontSlant::ITALIC, TEST_LOCATION);

  inputEditor.SetFontSlant(Text::FontSlant::OBLIQUE);
  DALI_TEST_EQUALS(inputEditor.GetFontSlant(), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTextBackgroundColor(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  UiColor color(Color::YELLOW);
  inputEditor.SetTextBackgroundColor(color);
  DALI_TEST_EQUALS(inputEditor.GetTextBackgroundColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  UiColor color2(Color::GREEN);
  inputEditor.SetTextBackgroundColor(color2);
  DALI_TEST_EQUALS(inputEditor.GetTextBackgroundColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  // Clear text background color
  inputEditor.ClearTextBackgroundColor();
  DALI_TEST_EQUALS(inputEditor.GetTextBackgroundColor().GetRgba(), Color::TRANSPARENT, TEST_LOCATION);

  // Set again after clear
  inputEditor.SetTextBackgroundColor(Color::BLUE);
  DALI_TEST_EQUALS(inputEditor.GetTextBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorMinimumFontSizeScale(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetMinimumFontSizeScale(0.5f);
  DALI_TEST_EQUALS(inputEditor.GetMinimumFontSizeScale(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputEditor.SetMinimumFontSizeScale(0.8f);
  DALI_TEST_EQUALS(inputEditor.GetMinimumFontSizeScale(), 0.8f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorMaximumFontSizeScale(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetMaximumFontSizeScale(2.0f);
  DALI_TEST_EQUALS(inputEditor.GetMaximumFontSizeScale(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputEditor.SetMaximumFontSizeScale(1.5f);
  DALI_TEST_EQUALS(inputEditor.GetMaximumFontSizeScale(), 1.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorSystemFontSizeScaleEnabled(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetSystemFontSizeScaleEnabled(true);
  DALI_TEST_EQUALS(inputEditor.IsSystemFontSizeScaleEnabled(), true, TEST_LOCATION);

  inputEditor.SetSystemFontSizeScaleEnabled(false);
  DALI_TEST_EQUALS(inputEditor.IsSystemFontSizeScaleEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorSetGetAutoGrowEnabled(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Default value should be false
  DALI_TEST_EQUALS(inputEditor.IsAutoGrowEnabled(), false, TEST_LOCATION);

  // Test SetAutoGrowEnabled with true
  inputEditor.SetAutoGrowEnabled(true);
  DALI_TEST_EQUALS(inputEditor.IsAutoGrowEnabled(), true, TEST_LOCATION);

  // Test SetAutoGrowEnabled with false
  inputEditor.SetAutoGrowEnabled(false);
  DALI_TEST_EQUALS(inputEditor.IsAutoGrowEnabled(), false, TEST_LOCATION);

  // Test setter
  inputEditor.SetAutoGrowEnabled(true);

  END_TEST;
}

int UtcDaliInputEditorAutoGrowEnabledProperty(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Default value should be false
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::AUTO_GROW_ENABLED), false, TEST_LOCATION);

  // Test setting via property
  inputEditor.SetProperty(InputEditor::Property::AUTO_GROW_ENABLED, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::AUTO_GROW_ENABLED), true, TEST_LOCATION);
  DALI_TEST_EQUALS(inputEditor.IsAutoGrowEnabled(), true, TEST_LOCATION);

  // Test setting via property string name
  inputEditor.SetProperty(PROPERTY_NAME_AUTO_GROW_ENABLED, false);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::AUTO_GROW_ENABLED), false, TEST_LOCATION);
  DALI_TEST_EQUALS(inputEditor.IsAutoGrowEnabled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTypingTextColor(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  UiColor color(Color::BLUE);
  inputEditor.SetTypingTextColor(color);
  DALI_TEST_EQUALS(inputEditor.GetTypingTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  UiColor color2(Color::RED);
  inputEditor.SetTypingTextColor(color2);
  DALI_TEST_EQUALS(inputEditor.GetTypingTextColor().GetRgba(), Color::RED, TEST_LOCATION);

  // Test setter
  inputEditor.SetTypingTextColor(Color::GREEN);
  DALI_TEST_EQUALS(inputEditor.GetTypingTextColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTypingFontFamily(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetTypingFontFamily("Arial");
  DALI_TEST_EQUALS(inputEditor.GetTypingFontFamily(), std::string("Arial"), TEST_LOCATION);

  inputEditor.SetTypingFontFamily("Roboto");
  DALI_TEST_EQUALS(inputEditor.GetTypingFontFamily(), std::string("Roboto"), TEST_LOCATION);

  // Test setter
  inputEditor.SetTypingFontFamily("DejaVu Sans");
  DALI_TEST_EQUALS(inputEditor.GetTypingFontFamily(), std::string("DejaVu Sans"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTypingFontSize(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetTypingFontSize(20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputEditor.GetTypingFontSize(), 20.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  inputEditor.SetTypingFontSize(32.5f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputEditor.GetTypingFontSize(), 32.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test setter
  inputEditor.SetTypingFontSize(28.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputEditor.GetTypingFontSize(), 28.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTypingFontWeight(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetTypingFontWeight(Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontWeight(), Text::FontWeight::BOLD, TEST_LOCATION);

  inputEditor.SetTypingFontWeight(Text::FontWeight::LIGHT);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontWeight(), Text::FontWeight::LIGHT, TEST_LOCATION);

  // Test setter
  inputEditor.SetTypingFontWeight(Text::FontWeight::MEDIUM);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontWeight(), Text::FontWeight::MEDIUM, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTypingFontWidth(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetTypingFontWidth(Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontWidth(), Text::FontWidth::EXPANDED, TEST_LOCATION);

  inputEditor.SetTypingFontWidth(Text::FontWidth::CONDENSED);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontWidth(), Text::FontWidth::CONDENSED, TEST_LOCATION);

  // Test setter
  inputEditor.SetTypingFontWidth(Text::FontWidth::NORMAL);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontWidth(), Text::FontWidth::NORMAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorTypingFontSlant(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  inputEditor.SetTypingFontSlant(Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontSlant(), Text::FontSlant::ITALIC, TEST_LOCATION);

  inputEditor.SetTypingFontSlant(Text::FontSlant::OBLIQUE);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontSlant(), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  // Test setter
  inputEditor.SetTypingFontSlant(Text::FontSlant::NORMAL);
  DALI_TEST_EQUALS(inputEditor.GetTypingFontSlant(), Text::FontSlant::NORMAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorAdjustedFontSizeScale(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Test clamping to minimum from default scale 1.0
  inputEditor.SetMinimumFontSizeScale(1.2f);
  inputEditor.SetMaximumFontSizeScale(2.0f);
  inputEditor.SetSystemFontSizeScaleEnabled(false);
  DALI_TEST_EQUALS(inputEditor.GetAdjustedFontSizeScale(), 1.2f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test clamping to maximum from default scale 1.0
  inputEditor.SetMinimumFontSizeScale(0.1f);
  inputEditor.SetMaximumFontSizeScale(0.8f);
  DALI_TEST_EQUALS(inputEditor.GetAdjustedFontSizeScale(), 0.8f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test normal range with default scale 1.0
  inputEditor.SetMinimumFontSizeScale(0.5f);
  inputEditor.SetMaximumFontSizeScale(2.0f);
  DALI_TEST_EQUALS(inputEditor.GetAdjustedFontSizeScale(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInputEditorFontVariation(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Set via axis API
  Dali::Vector<Text::FontVariationAxis> axes;
  axes.PushBack(Text::FontVariationAxis("wght", 700.0f));
  axes.PushBack(Text::FontVariationAxis("wdth", 90.0f));

  inputEditor.SetFontVariation(axes);

  Dali::Vector<Text::FontVariationAxis> result = inputEditor.GetFontVariation();

  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetValue(), 700.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetValue(), 90.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Set via string API
  inputEditor.SetFontVariation("wght=500,wdth=80");

  result = inputEditor.GetFontVariation();

  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetValue(), 500.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetValue(), 80.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Clear
  inputEditor.ClearFontVariation();

  result = inputEditor.GetFontVariation();
  DALI_TEST_EQUALS(result.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

// Property
int UtcDaliInputEditorGetProperty(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Check Property Indices are correct
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TEXT) == InputEditor::Property::TEXT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_FONT_FAMILY) == InputEditor::Property::FONT_FAMILY);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_FONT_SIZE) == InputEditor::Property::FONT_SIZE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TEXT_COLOR) == InputEditor::Property::TEXT_COLOR);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_LINE_WRAP_MODE) == InputEditor::Property::LINE_WRAP_MODE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_HORIZONTAL_ALIGNMENT) == InputEditor::Property::HORIZONTAL_ALIGNMENT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_VERTICAL_ALIGNMENT) == InputEditor::Property::VERTICAL_ALIGNMENT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_OVERFLOW_MODE) == InputEditor::Property::OVERFLOW_MODE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_LINE_HEIGHT) == InputEditor::Property::LINE_HEIGHT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_LINE_HEIGHT_MODE) == InputEditor::Property::LINE_HEIGHT_MODE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_PLACEHOLDER) == InputEditor::Property::PLACEHOLDER);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_PLACEHOLDER_COLOR) == InputEditor::Property::PLACEHOLDER_COLOR);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SHOW_PLACEHOLDER_ON_FOCUS) == InputEditor::Property::SHOW_PLACEHOLDER_ON_FOCUS);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_CURSOR_WIDTH) == InputEditor::Property::CURSOR_WIDTH);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_CURSOR_COLOR) == InputEditor::Property::CURSOR_COLOR);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_CURSOR_BLINK_ENABLED) == InputEditor::Property::CURSOR_BLINK_ENABLED);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_CURSOR_BLINK_INTERVAL) == InputEditor::Property::CURSOR_BLINK_INTERVAL);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_CURSOR_POSITION) == InputEditor::Property::CURSOR_POSITION);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTION_ENABLED) == InputEditor::Property::SELECTION_ENABLED);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTION_COLOR) == InputEditor::Property::SELECTION_COLOR);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTED_TEXT) == InputEditor::Property::SELECTED_TEXT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTED_TEXT_START) == InputEditor::Property::SELECTED_TEXT_START);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTED_TEXT_END) == InputEditor::Property::SELECTED_TEXT_END);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TEXT_HANDLE_ENABLED) == InputEditor::Property::TEXT_HANDLE_ENABLED);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TEXT_HANDLE_COLOR) == InputEditor::Property::TEXT_HANDLE_COLOR);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_CURSOR_HANDLE_IMAGE) == InputEditor::Property::CURSOR_HANDLE_IMAGE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_CURSOR_HANDLE_PRESSED_IMAGE) == InputEditor::Property::CURSOR_HANDLE_PRESSED_IMAGE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT) == InputEditor::Property::SELECTION_HANDLE_IMAGE_LEFT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT) == InputEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT) == InputEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT) == InputEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_LENGTH) == InputEditor::Property::MAXIMUM_LENGTH);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_EDITABLE) == InputEditor::Property::EDITABLE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_LAYOUT_DIRECTION_MODE) == InputEditor::Property::LAYOUT_DIRECTION_MODE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_MARKUP_ENABLED) == InputEditor::Property::MARKUP_ENABLED);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_FONT_WEIGHT) == InputEditor::Property::FONT_WEIGHT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_FONT_WIDTH) == InputEditor::Property::FONT_WIDTH);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_FONT_SLANT) == InputEditor::Property::FONT_SLANT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TEXT_BACKGROUND_COLOR) == InputEditor::Property::TEXT_BACKGROUND_COLOR);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_MINIMUM_FONT_SIZE_SCALE) == InputEditor::Property::MINIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_MAXIMUM_FONT_SIZE_SCALE) == InputEditor::Property::MAXIMUM_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_SYSTEM_FONT_SIZE_SCALE_ENABLED) == InputEditor::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_AUTO_GROW_ENABLED) == InputEditor::Property::AUTO_GROW_ENABLED);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TYPING_TEXT_COLOR) == InputEditor::Property::TYPING_TEXT_COLOR);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_FAMILY) == InputEditor::Property::TYPING_FONT_FAMILY);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_SIZE) == InputEditor::Property::TYPING_FONT_SIZE);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_WEIGHT) == InputEditor::Property::TYPING_FONT_WEIGHT);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_WIDTH) == InputEditor::Property::TYPING_FONT_WIDTH);
  DALI_TEST_CHECK(inputEditor.GetPropertyIndex(PROPERTY_NAME_TYPING_FONT_SLANT) == InputEditor::Property::TYPING_FONT_SLANT);

  END_TEST;
}

int UtcDaliInputEditorSetProperty(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // TEXT
  inputEditor.SetProperty(InputEditor::Property::TEXT, "Hello world");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::TEXT), std::string("Hello world"), TEST_LOCATION);

  // FONT_FAMILY
  inputEditor.SetProperty(InputEditor::Property::FONT_FAMILY, "Arial");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::FONT_FAMILY), std::string("Arial"), TEST_LOCATION);

  // FONT_SIZE
  inputEditor.SetProperty(InputEditor::Property::FONT_SIZE, 20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputEditor.GetProperty<float>(InputEditor::Property::FONT_SIZE), 20.0f, TEST_LOCATION);

  // TEXT_COLOR
  inputEditor.SetProperty(InputEditor::Property::TEXT_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Vector4>(InputEditor::Property::TEXT_COLOR), Color::BLUE, TEST_LOCATION);

  // LINE_WRAP_MODE
  inputEditor.SetProperty(InputEditor::Property::LINE_WRAP_MODE, Text::LineWrapMode::WORD);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::LineWrapMode>(InputEditor::Property::LINE_WRAP_MODE), Text::LineWrapMode::WORD, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::LINE_WRAP_MODE, "CHARACTER");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::LineWrapMode>(InputEditor::Property::LINE_WRAP_MODE), Text::LineWrapMode::CHARACTER, TEST_LOCATION);

  // HORIZONTAL_ALIGNMENT
  inputEditor.SetProperty(InputEditor::Property::HORIZONTAL_ALIGNMENT, Text::Alignment::CENTER);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::Alignment>(InputEditor::Property::HORIZONTAL_ALIGNMENT), Text::Alignment::CENTER, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::HORIZONTAL_ALIGNMENT, "END");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::Alignment>(InputEditor::Property::HORIZONTAL_ALIGNMENT), Text::Alignment::END, TEST_LOCATION);

  // VERTICAL_ALIGNMENT
  inputEditor.SetProperty(InputEditor::Property::VERTICAL_ALIGNMENT, Text::Alignment::CENTER);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::Alignment>(InputEditor::Property::VERTICAL_ALIGNMENT), Text::Alignment::CENTER, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::VERTICAL_ALIGNMENT, "END");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::Alignment>(InputEditor::Property::VERTICAL_ALIGNMENT), Text::Alignment::END, TEST_LOCATION);

  // OVERFLOW_MODE
  inputEditor.SetProperty(InputEditor::Property::OVERFLOW_MODE, Text::OverflowMode::ELLIPSIS);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::OverflowMode>(InputEditor::Property::OVERFLOW_MODE), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::OVERFLOW_MODE, "CLIP");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::OverflowMode>(InputEditor::Property::OVERFLOW_MODE), Text::OverflowMode::CLIP, TEST_LOCATION);

  // LINE_HEIGHT
  inputEditor.SetProperty(InputEditor::Property::LINE_HEIGHT, 1.5f);
  DALI_TEST_EQUALS(inputEditor.GetProperty<float>(InputEditor::Property::LINE_HEIGHT), 1.5f, TEST_LOCATION);

  // LINE_HEIGHT_MODE
  inputEditor.SetProperty(InputEditor::Property::LINE_HEIGHT_MODE, Text::LineHeightMode::ABSOLUTE);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::LineHeightMode>(InputEditor::Property::LINE_HEIGHT_MODE), Text::LineHeightMode::ABSOLUTE, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::LINE_HEIGHT_MODE, "RELATIVE");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::LineHeightMode>(InputEditor::Property::LINE_HEIGHT_MODE), Text::LineHeightMode::RELATIVE, TEST_LOCATION);

  // PLACEHOLDER
  inputEditor.SetProperty(InputEditor::Property::PLACEHOLDER, "Enter text");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::PLACEHOLDER), std::string("Enter text"), TEST_LOCATION);

  // PLACEHOLDER_COLOR
  inputEditor.SetProperty(InputEditor::Property::PLACEHOLDER_COLOR, Color::GRAY);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Vector4>(InputEditor::Property::PLACEHOLDER_COLOR), Color::GRAY, TEST_LOCATION);

  // SHOW_PLACEHOLDER_ON_FOCUS
  inputEditor.SetProperty(InputEditor::Property::SHOW_PLACEHOLDER_ON_FOCUS, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::SHOW_PLACEHOLDER_ON_FOCUS), true, TEST_LOCATION);

  // CURSOR_WIDTH
  inputEditor.SetProperty(InputEditor::Property::CURSOR_WIDTH, 2);
  DALI_TEST_EQUALS(inputEditor.GetProperty<int>(InputEditor::Property::CURSOR_WIDTH), 2, TEST_LOCATION);

  // CURSOR_COLOR
  inputEditor.SetProperty(InputEditor::Property::CURSOR_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Vector4>(InputEditor::Property::CURSOR_COLOR), Color::BLUE, TEST_LOCATION);

  // CURSOR_BLINK_ENABLED
  inputEditor.SetProperty(InputEditor::Property::CURSOR_BLINK_ENABLED, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::CURSOR_BLINK_ENABLED), true, TEST_LOCATION);

  // CURSOR_BLINK_INTERVAL
  inputEditor.SetProperty(InputEditor::Property::CURSOR_BLINK_INTERVAL, 0.5f);
  DALI_TEST_EQUALS(inputEditor.GetProperty<float>(InputEditor::Property::CURSOR_BLINK_INTERVAL), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // CURSOR_POSITION
  inputEditor.SetProperty(InputEditor::Property::CURSOR_POSITION, 5);
  DALI_TEST_EQUALS(inputEditor.GetProperty<int>(InputEditor::Property::CURSOR_POSITION), 5, TEST_LOCATION);

  // SELECTION_ENABLED
  inputEditor.SetProperty(InputEditor::Property::SELECTION_ENABLED, false);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::SELECTION_ENABLED), false, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::SELECTION_ENABLED, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::SELECTION_ENABLED), true, TEST_LOCATION);

  // SELECTION_COLOR
  inputEditor.SetProperty(InputEditor::Property::SELECTION_COLOR, Color::CYAN);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Vector4>(InputEditor::Property::SELECTION_COLOR), Color::CYAN, TEST_LOCATION);

  // SELECTED_TEXT (read-only)
  // Get selected text returns empty string if no selection
  DALI_TEST_CHECK(inputEditor.GetProperty<Dali::String>(InputEditor::Property::SELECTED_TEXT).Size() >= 0u);

  // SELECTED_TEXT_START (read-only)
  DALI_TEST_CHECK(inputEditor.GetProperty<int>(InputEditor::Property::SELECTED_TEXT_START) >= 0);

  // SELECTED_TEXT_END (read-only)
  DALI_TEST_CHECK(inputEditor.GetProperty<int>(InputEditor::Property::SELECTED_TEXT_END) >= 0);

  // TEXT_HANDLE_ENABLED
  inputEditor.SetProperty(InputEditor::Property::TEXT_HANDLE_ENABLED, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::TEXT_HANDLE_ENABLED), true, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::TEXT_HANDLE_ENABLED, false);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::TEXT_HANDLE_ENABLED), false, TEST_LOCATION);

  // TEXT_HANDLE_COLOR
  inputEditor.SetProperty(InputEditor::Property::TEXT_HANDLE_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Vector4>(InputEditor::Property::TEXT_HANDLE_COLOR), Color::BLUE, TEST_LOCATION);

  // CURSOR_HANDLE_IMAGE
  inputEditor.SetProperty(InputEditor::Property::CURSOR_HANDLE_IMAGE, "cursor-handle.png");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::CURSOR_HANDLE_IMAGE), std::string("cursor-handle.png"), TEST_LOCATION);

  // CURSOR_HANDLE_PRESSED_IMAGE
  inputEditor.SetProperty(InputEditor::Property::CURSOR_HANDLE_PRESSED_IMAGE, "cursor-handle-pressed.png");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::CURSOR_HANDLE_PRESSED_IMAGE), std::string("cursor-handle-pressed.png"), TEST_LOCATION);

  // SELECTION_HANDLE_IMAGE_LEFT
  inputEditor.SetProperty(InputEditor::Property::SELECTION_HANDLE_IMAGE_LEFT, "selection-left.png");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::SELECTION_HANDLE_IMAGE_LEFT), std::string("selection-left.png"), TEST_LOCATION);

  // SELECTION_HANDLE_IMAGE_RIGHT
  inputEditor.SetProperty(InputEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT, "selection-right.png");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT), std::string("selection-right.png"), TEST_LOCATION);

  // SELECTION_HANDLE_PRESSED_IMAGE_LEFT
  inputEditor.SetProperty(InputEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, "selection-left-pressed.png");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT), std::string("selection-left-pressed.png"), TEST_LOCATION);

  // SELECTION_HANDLE_PRESSED_IMAGE_RIGHT
  inputEditor.SetProperty(InputEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, "selection-right-pressed.png");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT), std::string("selection-right-pressed.png"), TEST_LOCATION);

  // MAXIMUM_LENGTH
  inputEditor.SetProperty(InputEditor::Property::MAXIMUM_LENGTH, 50);
  DALI_TEST_EQUALS(inputEditor.GetProperty<int>(InputEditor::Property::MAXIMUM_LENGTH), 50, TEST_LOCATION);

  // EDITABLE
  inputEditor.SetProperty(InputEditor::Property::EDITABLE, false);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::EDITABLE), false, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::EDITABLE, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::EDITABLE), true, TEST_LOCATION);

  // LAYOUT_DIRECTION_MODE
  inputEditor.SetProperty(InputEditor::Property::LAYOUT_DIRECTION_MODE, Text::LayoutDirectionMode::LOCALE);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::LayoutDirectionMode>(InputEditor::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::LOCALE, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::LAYOUT_DIRECTION_MODE, "CONTENTS");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::LayoutDirectionMode>(InputEditor::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::CONTENTS, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::LAYOUT_DIRECTION_MODE, "INHERIT");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::LayoutDirectionMode>(InputEditor::Property::LAYOUT_DIRECTION_MODE), Text::LayoutDirectionMode::INHERIT, TEST_LOCATION);

  // MARKUP_ENABLED
  inputEditor.SetProperty(InputEditor::Property::MARKUP_ENABLED, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::MARKUP_ENABLED), true, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::MARKUP_ENABLED, false);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::MARKUP_ENABLED), false, TEST_LOCATION);

  // FONT_WEIGHT
  inputEditor.SetProperty(InputEditor::Property::FONT_WEIGHT, Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontWeight>(InputEditor::Property::FONT_WEIGHT), Text::FontWeight::BOLD, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::FONT_WEIGHT, "LIGHT");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontWeight>(InputEditor::Property::FONT_WEIGHT), Text::FontWeight::LIGHT, TEST_LOCATION);

  // FONT_WIDTH
  inputEditor.SetProperty(InputEditor::Property::FONT_WIDTH, Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontWidth>(InputEditor::Property::FONT_WIDTH), Text::FontWidth::EXPANDED, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::FONT_WIDTH, "CONDENSED");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontWidth>(InputEditor::Property::FONT_WIDTH), Text::FontWidth::CONDENSED, TEST_LOCATION);

  // FONT_SLANT
  inputEditor.SetProperty(InputEditor::Property::FONT_SLANT, Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontSlant>(InputEditor::Property::FONT_SLANT), Text::FontSlant::ITALIC, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::FONT_SLANT, "OBLIQUE");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontSlant>(InputEditor::Property::FONT_SLANT), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  // TEXT_BACKGROUND_COLOR
  inputEditor.SetProperty(InputEditor::Property::TEXT_BACKGROUND_COLOR, Color::YELLOW);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Vector4>(InputEditor::Property::TEXT_BACKGROUND_COLOR), Color::YELLOW, TEST_LOCATION);

  // MINIMUM_FONT_SIZE_SCALE
  inputEditor.SetProperty(InputEditor::Property::MINIMUM_FONT_SIZE_SCALE, 0.5f);
  DALI_TEST_EQUALS(inputEditor.GetProperty<float>(InputEditor::Property::MINIMUM_FONT_SIZE_SCALE), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // MAXIMUM_FONT_SIZE_SCALE
  inputEditor.SetProperty(InputEditor::Property::MAXIMUM_FONT_SIZE_SCALE, 2.0f);
  DALI_TEST_EQUALS(inputEditor.GetProperty<float>(InputEditor::Property::MAXIMUM_FONT_SIZE_SCALE), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // SYSTEM_FONT_SIZE_SCALE_ENABLED
  inputEditor.SetProperty(InputEditor::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::SYSTEM_FONT_SIZE_SCALE_ENABLED), true, TEST_LOCATION);

  // AUTO_GROW_ENABLED
  inputEditor.SetProperty(InputEditor::Property::AUTO_GROW_ENABLED, true);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::AUTO_GROW_ENABLED), true, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::AUTO_GROW_ENABLED, false);
  DALI_TEST_EQUALS(inputEditor.GetProperty<bool>(InputEditor::Property::AUTO_GROW_ENABLED), false, TEST_LOCATION);

  // TYPING_TEXT_COLOR
  inputEditor.SetProperty(InputEditor::Property::TYPING_TEXT_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Vector4>(InputEditor::Property::TYPING_TEXT_COLOR), Color::BLUE, TEST_LOCATION);

  // TYPING_FONT_FAMILY
  inputEditor.SetProperty(InputEditor::Property::TYPING_FONT_FAMILY, "Arial");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Dali::String>(InputEditor::Property::TYPING_FONT_FAMILY), std::string("Arial"), TEST_LOCATION);

  // TYPING_FONT_SIZE
  inputEditor.SetProperty(InputEditor::Property::TYPING_FONT_SIZE, 20.0f);
  // TODO: Enable once UTC provides a font client for FONT_SIZE property resolution.
  // DALI_TEST_EQUALS(inputEditor.GetProperty<float>(InputEditor::Property::TYPING_FONT_SIZE), 20.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // TYPING_FONT_WEIGHT
  inputEditor.SetProperty(InputEditor::Property::TYPING_FONT_WEIGHT, Text::FontWeight::BOLD);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontWeight>(InputEditor::Property::TYPING_FONT_WEIGHT), Text::FontWeight::BOLD, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::TYPING_FONT_WEIGHT, "LIGHT");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontWeight>(InputEditor::Property::TYPING_FONT_WEIGHT), Text::FontWeight::LIGHT, TEST_LOCATION);

  // TYPING_FONT_WIDTH
  inputEditor.SetProperty(InputEditor::Property::TYPING_FONT_WIDTH, Text::FontWidth::EXPANDED);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontWidth>(InputEditor::Property::TYPING_FONT_WIDTH), Text::FontWidth::EXPANDED, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::TYPING_FONT_WIDTH, "CONDENSED");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontWidth>(InputEditor::Property::TYPING_FONT_WIDTH), Text::FontWidth::CONDENSED, TEST_LOCATION);

  // TYPING_FONT_SLANT
  inputEditor.SetProperty(InputEditor::Property::TYPING_FONT_SLANT, Text::FontSlant::ITALIC);
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontSlant>(InputEditor::Property::TYPING_FONT_SLANT), Text::FontSlant::ITALIC, TEST_LOCATION);

  inputEditor.SetProperty(InputEditor::Property::TYPING_FONT_SLANT, "OBLIQUE");
  DALI_TEST_EQUALS(inputEditor.GetProperty<Text::FontSlant>(InputEditor::Property::TYPING_FONT_SLANT), Text::FontSlant::OBLIQUE, TEST_LOCATION);

  END_TEST;
}

// Localization test helpers for InputEditor
namespace
{

bool InputEditorLocalizationOverride(StringView resourceId, StringView domain, Dali::String& outString)
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

void CleanupInputEditorLocalization(InputEditor& inputEditor)
{
  inputEditor.ClearTranslatablePlaceholder();
  UiLocalizationManager::Get().ClearLocalizedStringOverride();
  UiLocalizationManager::Get().SetBypassEnabled(false);
  UiLocalizationManager::Get().SetDefaultDomain("");
}

} // anonymous namespace

int UtcDaliInputEditorSetTranslatablePlaceholderDefaultDomainP(void)
{
  TestApplication application;
  InputEditor inputEditor = InputEditor::New();
  application.GetScene().Add(inputEditor);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputEditorLocalizationOverride);

  inputEditor.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER");
  DALI_TEST_EQUALS(inputEditor.GetTranslatablePlaceholder(), "IDS_INPUT_PLACEHOLDER", TEST_LOCATION);
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  CleanupInputEditorLocalization(inputEditor);
  END_TEST;
}

int UtcDaliInputEditorSetTranslatablePlaceholderExplicitDomainP(void)
{
  TestApplication application;
  InputEditor inputEditor = InputEditor::New();
  application.GetScene().Add(inputEditor);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputEditorLocalizationOverride);

  inputEditor.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER", "domainA");
  DALI_TEST_EQUALS(inputEditor.GetTranslatablePlaceholder(), "IDS_INPUT_PLACEHOLDER", TEST_LOCATION);
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder A", TEST_LOCATION);

  // Change default domain - explicit domain binding should remain
  locManager.SetDefaultDomain("domainB");
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder A", TEST_LOCATION);

  CleanupInputEditorLocalization(inputEditor);
  END_TEST;
}

int UtcDaliInputEditorSetTranslatablePlaceholderDefaultDomainAfterExplicitDomainP(void)
{
  TestApplication application;
  InputEditor inputEditor = InputEditor::New();
  application.GetScene().Add(inputEditor);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputEditorLocalizationOverride);

  // Set default domain to domainB
  locManager.SetDefaultDomain("domainB");

  // First, set with explicit domainA
  inputEditor.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER", "domainA");
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder A", TEST_LOCATION);

  // Now call SetTranslatablePlaceholder(resourceId) without domain.
  // This should use default domain (domainB), NOT reuse the previous explicit domainA.
  inputEditor.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER");
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder B", TEST_LOCATION);

  CleanupInputEditorLocalization(inputEditor);
  END_TEST;
}

int UtcDaliInputEditorClearTranslatablePlaceholderP(void)
{
  TestApplication application;
  InputEditor inputEditor = InputEditor::New();
  application.GetScene().Add(inputEditor);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputEditorLocalizationOverride);

  inputEditor.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER");
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  inputEditor.ClearTranslatablePlaceholder();
  // Current placeholder value is maintained after clear
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);
  DALI_TEST_EQUALS(inputEditor.GetTranslatablePlaceholder(), Dali::String(), TEST_LOCATION);

  // RefreshBindings should not change the placeholder after clear
  locManager.RefreshBindings();
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  CleanupInputEditorLocalization(inputEditor);
  END_TEST;
}

int UtcDaliInputEditorGetInputMethodContextP(void)
{
  UiTestApplication application;
  InputEditor inputEditor = InputEditor::New();
  DALI_TEST_CHECK(inputEditor);

  // Get the InputMethodContext - it should not be empty after control is created
  InputMethodContext context = inputEditor.GetInputMethodContext();
  DALI_TEST_CHECK(context);

  // Calling GetInputMethodContext() twice should return the same context handle
  InputMethodContext context2 = inputEditor.GetInputMethodContext();
  DALI_TEST_CHECK(context2);
  DALI_TEST_CHECK(context == context2);

  END_TEST;
}

int UtcDaliInputEditorSetPlaceholderDoesNotClearTranslatablePlaceholderP(void)
{
  TestApplication application;
  InputEditor inputEditor = InputEditor::New();
  application.GetScene().Add(inputEditor);

  UiLocalizationManager locManager = UiLocalizationManager::Get();
  locManager.SetLocalizedStringOverride(&InputEditorLocalizationOverride);

  inputEditor.SetTranslatablePlaceholder("IDS_INPUT_PLACEHOLDER");
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  inputEditor.SetPlaceholder("Manual Placeholder");
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Manual Placeholder", TEST_LOCATION);
  // TranslatablePlaceholder binding is still active
  DALI_TEST_EQUALS(inputEditor.GetTranslatablePlaceholder(), "IDS_INPUT_PLACEHOLDER", TEST_LOCATION);

  // RefreshBindings overwrites Placeholder with localized string
  locManager.RefreshBindings();
  DALI_TEST_EQUALS(inputEditor.GetPlaceholder(), "Placeholder Default", TEST_LOCATION);

  CleanupInputEditorLocalization(inputEditor);
  END_TEST;
}
