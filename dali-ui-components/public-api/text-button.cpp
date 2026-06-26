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

// CLASS HEADER
#include <dali-ui-components/public-api/text-button.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/text-button-impl.h>

namespace Dali
{
namespace Ui
{

TextButton::TextButton() = default;

TextButton::~TextButton() = default;

TextButton TextButton::New()
{
  return New(TextButtonStyle::Default());
}

TextButton TextButton::New(TextButtonStyle style)
{
  return Internal::TextButtonImpl::New(style);
}

TextButton TextButton::New(const Dali::String& text)
{
  TextButton button = New(TextButtonStyle::Default());
  button.SetText(text);
  return button;
}

TextButton TextButton::New(const Dali::String& text, TextButtonStyle style)
{
  TextButton button = New(style);
  button.SetText(text);
  return button;
}

TextButton TextButton::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<TextButton, Internal::TextButtonImpl>(handle);
}

TextButton::TextButton(const TextButton& handle)
: InteractiveView(handle)
{
}

TextButton::TextButton(TextButton&& rhs) noexcept = default;

TextButton& TextButton::operator=(const TextButton& handle)
{
  if(&handle != this)
  {
    InteractiveView::operator=(handle);
  }
  return *this;
}

TextButton& TextButton::operator=(TextButton&& rhs) noexcept = default;

void TextButton::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
}

Dali::String TextButton::GetText() const
{
  return GetImpl(*this).GetText();
}

void TextButton::SetHorizontalAlignment(LayoutAlignment alignment)
{
  GetImpl(*this).SetHorizontalAlignment(alignment);
}

LayoutAlignment TextButton::GetHorizontalAlignment() const
{
  return GetImpl(*this).GetHorizontalAlignment();
}

void TextButton::SetVerticalAlignment(LayoutAlignment alignment)
{
  GetImpl(*this).SetVerticalAlignment(alignment);
}

LayoutAlignment TextButton::GetVerticalAlignment() const
{
  return GetImpl(*this).GetVerticalAlignment();
}

void TextButton::SetTextColor(const UiColor& color)
{
  GetImpl(*this).SetTextColor(color);
}

UiColor TextButton::GetTextColor() const
{
  return GetImpl(*this).GetTextColor();
}

void TextButton::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
}

float TextButton::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

void TextButton::SetFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
}

Dali::String TextButton::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

void TextButton::SetTextUnderline(const Text::Underline& underline)
{
  GetImpl(*this).SetTextUnderline(underline);
}

bool TextButton::IsTextUnderlineEnabled() const
{
  return GetImpl(*this).IsTextUnderlineEnabled();
}

Text::Underline TextButton::GetTextUnderline() const
{
  return GetImpl(*this).GetTextUnderline();
}

TextButton::TextButton(Internal::TextButtonImpl& implementation)
: InteractiveView(implementation)
{
}

TextButton::TextButton(Dali::Internal::CustomActor* internal)
: InteractiveView(internal)
{
  VerifyCustomActorPointer<Internal::TextButtonImpl>(internal);
}

} // namespace Ui
} // namespace Dali
