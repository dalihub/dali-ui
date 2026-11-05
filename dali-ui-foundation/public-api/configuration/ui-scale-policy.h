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

/**
 * @brief Controls how a view participates in the system-driven UiScale.
 *
 * Mirrors the semantics of LayoutDirection / RTL: each view can inherit
 * from its parent, force-apply the system scale, or opt out of it.
 *
 * The policy selects the SYSTEM-scale term of a view's effective scale only. The
 * per-window scale set through LayoutScaleManager::SetWindowScale() is an independent
 * multiplier on top of whichever term the policy selects, so it applies to DISABLED
 * views as well; the comments below name the term the policy chooses, not the
 * resulting effective scale. (Both terms are 1.0 while
 * LayoutScaleManager::SetScalable(false) is in force.)
 */
enum class UiScalePolicy : uint8_t
{
  INHERIT  = 0, ///< Inherit effective scale from parent; root takes the UI scale of LayoutScaleManager (default).
  ENABLED  = 1, ///< Always apply the current LayoutScaleManager UI scale, ignoring parent policy.
  DISABLED = 2, ///< Opt out of the system scale (its term is 1.0); descendant INHERIT views also opt out.
};

} // namespace Ui
} //namespace DALI_NAMESPACE
