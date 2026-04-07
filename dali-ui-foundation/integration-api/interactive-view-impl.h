#pragma once

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/interactive-event-receiver.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>
#include <dali-ui-foundation/public-api/interactive-view.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

using InteractiveViewImplPtr = IntrusivePtr<InteractiveViewImpl>;

/**
 * @brief Implementation class for InteractiveView.
 *
 * InteractiveViewImpl is a ViewImpl subclass that guarantees an InteractiveTrait
 * is attached for the lifetime of the view. Component implementations such as
 * ButtonImpl and CheckboxImpl should subclass this instead of ViewImpl.
 *
 * @see Dali::Ui::InteractiveView
 */
class DALI_UI_API InteractiveViewImpl : public ViewImpl, public IInteractiveEventReceiver
{
public:
  /**
   * @brief Creates a new InteractiveViewImpl.
   *
   * @return An intrusive pointer to a newly allocated InteractiveViewImpl
   */
  static InteractiveViewImplPtr New();

  /**
   * @copydoc ViewImpl::OnInitialize
   *
   * Attaches an InteractiveTrait to the view so that interactive behavior
   * is always available without an explicit AsInteractive() call.
   */
  void OnInitialize() override;

  /**
   * @brief Returns the cached InteractiveTrait attached to this view.
   *
   * Safe to call on both const and non-const contexts because the trait
   * is guaranteed to be attached and cached after OnInitialize().
   *
   * @return The InteractiveTrait handle
   */
  Ui::InteractiveTrait GetInteractiveTrait() const;

protected:
  /**
   * @brief Constructor.
   */
  InteractiveViewImpl();

  /**
   * @brief Destructor.
   */
  ~InteractiveViewImpl() override;

  // --- IInteractiveEventReceiver ---

  /**
   * @brief Called when the view is clicked.
   *
   * Override in subclasses to respond to click events.
   * The default implementation does nothing.
   *
   * @param[in] view  The view that was clicked
   * @param[in] event The input event that triggered the click
   */
  void OnClicked(Ui::View view, const Ui::InputEvent& event) override;

  /**
   * @brief Called when the pressed state changes.
   *
   * Override in subclasses to respond to press/release events.
   * The default implementation does nothing.
   *
   * @param[in] view    The view whose pressed state changed
   * @param[in] pressed True when pressed, false when released
   * @param[in] event   The input event that caused the state change
   */
  void OnPressedChanged(Ui::View view, bool pressed, const Ui::InputEvent& event) override;

  /**
   * @brief Called when a long press is detected on the view.
   *
   * Override in subclasses to handle long press. Return true to suppress
   * the subsequent click event; return false to allow it.
   * The default implementation returns false.
   *
   * @param[in] view  The view that received the long press
   * @param[in] event The input event that triggered the long press
   * @return True to suppress the subsequent click, false to allow it
   */
  bool OnLongPressed(Ui::View view, const Ui::InputEvent& event) override;

private:
  Ui::InteractiveTrait mCachedTrait; ///< Cached handle to the attached InteractiveTrait
};

// Helpers for forwarding methods

inline InteractiveViewImpl& GetImpl(Ui::InteractiveView& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<InteractiveViewImpl&>(handle);
}

inline const InteractiveViewImpl& GetImpl(const Ui::InteractiveView& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const InteractiveViewImpl&>(handle);
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
