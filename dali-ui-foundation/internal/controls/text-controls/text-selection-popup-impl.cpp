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

// CLASS HEADER
#include <dali-ui-foundation/internal/controls/text-controls/text-selection-popup-impl.h>

// EXTERNAL INCLUDES
#if defined(__GLIBC__)
#include <libintl.h>
#endif
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <string.h>
#include <algorithm>
#include <cfloat>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/buttons/button-devel.h>
#include <dali-ui-foundation/devel-api/controls/control-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/devel-api/controls/text-controls/text-selection-popup-callback-interface.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>
#include <dali-ui-foundation/internal/controls/text-controls/text-selection-popup-property-handler.h>
#include <dali-ui-foundation/internal/helpers/color-conversion.h>
#include <dali-ui-foundation/public-api/controls/text-controls/text-label.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
#if defined(__GLIBC__)
#define GET_LOCALE_TEXT(string) dgettext("dali-toolkit", string)
#endif

const char* TEXT_SELECTION_POPUP_BUTTON_STYLE_NAME("TextSelectionPopupButton");
const Dali::Vector4 DEFAULT_OPTION_PRESSED_COLOR(Dali::Vector4(0.24f, 0.72f, 0.8f, 1.0f));
const float DEFAULT_OPTION_PRESSED_CORNER_RADIUS = 0.0f;
const Dali::Vector4 DEFAULT_LABEL_PADDING(Dali::Vector4(24.0f, 24.0f, 14.0f, 14.0f));

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

#ifdef DGETTEXT_ENABLED

#define POPUP_CUT_STRING GET_LOCALE_TEXT("IDS_COM_BODY_CUT")
#define POPUP_COPY_STRING GET_LOCALE_TEXT("IDS_COM_BODY_COPY")
#define POPUP_PASTE_STRING GET_LOCALE_TEXT("IDS_COM_BODY_PASTE")
#define POPUP_SELECT_STRING GET_LOCALE_TEXT("IDS_COM_SK_SELECT")
#define POPUP_SELECT_ALL_STRING GET_LOCALE_TEXT("IDS_COM_BODY_SELECT_ALL")
#define POPUP_CLIPBOARD_STRING GET_LOCALE_TEXT("IDS_COM_BODY_CLIPBOARD")

#else

#define POPUP_CUT_STRING "Cut"
#define POPUP_COPY_STRING "Copy"
#define POPUP_PASTE_STRING "Paste"
#define POPUP_SELECT_STRING "Select"
#define POPUP_SELECT_ALL_STRING "Select All"
#define POPUP_CLIPBOARD_STRING "Clipboard"

#endif

const char* const OPTION_SELECT_WORD = "option-select_word"; // "Select Word" popup option.
const char* const OPTION_SELECT_ALL("option-select_all");    // "Select All" popup option.
const char* const OPTION_CUT("optionCut");                   // "Cut" popup option.
const char* const OPTION_COPY("optionCopy");                 // "Copy" popup option.
const char* const OPTION_PASTE("optionPaste");               // "Paste" popup option.
const char* const OPTION_CLIPBOARD("optionClipboard");       // "Clipboard" popup option.

const char* IDS_LTR("IDS_LTR");
const char* RTL_DIRECTION("RTL");

BaseHandle Create()
{
  return Ui::TextSelectionPopup::New(NULL);
}

// Setup properties, signals and actions using the type-registry.

DALI_TYPE_REGISTRATION_BEGIN(Ui::TextSelectionPopup, Ui::Control, Create);

DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupMaxSize", VECTOR2, POPUP_MAX_SIZE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupMinSize", VECTOR2, POPUP_MIN_SIZE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "optionMaxSize", VECTOR2, OPTION_MAX_SIZE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "optionMinSize", VECTOR2, OPTION_MIN_SIZE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "optionDividerSize", VECTOR2, OPTION_DIVIDER_SIZE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "optionDividerPadding", VECTOR4, OPTION_DIVIDER_PADDING)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupClipboardButtonImage", STRING,
                           POPUP_CLIPBOARD_BUTTON_ICON_IMAGE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupCutButtonImage", STRING, POPUP_CUT_BUTTON_ICON_IMAGE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupCopyButtonImage", STRING, POPUP_COPY_BUTTON_ICON_IMAGE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupPasteButtonImage", STRING, POPUP_PASTE_BUTTON_ICON_IMAGE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupSelectButtonImage", STRING, POPUP_SELECT_BUTTON_ICON_IMAGE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupSelectAllButtonImage", STRING,
                           POPUP_SELECT_ALL_BUTTON_ICON_IMAGE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupDividerColor", VECTOR4, POPUP_DIVIDER_COLOR)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupIconColor", VECTOR4, POPUP_ICON_COLOR)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupPressedColor", VECTOR4, POPUP_PRESSED_COLOR)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupPressedCornerRadius", FLOAT, POPUP_PRESSED_CORNER_RADIUS)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupPressedImage", STRING, POPUP_PRESSED_IMAGE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupFadeInDuration", FLOAT, POPUP_FADE_IN_DURATION)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "popupFadeOutDuration", FLOAT, POPUP_FADE_OUT_DURATION)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "backgroundBorder", MAP, BACKGROUND_BORDER)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "background", MAP, BACKGROUND)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "labelMinimumSize", VECTOR2, LABEL_MINIMUM_SIZE)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "labelPadding", VECTOR4, LABEL_PADDING)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "labelTextVisual", MAP, LABEL_TEXT_VISUAL)
DALI_PROPERTY_REGISTRATION(Ui, TextSelectionPopup, "enableScrollBar", BOOLEAN, ENABLE_SCROLL_BAR)

DALI_TYPE_REGISTRATION_END()

} // namespace

Dali::Ui::TextSelectionPopup TextSelectionPopup::New(TextSelectionPopupCallbackInterface* callbackInterface)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextSelectionPopup::New\n");

  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr<TextSelectionPopup> impl = new TextSelectionPopup(callbackInterface);

  // Pass ownership to CustomActor handle
  Dali::Ui::TextSelectionPopup handle(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextSelectionPopup::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Ui::TextSelectionPopup selectionPopup = Ui::TextSelectionPopup::DownCast(Dali::BaseHandle(object));

  if (selectionPopup)
  {
    PropertyHandler::SetProperty(selectionPopup, index, value);
  }
}

Property::Value TextSelectionPopup::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;
  Ui::TextSelectionPopup selectionPopup = Ui::TextSelectionPopup::DownCast(Dali::BaseHandle(object));

  if (selectionPopup)
  {
    value = PropertyHandler::GetProperty(selectionPopup, index);
  }
  return value;
}

void TextSelectionPopup::SetProperties(const Property::Map& properties)
{
  Ui::TextSelectionPopup selectionPopup = Ui::TextSelectionPopup::DownCast(Self());

  if (selectionPopup)
  {
    const Property::Map::SizeType count = properties.Count();
    for (Property::Map::SizeType position = 0; position < count; ++position)
    {
      KeyValuePair keyValue = properties.GetKeyValue(position);
      Property::Key& key = keyValue.first;
      Property::Value& value = keyValue.second;
      PropertyHandler::SetProperty(selectionPopup, key.indexKey, value);
    }
  }
}

void TextSelectionPopup::GetProperties(Property::Map& properties)
{
  Property::Map map;

  map.Insert(Ui::TextSelectionPopup::Property::POPUP_MAX_SIZE, GetDimensionToCustomise(POPUP_MAXIMUM_SIZE));
  map.Insert(Ui::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE, GetDimensionToCustomise(OPTION_DIVIDER_SIZE));
  map.Insert(Ui::TextSelectionPopup::Property::OPTION_DIVIDER_PADDING,
             Vector4(mOptionDividerPadding.left, mOptionDividerPadding.right, mOptionDividerPadding.top,
                     mOptionDividerPadding.bottom));
  map.Insert(Ui::TextSelectionPopup::Property::LABEL_MINIMUM_SIZE, mLabelMinimumSize);
  map.Insert(Ui::TextSelectionPopup::Property::LABEL_PADDING,
             Vector4(mLabelPadding.left, mLabelPadding.right, mLabelPadding.top, mLabelPadding.bottom));
  map.Insert(Ui::TextSelectionPopup::Property::LABEL_TEXT_VISUAL, mLabelTextVisual);
  map.Insert(Ui::TextSelectionPopup::Property::ENABLE_SCROLL_BAR, mEnableScrollBar);
  map.Insert(Ui::TextSelectionPopup::Property::POPUP_DIVIDER_COLOR, mDividerColor);
  map.Insert(Ui::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION, mFadeInDuration);
  map.Insert(Ui::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION, mFadeOutDuration);
  map.Insert(Ui::TextSelectionPopup::Property::POPUP_PRESSED_COLOR, mPressedColor);
  map.Insert(Ui::TextSelectionPopup::Property::POPUP_PRESSED_CORNER_RADIUS, mPressedCornerRadius);

  Property::Map backgroundMap;
  Ui::Visual::Base backgroundVisual = DevelControl::GetVisual(*this, Ui::Control::Property::BACKGROUND);
  if (backgroundVisual)
  {
    backgroundVisual.CreatePropertyMap(backgroundMap);
  }
  map.Insert(Ui::TextSelectionPopup::Property::BACKGROUND, backgroundMap);

  Property::Map borderMap;
  Ui::Visual::Base borderVisual = DevelControl::GetVisual(*this, Ui::TextSelectionPopup::Property::BACKGROUND_BORDER);
  if (borderVisual)
  {
    borderVisual.CreatePropertyMap(borderMap);
  }
  map.Insert(Ui::TextSelectionPopup::Property::BACKGROUND_BORDER, borderMap);

  properties = map;
}

