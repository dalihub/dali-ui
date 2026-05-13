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
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/public-api/input-editor.h>

namespace Dali
{

namespace Ui
{

inline Integration::InputEditorImpl& GetImpl(InputEditor& inputEditor)
{
  DALI_ASSERT_ALWAYS(inputEditor);

  Dali::RefObject& handle = inputEditor.GetImplementation();

  return static_cast<Integration::InputEditorImpl&>(handle);
}

inline const Integration::InputEditorImpl& GetImpl(const InputEditor& inputEditor)
{
  DALI_ASSERT_ALWAYS(inputEditor);

  const Dali::RefObject& handle = inputEditor.GetImplementation();

  return static_cast<const Integration::InputEditorImpl&>(handle);
}

InputEditor::InputEditor()
{
}

InputEditor InputEditor::New()
{
  Integration::InputEditorImplPtr impl = Integration::InputEditorImpl::New();

  InputEditor inputEditor = InputEditor(*impl);

  // Second-phase initialization
  impl->Initialize();
  return inputEditor;
}

InputEditor::InputEditor(const InputEditor& inputEditor)
: View(inputEditor)
{
}

InputEditor::InputEditor(InputEditor&& rhs) noexcept
: View(std::move(rhs))
{
}

InputEditor::~InputEditor()
{
}

InputEditor& InputEditor::operator=(const InputEditor& handle)
{
  if(&handle != this)
  {
    View::operator=(handle);
  }
  return *this;
}

InputEditor& InputEditor::operator=(InputEditor&& rhs) noexcept
{
  View::operator=(std::move(rhs));
  return *this;
}

InputEditor InputEditor::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<InputEditor, Integration::InputEditorImpl>(handle);
}

InputEditor::InputEditor(Integration::InputEditorImpl& implementation)
: View(implementation)
{
}

InputEditor::InputEditor(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::InputEditorImpl>(internal);
}

// =============================================================================
// Properties
// =============================================================================

InputEditor& InputEditor::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
  return *this;
}

Dali::String InputEditor::GetText() const
{
  return GetImpl(*this).GetText();
}

InputEditor& InputEditor::SetFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
  return *this;
}

Dali::String InputEditor::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

InputEditor& InputEditor::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
  return *this;
}

float InputEditor::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

InputEditor& InputEditor::SetTextColor(const UiColor& color)
{
  GetImpl(*this).SetTextColor(color);
  return *this;
}

UiColor InputEditor::GetTextColor()
{
  return GetImpl(*this).GetTextColor();
}

InputEditor& InputEditor::SetLineWrapMode(Text::LineWrapMode mode)
{
  GetImpl(*this).SetLineWrapMode(mode);
  return *this;
}

Text::LineWrapMode InputEditor::GetLineWrapMode() const
{
  return GetImpl(*this).GetLineWrapMode();
}

InputEditor& InputEditor::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetHorizontalTextAlignment(alignment);
  return *this;
}

Text::Alignment InputEditor::GetHorizontalTextAlignment() const
{
  return GetImpl(*this).GetHorizontalTextAlignment();
}

InputEditor& InputEditor::SetVerticalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetVerticalTextAlignment(alignment);
  return *this;
}

Text::Alignment InputEditor::GetVerticalTextAlignment() const
{
  return GetImpl(*this).GetVerticalTextAlignment();
}

InputEditor& InputEditor::SetOverflowMode(Text::OverflowMode mode)
{
  GetImpl(*this).SetOverflowMode(mode);
  return *this;
}

Text::OverflowMode InputEditor::GetOverflowMode() const
{
  return GetImpl(*this).GetOverflowMode();
}

InputEditor& InputEditor::SetLineHeight(float lineHeight)
{
  GetImpl(*this).SetLineHeight(lineHeight);
  return *this;
}

float InputEditor::GetLineHeight() const
{
  return GetImpl(*this).GetLineHeight();
}

InputEditor& InputEditor::SetLineHeightMode(Text::LineHeightMode mode)
{
  GetImpl(*this).SetLineHeightMode(mode);
  return *this;
}

Text::LineHeightMode InputEditor::GetLineHeightMode() const
{
  return GetImpl(*this).GetLineHeightMode();
}

InputEditor& InputEditor::SetPlaceholder(const Dali::String& text)
{
  GetImpl(*this).SetPlaceholder(text);
  return *this;
}

Dali::String InputEditor::GetPlaceholder() const
{
  return GetImpl(*this).GetPlaceholder();
}

InputEditor& InputEditor::SetPlaceholderColor(const UiColor& color)
{
  GetImpl(*this).SetPlaceholderColor(color);
  return *this;
}

