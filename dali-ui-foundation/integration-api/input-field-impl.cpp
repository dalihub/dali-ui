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
#include <dali-ui-foundation/integration-api/input-field-property-handler.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/internal/text/text-style-helper.h>

#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/integration-api/view-integration.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/internal/text/rendering/text-backend.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/view-impl.h>
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

#define INPUT_FIELD_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui::Text, InputFieldPropertyIndex, Ui::Integration, InputFieldImpl, text, valueType, enumIndex)

// clang-format off
// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(InputFieldImpl, ViewImpl, Create)

INPUT_FIELD_PROPERTY_REGISTRATION("text",                       STRING,  TEXT                          )
INPUT_FIELD_PROPERTY_REGISTRATION("fontFamily",                 STRING,  FONT_FAMILY                   )
INPUT_FIELD_PROPERTY_REGISTRATION("fontSize",                   FLOAT,   FONT_SIZE                     )
INPUT_FIELD_PROPERTY_REGISTRATION("textColor",                  VECTOR4, TEXT_COLOR                    )
INPUT_FIELD_PROPERTY_REGISTRATION("horizontalAlignment",        INTEGER, HORIZONTAL_ALIGNMENT          )
INPUT_FIELD_PROPERTY_REGISTRATION("verticalAlignment",          INTEGER, VERTICAL_ALIGNMENT            )
INPUT_FIELD_PROPERTY_REGISTRATION("overflowMode",               INTEGER, OVERFLOW_MODE                 )
INPUT_FIELD_PROPERTY_REGISTRATION("placeholder",                STRING,  PLACEHOLDER                   )
INPUT_FIELD_PROPERTY_REGISTRATION("placeholderColor",           VECTOR4, PLACEHOLDER_COLOR             )
INPUT_FIELD_PROPERTY_REGISTRATION("showPlaceholderOnFocus",     BOOLEAN, SHOW_PLACEHOLDER_ON_FOCUS     )
INPUT_FIELD_PROPERTY_REGISTRATION("cursorWidth",                INTEGER, CURSOR_WIDTH                  )
INPUT_FIELD_PROPERTY_REGISTRATION("cursorColor",                VECTOR4, CURSOR_COLOR                  )
INPUT_FIELD_PROPERTY_REGISTRATION("cursorBlinkEnabled",         BOOLEAN, CURSOR_BLINK_ENABLED          )
INPUT_FIELD_PROPERTY_REGISTRATION("cursorBlinkInterval",        FLOAT,   CURSOR_BLINK_INTERVAL         )
INPUT_FIELD_PROPERTY_REGISTRATION("cursorPosition",             INTEGER, CURSOR_POSITION               )
INPUT_FIELD_PROPERTY_REGISTRATION("selectionEnabled",           BOOLEAN, SELECTION_ENABLED             )
INPUT_FIELD_PROPERTY_REGISTRATION("selectionColor",             VECTOR4, SELECTION_COLOR               )
INPUT_FIELD_PROPERTY_REGISTRATION("selectedText",               STRING,  SELECTED_TEXT                 )
INPUT_FIELD_PROPERTY_REGISTRATION("selectedTextStart",          INTEGER, SELECTED_TEXT_START           )
INPUT_FIELD_PROPERTY_REGISTRATION("selectedTextEnd",            INTEGER, SELECTED_TEXT_END             )
INPUT_FIELD_PROPERTY_REGISTRATION("maximumLength",              INTEGER, MAXIMUM_LENGTH                )
INPUT_FIELD_PROPERTY_REGISTRATION("editable",                   BOOLEAN, EDITABLE                      )
INPUT_FIELD_PROPERTY_REGISTRATION("layoutDirectionMode",        INTEGER, LAYOUT_DIRECTION_MODE         )
INPUT_FIELD_PROPERTY_REGISTRATION("fontWeight",                 INTEGER, FONT_WEIGHT                   )
INPUT_FIELD_PROPERTY_REGISTRATION("fontWidth",                  INTEGER, FONT_WIDTH                    )
INPUT_FIELD_PROPERTY_REGISTRATION("fontSlant",                  INTEGER, FONT_SLANT                    )
INPUT_FIELD_PROPERTY_REGISTRATION("textBackgroundColor",        VECTOR4, TEXT_BACKGROUND_COLOR         )
INPUT_FIELD_PROPERTY_REGISTRATION("fontSizeScale",              FLOAT,   FONT_SIZE_SCALE               )
INPUT_FIELD_PROPERTY_REGISTRATION("minimumFontSizeScale",       FLOAT,   MINIMUM_FONT_SIZE_SCALE       )
INPUT_FIELD_PROPERTY_REGISTRATION("maximumFontSizeScale",       FLOAT,   MAXIMUM_FONT_SIZE_SCALE       )
INPUT_FIELD_PROPERTY_REGISTRATION("systemFontSizeScaleEnabled", BOOLEAN, SYSTEM_FONT_SIZE_SCALE_ENABLED)