void TextSelectionPopup::EnableButtons(Ui::TextSelectionPopup::Buttons buttonsToEnable)
{
  mEnabledButtons = buttonsToEnable;
  mButtonsChanged = true;
}

void TextSelectionPopup::RaiseAbove(Actor target)
{
  if (mToolbar)
  {
    mToolbar.RaiseAbove(target);
  }
}

void TextSelectionPopup::ShowPopup()
{
  if ((!mPopupShowing || mButtonsChanged) && (Ui::TextSelectionPopup::NONE != mEnabledButtons))
  {
    Actor self = Self();
    AddPopupOptionsToToolbar(mShowIcons, mShowCaptions);

    Animation animation = Animation::New(mFadeInDuration);
    animation.AnimateTo(Property(self, Actor::Property::COLOR_ALPHA), 1.0f);
    animation.Play();
    mPopupShowing = true;
  }
}

void TextSelectionPopup::HidePopup()
{
  if (mPopupShowing)
  {
    mPopupShowing = false;
    Actor self = Self();
    Animation animation = Animation::New(mFadeOutDuration);
    animation.AnimateTo(Property(self, Actor::Property::COLOR_ALPHA), 0.0f);
    animation.FinishedSignal().Connect(this, &TextSelectionPopup::HideAnimationFinished);
    animation.Play();
  }
}

void TextSelectionPopup::OnInitialize()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextSelectionPopup::OnInitialize\n");
  Actor self = Self();
  self.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS);
  self.SetProperty(Actor::Property::COLOR_ALPHA, 0.0f);

  // Accessibility
  self.SetProperty(Ui::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);
  self.SetProperty(Ui::DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::DIALOG);
}

void TextSelectionPopup::HideAnimationFinished(Animation& animation)
{
  Actor self = Self();
  if (!mPopupShowing) // During the Hide/Fade animation there could be a call to Show the Popup again, mPopupShowing
                      // will be true in this case.
  {
    DALI_LOG_INFO(gLogFilter, Debug::General, "TextSelectionPopup::HideAnimationFinished\n");
    UnparentAndReset(mToolbar);
  }
}

bool TextSelectionPopup::OnCutButtonPressed(Ui::Button button)
{
  if (mCallbackInterface)
  {
    mCallbackInterface->TextPopupButtonTouched(Ui::TextSelectionPopup::CUT);
  }

  return true;
}

bool TextSelectionPopup::OnCopyButtonPressed(Ui::Button button)
{
  if (mCallbackInterface)
  {
    mCallbackInterface->TextPopupButtonTouched(Dali::Ui::TextSelectionPopup::COPY);
  }

  return true;
}

bool TextSelectionPopup::OnPasteButtonPressed(Ui::Button button)
{
  if (mCallbackInterface)
  {
    mCallbackInterface->TextPopupButtonTouched(Dali::Ui::TextSelectionPopup::PASTE);
  }

  return true;
}

bool TextSelectionPopup::OnSelectButtonPressed(Ui::Button button)
{
  if (mCallbackInterface)
  {
    mCallbackInterface->TextPopupButtonTouched(Dali::Ui::TextSelectionPopup::SELECT);
  }

  return true;
}

bool TextSelectionPopup::OnSelectAllButtonPressed(Ui::Button button)
{
  if (mCallbackInterface)
  {
    mCallbackInterface->TextPopupButtonTouched(Dali::Ui::TextSelectionPopup::SELECT_ALL);
  }

  return true;
}

