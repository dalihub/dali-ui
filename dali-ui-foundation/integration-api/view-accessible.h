#ifndef DALI_UI_VIEW_ACCESSIBLE_H
#define DALI_UI_VIEW_ACCESSIBLE_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/accessibility-devel.h> // LCOV_EXCL_LINE
#include <dali/devel-api/adaptor-framework/actor-accessible.h>
#include <dali/devel-api/atspi-interfaces/action.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h> // LCOV_EXCL_LINE
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>  // LCOV_EXCL_LINE
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/accessibility-highlight-overlay.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view-accessibility-types.h>

namespace Dali::Ui
{

using AccessibilityStates = uint32_t; // LCOV_EXCL_LINE

/**
 * @brief Represents the Accessible object for Dali::Ui::View and derived classes
 *
 * You can create a derived class (and override View::CreateAccessibleObject)
 * in order to customize Accessibility for a given view.
 *
 * @see Dali::Accessibility::Accessible
 * @see Dali::Accessibility::Component
 * @see Dali::Accessibility::Collection
 * @see Dali::Accessibility::Action
 * @see Dali::Accessibility::Value
 * @see Dali::Accessibility::Text
 * @see Dali::Accessibility::EditableText
 */
class DALI_UI_API ViewAccessible : public Dali::Accessibility::ActorAccessible, public Dali::Accessibility::Action
{
protected:
  Vector2                       mLastPosition{0.0f, 0.0f};
  Dali::WeakHandle<Dali::Actor> mCurrentHighlightActor;

  void ScrollToSelf();

  /**
   * @brief Register property notification to check highlighted object position
   */
  void RegisterPositionPropertyNotification();

  /**
   * @brief Remove property notification added by RegisterPropertyNotification
   */
  void UnregisterPositionPropertyNotification();

  /**
   * @brief Registers PropertySet signal to notify when ACCESSIBILITY_NAME or ACCESSIBILITY_DESCRIPTION is changed.
   * Note that those two signals only need for highlighted view. So, let us ensure to connect PropertySet signal
   * only if view has been grabbed.
   */
  void RegisterPropertySetSignal();

  /**
   * @brief Unregisters PropertySet signal to notify when ACCESSIBILITY_NAME or ACCESSIBILITY_DESCRIPTION is changed.
   */
  void UnregisterPropertySetSignal();

  /**
   * @brief Check if the actor is showing
   * @return True if the actor is showing
   */
  bool IsShowing();

public:
  ViewAccessible(Dali::Actor self);

  /**
   * @copydoc Dali::Accessibility::Accessible::GetName()
   */
  std::string GetName() const override;

  /**
   * @brief Returns the actor's name in the absence of ACCESSIBILITY_NAME property
   * @return A pair containing the name and a boolean indicating if empty names are allowed
   */
  virtual std::pair<std::string, bool> GetNameRaw() const;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetDescription()
   */
  std::string GetDescription() const override;

  /**
   * @brief Returns the actor's description in the absence of ACCESSIBILITY_DESCRIPTION property
   */
  virtual std::string GetDescriptionRaw() const;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetValue()
   */
  std::string GetValue() const override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetRole()
   */
  Dali::Integration::Accessibility::Role GetRole() const override; // LCOV_EXCL_LINE

  /**
   * @copydoc Dali::Accessibility::Accessible::GetLocalizedRoleName()
   */
  std::string GetLocalizedRoleName() const override;

  /**
   * @copydoc Dali::Accessibility::Accessible::GetStates()
   */
  Dali::Integration::Accessibility::States GetStates() override; // LCOV_EXCL_LINE

  /**
   * @copydoc Dali::Accessibility::Accessible::GetAttributes()
   */
  Dali::Devel::Accessibility::Attributes GetAttributes() const override; // LCOV_EXCL_LINE

  /**
   * @copydoc Dali::Accessibility::Accessible::InitDefaultFeatures()
   */
  void InitDefaultFeatures() override;

  /**
   * @copydoc Dali::Accessibility::Accessible::IsHidden()
   */
  bool IsHidden() const override;

  /**
   * @copydoc Dali::Accessibility::Component::GrabFocus()
   */
  bool GrabFocus() override;

