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

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/public-api/input-field.h>

namespace Dali
{

namespace Ui
{

inline Integration::InputFieldImpl& GetImpl(InputField& inputField)
{
  DALI_ASSERT_ALWAYS(inputField);

  Dali::RefObject& handle = inputField.GetImplementation();

  return static_cast<Integration::InputFieldImpl&>(handle);
}

inline const Integration::InputFieldImpl& GetImpl(const InputField& inputField)
{
  DALI_ASSERT_ALWAYS(inputField);

  const Dali::RefObject& handle = inputField.GetImplementation();

  return static_cast<const Integration::InputFieldImpl&>(handle);
}

InputField::InputField()
{
}

InputField InputField::New()
{
  Integration::InputFieldImplPtr impl = Integration::InputFieldImpl::New();

  InputField inputField = InputField(*impl);

  // Second-phase initialization
  impl->Initialize();
  return inputField;
}

InputField::InputField(const InputField& inputField)
: View(inputField)
{
}

InputField::InputField(InputField&& rhs) noexcept
: View(std::move(rhs))
{
}

InputField::~InputField()
{
}

InputField& InputField::operator=(const InputField& handle)
{
  if(&handle != this)
  {
    View::operator=(handle);
  }
  return *this;
}

InputField& InputField::operator=(InputField&& rhs) noexcept
{
  View::operator=(std::move(rhs));
  return *this;
}

InputField InputField::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<InputField, Integration::InputFieldImpl>(handle);
}

InputField::InputField(Integration::InputFieldImpl& implementation)
: View(implementation)
{
}

InputField::InputField(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::InputFieldImpl>(internal);
}

// =============================================================================
// Properties
// =============================================================================

InputField& InputField::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
  return *this;
}

Dali::String InputField::GetText() const
{
  return GetImpl(*this).GetText();
}

InputField& InputField::SetFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
  return *this;
}

Dali::String InputField::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

InputField& InputField::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
  return *this;
}

float InputField::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

InputField& InputField::SetTextColor(const UiColor& color)
{
  GetImpl(*this).SetTextColor(color);
  return *this;
}

UiColor InputField::GetTextColor()
{
  return GetImpl(*this).GetTextColor();
}

InputField& InputField::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetHorizontalTextAlignment(alignment);
  return *this;
}

Text::Alignment InputField::GetHorizontalTextAlignment() const
{
  return GetImpl(*this).GetHorizontalTextAlignment();
}

InputField& InputField::SetVerticalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetVerticalTextAlignment(alignment);
  return *this;
}

Text::Alignment InputField::GetVerticalTextAlignment() const
{
  return GetImpl(*this).GetVerticalTextAlignment();
}

InputField& InputField::SetPlaceholder(const Dali::String& text)
{
  GetImpl(*this).SetPlaceholder(text);
  return *this;
}

Dali::String InputField::GetPlaceholder() const
{
  return GetImpl(*this).GetPlaceholder();
}

InputField& InputField::SetPlaceholderColor(const UiColor& color)
{
  GetImpl(*this).SetPlaceholderColor(color);
  return *this;
}

UiColor InputField::GetPlaceholderColor()
{
  return GetImpl(*this).GetPlaceholderColor();
}

InputField& InputField::SetCursorWidth(int width)
{
  GetImpl(*this).SetCursorWidth(width);
  return *this;
}

int InputField::GetCursorWidth() const
{
  return GetImpl(*this).GetCursorWidth();
}

InputField& InputField::SetCursorColor(const UiColor& color)
{
  GetImpl(*this).SetCursorColor(color);
  return *this;
}

UiColor InputField::GetCursorColor()
{
  return GetImpl(*this).GetCursorColor();
}

InputField& InputField::SetSelectionColor(const UiColor& color)
{
  GetImpl(*this).SetSelectionColor(color);
  return *this;
}

UiColor InputField::GetSelectionColor()
{
  return GetImpl(*this).GetSelectionColor();
}

InputField& InputField::SetMaximumLength(int length)
{
  GetImpl(*this).SetMaximumLength(length);
  return *this;
}

int InputField::GetMaximumLength() const
{
  return GetImpl(*this).GetMaximumLength();
}

InputField& InputField::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  GetImpl(*this).SetLayoutDirectionMode(mode);
  return *this;
}

Text::LayoutDirectionMode InputField::GetLayoutDirectionMode() const
{
  return GetImpl(*this).GetLayoutDirectionMode();
}

InputField& InputField::SetFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetFontWeight(weight);
  return *this;
}

Text::FontWeight InputField::GetFontWeight() const
{
  return GetImpl(*this).GetFontWeight();
}

InputField& InputField::SetFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetFontWidth(width);
  return *this;
}

Text::FontWidth InputField::GetFontWidth() const
{
  return GetImpl(*this).GetFontWidth();
}

InputField& InputField::SetFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetFontSlant(slant);
  return *this;
}

Text::FontSlant InputField::GetFontSlant() const
{
  return GetImpl(*this).GetFontSlant();
}

InputField& InputField::SetUnderline(const Text::Underline& underline)
{
  GetImpl(*this).SetUnderline(underline);
  return *this;
}

void InputField::ResetUnderline()
{
  return GetImpl(*this).ResetUnderline();
}

InputField& InputField::SetShadow(const Text::Shadow& shadow)
{
  GetImpl(*this).SetShadow(shadow);
  return *this;
}

void InputField::ResetShadow()
{
  return GetImpl(*this).ResetShadow();
}

InputField& InputField::SetOutline(const Text::Outline& outline)
{
  GetImpl(*this).SetOutline(outline);
  return *this;
}

void InputField::ResetOutline()
{
  return GetImpl(*this).ResetOutline();
}

InputField& InputField::SetLineThrough(const Text::LineThrough& lineThrough)
{
  GetImpl(*this).SetLineThrough(lineThrough);
  return *this;
}

void InputField::ResetLineThrough()
{
  return GetImpl(*this).ResetLineThrough();
}

// =============================================================================
// Signals
// =============================================================================
Signal<void(View)>& InputField::TextChangedSignal()
{
  return GetImpl(*this).TextChangedSignal();
}

Signal<void(View)>& InputField::MaximumLengthReachedSignal()
{
  return GetImpl(*this).MaximumLengthReachedSignal();
}

} // namespace Ui

} // namespace Dali
