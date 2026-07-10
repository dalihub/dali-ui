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
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>
#include <dali/devel-api/object/type-registry.h>

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

void InputField::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
}

Dali::String InputField::GetText() const
{
  return GetImpl(*this).GetText();
}

void InputField::SetStyledText(const Text::StyledText& styledText)
{
  GetImpl(*this).SetStyledText(styledText);
}

void InputField::SetFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
}

Dali::String InputField::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

void InputField::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
}

float InputField::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

void InputField::SetTextColor(const UiColor& color)
{
  GetImpl(*this).SetTextColor(color);
}

UiColor InputField::GetTextColor()
{
  return GetImpl(*this).GetTextColor();
}

void InputField::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetHorizontalTextAlignment(alignment);
}

Text::Alignment InputField::GetHorizontalTextAlignment() const
{
  return GetImpl(*this).GetHorizontalTextAlignment();
}

void InputField::SetVerticalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetVerticalTextAlignment(alignment);
}

Text::Alignment InputField::GetVerticalTextAlignment() const
{
  return GetImpl(*this).GetVerticalTextAlignment();
}

void InputField::SetTextOverflowMode(Text::OverflowMode mode)
{
  GetImpl(*this).SetTextOverflowMode(mode);
}

Text::OverflowMode InputField::GetTextOverflowMode() const
{
  return GetImpl(*this).GetTextOverflowMode();
}

void InputField::SetPlaceholder(const Dali::String& text)
{
  GetImpl(*this).SetPlaceholder(text);
}

Dali::String InputField::GetPlaceholder() const
{
  return GetImpl(*this).GetPlaceholder();
}

void InputField::SetPlaceholderColor(const UiColor& color)
{
  GetImpl(*this).SetPlaceholderColor(color);
}

UiColor InputField::GetPlaceholderColor()
{
  return GetImpl(*this).GetPlaceholderColor();
}

void InputField::SetShowPlaceholderOnFocus(bool enabled)
{
  GetImpl(*this).SetShowPlaceholderOnFocus(enabled);
}

bool InputField::IsPlaceholderShownOnFocus() const
{
  return GetImpl(*this).IsPlaceholderShownOnFocus();
}

void InputField::SetCursorWidth(int width)
{
  GetImpl(*this).SetCursorWidth(width);
}

int InputField::GetCursorWidth() const
{
  return GetImpl(*this).GetCursorWidth();
}

void InputField::SetCursorColor(const UiColor& color)
{
  GetImpl(*this).SetCursorColor(color);
}

UiColor InputField::GetCursorColor()
{
  return GetImpl(*this).GetCursorColor();
}

void InputField::SetCursorBlinkEnabled(bool enabled)
{
  GetImpl(*this).SetCursorBlinkEnabled(enabled);
}

bool InputField::IsCursorBlinkEnabled() const
{
  return GetImpl(*this).IsCursorBlinkEnabled();
}

void InputField::SetCursorBlinkInterval(float interval)
{
  GetImpl(*this).SetCursorBlinkInterval(interval);
}

float InputField::GetCursorBlinkInterval() const
{
  return GetImpl(*this).GetCursorBlinkInterval();
}

void InputField::SetCursorPosition(uint32_t position)
{
  GetImpl(*this).SetCursorPosition(position);
}

uint32_t InputField::GetCursorPosition() const
{
  return GetImpl(*this).GetCursorPosition();
}

void InputField::SetSelectionEnabled(bool enabled)
{
  GetImpl(*this).SetSelectionEnabled(enabled);
}

bool InputField::IsSelectionEnabled() const
{
  return GetImpl(*this).IsSelectionEnabled();
}

void InputField::SetSelectionColor(const UiColor& color)
{
  GetImpl(*this).SetSelectionColor(color);
}

UiColor InputField::GetSelectionColor()
{
  return GetImpl(*this).GetSelectionColor();
}

void InputField::SetTextHandleEnabled(bool enabled)
{
  GetImpl(*this).SetTextHandleEnabled(enabled);
}

bool InputField::IsTextHandleEnabled() const
{
  return GetImpl(*this).IsTextHandleEnabled();
}

void InputField::SetTextHandleColor(const UiColor& color)
{
  GetImpl(*this).SetTextHandleColor(color);
}

UiColor InputField::GetTextHandleColor() const
{
  return GetImpl(*this).GetTextHandleColor();
}

void InputField::SetCursorHandleImage(const Dali::String& image)
{
  GetImpl(*this).SetCursorHandleImage(image);
}

Dali::String InputField::GetCursorHandleImage() const
{
  return GetImpl(*this).GetCursorHandleImage();
}

void InputField::SetCursorHandlePressedImage(const Dali::String& image)
{
  GetImpl(*this).SetCursorHandlePressedImage(image);
}

Dali::String InputField::GetCursorHandlePressedImage() const
{
  return GetImpl(*this).GetCursorHandlePressedImage();
}

