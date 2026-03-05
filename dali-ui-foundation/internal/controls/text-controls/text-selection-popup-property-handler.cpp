/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali-ui-foundation/internal/controls/text-controls/text-selection-popup-property-handler.h>
#include <dali/integration-api/debug.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gLogFilter;
#endif

namespace Dali::UI::Internal
{
void TextSelectionPopup::PropertyHandler::SetProperty(UI::TextSelectionPopup selectionPopup, Property::Index index,
                                                      const Property::Value& value)
{
  TextSelectionPopup& impl(GetImpl(selectionPopup));

  switch (index)
  {
    case UI::TextSelectionPopup::Property::POPUP_MAX_SIZE:
    {
      impl.SetDimensionToCustomise(POPUP_MAXIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case UI::TextSelectionPopup::Property::OPTION_MAX_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_MAXIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case UI::TextSelectionPopup::Property::OPTION_MIN_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_MINIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case UI::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_DIVIDER_SIZE, value.Get<Vector2>());
      break;
    }
    case UI::TextSelectionPopup::Property::OPTION_DIVIDER_PADDING:
    {
      Vector4 padding(value.Get<Vector4>());
      impl.SetOptionDividerPadding(Padding(padding.x, padding.y, padding.z, padding.w));
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(UI::TextSelectionPopup::CLIPBOARD, value.Get<std::string>());
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(UI::TextSelectionPopup::CUT, value.Get<std::string>());
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(UI::TextSelectionPopup::COPY, value.Get<std::string>());
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(UI::TextSelectionPopup::PASTE, value.Get<std::string>());
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(UI::TextSelectionPopup::SELECT, value.Get<std::string>());
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(UI::TextSelectionPopup::SELECT_ALL, value.Get<std::string>());
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_DIVIDER_COLOR:
    {
      impl.mDividerColor = value.Get<Vector4>();
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_ICON_COLOR:
    {
      impl.mIconColor = value.Get<Vector4>();
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_PRESSED_COLOR:
    {
      impl.mPressedColor = value.Get<Vector4>();
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_PRESSED_CORNER_RADIUS:
    {
      impl.mPressedCornerRadius = value.Get<float>();
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_PRESSED_IMAGE:
    {
      impl.SetPressedImage(value.Get<std::string>());
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION:
    {
      impl.mFadeInDuration = value.Get<float>();
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION:
    {
      impl.mFadeOutDuration = value.Get<float>();
      break;
    }
    case UI::TextSelectionPopup::Property::BACKGROUND_BORDER:
    {
      Property::Map map = value.Get<Property::Map>();
      impl.CreateBackgroundBorder(map);
      break;
    }
    case UI::TextSelectionPopup::Property::BACKGROUND:
    {
      Property::Map map = value.Get<Property::Map>();
      impl.CreateBackground(map);
      break;
    }
    case UI::TextSelectionPopup::Property::LABEL_MINIMUM_SIZE:
    {
      impl.mLabelMinimumSize = value.Get<Vector2>();
      break;
    }
    case UI::TextSelectionPopup::Property::LABEL_PADDING:
    {
      Vector4 padding(value.Get<Vector4>());
      impl.SetLabelPadding(Padding(padding.x, padding.y, padding.z, padding.w));
      break;
    }
    case UI::TextSelectionPopup::Property::LABEL_TEXT_VISUAL:
    {
      impl.mLabelTextVisual = value.Get<Property::Map>();
      break;
    }
    case UI::TextSelectionPopup::Property::ENABLE_SCROLL_BAR:
    {
      impl.mEnableScrollBar = value.Get<bool>();
      break;
    }
  }
}

Property::Value TextSelectionPopup::PropertyHandler::GetProperty(UI::TextSelectionPopup selectionPopup,
                                                                 Property::Index index)
{
  Property::Value value;
  TextSelectionPopup& impl(GetImpl(selectionPopup));

  switch (index)
  {
    case UI::TextSelectionPopup::Property::POPUP_MAX_SIZE:
    {
      value = impl.GetDimensionToCustomise(POPUP_MAXIMUM_SIZE);
      break;
    }
    case UI::TextSelectionPopup::Property::OPTION_MAX_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_MAXIMUM_SIZE);
      break;
    }
    case UI::TextSelectionPopup::Property::OPTION_MIN_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_MINIMUM_SIZE);
      break;
    }
    case UI::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_DIVIDER_SIZE);
      break;
    }
    case UI::TextSelectionPopup::Property::OPTION_DIVIDER_PADDING:
    {
      Padding padding = impl.GetOptionDividerPadding();
      value = Vector4(padding.left, padding.right, padding.top, padding.bottom);
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(UI::TextSelectionPopup::CLIPBOARD);
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(UI::TextSelectionPopup::CUT);
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(UI::TextSelectionPopup::COPY);
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(UI::TextSelectionPopup::PASTE);
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(UI::TextSelectionPopup::SELECT);
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(UI::TextSelectionPopup::SELECT_ALL);
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_DIVIDER_COLOR:
    {
      value = impl.mDividerColor;
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_ICON_COLOR:
    {
      value = impl.mIconColor;
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_PRESSED_COLOR:
    {
      value = impl.mPressedColor;
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_PRESSED_CORNER_RADIUS:
    {
      value = impl.mPressedCornerRadius;
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_PRESSED_IMAGE:
    {
      value = impl.GetPressedImage();
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION:
    {
      value = impl.mFadeInDuration;
      break;
    }
    case UI::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION:
    {
      value = impl.mFadeOutDuration;
      break;
    }
    case UI::TextSelectionPopup::Property::BACKGROUND_BORDER:
    {
      Property::Map map;
      UI::Visual::Base visual = DevelControl::GetVisual(impl, UI::TextSelectionPopup::Property::BACKGROUND_BORDER);
      if (visual)
      {
        visual.CreatePropertyMap(map);
      }
      value = map;
      break;
    }
    case UI::TextSelectionPopup::Property::BACKGROUND:
    {
      Property::Map map;
      UI::Visual::Base visual = DevelControl::GetVisual(impl, UI::Control::Property::BACKGROUND);
      if (visual)
      {
        visual.CreatePropertyMap(map);
      }
      value = map;
      break;
    }
    case UI::TextSelectionPopup::Property::LABEL_MINIMUM_SIZE:
    {
      value = impl.mLabelMinimumSize;
      break;
    }
    case UI::TextSelectionPopup::Property::LABEL_PADDING:
    {
      Padding padding = impl.GetLabelPadding();
      value = Vector4(padding.left, padding.right, padding.top, padding.bottom);
      break;
    }
    case UI::TextSelectionPopup::Property::LABEL_TEXT_VISUAL:
    {
      value = impl.mLabelTextVisual;
      break;
    }
    case UI::TextSelectionPopup::Property::ENABLE_SCROLL_BAR:
    {
      value = impl.mEnableScrollBar;
      break;
    }
  }

  return value;
}

} // namespace Dali::UI::Internal
