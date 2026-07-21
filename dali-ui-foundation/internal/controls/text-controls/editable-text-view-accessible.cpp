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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/editable-text-view-accessible.h>
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>

namespace Dali::Ui::Internal
{
void EditableTextViewAccessible::InitDefaultFeatures()
{
  TextViewAccessible::InitDefaultFeatures();
  AddFeature<Dali::Accessibility::EditableText>(SharedFromThis());
}

Dali::Integration::Accessibility::States EditableTextViewAccessible::CalculateStates() // LCOV_EXCL_LINE
{
  using Dali::Integration::Accessibility::State; // LCOV_EXCL_LINE

  auto states    = ViewAccessible::CalculateStates();
  auto focusView = Internal::KeyInputFocusManager::Get().GetCurrentFocusView();

  states[State::EDITABLE]  = true;
  states[State::FOCUSABLE] = true;
  states[State::FOCUSED]   = (Self() == focusView);
  return states;
}

std::size_t EditableTextViewAccessible::GetCursorOffset() const
{
  return GetTextController()->GetCursorPosition();
}

bool EditableTextViewAccessible::SetCursorOffset(std::size_t offset)
{
  if(offset > GetCharacterCount())
  {
    return false;
  }

  GetTextController()->ResetCursorPosition(offset);
  RequestTextRelayout();
  return true;
}

bool EditableTextViewAccessible::CopyText(std::size_t startPosition, std::size_t endPosition)
{
  if(IsHiddenInput())
  {
    return false;
  }

  auto        text      = GetWholeText();
  std::size_t utf8Start = 0u;
  std::size_t utf8End   = 0u;
  if(!ConvertToUtf8Range(text, startPosition, endPosition, utf8Start, utf8End))
  {
    return false;
  }

  GetTextController()->CopyStringToClipboard(text.substr(utf8Start, utf8End - utf8Start));
  return true;
}

bool EditableTextViewAccessible::CutText(std::size_t startPosition, std::size_t endPosition)
{
  if(!CopyText(startPosition, endPosition))
  {
    return false;
  }
  return DeleteText(startPosition, endPosition);
}

bool EditableTextViewAccessible::DeleteText(std::size_t startPosition, std::size_t endPosition)
{
  auto        text      = GetWholeText();
  std::size_t utf8Start = 0u;
  std::size_t utf8End   = 0u;
  if(!ConvertToUtf8Range(text, startPosition, endPosition, utf8Start, utf8End))
  {
    return false;
  }
  return SetTextContents(std::move(text.erase(utf8Start, utf8End - utf8Start)));
}

bool EditableTextViewAccessible::InsertText(std::size_t startPosition, std::string newText)
{
  auto        text       = GetWholeText();
  std::size_t utf8Offset = 0u;
  if(!ConvertToUtf8Offset(text, startPosition, utf8Offset))
  {
    return false;
  }
  return SetTextContents(std::move(text.insert(utf8Offset, newText)));
}

bool EditableTextViewAccessible::SetTextContents(std::string newContents)
{
  GetTextController()->SetText(std::move(newContents));
  return true;
}

bool EditableTextViewAccessible::SupportsSelection() const
{
  return true;
}

} // namespace Dali::Ui::Internal
