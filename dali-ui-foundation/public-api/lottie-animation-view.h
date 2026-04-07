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

#include <dali-ui-foundation/public-api/view.h>
#include <dali/public-api/common/dali-string.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class LottieAnimationViewImpl;
}

/**
 * @brief LottieAnimationView is a View for displaying Lottie vector animations.
 *
 * It renders JSON-based Lottie animation files and supports playback control
 * (play, pause, stop) and loop count configuration.
 *
 * @code
 * LottieAnimationView view = LottieAnimationView::New("animation.json");
 * view.SetLoopCount(-1); // infinite
 * view.Play();
 * @endcode
 */
class DALI_UI_API LottieAnimationView : public View
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized LottieAnimationView handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  LottieAnimationView();

  /**
   * @brief Creates an initialized LottieAnimationView.
   *
   * @param[in] url The URL of the Lottie JSON file. Defaults to empty string.
   * @return A handle to a newly allocated Dali resource
   */
  static LottieAnimationView New(const Dali::String& url = "");

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~LottieAnimationView();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] rhs Handle to copy
   */
  LottieAnimationView(const LottieAnimationView& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  LottieAnimationView(LottieAnimationView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  LottieAnimationView& operator=(const LottieAnimationView& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  LottieAnimationView& operator=(LottieAnimationView&& rhs) noexcept;

public: // Static Methods
  /**
   * @brief Downcasts a handle to LottieAnimationView handle.
   *
   * If handle points to a LottieAnimationView, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a LottieAnimationView or an uninitialized handle
   */
  static LottieAnimationView DownCast(BaseHandle handle);

public: // Image
  /**
   * @brief Sets the resource URL of the Lottie animation file.
   *
   * @param[in] url The URL of the Lottie JSON file
   * @return Reference to this for fluent chaining
   */
  LottieAnimationView& SetResourceUrl(const Dali::String& url);

public: // Playback Control
  /**
   * @brief Starts or resumes playback of the animation.
   *
   * @return Reference to this for fluent chaining
   */
  LottieAnimationView& Play();

  /**
   * @brief Pauses playback of the animation.
   *
   * @return Reference to this for fluent chaining
   */
  LottieAnimationView& Pause();

  /**
   * @brief Stops playback of the animation and resets to the first frame.
   *
   * @return Reference to this for fluent chaining
   */
  LottieAnimationView& Stop();

  /**
   * @brief Sets the number of times the animation should loop.
   *
   * A value of -1 means infinite looping. A value of 0 means the animation
   * will not play. A positive value specifies an exact loop count.
   *
   * @param[in] count The loop count (-1 for infinite)
   * @return Reference to this for fluent chaining
   */
  LottieAnimationView& SetLoopCount(int count);

  /**
   * @brief Gets the current loop count.
   *
   * @return The loop count, or -1 for infinite looping
   */
  int GetLoopCount() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The LottieAnimationView implementation
   */
  explicit DALI_INTERNAL LottieAnimationView(Integration::LottieAnimationViewImpl& implementation);

  /**
   * @brief Allows the creation of this LottieAnimationView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL LottieAnimationView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  DALI_UI_CHAIN_VIEW_METHODS(LottieAnimationView)
};

} // namespace Ui
} // namespace Dali
