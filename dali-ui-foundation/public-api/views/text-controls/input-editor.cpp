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
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/public-api/animation/input-editor-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/input-editor-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>
#include <dali/devel-api/object/type-registry.h>

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

float InputEditor::GetHeightForWidth(float width)
{
  return GetImpl(*this).GetHeightForWidth(width);
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

InputEditorAnimationBridge InputEditor::Animate(Animation animation)
{
  return InputEditorAnimationBridge(animation, *this);
}

InputEditorAnimationSpec InputEditor::NewAnimationSpec()
{
  return InputEditorAnimationSpec::New();
}

void InputEditor::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
}

Dali::String InputEditor::GetText() const
{
  return GetImpl(*this).GetText();
}

void InputEditor::SetStyledText(const Text::StyledText& styledText)
{
  GetImpl(*this).SetStyledText(styledText);
}

void InputEditor::SetFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
}

Dali::String InputEditor::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

void InputEditor::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
}

float InputEditor::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

void InputEditor::SetTextColor(const UiColor& color)
{
  GetImpl(*this).SetTextColor(color);
}

UiColor InputEditor::GetTextColor()
{
  return GetImpl(*this).GetTextColor();
}

void InputEditor::SetTextGradient(const Gradient::Base& gradient)
{
  GetImpl(*this).SetTextGradient(gradient);
}

Gradient::Base InputEditor::GetTextGradient() const
{
  return GetImpl(*this).GetTextGradient();
}

void InputEditor::SetTextGradientBoundsMode(Text::GradientBoundsMode mode)
{
  GetImpl(*this).SetTextGradientBoundsMode(mode);
}

Text::GradientBoundsMode InputEditor::GetTextGradientBoundsMode() const
{
  return GetImpl(*this).GetTextGradientBoundsMode();
}

void InputEditor::SetLineWrapMode(Text::LineWrapMode mode)
{
  GetImpl(*this).SetLineWrapMode(mode);
}

Text::LineWrapMode InputEditor::GetLineWrapMode() const
{
  return GetImpl(*this).GetLineWrapMode();
}

void InputEditor::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetHorizontalTextAlignment(alignment);
}

Text::Alignment InputEditor::GetHorizontalTextAlignment() const
{
  return GetImpl(*this).GetHorizontalTextAlignment();
}

void InputEditor::SetVerticalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetVerticalTextAlignment(alignment);
}

Text::Alignment InputEditor::GetVerticalTextAlignment() const
{
  return GetImpl(*this).GetVerticalTextAlignment();
}

void InputEditor::SetTextOverflowMode(Text::OverflowMode mode)
{
  GetImpl(*this).SetTextOverflowMode(mode);
}

Text::OverflowMode InputEditor::GetTextOverflowMode() const
{
  return GetImpl(*this).GetTextOverflowMode();
}

void InputEditor::SetLineHeight(float lineHeight)
{
  GetImpl(*this).SetLineHeight(lineHeight);
}

float InputEditor::GetLineHeight() const
{
  return GetImpl(*this).GetLineHeight();
}

void InputEditor::SetLineHeightMode(Text::LineHeightMode mode)
{
  GetImpl(*this).SetLineHeightMode(mode);
}

Text::LineHeightMode InputEditor::GetLineHeightMode() const
{
  return GetImpl(*this).GetLineHeightMode();
}

void InputEditor::SetPlaceholder(const Dali::String& text)
{
  GetImpl(*this).SetPlaceholder(text);
}

Dali::String InputEditor::GetPlaceholder() const
{
  return GetImpl(*this).GetPlaceholder();
}

void InputEditor::SetPlaceholderColor(const UiColor& color)
{
  GetImpl(*this).SetPlaceholderColor(color);
}

UiColor InputEditor::GetPlaceholderColor()
{
  return GetImpl(*this).GetPlaceholderColor();
}

