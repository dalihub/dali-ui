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
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/integration-api/input-field-property-handler.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>

namespace Dali::Ui::Integration
{

void InputFieldImpl::PropertyHandler::SetProperty(Ui::View view, Property::Index index, const Property::Value& value)
{
  InputFieldImpl& impl = static_cast<InputFieldImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_RELEASE_INFO("[%p] index : %d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Text::InputFieldPropertyIndex::TEXT:
    {
      impl.SetText(value.Get<Dali::String>());
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_FAMILY:
    {
      impl.SetFontFamily(value.Get<Dali::String>());
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_SIZE:
    {
      impl.SetFontSize(value.Get<float>());
      break;
    }
    case Text::InputFieldPropertyIndex::TEXT_COLOR:
    {
      impl.SetTextColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputFieldPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      Text::Alignment alignment;
      if(Text::GetHorizontalAlignmentEnumeration(value, alignment))
      {
        impl.SetHorizontalTextAlignment(alignment);
      }
      break;
    }
    case Text::InputFieldPropertyIndex::VERTICAL_ALIGNMENT:
    {
      Text::Alignment alignment;
      if(Text::GetVerticalAlignmentEnumeration(value, alignment))
      {
        impl.SetVerticalTextAlignment(alignment);
      }
      break;
    }
    case Text::InputFieldPropertyIndex::OVERFLOW_MODE:
    {
      Text::OverflowMode mode;
      if(Text::GetOverflowModeEnumeration(value, mode))
      {
        impl.SetOverflowMode(mode);
      }
      break;
    }
    case Text::InputFieldPropertyIndex::PLACEHOLDER:
    {
      impl.SetPlaceholder(value.Get<Dali::String>());
      break;
    }
    case Text::InputFieldPropertyIndex::PLACEHOLDER_COLOR:
    {
      impl.SetPlaceholderColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputFieldPropertyIndex::SHOW_PLACEHOLDER_ON_FOCUS:
    {
      impl.SetShowPlaceholderOnFocus(value.Get<bool>());
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_WIDTH:
    {
      impl.SetCursorWidth(value.Get<int>());
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_COLOR:
    {
      impl.SetCursorColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_BLINK_ENABLED:
    {
      impl.SetCursorBlinkEnabled(value.Get<bool>());
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_BLINK_INTERVAL:
    {
      impl.SetCursorBlinkInterval(value.Get<float>());
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_POSITION:
    {
      impl.SetCursorPosition(static_cast<uint32_t>(value.Get<int>()));
      break;
    }
    case Text::InputFieldPropertyIndex::SELECTION_ENABLED:
    {
      impl.SetSelectionEnabled(value.Get<bool>());
      break;
    }
    case Text::InputFieldPropertyIndex::SELECTION_COLOR:
    {
      impl.SetSelectionColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputFieldPropertyIndex::MAXIMUM_LENGTH:
    {
      impl.SetMaximumLength(value.Get<int>());
      break;
    }
    case Text::InputFieldPropertyIndex::EDITABLE:
    {
      impl.SetEditable(value.Get<bool>());
      break;
    }
    case Text::InputFieldPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      Text::LayoutDirectionMode mode;
      if(Text::GetLayoutDirectionModeEnumeration(value, mode))
      {
        impl.SetLayoutDirectionMode(mode);
      }
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_WEIGHT:
    {
      Text::FontWeight weight;
      if(Text::GetFontWeightEnumeration(value, weight))
      {
        impl.SetFontWeight(weight);
      }
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_WIDTH:
    {
      Text::FontWidth width;
      if(Text::GetFontWidthEnumeration(value, width))
      {
        impl.SetFontWidth(width);
      }
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_SLANT:
    {
      Text::FontSlant slant;
      if(Text::GetFontSlantEnumeration(value, slant))
      {
        impl.SetFontSlant(slant);
      }
      break;
    }
    case Text::InputFieldPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      impl.SetTextBackgroundColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_SIZE_SCALE:
    {
      impl.SetFontSizeScale(value.Get<float>());
      break;
    }
    case Text::InputFieldPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMinimumFontSizeScale(value.Get<float>());
      break;
    }
    case Text::InputFieldPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMaximumFontSizeScale(value.Get<float>());
      break;
    }
    case Text::InputFieldPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      impl.SetSystemFontSizeScaleEnabled(value.Get<bool>());
      break;
    }
  }
}

Property::Value InputFieldImpl::PropertyHandler::GetProperty(Ui::View view, Property::Index index)
{
  Property::Value value;
  InputFieldImpl& impl = static_cast<InputFieldImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_RELEASE_INFO("[%p] index : %d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Text::InputFieldPropertyIndex::TEXT:
    {
      value = impl.GetText();
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_FAMILY:
    {
      value = impl.GetFontFamily();
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_SIZE:
    {
      value = impl.GetFontSize();
      break;
    }
    case Text::InputFieldPropertyIndex::TEXT_COLOR:
    {
      value = impl.GetTextColor().Resolve();
      break;
    }
    case Text::InputFieldPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      value = impl.GetHorizontalTextAlignment();
      break;
    }
    case Text::InputFieldPropertyIndex::VERTICAL_ALIGNMENT:
    {
      value = impl.GetVerticalTextAlignment();
      break;
    }
    case Text::InputFieldPropertyIndex::OVERFLOW_MODE:
    {
      value = impl.GetOverflowMode();
      break;
    }
    case Text::InputFieldPropertyIndex::PLACEHOLDER:
    {
      value = impl.GetPlaceholder();
      break;
    }
    case Text::InputFieldPropertyIndex::PLACEHOLDER_COLOR:
    {
      value = impl.GetPlaceholderColor().Resolve();
      break;
    }
    case Text::InputFieldPropertyIndex::SHOW_PLACEHOLDER_ON_FOCUS:
    {
      value = impl.IsPlaceholderShownOnFocus();
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_WIDTH:
    {
      value = impl.GetCursorWidth();
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_COLOR:
    {
      value = impl.GetCursorColor().Resolve();
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_BLINK_ENABLED:
    {
      value = impl.IsCursorBlinkEnabled();
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_BLINK_INTERVAL:
    {
      value = impl.GetCursorBlinkInterval();
      break;
    }
    case Text::InputFieldPropertyIndex::CURSOR_POSITION:
    {
      value = static_cast<int>(impl.GetCursorPosition());
      break;
    }
    case Text::InputFieldPropertyIndex::SELECTION_ENABLED:
    {
      value = impl.IsSelectionEnabled();
      break;
    }
    case Text::InputFieldPropertyIndex::SELECTION_COLOR:
    {
      value = impl.GetSelectionColor().Resolve();
      break;
    }
    case Text::InputFieldPropertyIndex::SELECTED_TEXT:
    {
      value = impl.GetSelectedText();
      break;
    }
    case Text::InputFieldPropertyIndex::SELECTED_TEXT_START:
    {
      value = static_cast<int>(impl.GetSelectedTextStart());
      break;
    }
    case Text::InputFieldPropertyIndex::SELECTED_TEXT_END:
    {
      value = static_cast<int>(impl.GetSelectedTextEnd());
      break;
    }
    case Text::InputFieldPropertyIndex::MAXIMUM_LENGTH:
    {
      value = impl.GetMaximumLength();
      break;
    }
    case Text::InputFieldPropertyIndex::EDITABLE:
    {
      value = impl.IsEditable();
      break;
    }
    case Text::InputFieldPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      value = impl.GetLayoutDirectionMode();
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_WEIGHT:
    {
      value = impl.GetFontWeight();
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_WIDTH:
    {
      value = impl.GetFontWidth();
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_SLANT:
    {
      value = impl.GetFontSlant();
      break;
    }
    case Text::InputFieldPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      value = impl.GetTextBackgroundColor().Resolve();
      break;
    }
    case Text::InputFieldPropertyIndex::FONT_SIZE_SCALE:
    {
      value = impl.GetFontSizeScale();
      break;
    }
    case Text::InputFieldPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMinimumFontSizeScale();
      break;
    }
    case Text::InputFieldPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMaximumFontSizeScale();
      break;
    }
    case Text::InputFieldPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      value = impl.IsSystemFontSizeScaleEnabled();
      break;
    }
  }
  return value;
}

} // namespace Dali::Ui::Integration
