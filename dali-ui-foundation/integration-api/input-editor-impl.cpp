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
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/integration-api/input-editor-property-handler.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/internal/text/text-style-helper.h>

#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/internal/text/rendering/text-backend.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

#include <algorithm>

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

BaseHandle Create()
{
  return BaseHandle();
}

#define INPUT_EDITOR_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui::Text, InputEditorPropertyIndex, Ui::Integration, InputEditorImpl, text, valueType, enumIndex)

#define INPUT_EDITOR_PROPERTY_REGISTRATION_READ_ONLY(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_READ_ONLY_EXTERNAL(Ui::Text, InputEditorPropertyIndex, Ui::Integration, InputEditorImpl, text, valueType, enumIndex)

// clang-format off
// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(InputEditorImpl, ViewImpl, Create)

INPUT_EDITOR_PROPERTY_REGISTRATION("text",                             STRING,  TEXT                                )
INPUT_EDITOR_PROPERTY_REGISTRATION("fontFamily",                       STRING,  FONT_FAMILY                         )
INPUT_EDITOR_PROPERTY_REGISTRATION("fontSize",                         FLOAT,   FONT_SIZE                           )
INPUT_EDITOR_PROPERTY_REGISTRATION("textColor",                        VECTOR4, TEXT_COLOR                          )
INPUT_EDITOR_PROPERTY_REGISTRATION("lineWrapMode",                     INTEGER, LINE_WRAP_MODE                      )
INPUT_EDITOR_PROPERTY_REGISTRATION("horizontalAlignment",              INTEGER, HORIZONTAL_ALIGNMENT                )
INPUT_EDITOR_PROPERTY_REGISTRATION("verticalAlignment",                INTEGER, VERTICAL_ALIGNMENT                  )
INPUT_EDITOR_PROPERTY_REGISTRATION("overflowMode",                     INTEGER, OVERFLOW_MODE                       )
INPUT_EDITOR_PROPERTY_REGISTRATION("lineHeight",                       FLOAT,   LINE_HEIGHT                         )
INPUT_EDITOR_PROPERTY_REGISTRATION("lineHeightMode",                   INTEGER, LINE_HEIGHT_MODE                    )
INPUT_EDITOR_PROPERTY_REGISTRATION("placeholder",                      STRING,  PLACEHOLDER                         )
INPUT_EDITOR_PROPERTY_REGISTRATION("placeholderColor",                 VECTOR4, PLACEHOLDER_COLOR                   )
INPUT_EDITOR_PROPERTY_REGISTRATION("showPlaceholderOnFocus",           BOOLEAN, SHOW_PLACEHOLDER_ON_FOCUS           )
INPUT_EDITOR_PROPERTY_REGISTRATION("cursorWidth",                      INTEGER, CURSOR_WIDTH                        )
INPUT_EDITOR_PROPERTY_REGISTRATION("cursorColor",                      VECTOR4, CURSOR_COLOR                        )
INPUT_EDITOR_PROPERTY_REGISTRATION("cursorBlinkEnabled",               BOOLEAN, CURSOR_BLINK_ENABLED                )
INPUT_EDITOR_PROPERTY_REGISTRATION("cursorBlinkInterval",              FLOAT,   CURSOR_BLINK_INTERVAL               )
INPUT_EDITOR_PROPERTY_REGISTRATION("cursorPosition",                   INTEGER, CURSOR_POSITION                     )
INPUT_EDITOR_PROPERTY_REGISTRATION("selectionEnabled",                 BOOLEAN, SELECTION_ENABLED                   )
INPUT_EDITOR_PROPERTY_REGISTRATION("selectionColor",                   VECTOR4, SELECTION_COLOR                     )
INPUT_EDITOR_PROPERTY_REGISTRATION_READ_ONLY("selectedText",           STRING,  SELECTED_TEXT                       )
INPUT_EDITOR_PROPERTY_REGISTRATION_READ_ONLY("selectedTextStart",      INTEGER, SELECTED_TEXT_START                 )
INPUT_EDITOR_PROPERTY_REGISTRATION_READ_ONLY("selectedTextEnd",        INTEGER, SELECTED_TEXT_END                   )
INPUT_EDITOR_PROPERTY_REGISTRATION("textHandleEnabled",                BOOLEAN, TEXT_HANDLE_ENABLED                 )
INPUT_EDITOR_PROPERTY_REGISTRATION("textHandleColor",                  VECTOR4, TEXT_HANDLE_COLOR                   )
INPUT_EDITOR_PROPERTY_REGISTRATION("cursorHandleImage",                STRING,  CURSOR_HANDLE_IMAGE                 )
INPUT_EDITOR_PROPERTY_REGISTRATION("cursorHandlePressedImage",         STRING,  CURSOR_HANDLE_PRESSED_IMAGE         )
INPUT_EDITOR_PROPERTY_REGISTRATION("selectionHandleImageLeft",         STRING,  SELECTION_HANDLE_IMAGE_LEFT         )
INPUT_EDITOR_PROPERTY_REGISTRATION("selectionHandleImageRight",        STRING,  SELECTION_HANDLE_IMAGE_RIGHT        )
INPUT_EDITOR_PROPERTY_REGISTRATION("selectionHandlePressedImageLeft",  STRING,  SELECTION_HANDLE_PRESSED_IMAGE_LEFT )
INPUT_EDITOR_PROPERTY_REGISTRATION("selectionHandlePressedImageRight", STRING,  SELECTION_HANDLE_PRESSED_IMAGE_RIGHT)
INPUT_EDITOR_PROPERTY_REGISTRATION("maximumLength",                    INTEGER, MAXIMUM_LENGTH                      )
INPUT_EDITOR_PROPERTY_REGISTRATION("editable",                         BOOLEAN, EDITABLE                            )
INPUT_EDITOR_PROPERTY_REGISTRATION("layoutDirectionMode",              INTEGER, LAYOUT_DIRECTION_MODE               )
INPUT_EDITOR_PROPERTY_REGISTRATION("markupEnabled",                    BOOLEAN, MARKUP_ENABLED                      )
INPUT_EDITOR_PROPERTY_REGISTRATION("fontWeight",                       INTEGER, FONT_WEIGHT                         )
INPUT_EDITOR_PROPERTY_REGISTRATION("fontWidth",                        INTEGER, FONT_WIDTH                          )
INPUT_EDITOR_PROPERTY_REGISTRATION("fontSlant",                        INTEGER, FONT_SLANT                          )
INPUT_EDITOR_PROPERTY_REGISTRATION("textBackgroundColor",              VECTOR4, TEXT_BACKGROUND_COLOR               )
INPUT_EDITOR_PROPERTY_REGISTRATION("fontSizeScale",                    FLOAT,   FONT_SIZE_SCALE                     )
INPUT_EDITOR_PROPERTY_REGISTRATION("minimumFontSizeScale",             FLOAT,   MINIMUM_FONT_SIZE_SCALE             )
INPUT_EDITOR_PROPERTY_REGISTRATION("maximumFontSizeScale",             FLOAT,   MAXIMUM_FONT_SIZE_SCALE             )
INPUT_EDITOR_PROPERTY_REGISTRATION("systemFontSizeScaleEnabled",       BOOLEAN, SYSTEM_FONT_SIZE_SCALE_ENABLED      )
INPUT_EDITOR_PROPERTY_REGISTRATION("typingTextColor",                  VECTOR4, TYPING_TEXT_COLOR                   )
INPUT_EDITOR_PROPERTY_REGISTRATION("typingFontFamily",                 STRING,  TYPING_FONT_FAMILY                  )
INPUT_EDITOR_PROPERTY_REGISTRATION("typingFontSize",                   FLOAT,   TYPING_FONT_SIZE                    )
INPUT_EDITOR_PROPERTY_REGISTRATION("typingFontWeight",                 INTEGER, TYPING_FONT_WEIGHT                  )
INPUT_EDITOR_PROPERTY_REGISTRATION("typingFontWidth",                  INTEGER, TYPING_FONT_WIDTH                   )
INPUT_EDITOR_PROPERTY_REGISTRATION("typingFontSlant",                  INTEGER, TYPING_FONT_SLANT                   )

