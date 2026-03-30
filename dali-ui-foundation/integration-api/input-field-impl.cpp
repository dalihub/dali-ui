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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/input-field-impl.h>

#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/focus-manager/keyboard-focus-manager-impl.h>
#include <dali-ui-foundation/internal/text/rendering/text-backend.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToStdString;

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{

const char* KEY_RETURN_NAME = "Return";

BaseHandle Create()
{
  return BaseHandle();
}

// Type Registration

DALI_TYPE_REGISTRATION_BEGIN(InputFieldImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

InputFieldImplPtr InputFieldImpl::New()
{
  return InputFieldImplPtr(new InputFieldImpl());
}

InputFieldImpl::InputFieldImpl()
: ViewImpl(),
  mAlignmentOffset(0.f),
  mHasBeenStaged(false),
  mTextChanged(false),
  mCursorPositionChanged(false),
  mSelectionStarted(false),
  mSelectionChanged(false),
  mSelectionCleared(false),
  mOldPosition(0u),
  mOldSelectionStart(0u),
  mOldSelectionEnd(0u)
{
}

InputFieldImpl::~InputFieldImpl()
{
  UnparentAndReset(mStencil);
}

// =============================================================================
// Properties
// =============================================================================
void InputFieldImpl::SetText(const Dali::String& text)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), text.CStr());

  mController->SetText(ToStdString(text));
}

Dali::String InputFieldImpl::GetText() const
{
  std::string text;
  mController->GetText(text);
  return ToDaliString(text);
}

void InputFieldImpl::SetFontFamily(const Dali::String& fontFamily)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), fontFamily.CStr());

  mController->SetDefaultFontFamily(ToStdString(fontFamily));
}

Dali::String InputFieldImpl::GetFontFamily() const
{
  return ToDaliString(mController->GetDefaultFontFamily());
}

void InputFieldImpl::SetFontSize(float fontSize)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), fontSize);

  if(!Equals(mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), fontSize))
  {
    mController->SetDefaultFontSize(fontSize, Text::Controller::PIXEL_SIZE);
  }
}

float InputFieldImpl::GetFontSize() const
{
  return mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE);
}

void InputFieldImpl::SetTextColor(const UiColor& color)
{
  UiColorManager::Get().UpdateBinding(color, View::DownCast(Self()), this, &InputFieldImpl::SetTextColorInternal);
  SetTextColorInternal(color.Resolve());
}

UiColor InputFieldImpl::GetTextColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(View::DownCast(Self()), this, &InputFieldImpl::SetTextColorInternal, outColor))
  {
    return outColor;
  }
  return mController->GetDefaultColor();
}

void InputFieldImpl::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), alignment);
  mController->SetHorizontalAlignment(alignment);
}

Text::Alignment InputFieldImpl::GetHorizontalTextAlignment() const
{
  return mController->GetHorizontalAlignment();
}

void InputFieldImpl::SetVerticalTextAlignment(Text::Alignment alignment)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), alignment);
  mController->SetVerticalAlignment(alignment);
}

Text::Alignment InputFieldImpl::GetVerticalTextAlignment() const
{
  return mController->GetVerticalAlignment();
}

void InputFieldImpl::SetPlaceholder(const Dali::String& text)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), text.CStr());

  const std::string placeholder = ToStdString(text);
  mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, placeholder);
  mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_ACTIVE, placeholder);
  // TODO: Add configuration option to control whether the placeholder
  //       should be visible when the input field is focused.
}

Dali::String InputFieldImpl::GetPlaceholder() const
{
  std::string text;
  mController->GetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, text);
  return ToDaliString(text);
}

void InputFieldImpl::SetPlaceholderColor(const UiColor& color)
{
  UiColorManager::Get().UpdateBinding(color, View::DownCast(Self()), this, &InputFieldImpl::SetPlaceholderColorInternal);
  SetPlaceholderColorInternal(color.Resolve());
}

UiColor InputFieldImpl::GetPlaceholderColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(View::DownCast(Self()), this, &InputFieldImpl::SetPlaceholderColorInternal, outColor))
  {
    return outColor;
  }
  return mController->GetPlaceholderTextColor();
}

