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
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

/**
 * @brief Enumeration for cutout policy.
 *
 * A cutout discards part of what is drawn, leaving a transparent hole. The region
 * cut is the View's own rectangle, which is not the same as the area drawn over: a
 * blur radius or an extra size makes that area extend beyond the View. Cutting out
 * the view is how such an area is kept from being drawn underneath the View, as a
 * drop shadow is.
 *
 * @note When the two areas coincide, CUTOUT_VIEW discards everything.
 */
enum class CutoutPolicy
{
  NONE,                              ///< Draw the whole area (Default)
  CUTOUT_VIEW,                       ///< Discard the view's rectangle, ignoring its corner radius
  CUTOUT_VIEW_WITH_CORNER_RADIUS,    ///< Discard the view's rectangle, following its corner radius
  CUTOUT_OUTSIDE,                    ///< Discard everything outside the view's rectangle, ignoring its corner radius
  CUTOUT_OUTSIDE_WITH_CORNER_RADIUS, ///< Discard everything outside the view's rectangle, following its corner radius
};

} // namespace Ui
} // namespace DALI_NAMESPACE