DALI_TYPE_REGISTRATION_END()
// clang-format on

/**
 * @brief Converts an input style change mask to a typing style change mask.
 * Only style attributes supported by TypingStyle are mapped.
 */
Text::TypingStyle::Mask ToTypingStyleMask(Text::InputStyle::Mask inputStyleMask)
{
  uint32_t typingStyleMask = Text::TypingStyle::NONE;

  if(inputStyleMask & Text::InputStyle::INPUT_COLOR)
  {
    typingStyleMask |= Text::TypingStyle::TEXT_COLOR;
  }

  if(inputStyleMask & Text::InputStyle::INPUT_FONT_FAMILY)
  {
    typingStyleMask |= Text::TypingStyle::FONT_FAMILY;
  }

  if(inputStyleMask & Text::InputStyle::INPUT_POINT_SIZE)
  {
    typingStyleMask |= Text::TypingStyle::FONT_SIZE;
  }

  if(inputStyleMask & Text::InputStyle::INPUT_FONT_WEIGHT)
  {
    typingStyleMask |= Text::TypingStyle::FONT_WEIGHT;
  }

  if(inputStyleMask & Text::InputStyle::INPUT_FONT_WIDTH)
  {
    typingStyleMask |= Text::TypingStyle::FONT_WIDTH;
  }

  if(inputStyleMask & Text::InputStyle::INPUT_FONT_SLANT)
  {
    typingStyleMask |= Text::TypingStyle::FONT_SLANT;
  }

  return static_cast<Text::TypingStyle::Mask>(typingStyleMask);
}

/**
 * @brief Sets key input focus via KeyInputFocusManager directly, bypassing FocusManager.
 *
 * Originally from ViewImpl::SetKeyInputFocus(). FocusManager's navigation focus
 * state is NOT updated by this call.
 *
 * @param[in] impl The ViewImpl whose handle should receive key input focus.
 */
void SetKeyInputFocus(ViewImpl& impl)
{
  Ui::View view = View::DownCast(impl.Self());
  if(view && view.IsOnScene())
  {
    Internal::KeyInputFocusManager::Get().SetFocus(view);
  }
}

/**
 * @brief Clears key input focus via KeyInputFocusManager directly, bypassing FocusManager.
 *
 * Originally from ViewImpl::ClearKeyInputFocus(). FocusManager's navigation focus
 * state is NOT affected.
 *
 * @param[in] impl The ViewImpl whose handle should lose key input focus.
 */
void ClearKeyInputFocus(ViewImpl& impl)
{
  Ui::View view = View::DownCast(impl.Self());
  if(view && view.IsOnScene())
  {
    Internal::KeyInputFocusManager::Get().RemoveFocus(view);
  }
}

/**
 * @brief Returns whether the given ViewImpl currently holds key input focus.
 *
 * Queries KeyInputFocusManager directly, independently of FocusManager's
 * navigation focus state.
 *
 * @param[in] impl The ViewImpl to check.
 * @return true if @p impl is the current key input focus owner, false otherwise.
 */
bool HasKeyInputFocus(ViewImpl& impl)
{
  bool result = false;
  if(impl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Ui::View currentFocusView = Internal::KeyInputFocusManager::Get().GetCurrentFocusView();
    result                    = (impl.Self() == currentFocusView);
  }
  return result;
}

/**
 * @brief Applies scale only to fixed size values.
 * Special size values such as WRAP_CONTENT and MATCH_PARENT are kept unchanged.
 */
float ScaleIfFixedSize(float value, float scale)
{
  return value >= 0.0f ? value * scale : value;
}

/**
 * @brief Restricts a value by applying the maximum bound first, then the minimum bound.
 * This keeps the minimum bound dominant when minValue is greater than maxValue.
 */
float ClampWithMinPriority(float value, float minValue, float maxValue)
{
  return std::max(std::min(value, maxValue), minValue);
}

} // namespace

InputEditorImplPtr InputEditorImpl::New()
{
  return InputEditorImplPtr(new InputEditorImpl());
}

InputEditorImpl::InputEditorImpl()
: ViewImpl(),
  mLineHeight(Text::LINE_HEIGHT_AUTO),
  mLineHeightMode(Text::LineHeightMode::RELATIVE),
  mOverflowMode(Text::OverflowMode::CLIP),
  mAlignmentOffset(0.f),
  mMeasureInvalidated(false),
  mHasBeenStaged(false),
  mTextChanged(false),
  mCursorPositionChanged(false),
  mSelectionStarted(false),
  mSelectionChanged(false),
  mSelectionCleared(false)
{
}

InputEditorImpl::~InputEditorImpl()
{
  UnparentAndReset(mStencil);
}

// =============================================================================
// Properties
// =============================================================================
void InputEditorImpl::SetText(const Dali::String& text)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), text.CStr());

  mController->SetText(ToStdString(text));
}

Dali::String InputEditorImpl::GetText() const
{
  std::string text;
  mController->GetText(text);
  return ToDaliString(text);
}

void InputEditorImpl::SetFontFamily(const Dali::String& fontFamily)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), fontFamily.CStr());

  mController->SetDefaultFontFamily(ToStdString(fontFamily));
}

Dali::String InputEditorImpl::GetFontFamily() const
{
  return ToDaliString(mController->GetDefaultFontFamily());
}

void InputEditorImpl::SetFontSize(float fontSize)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), fontSize);

  if(!Equals(mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), fontSize, Math::MACHINE_EPSILON_1000))
  {
    mController->SetDefaultFontSize(fontSize, Text::Controller::PIXEL_SIZE);
  }
}

float InputEditorImpl::GetFontSize() const
{
  return mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE);
}

void InputEditorImpl::SetTextColor(const UiColor& color)
{
  SetColorBinding("TextColor", color, this, &InputEditorImpl::SetTextColorInternal);
}

UiColor InputEditorImpl::GetTextColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "TextColor", outColor))
  {
    return outColor;
  }
  return mController->GetDefaultColor();
}

void InputEditorImpl::SetLineWrapMode(Text::LineWrapMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  mController->SetLineWrapMode(mode);
}

Text::LineWrapMode InputEditorImpl::GetLineWrapMode() const
{
  return mController->GetLineWrapMode();
}

void InputEditorImpl::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), alignment);
  mController->SetHorizontalAlignment(alignment);
}

Text::Alignment InputEditorImpl::GetHorizontalTextAlignment() const
{
  return mController->GetHorizontalAlignment();
}

void InputEditorImpl::SetVerticalTextAlignment(Text::Alignment alignment)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), alignment);
  mController->SetVerticalAlignment(alignment);
}

Text::Alignment InputEditorImpl::GetVerticalTextAlignment() const
{
  return mController->GetVerticalAlignment();
}

void InputEditorImpl::SetOverflowMode(Text::OverflowMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  if(mode != mOverflowMode)
  {
    mOverflowMode = mode;
    switch(mode)
    {
      case Text::OverflowMode::CLIP:
      {
        mController->SetTextElideEnabled(false);
        break;
      }
      case Text::OverflowMode::ELLIPSIS:
      {
        mController->SetTextElideEnabled(true);
        break;
      }
    }
    mController->InvalidateFontData();
  }
}

