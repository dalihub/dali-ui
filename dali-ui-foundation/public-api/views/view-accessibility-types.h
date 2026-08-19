#ifndef DALI_UI_VIEW_ACCESSIBILITY_TYPES_H
#define DALI_UI_VIEW_ACCESSIBILITY_TYPES_H

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

#include <cstdint> // LCOV_EXCL_LINE

namespace Dali::Ui
{
/**
 * @brief Contains public accessibility semantic types used by Dali UI views.
 *
 * These types describe the accessibility meaning exposed by views without
 * requiring application code to depend on the platform accessibility backend.
 *
 * @SINCE_2_5.30
 */
namespace Accessibility
{

/**
 * @brief Represents an application-controlled accessibility state of a View.
 *
 * These states are semantic hints supplied by application or View code.
 * Dali UI combines them with runtime states such as visibility, focus, and
 * sensitivity before exposing the final accessibility state set.
 *
 * @SINCE_2_5.30
 */
enum class State : uint32_t
{
  ENABLED = 0, ///< The View is enabled for accessibility interaction. @SINCE_2_5.30
  SELECTED,    ///< The View or item is selected. @SINCE_2_5.30
  CHECKED,     ///< The View has a checked or toggled-on state. @SINCE_2_5.30
  BUSY,        ///< The View is busy and its value or contents may still be updating. @SINCE_2_5.30
  EXPANDED,    ///< The View is expanded and its child content is currently shown. @SINCE_2_5.30
  MAX_COUNT    ///< Sentinel value used to validate accessibility state values. @SINCE_2_5.30
};

/**
 * @brief Identifies a part of a View that a screen reader should announce.
 *
 * Values are managed through View's Add, Remove, Clear, and Has methods. This
 * enum is not a bit mask and does not provide bitwise operators.
 *
 * @SINCE_2_5.30
 */
enum class ReadingInfo : uint32_t
{
  NAME = 0,    ///< Announce the accessibility name. @SINCE_2_5.30
  ROLE,        ///< Announce the accessibility role. @SINCE_2_5.30
  DESCRIPTION, ///< Announce the accessibility description. @SINCE_2_5.30
  STATE,       ///< Announce the accessibility state. @SINCE_2_5.30
  MAX_COUNT    ///< Sentinel value used to validate reading information values. @SINCE_2_5.30
};

/**
 * @brief Describes a change in the reading lifecycle of a View.
 *
 * @SINCE_2_5.30
 */
enum class ReadingStatus : uint32_t
{
  SKIPPED = 0, ///< Reading was skipped before it could be completed. @SINCE_2_5.30
  PAUSED,      ///< Reading was paused. @SINCE_2_5.30
  RESUMED,     ///< Reading resumed after being paused. @SINCE_2_5.30
  CANCELLED,   ///< Pending or active reading was cancelled. @SINCE_2_5.30
  STOPPED      ///< Reading stopped or completed. @SINCE_2_5.30
};

/**
 * @brief First numeric value reserved for public Dali UI accessibility roles.
 *
 * @SINCE_2_5.30
 */
constexpr const uint32_t ROLE_START_INDEX = 200;

/**
 * @brief Represents the accessibility purpose of a View.
 *
 * A role describes what a View is, rather than how it is implemented.
 * For example, a custom View that behaves like a button should use @c BUTTON
 * even if it is not implemented by a button-specific class.
 *
 * @SINCE_2_5.30
 */
enum class Role : uint32_t
{
  ADJUSTABLE = ROLE_START_INDEX, ///< A View whose value can be adjusted within a range, such as a slider. @SINCE_2_5.30
  ALERT,                         ///< A View or region containing important time-sensitive information. @SINCE_2_5.30
  BUTTON,                        ///< A View that performs an action when activated. @SINCE_2_5.30
  CHECK_BOX,                     ///< A View that represents a checked or unchecked choice. @SINCE_2_5.30
  COMBO_BOX,                     ///< A View that combines an entry or button with a selectable list. @SINCE_2_5.30
  CONTAINER,                     ///< A grouping object that contains other accessible Views. @SINCE_2_5.30
  DIALOG,                        ///< A dialog window or dialog-like View. @SINCE_2_5.30
  ENTRY,                         ///< A View that accepts editable text input. @SINCE_2_5.30
  HEADER,                        ///< A heading or header that labels a section. @SINCE_2_5.30
  IMAGE,                         ///< A View that primarily presents image content. @SINCE_2_5.30
  LINK,                          ///< A View that navigates to another location when activated. @SINCE_2_5.30
  LIST,                          ///< A View that contains a list of selectable or readable items. @SINCE_2_5.30
  LIST_ITEM,                     ///< An item inside a list. @SINCE_2_5.30
  MENU,                          ///< A menu containing menu items or submenus. @SINCE_2_5.30
  MENU_BAR,                      ///< A bar that contains top-level menus. @SINCE_2_5.30
  MENU_ITEM,                     ///< An item inside a menu. @SINCE_2_5.30
  NONE,                          ///< No specific accessibility role is assigned. @SINCE_2_5.30
  NOTIFICATION,                  ///< A non-modal notification message. @SINCE_2_5.30
  PASSWORD_TEXT,                 ///< A text entry whose contents are obscured. @SINCE_2_5.30
  POPUP_MENU,                    ///< A temporary popup menu. @SINCE_2_5.30
  PROGRESS_BAR,                  ///< A View that displays progress toward completion. @SINCE_2_5.30
  RADIO_BUTTON,                  ///< A View that represents one choice from a mutually exclusive group. @SINCE_2_5.30
  SCROLL_BAR,                    ///< A View used to scroll content. @SINCE_2_5.30
  SPIN_BUTTON,                   ///< A View used to increment or decrement a value. @SINCE_2_5.30
  TAB,                           ///< A tab item that selects one page from a tab list. @SINCE_2_5.30
  TAB_LIST,                      ///< A View containing a set of tabs. @SINCE_2_5.30
  TEXT,                          ///< A View or region that primarily presents text. @SINCE_2_5.30
  TOGGLE_BUTTON,                 ///< A button that can remain in an on or off state. @SINCE_2_5.30
  TOOL_BAR,                      ///< A toolbar containing frequently used controls. @SINCE_2_5.30
  SCENE_3D,                      ///< A View that presents an interactive 3D scene. @SINCE_2_5.30
  MODEL,                         ///< A View that presents a model or model-like object. @SINCE_2_5.30
  SWITCH,                        ///< A switch that can be on or off. @SINCE_2_5.30
  MAX_COUNT                      ///< Sentinel value used to validate accessibility role values. @SINCE_2_5.30
};

/**
 * @brief Represents a semantic relation between accessible Views.
 *
 * Relations describe how one accessible View is connected to another, such
 * as a label describing a text entry or a popup belonging to a button.
 *
 * @SINCE_2_5.30
 */
enum class RelationType : uint32_t
{
  NULL_OF,          ///< No relation is specified. @SINCE_2_5.30
  LABEL_FOR,        ///< This View labels another View. @SINCE_2_5.30
  LABELLED_BY,      ///< This View is labelled by another View. @SINCE_2_5.30
  CONTROLLER_FOR,   ///< This View changes or controls another View. @SINCE_2_5.30
  CONTROLLED_BY,    ///< This View is changed or controlled by another View. @SINCE_2_5.30
  MEMBER_OF,        ///< This View is a member of a related group. @SINCE_2_5.30
  TOOLTIP_FOR,      ///< This View is a tooltip for another View. @SINCE_2_5.30
  NODE_CHILD_OF,    ///< This View is a node child of another View. @SINCE_2_5.30
  NODE_PARENT_OF,   ///< This View is a node parent of another View. @SINCE_2_5.30
  EXTENDED,         ///< This View has an implementation-specific extended relation. @SINCE_2_5.30
  FLOWS_TO,         ///< Reading or navigation flows from this View to another View. @SINCE_2_5.30
  FLOWS_FROM,       ///< Reading or navigation flows from another View to this View. @SINCE_2_5.30
  SUBWINDOW_OF,     ///< This View is a subwindow of another View. @SINCE_2_5.30
  EMBEDS,           ///< This View embeds another accessible View tree. @SINCE_2_5.30
  EMBEDDED_BY,      ///< This View is embedded by another accessible View tree. @SINCE_2_5.30
  POPUP_FOR,        ///< This View is a popup for another View. @SINCE_2_5.30
  PARENT_WINDOW_OF, ///< This View is the parent window of another View. @SINCE_2_5.30
  DESCRIPTION_FOR,  ///< This View provides a description for another View. @SINCE_2_5.30
  DESCRIBED_BY,     ///< This View is described by another View. @SINCE_2_5.30
  DETAILS,          ///< This View provides detailed information for another View. @SINCE_2_5.30
  DETAILS_FOR,      ///< This View has detailed information provided by another View. @SINCE_2_5.30
  ERROR_MESSAGE,    ///< This View provides an error message for another View. @SINCE_2_5.30
  ERROR_FOR,        ///< This View has an error described by another View. @SINCE_2_5.30
  MAX_COUNT         ///< Sentinel value used to validate accessibility relation values. @SINCE_2_5.30
};

} // namespace Accessibility
} // namespace Dali::Ui

#endif // DALI_UI_VIEW_ACCESSIBILITY_TYPES_H