void InputFieldImpl::SetCursorWidth(int width)
{
  DALI_LOG_RELEASE_INFO("[%p] width:%d\n", mController.Get(), width);

  mDecorator->SetCursorWidth(width);
  mController->GetLayoutEngine().SetCursorWidth(width);
}

int InputFieldImpl::GetCursorWidth() const
{
  return mDecorator->GetCursorWidth();
}

void InputFieldImpl::SetCursorColor(const UiColor& color)
{
  UiColorManager::Get().UpdateBinding(color, View::DownCast(Self()), this, &InputFieldImpl::SetCursorColorInternal);
  SetCursorColorInternal(color.Resolve());
}

UiColor InputFieldImpl::GetCursorColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(View::DownCast(Self()), this, &InputFieldImpl::SetCursorColorInternal, outColor))
  {
    return outColor;
  }
  return mDecorator->GetColor(Text::PRIMARY_CURSOR);
}

void InputFieldImpl::SetSelectionColor(const UiColor& color)
{
  UiColorManager::Get().UpdateBinding(color, View::DownCast(Self()), this, &InputFieldImpl::SetSelectionColorInternal);
  SetSelectionColorInternal(color.Resolve());
}

UiColor InputFieldImpl::GetSelectionColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(View::DownCast(Self()), this, &InputFieldImpl::SetSelectionColorInternal, outColor))
  {
    return outColor;
  }
  return mDecorator->GetHighlightColor();
}

void InputFieldImpl::SetMaximumLength(int length)
{
  mController->SetMaximumNumberOfCharacters(static_cast<uint32_t>(length));
}

int InputFieldImpl::GetMaximumLength() const
{
  return static_cast<int>(mController->GetMaximumNumberOfCharacters());
}

void InputFieldImpl::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  if(mController->GetLayoutDirectionMode() != mode)
  {
    mController->SetLayoutDirectionMode(mode);
    RequestTextRelayout();
  }
}

Text::LayoutDirectionMode InputFieldImpl::GetLayoutDirectionMode() const
{
  return mController->GetLayoutDirectionMode();
}

// =============================================================================
// Signals
// =============================================================================
Signal<void(View)>& InputFieldImpl::TextChangedSignal()
{
  return mTextChangedSignal;
}

Signal<void(View)>& InputFieldImpl::MaximumLengthReachedSignal()
{
  return mMaxLengthReachedSignal;
}

void InputFieldImpl::OnInitialize()
{
  // Call base class initialization
  ViewImpl::OnInitialize();

  Actor self = Self();

  mController = Text::Controller::New(this, this, this, this);
  mController->SetGlyphType(TextAbstraction::BITMAP_GLYPH);
  mDecorator = Text::Decorator::New(*mController, *mController);

  mInputMethodContext = InputMethodContext::New(self);

  mController->GetLayoutEngine().SetLayout(Text::Layout::Engine::SINGLE_LINE_BOX);

  // Enables the text input.
  mController->EnableTextInput(mDecorator, mInputMethodContext);

  // Enables the horizontal scrolling after the text input has been enabled.
  mController->SetHorizontalScrollEnabled(true);

  // Disables the vertical scrolling.
  mController->SetVerticalScrollEnabled(false);

  // Disable the smooth handle panning.
  mController->SetSmoothHandlePanEnabled(false);

  mController->SetNoTextDoubleTapAction(Text::Controller::NoTextTap::HIGHLIGHT);
  mController->SetNoTextLongPressAction(Text::Controller::NoTextTap::HIGHLIGHT);

  // Sets layoutDirection value
  Dali::Stage                 stage           = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(stage.GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  mController->SetLayoutDirection(layoutDirection);

  self.LayoutDirectionChangedSignal().Connect(this, &InputFieldImpl::OnLayoutDirectionChanged);

  if(Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &InputFieldImpl::OnLocaleChanged);
  }

  // Forward input events to controller
  EnableGestureDetection(static_cast<GestureType::Value>(GestureType::TAP | GestureType::PAN | GestureType::LONG_PRESS));
  GetTapGestureDetector().SetMaximumTapsRequired(2);
  GetTapGestureDetector().ReceiveAllTapEvents(true);

  self.TouchedSignal().Connect(this, &InputFieldImpl::OnTouched);

  // Set BoundingBox to stage size if not already set.
  Rect<int> boundingBox;
  mDecorator->GetBoundingBox(boundingBox);

  if(boundingBox.IsEmpty())
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    mDecorator->SetBoundingBox(Rect<int>(0.0f, 0.0f, stageSize.width, stageSize.height));
  }

  // Flip vertically the 'left' selection handle
  mDecorator->FlipHandleVertically(Text::LEFT_SELECTION_HANDLE, true);

  // Fill-parent area by default
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT);
  self.OnSceneSignal().Connect(this, &InputFieldImpl::OnSceneConnect);

  View                   view         = Dali::Ui::View::DownCast(self);
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl::Get(viewInternal).SetInputMethodContext(mInputMethodContext);

  EnableClipping();

  // TODO: Re-enable when grab handle and popup support are fully implemented.
  mController->SetGrabHandleEnabled(false);
  mController->SetGrabHandlePopupEnabled(false);
}

void InputFieldImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Actor self = Self();

  Extents padding = GetViewPadding();
  float   width   = std::max(size.x - (padding.start + padding.end), 0.0f);
  float   height  = std::max(size.y - (padding.top + padding.bottom), 0.0f);
  Vector2 contentSize(width, height);
  DALI_LOG_RELEASE_INFO("[%p] size:%f,%f, contentSize:%f,%f\n", mController.Get(), size.x, size.y, contentSize.x,
                        contentSize.y);

  // Support Right-To-Left of padding
  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  if(mStencil)
  {
    mStencil.SetProperty(Actor::Property::POSITION, Vector2(padding.start, padding.top));
    ResizeActor(mStencil, contentSize);
  }
  if(mActiveLayer)
  {
    mActiveLayer.SetProperty(Actor::Property::POSITION, Vector2(padding.start, padding.top));
    ResizeActor(mActiveLayer, contentSize);
  }
  if(mCursorLayer)
  {
    if(!mStencil)
    {
      // If there is a stencil, the cursor layer is added to the stencil in RenderText.
      // Do not calculate the position because the stencil has already been resized excluding the padding size.
      mCursorLayer.SetProperty(Actor::Property::POSITION, Vector2(padding.start, padding.top));
    }
    ResizeActor(mCursorLayer, contentSize);
  }

  // If there is text changed, callback is called.
  if(mTextChanged)
  {
    EmitTextChangedSignal();
  }

  Text::Controller::UpdateTextType updateTextType = mController->Relayout(contentSize, layoutDirection);

  if((Text::Controller::NONE_UPDATED != updateTextType) || !mRenderer)
  {
    mController->SetLayoutOffsetWithPadding(Vector2(padding.start, padding.top));

    if(mDecorator &&
       (Text::Controller::NONE_UPDATED != (Text::Controller::DECORATOR_UPDATED & updateTextType)))
    {
      mDecorator->Relayout(contentSize, container);
    }

    if(!mRenderer)
    {
      mRenderer      = Text::Backend::Get().NewRenderer();
      updateTextType = static_cast<Text::Controller::UpdateTextType>(updateTextType | Text::Controller::MODEL_UPDATED);
    }

    RenderText(updateTextType);
  }

  if(mCursorPositionChanged)
  {
    // TODO
  }

  if(mSelectionStarted)
  {
    // TODO
  }

  if(mSelectionChanged)
  {
    // TODO
  }

  if(mSelectionCleared)
  {
    // TODO
  }
}

Vector3 InputFieldImpl::GetNaturalSize()
{
  Extents padding     = GetViewPadding();
  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += (padding.start + padding.end);
  naturalSize.height += (padding.top + padding.bottom);

  return naturalSize;
}

float InputFieldImpl::GetHeightForWidth(float width)
{
  Extents padding = GetViewPadding();
  return mController->GetHeightForWidth(width - (padding.start + padding.end)) + padding.top + padding.bottom;
}

