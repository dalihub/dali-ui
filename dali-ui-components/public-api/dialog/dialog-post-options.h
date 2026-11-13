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
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API DialogPostOptions
{
public:
  /**
   * @brief Creates options with default values.
   * Animation is enabled, and the style and transition handles are empty.
   * @SINCE_3_0.0
   */
  DialogPostOptions();

  /// @brief Copy constructor.
  /// @SINCE_3_0.0
  DialogPostOptions(const DialogPostOptions& rhs);

  /// @brief Move constructor. The moved-from object must not be used except for assignment or destruction.
  /// @SINCE_3_0.0
  DialogPostOptions(DialogPostOptions&& rhs) noexcept;

  /// @brief Copy assignment.
  /// @SINCE_3_0.0
  DialogPostOptions& operator=(const DialogPostOptions& rhs);

  /// @brief Move assignment. The moved-from object must not be used except for assignment or destruction.
  /// @SINCE_3_0.0
  DialogPostOptions& operator=(DialogPostOptions&& rhs) noexcept;

  /// @brief Destructor.
  /// @SINCE_3_0.0
  ~DialogPostOptions();

  /// @brief Sets whether to use the Navigator's modal enter transition. The default is true.
  /// @SINCE_3_0.0
  void SetAnimated(bool animated);

  /// @brief Gets whether to use the Navigator's modal enter transition.
  /// @SINCE_3_0.0
  bool IsAnimated() const;

  /**
   * @brief Sets the style of the internally generated scrim; empty selects DialogContainerStyle::Default().
   * Use DialogContainerStyle::NoScrimPreset() to remove tint and blur while
   * retaining background input blocking and the Dialog's dismiss policy.
   * @SINCE_3_0.0
   */
  void SetContainerStyle(const DialogContainerStyle& style);

  /// @brief Gets the style of the internally generated scrim. May be an empty handle.
  /// @SINCE_3_0.0
  const DialogContainerStyle& GetContainerStyle() const;

  /**
   * @brief Sets an optional content transition; empty uses the Navigator's modal default.
   * Animator and snap callbacks receive the Dialog, not its DialogContainer.
   * Navigator independently fades the scrim on the same animation timeline.
   * @SINCE_3_0.0
   */
  void SetTransitionSpec(const NavigationTransitionSpec& spec);

  /// @brief Gets the content transition. May be an empty handle.
  /// @SINCE_3_0.0
  const NavigationTransitionSpec& GetTransitionSpec() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};
} // namespace Ui
} // namespace DALI_NAMESPACE
