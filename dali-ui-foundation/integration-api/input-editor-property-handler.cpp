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

void InputEditorImpl::PropertyHandler::SetProperty(Ui::View view, Property::Index index, const Property::Value& value)
{
  InputEditorImpl& impl = static_cast<InputEditorImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_RELEASE_INFO("[%p] index : %d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Text::InputEditorPropertyIndex::TEXT:
    {
      impl.SetText(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_FAMILY:
    {
      impl.SetFontFamily(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_SIZE:
    {
      impl.SetFontSize(value.Get<float>());
      break;
    }
    case Text::InputEditorPropertyIndex::TEXT_COLOR:
    {
      impl.SetTextColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputEditorPropertyIndex::LINE_WRAP_MODE:
    {
      Text::LineWrapMode mode(static_cast<Text::LineWrapMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetLineWrapModeEnumeration(value, mode))
      {
        impl.SetLineWrapMode(mode);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      Text::Alignment alignment(static_cast<Text::Alignment>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetHorizontalAlignmentEnumeration(value, alignment))
      {
        impl.SetHorizontalTextAlignment(alignment);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::VERTICAL_ALIGNMENT:
    {
      Text::Alignment alignment(static_cast<Text::Alignment>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetVerticalAlignmentEnumeration(value, alignment))
      {
        impl.SetVerticalTextAlignment(alignment);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::OVERFLOW_MODE:
    {
      Text::OverflowMode mode(static_cast<Text::OverflowMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetOverflowModeEnumeration(value, mode))
      {
        impl.SetOverflowMode(mode);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::PLACEHOLDER:
    {
      impl.SetPlaceholder(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::PLACEHOLDER_COLOR:
    {
      impl.SetPlaceholderColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputEditorPropertyIndex::SHOW_PLACEHOLDER_ON_FOCUS:
    {
      impl.SetShowPlaceholderOnFocus(value.Get<bool>());
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_WIDTH:
    {
      impl.SetCursorWidth(value.Get<int>());
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_COLOR:
    {
      impl.SetCursorColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_BLINK_ENABLED:
    {
      impl.SetCursorBlinkEnabled(value.Get<bool>());
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_BLINK_INTERVAL:
    {
      impl.SetCursorBlinkInterval(value.Get<float>());
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_POSITION:
    {
      impl.SetCursorPosition(static_cast<uint32_t>(value.Get<int>()));
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_ENABLED:
    {
      impl.SetSelectionEnabled(value.Get<bool>());
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_COLOR:
    {
      impl.SetSelectionColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputEditorPropertyIndex::TEXT_HANDLE_ENABLED:
    {
      impl.SetTextHandleEnabled(value.Get<bool>());
      break;
    }
    case Text::InputEditorPropertyIndex::TEXT_HANDLE_COLOR:
    {
      impl.SetTextHandleColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_HANDLE_IMAGE:
    {
      impl.SetCursorHandleImage(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_HANDLE_PRESSED_IMAGE:
    {
      impl.SetCursorHandlePressedImage(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_LEFT:
    {
      impl.SetSelectionHandleImageLeft(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_RIGHT:
    {
      impl.SetSelectionHandleImageRight(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
    {
      impl.SetSelectionHandlePressedImageLeft(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
    {
      impl.SetSelectionHandlePressedImageRight(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::MAXIMUM_LENGTH:
    {
      impl.SetMaximumLength(value.Get<int>());
      break;
    }
    case Text::InputEditorPropertyIndex::EDITABLE:
    {
      impl.SetEditable(value.Get<bool>());
      break;
    }
    case Text::InputEditorPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      Text::LayoutDirectionMode mode(static_cast<Text::LayoutDirectionMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetLayoutDirectionModeEnumeration(value, mode))
      {
        impl.SetLayoutDirectionMode(mode);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::MARKUP_ENABLED:
    {
      impl.SetMarkupEnabled(value.Get<bool>());
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_WEIGHT:
    {
      Text::FontWeight weight(static_cast<Text::FontWeight>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontWeightEnumeration(value, weight))
      {
        impl.SetFontWeight(weight);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_WIDTH:
    {
      Text::FontWidth width(static_cast<Text::FontWidth>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontWidthEnumeration(value, width))
      {
        impl.SetFontWidth(width);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_SLANT:
    {
      Text::FontSlant slant(static_cast<Text::FontSlant>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontSlantEnumeration(value, slant))
      {
        impl.SetFontSlant(slant);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      impl.SetTextBackgroundColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_SIZE_SCALE:
    {
      impl.SetFontSizeScale(value.Get<float>());
      break;
    }
    case Text::InputEditorPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMinimumFontSizeScale(value.Get<float>());
      break;
    }
    case Text::InputEditorPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMaximumFontSizeScale(value.Get<float>());
      break;
    }
    case Text::InputEditorPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      impl.SetSystemFontSizeScaleEnabled(value.Get<bool>());
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_TEXT_COLOR:
    {
      impl.SetTypingTextColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_FAMILY:
    {
      impl.SetTypingFontFamily(value.Get<Dali::String>());
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_SIZE:
    {
      impl.SetTypingFontSize(value.Get<float>());
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_WEIGHT:
    {
      Text::FontWeight weight(static_cast<Text::FontWeight>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontWeightEnumeration(value, weight))
      {
        impl.SetTypingFontWeight(weight);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_WIDTH:
    {
      Text::FontWidth width(static_cast<Text::FontWidth>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontWidthEnumeration(value, width))
      {
        impl.SetTypingFontWidth(width);
      }
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_SLANT:
    {
      Text::FontSlant slant(static_cast<Text::FontSlant>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontSlantEnumeration(value, slant))
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
  DALI_LOG_RELEASE_INFO("[%p] index : %d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Text::InputEditorPropertyIndex::TEXT:
    {
      value = impl.GetText();
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_FAMILY:
    {
      value = impl.GetFontFamily();
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_SIZE:
    {
      value = impl.GetFontSize();
      break;
    }
    case Text::InputEditorPropertyIndex::TEXT_COLOR:
    {
      value = impl.GetTextColor().GetRgba();
      break;
    }
    case Text::InputEditorPropertyIndex::LINE_WRAP_MODE:
    {
      value = impl.GetLineWrapMode();
      break;
    }
    case Text::InputEditorPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      value = impl.GetHorizontalTextAlignment();
      break;
    }
    case Text::InputEditorPropertyIndex::VERTICAL_ALIGNMENT:
    {
      value = impl.GetVerticalTextAlignment();
      break;
    }
    case Text::InputEditorPropertyIndex::OVERFLOW_MODE:
    {
      value = impl.GetOverflowMode();
      break;
    }
    case Text::InputEditorPropertyIndex::PLACEHOLDER:
    {
      value = impl.GetPlaceholder();
      break;
    }
    case Text::InputEditorPropertyIndex::PLACEHOLDER_COLOR:
    {
      value = impl.GetPlaceholderColor().GetRgba();
      break;
    }
    case Text::InputEditorPropertyIndex::SHOW_PLACEHOLDER_ON_FOCUS:
    {
      value = impl.IsPlaceholderShownOnFocus();
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_WIDTH:
    {
      value = impl.GetCursorWidth();
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_COLOR:
    {
      value = impl.GetCursorColor().GetRgba();
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_BLINK_ENABLED:
    {
      value = impl.IsCursorBlinkEnabled();
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_BLINK_INTERVAL:
    {
      value = impl.GetCursorBlinkInterval();
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_POSITION:
    {
      value = static_cast<int>(impl.GetCursorPosition());
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_ENABLED:
    {
      value = impl.IsSelectionEnabled();
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_COLOR:
    {
      value = impl.GetSelectionColor().GetRgba();
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTED_TEXT:
    {
      value = impl.GetSelectedText();
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTED_TEXT_START:
    {
      value = static_cast<int>(impl.GetSelectedTextStart());
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTED_TEXT_END:
    {
      value = static_cast<int>(impl.GetSelectedTextEnd());
      break;
    }
    case Text::InputEditorPropertyIndex::TEXT_HANDLE_ENABLED:
    {
      value = impl.IsTextHandleEnabled();
      break;
    }
    case Text::InputEditorPropertyIndex::TEXT_HANDLE_COLOR:
    {
      value = impl.GetTextHandleColor().GetRgba();
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_HANDLE_IMAGE:
    {
      value = impl.GetCursorHandleImage();
      break;
    }
    case Text::InputEditorPropertyIndex::CURSOR_HANDLE_PRESSED_IMAGE:
    {
      value = impl.GetCursorHandlePressedImage();
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_LEFT:
    {
      value = impl.GetSelectionHandleImageLeft();
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_RIGHT:
    {
      value = impl.GetSelectionHandleImageRight();
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
    {
      value = impl.GetSelectionHandlePressedImageLeft();
      break;
    }
    case Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
    {
      value = impl.GetSelectionHandlePressedImageRight();
      break;
    }
    case Text::InputEditorPropertyIndex::MAXIMUM_LENGTH:
    {
      value = impl.GetMaximumLength();
      break;
    }
    case Text::InputEditorPropertyIndex::EDITABLE:
    {
      value = impl.IsEditable();
      break;
    }
    case Text::InputEditorPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      value = impl.GetLayoutDirectionMode();
      break;
    }
    case Text::InputEditorPropertyIndex::MARKUP_ENABLED:
    {
      value = impl.IsMarkupEnabled();
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_WEIGHT:
    {
      value = impl.GetFontWeight();
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_WIDTH:
    {
      value = impl.GetFontWidth();
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_SLANT:
    {
      value = impl.GetFontSlant();
      break;
    }
    case Text::InputEditorPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      value = impl.GetTextBackgroundColor().GetRgba();
      break;
    }
    case Text::InputEditorPropertyIndex::FONT_SIZE_SCALE:
    {
      value = impl.GetFontSizeScale();
      break;
    }
    case Text::InputEditorPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMinimumFontSizeScale();
      break;
    }
    case Text::InputEditorPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMaximumFontSizeScale();
      break;
    }
    case Text::InputEditorPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      value = impl.IsSystemFontSizeScaleEnabled();
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_TEXT_COLOR:
    {
      value = impl.GetTypingTextColor().GetRgba();
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_FAMILY:
    {
      value = impl.GetTypingFontFamily();
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_SIZE:
    {
      value = impl.GetTypingFontSize();
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_WEIGHT:
    {
      value = impl.GetTypingFontWeight();
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_WIDTH:
    {
      value = impl.GetTypingFontWidth();
      break;
    }
    case Text::InputEditorPropertyIndex::TYPING_FONT_SLANT:
    {
      value = impl.GetTypingFontSlant();
      break;
    }
  }
  return value;
}

} // namespace Dali::Ui::Integration
