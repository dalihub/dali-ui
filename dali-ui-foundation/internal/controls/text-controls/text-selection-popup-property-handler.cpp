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

namespace Dali::Ui::Internal
{
void TextSelectionPopup::PropertyHandler::SetProperty(Ui::TextSelectionPopup selectionPopup, Property::Index index,
                                                      const Property::Value& value)
{
  TextSelectionPopup& impl(GetImpl(selectionPopup));

  switch (index)
  {
    case Ui::TextSelectionPopup::Property::POPUP_MAX_SIZE:
    {
      impl.SetDimensionToCustomise(POPUP_MAXIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case Ui::TextSelectionPopup::Property::OPTION_MAX_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_MAXIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case Ui::TextSelectionPopup::Property::OPTION_MIN_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_MINIMUM_SIZE, value.Get<Vector2>());
      break;
    }
    case Ui::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE:
    {
      impl.SetDimensionToCustomise(OPTION_DIVIDER_SIZE, value.Get<Vector2>());
      break;
    }
    case Ui::TextSelectionPopup::Property::OPTION_DIVIDER_PADDING:
    {
      Vector4 padding(value.Get<Vector4>());
      impl.SetOptionDividerPadding(Padding(padding.x, padding.y, padding.z, padding.w));
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Ui::TextSelectionPopup::CLIPBOARD, value.Get<std::string>());
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Ui::TextSelectionPopup::CUT, value.Get<std::string>());
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Ui::TextSelectionPopup::COPY, value.Get<std::string>());
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Ui::TextSelectionPopup::PASTE, value.Get<std::string>());
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Ui::TextSelectionPopup::SELECT, value.Get<std::string>());
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
    {
      impl.SetButtonImage(Ui::TextSelectionPopup::SELECT_ALL, value.Get<std::string>());
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_DIVIDER_COLOR:
    {
      impl.mDividerColor = value.Get<Vector4>();
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_ICON_COLOR:
    {
      impl.mIconColor = value.Get<Vector4>();
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_PRESSED_COLOR:
    {
      impl.mPressedColor = value.Get<Vector4>();
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_PRESSED_CORNER_RADIUS:
    {
      impl.mPressedCornerRadius = value.Get<float>();
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_PRESSED_IMAGE:
    {
      impl.SetPressedImage(value.Get<std::string>());
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION:
    {
      impl.mFadeInDuration = value.Get<float>();
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION:
    {
      impl.mFadeOutDuration = value.Get<float>();
      break;
    }
    case Ui::TextSelectionPopup::Property::BACKGROUND_BORDER:
    {
      Property::Map map = value.Get<Property::Map>();
      impl.CreateBackgroundBorder(map);
      break;
    }
    case Ui::TextSelectionPopup::Property::BACKGROUND:
    {
      Property::Map map = value.Get<Property::Map>();
      impl.CreateBackground(map);
      break;
    }
    case Ui::TextSelectionPopup::Property::LABEL_MINIMUM_SIZE:
    {
      impl.mLabelMinimumSize = value.Get<Vector2>();
      break;
    }
    case Ui::TextSelectionPopup::Property::LABEL_PADDING:
    {
      Vector4 padding(value.Get<Vector4>());
      impl.SetLabelPadding(Padding(padding.x, padding.y, padding.z, padding.w));
      break;
    }
    case Ui::TextSelectionPopup::Property::LABEL_TEXT_VISUAL:
    {
      impl.mLabelTextVisual = value.Get<Property::Map>();
      break;
    }
    case Ui::TextSelectionPopup::Property::ENABLE_SCROLL_BAR:
    {
      impl.mEnableScrollBar = value.Get<bool>();
      break;
    }
  }
}

Property::Value TextSelectionPopup::PropertyHandler::GetProperty(Ui::TextSelectionPopup selectionPopup,
                                                                 Property::Index index)
{
  Property::Value value;
  TextSelectionPopup& impl(GetImpl(selectionPopup));

  switch (index)
  {
    case Ui::TextSelectionPopup::Property::POPUP_MAX_SIZE:
    {
      value = impl.GetDimensionToCustomise(POPUP_MAXIMUM_SIZE);
      break;
    }
    case Ui::TextSelectionPopup::Property::OPTION_MAX_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_MAXIMUM_SIZE);
      break;
    }
    case Ui::TextSelectionPopup::Property::OPTION_MIN_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_MINIMUM_SIZE);
      break;
    }
    case Ui::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE:
    {
      value = impl.GetDimensionToCustomise(OPTION_DIVIDER_SIZE);
      break;
    }
    case Ui::TextSelectionPopup::Property::OPTION_DIVIDER_PADDING:
    {
      Padding padding = impl.GetOptionDividerPadding();
      value = Vector4(padding.left, padding.right, padding.top, padding.bottom);
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Ui::TextSelectionPopup::CLIPBOARD);
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Ui::TextSelectionPopup::CUT);
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Ui::TextSelectionPopup::COPY);
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Ui::TextSelectionPopup::PASTE);
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Ui::TextSelectionPopup::SELECT);
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
    {
      value = impl.GetButtonImage(Ui::TextSelectionPopup::SELECT_ALL);
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_DIVIDER_COLOR:
    {
      value = impl.mDividerColor;
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_ICON_COLOR:
    {
      value = impl.mIconColor;
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_PRESSED_COLOR:
    {
      value = impl.mPressedColor;
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_PRESSED_CORNER_RADIUS:
    {
      value = impl.mPressedCornerRadius;
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_PRESSED_IMAGE:
    {
      value = impl.GetPressedImage();
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION:
    {
      value = impl.mFadeInDuration;
      break;
    }
    case Ui::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION:
    {
      value = impl.mFadeOutDuration;
      break;
    }
    case Ui::TextSelectionPopup::Property::BACKGROUND_BORDER:
    {
      Property::Map map;
      Ui::Visual::Base visual = DevelControl::GetVisual(impl, Ui::TextSelectionPopup::Property::BACKGROUND_BORDER);
      if (visual)
      {
        visual.CreatePropertyMap(map);
      }
      value = map;
      break;
    }
    case Ui::TextSelectionPopup::Property::BACKGROUND:
    {
      Property::Map map;
      Ui::Visual::Base visual = DevelControl::GetVisual(impl, Ui::Control::Property::BACKGROUND);
      if (visual)
      {
        visual.CreatePropertyMap(map);
      }
      value = map;
      break;
    }
    case Ui::TextSelectionPopup::Property::LABEL_MINIMUM_SIZE:
    {
      value = impl.mLabelMinimumSize;
      break;
    }
    case Ui::TextSelectionPopup::Property::LABEL_PADDING:
    {
      Padding padding = impl.GetLabelPadding();
      value = Vector4(padding.left, padding.right, padding.top, padding.bottom);
      break;
    }
    case Ui::TextSelectionPopup::Property::LABEL_TEXT_VISUAL:
    {
      value = impl.mLabelTextVisual;
      break;
    }
    case Ui::TextSelectionPopup::Property::ENABLE_SCROLL_BAR:
    {
      value = impl.mEnableScrollBar;
      break;
    }
  }

  return value;
}

} // namespace Dali::Ui::Internal
