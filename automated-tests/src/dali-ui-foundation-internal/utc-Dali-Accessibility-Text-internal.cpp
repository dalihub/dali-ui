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

int UtcDaliAccessibilityTextToolkitOffsetCompatibilityInternalP(void)
{
  UiTestApplication application;

  InputEditor editor     = InputEditor::New();
  auto*       accessible = Dali::Accessibility::Accessible::Get(editor);
  auto*       text       = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  DALI_TEST_CHECK(text);

  const std::string utf8Text = u8"가a";
  editor.SetText(Dali::String(utf8Text.c_str()));

  // Preserve dali-toolkit compatibility: Text offsets and character count use UTF-8 bytes.
  DALI_TEST_EQUALS(text->GetCharacterCount(), utf8Text.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 3u), u8"가", TEST_LOCATION);
  auto range = text->GetTextAtOffset(3u, Dali::Devel::Accessibility::TextBoundary::CHARACTER);
  DALI_TEST_EQUALS(range.content, "a", TEST_LOCATION);
  DALI_TEST_EQUALS(range.startOffset, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(range.endOffset, 4u, TEST_LOCATION);

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

  field.SetPasswordMode(Text::PasswordMode::NONE);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::ENTRY, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0u, 6u), "secret", TEST_LOCATION);

  field.SetAccessibilityRole(Ui::Accessibility::Role::CHECK_BOX);
  field.SetPasswordMode(Text::PasswordMode::HIDE_ALL);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::CHECK_BOX, TEST_LOCATION);

  END_TEST;
}
