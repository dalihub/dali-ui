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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/int-pair.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

/**
 * @brief A Lottie image for a selectable control, bundling the animation url with
 * the explicit integer frame ranges that play on select and on deselect.
 *
 * A selectable control (CheckBox, RadioButton, Switch, ...) drives a single Lottie
 * glyph by playing a "select" segment when it becomes selected and a "deselect"
 * segment when it becomes deselected. Those two segments are inseparable from the
 * url: a url without both ranges is meaningless. SelectableLottieImage therefore has
 * a single construction path that takes the url AND both ranges together — there is
 * no way to register a url without also supplying both frame ranges.
 *
 * The frame ranges are EXPLICIT integer frame numbers (not marker names); they map
 * directly onto LottieAnimationView::SetPlaybackRange() / JumpToFrame().
 * Each Int32Pair represents an inclusive [GetStart(), GetEnd()] segment. Range
 * normalization and clamping are performed by the consuming animation view.
 *
 * SelectableLottieImage is copyable, immutable value data consumed when creating a
 * SelectableLottieAnimationView. The view copies the description data it needs and
 * does not retain this object.
 *
 * A moved-from image is invalid and must only be destroyed or assigned a new value.
 *
 * @code
 *   // url + select segment [0,19] + deselect segment [20,38] + inner-fill recolour key path
 *   SelectableLottieImage image("checkbox.json",
 *                               Int32Pair(0, 19),
 *                               Int32Pair(20, 38),
 *                               "check_box .inner_fill.color");
 * @endcode
 */
class DALI_UI_API SelectableLottieImage
{
public:
  /**
   * @brief Creates a SelectableLottieImage from a url and both frame ranges.
   *
   * This is the ONLY construction path: a url can never be registered without both
   * the select and the deselect frame ranges.
   *
   * @param[in] url              The Lottie animation url
   * @param[in] selectionPlaybackRange The frame segment played when the control becomes selected
   * @param[in] deselectionPlaybackRange The frame segment played when the control becomes deselected
   * @param[in] innerFillKeyPath The asset key path of the inner fill recoloured with the state
   *                             colours; an empty string keeps the renderer's generic default
   */
  SelectableLottieImage(const Dali::String& url,
                        const Int32Pair&    selectionPlaybackRange,
                        const Int32Pair&    deselectionPlaybackRange,
                        const Dali::String& innerFillKeyPath = Dali::String());

  /**
   * @brief Deleted default constructor: a url is inseparable from both frame ranges.
   */
  SelectableLottieImage() = delete;

  /** @brief Copies the image description. */
  SelectableLottieImage(const SelectableLottieImage& rhs);

  /** @brief Transfers ownership of the image description. */
  SelectableLottieImage(SelectableLottieImage&& rhs) noexcept;

  /** @brief Copies the image description. */
  SelectableLottieImage& operator=(const SelectableLottieImage& rhs);

  /** @brief Transfers ownership of the image description. */
  SelectableLottieImage& operator=(SelectableLottieImage&& rhs) noexcept;

  /** @brief Destroys the image description. */
  ~SelectableLottieImage();

  /**
   * @brief Returns the Lottie animation url.
   *
   * @return The url
   */
  const Dali::String& GetUrl() const;

  /**
   * @brief Returns the frame segment played when the control becomes selected.
   *
   * @return The select frame range
   * @deprecated Use GetSelectionPlaybackRange() instead.
   */
  Int32Pair GetSelectRange() const;

  /**
   * @brief Returns the frame segment played when the control becomes deselected.
   *
   * @return The deselect frame range
   * @deprecated Use GetDeselectionPlaybackRange() instead.
   */
  Int32Pair GetDeselectRange() const;

  /**
   * @brief Returns the asset key path of the inner fill recoloured with the state colours.
   *
   * @return The inner-fill key path (empty when the renderer's generic default should apply)
   */
  const Dali::String& GetInnerFillKeyPath() const;

public: // Playback ranges
  /**
   * @brief Returns the frame segment played when the control becomes selected.
   *
   * @return The select frame range
   */
  Int32Pair GetSelectionPlaybackRange() const;

  /**
   * @brief Returns the frame segment played when the control becomes deselected.
   *
   * @return The deselect frame range
   */
  Int32Pair GetDeselectionPlaybackRange() const;

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Ui
} //namespace DALI_NAMESPACE
