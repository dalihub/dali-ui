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

#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/traits/interactive-trait.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{

namespace Ui
{
namespace Provider
{

class InteractiveViewImpl;

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
class DALI_UI_API InteractiveViewImpl : public ViewImpl
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
   * @copydoc InteractiveView::PressedChangedSignal()
   */
  Signal<void(View, bool, InputEvent)>& PressedChangedSignal();

  /**
   * @copydoc InteractiveView::PseudoDisabledChangedSignal()
   */
  Signal<void(View, bool)>& PseudoDisabledChangedSignal();

  /**
   * @copydoc InteractiveView::ClickedSignal()
   */
  Signal<void(View, InputEvent)>& ClickedSignal();

  /**
   * @copydoc InteractiveView::LongPressedSignal()
   */
  Signal<bool(View, InputEvent)>& LongPressedSignal();

  /**
   * @copydoc InteractiveView::IsPressed()
   */
  bool IsPressed() const;

  /**
   * @copydoc InteractiveView::IsPseudoDisabled()
   */
  bool IsPseudoDisabled() const;

  /**
   * @copydoc InteractiveView::SetPseudoDisabled()
   */
  void SetPseudoDisabled(bool pseudoDisabled);

  /**
   * @copydoc InteractiveView::IsClickable()
   */
  bool IsClickable() const;

  /**
   * @copydoc InteractiveView::SetClickable()
   */
  void SetClickable(bool clickable);

  /**
   * @copydoc InteractiveView::GetKeyClickPolicy()
   */
  KeyClickPolicy GetKeyClickPolicy() const;

  /**
   * @copydoc InteractiveView::SetKeyClickPolicy()
   */
  void SetKeyClickPolicy(KeyClickPolicy policy);

protected:
  /**
   * @brief Gets the guaranteed InteractiveTrait.
   *
   * @return The InteractiveTrait attached to this view
   */
  InteractiveTrait GetInteractiveTrait() const;

  /**
   * @brief Constructor.
   */
  InteractiveViewImpl();

  /**
   * @brief Destructor.
   */
  ~InteractiveViewImpl() override;
};

// Helpers for forwarding methods

inline DALI_UI_API InteractiveViewImpl& GetImpl(InteractiveView& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<InteractiveViewImpl&>(handle);
}

inline DALI_UI_API const InteractiveViewImpl& GetImpl(const InteractiveView& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const InteractiveViewImpl&>(handle);
}

} // namespace Provider
} // namespace Ui

} // namespace Dali