UiColor InputEditor::GetPlaceholderColor()
{
  return GetImpl(*this).GetPlaceholderColor();
}

InputEditor& InputEditor::SetShowPlaceholderOnFocus(bool enabled)
{
  GetImpl(*this).SetShowPlaceholderOnFocus(enabled);
  return *this;
}

bool InputEditor::IsPlaceholderShownOnFocus() const
{
  return GetImpl(*this).IsPlaceholderShownOnFocus();
}

InputEditor& InputEditor::SetCursorWidth(int width)
{
  GetImpl(*this).SetCursorWidth(width);
  return *this;
}

int InputEditor::GetCursorWidth() const
{
  return GetImpl(*this).GetCursorWidth();
}

InputEditor& InputEditor::SetCursorColor(const UiColor& color)
{
  GetImpl(*this).SetCursorColor(color);
  return *this;
}

UiColor InputEditor::GetCursorColor()
{
  return GetImpl(*this).GetCursorColor();
}

InputEditor& InputEditor::SetCursorBlinkEnabled(bool enabled)
{
  GetImpl(*this).SetCursorBlinkEnabled(enabled);
  return *this;
}

bool InputEditor::IsCursorBlinkEnabled() const
{
  return GetImpl(*this).IsCursorBlinkEnabled();
}

InputEditor& InputEditor::SetCursorBlinkInterval(float interval)
{
  GetImpl(*this).SetCursorBlinkInterval(interval);
  return *this;
}

float InputEditor::GetCursorBlinkInterval() const
{
  return GetImpl(*this).GetCursorBlinkInterval();
}

InputEditor& InputEditor::SetCursorPosition(uint32_t position)
{
  GetImpl(*this).SetCursorPosition(position);
  return *this;
}

uint32_t InputEditor::GetCursorPosition() const
{
  return GetImpl(*this).GetCursorPosition();
}

InputEditor& InputEditor::SetSelectionEnabled(bool enabled)
{
  GetImpl(*this).SetSelectionEnabled(enabled);
  return *this;
}

bool InputEditor::IsSelectionEnabled() const
{
  return GetImpl(*this).IsSelectionEnabled();
}

InputEditor& InputEditor::SetSelectionColor(const UiColor& color)
{
  GetImpl(*this).SetSelectionColor(color);
  return *this;
}

UiColor InputEditor::GetSelectionColor()
{
  return GetImpl(*this).GetSelectionColor();
}

InputEditor& InputEditor::SetTextHandleEnabled(bool enabled)
{
  GetImpl(*this).SetTextHandleEnabled(enabled);
  return *this;
}

bool InputEditor::IsTextHandleEnabled() const
{
  return GetImpl(*this).IsTextHandleEnabled();
}

InputEditor& InputEditor::SetTextHandleColor(const UiColor& color)
{
  GetImpl(*this).SetTextHandleColor(color);
  return *this;
}

UiColor InputEditor::GetTextHandleColor() const
{
  return GetImpl(*this).GetTextHandleColor();
}

InputEditor& InputEditor::SetCursorHandleImage(const Dali::String& image)
{
  GetImpl(*this).SetCursorHandleImage(image);
  return *this;
}

Dali::String InputEditor::GetCursorHandleImage() const
{
  return GetImpl(*this).GetCursorHandleImage();
}

InputEditor& InputEditor::SetCursorHandlePressedImage(const Dali::String& image)
{
  GetImpl(*this).SetCursorHandlePressedImage(image);
  return *this;
}

Dali::String InputEditor::GetCursorHandlePressedImage() const
{
  return GetImpl(*this).GetCursorHandlePressedImage();
}

InputEditor& InputEditor::SetSelectionHandleImageLeft(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandleImageLeft(image);
  return *this;
}

Dali::String InputEditor::GetSelectionHandleImageLeft() const
{
  return GetImpl(*this).GetSelectionHandleImageLeft();
}

InputEditor& InputEditor::SetSelectionHandleImageRight(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandleImageRight(image);
  return *this;
}

Dali::String InputEditor::GetSelectionHandleImageRight() const
{
  return GetImpl(*this).GetSelectionHandleImageRight();
}

InputEditor& InputEditor::SetSelectionHandlePressedImageLeft(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandlePressedImageLeft(image);
  return *this;
}

Dali::String InputEditor::GetSelectionHandlePressedImageLeft() const
{
  return GetImpl(*this).GetSelectionHandlePressedImageLeft();
}

InputEditor& InputEditor::SetSelectionHandlePressedImageRight(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandlePressedImageRight(image);
  return *this;
}

