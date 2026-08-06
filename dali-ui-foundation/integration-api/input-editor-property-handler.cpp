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
 */

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/integration-api/input-editor-property-handler.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>

namespace Dali::Ui::Integration
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif
} // unnamed namespace

void InputEditorImpl::PropertyHandler::SetProperty(Ui::View view, Property::Index index, const Property::Value& value)
{
  InputEditorImpl& impl = static_cast<InputEditorImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Set property index:%d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Ui::Text::InputEditorPropertyIndex::TEXT:
    {
      impl.SetText(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_FAMILY:
    {
      impl.SetFontFamily(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_SIZE:
    {
      impl.SetFontSize(value.Get<float>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TEXT_COLOR:
    {
      impl.SetTextColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::LINE_WRAP_MODE:
    {
      Ui::Text::LineWrapMode mode(static_cast<Ui::Text::LineWrapMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetLineWrapModeEnumeration(value, mode))
      {
        impl.SetLineWrapMode(mode);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      Ui::Text::Alignment alignment(static_cast<Ui::Text::Alignment>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetHorizontalAlignmentEnumeration(value, alignment))
      {
        impl.SetHorizontalTextAlignment(alignment);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::VERTICAL_ALIGNMENT:
    {
      Ui::Text::Alignment alignment(static_cast<Ui::Text::Alignment>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetVerticalAlignmentEnumeration(value, alignment))
      {
        impl.SetVerticalTextAlignment(alignment);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::OVERFLOW_MODE:
    {
      Ui::Text::OverflowMode mode(static_cast<Ui::Text::OverflowMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetOverflowModeEnumeration(value, mode))
      {
        impl.SetTextOverflowMode(mode);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::LINE_HEIGHT:
    {
      impl.SetLineHeight(value.Get<float>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::LINE_HEIGHT_MODE:
    {
      Ui::Text::LineHeightMode mode(static_cast<Ui::Text::LineHeightMode>(-1));
      if(Ui::Text::GetLineHeightModeEnumeration(value, mode))
      {
        impl.SetLineHeightMode(mode);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::PLACEHOLDER:
    {
      impl.SetPlaceholder(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::PLACEHOLDER_COLOR:
    {
      impl.SetPlaceholderColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SHOW_PLACEHOLDER_ON_FOCUS:
    {
      impl.SetShowPlaceholderOnFocus(value.Get<bool>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_WIDTH:
    {
      impl.SetCursorWidth(value.Get<int>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_COLOR:
    {
      impl.SetCursorColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_BLINK_ENABLED:
    {
      impl.SetCursorBlinkEnabled(value.Get<bool>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_BLINK_INTERVAL:
    {
      impl.SetCursorBlinkInterval(value.Get<float>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_POSITION:
    {
      impl.SetCursorPosition(static_cast<uint32_t>(value.Get<int>()));
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_ENABLED:
    {
      impl.SetSelectionEnabled(value.Get<bool>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_COLOR:
    {
      impl.SetSelectionColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TEXT_HANDLE_ENABLED:
    {
      impl.SetTextHandleEnabled(value.Get<bool>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TEXT_HANDLE_COLOR:
    {
      impl.SetTextHandleColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_HANDLE_IMAGE:
    {
      impl.SetCursorHandleImage(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_HANDLE_PRESSED_IMAGE:
    {
      impl.SetCursorHandlePressedImage(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_LEFT:
    {
      impl.SetSelectionHandleImageLeft(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_RIGHT:
    {
      impl.SetSelectionHandleImageRight(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
    {
      impl.SetSelectionHandlePressedImageLeft(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
    {
      impl.SetSelectionHandlePressedImageRight(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::MAXIMUM_LENGTH:
    {
      impl.SetMaximumLength(value.Get<int>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::EDITABLE:
    {
      impl.SetEditable(value.Get<bool>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      Ui::Text::LayoutDirectionMode mode(static_cast<Ui::Text::LayoutDirectionMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetLayoutDirectionModeEnumeration(value, mode))
      {
        impl.SetLayoutDirectionMode(mode);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_WEIGHT:
    {
      Ui::Text::FontWeight weight(static_cast<Ui::Text::FontWeight>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontWeightEnumeration(value, weight))
      {
        impl.SetFontWeight(weight);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_WIDTH:
    {
      Ui::Text::FontWidth width(static_cast<Ui::Text::FontWidth>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontWidthEnumeration(value, width))
      {
        impl.SetFontWidth(width);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_SLANT:
    {
      Ui::Text::FontSlant slant(static_cast<Ui::Text::FontSlant>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontSlantEnumeration(value, slant))
      {
        impl.SetFontSlant(slant);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      impl.SetTextBackgroundColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMinimumFontSizeScale(value.Get<float>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMaximumFontSizeScale(value.Get<float>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      impl.SetSystemFontSizeScaleEnabled(value.Get<bool>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::AUTO_GROW_ENABLED:
    {
      impl.SetAutoGrowEnabled(value.Get<bool>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_TEXT_COLOR:
    {
      impl.SetTypingTextColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_FAMILY:
    {
      impl.SetTypingFontFamily(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_SIZE:
    {
      impl.SetTypingFontSize(value.Get<float>());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_WEIGHT:
    {
      Ui::Text::FontWeight weight(static_cast<Ui::Text::FontWeight>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontWeightEnumeration(value, weight))
      {
        impl.SetTypingFontWeight(weight);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_WIDTH:
    {
      Ui::Text::FontWidth width(static_cast<Ui::Text::FontWidth>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontWidthEnumeration(value, width))
      {
        impl.SetTypingFontWidth(width);
      }
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_SLANT:
    {
      Ui::Text::FontSlant slant(static_cast<Ui::Text::FontSlant>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontSlantEnumeration(value, slant))
      {
        impl.SetTypingFontSlant(slant);
      }
      break;
    }
  }
}

Property::Value InputEditorImpl::PropertyHandler::GetProperty(Ui::View view, Property::Index index)
{
  Property::Value  value;
  InputEditorImpl& impl = static_cast<InputEditorImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Get property index:%d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Ui::Text::InputEditorPropertyIndex::TEXT:
    {
      value = impl.GetText();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_FAMILY:
    {
      value = impl.GetFontFamily();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_SIZE:
    {
      value = impl.GetFontSize();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TEXT_COLOR:
    {
      value = impl.GetTextColor().GetRgba();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::LINE_WRAP_MODE:
    {
      value = impl.GetLineWrapMode();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      value = impl.GetHorizontalTextAlignment();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::VERTICAL_ALIGNMENT:
    {
      value = impl.GetVerticalTextAlignment();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::OVERFLOW_MODE:
    {
      value = impl.GetTextOverflowMode();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::LINE_HEIGHT:
    {
      value = impl.GetLineHeight();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::LINE_HEIGHT_MODE:
    {
      value = impl.GetLineHeightMode();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::PLACEHOLDER:
    {
      value = impl.GetPlaceholder();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::PLACEHOLDER_COLOR:
    {
      value = impl.GetPlaceholderColor().GetRgba();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SHOW_PLACEHOLDER_ON_FOCUS:
    {
      value = impl.IsPlaceholderShownOnFocus();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_WIDTH:
    {
      value = impl.GetCursorWidth();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_COLOR:
    {
      value = impl.GetCursorColor().GetRgba();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_BLINK_ENABLED:
    {
      value = impl.IsCursorBlinkEnabled();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_BLINK_INTERVAL:
    {
      value = impl.GetCursorBlinkInterval();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_POSITION:
    {
      value = static_cast<int>(impl.GetCursorPosition());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_ENABLED:
    {
      value = impl.IsSelectionEnabled();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_COLOR:
    {
      value = impl.GetSelectionColor().GetRgba();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTED_TEXT:
    {
      value = impl.GetSelectedText();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTED_TEXT_START:
    {
      value = static_cast<int>(impl.GetSelectedTextStart());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTED_TEXT_END:
    {
      value = static_cast<int>(impl.GetSelectedTextEnd());
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TEXT_HANDLE_ENABLED:
    {
      value = impl.IsTextHandleEnabled();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TEXT_HANDLE_COLOR:
    {
      value = impl.GetTextHandleColor().GetRgba();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_HANDLE_IMAGE:
    {
      value = impl.GetCursorHandleImage();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::CURSOR_HANDLE_PRESSED_IMAGE:
    {
      value = impl.GetCursorHandlePressedImage();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_LEFT:
    {
      value = impl.GetSelectionHandleImageLeft();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_RIGHT:
    {
      value = impl.GetSelectionHandleImageRight();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
    {
      value = impl.GetSelectionHandlePressedImageLeft();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
    {
      value = impl.GetSelectionHandlePressedImageRight();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::MAXIMUM_LENGTH:
    {
      value = impl.GetMaximumLength();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::EDITABLE:
    {
      value = impl.IsEditable();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      value = impl.GetLayoutDirectionMode();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_WEIGHT:
    {
      value = impl.GetFontWeight();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_WIDTH:
    {
      value = impl.GetFontWidth();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::FONT_SLANT:
    {
      value = impl.GetFontSlant();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      value = impl.GetTextBackgroundColor().GetRgba();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMinimumFontSizeScale();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMaximumFontSizeScale();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      value = impl.IsSystemFontSizeScaleEnabled();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::AUTO_GROW_ENABLED:
    {
      value = impl.IsAutoGrowEnabled();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_TEXT_COLOR:
    {
      value = impl.GetTypingTextColor().GetRgba();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_FAMILY:
    {
      value = impl.GetTypingFontFamily();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_SIZE:
    {
      value = impl.GetTypingFontSize();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_WEIGHT:
    {
      value = impl.GetTypingFontWeight();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_WIDTH:
    {
      value = impl.GetTypingFontWidth();
      break;
    }
    case Ui::Text::InputEditorPropertyIndex::TYPING_FONT_SLANT:
    {
      value = impl.GetTypingFontSlant();
      break;
    }
  }
  return value;
}

} // namespace Dali::Ui::Integration