DALI_TYPE_REGISTRATION_END()
// clang-format on

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

} // namespace

InputFieldImplPtr InputFieldImpl::New()
{
  return InputFieldImplPtr(new InputFieldImpl());
}

InputFieldImpl::InputFieldImpl()
: ViewImpl(),
  mOverflowMode(Text::OverflowMode::CLIP),
  mAlignmentOffset(0.f),
  mMeasureInvalidated(false),
  mHasBeenStaged(false),
  mTextChanged(false),
  mCursorPositionChanged(false),
  mSelectionStarted(false),
  mSelectionChanged(false),
  mSelectionCleared(false),
  mSelectionEnabled(true)
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

  if(!Equals(mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), fontSize, Math::MACHINE_EPSILON_1000))
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
  SetColorBinding("TextColor", color, this, &InputFieldImpl::SetTextColorInternal);
}

UiColor InputFieldImpl::GetTextColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "TextColor", outColor))
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

void InputFieldImpl::SetOverflowMode(Text::OverflowMode mode)
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

Text::OverflowMode InputFieldImpl::GetOverflowMode() const
{
  return mOverflowMode;
}

void InputFieldImpl::SetPlaceholder(const Dali::String& text)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), text.CStr());

  const std::string placeholder = ToStdString(text);
  mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, placeholder);
  mController->SetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_ACTIVE, placeholder);
}

Dali::String InputFieldImpl::GetPlaceholder() const
{
  std::string text;
  mController->GetPlaceholderText(Text::Controller::PLACEHOLDER_TYPE_INACTIVE, text);
  return ToDaliString(text);
}

void InputFieldImpl::SetPlaceholderColor(const UiColor& color)
{
  SetColorBinding("PlaceholderColor", color, this, &InputFieldImpl::SetPlaceholderColorInternal);
}

UiColor InputFieldImpl::GetPlaceholderColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "PlaceholderColor", outColor))
  {
    return outColor;
  }
  return mController->GetPlaceholderTextColor();
}

void InputFieldImpl::SetShowPlaceholderOnFocus(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);

  mController->SetShowPlaceholderOnFocus(enabled);
}

bool InputFieldImpl::IsPlaceholderShownOnFocus() const
{
  return mController->IsPlaceholderShownOnFocus();
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
  SetColorBinding("CursorColor", color, this, &InputFieldImpl::SetCursorColorInternal);
}

UiColor InputFieldImpl::GetCursorColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "CursorColor", outColor))
  {
    return outColor;
  }
  return mDecorator->GetColor(Text::PRIMARY_CURSOR);
}

void InputFieldImpl::SetCursorBlinkEnabled(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetEnableCursorBlink(enabled);
  RequestTextRelayout();
}

bool InputFieldImpl::IsCursorBlinkEnabled() const
{
  return mController->GetEnableCursorBlink();
}

void InputFieldImpl::SetCursorBlinkInterval(float interval)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), interval);
  mDecorator->SetCursorBlinkInterval(interval);
}

float InputFieldImpl::GetCursorBlinkInterval() const
{
  return mDecorator->GetCursorBlinkInterval();
}

void InputFieldImpl::SetCursorPosition(uint32_t position)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), position);
  if(mController->SetPrimaryCursorPosition(position, HasKeyInputFocus(*this)))
  {
    SetKeyInputFocus(*this);
  }
}

uint32_t InputFieldImpl::GetCursorPosition() const
{
  return mController->GetPrimaryCursorPosition();
}

void InputFieldImpl::SetSelectionEnabled(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetSelectionEnabled(enabled);
  mController->SetShiftSelectionEnabled(enabled);
}

bool InputFieldImpl::IsSelectionEnabled() const
{
  return mController->IsSelectionEnabled();
}

void InputFieldImpl::SetSelectionColor(const UiColor& color)
{
  SetColorBinding("SelectionColor", color, this, &InputFieldImpl::SetSelectionColorInternal);
}

UiColor InputFieldImpl::GetSelectionColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "SelectionColor", outColor))
  {
    return outColor;
  }
  return mDecorator->GetHighlightColor();
}

