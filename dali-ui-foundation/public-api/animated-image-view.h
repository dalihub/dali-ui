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

#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class AnimatedImageViewImpl;
}

/**
 * @brief AnimatedImageView is a View for displaying animated image resources such as GIF files.
 *
 * It supports playback control (play, pause, stop) and loop count configuration.
 * The animation starts automatically when Play() is called after the image is loaded.
 *
 * @code
 * AnimatedImageView view = AnimatedImageView::New("animation.gif");
 * view.SetLoopCount(3);
 * view.Play();
 * @endcode
 */
class DALI_UI_API AnimatedImageView : public View
{
public:
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized AnimatedImageView handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  AnimatedImageView();

  /**
   * @brief Creates an initialized AnimatedImageView.
   *
   * @param[in] url The URL of the animated image resource. Defaults to empty string.
   * @return A handle to a newly allocated Dali resource
   */
  static AnimatedImageView New(const Dali::String& url = "");

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AnimatedImageView();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] rhs Handle to copy
   */
  AnimatedImageView(const AnimatedImageView& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  AnimatedImageView(AnimatedImageView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  AnimatedImageView& operator=(const AnimatedImageView& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  AnimatedImageView& operator=(AnimatedImageView&& rhs) noexcept;

public: // Static Methods
  /**
   * @brief Downcasts a handle to AnimatedImageView handle.
   *
   * If handle points to an AnimatedImageView, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to an AnimatedImageView or an uninitialized handle
   */
  static AnimatedImageView DownCast(BaseHandle handle);

public: // Image
  /**
   * @brief Sets the resource URL of the animated image to display.
   *
   * @param[in] url The URL of the animated image resource (e.g. GIF)
   * @return Reference to this for fluent chaining
   */
  AnimatedImageView& SetResourceUrl(const Dali::String& url);

  /**
   * @brief Gets the current resource URL.
   *
   * @return The URL of the animated image currently set on this view
   */
  Dali::String GetResourceUrl() const;

  /**
   * @brief Sets the color multiplier applied to the image.
   *
   * @param[in] color The RGBA color to multiply with the image
   * @return Reference to this for fluent chaining
   */
  AnimatedImageView& SetImageColor(const UiColor& color);

  /**
   * @brief Gets the color multiplier applied to the image.
   *
   * @return The current image color
   */
  UiColor GetImageColor() const;

public: // Playback Control
  /**
   * @brief Starts or resumes playback of the animation.
   *
   * @return Reference to this for fluent chaining
   */
  AnimatedImageView& Play();

  /**
   * @brief Pauses playback of the animation.
   *
   * @return Reference to this for fluent chaining
   */
  AnimatedImageView& Pause();

  /**
   * @brief Stops playback of the animation and resets to the first frame.
   *
   * @return Reference to this for fluent chaining
   */
  AnimatedImageView& Stop();

  /**
   * @brief Sets the number of times the animation should loop.
   *
   * A value of -1 means infinite looping. A value of 0 means the animation
   * will not play. A positive value specifies an exact loop count.
   *
   * @param[in] count The loop count (-1 for infinite)
   * @return Reference to this for fluent chaining
   */
  AnimatedImageView& SetLoopCount(int count);

  /**
   * @brief Gets the current loop count.
   *
   * @return The loop count, or -1 for infinite looping
   */
  int GetLoopCount() const;

public: // Loading Status & Signals
  /**
   * @brief Gets the current loading status of the image.
   *
   * @return The current ResourceStatus of the image visual
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  /**
   * @brief Returns the signal emitted when the image has finished loading and is ready to display.
   *
   * @return A reference to the ResourceReady signal
   */
  ResourceReadySignalType& ResourceReadySignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The AnimatedImageView implementation
   */
  explicit DALI_INTERNAL AnimatedImageView(Integration::AnimatedImageViewImpl& implementation);

  /**
   * @brief Allows the creation of this AnimatedImageView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL AnimatedImageView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  DALI_UI_CHAIN_VIEW_METHODS(AnimatedImageView)
};

} // namespace Ui
} // namespace Dali
