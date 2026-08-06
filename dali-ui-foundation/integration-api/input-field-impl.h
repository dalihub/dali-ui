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
#include <dali/integration-api/adaptor-framework/input-method-context-integ.h>
#include <dali/integration-api/system/system-settings.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/size-negotiated-view-impl.h>
#include <dali-ui-foundation/integration-api/text/text-anchor-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-atlas-gradient-apply-state.h>
#include <dali-ui-foundation/integration-api/text/text-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-editable-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-selectable-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-update-type.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/input-field-properties.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{

namespace Ui
{

class TextAnchor;

namespace Internal
{
namespace Text
{
struct EditableTextGradientPropertyData;
using EditableTextGradientPropertyDataPtr = std::unique_ptr<EditableTextGradientPropertyData>;
} // namespace Text
} // namespace Internal

namespace Text
{
class Controller;
class Decorator;
class Renderer;

using ControllerPtr = IntrusivePtr<Controller>;
using DecoratorPtr  = IntrusivePtr<Decorator>;
using RendererPtr   = IntrusivePtr<Renderer>;
} // namespace Text

namespace Integration
{

class InputFieldAccessible;
class InputFieldImpl;
using InputFieldImplPtr = IntrusivePtr<InputFieldImpl>;

/**
 * @brief This is the internal implementation class for InputField.
 *
 * @see Dali::Ui::InputFieldImpl
 */
class DALI_UI_API InputFieldImpl : public SizeNegotiatedViewImpl,
                                   public Text::ControlInterface,
                                   public Text::EditableControlInterface,
                                   public Text::SelectableControlInterface,
                                   public Text::AnchorControlInterface
{
  friend class InputFieldAccessible;

public:
  // Creation & Destruction

  /**
   * @brief Creates a new InputField.
   */
  static InputFieldImplPtr New();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~InputFieldImpl();

public:
  // API

  /**
   * @copydoc Dali::Ui::InputField::SetText
   */
  void SetText(const Dali::String& text);

  /**
   * @copydoc Dali::Ui::InputField::GetText
   */
  Dali::String GetText() const;

  /**
   * @copydoc Dali::Ui::InputField::SetStyledText
   */
  void SetStyledText(const Ui::Text::StyledText& styledText);

  /**
   * @copydoc Dali::Ui::InputField::SetFontFamily
   */
  void SetFontFamily(const Dali::String& fontFamily);

  /**
   * @copydoc Dali::Ui::InputField::GetFontFamily
   */
  Dali::String GetFontFamily() const;

  /**
   * @copydoc Dali::Ui::InputField::SetFontSize
   */
  void SetFontSize(float fontSize);

  /**
   * @copydoc Dali::Ui::InputField::GetFontSize
   */
  float GetFontSize() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTextColor
   */
  void SetTextColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputField::GetTextColor
   */
  UiColor GetTextColor();

  /**
   * @copydoc Dali::Ui::InputField::SetHorizontalTextAlignment
   */
  void SetHorizontalTextAlignment(Ui::Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::InputField::GetHorizontalTextAlignment
   */
  Ui::Text::Alignment GetHorizontalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::InputField::SetVerticalTextAlignment
   */
  void SetVerticalTextAlignment(Ui::Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::InputField::GetVerticalTextAlignment
   */
  Ui::Text::Alignment GetVerticalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTextOverflowMode
   */
  void SetTextOverflowMode(Ui::Text::OverflowMode mode);

  /**
   * @copydoc Dali::Ui::InputField::GetTextOverflowMode
   */
  Ui::Text::OverflowMode GetTextOverflowMode() const;

  /**
   * @copydoc Dali::Ui::InputField::SetPlaceholder
   */
  void SetPlaceholder(const Dali::String& text);

  /**
   * @copydoc Dali::Ui::InputField::GetPlaceholder
   */
  Dali::String GetPlaceholder() const;

  /**
   * @copydoc Dali::Ui::InputField::SetPlaceholderColor
   */
  void SetPlaceholderColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputField::GetPlaceholderColor
   */
  UiColor GetPlaceholderColor();

  /**
   * @copydoc Dali::Ui::InputField::SetTextGradient
   */
  void SetTextGradient(const Dali::Ui::Gradient::Base& gradient);

  /**
   * @copydoc Dali::Ui::InputField::GetTextGradient
   */
  Dali::Ui::Gradient::Base GetTextGradient() const;

  /**
   * @copydoc Dali::Ui::InputField::SetPlaceholderTextGradient
   */
  void SetPlaceholderTextGradient(const Dali::Ui::Gradient::Base& gradient);

  /**
   * @copydoc Dali::Ui::InputField::GetPlaceholderTextGradient
   */
  Dali::Ui::Gradient::Base GetPlaceholderTextGradient() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTextGradientBoundsMode
   */
  void SetTextGradientBoundsMode(Ui::Text::GradientBoundsMode mode);

  /**
   * @copydoc Dali::Ui::InputField::GetTextGradientBoundsMode
   */
  Ui::Text::GradientBoundsMode GetTextGradientBoundsMode() const;

  /**
   * @brief Ensures the hidden source property used by TextGradientStartOffset animation.
   *
   * @return The hidden property index, or Property::INVALID_INDEX if no renderable text gradient is authored.
   */
  Dali::Property::Index EnsureGradientAnimOffset();

  /**
   * @brief Ensures the hidden source property used by PlaceholderTextGradientStartOffset animation.
   *
   * @return The hidden property index, or Property::INVALID_INDEX if no renderable placeholder text gradient is authored.
   */
  Dali::Property::Index EnsurePlaceholderGradientAnimOffset();

  /**
   * @copydoc Dali::Ui::InputField::SetShowPlaceholderOnFocus
   */
  void SetShowPlaceholderOnFocus(bool enabled);

  /**
   * @copydoc Dali::Ui::InputField::IsPlaceholderShownOnFocus
   */
  bool IsPlaceholderShownOnFocus() const;

  /**
   * @copydoc Dali::Ui::InputField::SetCursorWidth
   */
  void SetCursorWidth(int width);

  /**
   * @copydoc Dali::Ui::InputField::GetCursorWidth
   */
  int GetCursorWidth() const;

  /**
   * @copydoc Dali::Ui::InputField::SetCursorColor
   */
  void SetCursorColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputField::GetCursorColor
   */
  UiColor GetCursorColor();

  /**
   * @copydoc Dali::Ui::InputField::SetCursorBlinkEnabled
   */
  void SetCursorBlinkEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputField::IsCursorBlinkEnabled
   */
  bool IsCursorBlinkEnabled() const;

  /**
   * @copydoc Dali::Ui::InputField::SetCursorBlinkInterval
   */
  void SetCursorBlinkInterval(float interval);

  /**
   * @copydoc Dali::Ui::InputField::GetCursorBlinkInterval
   */
  float GetCursorBlinkInterval() const;

  /**
   * @copydoc Dali::Ui::InputField::SetCursorPosition
   */
  void SetCursorPosition(uint32_t position);

  /**
   * @copydoc Dali::Ui::InputField::GetCursorPosition
   */
  uint32_t GetCursorPosition() const;

  /**
   * @copydoc Dali::Ui::InputField::SetSelectionEnabled
   */
  void SetSelectionEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputField::IsSelectionEnabled
   */
  bool IsSelectionEnabled() const;

  /**
   * @copydoc Dali::Ui::InputField::SetSelectionColor
   */
  void SetSelectionColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputField::GetSelectionColor
   */
  UiColor GetSelectionColor();

  /**
   * @copydoc Dali::Ui::InputField::SetTextHandleEnabled
   */
  void SetTextHandleEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputField::IsTextHandleEnabled
   */
  bool IsTextHandleEnabled() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTextHandleColor
   */
  void SetTextHandleColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputField::GetTextHandleColor
   */
  UiColor GetTextHandleColor() const;

  /**
   * @copydoc Dali::Ui::InputField::SetCursorHandleImage
   */
  void SetCursorHandleImage(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputField::GetCursorHandleImage
   */
  Dali::String GetCursorHandleImage() const;

  /**
   * @copydoc Dali::Ui::InputField::SetCursorHandlePressedImage
   */
  void SetCursorHandlePressedImage(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputField::GetCursorHandlePressedImage
   */
  Dali::String GetCursorHandlePressedImage() const;

  /**
   * @copydoc Dali::Ui::InputField::SetSelectionHandleImageLeft
   */
  void SetSelectionHandleImageLeft(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputField::GetSelectionHandleImageLeft
   */
  Dali::String GetSelectionHandleImageLeft() const;

  /**
   * @copydoc Dali::Ui::InputField::SetSelectionHandleImageRight
   */
  void SetSelectionHandleImageRight(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputField::GetSelectionHandleImageRight
   */
  Dali::String GetSelectionHandleImageRight() const;

  /**
   * @copydoc Dali::Ui::InputField::SetSelectionHandlePressedImageLeft
   */
  void SetSelectionHandlePressedImageLeft(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputField::GetSelectionHandlePressedImageLeft
   */
  Dali::String GetSelectionHandlePressedImageLeft() const;

  /**
   * @copydoc Dali::Ui::InputField::SetSelectionHandlePressedImageRight
   */
  void SetSelectionHandlePressedImageRight(const Dali::String& image);

  /**
   * @copydoc Dali::Ui::InputField::GetSelectionHandlePressedImageRight
   */
  Dali::String GetSelectionHandlePressedImageRight() const;

  /**
   * @copydoc Dali::Ui::InputField::SetMaximumLength
   */
  void SetMaximumLength(int length);

  /**
   * @copydoc Dali::Ui::InputField::GetMaximumLength
   */
  int GetMaximumLength() const;

  /**
   * @copydoc Dali::Ui::InputField::SetInputFilter
   */
  void SetInputFilter(const Ui::Text::InputFilter& inputFilter);

  /**
   * @copydoc Dali::Ui::InputField::GetInputFilter
   */
  Ui::Text::InputFilter GetInputFilter() const;

  /**
   * @copydoc Dali::Ui::InputField::SetPasswordMode
   */
  void SetPasswordMode(Ui::Text::PasswordMode mode);

  /**
   * @copydoc Dali::Ui::InputField::GetPasswordMode
   */
  Ui::Text::PasswordMode GetPasswordMode() const;

  /**
   * @copydoc Dali::Ui::InputField::SetPasswordMaskCharacter
   */
  void SetPasswordMaskCharacter(uint32_t character);

  /**
   * @copydoc Dali::Ui::InputField::GetPasswordMaskCharacter
   */
  uint32_t GetPasswordMaskCharacter() const;

  /**
   * @copydoc Dali::Ui::InputField::SetPasswordRevealDuration
   */
  void SetPasswordRevealDuration(uint32_t duration);

  /**
   * @copydoc Dali::Ui::InputField::GetPasswordRevealDuration
   */
  uint32_t GetPasswordRevealDuration() const;

  /**
   * @copydoc Dali::Ui::InputField::SetLayoutDirectionMode
   */
  void SetLayoutDirectionMode(Ui::Text::LayoutDirectionMode mode);

  /**
   * @copydoc Dali::Ui::InputField::GetLayoutDirectionMode
   */
  Ui::Text::LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @copydoc Dali::Ui::InputField::SetFontWeight
   */
  void SetFontWeight(Ui::Text::FontWeight weight);

  /**
   * @copydoc Dali::Ui::InputField::GetFontWeight
   */
  Ui::Text::FontWeight GetFontWeight() const;

  /**
   * @copydoc Dali::Ui::InputField::SetFontWidth
   */
  void SetFontWidth(Ui::Text::FontWidth width);

  /**
   * @copydoc Dali::Ui::InputField::GetFontWidth
   */
  Ui::Text::FontWidth GetFontWidth() const;

  /**
   * @copydoc Dali::Ui::InputField::SetFontSlant
   */
  void SetFontSlant(Ui::Text::FontSlant slant);

  /**
   * @copydoc Dali::Ui::InputField::GetFontSlant
   */
  Ui::Text::FontSlant GetFontSlant() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTextBackgroundColor
   */
  void SetTextBackgroundColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputField::GetTextBackgroundColor
   */
  UiColor GetTextBackgroundColor() const;

  /**
   * @copydoc Dali::Ui::InputField::ClearTextBackgroundColor
   */
  void ClearTextBackgroundColor();

  /**
   * @copydoc Dali::Ui::InputField::SetTextUnderline
   */
  void SetTextUnderline(const Ui::Text::Underline& underline);

  /**
   * @copydoc Dali::Ui::InputField::GetTextUnderline
   */
  Ui::Text::Underline GetTextUnderline() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTextShadow
   */
  void SetTextShadow(const Ui::Text::Shadow& shadow);

  /**
   * @copydoc Dali::Ui::InputField::GetTextShadow
   */
  Ui::Text::Shadow GetTextShadow() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTextOutline
   */
  void SetTextOutline(const Ui::Text::Outline& outline);

  /**
   * @copydoc Dali::Ui::InputField::GetTextOutline
   */
  Ui::Text::Outline GetTextOutline() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTextLineThrough
   */
  void SetTextLineThrough(const Ui::Text::LineThrough& lineThrough);

  /**
   * @copydoc Dali::Ui::InputField::GetTextLineThrough
   */
  Ui::Text::LineThrough GetTextLineThrough() const;

  /**
   * @brief Sets the explicit font size scale used internally.
   */
  void SetFontSizeScale(float scale);

  /**
   * @brief Gets the explicit font size scale used internally.
   */
  float GetFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::InputField::SetMinimumFontSizeScale
   */
  void SetMinimumFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::InputField::GetMinimumFontSizeScale
   */
  float GetMinimumFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::InputField::SetMaximumFontSizeScale
   */
  void SetMaximumFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::InputField::GetMaximumFontSizeScale
   */
  float GetMaximumFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::InputField::SetSystemFontSizeScaleEnabled
   */
  void SetSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::InputField::IsSystemFontSizeScaleEnabled
   */
  bool IsSystemFontSizeScaleEnabled() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTypingTextColor
   */
  void SetTypingTextColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::InputField::GetTypingTextColor
   */
  UiColor GetTypingTextColor() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTypingFontFamily
   */
  void SetTypingFontFamily(const Dali::String& fontFamily);

  /**
   * @copydoc Dali::Ui::InputField::GetTypingFontFamily
   */
  Dali::String GetTypingFontFamily() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTypingFontSize
   */
  void SetTypingFontSize(float fontSize);

  /**
   * @copydoc Dali::Ui::InputField::GetTypingFontSize
   */
  float GetTypingFontSize() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTypingFontWeight
   */
  void SetTypingFontWeight(Ui::Text::FontWeight weight);

  /**
   * @copydoc Dali::Ui::InputField::GetTypingFontWeight
   */
  Ui::Text::FontWeight GetTypingFontWeight() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTypingFontWidth
   */
  void SetTypingFontWidth(Ui::Text::FontWidth width);

  /**
   * @copydoc Dali::Ui::InputField::GetTypingFontWidth
   */
  Ui::Text::FontWidth GetTypingFontWidth() const;

  /**
   * @copydoc Dali::Ui::InputField::SetTypingFontSlant
   */
  void SetTypingFontSlant(Ui::Text::FontSlant slant);

  /**
   * @copydoc Dali::Ui::InputField::GetTypingFontSlant
   */
  Ui::Text::FontSlant GetTypingFontSlant() const;

  /**
   * @see Dali::Ui::InputField::SetFontVariation
   */
  void SetFontVariation(const Dali::Vector<Ui::Text::FontVariation::Axis>& axes);

  /**
   * @see Dali::Ui::InputField::SetFontVariation(const Dali::String&)
   */
  void SetFontVariation(const Dali::String& settings);

  /**
   * @copydoc Dali::Ui::InputField::GetFontVariation
   */
  Dali::Vector<Ui::Text::FontVariation::Axis> GetFontVariation() const;

  /**
   * @see Dali::Ui::InputField::SetTranslatablePlaceholder
   */
  void SetTranslatablePlaceholder(StringView resourceId);

  /**
   * @see Dali::Ui::InputField::SetTranslatablePlaceholder(StringView resourceId, StringView domain)
   */
  void SetTranslatablePlaceholder(StringView resourceId, StringView domain);

  /**
   * @copydoc Dali::Ui::InputField::GetTranslatablePlaceholder
   */
  Dali::String GetTranslatablePlaceholder() const;

  /**
   * @copydoc Dali::Ui::InputField::ClearTranslatablePlaceholder
   */
  void ClearTranslatablePlaceholder();

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
   * @copydoc Dali::Ui::InputField::GetAdjustedFontSizeScale
   */
  float GetAdjustedFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::InputField::GetSelectedTextStart
   */
  uint32_t GetSelectedTextStart() const;

  /**
   * @copydoc Dali::Ui::InputField::GetSelectedTextEnd
   */
  uint32_t GetSelectedTextEnd() const;

  // Method

  /**
   * @copydoc InputField::GetInputMethodContext()
   */
  InputMethodContext GetInputMethodContext();

public: // Signals
  /**
   * @copydoc Dali::Ui::InputField::TextChangedSignal()
   */
  Signal<void(View)>& TextChangedSignal();

  /**
   * @copydoc Dali::Ui::InputField::MaximumLengthReachedSignal()
   */
  Signal<void(View)>& MaximumLengthReachedSignal();

  /**
   * @copydoc Dali::Ui::InputField::InputRejectedSignal()
   */
  Signal<void(View, Ui::Text::InputFilter::RejectReason)>& InputRejectedSignal();

  /**
   * @copydoc Dali::Ui::InputField::CursorPositionChangedSignal()
   */
  Signal<void(View, uint32_t)>& CursorPositionChangedSignal();

  /**
   * @copydoc Dali::Ui::InputField::SelectionStartedSignal()
   */
  Signal<void(View)>& SelectionStartedSignal();

  /**
   * @copydoc Dali::Ui::InputField::SelectionChangedSignal()
   */
  Signal<void(View, uint32_t, uint32_t)>& SelectionChangedSignal();

  /**
   * @copydoc Dali::Ui::InputField::SelectionClearedSignal()
   */
  Signal<void(View)>& SelectionClearedSignal();

  /**
   * @copydoc Dali::Ui::InputField::TypingStyleChangedSignal()
   */
  Signal<void(View, Ui::Text::TypingStyle::Mask)>& TypingStyleChangedSignal();

protected:
  // Construction

  /**
   * @brief InputFieldImpl constructor.
   */
  InputFieldImpl();

private: // Config
  /**
   * @brief Applies default values from UiConfig if applied.
   */
  void ApplyInitialConfig();

private: // UiScale
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
  Insets GetEffectiveTextPadding() const;

private: // System FontSize
  /**
   * @brief Applies the current platform font size preference.
   *
   * @param[in] fontSize The platform font size preference.
   */
  void ApplySystemFontSize(Dali::Integration::SystemSettings::FontSize fontSize);

  /**
   * @brief Called when the platform font size preference changes.
   *
   * @param[in] fontSize The changed platform font size preference.
   */
  void OnSystemFontSizeChanged(Dali::Integration::SystemSettings::FontSize fontSize);

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
   * @copydoc CustomActorImpl::OnAnimateAnimatableProperty()
   */
  void OnAnimateAnimatableProperty(Animation& animation, Dali::Property::Index index,
                                   Dali::Animation::State state) override;

  /**
   * @copydoc CustomActorImpl::OnConstraintAnimatableProperty()
   */
  void OnConstraintAnimatableProperty(Constraint& constraint, Dali::Property::Index index, bool applied) override;

  /**
   * @copydoc ViewImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc SizeNegotiatedViewImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

private: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnAccessibilityActivate()
   */
  bool OnAccessibilityActivate() override;

  /**
   * @copydoc ViewImpl::OnFocusChanged()
   */
  void OnFocusChanged(bool focused) override;

  /**
   * @copydoc ViewImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc ViewImpl::FilterKeyEvent()
   */
  bool FilterKeyEvent(const KeyEvent& event) override;

  /**
   * @copydoc ViewImpl::OnKeyEvent()
   */
  bool OnKeyEvent(const KeyEvent& event) override;

  /**
   * @brief Synchronizes accessibility anchor actors when the bridge status changes.
   */
  void OnAccessibilityStatusChanged();

protected: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  LayoutRect OnArrange(const LayoutRect& bounds) override;

private: // From ControlInterface
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

public: // From EditableControlInterface
  /**
   * @copydoc Text::EditableControlInterface::AddDecoration()
   */
  void AddDecoration(Actor& actor, Text::DecorationType type, bool needsClipping) override;

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
  std::string CopyText() override;

  /**
   * @copydoc Dali::EditableControlInterface::CutText()
   */
  std::string CutText() override;

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
  void InputRejected(Ui::Text::InputFilter::RejectReason reason) override;

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

public: // From SelectableControlInterface
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
  Text::Uint32Pair GetTextSelectionRange() const override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectionChanged()
   */
  void SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd) override;

private: // From AnchorControlInterface
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
   * @copydoc Dali::Ui::Text::Controller::OnInputMethodContextEvent()
   */
  Dali::Integration::InputMethodContext::CallbackData OnInputMethodContextEvent(
    InputMethodContext                                      inputMethodContext,
    const Dali::Integration::InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief Connection needed to re-render text, when a Input Field returns to the scene.
   */
  void OnSceneConnect(Dali::Actor actor);

  /**
   * @brief Updates on-demand inline replacement data from the final text layout.
   *
   * @param[in] ownerSize The control size.
   * @param[in] padding The effective text padding.
   */
  void UpdateInlineReplacementData(const Vector2& ownerSize, const Insets& padding);

  /**
   * @brief Removes on-demand inline replacement data and disconnects resource notifications.
   */
  void ClearInlineReplacementData();

  /**
   * @brief Refreshes inline replacement visuals after resource loading.
   *
   * @param[in] view The visual owner.
   */
  void OnInlineReplacementResourcesReady(Ui::View view);

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
   * @brief Callback when InputField is touched
   *
   * @param[in] actor InputField touched
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
   * @brief Callback when keyboard status changes.
   * @param[in] inputMethodContext The input method context.
   * @param[in] state The input panel state.
   */
  void OnKeyboardStatusChanged(InputMethodContext context, InputMethodContext::State state);

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
   * @brief Render view, create and attach actor(s) to this Input Field.
   */
  void RenderText(Text::UpdateTextType updateTextType);

  /**
   * @brief Synchronizes the active normal/placeholder atlas gradient resource with the current renderer.
   *
   * @return True if the current renderer remains usable after synchronization.
   */
  bool SyncAtlasGradientState();

  /**
   * @brief Applies the active normal/placeholder atlas gradient resource to a newly-created renderer.
   */
  void ApplyAtlasGradientState();

  /**
   * @brief Updates the normal hidden TextGradient start offset source property from the authored gradient.
   */
  void SyncGradientAnimProperties();

  /**
   * @brief Updates the placeholder hidden TextGradient start offset source property from the authored gradient.
   */
  void SyncPlaceholderGradientAnimProperties();

  /**
   * @brief Binds the active normal/placeholder hidden start offset source property to the atlas renderer.
   */
  void BindGradientAnimProperties();

  /**
   * @brief Returns true when the active normal/placeholder channel has a renderer-supported gradient.
   *
   * @return True if the active channel supports TextGradient animation.
   */
  bool IsActiveGradientAnimSupported() const;

  /**
   * @brief Updates TextGradient animation constraint apply rate on the atlas renderer.
   *
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  void SetGradientAnimApplyRate(bool notifyToConstraint = false);

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
  void EmitInputRejected(Ui::Text::InputFilter::RejectReason reason);

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
  void EmitTypingStyleChanged(Ui::Text::TypingStyle::Mask mask);

  /**
   * @brief Callback applied when localized placeholder is updated.
   *
   * @param[in] target The target handle (unused, bound to this).
   * @param[in] text The localized placeholder text.
   */
  void ApplyLocalizedPlaceholder(BaseHandle target, const Dali::String& text);

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
  InputFieldImpl(const InputFieldImpl&)            = delete;
  InputFieldImpl(InputFieldImpl&&)                 = delete;
  InputFieldImpl& operator=(const InputFieldImpl&) = delete;
  InputFieldImpl& operator=(InputFieldImpl&&)      = delete;

private:
  // Data
  Signal<void(View)>                                      mTextChangedSignal;
  Signal<void(View)>                                      mMaxLengthReachedSignal;
  Signal<void(View, Ui::Text::InputFilter::RejectReason)> mInputRejectedSignal;
  Signal<void(View, uint32_t)>                            mCursorPositionChangedSignal;
  Signal<void(View)>                                      mSelectionStartedSignal;
  Signal<void(View, uint32_t, uint32_t)>                  mSelectionChangedSignal;
  Signal<void(View)>                                      mSelectionClearedSignal;
  Signal<void(View, Ui::Text::TypingStyle::Mask)>         mTypingStyleChangedSignal;

  Internal::Text::EditableTextGradientPropertyDataPtr mTextGradientPropertyData;

  InputMethodContext              mInputMethodContext;
  TapGestureDetector              mTapGestureDetector;
  PanGestureDetector              mPanGestureDetector;
  LongPressGestureDetector        mLongPressGestureDetector;
  Ui::Text::ControllerPtr         mController;
  Ui::Text::RendererPtr           mRenderer;
  Text::Gradient::AtlasApplyState mAtlasApplyState;
  Ui::Text::DecoratorPtr          mDecorator;
  Actor                           mStencil;
  std::vector<Actor>              mClippingDecorationActors; ///< Decoration actors which need clipping.
  std::vector<Ui::TextAnchor>     mAnchorActors;
  Actor                           mRenderableActor;
  Actor                           mActiveLayer;
  Actor                           mCursorLayer;
  Actor                           mBackgroundActor;

  Ui::Text::OverflowMode mOverflowMode;
  float                  mAlignmentOffset;
  bool                   mMeasureInvalidated : 1;
  bool                   mHasBeenStaged : 1;
  bool                   mHasTextGradientPropertyData : 1; ///< Whether editable TextGradient property data has been created.
  bool                   mTextChanged : 1;                 ///< If true, emits TextChangedSignal in next OnRelayout().
  bool                   mCursorPositionChanged : 1;       ///< If true, emits CursorPositionChangedSignal at the end of OnRelayout().
  bool                   mSelectionStarted : 1;            ///< If true, emits SelectionStartedSignal at the end of OnRelayout().
  bool                   mSelectionChanged : 1;            ///< If true, emits SelectionChangedSignal at the end of OnRelayout().
  bool                   mSelectionCleared : 1;            ///< If true, emits SelectionClearedSignal at the end of OnRelayout().
  bool                   mFocusGainedByTouch : 1;          ///< If true, focus was gained by touch, skip scroll in focus gained.

  Dali::String mTranslatablePlaceholder; ///< Resource ID for translatable placeholder binding.

protected:
  struct PropertyHandler;
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