bool TextSelectionPopup::OnClipboardButtonPressed(Ui::Button button)
{
  if (mCallbackInterface)
  {
    mCallbackInterface->TextPopupButtonTouched(Dali::Ui::TextSelectionPopup::CLIPBOARD);
  }

  return true;
}

void TextSelectionPopup::SetDimensionToCustomise(const PopupCustomisations& settingToCustomise, const Size& dimension)
{
  switch (settingToCustomise)
  {
    case POPUP_MAXIMUM_SIZE:
    {
      mPopupMaxSize = dimension;
      if (mToolbar)
      {
        mToolbar.SetProperty(Ui::TextSelectionToolbar::Property::MAX_SIZE, dimension);
      }
      break;
    }
    case OPTION_MAXIMUM_SIZE:
    {
      mOptionMaxSize = dimension;
      // Option max size not currently currently supported
      break;
    }
    case OPTION_MINIMUM_SIZE:
    {
      mOptionMinSize = dimension;
      // Option min size not currently currently supported
      break;
    }
    case OPTION_DIVIDER_SIZE:
    {
      mOptionDividerSize = dimension;
      if (mToolbar)
      {
        // Resize Dividers not currently supported
      }
      break;
    }
  } // switch
}

Size TextSelectionPopup::GetDimensionToCustomise(const PopupCustomisations& settingToCustomise) const
{
  switch (settingToCustomise)
  {
    case POPUP_MAXIMUM_SIZE:
    {
      if (mToolbar)
      {
        return mToolbar.GetProperty(Ui::TextSelectionToolbar::Property::MAX_SIZE).Get<Vector2>();
      }
      else
      {
        return mPopupMaxSize;
      }
    }
    case OPTION_MAXIMUM_SIZE:
    {
      return mOptionMaxSize;
    }
    case OPTION_MINIMUM_SIZE:
    {
      return mOptionMinSize;
    }
    case OPTION_DIVIDER_SIZE:
    {
      return mOptionDividerSize;
    }
  } // switch

  return Size::ZERO;
}

void TextSelectionPopup::SetButtonImage(Ui::TextSelectionPopup::Buttons button, const std::string& image)
{
  switch (button)
  {
    case Ui::TextSelectionPopup::CLIPBOARD:
    {
      mClipboardIconImage = image;
      break;
    }
    case Ui::TextSelectionPopup::CUT:
    {
      mCutIconImage = image;
      break;
    }
    case Ui::TextSelectionPopup::COPY:
    {
      mCopyIconImage = image;
      break;
    }
    case Ui::TextSelectionPopup::PASTE:
    {
      mPasteIconImage = image;
      break;
    }
    case Ui::TextSelectionPopup::SELECT:
    {
      mSelectIconImage = image;
      break;
    }
    case Ui::TextSelectionPopup::SELECT_ALL:
    {
      mSelectAllIconImage = image;
      break;
    }
    default:
    {
      DALI_ASSERT_DEBUG("TextSelectionPopup SetPopupImage Unknown Button");
    }
  } // switch
}

const std::string& TextSelectionPopup::GetButtonImage(Ui::TextSelectionPopup::Buttons button) const
{
  switch (button)
  {
    case Ui::TextSelectionPopup::CLIPBOARD:
    {
      return mClipboardIconImage;
      break;
    }
    case Ui::TextSelectionPopup::CUT:
    {
      return mCutIconImage;
      break;
    }
    case Ui::TextSelectionPopup::COPY:
    {
      return mCopyIconImage;
      break;
    }
    case Ui::TextSelectionPopup::PASTE:
    {
      return mPasteIconImage;
      break;
    }
    case Ui::TextSelectionPopup::SELECT:
    {
      return mSelectIconImage;
      break;
    }
    case Ui::TextSelectionPopup::SELECT_ALL:
    {
      return mSelectAllIconImage;
      break;
    }
    case Ui::TextSelectionPopup::NONE:
    {
      break;
    }
  } // switch

  DALI_ASSERT_DEBUG("TextSelectionPopup GetPopupImage Unknown Button");
  static std::string empty;
  return empty;
}

void TextSelectionPopup::SetPressedImage(const std::string& filename)
{
  mPressedImage = filename;
}

std::string TextSelectionPopup::GetPressedImage() const
{
  return mPressedImage;
}