  /**
   * @copydoc Dali::Accessibility::Component::GrabHighlight()
   */
  bool GrabHighlight() override;

  /**
   * @copydoc Dali::Accessibility::Component::ClearHighlight()
   */
  bool ClearHighlight() override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionName()
   */
  std::string GetActionName(size_t index) const override;

  /**
   * @copydoc Dali::Accessibility::Action::GetLocalizedActionName()
   */
  std::string GetLocalizedActionName(size_t index) const override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionDescription()
   */
  std::string GetActionDescription(size_t index) const override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionCount()
   */
  size_t GetActionCount() const override;

  /**
   * @copydoc Dali::Accessibility::Action::GetActionKeyBinding()
   */
  std::string GetActionKeyBinding(size_t index) const override;

  /**
   * @copydoc Dali::Accessibility::Action::DoAction(std::size_t)
   */
  bool DoAction(size_t index) override;

  /**
   * @copydoc Dali::Accessibility::Action::DoAction(const std::string&)
   */
  bool DoAction(const std::string& name) override;

  /**
   * @copydoc Dali::Accessibility::Accessible::DoGesture()
   */
  bool DoGesture(const Dali::Devel::Accessibility::GestureInfo& gestureInfo) override; // LCOV_EXCL_LINE

  /**
   * @copydoc Dali::Accessibility::Accessible::GetRelationSet()
   */
  std::vector<Dali::Devel::Accessibility::Relation> GetRelationSet() override; // LCOV_EXCL_LINE

  /**
   * @copydoc Dali::Accessibility::Accessible::GetStringProperty()
   */
  std::string GetStringProperty(std::string propertyName) const override;

  /**
   * @copydoc Dali::Accessibility::Component::IsScrollable()
   */
  bool IsScrollable() const override;

  /**
   * @brief Function to set a custom highlight overlay
   *
   * This function sets a custom highlight overlay at the specified position and size.
   *
   * @param position A Vector2 representing the position of the overlay
   * @param size A Vector2 representing the size of the overlay
   */
  void SetCustomHighlightOverlay(Vector2 position, Vector2 size);

  /**
   * @brief Function to reset the custom highlight overlay
   *
   * This function resets the custom highlight overlay.
   */
  void ResetCustomHighlightOverlay();

  /**
   * @copydoc Dali::Accessibility::Accessible::GetStates()
   */
  virtual Dali::Integration::Accessibility::States CalculateStates(); // LCOV_EXCL_LINE

  /**
   * @brief Makes sure that a given child (descendant) of this container (e.g. ItemView) is visible
   * @return false if scrolling is not supported or child is already visible
   */
  virtual bool ScrollToChild(Actor child);

  /**
   * @brief Returns the index of the property that represents this actor's name
   */
  virtual Dali::Property::Index GetNamePropertyIndex();

  /**
   * @brief Returns the index of the property that represents this actor's description
   */
  virtual Dali::Property::Index GetDescriptionPropertyIndex();

  /**
   * @brief Sets last object position
   * @param[in] position Last object position
   */
  void SetLastPosition(Vector2 position);

  /**
   * @brief Gets last object position
   * @return The Last object position
   */
  Vector2 GetLastPosition() const;

  /**
   * @brief Handles AcessibilityState property change; Only called when the view is highlighted.
   */
  void OnStatePropertySet(AccessibilityStates newStates);

  /**
   * @brief Returns true if given actor is considered as modal by propeties set.
   */
  static bool IsModal(Actor actor);

  /**
   * @brief Returns true if given actor is considered as 3D scene view by propeties set.
   */
  static bool IsScene3D(Actor actor);

private:
  /**
   * @brief Appliys relavant accessibility properties to AT-SPI states.
   */
  void ApplyAccessibilityProps(Dali::Integration::Accessibility::States& states); // LCOV_EXCL_LINE

private:
  /**
   * @brief Grabs snapshot of previous state when the view is highlighted.
   */
  AccessibilityStates           mStatesSnapshot;
  AccessibilityHighlightOverlay mHighlightOverlay;
};

} // namespace Dali::Ui

#endif // DALI_UI_VIEW_ACCESSIBLE_H
