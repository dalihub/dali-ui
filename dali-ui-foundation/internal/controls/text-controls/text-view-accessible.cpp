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

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <algorithm>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/text-view-accessible.h>
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/hidden-text.h>
#include <dali-ui-foundation/public-api/text/text-utils.h>

namespace Dali::Ui::Internal
{
bool TextViewAccessible::ConvertToUtf8Range(const std::string& string, std::size_t begin, std::size_t end,
                                            std::size_t& utf8Begin, std::size_t& utf8End)
{
  if(end <= begin || string.size() > std::numeric_limits<std::uint32_t>::max() ||
     begin > std::numeric_limits<std::uint32_t>::max() || end > std::numeric_limits<std::uint32_t>::max())
  {
    return false;
  }

  std::uint32_t utf8Begin32 = 0u;
  std::uint32_t utf8End32   = 0u;
  if(!Ui::Text::Utf32ToUtf8Range(Dali::StringView(string.data(), static_cast<std::uint32_t>(string.size())),
                                 static_cast<std::uint32_t>(begin),
                                 static_cast<std::uint32_t>(end),
                                 utf8Begin32,
                                 utf8End32))
  {
    return false;
  }

  utf8Begin = utf8Begin32;
  utf8End   = utf8End32;
  return true;
}

bool TextViewAccessible::ConvertToUtf8Offset(const std::string& string, std::size_t offset,
                                             std::size_t& utf8Offset)
{
  if(string.size() > std::numeric_limits<std::uint32_t>::max() ||
     offset > std::numeric_limits<std::uint32_t>::max())
  {
    return false;
  }

  std::uint32_t utf8Offset32 = 0u;
  if(!Ui::Text::Utf32ToUtf8Index(Dali::StringView(string.data(), static_cast<std::uint32_t>(string.size())),
                                 static_cast<std::uint32_t>(offset),
                                 utf8Offset32))
  {
    return false;
  }

  utf8Offset = utf8Offset32;
  return true;
}

void TextViewAccessible::InitDefaultFeatures()
{
  ViewAccessible::InitDefaultFeatures();
  AddFeature<Dali::Accessibility::Text>(SharedFromThis());
  AddFeature<Dali::Accessibility::Hypertext>(SharedFromThis());
}

std::size_t TextViewAccessible::GetCharacterCount() const
{
  return GetTextController()->GetNumberOfCharacters();
}

std::size_t TextViewAccessible::GetCursorOffset() const
{
  return 0u;
}

Bounds TextViewAccessible::GetRangeExtents(std::size_t startOffset, std::size_t endOffset,
                                           Dali::Devel::Accessibility::CoordinateType type) // LCOV_EXCL_LINE
{
  if(endOffset <= startOffset || endOffset > GetCharacterCount())
  {
    return {0, 0, 0, 0};
  }

  auto controller   = GetTextController();
  auto rect         = controller->GetTextBoundingRectangle(startOffset, endOffset - 1);
  auto layoutOffset = controller->GetLayoutOffsetWithPadding();
  auto extents      = GetExtents(type);

  rect.x += extents.x + layoutOffset.x;
  rect.y += extents.y + layoutOffset.y;
  return rect;
}

Dali::Devel::Accessibility::Range TextViewAccessible::GetRangeOfSelection(std::size_t selectionIndex) const // LCOV_EXCL_LINE
{
  if(!SupportsSelection() || selectionIndex > 0)
  {
    return {};
  }

  auto indices     = GetTextController()->GetSelectionIndexes();
  auto startOffset = static_cast<std::size_t>(indices.first);
  auto endOffset   = static_cast<std::size_t>(indices.second);
  auto text        = GetText(startOffset, endOffset);

  return {startOffset, endOffset, std::move(text)};
}

std::string TextViewAccessible::GetText(std::size_t startOffset, std::size_t endOffset) const
{
  auto        text            = GetWholeText();
  std::size_t utf8StartOffset = 0u;
  std::size_t utf8EndOffset   = 0u;
  if(!ConvertToUtf8Range(text, startOffset, endOffset, utf8StartOffset, utf8EndOffset))
  {
    return {};
  }

  if(IsHiddenInput())
  {
    std::uint32_t substituteCharacterUtf32 = GetSubstituteCharacter();
    std::string   substituteCharacterUtf8;
    std::string   substituteText;

    Ui::Text::Utf32ToUtf8(&substituteCharacterUtf32, 1, substituteCharacterUtf8);
    substituteText.reserve(substituteCharacterUtf8.size() * (endOffset - startOffset));
    for(std::size_t index = startOffset; index < endOffset; ++index)
    {
      substituteText.append(substituteCharacterUtf8);
    }
    return substituteText;
  }

  return text.substr(utf8StartOffset, utf8EndOffset - utf8StartOffset);
}

Dali::Devel::Accessibility::Range TextViewAccessible::GetTextAtOffset(
  std::size_t offset, Dali::Devel::Accessibility::TextBoundary boundary) const // LCOV_EXCL_LINE
{
  Dali::Devel::Accessibility::Range range{}; // LCOV_EXCL_LINE
  if(IsHiddenInput())
  {
    return range;
  }

  auto text     = GetWholeText();
  auto textSize = GetCharacterCount();

  switch(boundary)
  {
    case Dali::Devel::Accessibility::TextBoundary::CHARACTER: // LCOV_EXCL_LINE
    {
      if(offset < textSize)
      {
        range.content     = GetText(offset, offset + 1u);
        range.startOffset = offset;
        range.endOffset   = offset + 1;
      }
      break;
    }
    case Dali::Devel::Accessibility::TextBoundary::WORD: // LCOV_EXCL_LINE
    case Dali::Devel::Accessibility::TextBoundary::LINE: // LCOV_EXCL_LINE
    {
      if(text.empty() || text.size() > std::numeric_limits<std::uint32_t>::max())
      {
        break;
      }

      std::vector<std::uint32_t> characters(text.size());
      auto                       characterCount = Ui::Text::Utf8ToUtf32(
        reinterpret_cast<const std::uint8_t*>(text.data()),
        static_cast<std::uint32_t>(text.size()),
        characters.data());
      if(characterCount != textSize)
      {
        break;
      }
      characters.resize(characterCount);

      std::vector<char> breaks(characterCount, '\0');
      if(boundary == Dali::Devel::Accessibility::TextBoundary::WORD) // LCOV_EXCL_LINE
      {
        TextAbstraction::Segmentation::Get().GetWordBreakPositions(
          characters.data(), characterCount, breaks.data());
      }
      else
      {
        TextAbstraction::Segmentation::Get().GetLineBreakPositions(
          characters.data(), characterCount, breaks.data());
      }

      std::size_t index   = 0u;
      std::size_t counter = 0u;
      while(index < textSize && counter <= offset)
      {
        auto start = index;
        if(breaks[index])
        {
          while(index < textSize && breaks[index])
          {
            index++;
          }
          counter++;
        }
        else
        {
          if(boundary == Dali::Devel::Accessibility::TextBoundary::WORD) // LCOV_EXCL_LINE
          {
            index++;
          }
          if(boundary == Dali::Devel::Accessibility::TextBoundary::LINE) // LCOV_EXCL_LINE
          {
            counter++;
          }
        }

        if((counter > 0) && ((counter - 1) == offset))
        {
          auto endOffset    = std::min(index + 1u, textSize);
          range.content     = GetText(start, endOffset);
          range.startOffset = start;
          range.endOffset   = endOffset;
        }

        if(boundary == Dali::Devel::Accessibility::TextBoundary::LINE) // LCOV_EXCL_LINE
        {
          index++;
        }
      }
      break;
    }
    case Dali::Devel::Accessibility::TextBoundary::SENTENCE:  // LCOV_EXCL_LINE
    case Dali::Devel::Accessibility::TextBoundary::PARAGRAPH: // LCOV_EXCL_LINE
    default:
    {
      break;
    }
  }

  return range;
}

bool TextViewAccessible::SetCursorOffset(std::size_t offset)
{
  return false;
}

bool TextViewAccessible::RemoveSelection(std::size_t selectionIndex)
{
  if(!SupportsSelection() || selectionIndex > 0)
  {
    return false;
  }

  GetTextController()->SetSelection(0, 0);
  return true;
}

bool TextViewAccessible::SetRangeOfSelection(std::size_t selectionIndex, std::size_t startOffset,
                                             std::size_t endOffset)
{
  if(!SupportsSelection() || selectionIndex > 0)
  {
    return false;
  }

  // Lack of ValidateRange() is intentional for dali-toolkit compatibility.
  GetTextController()->SetSelection(startOffset, endOffset);
  return true;
}

Dali::Accessibility::Accessible* TextViewAccessible::GetLink(std::int32_t linkIndex) const
{
  if(linkIndex < 0 || linkIndex >= GetLinkCount())
  {
    return nullptr;
  }
  return Dali::Accessibility::Accessible::Get(GetTextAnchors()[linkIndex]);
}

std::int32_t TextViewAccessible::GetLinkCount() const
{
  return static_cast<std::int32_t>(GetTextAnchors().size());
}

std::int32_t TextViewAccessible::GetLinkIndex(std::int32_t characterOffset) const
{
  if(characterOffset < 0)
  {
    return -1;
  }
  return GetTextController()->GetAnchorIndex(static_cast<std::size_t>(characterOffset));
}

std::string TextViewAccessible::GetWholeText() const
{
  std::string text;
  GetTextController()->GetText(text);
  return text;
}

std::string TextViewAccessible::GetCurrentPlaceholderText() const
{
  auto focusView = Internal::KeyInputFocusManager::Get().GetCurrentFocusView();
  bool hasFocus  = Self() == focusView;

  Ui::Text::Controller::PlaceholderType placeholderType =
    hasFocus ? Ui::Text::Controller::PLACEHOLDER_TYPE_ACTIVE : Ui::Text::Controller::PLACEHOLDER_TYPE_INACTIVE;

  std::string placeholderText;
  GetTextController()->GetPlaceholderText(placeholderType, placeholderText);
  return placeholderText;
}

std::uint32_t TextViewAccessible::GetSubstituteCharacter() const
{
  return Ui::Text::DEFAULT_PASSWORD_MASK_CHARACTER;
}

bool TextViewAccessible::IsHiddenInput() const
{
  return false;
}

bool TextViewAccessible::SupportsSelection() const
{
  return false;
}

} // namespace Dali::Ui::Internal