Text::OverflowMode InputEditorImpl::GetOverflowMode() const
{
  return mOverflowMode;
}

void InputEditorImpl::SetLineHeight(float lineHeight)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), lineHeight);
  if(mLineHeight != lineHeight)
  {
    mLineHeight = lineHeight;
    UpdateLineHeight();
  }
}

float InputEditorImpl::GetLineHeight() const
{
  return mLineHeight;
}

void InputEditorImpl::SetLineHeightMode(Text::LineHeightMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  if(mLineHeightMode != mode)
  {
    mLineHeightMode = mode;
    UpdateLineHeight();
  }
}

Text::LineHeightMode InputEditorImpl::GetLineHeightMode() const
{
  return mLineHeightMode;
}

void InputEditorImpl::SetPlaceholder(const Dali::String& text)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), text.CStr());

  const std::string placeholder = ToStdString(text);
  mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, placeholder);
  mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_ACTIVE, placeholder);
}

Dali::String InputEditorImpl::GetPlaceholder() const
{
  std::string text;
  mController->GetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, text);
  return ToDaliString(text);
}

void InputEditorImpl::SetPlaceholderColor(const UiColor& color)
{
  SetColorBinding("PlaceholderColor", color, this, &InputEditorImpl::SetPlaceholderColorInternal);
}

UiColor InputEditorImpl::GetPlaceholderColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "PlaceholderColor", outColor))
  {
    return outColor;
  }
  return mController->GetPlaceholderTextColor();
}

void InputEditorImpl::SetShowPlaceholderOnFocus(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);

  mController->SetShowPlaceholderOnFocus(enabled);
}

bool InputEditorImpl::IsPlaceholderShownOnFocus() const
{
  return mController->IsPlaceholderShownOnFocus();
}

void InputEditorImpl::SetCursorWidth(int width)
{
  DALI_LOG_RELEASE_INFO("[%p] width:%d\n", mController.Get(), width);

  mDecorator->SetCursorWidth(width);
  mController->GetLayoutEngine().SetCursorWidth(width);
}

int InputEditorImpl::GetCursorWidth() const
{
  return mDecorator->GetCursorWidth();
}

void InputEditorImpl::SetCursorColor(const UiColor& color)
{
  SetColorBinding("CursorColor", color, this, &InputEditorImpl::SetCursorColorInternal);
}

UiColor InputEditorImpl::GetCursorColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "CursorColor", outColor))
  {
    return outColor;
  }
  return mDecorator->GetColor(Text::PRIMARY_CURSOR);
}

void InputEditorImpl::SetCursorBlinkEnabled(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetEnableCursorBlink(enabled);
  RequestTextRelayout();
}

bool InputEditorImpl::IsCursorBlinkEnabled() const
{
  return mController->GetEnableCursorBlink();
}

void InputEditorImpl::SetCursorBlinkInterval(float interval)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), interval);
  mDecorator->SetCursorBlinkInterval(interval);
}

float InputEditorImpl::GetCursorBlinkInterval() const
{
  return mDecorator->GetCursorBlinkInterval();
}

void InputEditorImpl::SetCursorPosition(uint32_t position)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), position);
  if(mController->SetPrimaryCursorPosition(position, HasKeyInputFocus(*this)))
  {
    SetKeyInputFocus(*this);
  }
}

uint32_t InputEditorImpl::GetCursorPosition() const
{
  return mController->GetPrimaryCursorPosition();
}

void InputEditorImpl::SetSelectionEnabled(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetSelectionEnabled(enabled);
  mController->SetShiftSelectionEnabled(enabled);
}

bool InputEditorImpl::IsSelectionEnabled() const
{
  return mController->IsSelectionEnabled();
}

void InputEditorImpl::SetSelectionColor(const UiColor& color)
{
  SetColorBinding("SelectionColor", color, this, &InputEditorImpl::SetSelectionColorInternal);
}

UiColor InputEditorImpl::GetSelectionColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "SelectionColor", outColor))
  {
    return outColor;
  }
  return mDecorator->GetHighlightColor();
}

void InputEditorImpl::SetTextHandleEnabled(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetGrabHandleEnabled(enabled);
  RequestTextRelayout();
}

bool InputEditorImpl::IsTextHandleEnabled() const
{
  return mController->IsGrabHandleEnabled();
}

void InputEditorImpl::SetTextHandleColor(const UiColor& color)
{
  SetColorBinding("TextHandleColor", color, this, &InputEditorImpl::SetTextHandleColorInternal);
}

UiColor InputEditorImpl::GetTextHandleColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "TextHandleColor", outColor))
  {
    return outColor;
  }
  return mDecorator->GetHandleColor();
}

void InputEditorImpl::SetCursorHandleImage(const Dali::String& image)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), image.CStr());
  mDecorator->SetHandleImage(Text::GRAB_HANDLE, Text::HANDLE_IMAGE_RELEASED, ToStdString(image));
  RequestTextRelayout();
}

Dali::String InputEditorImpl::GetCursorHandleImage() const
{
  return ToDaliString(mDecorator->GetHandleImage(Text::GRAB_HANDLE, Text::HANDLE_IMAGE_RELEASED));
}

void InputEditorImpl::SetCursorHandlePressedImage(const Dali::String& image)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), image.CStr());
  mDecorator->SetHandleImage(Text::GRAB_HANDLE, Text::HANDLE_IMAGE_PRESSED, ToStdString(image));
  RequestTextRelayout();
}

Dali::String InputEditorImpl::GetCursorHandlePressedImage() const
{
  return ToDaliString(mDecorator->GetHandleImage(Text::GRAB_HANDLE, Text::HANDLE_IMAGE_PRESSED));
}

void InputEditorImpl::SetSelectionHandleImageLeft(const Dali::String& image)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), image.CStr());
  mDecorator->SetHandleImage(Text::LEFT_SELECTION_HANDLE, Text::HANDLE_IMAGE_RELEASED, ToStdString(image));
  RequestTextRelayout();
}

Dali::String InputEditorImpl::GetSelectionHandleImageLeft() const
{
  return ToDaliString(mDecorator->GetHandleImage(Text::LEFT_SELECTION_HANDLE, Text::HANDLE_IMAGE_RELEASED));
}

void InputEditorImpl::SetSelectionHandleImageRight(const Dali::String& image)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), image.CStr());
  mDecorator->SetHandleImage(Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_IMAGE_RELEASED, ToStdString(image));
  RequestTextRelayout();
}

Dali::String InputEditorImpl::GetSelectionHandleImageRight() const
{
  return ToDaliString(mDecorator->GetHandleImage(Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_IMAGE_RELEASED));
}

void InputEditorImpl::SetSelectionHandlePressedImageLeft(const Dali::String& image)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), image.CStr());
  mDecorator->SetHandleImage(Text::LEFT_SELECTION_HANDLE, Text::HANDLE_IMAGE_PRESSED, ToStdString(image));
  RequestTextRelayout();
}

Dali::String InputEditorImpl::GetSelectionHandlePressedImageLeft() const
{
  return ToDaliString(mDecorator->GetHandleImage(Text::LEFT_SELECTION_HANDLE, Text::HANDLE_IMAGE_PRESSED));
}

void InputEditorImpl::SetSelectionHandlePressedImageRight(const Dali::String& image)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), image.CStr());
  mDecorator->SetHandleImage(Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_IMAGE_PRESSED, ToStdString(image));
  RequestTextRelayout();
}

