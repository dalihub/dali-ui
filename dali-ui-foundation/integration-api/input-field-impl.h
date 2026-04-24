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
#include <dali-ui-foundation/public-api/text/input-field-properties.h>
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

class InputFieldImpl;
using InputFieldImplPtr = IntrusivePtr<InputFieldImpl>;

/**
 * @brief This is the internal implementation class for InputField.
 *
 * @see Dali::Ui::InputFieldImpl
 */
class DALI_UI_API InputFieldImpl : public ViewImpl, public Text::ControlInterface, public Text::EditableControlInterface, public Text::SelectableControlInterface, public Text::AnchorControlInterface
{
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
  void SetHorizontalTextAlignment(Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::InputField::GetHorizontalTextAlignment
   */
  Text::Alignment GetHorizontalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::InputField::SetVerticalTextAlignment
   */
  void SetVerticalTextAlignment(Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::InputField::GetVerticalTextAlignment
   */
  Text::Alignment GetVerticalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::InputField::SetOverflowMode
   */
  void SetOverflowMode(Text::OverflowMode mode);

  /**
   * @copydoc Dali::Ui::InputField::GetOverflowMode
   */
  Text::OverflowMode GetOverflowMode() const;

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
   * @copydoc Dali::Ui::InputField::SetMaximumLength
   */
  void SetMaximumLength(int length);

  /**
   * @copydoc Dali::Ui::InputField::GetMaximumLength
   */
  int GetMaximumLength() const;

  /**
   * @copydoc Dali::Ui::InputField::SetLayoutDirectionMode
   */
  void SetLayoutDirectionMode(Text::LayoutDirectionMode mode);

  /**
   * @copydoc Dali::Ui::InputField::GetLayoutDirectionMode
   */
  Text::LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @copydoc Dali::Ui::InputField::SetFontWeight
   */
  void SetFontWeight(Text::FontWeight weight);

  /**
   * @copydoc Dali::Ui::InputField::GetFontWeight
   */
  Text::FontWeight GetFontWeight() const;

  /**
   * @copydoc Dali::Ui::InputField::SetFontWidth
   */
  void SetFontWidth(Text::FontWidth width);

  /**
   * @copydoc Dali::Ui::InputField::GetFontWidth
   */
  Text::FontWidth GetFontWidth() const;

  /**
   * @copydoc Dali::Ui::InputField::SetFontSlant
   */
  void SetFontSlant(Text::FontSlant slant);

  /**
   * @copydoc Dali::Ui::InputField::GetFontSlant
   */
  Text::FontSlant GetFontSlant() const;

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
   * @copydoc Dali::Ui::InputField::SetUnderline
   */
  void SetUnderline(const Text::Underline& underline);

  /**
   * @copydoc Dali::Ui::InputField::ClearUnderline
   */
  void ClearUnderline();

  /**
   * @copydoc Dali::Ui::InputField::SetShadow
   */
  void SetShadow(const Text::Shadow& shadow);

  /**
   * @copydoc Dali::Ui::InputField::ClearShadow
   */
  void ClearShadow();

  /**
   * @copydoc Dali::Ui::InputField::SetOutline
   */
  void SetOutline(const Text::Outline& outline);

  /**
   * @copydoc Dali::Ui::InputField::ClearOutline
   */
  void ClearOutline();

  /**
   * @copydoc Dali::Ui::InputField::SetLineThrough
   */
  void SetLineThrough(const Text::LineThrough& lineThrough);

  /**
   * @copydoc Dali::Ui::InputField::ClearLineThrough
   */
  void ClearLineThrough();

  /**
   * @copydoc Dali::Ui::InputField::SetFontSizeScale
   */
  void SetFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::InputField::GetFontSizeScale
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
   * @see Dali::Ui::InputField::SetFontVariation
   */
  void SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes);

  /**
   * @see Dali::Ui::InputField::SetFontVariation(const Dali::String&)
   */
  void SetFontVariation(const Dali::String& settings);

  /**
   * @copydoc Dali::Ui::InputField::GetFontVariation
   */
  Dali::Vector<Text::FontVariationAxis> GetFontVariation() const;

  /**
   * @copydoc Dali::Ui::InputField::ClearFontVariation
   */
  void ClearFontVariation();

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

  // Method

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
   * @copydoc Dali::Ui::InputField::CursorPositionChangedSignal()
   */
  Signal<void(View, uint32_t)>& CursorPositionChangedSignal();

protected:
  // Construction

  /**
   * @brief InputFieldImpl constructor.
   */
  InputFieldImpl();

public: // Config
  /**
   * @brief Applies default values from UiConfigManager if initialized.
   */
  void ApplyInitialConfig();

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
  void RequestAsyncRender();

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
   * @copydoc Text::EditableControlInterface::MaxLengthReached()
   */
  void MaxLengthReached() override;

  /**
   * @copydoc Text::EditableControlInterface::CursorPositionChanged()
   */
  void CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition) override;