void InputField::SetSelectionHandleImageLeft(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandleImageLeft(image);
}

Dali::String InputField::GetSelectionHandleImageLeft() const
{
  return GetImpl(*this).GetSelectionHandleImageLeft();
}

void InputField::SetSelectionHandleImageRight(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandleImageRight(image);
}

Dali::String InputField::GetSelectionHandleImageRight() const
{
  return GetImpl(*this).GetSelectionHandleImageRight();
}

void InputField::SetSelectionHandlePressedImageLeft(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandlePressedImageLeft(image);
}

Dali::String InputField::GetSelectionHandlePressedImageLeft() const
{
  return GetImpl(*this).GetSelectionHandlePressedImageLeft();
}

void InputField::SetSelectionHandlePressedImageRight(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandlePressedImageRight(image);
}

Dali::String InputField::GetSelectionHandlePressedImageRight() const
{
  return GetImpl(*this).GetSelectionHandlePressedImageRight();
}

void InputField::SetMaximumLength(int length)
{
  GetImpl(*this).SetMaximumLength(length);
}

int InputField::GetMaximumLength() const
{
  return GetImpl(*this).GetMaximumLength();
}

void InputField::SetInputFilter(const Text::InputFilter& inputFilter)
{
  GetImpl(*this).SetInputFilter(inputFilter);
}

Text::InputFilter InputField::GetInputFilter() const
{
  return GetImpl(*this).GetInputFilter();
}

void InputField::SetPasswordMode(Text::PasswordMode mode)
{
  GetImpl(*this).SetPasswordMode(mode);
}

Text::PasswordMode InputField::GetPasswordMode() const
{
  return GetImpl(*this).GetPasswordMode();
}

void InputField::SetPasswordMaskCharacter(uint32_t character)
{
  GetImpl(*this).SetPasswordMaskCharacter(character);
}

uint32_t InputField::GetPasswordMaskCharacter() const
{
  return GetImpl(*this).GetPasswordMaskCharacter();
}

void InputField::SetPasswordRevealDuration(uint32_t duration)
{
  GetImpl(*this).SetPasswordRevealDuration(duration);
}

uint32_t InputField::GetPasswordRevealDuration() const
{
  return GetImpl(*this).GetPasswordRevealDuration();
}

void InputField::SetEditable(bool editable)
{
  GetImpl(*this).SetEditable(editable);
}

bool InputField::IsEditable() const
{
  return GetImpl(*this).IsEditable();
}

void InputField::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  GetImpl(*this).SetLayoutDirectionMode(mode);
}

Text::LayoutDirectionMode InputField::GetLayoutDirectionMode() const
{
  return GetImpl(*this).GetLayoutDirectionMode();
}

void InputField::SetFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetFontWeight(weight);
}

Text::FontWeight InputField::GetFontWeight() const
{
  return GetImpl(*this).GetFontWeight();
}

void InputField::SetFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetFontWidth(width);
}

Text::FontWidth InputField::GetFontWidth() const
{
  return GetImpl(*this).GetFontWidth();
}

void InputField::SetFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetFontSlant(slant);
}

Text::FontSlant InputField::GetFontSlant() const
{
  return GetImpl(*this).GetFontSlant();
}

void InputField::SetTextBackgroundColor(const UiColor& color)
{
  GetImpl(*this).SetTextBackgroundColor(color);
}

UiColor InputField::GetTextBackgroundColor() const
{
  return GetImpl(*this).GetTextBackgroundColor();
}

void InputField::ClearTextBackgroundColor()
{
  GetImpl(*this).ClearTextBackgroundColor();
}

void InputField::SetTextUnderline(const Text::Underline& underline)
{
  GetImpl(*this).SetTextUnderline(underline);
}

Text::Underline InputField::GetTextUnderline() const
{
  return GetImpl(*this).GetTextUnderline();
}

void InputField::SetTextShadow(const Text::Shadow& shadow)
{
  GetImpl(*this).SetTextShadow(shadow);
}

Text::Shadow InputField::GetTextShadow() const
{
  return GetImpl(*this).GetTextShadow();
}

void InputField::SetTextOutline(const Text::Outline& outline)
{
  GetImpl(*this).SetTextOutline(outline);
}

Text::Outline InputField::GetTextOutline() const
{
  return GetImpl(*this).GetTextOutline();
}

void InputField::SetTextLineThrough(const Text::LineThrough& lineThrough)
{
  GetImpl(*this).SetTextLineThrough(lineThrough);
}

Text::LineThrough InputField::GetTextLineThrough() const
{
  return GetImpl(*this).GetTextLineThrough();
}

void InputField::SetMinimumFontSizeScale(float scale)
{
  GetImpl(*this).SetMinimumFontSizeScale(scale);
}

float InputField::GetMinimumFontSizeScale() const
{
  return GetImpl(*this).GetMinimumFontSizeScale();
}