void TextSelectionPopup::SetOptionDividerPadding(const Padding& padding)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextSelectionPopup::SetOptionDividerPadding padding(%f,%f,%f,%f)\n",
                padding.left, padding.right, padding.top, padding.bottom);
  mOptionDividerPadding = Padding(padding.left, padding.right, padding.top, padding.bottom);
}

Padding TextSelectionPopup::GetOptionDividerPadding() const
{
  return mOptionDividerPadding;
}

void TextSelectionPopup::SetLabelPadding(const Padding& padding)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextSelectionPopup::SetLabelPadding padding(%f,%f,%f,%f)\n", padding.left,
                padding.right, padding.top, padding.bottom);
  mLabelPadding = Padding(padding.left, padding.right, padding.top, padding.bottom);
}

Padding TextSelectionPopup::GetLabelPadding() const
{
  return mLabelPadding;
}

void TextSelectionPopup::CreateOrderedListOfPopupOptions()
{
  mOrderListOfButtons.clear();
  mOrderListOfButtons.reserve(8u);

  // Create button for each possible option using Option priority
  mOrderListOfButtons.push_back(ButtonRequirement(Ui::TextSelectionPopup::CUT, mCutOptionPriority, OPTION_CUT,
                                                  POPUP_CUT_STRING,
                                                  0 != (mEnabledButtons & Ui::TextSelectionPopup::CUT)));
  mOrderListOfButtons.push_back(ButtonRequirement(Ui::TextSelectionPopup::COPY, mCopyOptionPriority, OPTION_COPY,
                                                  POPUP_COPY_STRING,
                                                  0 != (mEnabledButtons & Ui::TextSelectionPopup::COPY)));
  mOrderListOfButtons.push_back(ButtonRequirement(Ui::TextSelectionPopup::PASTE, mPasteOptionPriority, OPTION_PASTE,
                                                  POPUP_PASTE_STRING,
                                                  0 != (mEnabledButtons & Ui::TextSelectionPopup::PASTE)));
  mOrderListOfButtons.push_back(ButtonRequirement(Ui::TextSelectionPopup::SELECT, mSelectOptionPriority,
                                                  OPTION_SELECT_WORD, POPUP_SELECT_STRING,
                                                  0 != (mEnabledButtons & Ui::TextSelectionPopup::SELECT)));
  mOrderListOfButtons.push_back(ButtonRequirement(Ui::TextSelectionPopup::SELECT_ALL, mSelectAllOptionPriority,
                                                  OPTION_SELECT_ALL, POPUP_SELECT_ALL_STRING,
                                                  0 != (mEnabledButtons & Ui::TextSelectionPopup::SELECT_ALL)));
  mOrderListOfButtons.push_back(ButtonRequirement(Ui::TextSelectionPopup::CLIPBOARD, mClipboardOptionPriority,
                                                  OPTION_CLIPBOARD, POPUP_CLIPBOARD_STRING,
                                                  0 != (mEnabledButtons & Ui::TextSelectionPopup::CLIPBOARD)));

  // Sort the buttons according their priorities.
  std::sort(mOrderListOfButtons.begin(), mOrderListOfButtons.end(), TextSelectionPopup::ButtonPriorityCompare());
}

