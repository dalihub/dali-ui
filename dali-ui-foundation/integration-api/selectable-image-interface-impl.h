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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali-ui-foundation/public-api/views/view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Implementation-side base for a selectable image's behaviour.
 *
 * This is the implementation counterpart of the public Ui::SelectableImageInterface handle. It is a
 * plain Dali::BaseObject (NOT a scene actor), so a concrete image implementation composes the
 * actual drawing view (e.g. a Ui::LottieAnimationView member) rather than inheriting one.
 *
 * SetSelected() is a non-virtual template method that performs the shared "render this state"
 * contract and delegates the format-specific work to the protected virtual OnSelectedChanged()
 * hook, so a new image kind only reimplements OnSelectedChanged() plus the pure virtuals below.
 *
 * @see Dali::Ui::SelectableImageInterface
 */
class DALI_UI_API SelectableImageInterfaceImpl : public Dali::BaseObject
{
public:
  /**
   * @brief Renders the selected/deselected state without animating the transition.
   *
   * @param[in] selected True to render the selected state, false the deselected state
   */
  void SetSelected(bool selected)
  {
    SetSelected(selected, false);
  }

  /**
   * @brief Renders the selected/deselected state, optionally animating the transition.
   *
   * @param[in] selected True to render the selected state, false the deselected state
   * @param[in] animated True to animate the transition, false to snap to the target state
   */
  void SetSelected(bool selected, bool animated)
  {
    OnSelectedChanged(selected, animated);
  }

  /**
   * @copydoc Dali::Ui::SelectableImageInterface::GetView
   */
  virtual Ui::View GetView() const = 0;

  /**
   * @copydoc Dali::Ui::SelectableImageInterface::SetStateColors
   */
  virtual void SetStateColors(const Vector4& deselected, const Vector4& selected) = 0;

  /**
   * @copydoc Dali::Ui::SelectableImageInterface::IsTransitioning
   */
  virtual bool IsTransitioning() const = 0;

  /**
   * @copydoc Dali::Ui::SelectableImageInterface::TransitionFinishedSignal
   */
  virtual SelectableImageInterface::TransitionFinishedSignalType& TransitionFinishedSignal() = 0;

protected:
  /**
   * @brief Constructor.
   */
  SelectableImageInterfaceImpl() = default;

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~SelectableImageInterfaceImpl() override = default;

  /**
   * @brief Format-specific hook that renders the requested state.
   *
   * Invoked by SetSelected(); a concrete image kind performs its own drawing/playback here.
   *
   * @param[in] selected True to render the selected state, false the deselected state
   * @param[in] animated True to animate the transition, false to snap to the target state
   */
  virtual void OnSelectedChanged(bool selected, bool animated) = 0;
};

} // namespace Integration

inline Integration::SelectableImageInterfaceImpl& GetImpl(SelectableImageInterface& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::SelectableImageInterfaceImpl&>(handle);
}

inline const Integration::SelectableImageInterfaceImpl& GetImpl(const SelectableImageInterface& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::SelectableImageInterfaceImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
