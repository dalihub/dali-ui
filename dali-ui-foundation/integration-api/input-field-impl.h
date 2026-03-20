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
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>

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
  void SetTextColor(const Vector4& color);

  /**
   * @copydoc Dali::Ui::InputField::GetTextColor
   */
  const Vector4& GetTextColor() const;

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
  void SetPlaceholderColor(const Vector4& color);

  /**
   * @copydoc Dali::Ui::InputField::GetPlaceholderColor
   */
  const Vector4& GetPlaceholderColor() const;

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
  void SetCursorColor(const Vector4& color);

  /**
   * @copydoc Dali::Ui::InputField::GetCursorColor
   */
  const Vector4& GetCursorColor() const;

  /**
   * @copydoc Dali::Ui::InputField::SetSelectionColor
   */
  void SetSelectionColor(const Vector4& color);

  /**
   * @copydoc Dali::Ui::InputField::GetSelectionColor
   */
  const Vector4& GetSelectionColor() const;

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

public: // Signals
  /**
   * @copydoc Dali::Ui::InputField::TextChangedSignal()
   */
  Signal<void(View)>& TextChangedSignal();

  /**
   * @copydoc Dali::Ui::InputField::MaximumLengthReachedSignal()
   */
  Signal<void(View)>& MaximumLengthReachedSignal();

protected:
  // Construction

  /**
   * @brief InputFieldImpl constructor.
   */
  InputFieldImpl();

public: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Integration::ViewImpl::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Integration::ViewImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Integration::ViewImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

private: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnKeyInputFocusGained()
   */
  void OnKeyInputFocusGained() override;

  /**
   * @copydoc Integration::ViewImpl::OnKeyInputFocusLost()
   */
  void OnKeyInputFocusLost() override;

  /**
   * @copydoc Integration::ViewImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Integration::ViewImpl::OnKeyEvent()
   */
  bool OnKeyEvent(const KeyEvent& event) override;

private: // From Control
  /**
   * @copydoc Control::OnTap()
   */
  void OnTap(const TapGesture& tap) override;

  /**
   * @copydoc Control::OnPan()
   */
  void OnPan(const PanGesture& gesture) override;

  /**
   * @copydoc Control::OnLongPress()
   */
  void OnLongPress(const LongPressGesture& gesture) override;

protected: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc Integration::ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

public: // From ControlInterface
  /**
   * @copydoc Text::ControlInterface::RequestTextRelayout()
   */
  void RequestTextRelayout() override;

  // From EditableControlInterface

  /**
   * @copydoc Text::EditableControlInterface::TextChanged()
   */
  void TextInserted(unsigned int position, unsigned int length, const std::string& content) override;

  /**
   * @copydoc Text::EditableControlInterface::TextDeleted()
   */
  void TextDeleted(unsigned int position, unsigned int length, const std::string& content) override;

  /**
   * @copydoc Text::EditableControlInterface::CursorPositionChanged()
   */
  void CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition) override;

  /**
   * @copydoc Text::EditableControlInterface::TextChanged()
   */
  void TextChanged(bool immediate) override;

  /**
   * @copydoc Text::EditableControlInterface::MaxLengthReached()
   */
  void MaxLengthReached() override;

  /**
   * @copydoc Text::EditableControlInterface::InputStyleChanged()
   */
  void InputStyleChanged(Text::InputStyle::Mask inputStyleMask) override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectionChanged()
   */
  void SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd) override;

  /**
   * @copydoc Text::EditableControlInterface::AddDecoration()
   */
  void AddDecoration(Actor& actor, Ui::Text::DecorationType type, bool needsClipping) override;

  /**
   * @copydoc Text::EditableControlInterface::InputFiltered()
   */
  void InputFiltered(Ui::InputFilter::Property::Type type) override;

  /**
   * @copydoc Text::EditableControlInterface::GetControlBackgroundColor()
   */
  void GetControlBackgroundColor(Vector4& color) const override;

  // From SelectableControlInterface
public:
  /**
   * @copydoc Text::SelectableControlInterface::SetTextSelectionRange()
   */
  void SetTextSelectionRange(const uint32_t* start, const uint32_t* end) override;

  /**
   * @copydoc Text::SelectableControlInterface::GetTextSelectionRange()
   */
  Uint32Pair GetTextSelectionRange() const override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectWholeText()
   */
  void SelectWholeText() override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectNone()
   */
  void SelectNone() override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectText()
   */
  void SelectText(const uint32_t start, const uint32_t end) override;

  /**
   * @copydoc Text::SelectableControlInterface::GetSelectedText()
   */
  string GetSelectedText() const override;

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

  // From AnchorControlInterface
public:
  /**
   * @copydoc Text::AnchorControlInterface::AnchorClicked()
   */
  bool AnchorClicked(uint32_t cursorPosition, std::string& href) override;

  /**
   * @copydoc Text::AnchorControlInterface::EmitAnchorClickedSignal()
   */
  void EmitAnchorClickedSignal(const std::string& href) override;

private: // Implementation
  /**
   * @copydoc Dali::Ui::Text::Controller::(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
   */
  InputMethodContext::CallbackData OnInputMethodContextEvent(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief Connection needed to re-render text, when a Input Field returns to the scene.
   */
  void OnSceneConnect(Dali::Actor actor);

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
  void EmitTextChangedSignal();

private:
  // Not copyable or movable
  InputFieldImpl(const InputFieldImpl&)            = delete;
  InputFieldImpl(InputFieldImpl&&)                 = delete;
  InputFieldImpl& operator=(const InputFieldImpl&) = delete;
  InputFieldImpl& operator=(InputFieldImpl&&)      = delete;

private:
  // Data
  Signal<void(View)> mTextChangedSignal;
  Signal<void(View)> mMaxLengthReachedSignal;

  InputMethodContext          mInputMethodContext;
  Text::ControllerPtr         mController;
  Text::RendererPtr           mRenderer;
  Text::DecoratorPtr          mDecorator;
  Ui::View                    mStencil;
  std::vector<Actor>          mClippingDecorationActors; ///< Decoration actors which need clipping.
  std::vector<Ui::TextAnchor> mAnchorActors;
  Dali::InputMethodOptions    mInputMethodOptions;

  Actor mRenderableActor;
  Actor mActiveLayer;
  Actor mCursorLayer;
  Actor mBackgroundActor;

  float mAlignmentOffset;
  bool  mHasBeenStaged : 1;
  bool  mTextChanged : 1;           ///< If true, emits TextChangedSignal in next OnRelayout().
  bool  mCursorPositionChanged : 1; ///< If true, emits CursorPositionChangedSignal at the end of OnRelayout().
  bool  mSelectionStarted : 1;      ///< If true, emits SelectionStartedSignal at the end of OnRelayout().
  bool  mSelectionChanged : 1;      ///< If true, emits SelectionChangedSignal at the end of OnRelayout().
  bool  mSelectionCleared : 1;      ///< If true, emits SelectionClearedSignal at the end of OnRelayout().

  uint32_t mOldPosition;       ///< args for cursor position changed event
  uint32_t mOldSelectionStart; ///< args for selection changed event
  uint32_t mOldSelectionEnd;   ///< args for selection changed event
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
