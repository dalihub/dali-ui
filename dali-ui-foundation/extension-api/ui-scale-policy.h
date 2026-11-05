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
#include <dali/public-api/common/dali-namespace.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Extension
{
/**
 * @brief Controls how a view participates in the system-driven UI scale.
 *
 * Mirrors the semantics of LayoutDirection / RTL: each view can inherit
 * from its parent, force-apply the system scale, or opt out of it.
 *
 * The policy selects the SYSTEM-scale term of a view's effective scale only. The
 * per-window scale set through Extension::LayoutScaleManager::SetWindowScale() is an
 * independent multiplier on top of whichever term the policy selects, so it applies to
 * DISABLED views as well; the comments below name the term the policy chooses, not the
 * resulting effective scale. (Both terms are 1.0 while
 * Extension::LayoutScaleManager::SetScalable(false) is in force.)
 *
 * A view's policy is set and read through Extension::View::SetUiScalePolicy() and
 * Extension::View::GetUiScalePolicy() (dali-ui-foundation/extension-api/view.h). Like the
 * rest of the scale configuration, this is an extension API for platform integrators and
 * module providers, and not part of the application-facing public API.
 *
 * It replaces the deprecated Ui::UiScalePolicy, which only the deprecated
 * Ui::View::SetUiScalePolicy(), Ui::View::GetUiScalePolicy() and their Ui::ViewImpl
 * counterparts take and return. The two are distinct types with the same enumerators and
 * values: the enumerators of the same name select the same policy, and static_cast converts
 * a value between them. Inside a ViewImpl subclass write Ui::Extension::UiScalePolicy: an
 * unqualified Extension:: names CustomActorImpl::Extension there.
 *
 * @code
 *   Extension::View::SetUiScalePolicy(panel, Extension::UiScalePolicy::DISABLED);
 * @endcode
 */
enum class UiScalePolicy : uint8_t
{
  INHERIT  = 0, ///< Inherit effective scale from parent; root takes the UI scale of Extension::LayoutScaleManager (default).
  ENABLED  = 1, ///< Always apply the current Extension::LayoutScaleManager UI scale, ignoring parent policy.
  DISABLED = 2, ///< Opt out of the system scale (its term is 1.0); descendant INHERIT views also opt out.
};

} // namespace Extension
} // namespace Ui
} //namespace DALI_NAMESPACE
