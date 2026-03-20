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

InputField& InputField::SetTextColor(const Vector4& color)
{
  GetImpl(*this).SetTextColor(color);
  return *this;
}

const Vector4& InputField::GetTextColor() const
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

InputField& InputField::SetPlaceholderColor(const Vector4& color)
{
  GetImpl(*this).SetPlaceholderColor(color);
  return *this;
}

const Vector4& InputField::GetPlaceholderColor() const
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

InputField& InputField::SetCursorColor(const Vector4& color)
{
  GetImpl(*this).SetCursorColor(color);
  return *this;
}

const Vector4& InputField::GetCursorColor() const
{
  return GetImpl(*this).GetCursorColor();
}

InputField& InputField::SetSelectionColor(const Vector4& color)
{
  GetImpl(*this).SetSelectionColor(color);
  return *this;
}

const Vector4& InputField::GetSelectionColor() const
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