void InputFieldImpl::SetMaximumLength(int length)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), length);
  mController->SetMaximumNumberOfCharacters(static_cast<uint32_t>(length));
}

int InputFieldImpl::GetMaximumLength() const
{
  return static_cast<int>(mController->GetMaximumNumberOfCharacters());
}

void InputFieldImpl::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
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

void InputFieldImpl::SetFontWeight(Text::FontWeight weight)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontWeight::Name[weight]);
  mController->SetDefaultFontWeight(weight);
}

Text::FontWeight InputFieldImpl::GetFontWeight() const
{
  return mController->GetDefaultFontWeight();
}

void InputFieldImpl::SetFontWidth(Text::FontWidth width)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontWidth::Name[width]);
  mController->SetDefaultFontWidth(width);
}

Text::FontWidth InputFieldImpl::GetFontWidth() const
{
  return mController->GetDefaultFontWidth();
}

void InputFieldImpl::SetFontSlant(Text::FontSlant slant)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontSlant::Name[slant]);
  mController->SetDefaultFontSlant(slant);
}

Text::FontSlant InputFieldImpl::GetFontSlant() const
{
  return mController->GetDefaultFontSlant();
}

void InputFieldImpl::SetTextBackgroundColor(const UiColor& color)
{
  SetColorBinding("TextBackgroundColor", color, this, &InputFieldImpl::SetTextBackgroundColorInternal);
  if(!mController->IsBackgroundEnabled())
  {
    mController->SetBackgroundEnabled(true);
  }
}

UiColor InputFieldImpl::GetTextBackgroundColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "TextBackgroundColor", outColor))
  {
    return outColor;
  }
  return mController->GetBackgroundColor();
}

void InputFieldImpl::ClearTextBackgroundColor()
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

void InputFieldImpl::SetUnderline(const Text::Underline& underline)
{
  const UiColor& color = underline.GetColor();

  SetColorBinding("UnderlineColor", color, this, &InputFieldImpl::SetUnderlineColorInternal);

  if(Text::ApplyUnderlineStyle(mController, underline))
  {
    mRenderer.Reset();
  }
}

void InputFieldImpl::ClearUnderline()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "UnderlineColor");
  if(mController->IsUnderlineEnabled())
  {
    mController->SetUnderlineEnabled(false);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetShadow(const Text::Shadow& shadow)
{
  const UiColor& color = shadow.GetColor();

  SetColorBinding("ShadowColor", color, this, &InputFieldImpl::SetShadowColorInternal);

  if(Text::ApplyShadowStyle(mController, shadow))
  {
    mRenderer.Reset();
  }
}

void InputFieldImpl::ClearShadow()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "ShadowColor");
  if(Vector2::ZERO != mController->GetShadowOffset())
  {
    mController->SetShadowOffset(Vector2::ZERO);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetOutline(const Text::Outline& outline)
{
  const UiColor& color = outline.GetColor();

  SetColorBinding("OutlineColor", color, this, &InputFieldImpl::SetOutlineColorInternal);

  if(Text::ApplyOutlineStyle(mController, outline))
  {
    mRenderer.Reset();
  }
}

void InputFieldImpl::ClearOutline()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "OutlineColor");
  if(0u != mController->GetOutlineWidth())
  {
    mController->SetOutlineWidth(0u);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetLineThrough(const Text::LineThrough& lineThrough)
{
  const UiColor& color = lineThrough.GetColor();

  SetColorBinding("LineThroughColor", color, this, &InputFieldImpl::SetLineThroughColorInternal);

  if(Text::ApplyLineThroughStyle(mController, lineThrough))
  {
    mRenderer.Reset();
  }
}

void InputFieldImpl::ClearLineThrough()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "LineThroughColor");
  if(mController->IsStrikethroughEnabled())
  {
    mController->SetStrikethroughEnabled(false);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetFontSizeScale(scale);
}

float InputFieldImpl::GetFontSizeScale() const
{
  return mController->GetFontSizeScale();
}

void InputFieldImpl::SetMinimumFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetMinimumFontSizeScale(scale);
}

float InputFieldImpl::GetMinimumFontSizeScale() const
{
  return mController->GetMinimumFontSizeScale();
}

void InputFieldImpl::SetMaximumFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetMaximumFontSizeScale(scale);
}

float InputFieldImpl::GetMaximumFontSizeScale() const
{
  return mController->GetMaximumFontSizeScale();
}

