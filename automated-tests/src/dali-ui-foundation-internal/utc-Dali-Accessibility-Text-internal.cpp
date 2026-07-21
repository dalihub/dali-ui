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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/atspi-interfaces/editable-text.h>
#include <dali/devel-api/atspi-interfaces/hypertext.h>
#include <dali/devel-api/atspi-interfaces/text.h>
#include <limits>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_accessibility_text_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_accessibility_text_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAccessibilityTextControlsInterfacesInternalP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetText("label text");

  auto* labelAccessible = Dali::Accessibility::Accessible::Get(label);
  DALI_TEST_CHECK(labelAccessible);
  DALI_TEST_EQUALS(labelAccessible->GetRole(), Dali::Integration::Accessibility::Role::LABEL, TEST_LOCATION);
  DALI_TEST_EQUALS(labelAccessible->GetName(), "label text", TEST_LOCATION);

  auto* labelText      = dynamic_cast<Dali::Accessibility::Text*>(labelAccessible);
  auto* labelHypertext = dynamic_cast<Dali::Accessibility::Hypertext*>(labelAccessible);
  DALI_TEST_CHECK(labelText);
  DALI_TEST_CHECK(labelHypertext);
  DALI_TEST_CHECK(dynamic_cast<Dali::Accessibility::EditableText*>(labelAccessible) == nullptr);
  DALI_TEST_EQUALS(labelText->GetText(0u, 5u), "label", TEST_LOCATION);
  DALI_TEST_EQUALS(labelHypertext->GetLinkCount(), 0, TEST_LOCATION);

  auto labelInterfaces = labelAccessible->GetInterfaces();
  DALI_TEST_CHECK(labelInterfaces[Dali::Integration::Accessibility::AccessibilityInterface::TEXT]);
  DALI_TEST_CHECK(labelInterfaces[Dali::Integration::Accessibility::AccessibilityInterface::HYPERTEXT]);
  DALI_TEST_CHECK(!labelInterfaces[Dali::Integration::Accessibility::AccessibilityInterface::EDITABLE_TEXT]);

  InputField field = InputField::New();
  field.SetText("field text");

  auto* fieldAccessible = Dali::Accessibility::Accessible::Get(field);
  DALI_TEST_CHECK(fieldAccessible);
  DALI_TEST_EQUALS(fieldAccessible->GetRole(), Dali::Integration::Accessibility::Role::ENTRY, TEST_LOCATION);
  DALI_TEST_CHECK(dynamic_cast<Dali::Accessibility::Text*>(fieldAccessible));
  DALI_TEST_CHECK(dynamic_cast<Dali::Accessibility::EditableText*>(fieldAccessible));
  DALI_TEST_CHECK(dynamic_cast<Dali::Accessibility::Hypertext*>(fieldAccessible));

  auto fieldInterfaces = fieldAccessible->GetInterfaces();
  DALI_TEST_CHECK(fieldInterfaces[Dali::Integration::Accessibility::AccessibilityInterface::TEXT]);
  DALI_TEST_CHECK(fieldInterfaces[Dali::Integration::Accessibility::AccessibilityInterface::HYPERTEXT]);
  DALI_TEST_CHECK(fieldInterfaces[Dali::Integration::Accessibility::AccessibilityInterface::EDITABLE_TEXT]);

  InputEditor editor = InputEditor::New();
  editor.SetText("editor text");

  auto* editorAccessible = Dali::Accessibility::Accessible::Get(editor);
  DALI_TEST_CHECK(editorAccessible);
  DALI_TEST_EQUALS(editorAccessible->GetRole(), Dali::Integration::Accessibility::Role::ENTRY, TEST_LOCATION);
  DALI_TEST_CHECK(dynamic_cast<Dali::Accessibility::Text*>(editorAccessible));
  DALI_TEST_CHECK(dynamic_cast<Dali::Accessibility::EditableText*>(editorAccessible));
  DALI_TEST_CHECK(dynamic_cast<Dali::Accessibility::Hypertext*>(editorAccessible));

  END_TEST;
}

