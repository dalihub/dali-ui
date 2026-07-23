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
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class SelectableLottieAnimationViewImpl;
}

/**
 * @brief A selectable image backed by a single Lottie animation.
 *
 * SelectableLottieAnimationView renders a selectable control's two visual states with one
 * Lottie glyph: it plays a "select" segment when told to render the selected state and a
 * "deselect" segment for the deselected state, and recolours the checked inner fill with the
 * pre-resolved state colours pushed by the owning component.
 *
 * It is a SelectableImageInterface (a behaviour handle), NOT a View: it COMPOSES a LottieAnimationView
 * internally and exposes it via GetView() so the owning control can place it into its tree.
 * The selection state itself is owned by the component; this image only renders the state it is
 * told to via SetSelected().
 *
 * @code
 * SelectableLottieAnimationView image = SelectableLottieAnimationView::New(
 *   SelectableLottieImage("checkbox.json",
 *                         SelectableLottieImage::FrameRange(0, 19),     // select segment
 *                         SelectableLottieImage::FrameRange(20, 38),    // deselect segment
 *                         "check_box .inner_fill.color"));             // inner-fill key path
 * Self().Add(image.GetView());
 * image.SetStateColors(deselectedRgba, selectedRgba);
 * image.SetSelected(true, true); // animate to selected
 * @endcode
 */
class DALI_UI_API SelectableLottieAnimationView : public SelectableImageInterface
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized SelectableLottieAnimationView handle.
   */
  SelectableLottieAnimationView() = default;

  /**
   * @brief Creates an initialized SelectableLottieAnimationView.
   *
   * @param[in] image The Lottie url bundled with the select/deselect frame ranges
   * @return A handle to a newly allocated Dali resource
   */
  static SelectableLottieAnimationView New(const SelectableLottieImage& image);

public: // Static Methods
  /**
   * @brief Downcasts a handle to SelectableLottieAnimationView handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a SelectableLottieAnimationView or an uninitialized handle
   */
  static SelectableLottieAnimationView DownCast(BaseHandle handle);

protected:
  /**
   * @brief Creates a SelectableLottieAnimationView handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit SelectableLottieAnimationView(Integration::SelectableLottieAnimationViewImpl* impl);
};

} // namespace Ui
} // namespace Dali