Dali::String InputEditorImpl::GetSelectionHandlePressedImageRight() const
{
  return ToDaliString(mDecorator->GetHandleImage(Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_IMAGE_PRESSED));
}

void InputEditorImpl::SetMaximumLength(int length)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), length);
  mController->SetMaximumNumberOfCharacters(static_cast<uint32_t>(length));
}

int InputEditorImpl::GetMaximumLength() const
{
  return static_cast<int>(mController->GetMaximumNumberOfCharacters());
}

void InputEditorImpl::SetInputFilter(const Text::InputFilter& inputFilter)
{
  DALI_LOG_RELEASE_INFO("[%p] pattern allow:%s, deny:%s\n", mController.Get(), inputFilter.GetAllowPattern().CStr(), inputFilter.GetDenyPattern().CStr());
  mController->SetInputFilter(inputFilter);
}

void InputEditorImpl::ClearInputFilter()
{
  mController->ClearInputFilter();
}

void InputEditorImpl::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  if(mController->GetLayoutDirectionMode() != mode)
  {
    mController->SetLayoutDirectionMode(mode);
    RequestTextRelayout();
  }
}

Text::LayoutDirectionMode InputEditorImpl::GetLayoutDirectionMode() const
{
  return mController->GetLayoutDirectionMode();
}

void InputEditorImpl::SetMarkupEnabled(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetMarkupProcessorEnabled(enabled);
}

bool InputEditorImpl::IsMarkupEnabled() const
{
  return mController->IsMarkupProcessorEnabled();
}

void InputEditorImpl::SetFontWeight(Text::FontWeight weight)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), Text::GetFontWeightName(weight));
  mController->SetDefaultFontWeight(Text::ToTextAbstractionFontWeight(weight));
}

Text::FontWeight InputEditorImpl::GetFontWeight() const
{
  return Text::ToFontWeight(mController->GetDefaultFontWeight());
}

void InputEditorImpl::SetFontWidth(Text::FontWidth width)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), Text::GetFontWidthName(width));
  mController->SetDefaultFontWidth(Text::ToTextAbstractionFontWidth(width));
}

Text::FontWidth InputEditorImpl::GetFontWidth() const
{
  return Text::ToFontWidth(mController->GetDefaultFontWidth());
}

void InputEditorImpl::SetFontSlant(Text::FontSlant slant)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), Text::GetFontSlantName(slant));
  mController->SetDefaultFontSlant(Text::ToTextAbstractionFontSlant(slant));
}

Text::FontSlant InputEditorImpl::GetFontSlant() const
{
  return Text::ToFontSlant(mController->GetDefaultFontSlant());
}

void InputEditorImpl::SetTextBackgroundColor(const UiColor& color)
{
  SetColorBinding("TextBackgroundColor", color, this, &InputEditorImpl::SetTextBackgroundColorInternal);
  if(!mController->IsBackgroundEnabled())
  {
    mController->SetBackgroundEnabled(true);
  }
}

UiColor InputEditorImpl::GetTextBackgroundColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "TextBackgroundColor", outColor))
  {
    return outColor;
  }
  return mController->GetBackgroundColor();
}

void InputEditorImpl::ClearTextBackgroundColor()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "TextBackgroundColor");
  if(mController->IsBackgroundEnabled())
  {
    mController->SetBackgroundEnabled(false);
    mController->SetBackgroundColor(Color::TRANSPARENT);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetUnderline(const Text::Underline& underline)
{
  const UiColor& color = underline.GetColor();

  SetColorBinding("UnderlineColor", color, this, &InputEditorImpl::SetUnderlineColorInternal);

  if(Text::ApplyUnderlineStyle(mController, underline))
  {
    mRenderer.Reset();
  }
}

void InputEditorImpl::ClearUnderline()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "UnderlineColor");
  if(mController->IsUnderlineEnabled())
  {
    mController->SetUnderlineEnabled(false);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetShadow(const Text::Shadow& shadow)
{
  const UiColor& color = shadow.GetColor();

  SetColorBinding("ShadowColor", color, this, &InputEditorImpl::SetShadowColorInternal);

  if(Text::ApplyShadowStyle(mController, shadow))
  {
    mRenderer.Reset();
  }
}

void InputEditorImpl::ClearShadow()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "ShadowColor");
  if(Vector2::ZERO != mController->GetShadowOffset())
  {
    mController->SetShadowOffset(Vector2::ZERO);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetOutline(const Text::Outline& outline)
{
  const UiColor& color = outline.GetColor();

  SetColorBinding("OutlineColor", color, this, &InputEditorImpl::SetOutlineColorInternal);

  if(Text::ApplyOutlineStyle(mController, outline))
  {
    mRenderer.Reset();
  }
}

void InputEditorImpl::ClearOutline()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "OutlineColor");
  if(0u != mController->GetOutlineWidth())
  {
    mController->SetOutlineWidth(0u);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetLineThrough(const Text::LineThrough& lineThrough)
{
  const UiColor& color = lineThrough.GetColor();

  SetColorBinding("LineThroughColor", color, this, &InputEditorImpl::SetLineThroughColorInternal);

  if(Text::ApplyLineThroughStyle(mController, lineThrough))
  {
    mRenderer.Reset();
  }
}

void InputEditorImpl::ClearLineThrough()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "LineThroughColor");
  if(mController->IsStrikethroughEnabled())
  {
    mController->SetStrikethroughEnabled(false);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetFontSizeScale(scale);
}

float InputEditorImpl::GetFontSizeScale() const
{
  return mController->GetFontSizeScale();
}

void InputEditorImpl::SetMinimumFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetMinimumFontSizeScale(scale);
}

float InputEditorImpl::GetMinimumFontSizeScale() const
{
  return mController->GetMinimumFontSizeScale();
}

void InputEditorImpl::SetMaximumFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetMaximumFontSizeScale(scale);
}

float InputEditorImpl::GetMaximumFontSizeScale() const
{
  return mController->GetMaximumFontSizeScale();
}

void InputEditorImpl::SetSystemFontSizeScaleEnabled(bool enabled)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetSystemFontSizeScaleEnabled(enabled);
}

bool InputEditorImpl::IsSystemFontSizeScaleEnabled() const
{
  return mController->IsSystemFontSizeScaleEnabled();
}

void InputEditorImpl::SetTypingTextColor(const UiColor& color)
{
  SetColorBinding("TypingTextColor", color, this, &InputEditorImpl::SetTypingTextColorInternal);
}

UiColor InputEditorImpl::GetTypingTextColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "TypingTextColor", outColor))
  {
    return outColor;
  }
  return mController->GetInputColor();
}

void InputEditorImpl::SetTypingFontFamily(const Dali::String& fontFamily)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), fontFamily.CStr());
  mController->SetInputFontFamily(ToStdString(fontFamily));
}

Dali::String InputEditorImpl::GetTypingFontFamily() const
{
  return ToDaliString(mController->GetInputFontFamily());
}

void InputEditorImpl::SetTypingFontSize(float fontSize)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), fontSize);
  mController->SetInputFontSize(fontSize, Text::Controller::PIXEL_SIZE);
}

float InputEditorImpl::GetTypingFontSize() const
{
  return mController->GetInputFontSize(Text::Controller::PIXEL_SIZE);
}

void InputEditorImpl::SetTypingFontWeight(Text::FontWeight weight)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), Text::GetFontWeightName(weight));
  mController->SetInputFontWeight(Text::ToTextAbstractionFontWeight(weight));
}