void TextSelectionPopup::AddOption(const ButtonRequirement& button, bool showDivider, bool showIcons, bool showCaption)
{
  // 1. Create a option.
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextSelectionPopup::AddOption\n");

  Ui::PushButton option = Ui::PushButton::New();
  option.SetProperty(Dali::Actor::Property::NAME, button.name);
  option.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
  option.SetProperty(Actor::Property::MINIMUM_SIZE, mLabelMinimumSize);

  switch (button.id)
  {
    case Ui::TextSelectionPopup::CUT:
    {
      option.ClickedSignal().Connect(this, &TextSelectionPopup::OnCutButtonPressed);
      break;
    }
    case Ui::TextSelectionPopup::COPY:
    {
      option.ClickedSignal().Connect(this, &TextSelectionPopup::OnCopyButtonPressed);
      break;
    }
    case Ui::TextSelectionPopup::PASTE:
    {
      option.ClickedSignal().Connect(this, &TextSelectionPopup::OnPasteButtonPressed);
      break;
    }
    case Ui::TextSelectionPopup::SELECT:
    {
      option.ClickedSignal().Connect(this, &TextSelectionPopup::OnSelectButtonPressed);
      break;
    }
    case Ui::TextSelectionPopup::SELECT_ALL:
    {
      option.ClickedSignal().Connect(this, &TextSelectionPopup::OnSelectAllButtonPressed);
      break;
    }
    case Ui::TextSelectionPopup::CLIPBOARD:
    {
      option.ClickedSignal().Connect(this, &TextSelectionPopup::OnClipboardButtonPressed);
      break;
    }
    case Ui::TextSelectionPopup::NONE:
    {
      // Nothing to do:
      break;
    }
  }

  // 2. Set the options contents.
  if (showCaption)
  {
    // PushButton layout properties.
    const Padding padding(mLabelPadding);
    option.SetProperty(Ui::PushButton::Property::LABEL_PADDING, padding);

    // Label properties.
    Property::Map buttonLabelProperties;
    buttonLabelProperties.Insert(Ui::TextVisual::Property::TEXT, button.caption);
    buttonLabelProperties.Merge(mLabelTextVisual);

    option.SetProperty(Ui::Button::Property::LABEL, buttonLabelProperties);
  }
  if (showIcons)
  {
    option.SetProperty(Ui::PushButton::Property::ICON_PADDING, Vector4(10.0f, 10.0f, 10.0f, 10.0f));
    option.SetProperty(Ui::DevelButton::Property::LABEL_RELATIVE_ALIGNMENT, "BOTTOM");

    // TODO: This is temporarily disabled until the text-selection-popup image API is changed to strings.
    // option.SetProperty(  Ui::Button::Property::SELECTED_VISUAL, button.icon );
    // option.SetProperty(  Ui::Button::Property::UNSELECTED_VISUAL, button.icon );
  }

  // 3. Set the normal option image (blank / Transparent).
  option.SetProperty(Ui::Button::Property::UNSELECTED_BACKGROUND_VISUAL, "");

  // 4. Set the pressed option image.
  Property::Value selectedBackgroundValue(mPressedImage);
  if (mPressedImage.empty())
  {
    // The image can be blank, the color can be used in that case.
    selectedBackgroundValue =
        Property::Value{{Ui::Visual::Property::TYPE, Ui::Visual::COLOR},
                        {Ui::ColorVisual::Property::MIX_COLOR, mPressedColor},
                        {Ui::DevelVisual::Property::CORNER_RADIUS, mPressedCornerRadius},
                        {Ui::DevelVisual::Property::CORNER_RADIUS_POLICY, Ui::Visual::Transform::Policy::RELATIVE}};
  }
  option.SetProperty(Ui::Button::Property::SELECTED_BACKGROUND_VISUAL, selectedBackgroundValue);
  // The value set by user takes precedence over the theme value.
  if (mLabelTextVisual.Count() == 0)
  {
    option.SetProperty(Ui::Control::Property::STYLE_NAME, TEXT_SELECTION_POPUP_BUTTON_STYLE_NAME);
  }

  // 5 Add option to tool bar
  mToolbar.AddOption(option);

  // 6. Add the divider
  if (showDivider)
  {
    const Size size(mOptionDividerSize.width, 0.0f); // Height FILL_TO_PARENT
    const Padding padding(mOptionDividerPadding);

    Ui::Control divider = Ui::Control::New();
#ifdef DECORATOR_DEBUG
    divider.SetProperty(Dali::Actor::Property::NAME, "Text's popup divider");
#endif
    divider.SetProperty(Actor::Property::SIZE, size);
    divider.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT);
    divider.SetProperty(Actor::Property::PADDING, padding);
    divider.SetBackgroundColor(mDividerColor);
    mToolbar.AddDivider(divider);
  }
}

std::size_t TextSelectionPopup::GetNumberOfEnabledOptions() const
{
  std::size_t numberOfOptions = 0u;
  for (std::vector<ButtonRequirement>::const_iterator it = mOrderListOfButtons.begin(),
                                                      endIt = mOrderListOfButtons.end();
       (it != endIt); ++it)
  {
    const ButtonRequirement& button(*it);
    if (button.enabled)
    {
      ++numberOfOptions;
    }
  }

  return numberOfOptions;
}