void InputEditor::SetPlaceholderTextGradient(const Gradient::Base& gradient)
{
  GetImpl(*this).SetPlaceholderTextGradient(gradient);
}

Gradient::Base InputEditor::GetPlaceholderTextGradient() const
{
  return GetImpl(*this).GetPlaceholderTextGradient();
}

void InputEditor::SetShowPlaceholderOnFocus(bool enabled)
{
  GetImpl(*this).SetShowPlaceholderOnFocus(enabled);
}

bool InputEditor::IsPlaceholderShownOnFocus() const
{
  return GetImpl(*this).IsPlaceholderShownOnFocus();
}

void InputEditor::SetCursorWidth(int width)
{
  GetImpl(*this).SetCursorWidth(width);
}

int InputEditor::GetCursorWidth() const
{
  return GetImpl(*this).GetCursorWidth();
}

void InputEditor::SetCursorColor(const UiColor& color)
{
  GetImpl(*this).SetCursorColor(color);
}

UiColor InputEditor::GetCursorColor()
{
  return GetImpl(*this).GetCursorColor();
}

void InputEditor::SetCursorBlinkEnabled(bool enabled)
{
  GetImpl(*this).SetCursorBlinkEnabled(enabled);
}

bool InputEditor::IsCursorBlinkEnabled() const
{
  return GetImpl(*this).IsCursorBlinkEnabled();
}

void InputEditor::SetCursorBlinkInterval(float interval)
{
  GetImpl(*this).SetCursorBlinkInterval(interval);
}

float InputEditor::GetCursorBlinkInterval() const
{
  return GetImpl(*this).GetCursorBlinkInterval();
}

void InputEditor::SetCursorPosition(uint32_t position)
{
  GetImpl(*this).SetCursorPosition(position);
}

uint32_t InputEditor::GetCursorPosition() const
{
  return GetImpl(*this).GetCursorPosition();
}

void InputEditor::SetSelectionEnabled(bool enabled)
{
  GetImpl(*this).SetSelectionEnabled(enabled);
}

bool InputEditor::IsSelectionEnabled() const
{
  return GetImpl(*this).IsSelectionEnabled();
}

void InputEditor::SetSelectionColor(const UiColor& color)
{
  GetImpl(*this).SetSelectionColor(color);
}

UiColor InputEditor::GetSelectionColor()
{
  return GetImpl(*this).GetSelectionColor();
}

void InputEditor::SetTextHandleEnabled(bool enabled)
{
  GetImpl(*this).SetTextHandleEnabled(enabled);
}

bool InputEditor::IsTextHandleEnabled() const
{
  return GetImpl(*this).IsTextHandleEnabled();
}

void InputEditor::SetTextHandleColor(const UiColor& color)
{
  GetImpl(*this).SetTextHandleColor(color);
}

UiColor InputEditor::GetTextHandleColor() const
{
  return GetImpl(*this).GetTextHandleColor();
}

void InputEditor::SetCursorHandleImage(const Dali::String& image)
{
  GetImpl(*this).SetCursorHandleImage(image);
}

Dali::String InputEditor::GetCursorHandleImage() const
{
  return GetImpl(*this).GetCursorHandleImage();
}

void InputEditor::SetCursorHandlePressedImage(const Dali::String& image)
{
  GetImpl(*this).SetCursorHandlePressedImage(image);
}

Dali::String InputEditor::GetCursorHandlePressedImage() const
{
  return GetImpl(*this).GetCursorHandlePressedImage();
}

void InputEditor::SetSelectionHandleImageLeft(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandleImageLeft(image);
}

Dali::String InputEditor::GetSelectionHandleImageLeft() const
{
  return GetImpl(*this).GetSelectionHandleImageLeft();
}

void InputEditor::SetSelectionHandleImageRight(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandleImageRight(image);
}

Dali::String InputEditor::GetSelectionHandleImageRight() const
{
  return GetImpl(*this).GetSelectionHandleImageRight();
}