Text::FontWeight InputEditorImpl::GetTypingFontWeight() const
{
  return Text::ToFontWeight(mController->GetInputFontWeight());
}

void InputEditorImpl::SetTypingFontWidth(Text::FontWidth width)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), Text::GetFontWidthName(width));
  mController->SetInputFontWidth(Text::ToTextAbstractionFontWidth(width));
}

Text::FontWidth InputEditorImpl::GetTypingFontWidth() const
{
  return Text::ToFontWidth(mController->GetInputFontWidth());
}

void InputEditorImpl::SetTypingFontSlant(Text::FontSlant slant)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), Text::GetFontSlantName(slant));
  mController->SetInputFontSlant(Text::ToTextAbstractionFontSlant(slant));
}

Text::FontSlant InputEditorImpl::GetTypingFontSlant() const
{
  return Text::ToFontSlant(mController->GetInputFontSlant());
}

void InputEditorImpl::SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] number of candidates:%u\n", mController.Get(), axes.Count());
  mController->SetVariations(axes);
}

void InputEditorImpl::SetFontVariation(const Dali::String& settings)
{
  if(settings.Empty())
  {
    DALI_LOG_WARNING("[%p] Empty font variation string is not allowed. Use ClearFontVariation() instead.\n", mController.Get());
    return;
  }

  auto axes = Text::FontVariation::FromString(settings);
  if(axes.Empty())
  {
    DALI_LOG_WARNING("[%p] Failed to parse font variation string: %s\n", mController.Get(), settings.CStr());
    return;
  }

  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), settings.CStr());
  SetFontVariation(axes);
}

Dali::Vector<Text::FontVariationAxis> InputEditorImpl::GetFontVariation() const
{
  return mController->GetVariations();
}

void InputEditorImpl::ClearFontVariation()
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  mController->ClearVariationsMap();
}

// Integration-only implementation for now until public API support is introduced.
void InputEditorImpl::SetLetterSpacing(float spacing)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), spacing);
  mController->SetCharacterSpacing(spacing);
}

float InputEditorImpl::GetLetterSpacing() const
{
  return mController->GetCharacterSpacing();
}

// =============================================================================
// Read Only
// =============================================================================
float InputEditorImpl::GetAdjustedFontSizeScale() const
{
  return mController->GetAdjustedFontSizeScale();
}

uint32_t InputEditorImpl::GetSelectedTextStart() const
{
  Uint32Pair range = mController->GetTextSelectionRange();
  return range.first;
}

uint32_t InputEditorImpl::GetSelectedTextEnd() const
{
  Uint32Pair range = mController->GetTextSelectionRange();
  return range.second;
}

// =============================================================================
// Method
// =============================================================================

// =============================================================================
// Signals
// =============================================================================
Signal<void(View)>& InputEditorImpl::TextChangedSignal()
{
  return mTextChangedSignal;
}

Signal<void(View)>& InputEditorImpl::MaximumLengthReachedSignal()
{
  return mMaxLengthReachedSignal;
}

Signal<void(View, Text::InputFilter::RejectReason)>& InputEditorImpl::InputRejectedSignal()
{
  return mInputRejectedSignal;
}

Signal<void(View, uint32_t)>& InputEditorImpl::CursorPositionChangedSignal()
{
  return mCursorPositionChangedSignal;
}

Signal<void(View)>& InputEditorImpl::SelectionStartedSignal()
{
  return mSelectionStartedSignal;
}

Signal<void(View, uint32_t, uint32_t)>& InputEditorImpl::SelectionChangedSignal()
{
  return mSelectionChangedSignal;
}

Signal<void(View)>& InputEditorImpl::SelectionClearedSignal()
{
  return mSelectionClearedSignal;
}

Signal<void(View, Text::TypingStyle::Mask)>& InputEditorImpl::TypingStyleChangedSignal()
{
  return mTypingStyleChangedSignal;
}

// =============================================================================
// Config
// =============================================================================
void InputEditorImpl::ApplyInitialConfig()
{
  // UiConfigManager may not be initialized during preload phase
  auto configureManager = UiConfigManager::Get();
  if(!configureManager.IsInitialized())
  {
    DALI_LOG_RELEASE_INFO("ApplyInitialConfig skipped: UiConfigManager is not initialized (possible preload phase)\n");
    return;
  }

  const auto& config = configureManager.GetConfig();
  SetFontSize(config.GetDefaultFontSize());
  SetTextColor(config.GetDefaultTextColor());
  SetPlaceholderColor(config.GetDefaultPlaceholderTextColor());
  SetShowPlaceholderOnFocus(config.IsPlaceholderTextShownOnFocus());
}

// =============================================================================
// UiScale
// =============================================================================
bool InputEditorImpl::SetTextUiScale(float scale)
{
  mDecorator->SetUiScale(scale);
  return mController->SetUiScale(scale);
}

float InputEditorImpl::GetTextUiScale() const
{
  return mController->GetUiScale();
}

Extents InputEditorImpl::GetEffectiveTextPadding() const
{
  Extents     padding     = GetPadding();
  const float textUiScale = GetTextUiScale();
  padding.start           = static_cast<uint16_t>(static_cast<float>(padding.start) * textUiScale);
  padding.end             = static_cast<uint16_t>(static_cast<float>(padding.end) * textUiScale);
  padding.top             = static_cast<uint16_t>(static_cast<float>(padding.top) * textUiScale);
  padding.bottom          = static_cast<uint16_t>(static_cast<float>(padding.bottom) * textUiScale);
  return padding;
}

// =============================================================================
// ViewImpl
// =============================================================================
void InputEditorImpl::OnInitialize()
{
  // Call base class initialization
  ViewImpl::OnInitialize();

  Actor self = Self();

  mController = Text::Controller::New(this, this, this, this);
  mController->SetGlyphType(TextAbstraction::BITMAP_GLYPH);
  mDecorator = Text::Decorator::New(*mController, *mController);

  mInputMethodContext = InputMethodContext::New(self);

  mController->GetLayoutEngine().SetLayout(Text::Layout::Engine::MULTI_LINE_BOX);

  // Enables the text input.
  mController->EnableTextInput(mDecorator, mInputMethodContext);

  // Disable horizontal scrolling for multi-line text wrapping.
  mController->SetHorizontalScrollEnabled(false);

  // Enable vertical scrolling for multi-line editing.
  mController->SetVerticalScrollEnabled(true);

  // Disable the smooth handle panning.
  mController->SetSmoothHandlePanEnabled(false);

  mController->SetNoTextDoubleTapAction(Text::Controller::NoTextTap::HIGHLIGHT);
  mController->SetNoTextLongPressAction(Text::Controller::NoTextTap::HIGHLIGHT);

  // Disable the text ellipsis.
  mController->SetTextElideEnabled(false);

  // Sets layoutDirection value
  Dali::Stage                 stage           = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(stage.GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  mController->SetLayoutDirection(layoutDirection);

  self.LayoutDirectionChangedSignal().Connect(this, &InputEditorImpl::OnLayoutDirectionChanged);

  auto viewHandle = View::DownCast(self);
  viewHandle.SetFocusable(true);
  viewHandle.SetTouchFocusable(true);

  if(Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &InputEditorImpl::OnLocaleChanged);
  }

  // Forward input events to controller
  mTapGestureDetector = TapGestureDetector::New();
  mTapGestureDetector.SetMaximumTapsRequired(2);
  mTapGestureDetector.ReceiveAllTapEvents(true);
  mTapGestureDetector.DetectedSignal().Connect(this, &InputEditorImpl::OnTapDetected);
  mTapGestureDetector.Attach(self);

  mPanGestureDetector = PanGestureDetector::New();
  mPanGestureDetector.SetMaximumTouchesRequired(2);
  mPanGestureDetector.DetectedSignal().Connect(this, &InputEditorImpl::OnPanDetected);
  mPanGestureDetector.Attach(self);

  mLongPressGestureDetector = LongPressGestureDetector::New();
  mLongPressGestureDetector.DetectedSignal().Connect(this, &InputEditorImpl::OnLongPressDetected);
  mLongPressGestureDetector.Attach(self);

  self.TouchedSignal().Connect(this, &InputEditorImpl::OnTouched);

  // Set BoundingBox to stage size if not already set.
  BoundsInteger boundingBox;
  mDecorator->GetBoundingBox(boundingBox);

  if(boundingBox.IsEmpty())
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    mDecorator->SetBoundingBox(BoundsInteger(0.0f, 0.0f, stageSize.width, stageSize.height));
  }

  // Flip vertically the 'left' selection handle
  mDecorator->FlipHandleVertically(Text::LEFT_SELECTION_HANDLE, true);

  // Fill-parent area by default
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT);
  self.OnSceneSignal().Connect(this, &InputEditorImpl::OnSceneConnect);

  View      view         = Dali::Ui::View::DownCast(self);
  ViewImpl& viewInternal = Ui::GetImpl(view);
  Internal::ViewDataImpl::Get(viewInternal).SetInputMethodContext(mInputMethodContext);

  EnableClipping();

  // TODO: Re-enable when grab handle and popup support are fully implemented.
  mController->SetGrabHandleEnabled(false);
  mController->SetGrabHandlePopupEnabled(false);

  ApplyInitialConfig();
}

void InputEditorImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Actor self = Self();

  Extents padding = GetEffectiveTextPadding();
  float   width   = std::max(size.x - static_cast<float>(padding.start + padding.end), 0.0f);
  float   height  = std::max(size.y - static_cast<float>(padding.top + padding.bottom), 0.0f);
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
    EmitTextChanged();
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
    EmitCursorPositionChanged();
  }

  if(mSelectionStarted)
  {
    EmitSelectionStarted();
  }

  if(mSelectionChanged)
  {
    EmitSelectionChanged();
  }

  if(mSelectionCleared)
  {
    EmitSelectionCleared();
  }

  // The input-editor emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The input-editor adds an idle callback to the adaptor to emit the signals after the size negotiation.
  if(!mController->IsInputStyleChangedSignalsQueueEmpty())
  {
    mController->RequestProcessInputStyleChangedSignals();
  }
}

Vector3 InputEditorImpl::GetNaturalSize()
{
  Extents padding     = GetEffectiveTextPadding();
  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += static_cast<float>(padding.start + padding.end);
  naturalSize.height += static_cast<float>(padding.top + padding.bottom);
  return naturalSize;
}

float InputEditorImpl::GetHeightForWidth(float width)
{
  Extents padding      = GetEffectiveTextPadding();
  float   contentWidth = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  return mController->GetHeightForWidth(contentWidth) + static_cast<float>(padding.top + padding.bottom);
}

void InputEditorImpl::OnFocusChanged(bool focused)
{
  if(focused)
  {
    OnFocusGained();
  }
  else
  {
    OnFocusLost();
  }
  ViewImpl::OnFocusChanged(focused);
}

void InputEditorImpl::OnFocusGained()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  if(mInputMethodContext && IsEditable())
  {
    // All input panel properties, such as layout, return key type, and input hint, should be set before input panel activates (or shows).
    mInputMethodContext.ApplyOptions(mInputMethodOptions);
    mInputMethodContext.NotifyTextInputMultiLine(true);

    mInputMethodContext.StatusChangedSignal().Connect(this, &InputEditorImpl::OnKeyboardStatusChanged);

    mInputMethodContext.KeyboardEventReceivedSignal().Connect(this, &InputEditorImpl::OnInputMethodContextEvent);

    // Notify that the text editing start.
    mInputMethodContext.Activate();

    // When window gain lost focus, the inputMethodContext is deactivated. Thus when window gain focus again, the inputMethodContext must be activated.
    mInputMethodContext.SetRestoreAfterFocusLost(true);
  }

  if(IsEditable() && mController->IsUserInteractionEnabled())
  {
    mController->KeyboardFocusGainEvent(); // Called in the case of no virtual keyboard to trigger this event
  }
}

void InputEditorImpl::OnFocusLost()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  if(mInputMethodContext)
  {
    mInputMethodContext.StatusChangedSignal().Disconnect(this, &InputEditorImpl::OnKeyboardStatusChanged);
    // The text editing is finished. Therefore the inputMethodContext don't have restore activation.
    mInputMethodContext.SetRestoreAfterFocusLost(false);

    // Notify that the text editing finish.
    mInputMethodContext.Deactivate();

    mInputMethodContext.KeyboardEventReceivedSignal().Disconnect(this, &InputEditorImpl::OnInputMethodContextEvent);
  }

  mController->KeyboardFocusLostEvent();
}

void InputEditorImpl::OnSceneConnection(int depth)
{
  // Sets the depth to the visuals inside the text's decorator.
  mDecorator->SetTextDepth(depth);

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().

  // Call the Control::OnSceneConnection() to set the depth of the background.
  ViewImpl::OnSceneConnection(depth);
}

bool InputEditorImpl::OnKeyEvent(const KeyEvent& event)
{
  DALI_LOG_RELEASE_INFO("[%p] keyCode:%d\n", mController.Get(), event.GetKeyCode());

  if(Dali::DALI_KEY_ESCAPE == event.GetKeyCode() && mController->ShouldClearFocusOnEscape())
  {
    // Make sure ClearKeyInputFocus when only key is up
    if(event.GetState() == KeyEvent::UP)
    {
      Dali::Ui::FocusManager focusManager = Dali::Ui::FocusManager::Get();
      if(focusManager)
      {
        focusManager.ClearFocus();
      }
      ClearKeyInputFocus(*this);
    }

    return true;
  }

  return mController->KeyEvent(event);
}

void InputEditorImpl::OnTapDetected(Actor actor, TapGesture gesture)
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());

  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  Extents        padding    = GetEffectiveTextPadding();
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->TapEvent(gesture.GetNumberOfTaps(), localPoint.x - static_cast<float>(padding.start), localPoint.y - static_cast<float>(padding.top));
  mController->AnchorEvent(localPoint.x - static_cast<float>(padding.start), localPoint.y - static_cast<float>(padding.top));

  Dali::Ui::FocusManager keyboardFocusManager = Dali::Ui::FocusManager::Get();
  if(keyboardFocusManager)
  {
    keyboardFocusManager.SetCurrentFocusView(Ui::View::DownCast(Self()));
  }
  SetKeyInputFocus(*this);
}

void InputEditorImpl::OnPanDetected(Actor actor, PanGesture gesture)
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

void InputEditorImpl::OnLongPressDetected(Actor actor, LongPressGesture gesture)
{
  if(mInputMethodContext && IsEditable())
  {
    mInputMethodContext.Activate();
  }
  Extents        padding    = GetEffectiveTextPadding();
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->LongPressEvent(gesture.GetState(), localPoint.x - static_cast<float>(padding.start), localPoint.y - static_cast<float>(padding.top));
  SetKeyInputFocus(*this);
}