void InputFieldImpl::OnKeyInputFocusGained()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  if(mInputMethodContext && IsEditable())
  {
    // All input panel properties, such as layout, return key type, and input hint, should be set before input panel activates (or shows).
    mInputMethodContext.ApplyOptions(mInputMethodOptions);
    mInputMethodContext.NotifyTextInputMultiLine(false);

    mInputMethodContext.StatusChangedSignal().Connect(this, &InputFieldImpl::OnKeyboardStatusChanged);

    mInputMethodContext.KeyboardEventReceivedSignal().Connect(this, &InputFieldImpl::OnInputMethodContextEvent);

    // Notify that the text editing start.
    mInputMethodContext.Activate();

    // When window gain lost focus, the inputMethodContext is deactivated. Thus when window gain focus again, the inputMethodContext must be activated.
    mInputMethodContext.SetRestoreAfterFocusLost(true);
  }

  if(IsEditable() && mController->IsUserInteractionEnabled())
  {
    mController->KeyboardFocusGainEvent(); // Called in the case of no virtual keyboard to trigger this event
  }

  EmitKeyInputFocusSignal(true); // Calls back into the Control hence done last.
}

void InputFieldImpl::OnKeyInputFocusLost()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  if(mInputMethodContext)
  {
    mInputMethodContext.StatusChangedSignal().Disconnect(this, &InputFieldImpl::OnKeyboardStatusChanged);
    // The text editing is finished. Therefore the inputMethodContext don't have restore activation.
    mInputMethodContext.SetRestoreAfterFocusLost(false);

    // Notify that the text editing finish.
    mInputMethodContext.Deactivate();

    mInputMethodContext.KeyboardEventReceivedSignal().Disconnect(this, &InputFieldImpl::OnInputMethodContextEvent);
  }

  mController->KeyboardFocusLostEvent();

  EmitKeyInputFocusSignal(false); // Calls back into the Control hence done last.
}

void InputFieldImpl::OnSceneConnection(int depth)
{
  // Sets the depth to the visuals inside the text's decorator.
  mDecorator->SetTextDepth(depth);

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().

  // Call the Control::OnSceneConnection() to set the depth of the background.
  ViewImpl::OnSceneConnection(depth);
}

bool InputFieldImpl::OnKeyEvent(const KeyEvent& event)
{
  DALI_LOG_RELEASE_INFO("[%p] keyCode:%d\n", mController.Get(), event.GetKeyCode());

  if(Dali::DALI_KEY_ESCAPE == event.GetKeyCode() && mController->ShouldClearFocusOnEscape())
  {
    // Make sure ClearKeyInputFocus when only key is up
    if(event.GetState() == KeyEvent::UP)
    {
      Dali::Ui::KeyboardFocusManager keyboardFocusManager = Dali::Ui::KeyboardFocusManager::Get();
      if(keyboardFocusManager)
      {
        keyboardFocusManager.ClearFocus();
      }
      ClearKeyInputFocus();
    }

    return true;
  }
  else if((Dali::DevelKey::DALI_KEY_RETURN == event.GetKeyCode() && strcmp(KEY_RETURN_NAME, event.GetKeyName().CStr()) == 0) ||
          Dali::DevelKey::DALI_KEY_KP_ENTER == event.GetKeyCode())
  {
    // Do nothing when enter is coming.
    return false;
  }

  return mController->KeyEvent(event);
}

void InputFieldImpl::OnTap(const TapGesture& gesture)
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());

  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  Extents        padding    = GetViewPadding();
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->TapEvent(gesture.GetNumberOfTaps(), localPoint.x - padding.start, localPoint.y - padding.top);
  mController->AnchorEvent(localPoint.x - padding.start, localPoint.y - padding.top);

  Dali::Ui::KeyboardFocusManager keyboardFocusManager = Dali::Ui::KeyboardFocusManager::Get();
  if(keyboardFocusManager)
  {
    keyboardFocusManager.SetCurrentFocusActor(Self());
  }
  SetKeyInputFocus();
}

void InputFieldImpl::OnPan(const PanGesture& gesture)
{
  if(!mController->IsScrollable(gesture.GetDisplacement()))
  {
    Dali::DevelActor::SetNeedGesturePropagation(Self(), true);
  }
  else
  {
    Dali::DevelActor::SetNeedGesturePropagation(Self(), false);
  }
  mController->PanEvent(gesture.GetState(), gesture.GetDisplacement());
}

