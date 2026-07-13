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

#ifndef DALI_UI_FOUNDATION_I_SELECTABLE_IMAGE_H
#define DALI_UI_FOUNDATION_I_SELECTABLE_IMAGE_H

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class ISelectableImageImpl;
}

/**
 * @brief Behaviour handle for an image that renders a selectable control's two visual
 * states (selected / deselected) and the transition between them.
 *
 * A selectable control (CheckBox, RadioButton, Switch, ...) owns the selection state
 * itself; it only tells the image which state to render via SetSelected(). The concrete
 * image kind (Lottie today; possibly png/svg/gif later) decides how to render that state
 * and, crucially, which scene view actually draws it: the drawing view is obtained with
 * GetView() and composed into the control's tree, so an ISelectableImage is NOT itself a
 * View.
 *
 * This is a BaseHandle whose implementation (Integration::ISelectableImageImpl) carries the
 * behaviour. A concrete image handle (e.g. SelectableLottieAnimationView) derives from this
 * interface and creates the matching implementation.
 *
 * @code
 * ISelectableImage image = SelectableLottieAnimationView::New(...);
 * Ui::View view = image.GetView(); // the actual scene view to place
 * image.SetStateColors(deselectedRgba, selectedRgba);
 * image.SetSelected(true, true);   // animate to selected
 * @endcode
 */
class DALI_UI_API ISelectableImage : public BaseHandle
{
public:
  /// @brief Transition-finished signal type. Emitted when the state transition completes.
  using TransitionFinishedSignalType = Signal<void(View)>;

  /**
   * @brief Creates an uninitialized ISelectableImage handle.
   */
  ISelectableImage() = default;

  /**
   * @brief Downcasts a handle to ISelectableImage.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized ISelectableImage on success, otherwise empty
   */
  static ISelectableImage DownCast(BaseHandle handle);

  /**
   * @brief Returns the scene view that draws this image.
   *
   * The returned View is created and owned by the implementation; place it into the
   * control's tree (e.g. Self().Add(image.GetView())).
   *
   * @return The View that renders this image
   */
  Ui::View GetView() const;

  /**
   * @brief Renders the selected/deselected state without animating the transition.
   *
   * @param[in] selected True to render the selected state, false the deselected state
   */
  void SetSelected(bool selected);

  /**
   * @brief Renders the selected/deselected state, optionally animating the transition.
   *
   * @param[in] selected True to render the selected state, false the deselected state
   * @param[in] animated True to animate the transition, false to snap to the target state
   */
  void SetSelected(bool selected, bool animated);

  /**
   * @brief Sets the pre-resolved colours for the deselected and selected states.
   *
   * @param[in] deselected The RGBA colour for the deselected state
   * @param[in] selected   The RGBA colour for the selected state
   */
  void SetStateColors(const Vector4& deselected, const Vector4& selected);

  /**
   * @brief Returns whether a state transition is currently playing.
   *
   * @return True if the image is transitioning between states
   */
  bool IsTransitioning() const;

  /**
   * @brief Returns the signal emitted when a state transition finishes.
   *
   * @return A reference to the TransitionFinished signal
   */
  TransitionFinishedSignalType& TransitionFinishedSignal();

protected:
  /**
   * @brief Creates an ISelectableImage handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit ISelectableImage(Integration::ISelectableImageImpl* impl);
};

} // namespace Ui
} // namespace Dali

#endif // DALI_UI_FOUNDATION_I_SELECTABLE_IMAGE_H
