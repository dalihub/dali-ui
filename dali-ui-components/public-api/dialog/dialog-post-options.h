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
enum class DialogDismissPolicy : uint32_t
{
  NONE           = 0u,
  BACK           = 1u,
  SCRIM          = 2u,
  BACK_AND_SCRIM = 3u
};

/// @brief The reason a managed presentation ended.
enum class DialogDismissReason : uint32_t
{
  PROGRAMMATIC = 0u,
  BACK         = 1u,
  SCRIM        = 2u,
  HOST_REMOVED = 3u
};

/**
 * @brief Options applied when a new Dialog presentation is registered.
 * Empty style and transition handles use the existing defaults. LayoutParams
 * on the Dialog determine placement exactly as in a manually built container.
 * Reposting to the same Navigator does not replace an active session's options.
 * For a custom scrim view or complex scrim composition, use a separate manual
 * DialogContainer instead; do not mix that ownership path with Post/Dismiss.
 */
struct DialogPostOptions
{
  /// Whether to use the Navigator's modal enter transition.
  bool animated{true};
  /**
   * @brief Style of the internally generated scrim; empty selects Default().
   * Use DialogContainerStyle::NoScrimPreset() to remove tint and blur while
   * retaining background input blocking and the Dialog's dismiss policy.
   */
  DialogContainerStyle containerStyle;
  /**
   * @brief Optional content transition; empty uses the Navigator's modal default.
   * Animator and snap callbacks receive the Dialog, not its DialogContainer.
   * Navigator independently fades the scrim on the same animation timeline.
   */
  NavigationTransitionSpec transitionSpec;
};
} // namespace Ui
} // namespace DALI_NAMESPACE
