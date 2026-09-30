#ifndef DALI_UI_INTERNAL_VIEW_INTERNAL_DEPTH_SLOTS_H
#define DALI_UI_INTERNAL_VIEW_INTERNAL_DEPTH_SLOTS_H

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
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>

namespace DALI_NAMESPACE::Ui::Internal
{
// Half the gap between two adjacent DepthIndex::Ranges anchors, so that a layer splits evenly into
// a View half and an application half. See VisualBaseImpl::GetDepthIndex for what each half is for.
constexpr uint32_t MAXIMUM_VISUAL_OBJECTS_COUNT = (Dali::Ui::Integration::DepthIndex::Ranges::CONTENT - Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND) / 2;

/**
 * @brief Fixed depth-index slots of the View's own visuals inside each band's lower
 * (View-internal) half.
 *
 * Every View-internal visual or renderer that needs a fixed depth index MUST take its slot from
 * here, never from an inline calculation at the use site, so that collisions stay visible in one
 * place and are rejected at compile time.
 *
 * A visual that must render above everything in a band goes at the BOTTOM of the next band's
 * View-internal half; further slots in the same band stack upwards from the anchor. Never use
 * anchor - 1: that is the top of the application half below, and would tie with a full
 * container's last visual.
 */
namespace ViewInternalDepthIndex
{
constexpr int BACKGROUND_BLUR = Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND_EFFECT;     ///< BackgroundBlurEffect's output, below everything the View draws.
constexpr int INNER_SHADOW    = Dali::Ui::Integration::DepthIndex::Ranges::DECORATION;            ///< Above all CONTENT visuals.
constexpr int BORDERLINE      = Dali::Ui::Integration::DepthIndex::Ranges::FOREGROUND_EFFECT;     ///< Above all DECORATION visuals.
constexpr int FOREGROUND_BLUR = Dali::Ui::Integration::DepthIndex::Ranges::FOREGROUND_EFFECT + 1; ///< GaussianBlurEffect's output, above BORDERLINE.

// Slots sharing a band must be strictly ordered — an equality here is a collision.
static_assert(BORDERLINE < FOREGROUND_BLUR);

// No slot may leave its band's View-internal half [anchor, anchor + MAXIMUM_VISUAL_OBJECTS_COUNT).
static_assert(FOREGROUND_BLUR < Dali::Ui::Integration::DepthIndex::Ranges::FOREGROUND_EFFECT + static_cast<int>(MAXIMUM_VISUAL_OBJECTS_COUNT));
static_assert(INNER_SHADOW < Dali::Ui::Integration::DepthIndex::Ranges::DECORATION + static_cast<int>(MAXIMUM_VISUAL_OBJECTS_COUNT));
static_assert(BACKGROUND_BLUR < Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND_EFFECT + static_cast<int>(MAXIMUM_VISUAL_OBJECTS_COUNT));
} // namespace ViewInternalDepthIndex

} // namespace DALI_NAMESPACE::Ui::Internal

#endif // DALI_UI_INTERNAL_VIEW_INTERNAL_DEPTH_SLOTS_H