void InputField::SetMaximumFontSizeScale(float scale)
{
  GetImpl(*this).SetMaximumFontSizeScale(scale);
}

float InputField::GetMaximumFontSizeScale() const
{
  return GetImpl(*this).GetMaximumFontSizeScale();
}

void InputField::SetSystemFontSizeScaleEnabled(bool enabled)
{
  GetImpl(*this).SetSystemFontSizeScaleEnabled(enabled);
}

bool InputField::IsSystemFontSizeScaleEnabled() const
{
  return GetImpl(*this).IsSystemFontSizeScaleEnabled();
}

void InputField::SetTypingTextColor(const UiColor& color)
{
  GetImpl(*this).SetTypingTextColor(color);
}

UiColor InputField::GetTypingTextColor() const
{
  return GetImpl(*this).GetTypingTextColor();
}

void InputField::SetTypingFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetTypingFontFamily(fontFamily);
}

Dali::String InputField::GetTypingFontFamily() const
{
  return GetImpl(*this).GetTypingFontFamily();
}

void InputField::SetTypingFontSize(float fontSize)
{
  GetImpl(*this).SetTypingFontSize(fontSize);
}

float InputField::GetTypingFontSize() const
{
  return GetImpl(*this).GetTypingFontSize();
}

void InputField::SetTypingFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetTypingFontWeight(weight);
}

Text::FontWeight InputField::GetTypingFontWeight() const
{
  return GetImpl(*this).GetTypingFontWeight();
}

void InputField::SetTypingFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetTypingFontWidth(width);
}

Text::FontWidth InputField::GetTypingFontWidth() const
{
  return GetImpl(*this).GetTypingFontWidth();
}

void InputField::SetTypingFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetTypingFontSlant(slant);
}

Text::FontSlant InputField::GetTypingFontSlant() const
{
  return GetImpl(*this).GetTypingFontSlant();
}

void InputField::SetFontVariation(const Dali::Vector<Text::FontVariation::Axis>& axes)
{
  GetImpl(*this).SetFontVariation(axes);
}

void InputField::SetFontVariation(const Dali::String& settings)
{
  GetImpl(*this).SetFontVariation(settings);
}

Dali::Vector<Text::FontVariation::Axis> InputField::GetFontVariation() const
{
  return GetImpl(*this).GetFontVariation();
}

void InputField::SetTranslatablePlaceholder(StringView resourceId)
{
  GetImpl(*this).SetTranslatablePlaceholder(resourceId);
}

void InputField::SetTranslatablePlaceholder(StringView resourceId, StringView domain)
{
  GetImpl(*this).SetTranslatablePlaceholder(resourceId, domain);
}

Dali::String InputField::GetTranslatablePlaceholder() const
{
  return GetImpl(*this).GetTranslatablePlaceholder();
}

void InputField::ClearTranslatablePlaceholder()
{
  GetImpl(*this).ClearTranslatablePlaceholder();
}
float InputField::GetAdjustedFontSizeScale() const
{
  return GetImpl(*this).GetAdjustedFontSizeScale();
}

Dali::String InputField::GetSelectedText() const
{
  return GetImpl(*this).GetSelectedText();
}

uint32_t InputField::GetSelectedTextStart() const
{
  return GetImpl(*this).GetSelectedTextStart();
}

uint32_t InputField::GetSelectedTextEnd() const
{
  return GetImpl(*this).GetSelectedTextEnd();
}
void InputField::SelectText(uint32_t startIndex, uint32_t endIndex)
{
  GetImpl(*this).SelectText(startIndex, endIndex);
}

void InputField::SelectWholeText()
{
  GetImpl(*this).SelectWholeText();
}

void InputField::ClearSelection()
{
  GetImpl(*this).ClearSelection();
}

InputMethodContext InputField::GetInputMethodContext()
{
  return GetImpl(*this).GetInputMethodContext();
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

Signal<void(View, Text::InputFilter::RejectReason)>& InputField::InputRejectedSignal()
{
  return GetImpl(*this).InputRejectedSignal();
}

Signal<void(View, uint32_t)>& InputField::CursorPositionChangedSignal()
{
  return GetImpl(*this).CursorPositionChangedSignal();
}

Signal<void(View)>& InputField::SelectionStartedSignal()
{
  return GetImpl(*this).SelectionStartedSignal();
}

Signal<void(View, uint32_t, uint32_t)>& InputField::SelectionChangedSignal()
{
  return GetImpl(*this).SelectionChangedSignal();
}

Signal<void(View)>& InputField::SelectionClearedSignal()
{
  return GetImpl(*this).SelectionClearedSignal();
}

Signal<void(View, Text::TypingStyle::Mask)>& InputField::TypingStyleChangedSignal()
{
  return GetImpl(*this).TypingStyleChangedSignal();
}

} // namespace Ui

} // namespace Dali