void InputFieldImpl::OnLongPress(const LongPressGesture& gesture)
{
  if(mInputMethodContext && IsEditable())
  {
    mInputMethodContext.Activate();
  }
  Extents        padding    = GetViewPadding();
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->LongPressEvent(gesture.GetState(), localPoint.x - padding.start, localPoint.y - padding.top);

  SetKeyInputFocus();
}

MeasuredSize InputFieldImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("[%p] widthConstraint:%f, heightConstraint:%f\n", mController.Get(), widthConstraint, heightConstraint);

  const float layoutWidth  = GetRequestedWidth();
  const float layoutHeight = GetRequestedHeight();

  const float minWidth  = GetMinimumWidth();
  const float maxWidth  = GetMaximumWidth();
  const float minHeight = GetMinimumHeight();
  const float maxHeight = GetMaximumHeight();

  Vector3 naturalSize = GetNaturalSize();

  float naturalWidth  = std::max(0.0f, naturalSize.width);
  float naturalHeight = std::max(0.0f, naturalSize.height);

  if(GetText().Empty())
  {
    // GetNaturalSize() includes view padding, but GetDefaultFontLineHeight() does not.
    // Therefore, when text is empty, padding must be added explicitly to keep
    // measurement consistent with the normal natural size path.
    Extents padding = GetViewPadding();
    naturalHeight   = mController->GetDefaultFontLineHeight() + (padding.top + padding.bottom);
  }

  float measuredWidth  = 0.0f;
  float measuredHeight = 0.0f;

  // Width
  if(layoutWidth > 0.0f)
  {
    measuredWidth = layoutWidth;
  }
  else if(layoutWidth == MATCH_PARENT)
  {
    measuredWidth = std::max(0.0f, widthConstraint);
  }
  else
  {
    // If widthConstraint is valid, also respect it.
    const float allowedMaxWidth = (widthConstraint >= 0.0f) ? std::min(maxWidth, widthConstraint) : maxWidth;
    measuredWidth               = std::max(std::min(naturalWidth, allowedMaxWidth), minWidth);
  }

  // Height
  if(layoutHeight > 0.0f)
  {
    measuredHeight = layoutHeight;
  }
  else if(layoutHeight == MATCH_PARENT)
  {
    measuredHeight = std::max(0.0f, heightConstraint);
  }
  else
  {
    const float allowedMaxHeight = (heightConstraint >= 0.0f) ? std::min(maxHeight, heightConstraint) : maxHeight;
    measuredHeight               = std::max(std::min(naturalHeight, allowedMaxHeight), minHeight);
  }

  DALI_LOG_RELEASE_INFO("[%p] measured:%f,%f\n", mController.Get(), measuredWidth, measuredHeight);

  return MeasuredSize(measuredWidth, measuredHeight);
}

MeasuredSize InputFieldImpl::OnArrange(const LayoutRect& bounds)
{
  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, bounds.x);
  self.SetProperty(Actor::Property::POSITION_Y, bounds.y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, bounds.width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, bounds.height);

  DALI_LOG_RELEASE_INFO("[%p] pos:%f,%f, size:%f,%f\n", mController.Get(), bounds.x, bounds.y, bounds.width,
                        bounds.height);
  return {bounds.width, bounds.height};
}

// =============================================================================
// ControlInterface
// =============================================================================
void InputFieldImpl::RequestTextRelayout()
{
  // Signal that a Relayout may be needed
  RelayoutRequest();
}

// =============================================================================
// EditableControlInterface
// =============================================================================
void InputFieldImpl::TextInserted(unsigned int position, unsigned int length, const std::string& content)
{
  // TODO: Accessible
}

void InputFieldImpl::TextDeleted(unsigned int position, unsigned int length, const std::string& content)
{
  // TODO: Accessible
}

void InputFieldImpl::CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition)
{
  if((oldPosition != newPosition) && !mCursorPositionChanged)
  {
    mCursorPositionChanged = true;
    mOldPosition           = oldPosition;
  }
}

void InputFieldImpl::TextChanged(bool immediate)
{
  if(immediate) // Emits TextChangedSignal immediately
  {
    EmitTextChangedSignal();
  }
  else
  {
    mTextChanged = true;
  }
}

void InputFieldImpl::MaxLengthReached()
{
  Ui::View handle(GetOwner());
  mMaxLengthReachedSignal.Emit(handle);
}

