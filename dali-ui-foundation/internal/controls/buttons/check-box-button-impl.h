#ifndef DALI_UI_INTERNAL_CHECK_BOX_BUTTON_H
#define DALI_UI_INTERNAL_CHECK_BOX_BUTTON_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/controls/buttons/check-box-button.h>

#include "button-impl.h"

namespace Dali
{
namespace Ui
{
namespace Internal
{
/**
 * CheckBoxButton implementation class.
 *
 * \sa Dali::Ui::CheckBoxButton
 */
class CheckBoxButton : public Button
{
public:
  /**
   * Create a new CheckBoxButton.
   * @return A smart-pointer to the newly allocated CheckBoxButton.
   */
  static Dali::Ui::CheckBoxButton New();

private:
  /**
   * Construct a new CheckBoxButton.
   */
  CheckBoxButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CheckBoxButton();

private: // From Button
  /**
   * @copydoc Ui::Internal::Button::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Ui::Internal::Control::CreateAccessibleObject()
   */
  DevelControl::ControlAccessible* CreateAccessibleObject() override;

private:
  // Undefined
  CheckBoxButton(const CheckBoxButton&);

  // Undefined
  CheckBoxButton& operator=(const CheckBoxButton&);

protected:
  class CheckBoxButtonAccessible : public Button::ButtonAccessible
  {
  public:
    using Button::ButtonAccessible::ButtonAccessible;

    /**
     * @copydoc Dali::Ui::DevelControl::ControlAccessible::CalculateStates()
     */
    Dali::Accessibility::States CalculateStates() override;
  };

  void OnStateChange(State newState) override;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Ui::Internal::CheckBoxButton& GetImplementation(Ui::CheckBoxButton& button)
{
  DALI_ASSERT_ALWAYS(button);

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Ui::Internal::CheckBoxButton&>(handle);
}

inline const Ui::Internal::CheckBoxButton& GetImplementation(const Ui::CheckBoxButton& button)
{
  DALI_ASSERT_ALWAYS(button);

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Ui::Internal::CheckBoxButton&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_CHECK_BOX_BUTTON_H