Dali::String InputEditor::GetSelectionHandlePressedImageRight() const
{
  return GetImpl(*this).GetSelectionHandlePressedImageRight();
}

InputEditor& InputEditor::SetMaximumLength(int length)
{
  GetImpl(*this).SetMaximumLength(length);
  return *this;
}

int InputEditor::GetMaximumLength() const
{
  return GetImpl(*this).GetMaximumLength();
}

InputEditor& InputEditor::SetInputFilter(const Text::InputFilter& inputFilter)
{
  GetImpl(*this).SetInputFilter(inputFilter);
  return *this;
}

InputEditor& InputEditor::ClearInputFilter()
{
  GetImpl(*this).ClearInputFilter();
  return *this;
}

InputEditor& InputEditor::SetEditable(bool editable)
{
  GetImpl(*this).SetEditable(editable);
  return *this;
}

bool InputEditor::IsEditable() const
{
  return GetImpl(*this).IsEditable();
}

InputEditor& InputEditor::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  GetImpl(*this).SetLayoutDirectionMode(mode);
  return *this;
}

Text::LayoutDirectionMode InputEditor::GetLayoutDirectionMode() const
{
  return GetImpl(*this).GetLayoutDirectionMode();
}

InputEditor& InputEditor::SetMarkupEnabled(bool enabled)
{
  GetImpl(*this).SetMarkupEnabled(enabled);
  return *this;
}

bool InputEditor::IsMarkupEnabled() const
{
  return GetImpl(*this).IsMarkupEnabled();
}

InputEditor& InputEditor::SetFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetFontWeight(weight);
  return *this;
}

Text::FontWeight InputEditor::GetFontWeight() const
{
  return GetImpl(*this).GetFontWeight();
}

InputEditor& InputEditor::SetFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetFontWidth(width);
  return *this;
}

Text::FontWidth InputEditor::GetFontWidth() const
{
  return GetImpl(*this).GetFontWidth();
}

InputEditor& InputEditor::SetFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetFontSlant(slant);
  return *this;
}

Text::FontSlant InputEditor::GetFontSlant() const
{
  return GetImpl(*this).GetFontSlant();
}

InputEditor& InputEditor::SetTextBackgroundColor(const UiColor& color)
{
  GetImpl(*this).SetTextBackgroundColor(color);
  return *this;
}

UiColor InputEditor::GetTextBackgroundColor() const
{
  return GetImpl(*this).GetTextBackgroundColor();
}

void InputEditor::ClearTextBackgroundColor()
{
  GetImpl(*this).ClearTextBackgroundColor();
}

InputEditor& InputEditor::SetUnderline(const Text::Underline& underline)
{
  GetImpl(*this).SetUnderline(underline);
  return *this;
}

void InputEditor::ClearUnderline()
{
  return GetImpl(*this).ClearUnderline();
}

InputEditor& InputEditor::SetShadow(const Text::Shadow& shadow)
{
  GetImpl(*this).SetShadow(shadow);
  return *this;
}

void InputEditor::ClearShadow()
{
  return GetImpl(*this).ClearShadow();
}

InputEditor& InputEditor::SetOutline(const Text::Outline& outline)
{
  GetImpl(*this).SetOutline(outline);
  return *this;
}

void InputEditor::ClearOutline()
{
  return GetImpl(*this).ClearOutline();
}

InputEditor& InputEditor::SetLineThrough(const Text::LineThrough& lineThrough)
{
  GetImpl(*this).SetLineThrough(lineThrough);
  return *this;
}

void InputEditor::ClearLineThrough()
{
  return GetImpl(*this).ClearLineThrough();
}

InputEditor& InputEditor::SetFontSizeScale(float scale)
{
  GetImpl(*this).SetFontSizeScale(scale);
  return *this;
}

float InputEditor::GetFontSizeScale() const
{
  return GetImpl(*this).GetFontSizeScale();
}

InputEditor& InputEditor::SetMinimumFontSizeScale(float scale)
{
  GetImpl(*this).SetMinimumFontSizeScale(scale);
  return *this;
}

float InputEditor::GetMinimumFontSizeScale() const
{
  return GetImpl(*this).GetMinimumFontSizeScale();
}

InputEditor& InputEditor::SetMaximumFontSizeScale(float scale)
{
  GetImpl(*this).SetMaximumFontSizeScale(scale);
  return *this;
}

float InputEditor::GetMaximumFontSizeScale() const
{
  return GetImpl(*this).GetMaximumFontSizeScale();
}

InputEditor& InputEditor::SetSystemFontSizeScaleEnabled(bool enabled)
{
  GetImpl(*this).SetSystemFontSizeScaleEnabled(enabled);
  return *this;
}