void InputFieldImpl::InputStyleChanged(Text::InputStyle::Mask inputStyleMask)
{
  // TODO
}

void InputFieldImpl::SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd)
{
  if(((oldStart != newStart) || (oldEnd != newEnd)) && !mSelectionChanged)
  {
    if(newStart == newEnd)
    {
      mSelectionCleared = true;
    }
    else
    {
      if(oldStart == oldEnd)
      {
        mSelectionStarted = true;
      }
    }

    mSelectionChanged  = true;
    mOldSelectionStart = oldStart;
    mOldSelectionEnd   = oldEnd;

    if(mOldSelectionStart > mOldSelectionEnd)
    {
      //swap
      uint32_t temp      = mOldSelectionStart;
      mOldSelectionStart = mOldSelectionEnd;
      mOldSelectionEnd   = temp;
    }
  }
}

void InputFieldImpl::AddDecoration(Actor& actor, Text::DecorationType type, bool needsClipping)
{
  if(actor)
  {
    if(needsClipping)
    {
      mClippingDecorationActors.push_back(actor);
    }

    // If the actor is a layer type, add it.
    if(type == Text::DecorationType::ACTIVE_LAYER)
    {
      AddLayer(mActiveLayer, actor);
    }
    else if(type == Text::DecorationType::CURSOR_LAYER)
    {
      AddLayer(mCursorLayer, actor);
    }
  }
}

void InputFieldImpl::InputFiltered(Ui::InputFilter::Property::Type type)
{
  // TODO
}

void InputFieldImpl::GetControlBackgroundColor(Vector4& color) const
{
  Property::Value propValue = Self().GetProperty(Ui::View::Property::BACKGROUND);
  Property::Map*  resultMap = propValue.GetMap();

  Property::Value* colorValue = nullptr;
  if(resultMap && (colorValue = resultMap->Find(ColorVisual::Property::MIX_COLOR)))
  {
    colorValue->Get(color);
  }
}

// =============================================================================
// SelectableControlInterface
// =============================================================================
void InputFieldImpl::SetTextSelectionRange(const uint32_t* start, const uint32_t* end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SetTextSelectionRange(start, end);
    SetKeyInputFocus();
  }
}

Uint32Pair InputFieldImpl::GetTextSelectionRange() const
{
  Uint32Pair range;
  if(mController && mController->IsShowingRealText())
  {
    range = mController->GetTextSelectionRange();
  }
  return range;
}

void InputFieldImpl::SelectWholeText()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectWholeText();
    SetKeyInputFocus();
  }
}

void InputFieldImpl::SelectNone()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectNone();
  }
}

void InputFieldImpl::SelectText(const uint32_t start, const uint32_t end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectText(start, end);
    SetKeyInputFocus();
  }
}

std::string InputFieldImpl::GetSelectedText() const
{
  std::string selectedText = "";
  if(mController && mController->IsShowingRealText())
  {
    selectedText = mController->GetSelectedText();
  }
  return selectedText;
}

bool InputFieldImpl::IsEditable() const
{
  return mController->IsEditable();
}

void InputFieldImpl::SetEditable(bool editable)
{
  mController->SetEditable(editable);
  if(mInputMethodContext && !editable)
  {
    mInputMethodContext.Deactivate();
  }
}

std::string InputFieldImpl::CopyText()
{
  std::string copiedText = "";
  if(mController && mController->IsShowingRealText())
  {
    copiedText = mController->CopyText();
  }
  return copiedText;
}

std::string InputFieldImpl::CutText()
{
  std::string cutText = "";
  if(mController && mController->IsShowingRealText())
  {
    cutText = mController->CutText();
  }
  return cutText;
}

void InputFieldImpl::PasteText()
{
  if(mController)
  {
    SetKeyInputFocus(); //Giving focus to the field that was passed to the PasteText in case the passed field (current field) doesn't have focus.
    mController->PasteText();
  }
}

// =============================================================================
// AnchorControlInterface
// =============================================================================
bool InputFieldImpl::AnchorClicked(uint32_t cursorPosition, std::string& href)
{
  return mController->AnchorClickEvent(cursorPosition, href);
}

void InputFieldImpl::EmitAnchorClickedSignal(const std::string& href)
{
  // TODO
}