int UtcDaliAccessibilityTextUnicodeCharacterOffsetsInternalP(void)
{
  UiTestApplication application;

  InputEditor editor     = InputEditor::New();
  auto*       accessible = Dali::Accessibility::Accessible::Get(editor);
  auto*       text       = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  DALI_TEST_CHECK(text);

  const std::string utf8Text = u8"가a😀";
  editor.SetText(Dali::String(utf8Text.c_str()));

  DALI_TEST_EQUALS(text->GetCharacterCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 1u), u8"가", TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(1u, 3u), u8"a😀", TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 4u), "", TEST_LOCATION);

  auto range = text->GetTextAtOffset(2u, Dali::Devel::Accessibility::TextBoundary::CHARACTER);
  DALI_TEST_EQUALS(range.content, u8"😀", TEST_LOCATION);
  DALI_TEST_EQUALS(range.startOffset, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(range.endOffset, 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityTextUnicodeRangeGeometryInternalP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetMultiLine(true);
  label.SetFontSize(20.0f);
  label.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
  label.SetLineHeight(1.0f);
  label.SetText(u8"가a😀\n나b");
  label.SetRequestedWidth(240.0f);
  label.SetRequestedHeight(240.0f);
  label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  label.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(label);

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  application.SendNotification();
  application.Render();

  auto* accessible = Dali::Accessibility::Accessible::Get(label);
  auto* text       = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  DALI_TEST_CHECK(text);

  auto empty = text->GetRangeExtents(0u, 0u, Dali::Devel::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(empty, Bounds(0.0f, 0.0f, 0.0f, 0.0f), TEST_LOCATION);

  auto emoji = text->GetRangeExtents(2u, 3u, Dali::Devel::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_CHECK(emoji.width > 0.0f);
  DALI_TEST_CHECK(emoji.height > 0.0f);

  auto whole = text->GetRangeExtents(0u, text->GetCharacterCount(),
                                     Dali::Devel::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_CHECK(whole.width > 0.0f && whole.width <= 240.0f);
  DALI_TEST_CHECK(whole.height > 0.0f && whole.height <= 240.0f);

  label.SetLineHeight(2.0f);
  application.SendNotification();
  application.Render();

  auto wholeAfter = text->GetRangeExtents(0u, text->GetCharacterCount(),
                                          Dali::Devel::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_CHECK(wholeAfter.height > whole.height);

  END_TEST;
}

int UtcDaliAccessibilityTextRangeGeometryLayoutOffsetInternalP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetFontSize(20.0f);
  label.SetHorizontalTextAlignment(Text::Alignment::START);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetText("geometry");
  label.SetRequestedWidth(240.0f);
  label.SetRequestedHeight(160.0f);
  label.SetProperty(View::Property::PADDING, Vector4(24.0f, 8.0f, 12.0f, 4.0f));
  label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  label.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(label);

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  application.SendNotification();
  application.Render();

  auto* accessible = Dali::Accessibility::Accessible::Get(label);
  auto* text       = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  DALI_TEST_CHECK(text);

  auto actorExtents = accessible->GetExtents(Dali::Devel::Accessibility::CoordinateType::WINDOW);
  auto textExtents  = text->GetRangeExtents(0u, text->GetCharacterCount(),
                                            Dali::Devel::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_CHECK(textExtents.x >= actorExtents.x + 24.0f);
  DALI_TEST_CHECK(textExtents.y > actorExtents.y + 12.0f);
  DALI_TEST_CHECK(textExtents.x + textExtents.width <= actorExtents.x + actorExtents.width - 8.0f);
  DALI_TEST_CHECK(textExtents.y + textExtents.height <= actorExtents.y + actorExtents.height - 4.0f);

  auto actorScreenExtents = accessible->GetExtents(Dali::Devel::Accessibility::CoordinateType::SCREEN);
  auto textScreenExtents  = text->GetRangeExtents(0u, text->GetCharacterCount(),
                                                  Dali::Devel::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(textScreenExtents.x - actorScreenExtents.x,
                   textExtents.x - actorExtents.x,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(textScreenExtents.y - actorScreenExtents.y,
                   textExtents.y - actorExtents.y,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);

  label.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
  application.SendNotification();
  application.Render();

  actorExtents = accessible->GetExtents(Dali::Devel::Accessibility::CoordinateType::WINDOW);
  textExtents  = text->GetRangeExtents(0u, text->GetCharacterCount(),
                                       Dali::Devel::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_CHECK(textExtents.x >= actorExtents.x + 8.0f);
  DALI_TEST_CHECK(textExtents.x + textExtents.width <= actorExtents.x + actorExtents.width - 24.0f);

  END_TEST;
}

int UtcDaliAccessibilityEditableTextToolkitCompatibilityInternalP(void)
{
  UiTestApplication application;

  InputField field = InputField::New();
  field.SetText("abcdef");

  auto* accessible   = Dali::Accessibility::Accessible::Get(field);
  auto* text         = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  auto* editableText = dynamic_cast<Dali::Accessibility::EditableText*>(accessible);
  DALI_TEST_CHECK(text);
  DALI_TEST_CHECK(editableText);

  DALI_TEST_EQUALS(text->SetCursorOffset(100u), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetCursorOffset(2u), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetCursorOffset(), 2u, TEST_LOCATION);

  DALI_TEST_EQUALS(editableText->DeleteText(5u, 1u), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editableText->DeleteText(1u, 5u), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 2u), "af", TEST_LOCATION);
  DALI_TEST_EQUALS(editableText->InsertText(1u, "bcde"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 6u), "abcdef", TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityEditableTextUnicodeCharacterOffsetsInternalP(void)
{
  UiTestApplication application;

  InputField field = InputField::New();
  field.SetText(u8"가a😀나");

  auto* accessible   = Dali::Accessibility::Accessible::Get(field);
  auto* text         = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  auto* editableText = dynamic_cast<Dali::Accessibility::EditableText*>(accessible);
  DALI_TEST_CHECK(text);
  DALI_TEST_CHECK(editableText);

  DALI_TEST_EQUALS(text->GetCharacterCount(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetCursorOffset(4u), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetCursorOffset(), 4u, TEST_LOCATION);

  DALI_TEST_EQUALS(text->SetRangeOfSelection(0u, 1u, 3u), true, TEST_LOCATION);
  auto selection = text->GetRangeOfSelection(0u);
  DALI_TEST_EQUALS(selection.startOffset, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(selection.endOffset, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(selection.content, u8"a😀", TEST_LOCATION);

  DALI_TEST_EQUALS(text->SetRangeOfSelection(1u, 1u, 3u), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetRangeOfSelection(0u, 3u, 1u), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetRangeOfSelection(0u, 0u, 5u), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetRangeOfSelection(0u, std::numeric_limits<std::size_t>::max(),
                                             std::numeric_limits<std::size_t>::max()),
                   false,
                   TEST_LOCATION);

  selection = text->GetRangeOfSelection(0u);
  DALI_TEST_EQUALS(selection.startOffset, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(selection.endOffset, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(selection.content, u8"a😀", TEST_LOCATION);

  DALI_TEST_EQUALS(editableText->DeleteText(1u, 3u), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 2u), u8"가나", TEST_LOCATION);
  DALI_TEST_EQUALS(editableText->InsertText(1u, u8"a😀"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 4u), u8"가a😀나", TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityInputFieldPasswordRoleInternalP(void)
{
  UiTestApplication application;

  InputField field = InputField::New();
  field.SetText("secret");

  auto* accessible = Dali::Accessibility::Accessible::Get(field);
  auto* text       = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_CHECK(text);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::ENTRY, TEST_LOCATION);

  field.SetPasswordMode(Text::PasswordMode::HIDE_ALL);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::PASSWORD_TEXT, TEST_LOCATION);
  DALI_TEST_CHECK(text->GetText(0u, 6u) != "secret");
  DALI_TEST_EQUALS(text->GetTextAtOffset(0u, Dali::Devel::Accessibility::TextBoundary::CHARACTER).content,
                   "",
                   TEST_LOCATION);

  field.SetText(u8"가😀");
  DALI_TEST_EQUALS(text->GetCharacterCount(), 2u, TEST_LOCATION);
  auto maskedText = text->GetText(0u, 2u);
  DALI_TEST_EQUALS(Text::Utf8ToUtf32Length(Dali::StringView(maskedText.c_str())), 2u, TEST_LOCATION);

  field.SetPasswordMode(Text::PasswordMode::NONE);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::ENTRY, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 2u), u8"가😀", TEST_LOCATION);

  field.SetAccessibilityRole(Ui::Accessibility::Role::CHECK_BOX);
  field.SetPasswordMode(Text::PasswordMode::HIDE_ALL);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::CHECK_BOX, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityInputFieldPasswordClipboardProtectionInternalP(void)
{
  UiTestApplication application;

  InputField field = InputField::New();
  field.SetText("secret");
  field.SetPasswordMode(Text::PasswordMode::HIDE_ALL);

  auto* accessible   = Dali::Accessibility::Accessible::Get(field);
  auto* editableText = dynamic_cast<Dali::Accessibility::EditableText*>(accessible);
  DALI_TEST_CHECK(editableText);

  DALI_TEST_EQUALS(editableText->CopyText(0u, 6u), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editableText->CutText(0u, 6u), false, TEST_LOCATION);

  field.SetPasswordMode(Text::PasswordMode::NONE);
  DALI_TEST_EQUALS(field.GetText(), "secret", TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityEditableTextReadOnlyInternalP(void)
{
  UiTestApplication application;

  InputField field = InputField::New();
  field.SetText("field");
  field.SetEditable(false);

  auto* fieldAccessible = Dali::Accessibility::Accessible::Get(field);
  auto* fieldText       = dynamic_cast<Dali::Accessibility::Text*>(fieldAccessible);
  auto* fieldEditable   = dynamic_cast<Dali::Accessibility::EditableText*>(fieldAccessible);
  DALI_TEST_CHECK(fieldText);
  DALI_TEST_CHECK(fieldEditable);

  auto states = fieldAccessible->GetStates();
  DALI_TEST_CHECK(!states[Dali::Integration::Accessibility::State::EDITABLE]);
  DALI_TEST_CHECK(states[Dali::Integration::Accessibility::State::READ_ONLY]);
  DALI_TEST_EQUALS(fieldText->SetCursorOffset(2u), true, TEST_LOCATION);
  DALI_TEST_EQUALS(fieldEditable->CutText(0u, 1u), false, TEST_LOCATION);
  DALI_TEST_EQUALS(fieldEditable->DeleteText(0u, 1u), false, TEST_LOCATION);
  DALI_TEST_EQUALS(fieldEditable->InsertText(0u, "x"), false, TEST_LOCATION);
  DALI_TEST_EQUALS(fieldEditable->SetTextContents("changed"), false, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetText(), "field", TEST_LOCATION);

  field.SetEditable(true);
  states = fieldAccessible->GetStates();
  DALI_TEST_CHECK(states[Dali::Integration::Accessibility::State::EDITABLE]);
  DALI_TEST_CHECK(!states[Dali::Integration::Accessibility::State::READ_ONLY]);
  DALI_TEST_EQUALS(fieldEditable->InsertText(0u, "x"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(field.GetText(), "xfield", TEST_LOCATION);

  InputEditor editor = InputEditor::New();
  editor.SetText("editor");
  editor.SetEditable(false);

  auto* editorAccessible = Dali::Accessibility::Accessible::Get(editor);
  auto* editorEditable   = dynamic_cast<Dali::Accessibility::EditableText*>(editorAccessible);
  DALI_TEST_CHECK(editorEditable);
  states = editorAccessible->GetStates();
  DALI_TEST_CHECK(!states[Dali::Integration::Accessibility::State::EDITABLE]);
  DALI_TEST_CHECK(states[Dali::Integration::Accessibility::State::READ_ONLY]);
  DALI_TEST_EQUALS(editorEditable->SetTextContents("changed"), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editor.GetText(), "editor", TEST_LOCATION);

  END_TEST;
}
