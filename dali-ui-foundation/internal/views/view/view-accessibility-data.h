#ifndef DALI_UI_VIEW_ACCESSIBILITY_DATA_H
#define DALI_UI_VIEW_ACCESSIBILITY_DATA_H

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
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
// private inner class
class ViewDataImpl::AccessibilityData : public ConnectionTracker
{
  friend class Ui::ViewAccessible;

public:
  /// @brief AccessibilityActivate signal type.
  typedef Signal<void()> AccessibilityActivateSignalType;

  /// @brief AccessibilityReadingSkipped signal type.
  typedef Signal<void()> AccessibilityReadingSkippedSignalType;

  /// @brief AccessibilityReadingPaused signal type.
  typedef Signal<void()> AccessibilityReadingPausedSignalType;

  /// @brief AccessibilityReadingResumed signal type.
  typedef Signal<void()> AccessibilityReadingResumedSignalType;

  /// @brief AccessibilityReadingCancelled signal type.
  typedef Signal<void()> AccessibilityReadingCancelledSignalType;

  /// @brief AccessibilityReadingStopped signal type.
  typedef Signal<void()> AccessibilityReadingStoppedSignalType;

  /// @brief AccessibilityGetName signal type.
  typedef Signal<void(Dali::String&)> AccessibilityGetNameSignalType;

  /// @brief AccessibilityGetDescription signal type.
  typedef Signal<void(Dali::String&)> AccessibilityGetDescriptionSignalType;

  /// @brief AccessibilityDoGesture signal type.
  typedef Signal<void(std::pair<Dali::Accessibility::GestureInfo, bool>&)> AccessibilityDoGestureSignalType;

  /// @brief AccessibilityAction signal type.
  typedef Signal<bool(const Dali::Accessibility::ActionInfo&)> AccessibilityActionSignalType;

  /// @brief AccessibilityHighlighted signal type.
  /// @param bool highlighted true if View is highlighted, false if highlight is removed.
  typedef Signal<void(bool)> AccessibilityHighlightedSignalType;

  // Constructor
  AccessibilityData(ViewImpl& viewImpl);

  /**
   * @copydoc Dali::Ui::ViewImpl::Impl::AppendAccessibilityAttribute()
   */
  void AppendAccessibilityAttribute(const Dali::String& key, const Dali::String& value);

  /**
   * @copydoc Dali::Ui::ViewImpl::Impl::RemoveAccessibilityAttribute()
   */
  void RemoveAccessibilityAttribute(const Dali::String& key);

  /**
   * @copydoc Dali::Ui::ViewImpl::Impl::ClearAccessibilityAttributes()
   */
  void ClearAccessibilityAttributes();

  /**
   * @copydoc Dali::Ui::ViewImpl::Impl::SetAccessibilityReadingInfoType()
   */
  void SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types);

  /**
   * @copydoc Dali::Ui::ViewImpl::Impl::GetAccessibilityReadingInfoType()
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
   * @param[in] handle Handle of the view.
   * @param[in] index The index of property.
   * @param[in] value The value of property.
   */
  void OnAccessibilityPropertySet(Dali::Handle handle, Dali::Property::Index index,
                                  const Dali::Property::Value& value);

  /**
   * @copydoc Dali::Ui::ViewImpl::Impl::GetAccessibleObject()
   */
  std::shared_ptr<Ui::ViewAccessible> GetAccessibleObject();

public:
  /**
   * @brief Helper function to get default reading info type attributes
   */
  static Dali::Accessibility::ReadingInfoTypes GetDefaultReadingInfoTypes();

  /**
   * @brief Helper function to get view's default state attributes
   */
  static AccessibilityStates GetDefaultViewAccessibilityStates();

public:
  AccessibilityActivateSignalType         mAccessibilityActivateSignal;
  AccessibilityReadingSkippedSignalType   mAccessibilityReadingSkippedSignal;
  AccessibilityReadingPausedSignalType    mAccessibilityReadingPausedSignal;
  AccessibilityReadingResumedSignalType   mAccessibilityReadingResumedSignal;
  AccessibilityReadingCancelledSignalType mAccessibilityReadingCancelledSignal;
  AccessibilityReadingStoppedSignalType   mAccessibilityReadingStoppedSignal;

  AccessibilityGetNameSignalType        mAccessibilityGetNameSignal;
  AccessibilityGetDescriptionSignalType mAccessibilityGetDescriptionSignal;
  AccessibilityDoGestureSignalType      mAccessibilityDoGestureSignal;

  AccessibilityActionSignalType      mAccessibilityActionSignal;
  AccessibilityHighlightedSignalType mAccessibilityHighlightedSignal;

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
    AccessibilityProps(AccessibilityProps&&)      = default;

    std::string name{};
    std::string description{};
    std::string value{};
    std::string automationId{};

    AccessibilityStates                                                               states{};
    std::map<Dali::Accessibility::RelationType, std::set<Accessibility::Accessible*>> relations{};
    Property::Map                                                                     extraAttributes{};

    TriStateProperty isHighlightable : 3;
    bool             isHidden : 1;
    bool             isScrollable : 1;
    bool             isModal : 1;
  } mAccessibilityProps;

private:
  // Accessibility - notification for highlighted object to check if it is showing.
  Dali::PropertyNotification                  mAccessibilityPositionNotification;
  Dali::Accessibility::ScreenRelativeMoveType mAccessibilityLastScreenRelativeMoveType{
    Accessibility::ScreenRelativeMoveType::OUTSIDE};

  ViewImpl& mViewImpl;

  bool mIsAccessibilityPositionPropertyNotificationSet : 1;
  bool mIsAccessibilityPropertySetSignalRegistered : 1;
};
} // namespace Internal
} // namespace Ui
} // namespace Dali
#endif // DALI_UI_VIEW_ACCESSIBILITY_DATA_H