MeasuredSize InputEditorImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("[%p] widthConstraint:%f, heightConstraint:%f\n", mController.Get(), widthConstraint, heightConstraint);

  mMeasureInvalidated = false;

  const float effectiveScale = GetEffectiveScale();
  if(SetTextUiScale(effectiveScale))
  {
    mController->InvalidateFontData();
  }

  const float requestedWidth  = ScaleIfFixedSize(GetRequestedWidth(), effectiveScale);
  const float requestedHeight = ScaleIfFixedSize(GetRequestedHeight(), effectiveScale);

  const float minWidth  = GetMinimumWidth() * effectiveScale;
  const float maxWidth  = GetMaximumWidth() * effectiveScale;
  const float minHeight = GetMinimumHeight() * effectiveScale;
  const float maxHeight = GetMaximumHeight() * effectiveScale;

  float measuredWidth  = 0.0f;
  float measuredHeight = 0.0f;

  // Width
  if(requestedWidth >= 0.0f)
  {
    measuredWidth = ClampWithMinPriority(requestedWidth, minWidth, maxWidth);
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    // MATCH_PARENT: report minimum desired size; actual size is determined
    // by the parent during the Arrange phase.
    measuredWidth = minWidth;
  }
  else // WRAP_CONTENT
  {
    const Vector3 naturalSize     = GetNaturalSize();
    const float   naturalWidth    = std::max(0.0f, naturalSize.width);
    const float   allowedMaxWidth = (widthConstraint >= 0.0f) ? std::min(maxWidth, widthConstraint) : maxWidth;

    measuredWidth = ClampWithMinPriority(naturalWidth, minWidth, allowedMaxWidth);
  }

  // Height
  if(requestedHeight >= 0.0f)
  {
    measuredHeight = ClampWithMinPriority(requestedHeight, minHeight, maxHeight);
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    measuredHeight = minHeight;
  }
  else // WRAP_CONTENT
  {
    const float allowedMaxHeight = (heightConstraint >= 0.0f) ? std::min(maxHeight, heightConstraint) : maxHeight;

    // When width is MATCH_PARENT, measuredWidth is minWidth (0 by default).
    // Use widthConstraint for height calculation since that represents the
    // actual available width the editor will receive in Arrange.
    const float widthForHeight = (requestedWidth == MATCH_PARENT) ? std::max(0.0f, widthConstraint) : measuredWidth;

    float height = 0.0f;
    if(widthForHeight > 0.0f)
    {
      height = std::max(0.0f, GetHeightForWidth(widthForHeight));
    }

    if(GetText().Empty())
    {
      // GetNaturalSize() includes view padding, but GetDefaultFontLineHeight() does not.
      // Therefore, when text is empty, padding must be added explicitly to keep
      // measurement consistent with the normal natural size path.
      const Extents padding = GetEffectiveTextPadding();
      height                = mController->GetDefaultFontLineHeight() + padding.top + padding.bottom;
    }

    measuredHeight = ClampWithMinPriority(height, minHeight, allowedMaxHeight);
  }

  DALI_LOG_RELEASE_INFO("[%p] measured:%f,%f\n", mController.Get(), measuredWidth, measuredHeight);
  return MeasuredSize(measuredWidth, measuredHeight);
}

MeasuredSize InputEditorImpl::OnArrange(const LayoutRect& bounds)
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
void InputEditorImpl::RequestTextRelayout()
{
  // Signal that a Relayout may be needed
  RelayoutRequest();
}

void InputEditorImpl::InvalidateTextMeasure()
{
  if(!mMeasureInvalidated)
  {
    // Only invalidate measure when size depends on content.
    if(GetRequestedWidth() == WRAP_CONTENT || GetRequestedHeight() == WRAP_CONTENT)
    {
      InvalidateMeasure();
      mMeasureInvalidated = true;
    }
  }
}

void InputEditorImpl::RequestAsyncRender()
{
}

// =============================================================================
// EditableControlInterface
// =============================================================================
void InputEditorImpl::AddDecoration(Actor& actor, Text::DecorationType type, bool needsClipping)
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

void InputEditorImpl::GetControlBackgroundColor(Vector4& color) const
{
  Property::Value propValue = Self().GetProperty(Ui::View::Property::BACKGROUND);
  Property::Map*  resultMap = propValue.GetMap();

  Property::Value* colorValue = nullptr;
  if(resultMap && (colorValue = resultMap->Find(Visual::Property::MIX_COLOR)))
  {
    colorValue->Get(color);
  }
}

bool InputEditorImpl::IsEditable() const
{
  return mController->IsEditable();
}

void InputEditorImpl::SetEditable(bool editable)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), editable);
  mController->SetEditable(editable);
  if(mInputMethodContext && !editable)
  {
    mInputMethodContext.Deactivate();
  }
}

std::string InputEditorImpl::CopyText()
{
  std::string copiedText = "";
  if(mController && mController->IsShowingRealText())
  {
    copiedText = mController->CopyText();
  }
  return copiedText;
}

std::string InputEditorImpl::CutText()
{
  std::string cutText = "";
  if(mController && mController->IsShowingRealText())
  {
    cutText = mController->CutText();
  }
  return cutText;
}

void InputEditorImpl::PasteText()
{
  if(mController)
  {
    SetKeyInputFocus(*this);
    mController->PasteText();
  }
}

void InputEditorImpl::TextChanged(bool immediate)
{
  if(immediate) // Emits TextChanged signal immediately
  {
    EmitTextChanged();
  }
  else
  {
    mTextChanged = true;
  }
}

void InputEditorImpl::MaximumLengthReached()
{
  EmitMaximumLengthReached();
}

void InputEditorImpl::CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition)
{
  if((oldPosition != newPosition) && !mCursorPositionChanged)
  {
    mCursorPositionChanged = true;
  }
}

void InputEditorImpl::InputStyleChanged(Text::InputStyle::Mask inputStyleMask)
{
  const Text::TypingStyle::Mask mask = ToTypingStyleMask(inputStyleMask);
  if(mask != Text::TypingStyle::NONE)
  {
    EmitTypingStyleChanged(mask);
  }
}

void InputEditorImpl::InputRejected(Text::InputFilter::RejectReason reason)
{
  EmitInputRejected(reason);
}

void InputEditorImpl::TextInserted(unsigned int position, unsigned int length, const std::string& content)
{
  // TODO: Accessible
}

void InputEditorImpl::TextDeleted(unsigned int position, unsigned int length, const std::string& content)
{
  // TODO: Accessible
}

// =============================================================================
// SelectableControlInterface
// =============================================================================
void InputEditorImpl::SelectText(const uint32_t start, const uint32_t end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectText(start, end);
    SetKeyInputFocus(*this);
  }
}

void InputEditorImpl::SelectWholeText()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectWholeText();
    SetKeyInputFocus(*this);
  }
}

void InputEditorImpl::ClearSelection()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectNone();
  }
}

Dali::String InputEditorImpl::GetSelectedText() const
{
  Dali::String selectedText = "";
  if(mController && mController->IsShowingRealText())
  {
    selectedText = ToDaliString(mController->GetSelectedText());
  }
  return selectedText;
}

void InputEditorImpl::SetTextSelectionRange(const uint32_t* start, const uint32_t* end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SetTextSelectionRange(start, end);
    SetKeyInputFocus(*this);
  }
}

Uint32Pair InputEditorImpl::GetTextSelectionRange() const
{
  Uint32Pair range;
  if(mController && mController->IsShowingRealText())
  {
    range = mController->GetTextSelectionRange();
  }
  return range;
}

void InputEditorImpl::SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd)
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

    mSelectionChanged = true;
  }
}

// =============================================================================
// AnchorControlInterface
// =============================================================================
bool InputEditorImpl::AnchorClicked(uint32_t cursorPosition, std::string& href)
{
  return mController->AnchorClickEvent(cursorPosition, href);
}