void InputFieldImpl::SetSystemFontSizeScaleEnabled(bool enabled)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetSystemFontSizeScaleEnabled(enabled);
}

bool InputFieldImpl::IsSystemFontSizeScaleEnabled() const
{
  return mController->IsSystemFontSizeScaleEnabled();
}

void InputFieldImpl::SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] number of candidates:%u\n", mController.Get(), axes.Count());
  mController->SetVariations(axes);
}

void InputFieldImpl::SetFontVariation(const Dali::String& settings)
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

Dali::Vector<Text::FontVariationAxis> InputFieldImpl::GetFontVariation() const
{
  return mController->GetVariations();
}

void InputFieldImpl::ClearFontVariation()
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  mController->ClearVariationsMap();
}

// Integration-only implementation for now until public API support is introduced.
void InputFieldImpl::SetLetterSpacing(float spacing)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), spacing);
  mController->SetCharacterSpacing(spacing);
}

float InputFieldImpl::GetLetterSpacing() const
{
  return mController->GetCharacterSpacing();
}

// =============================================================================
// Read Only
// =============================================================================
float InputFieldImpl::GetAdjustedFontSizeScale() const
{
  return mController->GetAdjustedFontSizeScale();
}

uint32_t InputFieldImpl::GetSelectedTextStart() const
{
  Uint32Pair range = mController->GetTextSelectionRange();
  return range.first;
}

uint32_t InputFieldImpl::GetSelectedTextEnd() const
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
Signal<void(View)>& InputFieldImpl::TextChangedSignal()
{
  return mTextChangedSignal;
}

Signal<void(View)>& InputFieldImpl::MaximumLengthReachedSignal()
{
  return mMaxLengthReachedSignal;
}

Signal<void(View, uint32_t)>& InputFieldImpl::CursorPositionChangedSignal()
{
  return mCursorPositionChangedSignal;
}

// =============================================================================
// Config
// =============================================================================
void InputFieldImpl::ApplyInitialConfig()
{
  // UiConfigManager may not be initialized during preload phase
  if(!UiConfigManager::Get().IsInitialized())
  {
    DALI_LOG_RELEASE_INFO("ApplyInitialConfig skipped: UiConfigManager is not initialized (possible preload phase)\n");
    return;
  }

  const auto& config = UiConfigManager::Get().GetConfig();
  SetFontSize(config.GetDefaultFontSize());
  SetTextColor(config.GetDefaultTextColor());
  SetPlaceholderColor(config.GetDefaultPlaceholderTextColor());
  SetShowPlaceholderOnFocus(config.IsPlaceholderTextShownOnFocus());
}

// =============================================================================
// ViewImpl
// =============================================================================
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

  // Disable the text ellipsis.
  mController->SetTextElideEnabled(false);

  // Sets layoutDirection value
  Dali::Stage                 stage           = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(stage.GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  mController->SetLayoutDirection(layoutDirection);

  self.LayoutDirectionChangedSignal().Connect(this, &InputFieldImpl::OnLayoutDirectionChanged);

  auto viewHandle = View::DownCast(self);
  viewHandle.SetFocusable(true);
  viewHandle.SetTouchFocusable(true);

  if(Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &InputFieldImpl::OnLocaleChanged);
  }

  // Forward input events to controller
  mTapGestureDetector = TapGestureDetector::New();
  mTapGestureDetector.SetMaximumTapsRequired(2);
  mTapGestureDetector.ReceiveAllTapEvents(true);
  mTapGestureDetector.DetectedSignal().Connect(this, &InputFieldImpl::OnTapDetected);
  mTapGestureDetector.Attach(self);

  mPanGestureDetector = PanGestureDetector::New();
  mPanGestureDetector.SetMaximumTouchesRequired(2);
  mPanGestureDetector.DetectedSignal().Connect(this, &InputFieldImpl::OnPanDetected);
  mPanGestureDetector.Attach(self);

  mLongPressGestureDetector = LongPressGestureDetector::New();
  mLongPressGestureDetector.DetectedSignal().Connect(this, &InputFieldImpl::OnLongPressDetected);
  mLongPressGestureDetector.Attach(self);

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

  View      view         = Dali::Ui::View::DownCast(self);
  ViewImpl& viewInternal = Ui::GetImpl(view);
  Internal::ViewDataImpl::Get(viewInternal).SetInputMethodContext(mInputMethodContext);

  EnableClipping();

  // TODO: Re-enable when grab handle and popup support are fully implemented.
  mController->SetGrabHandleEnabled(false);
  mController->SetGrabHandlePopupEnabled(false);

  ApplyInitialConfig();
}

void InputFieldImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Actor self = Self();

  Extents padding = GetPadding();
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
  Extents padding     = GetPadding();
  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += (padding.start + padding.end);
  naturalSize.height += (padding.top + padding.bottom);

  return naturalSize;
}

float InputFieldImpl::GetHeightForWidth(float width)
{
  Extents padding = GetPadding();
  return mController->GetHeightForWidth(width - (padding.start + padding.end)) + padding.top + padding.bottom;
}

void InputFieldImpl::OnFocusChanged(bool focused)
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

void InputFieldImpl::OnFocusGained()
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
}

void InputFieldImpl::OnFocusLost()
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
      Dali::Ui::FocusManager focusManager = Dali::Ui::FocusManager::Get();
      if(focusManager)
      {
        focusManager.ClearFocus();
      }
      ClearKeyInputFocus(*this);
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

void InputFieldImpl::OnTapDetected(Actor actor, const TapGesture& gesture)
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());

  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  Extents        padding    = GetPadding();
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->TapEvent(gesture.GetNumberOfTaps(), localPoint.x - padding.start, localPoint.y - padding.top);
  mController->AnchorEvent(localPoint.x - padding.start, localPoint.y - padding.top);

  Dali::Ui::FocusManager keyboardFocusManager = Dali::Ui::FocusManager::Get();
  if(keyboardFocusManager)
  {
    keyboardFocusManager.SetCurrentFocusView(Ui::View::DownCast(Self()));
  }
  SetKeyInputFocus(*this);
}

void InputFieldImpl::OnPanDetected(Actor actor, const PanGesture& gesture)
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

void InputFieldImpl::OnLongPressDetected(Actor actor, const LongPressGesture& gesture)
{
  if(mInputMethodContext && IsEditable())
  {
    mInputMethodContext.Activate();
  }
  Extents        padding    = GetPadding();
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->LongPressEvent(gesture.GetState(), localPoint.x - padding.start, localPoint.y - padding.top);

  SetKeyInputFocus(*this);
}

MeasuredSize InputFieldImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("[%p] widthConstraint:%f, heightConstraint:%f\n", mController.Get(), widthConstraint, heightConstraint);

  mMeasureInvalidated = false;

  const float requestedWidth  = GetRequestedWidth();
  const float requestedHeight = GetRequestedHeight();

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
    Extents padding = GetPadding();
    naturalHeight   = mController->GetDefaultFontLineHeight() + (padding.top + padding.bottom);
  }

  float measuredWidth  = 0.0f;
  float measuredHeight = 0.0f;

  // Width
  if(requestedWidth >= 0.0f)
  {
    measuredWidth = std::max(std::min(requestedWidth, maxWidth), minWidth);
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    // MATCH_PARENT: report minimum desired size; actual size is determined
    // by the parent during the Arrange phase.
    measuredWidth = minWidth;
  }
  else // WRAP_CONTENT
  {
    const float allowedMaxWidth = (widthConstraint >= 0.0f) ? std::min(maxWidth, widthConstraint) : maxWidth;
    measuredWidth               = std::max(std::min(naturalWidth, allowedMaxWidth), minWidth);
  }

  // Height
  if(requestedHeight >= 0.0f)
  {
    measuredHeight = std::max(std::min(requestedHeight, maxHeight), minHeight);
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    measuredHeight = minHeight;
  }
  else // WRAP_CONTENT
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

void InputFieldImpl::InvalidateTextMeasure()
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

void InputFieldImpl::RequestAsyncRender()
{
}

// =============================================================================
// EditableControlInterface
// =============================================================================
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

bool InputFieldImpl::IsEditable() const
{
  return mController->IsEditable();
}

void InputFieldImpl::SetEditable(bool editable)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), editable);
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
    SetKeyInputFocus(*this); //Giving focus to the field that was passed to the PasteText in case the passed field (current field) doesn't have focus.
    mController->PasteText();
  }
}

void InputFieldImpl::TextChanged(bool immediate)
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

void InputFieldImpl::MaxLengthReached()
{
  Ui::View handle(GetOwner());
  mMaxLengthReachedSignal.Emit(handle);
}

void InputFieldImpl::CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition)
{
  if((oldPosition != newPosition) && !mCursorPositionChanged)
  {
    mCursorPositionChanged = true;
  }
}

void InputFieldImpl::InputStyleChanged(Text::InputStyle::Mask inputStyleMask)
{
  // TODO
}

