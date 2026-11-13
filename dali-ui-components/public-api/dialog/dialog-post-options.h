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
 */

#include <dali-ui-components/public-api/navigator/navigation-transition-spec.h>
#include <dali-ui-components/public-api/styles/dialog-container-style.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
/// @brief Allowed user dismiss requests. Explicit removal bypasses this policy.
/// @SINCE_3_0.0
enum class DialogDismissPolicy : uint32_t
{
  NONE           = 0u,
  BACK           = 1u,
  SCRIM          = 2u,
  BACK_AND_SCRIM = 3u
};

/// @brief The reason a managed presentation ended.
/// @SINCE_3_0.0
enum class DialogDismissReason : uint32_t
{
  PROGRAMMATIC = 0u,
  BACK         = 1u,
  SCRIM        = 2u,
  HOST_REMOVED = 3u
};

/**
 * @brief Options applied when a new Dialog presentation is registered.
 *
 * This is a value-type object. Dialog::Post() reads the values at the time of the call,
 * so changing the object afterwards does not affect an already posted Dialog.
 * Empty style and transition handles use the existing defaults. LayoutParams
 * on the Dialog determine placement exactly as in a manually built container.
 * Reposting to the same Navigator does not replace an active session's options.
 * For a custom scrim view or complex scrim composition, use a separate manual
 * DialogContainer instead; do not mix that ownership path with Post/Dismiss.
 * @SINCE_3_0.1
 */
class DALI_UI_COMPONENTS_API DialogPostOptions
{
public:
  /**
   * @brief Creates options with default values.
   *
   * Animation is enabled, and the style and transition handles are empty.
   * @SINCE_3_0.1
   */
  DialogPostOptions();

  /**
   * @brief Creates options by copying other options.
   *
   * @SINCE_3_0.1
   * @param[in] rhs The options to copy.
   */
  DialogPostOptions(const DialogPostOptions& rhs);

  /**
   * @brief Creates options by moving other options.
   *
   * The moved-from object must not be used except for assignment or destruction.
   *
   * @SINCE_3_0.1
   * @param[in] rhs The options to move.
   */
  DialogPostOptions(DialogPostOptions&& rhs) noexcept;

  /**
   * @brief Copies other options to this object.
   *
   * @SINCE_3_0.1
   * @param[in] rhs The options to copy.
   */
  DialogPostOptions& operator=(const DialogPostOptions& rhs);

  /**
   * @brief Moves other options to this object.
   *
   * The moved-from object must not be used except for assignment or destruction.
   *
   * @SINCE_3_0.1
   * @param[in] rhs The options to move.
   */
  DialogPostOptions& operator=(DialogPostOptions&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.1
   */
  ~DialogPostOptions();

  /**
   * @brief Sets whether to use the Navigator's modal enter transition.
   *
   * The default is true.
   *
   * @SINCE_3_0.1
   * @param[in] animated True to use the modal enter transition.
   */
  void SetAnimated(bool animated);

  /**
   * @brief Returns whether to use the Navigator's modal enter transition.
   *
   * @SINCE_3_0.1
   * @return True if the modal enter transition is used.
   */
  bool IsAnimated() const;

  /**
   * @brief Sets the style of the internally generated scrim.
   *
   * An empty style selects DialogContainerStyle::Default().
   * Use DialogContainerStyle::NoScrimPreset() to remove tint and blur while
   * retaining background input blocking and the Dialog's dismiss policy.
   *
   * @SINCE_3_0.1
   * @param[in] style The scrim style.
   */
  void SetContainerStyle(const DialogContainerStyle& style);

  /**
   * @brief Returns the style of the internally generated scrim.
   *
   * @SINCE_3_0.1
   * @return The scrim style. It may be an empty handle.
   */
  const DialogContainerStyle& GetContainerStyle() const;

  /**
   * @brief Sets an optional content transition.
   *
   * An empty specification uses the Navigator's modal default.
   * Animator and snap callbacks receive the Dialog, not its DialogContainer.
   * Navigator independently fades the scrim on the same animation timeline.
   *
   * @SINCE_3_0.1
   * @param[in] spec The content transition specification.
   */
  void SetTransitionSpec(const NavigationTransitionSpec& spec);

  /**
   * @brief Returns the content transition.
   *
   * @SINCE_3_0.1
   * @return The content transition specification. It may be an empty handle.
   */
  const NavigationTransitionSpec& GetTransitionSpec() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};
} // namespace Ui
} // namespace DALI_NAMESPACE
