#pragma once

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
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/tap-gesture-detector.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/common-text-utils.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/decorator/text-decorator.h>
#include <dali-ui-foundation/internal/text/rendering/text-renderer.h>
#include <dali-ui-foundation/internal/text/text-anchor-control-interface.h>
#include <dali-ui-foundation/internal/text/text-control-interface.h>
#include <dali-ui-foundation/internal/text/text-editable-control-interface.h>
#include <dali-ui-foundation/internal/text/text-selectable-control-interface.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/input-editor-properties.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class InputEditorImpl;
using InputEditorImplPtr = IntrusivePtr<InputEditorImpl>;

/**
 * @brief This is the internal implementation class for InputEditor.
 *
 * @see Dali::Ui::InputEditorImpl
 */
class DALI_UI_API InputEditorImpl : public ViewImpl, public Text::ControlInterface, public Text::EditableControlInterface, public Text::SelectableControlInterface, public Text::AnchorControlInterface
{
public:
  // Creation & Destruction

  /**
   * @brief Creates a new InputEditor.
   */
  static InputEditorImplPtr New();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~InputEditorImpl();

public:
  // API

  /**
   * @copydoc Dali::Ui::InputEditor::SetText
   */
  void SetText(const Dali::String& text);

  /**
   * @copydoc Dali::Ui::InputEditor::GetText
   */
  Dali::String GetText() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetFontFamily
   */
  void SetFontFamily(const Dali::String& fontFamily);

  /**
   * @copydoc Dali::Ui::InputEditor::GetFontFamily
   */
  Dali::String GetFontFamily() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetFontSize
   */
  void SetFontSize(float fontSize);

  /**
   * @copydoc Dali::Ui::InputEditor::GetFontSize
   */
  float GetFontSize() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTextColor
   */
  void SetTextColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTextColor
   */
  UiColor GetTextColor();

  /**
   * @copydoc Dali::Ui::InputEditor::SetLineWrapMode
   */
  void SetLineWrapMode(Text::LineWrapMode mode);