// =============================================================================
// CustomActorImpl
// =============================================================================
void InputFieldImpl::OnChildAdd(Actor& child)
{
  // InputField internally uses non-View actors (e.g., layers, stencil, decorations).
  // Override to bypass ViewImpl's restriction that only allows View children.
}

// =============================================================================
// Implementation
// =============================================================================
InputMethodContext::CallbackData InputFieldImpl::OnInputMethodContextEvent(Dali::InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
{
  return mController->OnInputMethodContextEvent(inputMethodContext, inputMethodContextEvent);
}

void InputFieldImpl::OnSceneConnect(Dali::Actor actor)
{
  if(mHasBeenStaged)
  {
    RenderText(static_cast<Text::Controller::UpdateTextType>(Text::Controller::MODEL_UPDATED | Text::Controller::DECORATOR_UPDATED));
  }
  else
  {
    mHasBeenStaged = true;
  }
}

bool InputFieldImpl::OnTouched(Actor actor, const TouchEvent& touch)
{
  return false;
}

void InputFieldImpl::OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type)
{
  mController->ChangedLayoutDirection();
}

void InputFieldImpl::OnLocaleChanged(std::string locale)
{
  mController->InvalidateFontData();
}

void InputFieldImpl::OnKeyboardStatusChanged(bool keyboardShown)
{
  DALI_LOG_RELEASE_INFO("[%p] keyboardShown:%d\n", mController.Get(), keyboardShown);

  bool isFocused = false;

  Dali::Ui::KeyboardFocusManager keyboardFocusManager = Dali::Ui::KeyboardFocusManager::Get();
  if(keyboardFocusManager)
  {
    isFocused = keyboardFocusManager.GetCurrentFocusActor() == Self();
  }

  // Just hide the grab handle when keyboard is hidden.
  if(!keyboardShown)
  {
    if(!isFocused)
    {
      mController->KeyboardFocusLostEvent();
    }
  }
  else
  {
    mController->KeyboardFocusGainEvent(); // Initially called by OnKeyInputFocusGained
  }
}

void InputFieldImpl::EnableClipping()
{
  if(!mStencil)
  {
    // Creates an extra actor to be used as stencil buffer.
    mStencil = Actor::New();
    mStencil.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mStencil.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);

    // Enable the clipping property.
    mStencil.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    mStencil.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

    Self().Add(mStencil);
    if(mCursorLayer)
    {
      mStencil.Add(mCursorLayer);
    }
  }
}

void InputFieldImpl::ResizeActor(Actor& actor, const Vector2& size)
{
  if(actor.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
  {
    actor.SetProperty(Actor::Property::SIZE, size);
  }
}

void InputFieldImpl::AddLayer(Actor& layer, Actor& actor)
{
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  Self().Add(actor);
  layer = actor;
}

void InputFieldImpl::RenderText(Text::Controller::UpdateTextType updateTextType)
{
  Ui::Internal::CommonTextUtils::RenderText(Self(), mRenderer, mController, mDecorator, mAlignmentOffset, mRenderableActor, mBackgroundActor, mCursorLayer, mStencil, mClippingDecorationActors, mAnchorActors, updateTextType);
}

void InputFieldImpl::EmitTextChangedSignal()
{
  Ui::View handle(GetOwner());
  mTextChangedSignal.Emit(handle);
  mTextChanged = false;
}

// =============================================================================
// UiColorManager
// =============================================================================
void InputFieldImpl::SetTextColorInternal(const Vector4& color)
{
  if(mController->GetDefaultColor() != color)
  {
    mController->SetDefaultColor(color);
    mController->SetInputColor(color);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetPlaceholderColorInternal(const Vector4& color)
{
  if(mController->GetPlaceholderTextColor() != color)
  {
    mController->SetPlaceholderTextColor(color);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetCursorColorInternal(const Vector4& color)
{
  mDecorator->SetCursorColor(Text::PRIMARY_CURSOR, color);
  mDecorator->SetCursorColor(Text::SECONDARY_CURSOR, color);
  RequestTextRelayout();
}

void InputFieldImpl::SetSelectionColorInternal(const Vector4& color)
{
  mDecorator->SetHighlightColor(color);
  RequestTextRelayout();
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