  /**
   * @copydoc Text::EditableControlInterface::InputStyleChanged()
   */
  void InputStyleChanged(Text::InputStyle::Mask inputStyleMask) override;

  /**
   * @copydoc Text::EditableControlInterface::InputFiltered()
   */
  void InputFiltered(Ui::InputFilter::Property::Type type) override;

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
   * @copydoc Dali::Ui::Text::Controller::(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
   */
  InputMethodContext::CallbackData OnInputMethodContextEvent(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief Connection needed to re-render text, when a Input Field returns to the scene.
   */
  void OnSceneConnect(Dali::Actor actor);

  /**
   * @brief Callback when a tap gesture is detected.
   */
  void OnTapDetected(Actor actor, const TapGesture& tap);

  /**
   * @brief Callback when a pan gesture is detected.
   */
  void OnPanDetected(Actor actor, const PanGesture& pan);

  /**
   * @brief Callback when a long press gesture is detected.
   */
  void OnLongPressDetected(Actor actor, const LongPressGesture& longPress);

  /**
   * @brief Callback when InputField is touched
   *
   * @param[in] actor InputField touched
   * @param[in] touch Touch information
   */
  bool OnTouched(Actor actor, const TouchEvent& touch);

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
   * @brief Render view, create and attach actor(s) to this Input Field.
   */
  void RenderText(Text::Controller::UpdateTextType updateTextType);

  /**
   * @brief Emits TextChanged signal.
   */
  void EmitTextChanged();

  /**
   * @brief Emits CursorPositionChanged signal.
   */
  void EmitCursorPositionChanged();

private: // UiColorManager
  void SetTextColorInternal(const Vector4& color);
  void SetPlaceholderColorInternal(const Vector4& color);
  void SetCursorColorInternal(const Vector4& color);
  void SetSelectionColorInternal(const Vector4& color);
  void SetTextBackgroundColorInternal(const Vector4& color);
  void SetUnderlineColorInternal(const Vector4& color);
  void SetShadowColorInternal(const Vector4& color);
  void SetOutlineColorInternal(const Vector4& color);
  void SetLineThroughColorInternal(const Vector4& color);

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
  Signal<void(View)>           mTextChangedSignal;
  Signal<void(View)>           mMaxLengthReachedSignal;
  Signal<void(View, uint32_t)> mCursorPositionChangedSignal;

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

  Text::OverflowMode mOverflowMode;
  float              mAlignmentOffset;
  bool               mMeasureInvalidated : 1;
  bool               mHasBeenStaged : 1;
  bool               mTextChanged : 1;           ///< If true, emits TextChangedSignal in next OnRelayout().
  bool               mCursorPositionChanged : 1; ///< If true, emits CursorPositionChangedSignal at the end of OnRelayout().
  bool               mSelectionStarted : 1;      ///< If true, emits SelectionStartedSignal at the end of OnRelayout().
  bool               mSelectionChanged : 1;      ///< If true, emits SelectionChangedSignal at the end of OnRelayout().
  bool               mSelectionCleared : 1;      ///< If true, emits SelectionClearedSignal at the end of OnRelayout().
  bool               mSelectionEnabled : 1;      ///< Whether text selection is enabled.

protected:
  struct PropertyHandler;
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