  /**
   * @copydoc Dali::Ui::InputEditor::GetLineWrapMode
   */
  Text::LineWrapMode GetLineWrapMode() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetHorizontalTextAlignment
   */
  void SetHorizontalTextAlignment(Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::InputEditor::GetHorizontalTextAlignment
   */
  Text::Alignment GetHorizontalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetVerticalTextAlignment
   */
  void SetVerticalTextAlignment(Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::InputEditor::GetVerticalTextAlignment
   */
  Text::Alignment GetVerticalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetOverflowMode
   */
  void SetOverflowMode(Text::OverflowMode mode);

  /**
   * @copydoc Dali::Ui::InputEditor::GetOverflowMode
   */
  Text::OverflowMode GetOverflowMode() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetLineHeight
   */
  void SetLineHeight(float lineHeight);

  /**
   * @copydoc Dali::Ui::InputEditor::GetLineHeight
   */
  float GetLineHeight() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetLineHeightMode
   */
  void SetLineHeightMode(Text::LineHeightMode mode);

  /**
   * @copydoc Dali::Ui::InputEditor::GetLineHeightMode
   */
  Text::LineHeightMode GetLineHeightMode() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetPlaceholder
   */
  void SetPlaceholder(const Dali::String& text);

  /**
   * @copydoc Dali::Ui::InputEditor::GetPlaceholder
   */
  Dali::String GetPlaceholder() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetPlaceholderColor
   */
  void SetPlaceholderColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputEditor::GetPlaceholderColor
   */
  UiColor GetPlaceholderColor();

  /**
   * @copydoc Dali::Ui::InputEditor::SetShowPlaceholderOnFocus
   */
  void SetShowPlaceholderOnFocus(bool enabled);

  /**
   * @copydoc Dali::Ui::InputEditor::IsPlaceholderShownOnFocus
   */
  bool IsPlaceholderShownOnFocus() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetCursorWidth
   */
  void SetCursorWidth(int width);

  /**
   * @copydoc Dali::Ui::InputEditor::GetCursorWidth
   */
  int GetCursorWidth() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetCursorColor
   */
  void SetCursorColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputEditor::GetCursorColor
   */
  UiColor GetCursorColor();

  /**
   * @copydoc Dali::Ui::InputEditor::SetCursorBlinkEnabled
   */
  void SetCursorBlinkEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputEditor::IsCursorBlinkEnabled
   */
  bool IsCursorBlinkEnabled() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetCursorBlinkInterval
   */
  void SetCursorBlinkInterval(float interval);

  /**
   * @copydoc Dali::Ui::InputEditor::GetCursorBlinkInterval
   */
  float GetCursorBlinkInterval() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetCursorPosition
   */
  void SetCursorPosition(uint32_t position);

  /**
   * @copydoc Dali::Ui::InputEditor::GetCursorPosition
   */
  uint32_t GetCursorPosition() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetSelectionEnabled
   */
  void SetSelectionEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputEditor::IsSelectionEnabled
   */
  bool IsSelectionEnabled() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetSelectionColor
   */
  void SetSelectionColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputEditor::GetSelectionColor
   */
  UiColor GetSelectionColor();

  /**
   * @copydoc Dali::Ui::InputEditor::SetTextHandleEnabled
   */
  void SetTextHandleEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputEditor::IsTextHandleEnabled
   */
  bool IsTextHandleEnabled() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTextHandleColor
   */
  void SetTextHandleColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTextHandleColor
   */
  UiColor GetTextHandleColor() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetCursorHandleImage
   */
  void SetCursorHandleImage(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputEditor::GetCursorHandleImage
   */
  Dali::String GetCursorHandleImage() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetCursorHandlePressedImage
   */
  void SetCursorHandlePressedImage(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputEditor::GetCursorHandlePressedImage
   */
  Dali::String GetCursorHandlePressedImage() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetSelectionHandleImageLeft
   */
  void SetSelectionHandleImageLeft(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputEditor::GetSelectionHandleImageLeft
   */
  Dali::String GetSelectionHandleImageLeft() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetSelectionHandleImageRight
   */
  void SetSelectionHandleImageRight(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputEditor::GetSelectionHandleImageRight
   */
  Dali::String GetSelectionHandleImageRight() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetSelectionHandlePressedImageLeft
   */
  void SetSelectionHandlePressedImageLeft(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputEditor::GetSelectionHandlePressedImageLeft
   */
  Dali::String GetSelectionHandlePressedImageLeft() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetSelectionHandlePressedImageRight
   */
  void SetSelectionHandlePressedImageRight(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputEditor::GetSelectionHandlePressedImageRight
   */
  Dali::String GetSelectionHandlePressedImageRight() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetMaximumLength
   */
  void SetMaximumLength(int length);

  /**
   * @copydoc Dali::Ui::InputEditor::GetMaximumLength
   */
  int GetMaximumLength() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetInputFilter
   */
  void SetInputFilter(const Text::InputFilter& inputFilter);

  /**
   * @copydoc Dali::Ui::InputEditor::ClearInputFilter
   */
  void ClearInputFilter();

  /**
   * @copydoc Dali::Ui::InputEditor::SetLayoutDirectionMode
   */
  void SetLayoutDirectionMode(Text::LayoutDirectionMode mode);

  /**
   * @copydoc Dali::Ui::InputEditor::GetLayoutDirectionMode
   */
  Text::LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetMarkupEnabled
   */
  void SetMarkupEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputEditor::IsMarkupEnabled
   */
  bool IsMarkupEnabled() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetFontWeight
   */
  void SetFontWeight(Text::FontWeight weight);

  /**
   * @copydoc Dali::Ui::InputEditor::GetFontWeight
   */
  Text::FontWeight GetFontWeight() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetFontWidth
   */
  void SetFontWidth(Text::FontWidth width);

  /**
   * @copydoc Dali::Ui::InputEditor::GetFontWidth
   */
  Text::FontWidth GetFontWidth() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetFontSlant
   */
  void SetFontSlant(Text::FontSlant slant);

  /**
   * @copydoc Dali::Ui::InputEditor::GetFontSlant
   */
  Text::FontSlant GetFontSlant() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTextBackgroundColor
   */
  void SetTextBackgroundColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTextBackgroundColor
   */
  UiColor GetTextBackgroundColor() const;

  /**
   * @copydoc Dali::Ui::InputEditor::ClearTextBackgroundColor
   */
  void ClearTextBackgroundColor();

  /**
   * @copydoc Dali::Ui::InputEditor::SetUnderline
   */
  void SetUnderline(const Text::Underline& underline);

  /**
   * @copydoc Dali::Ui::InputEditor::ClearUnderline
   */
  void ClearUnderline();

  /**
   * @copydoc Dali::Ui::InputEditor::SetShadow
   */
  void SetShadow(const Text::Shadow& shadow);

  /**
   * @copydoc Dali::Ui::InputEditor::ClearShadow
   */
  void ClearShadow();

  /**
   * @copydoc Dali::Ui::InputEditor::SetOutline
   */
  void SetOutline(const Text::Outline& outline);

  /**
   * @copydoc Dali::Ui::InputEditor::ClearOutline
   */
  void ClearOutline();

  /**
   * @copydoc Dali::Ui::InputEditor::SetLineThrough
   */
  void SetLineThrough(const Text::LineThrough& lineThrough);

  /**
   * @copydoc Dali::Ui::InputEditor::ClearLineThrough
   */
  void ClearLineThrough();

  /**
   * @copydoc Dali::Ui::InputEditor::SetFontSizeScale
   */
  void SetFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::InputEditor::GetFontSizeScale
   */
  float GetFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetMinimumFontSizeScale
   */
  void SetMinimumFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::InputEditor::GetMinimumFontSizeScale
   */
  float GetMinimumFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetMaximumFontSizeScale
   */
  void SetMaximumFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::InputEditor::GetMaximumFontSizeScale
   */
  float GetMaximumFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetSystemFontSizeScaleEnabled
   */
  void SetSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputEditor::IsSystemFontSizeScaleEnabled
   */
  bool IsSystemFontSizeScaleEnabled() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTypingTextColor
   */
  void SetTypingTextColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTypingTextColor
   */
  UiColor GetTypingTextColor() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTypingFontFamily
   */
  void SetTypingFontFamily(const Dali::String& fontFamily);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTypingFontFamily
   */
  Dali::String GetTypingFontFamily() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTypingFontSize
   */
  void SetTypingFontSize(float fontSize);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTypingFontSize
   */
  float GetTypingFontSize() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTypingFontWeight
   */
  void SetTypingFontWeight(Text::FontWeight weight);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTypingFontWeight
   */
  Text::FontWeight GetTypingFontWeight() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTypingFontWidth
   */
  void SetTypingFontWidth(Text::FontWidth width);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTypingFontWidth
   */
  Text::FontWidth GetTypingFontWidth() const;

  /**
   * @copydoc Dali::Ui::InputEditor::SetTypingFontSlant
   */
  void SetTypingFontSlant(Text::FontSlant slant);

  /**
   * @copydoc Dali::Ui::InputEditor::GetTypingFontSlant
   */
  Text::FontSlant GetTypingFontSlant() const;

  /**
   * @see Dali::Ui::InputEditor::SetFontVariation
   */
  void SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes);

  /**
   * @see Dali::Ui::InputEditor::SetFontVariation(const Dali::String&)
   */
  void SetFontVariation(const Dali::String& settings);

  /**
   * @copydoc Dali::Ui::InputEditor::GetFontVariation
   */
  Dali::Vector<Text::FontVariationAxis> GetFontVariation() const;

  /**
   * @copydoc Dali::Ui::InputEditor::ClearFontVariation
   */
  void ClearFontVariation();

  /**
   * @brief Sets the additional spacing between letters in pixels.
   *
   * Positive values increase the spacing, while negative values reduce it.
   *
   * @param[in] spacing The additional letter spacing in pixels.
   */
  void SetLetterSpacing(float spacing);

  /**
   * @brief Gets the additional spacing between letters in pixels.
   *
   * @return The additional letter spacing in pixels.
   */
  float GetLetterSpacing() const;

  // Read Only
  /**
   * @copydoc Dali::Ui::InputEditor::GetAdjustedFontSizeScale
   */
  float GetAdjustedFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::InputEditor::GetSelectedTextStart
   */
  uint32_t GetSelectedTextStart() const;

  /**
   * @copydoc Dali::Ui::InputEditor::GetSelectedTextEnd
   */
  uint32_t GetSelectedTextEnd() const;

  // Method

public: // Signals
  /**
   * @copydoc Dali::Ui::InputEditor::TextChangedSignal()
   */
  Signal<void(View)>& TextChangedSignal();

  /**
   * @copydoc Dali::Ui::InputEditor::MaximumLengthReachedSignal()
   */
  Signal<void(View)>& MaximumLengthReachedSignal();

  /**
   * @copydoc Dali::Ui::InputEditor::InputRejectedSignal()
   */
  Signal<void(View, Text::InputFilter::RejectReason)>& InputRejectedSignal();

  /**
   * @copydoc Dali::Ui::InputEditor::CursorPositionChangedSignal()
   */
  Signal<void(View, uint32_t)>& CursorPositionChangedSignal();

  /**
   * @copydoc Dali::Ui::InputEditor::SelectionStartedSignal()
   */
  Signal<void(View)>& SelectionStartedSignal();

  /**
   * @copydoc Dali::Ui::InputEditor::SelectionChangedSignal()
   */
  Signal<void(View, uint32_t, uint32_t)>& SelectionChangedSignal();

  /**
   * @copydoc Dali::Ui::InputEditor::SelectionClearedSignal()
   */
  Signal<void(View)>& SelectionClearedSignal();

  /**
   * @copydoc Dali::Ui::InputEditor::TypingStyleChangedSignal()
   */
  Signal<void(View, Text::TypingStyle::Mask)>& TypingStyleChangedSignal();

protected:
  // Construction

  /**
   * @brief InputEditorImpl constructor.
   */
  InputEditorImpl();

public: // Config
  /**
   * @brief Applies default values from UiConfigManager if initialized.
   */
  void ApplyInitialConfig();

public: // UiScale
  /**
   * @brief Sets the UI scale used for text-specific metrics.
   *
   * This scale is used for text-specific size calculation, such as font size.
   *
   * @param[in] scale The text UI scale.
   * @return True if the scale was changed, false otherwise.
   */
  bool SetTextUiScale(float scale);

  /**
   * @brief Gets the UI scale used for text-specific metrics.
   *
   * @return The text UI scale.
   */
  float GetTextUiScale() const;

  /**
   * @brief Gets the effective padding used for text layout.
   *
   * The view padding is adjusted by the current text UI scale.
   *
   * @return The effective text padding.
   */
  Extents GetEffectiveTextPadding() const;

public: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc ViewImpl::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc ViewImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc ViewImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

private: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnFocusChanged()
   */
  void OnFocusChanged(bool focused) override;

  /**
   * @copydoc ViewImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc ViewImpl::OnKeyEvent()
   */
  bool OnKeyEvent(const KeyEvent& event) override;

protected: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

public: // From ControlInterface
  /**
   * @copydoc Text::ControlInterface::RequestTextRelayout()
   */
  void RequestTextRelayout() override;

  /**
   * @copydoc Text::ControlInterface::InvalidateTextMeasure()
   */
  void InvalidateTextMeasure() override;

  /**
   * @copydoc Text::ControlInterface::RequestAsyncRender()
   */
  void RequestAsyncRender() override;

  // From EditableControlInterface

  /**
   * @copydoc Text::EditableControlInterface::AddDecoration()
   */
  void AddDecoration(Actor& actor, Ui::Text::DecorationType type, bool needsClipping) override;

  /**
   * @copydoc Text::EditableControlInterface::GetControlBackgroundColor()
   */
  void GetControlBackgroundColor(Vector4& color) const override;

  /**
   * @copydoc Text::EditableControlInterface::IsEditable()
   */
  bool IsEditable() const override;

  /**
   * @copydoc Text::EditableControlInterface::SetEditable()
   */
  void SetEditable(bool editable) override;

  /**
   * @copydoc Dali::EditableControlInterface::CopyText()
   */
  string CopyText() override;

  /**
   * @copydoc Dali::EditableControlInterface::CutText()
   */
  string CutText() override;

  /**
   * @copydoc Text::EditableControlInterface::PasteText()
   */
  void PasteText() override;

  /**
   * @copydoc Text::EditableControlInterface::TextChanged()
   */
  void TextChanged(bool immediate) override;

  /**
   * @copydoc Text::EditableControlInterface::MaximumLengthReached()
   */
  void MaximumLengthReached() override;

  /**
   * @copydoc Text::EditableControlInterface::InputRejected()
   */
  void InputRejected(Text::InputFilter::RejectReason reason) override;

  /**
   * @copydoc Text::EditableControlInterface::CursorPositionChanged()
   */
  void CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition) override;

  /**
   * @copydoc Text::EditableControlInterface::InputStyleChanged()
   */
  void InputStyleChanged(Text::InputStyle::Mask inputStyleMask) override;

  /**
   * @copydoc Text::EditableControlInterface::TextChanged()
   */
  void TextInserted(unsigned int position, unsigned int length, const std::string& content) override;

  /**
   * @copydoc Text::EditableControlInterface::TextDeleted()
   */
  void TextDeleted(unsigned int position, unsigned int length, const std::string& content) override;

  // From SelectableControlInterface
public:
  /**
   * @copydoc Text::SelectableControlInterface::SelectText()
   */
  void SelectText(const uint32_t start, const uint32_t end) override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectWholeText()
   */
  void SelectWholeText() override;

  /**
   * @copydoc Text::SelectableControlInterface::ClearSelection()
   */
  void ClearSelection() override;

  /**
   * @copydoc Text::SelectableControlInterface::GetSelectedText()
   */
  Dali::String GetSelectedText() const override;

  /**
   * @copydoc Text::SelectableControlInterface::SetTextSelectionRange()
   */
  void SetTextSelectionRange(const uint32_t* start, const uint32_t* end) override;

  /**
   * @copydoc Text::SelectableControlInterface::GetTextSelectionRange()
   */
  Uint32Pair GetTextSelectionRange() const override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectionChanged()
   */
  void SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd) override;

  // From AnchorControlInterface
public:
  /**
   * @copydoc Text::AnchorControlInterface::AnchorClicked()
   */
  bool AnchorClicked(uint32_t cursorPosition, std::string& href) override;

  /**
   * @copydoc Text::AnchorControlInterface::EmitAnchorClicked()
   */
  void EmitAnchorClicked(const std::string& href) override;

private: // Implementation
  /**
   * @brief Handles focus gain (IME activation, controller notification).
   */
  void OnFocusGained();

  /**
   * @brief Handles focus loss (IME deactivation, controller notification).
   */
  void OnFocusLost();

  /**
   * @brief Updates the effective line height based on the current LineHeightMode.
   */
  void UpdateLineHeight();

  /**
   * @copydoc Dali::Ui::Text::Controller::(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
   */
  InputMethodContext::CallbackData OnInputMethodContextEvent(InputMethodContext inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief Connection needed to re-render text, when a Input Editor returns to the scene.
   */
  void OnSceneConnect(Dali::Actor actor);

  /**
   * @brief Callback when a tap gesture is detected.
   */
  void OnTapDetected(Actor actor, TapGesture tap);

  /**
   * @brief Callback when a pan gesture is detected.
   */
  void OnPanDetected(Actor actor, PanGesture pan);

  /**
   * @brief Callback when a long press gesture is detected.
   */
  void OnLongPressDetected(Actor actor, LongPressGesture longPress);

  /**
   * @brief Callback when InputEditor is touched
   *
   * @param[in] actor InputEditor touched
   * @param[in] touch Touch information
   */
  bool OnTouched(Actor actor, TouchEvent touch);

  /**
   * @brief Callback function for when the layout is changed.
   * @param[in] actor The actor whose layoutDirection is changed.
   * @param[in] type  The layoutDirection.
   */
  void OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type);

  /**
   * @brief Callback function for when the locale is changed.
   * @param[in] locale The new system locale.
   */
  void OnLocaleChanged(std::string locale);

  /**
   * @brief Callback when keyboard is shown/hidden.
   * @param[in] keyboardShown True if keyboard is shown.
   */
  void OnKeyboardStatusChanged(bool keyboardShown);

  /**
   * @brief Enable or disable clipping.
   */
  void EnableClipping();

  /**
   * @brief Resize actor to the given size.
   *
   * @param[in] actor The actor to be resized.
   * @param[in] size Size to change.
   */
  void ResizeActor(Actor& actor, const Vector2& size);

  /**
   * @brief Add a layer for active or cursor.
   * @param[in] layer The actor in which to store the layer.
   * @param[in] actor The new layer to add.
   */
  void AddLayer(Actor& layer, Actor& actor);

  /**
   * @brief Render view, create and attach actor(s) to this Input Editor.
   */
  void RenderText(Text::Controller::UpdateTextType updateTextType);

  /**
   * @brief Emits TextChanged signal.
   */
  void EmitTextChanged();

  /**
   * @brief Emits MaximumLengthReached signal.
   */
  void EmitMaximumLengthReached();

  /**
   * @brief Emits InputRejected signal.
   */
  void EmitInputRejected(Text::InputFilter::RejectReason reason);

  /**
   * @brief Emits CursorPositionChanged signal.
   */
  void EmitCursorPositionChanged();

  /**
   * @brief Emits SelectionStarted signal.
   */
  void EmitSelectionStarted();

  /**
   * @brief Emits SelectionChanged signal.
   */
  void EmitSelectionChanged();

  /**
   * @brief Emits SelectionCleared signal.
   */
  void EmitSelectionCleared();

  /**
   * @brief Emits TypingStyleChanged signal.
   */
  void EmitTypingStyleChanged(Text::TypingStyle::Mask mask);

private: // UiColorManager
  void SetTextColorInternal(const Vector4& color);
  void SetPlaceholderColorInternal(const Vector4& color);
  void SetCursorColorInternal(const Vector4& color);
  void SetSelectionColorInternal(const Vector4& color);
  void SetTextHandleColorInternal(const Vector4& color);
  void SetTextBackgroundColorInternal(const Vector4& color);
  void SetUnderlineColorInternal(const Vector4& color);
  void SetShadowColorInternal(const Vector4& color);
  void SetOutlineColorInternal(const Vector4& color);
  void SetLineThroughColorInternal(const Vector4& color);
  void SetTypingTextColorInternal(const Vector4& color);

  // Properties
public:
  /**
   * @copydoc View::OnPropertySet()
   */
  void OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue) override;

  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Dali::Property::Value GetProperty(BaseObject* object, Dali::Property::Index index);

private:
  // Not copyable or movable
  InputEditorImpl(const InputEditorImpl&)            = delete;
  InputEditorImpl(InputEditorImpl&&)                 = delete;
  InputEditorImpl& operator=(const InputEditorImpl&) = delete;
  InputEditorImpl& operator=(InputEditorImpl&&)      = delete;

private:
  // Data
  Signal<void(View)>                                  mTextChangedSignal;
  Signal<void(View)>                                  mMaxLengthReachedSignal;
  Signal<void(View, Text::InputFilter::RejectReason)> mInputRejectedSignal;
  Signal<void(View, uint32_t)>                        mCursorPositionChangedSignal;
  Signal<void(View)>                                  mSelectionStartedSignal;
  Signal<void(View, uint32_t, uint32_t)>              mSelectionChangedSignal;
  Signal<void(View)>                                  mSelectionClearedSignal;
  Signal<void(View, Text::TypingStyle::Mask)>         mTypingStyleChangedSignal;

  InputMethodContext          mInputMethodContext;
  TapGestureDetector          mTapGestureDetector;
  PanGestureDetector          mPanGestureDetector;
  LongPressGestureDetector    mLongPressGestureDetector;
  Text::ControllerPtr         mController;
  Text::RendererPtr           mRenderer;
  Text::DecoratorPtr          mDecorator;
  Actor                       mStencil;
  std::vector<Actor>          mClippingDecorationActors; ///< Decoration actors which need clipping.
  std::vector<Ui::TextAnchor> mAnchorActors;
  Dali::InputMethodOptions    mInputMethodOptions;

  Actor mRenderableActor;
  Actor mActiveLayer;
  Actor mCursorLayer;
  Actor mBackgroundActor;

  float                mLineHeight;
  Text::LineHeightMode mLineHeightMode;
  Text::OverflowMode   mOverflowMode;
  float                mAlignmentOffset;
  bool                 mMeasureInvalidated : 1;
  bool                 mHasBeenStaged : 1;
  bool                 mTextChanged : 1;           ///< If true, emits TextChangedSignal in next OnRelayout().
  bool                 mCursorPositionChanged : 1; ///< If true, emits CursorPositionChangedSignal at the end of OnRelayout().
  bool                 mSelectionStarted : 1;      ///< If true, emits SelectionStartedSignal at the end of OnRelayout().
  bool                 mSelectionChanged : 1;      ///< If true, emits SelectionChangedSignal at the end of OnRelayout().
  bool                 mSelectionCleared : 1;      ///< If true, emits SelectionClearedSignal at the end of OnRelayout().

protected:
  struct PropertyHandler;
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