void InputFieldImpl::InputFiltered(Ui::InputFilter::Property::Type type)
{
  // TODO
}

void InputFieldImpl::TextInserted(unsigned int position, unsigned int length, const std::string& content)
{
  // TODO: Accessible
}

void InputFieldImpl::TextDeleted(unsigned int position, unsigned int length, const std::string& content)
{
  // TODO: Accessible
}

// =============================================================================
// SelectableControlInterface
// =============================================================================
void InputFieldImpl::SelectText(const uint32_t start, const uint32_t end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectText(start, end);
    SetKeyInputFocus(*this);
  }
}

void InputFieldImpl::SelectWholeText()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectWholeText();
    SetKeyInputFocus(*this);
  }
}

void InputFieldImpl::ClearSelection()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectNone();
  }
}

Dali::String InputFieldImpl::GetSelectedText() const
{
  Dali::String selectedText = "";
  if(mController && mController->IsShowingRealText())
  {
    selectedText = ToDaliString(mController->GetSelectedText());
  }
  return selectedText;
}

void InputFieldImpl::SetTextSelectionRange(const uint32_t* start, const uint32_t* end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SetTextSelectionRange(start, end);
    SetKeyInputFocus(*this);
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

    mSelectionChanged = true;
  }
}

// =============================================================================
// AnchorControlInterface
// =============================================================================
bool InputFieldImpl::AnchorClicked(uint32_t cursorPosition, std::string& href)
{
  return mController->AnchorClickEvent(cursorPosition, href);
}

void InputFieldImpl::EmitAnchorClicked(const std::string& href)
{
  // TODO
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

void InputFieldImpl::EnableClipping()
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
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  IntegrationView::AddActorChild(Ui::View::DownCast(Self()), actor);
  layer = actor;
}

void InputFieldImpl::RenderText(Text::Controller::UpdateTextType updateTextType)
{
  Ui::Internal::CommonTextUtils::RenderText(Self(), mRenderer, mController, mDecorator, mAlignmentOffset, mRenderableActor, mBackgroundActor, mCursorLayer, mStencil, mClippingDecorationActors, mAnchorActors, updateTextType);
}

void InputFieldImpl::EmitTextChanged()
{
  Ui::View handle(GetOwner());
  mTextChangedSignal.Emit(handle);
  mTextChanged = false;
}

void InputFieldImpl::EmitCursorPositionChanged()
{
  Ui::View handle(GetOwner());
  mCursorPositionChangedSignal.Emit(handle, mController->GetPrimaryCursorPosition());
  mCursorPositionChanged = false;
}

// =============================================================================
// UiColorManager
// =============================================================================
void InputFieldImpl::SetTextColorInternal(const Vector4& color)
{
  if(mController->GetDefaultColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetDefaultColor(color);
    mController->SetInputColor(color);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetPlaceholderColorInternal(const Vector4& color)
{
  if(mController->GetPlaceholderTextColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetPlaceholderTextColor(color);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetCursorColorInternal(const Vector4& color)
{
  DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
  mDecorator->SetCursorColor(Text::PRIMARY_CURSOR, color);
  mDecorator->SetCursorColor(Text::SECONDARY_CURSOR, color);
  RequestTextRelayout();
}

void InputFieldImpl::SetSelectionColorInternal(const Vector4& color)
{
  DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
  mDecorator->SetHighlightColor(color);
  RequestTextRelayout();
}

void InputFieldImpl::SetTextBackgroundColorInternal(const Vector4& color)
{
  if(mController->GetBackgroundColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetBackgroundColor(color);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetUnderlineColorInternal(const Vector4& color)
{
  if(mController->GetUnderlineColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetUnderlineColor(color);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetShadowColorInternal(const Vector4& color)
{
  if(mController->GetShadowColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetShadowColor(color);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetOutlineColorInternal(const Vector4& color)
{
  if(mController->GetOutlineColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetOutlineColor(color);
    mRenderer.Reset();
  }
}

void InputFieldImpl::SetLineThroughColorInternal(const Vector4& color)
{
  if(mController->GetStrikethroughColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetStrikethroughColor(color);
    mRenderer.Reset();
  }
}

// =============================================================================
// Properties
// =============================================================================
void InputFieldImpl::OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue)
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

void InputFieldImpl::SetProperty(BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    PropertyHandler::SetProperty(view, index, value);
  }
}

Dali::Property::Value InputFieldImpl::GetProperty(BaseObject* object, Dali::Property::Index index)
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