void InputEditor::SetSelectionHandlePressedImageLeft(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandlePressedImageLeft(image);
}

Dali::String InputEditor::GetSelectionHandlePressedImageLeft() const
{
  return GetImpl(*this).GetSelectionHandlePressedImageLeft();
}

void InputEditor::SetSelectionHandlePressedImageRight(const Dali::String& image)
{
  GetImpl(*this).SetSelectionHandlePressedImageRight(image);
}

Dali::String InputEditor::GetSelectionHandlePressedImageRight() const
{
  return GetImpl(*this).GetSelectionHandlePressedImageRight();
}

void InputEditor::SetMaximumLength(int length)
{
  GetImpl(*this).SetMaximumLength(length);
}

int InputEditor::GetMaximumLength() const
{
  return GetImpl(*this).GetMaximumLength();
}

void InputEditor::SetInputFilter(const Text::InputFilter& inputFilter)
{
  GetImpl(*this).SetInputFilter(inputFilter);
}

Text::InputFilter InputEditor::GetInputFilter() const
{
  return GetImpl(*this).GetInputFilter();
}

void InputEditor::SetEditable(bool editable)
{
  GetImpl(*this).SetEditable(editable);
}

bool InputEditor::IsEditable() const
{
  return GetImpl(*this).IsEditable();
}

void InputEditor::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  GetImpl(*this).SetLayoutDirectionMode(mode);
}

Text::LayoutDirectionMode InputEditor::GetLayoutDirectionMode() const
{
  return GetImpl(*this).GetLayoutDirectionMode();
}

void InputEditor::SetFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetFontWeight(weight);
}

Text::FontWeight InputEditor::GetFontWeight() const
{
  return GetImpl(*this).GetFontWeight();
}

void InputEditor::SetFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetFontWidth(width);
}

Text::FontWidth InputEditor::GetFontWidth() const
{
  return GetImpl(*this).GetFontWidth();
}

void InputEditor::SetFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetFontSlant(slant);
}

Text::FontSlant InputEditor::GetFontSlant() const
{
  return GetImpl(*this).GetFontSlant();
}

void InputEditor::SetTextBackgroundColor(const UiColor& color)
{
  GetImpl(*this).SetTextBackgroundColor(color);
}

UiColor InputEditor::GetTextBackgroundColor() const
{
  return GetImpl(*this).GetTextBackgroundColor();
}

void InputEditor::ClearTextBackgroundColor()
{
  GetImpl(*this).ClearTextBackgroundColor();
}

void InputEditor::SetTextUnderline(const Text::Underline& underline)
{
  GetImpl(*this).SetTextUnderline(underline);
}

Text::Underline InputEditor::GetTextUnderline() const
{
  return GetImpl(*this).GetTextUnderline();
}

void InputEditor::SetTextShadow(const Text::Shadow& shadow)
{
  GetImpl(*this).SetTextShadow(shadow);
}

Text::Shadow InputEditor::GetTextShadow() const
{
  return GetImpl(*this).GetTextShadow();
}

void InputEditor::SetTextOutline(const Text::Outline& outline)
{
  GetImpl(*this).SetTextOutline(outline);
}

Text::Outline InputEditor::GetTextOutline() const
{
  return GetImpl(*this).GetTextOutline();
}

void InputEditor::SetTextLineThrough(const Text::LineThrough& lineThrough)
{
  GetImpl(*this).SetTextLineThrough(lineThrough);
}

Text::LineThrough InputEditor::GetTextLineThrough() const
{
  return GetImpl(*this).GetTextLineThrough();
}

void InputEditor::SetMinimumFontSizeScale(float scale)
{
  GetImpl(*this).SetMinimumFontSizeScale(scale);
}

float InputEditor::GetMinimumFontSizeScale() const
{
  return GetImpl(*this).GetMinimumFontSizeScale();
}

void InputEditor::SetMaximumFontSizeScale(float scale)
{
  GetImpl(*this).SetMaximumFontSizeScale(scale);
}