bool InputEditor::IsSystemFontSizeScaleEnabled() const
{
  return GetImpl(*this).IsSystemFontSizeScaleEnabled();
}

InputEditor& InputEditor::SetTypingTextColor(const UiColor& color)
{
  GetImpl(*this).SetTypingTextColor(color);
  return *this;
}

UiColor InputEditor::GetTypingTextColor() const
{
  return GetImpl(*this).GetTypingTextColor();
}

InputEditor& InputEditor::SetTypingFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetTypingFontFamily(fontFamily);
  return *this;
}

Dali::String InputEditor::GetTypingFontFamily() const
{
  return GetImpl(*this).GetTypingFontFamily();
}

InputEditor& InputEditor::SetTypingFontSize(float fontSize)
{
  GetImpl(*this).SetTypingFontSize(fontSize);
  return *this;
}

float InputEditor::GetTypingFontSize() const
{
  return GetImpl(*this).GetTypingFontSize();
}

InputEditor& InputEditor::SetTypingFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetTypingFontWeight(weight);
  return *this;
}

Text::FontWeight InputEditor::GetTypingFontWeight() const
{
  return GetImpl(*this).GetTypingFontWeight();
}

InputEditor& InputEditor::SetTypingFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetTypingFontWidth(width);
  return *this;
}

Text::FontWidth InputEditor::GetTypingFontWidth() const
{
  return GetImpl(*this).GetTypingFontWidth();
}

InputEditor& InputEditor::SetTypingFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetTypingFontSlant(slant);
  return *this;
}

Text::FontSlant InputEditor::GetTypingFontSlant() const
{
  return GetImpl(*this).GetTypingFontSlant();
}

InputEditor& InputEditor::SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes)
{
  GetImpl(*this).SetFontVariation(axes);
  return *this;
}

InputEditor& InputEditor::SetFontVariation(const Dali::String& settings)
{
  GetImpl(*this).SetFontVariation(settings);
  return *this;
}

Dali::Vector<Text::FontVariationAxis> InputEditor::GetFontVariation() const
{
  return GetImpl(*this).GetFontVariation();
}

InputEditor& InputEditor::ClearFontVariation()
{
  GetImpl(*this).ClearFontVariation();
  return *this;
}

// =============================================================================
// Read Only
// =============================================================================
float InputEditor::GetAdjustedFontSizeScale() const
{
  return GetImpl(*this).GetAdjustedFontSizeScale();
}

Dali::String InputEditor::GetSelectedText() const
{
  return GetImpl(*this).GetSelectedText();
}

uint32_t InputEditor::GetSelectedTextStart() const
{
  return GetImpl(*this).GetSelectedTextStart();
}

uint32_t InputEditor::GetSelectedTextEnd() const
{
  return GetImpl(*this).GetSelectedTextEnd();
}

// =============================================================================
// Method
// =============================================================================
InputEditor& InputEditor::SelectText(uint32_t startIndex, uint32_t endIndex)
{
  GetImpl(*this).SelectText(startIndex, endIndex);
  return *this;
}

InputEditor& InputEditor::SelectWholeText()
{
  GetImpl(*this).SelectWholeText();
  return *this;
}

InputEditor& InputEditor::ClearSelection()
{
  GetImpl(*this).ClearSelection();
  return *this;
}

// =============================================================================
// Signals
// =============================================================================
Signal<void(View)>& InputEditor::TextChangedSignal()
{
  return GetImpl(*this).TextChangedSignal();
}

Signal<void(View)>& InputEditor::MaximumLengthReachedSignal()
{
  return GetImpl(*this).MaximumLengthReachedSignal();
}

Signal<void(View, Text::InputFilter::RejectReason)>& InputEditor::InputRejectedSignal()
{
  return GetImpl(*this).InputRejectedSignal();
}

Signal<void(View, uint32_t)>& InputEditor::CursorPositionChangedSignal()
{
  return GetImpl(*this).CursorPositionChangedSignal();
}

Signal<void(View)>& InputEditor::SelectionStartedSignal()
{
  return GetImpl(*this).SelectionStartedSignal();
}

Signal<void(View, uint32_t, uint32_t)>& InputEditor::SelectionChangedSignal()
{
  return GetImpl(*this).SelectionChangedSignal();
}

Signal<void(View)>& InputEditor::SelectionClearedSignal()
{
  return GetImpl(*this).SelectionClearedSignal();
}

Signal<void(View, Text::TypingStyle::Mask)>& InputEditor::TypingStyleChangedSignal()
{
  return GetImpl(*this).TypingStyleChangedSignal();
}

} // namespace Ui

} // namespace Dali
