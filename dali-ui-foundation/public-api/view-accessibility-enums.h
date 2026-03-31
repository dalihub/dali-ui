#ifndef DALI_UI_VIEW_ACCESSIBILITY_ENUMS_H
#define DALI_UI_VIEW_ACCESSIBILITY_ENUMS_H

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

namespace Dali::Ui
{

/**
 * @brief Represents current state of a control.
 */
enum class AccessibilityState : uint32_t
{
  ENABLED = 0,
  SELECTED,
  CHECKED,
  BUSY,
  EXPANDED,
  MAX_COUNT
};

constexpr const uint32_t ROLE_START_INDEX = 200;

/**
 * @brief AccessibilityRole represents the purpose of a control.
 */
enum class AccessibilityRole : uint32_t
{
  ADJUSTABLE = ROLE_START_INDEX,
  ALERT,
  BUTTON,
  CHECK_BOX,
  COMBO_BOX,
  CONTAINER,
  DIALOG,
  ENTRY,
  HEADER,
  IMAGE,
  LINK,
  LIST,
  LIST_ITEM,
  MENU,
  MENU_BAR,
  MENU_ITEM,
  NONE,
  NOTIFICATION,
  PASSWORD_TEXT,
  POPUP_MENU,
  PROGRESS_BAR,
  RADIO_BUTTON,
  SCROLL_BAR,
  SPIN_BUTTON,
  TAB,
  TAB_LIST,
  TEXT,
  TOGGLE_BUTTON,
  TOOL_BAR,
  SCENE_3D,
  MODEL,
  MAX_COUNT
};

} // namespace Dali::Ui

#endif // DALI_UI_VIEW_ACCESSIBILITY_ENUMS_H
