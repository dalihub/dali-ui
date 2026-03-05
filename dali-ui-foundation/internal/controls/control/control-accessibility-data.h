#ifndef DALI_UI_CONTROL_ACCESSIBILITY_DATA_H
#define DALI_UI_CONTROL_ACCESSIBILITY_DATA_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
// private inner class
class Control::Impl::AccessibilityData : public ConnectionTracker
{
  friend class UI::DevelControl::ControlAccessible;

public:
  // Constructor
  AccessibilityData(Control& controlImpl);

  /**
   * @copydoc Dali::UI::Internal::Control::Impl::AppendAccessibilityAttribute()
   */
  void AppendAccessibilityAttribute(const std::string& key, const std::string value);

  /**
   * @copydoc Dali::UI::Internal::Control::Impl::RemoveAccessibilityAttribute()
   */
  void RemoveAccessibilityAttribute(const std::string& key);

  /**
   * @copydoc Dali::UI::Internal::Control::Impl::ClearAccessibilityAttributes()
   */
  void ClearAccessibilityAttributes();

  /**
   * @copydoc Dali::UI::Internal::Control::Impl::SetAccessibilityReadingInfoType()
   */
  void SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types);

  /**
   * @copydoc Dali::UI::Internal::Control::Impl::GetAccessibilityReadingInfoType()
   */
  Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType() const;

  /**
   * @brief Checks highlighted object geometry if it is showing or not
   */
  void CheckHighlightedObjectGeometry();

  /**
   * @brief Register property notification to check highlighted object position
   */
  void RegisterAccessibilityPositionPropertyNotification();

  /**
   * @brief Remove property notification added by RegisterAccessibilityPositionPropertyNotification
   */
  void UnregisterAccessibilityPositionPropertyNotification();

  /**
   * @brief Register PropertySet signal to check highlighted object name and description
   */
  void RegisterAccessibilityPropertySetSignal();

  /**
   * @brief Remove PropertySet signal added by RegisterAccessibilityPropertySetSignal
   */
  void UnregisterAccessibilityPropertySetSignal();

  /**
   * @brief Signal callback of PropertySet when this object is become highlighted, so
   * RegisterAccessibilityPropertySetSignal called.
   *
   * @param[in] handle Handle of the control.
   * @param[in] index The index of property.
   * @param[in] value The value of property.
   */
  void OnAccessibilityPropertySet(Dali::Handle& handle, Dali::Property::Index index,
                                  const Dali::Property::Value& value);

  /**
   * @copydoc Dali::UI::Internal::Control::Impl::GetAccessibleObject()
   */
  std::shared_ptr<UI::DevelControl::ControlAccessible> GetAccessibleObject();

public:
  /**
   * @brief Helper function to get default reading info type attributes
   */
  static Dali::Accessibility::ReadingInfoTypes GetDefaultReadingInfoTypes();

  /**
   * @brief Helper function to get control's default state attributes
   */
  static UI::DevelControl::AccessibilityStates GetDefaultControlAccessibilityStates();

public:
  UI::DevelControl::AccessibilityActivateSignalType mAccessibilityActivateSignal;
  UI::DevelControl::AccessibilityReadingSkippedSignalType mAccessibilityReadingSkippedSignal;
  UI::DevelControl::AccessibilityReadingPausedSignalType mAccessibilityReadingPausedSignal;
  UI::DevelControl::AccessibilityReadingResumedSignalType mAccessibilityReadingResumedSignal;
  UI::DevelControl::AccessibilityReadingCancelledSignalType mAccessibilityReadingCancelledSignal;
  UI::DevelControl::AccessibilityReadingStoppedSignalType mAccessibilityReadingStoppedSignal;

  UI::DevelControl::AccessibilityGetNameSignalType mAccessibilityGetNameSignal;
  UI::DevelControl::AccessibilityGetDescriptionSignalType mAccessibilityGetDescriptionSignal;
  UI::DevelControl::AccessibilityDoGestureSignalType mAccessibilityDoGestureSignal;

  UI::DevelControl::AccessibilityActionSignalType mAccessibilityActionSignal;
  UI::DevelControl::AccessibilityHighlightedSignalType mAccessibilityHighlightedSignal;

  struct AccessibilityProps
  {
    AccessibilityProps()
      : isHighlightable(TriStateProperty::AUTO),
        isHidden(false),
        isScrollable(false),
        isModal(false)
    {
    }

    AccessibilityProps(const AccessibilityProps&) = default;
    AccessibilityProps(AccessibilityProps&&) = default;

    std::string name{};
    std::string description{};
    std::string value{};
    std::string automationId{};

    DevelControl::AccessibilityStates states{};
    std::map<Dali::Accessibility::RelationType, std::set<Accessibility::Accessible*>> relations{};
    Property::Map extraAttributes{};

    TriStateProperty isHighlightable : 3;
    bool isHidden : 1;
    bool isScrollable : 1;
    bool isModal : 1;
  } mAccessibilityProps;

private:
  // Accessibility - notification for highlighted object to check if it is showing.
  Dali::PropertyNotification mAccessibilityPositionNotification;
  Dali::Accessibility::ScreenRelativeMoveType mAccessibilityLastScreenRelativeMoveType{
      Accessibility::ScreenRelativeMoveType::OUTSIDE};

  Control& mControlImpl;

  bool mIsAccessibilityPositionPropertyNotificationSet : 1;
  bool mIsAccessibilityPropertySetSignalRegistered : 1;
};
} // namespace Internal
} // namespace UI
} // namespace Dali
#endif // DALI_UI_CONTROL_ACCESSIBILITY_DATA_H