float InputEditor::GetMaximumFontSizeScale() const
{
  return GetImpl(*this).GetMaximumFontSizeScale();
}

void InputEditor::SetSystemFontSizeScaleEnabled(bool enabled)
{
  GetImpl(*this).SetSystemFontSizeScaleEnabled(enabled);
}

bool InputEditor::IsSystemFontSizeScaleEnabled() const
{
  return GetImpl(*this).IsSystemFontSizeScaleEnabled();
}

void InputEditor::SetAutoGrowEnabled(bool enabled)
{
  GetImpl(*this).SetAutoGrowEnabled(enabled);
}

bool InputEditor::IsAutoGrowEnabled() const
{
  return GetImpl(*this).IsAutoGrowEnabled();
}

void InputEditor::SetTypingTextColor(const UiColor& color)
{
  GetImpl(*this).SetTypingTextColor(color);
}

UiColor InputEditor::GetTypingTextColor() const
{
  return GetImpl(*this).GetTypingTextColor();
}

void InputEditor::SetTypingFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetTypingFontFamily(fontFamily);
}

Dali::String InputEditor::GetTypingFontFamily() const
{
  return GetImpl(*this).GetTypingFontFamily();
}

void InputEditor::SetTypingFontSize(float fontSize)
{
  GetImpl(*this).SetTypingFontSize(fontSize);
}

float InputEditor::GetTypingFontSize() const
{
  return GetImpl(*this).GetTypingFontSize();
}

void InputEditor::SetTypingFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetTypingFontWeight(weight);
}

Text::FontWeight InputEditor::GetTypingFontWeight() const
{
  return GetImpl(*this).GetTypingFontWeight();
}

void InputEditor::SetTypingFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetTypingFontWidth(width);
}

Text::FontWidth InputEditor::GetTypingFontWidth() const
{
  return GetImpl(*this).GetTypingFontWidth();
}

void InputEditor::SetTypingFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetTypingFontSlant(slant);
}

Text::FontSlant InputEditor::GetTypingFontSlant() const
{
  return GetImpl(*this).GetTypingFontSlant();
}

void InputEditor::SetFontVariation(const Dali::Vector<Text::FontVariation::Axis>& axes)
{
  GetImpl(*this).SetFontVariation(axes);
}

void InputEditor::SetFontVariation(const Dali::String& settings)
{
  GetImpl(*this).SetFontVariation(settings);
}

Dali::Vector<Text::FontVariation::Axis> InputEditor::GetFontVariation() const
{
  return GetImpl(*this).GetFontVariation();
}

void InputEditor::SetTranslatablePlaceholder(StringView resourceId)
{
  GetImpl(*this).SetTranslatablePlaceholder(resourceId);
}

void InputEditor::SetTranslatablePlaceholder(StringView resourceId, StringView domain)
{
  GetImpl(*this).SetTranslatablePlaceholder(resourceId, domain);
}

Dali::String InputEditor::GetTranslatablePlaceholder() const
{
  return GetImpl(*this).GetTranslatablePlaceholder();
}

void InputEditor::ClearTranslatablePlaceholder()
{
  GetImpl(*this).ClearTranslatablePlaceholder();
}
int InputEditor::GetLineCount()
{
  return GetImpl(*this).GetLineCount();
}

int InputEditor::GetLineCount(float width)
{
  return GetImpl(*this).GetLineCount(width);
}

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
void InputEditor::SelectText(uint32_t startIndex, uint32_t endIndex)
{
  GetImpl(*this).SelectText(startIndex, endIndex);
}

void InputEditor::SelectWholeText()
{
  GetImpl(*this).SelectWholeText();
}

void InputEditor::ClearSelection()
{
  GetImpl(*this).ClearSelection();
}

InputMethodContext InputEditor::GetInputMethodContext()
{
  return GetImpl(*this).GetInputMethodContext();
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