void TextSelectionPopup::AddPopupOptionsToToolbar(bool showIcons, bool showCaptions)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextSelectionPopup::AddPopupOptionsToToolbar\n");

  CreateOrderedListOfPopupOptions();

  mButtonsChanged = false;
  UnparentAndReset(mToolbar);

  if (!mToolbar)
  {
    Actor self = Self();
    mToolbar = Ui::TextSelectionToolbar::New();
    if (mPopupMaxSize != Vector2::ZERO) // If PopupMaxSize property set then apply to Toolbar. Toolbar currently is not
                                        // retriving this from json
    {
      mToolbar.SetProperty(Ui::TextSelectionToolbar::Property::MAX_SIZE, mPopupMaxSize);
    }
    mToolbar.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
#ifdef DECORATOR_DEBUG
    mToolbar.SetProperty(Dali::Actor::Property::NAME, "TextSelectionToolbar");
#endif
    mToolbar.SetProperty(Ui::TextSelectionToolbar::Property::ENABLE_SCROLL_BAR, mEnableScrollBar);
    self.Add(mToolbar);
  }

  // Whether to mirror the list of buttons (for right to left languages)
  bool mirror = false;
#if defined(__GLIBC__)
  char* idsLtr = GET_LOCALE_TEXT(IDS_LTR);
  if (NULL != idsLtr)
  {
    mirror = (0 == strcmp(idsLtr, RTL_DIRECTION));

    if (mirror)
    {
      std::reverse(mOrderListOfButtons.begin(), mOrderListOfButtons.end());
    }
  }
#endif

  // Iterate list of buttons and add active ones to Toolbar
  std::size_t numberOfOptionsRequired = GetNumberOfEnabledOptions();
  std::size_t numberOfOptionsAdded = 0u;
  for (std::vector<ButtonRequirement>::const_iterator it = mOrderListOfButtons.begin(),
                                                      endIt = mOrderListOfButtons.end();
       (it != endIt); ++it)
  {
    const ButtonRequirement& button(*it);
    if (button.enabled)
    {
      numberOfOptionsAdded++;
      AddOption(button, (numberOfOptionsAdded < numberOfOptionsRequired), showIcons, showCaptions);
    }
  }

  if (mirror)
  {
    mToolbar.ScrollTo(Vector2(mPopupMaxSize.x, 0.f));
  }
}

void TextSelectionPopup::CreateBackgroundBorder(Property::Map& propertyMap)
{
  // Removes previous image if necessary
  DevelControl::UnregisterVisual(*this, Ui::TextSelectionPopup::Property::BACKGROUND_BORDER);

  if (!propertyMap.Empty())
  {
    Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(propertyMap);

    if (visual)
    {
      DevelControl::RegisterVisual(*this, Ui::TextSelectionPopup::Property::BACKGROUND_BORDER, visual,
                                   DepthIndex::CONTENT);
    }
  }
}

void TextSelectionPopup::CreateBackground(Property::Map& propertyMap)
{
  // Removes previous image if necessary
  DevelControl::UnregisterVisual(*this, Ui::Control::Property::BACKGROUND);

  if (!propertyMap.Empty())
  {
    Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(propertyMap);

    if (visual)
    {
      DevelControl::RegisterVisual(*this, Ui::Control::Property::BACKGROUND, visual, DepthIndex::BACKGROUND);
    }
  }
}

TextSelectionPopup::TextSelectionPopup(TextSelectionPopupCallbackInterface* callbackInterface)
  : Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
    mToolbar(),
    mPopupMaxSize(),
    mOptionMaxSize(),
    mOptionMinSize(),
    mOptionDividerSize(),
    mOptionDividerPadding(),
    mLabelMinimumSize(),
    mLabelPadding(DEFAULT_LABEL_PADDING),
    mLabelTextVisual(),
    mEnableScrollBar(true),
    mEnabledButtons(Ui::TextSelectionPopup::NONE),
    mCallbackInterface(callbackInterface),
    mPressedColor(DEFAULT_OPTION_PRESSED_COLOR),
    mPressedCornerRadius(DEFAULT_OPTION_PRESSED_CORNER_RADIUS),
    mDividerColor(Color::WHITE),
    mIconColor(Color::WHITE),
    mSelectOptionPriority(1),
    mSelectAllOptionPriority(2),
    mCutOptionPriority(4),
    mCopyOptionPriority(3),
    mPasteOptionPriority(5),
    mClipboardOptionPriority(6),
    mFadeInDuration(0.0f),
    mFadeOutDuration(0.0f),
    mShowIcons(false),
    mShowCaptions(true),
    mPopupShowing(false),
    mButtonsChanged(false)
{
}

TextSelectionPopup::~TextSelectionPopup()
{
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