void InputEditorImpl::EmitAnchorClicked(const std::string& href)
{
  // TODO
}

// =============================================================================
// Implementation
// =============================================================================
void InputEditorImpl::UpdateLineHeight()
{
  bool rendererUpdateNeeded = false;
  if(Equals(mLineHeight, Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
  {
    // clear explicit line height and use the natural line height.
    rendererUpdateNeeded |= mController->SetRelativeLineSize(-1.0f);
    rendererUpdateNeeded |= mController->SetDefaultLineSize(0.0f);
  }
  else if(mLineHeightMode == Text::LineHeightMode::RELATIVE)
  {
    rendererUpdateNeeded |= mController->SetDefaultLineSize(0.0f);
    rendererUpdateNeeded |= mController->SetRelativeLineSize(mLineHeight);
  }
  else // LineHeightMode::ABSOLUTE
  {
    rendererUpdateNeeded |= mController->SetRelativeLineSize(-1.0f);
    rendererUpdateNeeded |= mController->SetDefaultLineSize(mLineHeight);
  }

  if(rendererUpdateNeeded)
  {
    mController->InvalidateFontData();
  }
}

InputMethodContext::CallbackData InputEditorImpl::OnInputMethodContextEvent(Dali::InputMethodContext inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
{
  return mController->OnInputMethodContextEvent(inputMethodContext, inputMethodContextEvent);
}

void InputEditorImpl::OnSceneConnect(Dali::Actor actor)
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

bool InputEditorImpl::OnTouched(Actor actor, TouchEvent touch)
{
  return false;
}

void InputEditorImpl::OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type)
{
  mController->ChangedLayoutDirection();
}

void InputEditorImpl::OnLocaleChanged(std::string locale)
{
  mController->InvalidateFontData();
}

void InputEditorImpl::OnKeyboardStatusChanged(bool keyboardShown)
{
  DALI_LOG_RELEASE_INFO("[%p] keyboardShown:%d\n", mController.Get(), keyboardShown);

  bool isFocused = false;

  Dali::Ui::FocusManager keyboardFocusManager = Dali::Ui::FocusManager::Get();
  if(keyboardFocusManager)
  {
    isFocused = keyboardFocusManager.GetCurrentFocusView() == Self();
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
    mController->KeyboardFocusGainEvent(); // Initially called by OnFocusGained
  }
}

void InputEditorImpl::EnableClipping()
{
  if(!mStencil)
  {
    // Creates an extra actor to be used as stencil buffer.
    mStencil = Actor::New();
    mStencil.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    mStencil.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);

    // Enable the clipping property.
    mStencil.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    mStencil.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

    IntegrationView::AddActorChild(Ui::View::DownCast(Self()), mStencil);
    if(mCursorLayer)
    {
      mStencil.Add(mCursorLayer);
    }
  }
}

void InputEditorImpl::ResizeActor(Actor& actor, const Vector2& size)
{
  if(actor.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
  {
    actor.SetProperty(Actor::Property::SIZE, size);
  }
}

void InputEditorImpl::AddLayer(Actor& layer, Actor& actor)
{
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  IntegrationView::AddActorChild(Ui::View::DownCast(Self()), actor);
  layer = actor;
}

void InputEditorImpl::RenderText(Text::Controller::UpdateTextType updateTextType)
{
  Ui::Internal::CommonTextUtils::RenderText(Self(), mRenderer, mController, mDecorator, mAlignmentOffset, mRenderableActor, mBackgroundActor, mCursorLayer, mStencil, mClippingDecorationActors, mAnchorActors, updateTextType);
}

void InputEditorImpl::EmitTextChanged()
{
  Ui::View handle(GetOwner());
  mTextChangedSignal.Emit(handle);
  mTextChanged = false;
}

void InputEditorImpl::EmitMaximumLengthReached()
{
  Ui::View handle(GetOwner());
  mMaxLengthReachedSignal.Emit(handle);
}

void InputEditorImpl::EmitInputRejected(Text::InputFilter::RejectReason reason)
{
  Ui::View handle(GetOwner());
  mInputRejectedSignal.Emit(handle, reason);
}

void InputEditorImpl::EmitCursorPositionChanged()
{
  Ui::View handle(GetOwner());
  mCursorPositionChangedSignal.Emit(handle, mController->GetPrimaryCursorPosition());
  mCursorPositionChanged = false;
}

void InputEditorImpl::EmitSelectionStarted()
{
  Ui::View handle(GetOwner());
  mSelectionStartedSignal.Emit(handle);
  mSelectionStarted = false;
}

void InputEditorImpl::EmitSelectionChanged()
{
  Ui::View   handle(GetOwner());
  Uint32Pair range = mController->GetTextSelectionRange();
  mSelectionChangedSignal.Emit(handle, range.first, range.second);
  mSelectionChanged = false;
}

void InputEditorImpl::EmitSelectionCleared()
{
  Ui::View handle(GetOwner());
  mSelectionClearedSignal.Emit(handle);
  mSelectionCleared = false;
}

void InputEditorImpl::EmitTypingStyleChanged(Text::TypingStyle::Mask mask)
{
  Ui::View handle(GetOwner());
  mTypingStyleChangedSignal.Emit(handle, mask);
}

// =============================================================================
// UiColorManager
// =============================================================================
void InputEditorImpl::SetTextColorInternal(const Vector4& color)
{
  if(mController->GetDefaultColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetDefaultColor(color);
    mController->SetInputColor(color);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetPlaceholderColorInternal(const Vector4& color)
{
  if(mController->GetPlaceholderTextColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetPlaceholderTextColor(color);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetCursorColorInternal(const Vector4& color)
{
  DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
  mDecorator->SetCursorColor(Text::PRIMARY_CURSOR, color);
  mDecorator->SetCursorColor(Text::SECONDARY_CURSOR, color);
  RequestTextRelayout();
}

void InputEditorImpl::SetSelectionColorInternal(const Vector4& color)
{
  DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
  mDecorator->SetHighlightColor(color);
  RequestTextRelayout();
}

void InputEditorImpl::SetTextHandleColorInternal(const Vector4& color)
{
  DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
  mDecorator->SetHandleColor(color);
  RequestTextRelayout();
}

void InputEditorImpl::SetTextBackgroundColorInternal(const Vector4& color)
{
  if(mController->GetBackgroundColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetBackgroundColor(color);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetUnderlineColorInternal(const Vector4& color)
{
  if(mController->GetUnderlineColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetUnderlineColor(color);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetShadowColorInternal(const Vector4& color)
{
  if(mController->GetShadowColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetShadowColor(color);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetOutlineColorInternal(const Vector4& color)
{
  if(mController->GetOutlineColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetOutlineColor(color);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetLineThroughColorInternal(const Vector4& color)
{
  if(mController->GetStrikethroughColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetStrikethroughColor(color);
    mRenderer.Reset();
  }
}

void InputEditorImpl::SetTypingTextColorInternal(const Vector4& color)
{
  DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
  mController->SetInputColor(color);
}

// =============================================================================
// Properties
// =============================================================================
void InputEditorImpl::OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue)
{
  switch(index)
  {
    default:
    {
      ViewImpl::OnPropertySet(index, propertyValue); // up call to control for non-handled properties
      break;
    }
  }
}

void InputEditorImpl::SetProperty(BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    PropertyHandler::SetProperty(view, index, value);
  }
}

Dali::Property::Value InputEditorImpl::GetProperty(BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    value = PropertyHandler::GetProperty(view, index);
  }
  return value;
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
