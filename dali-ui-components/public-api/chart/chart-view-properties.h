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
#include <dali/public-api/object/property-index-ranges.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Property index definitions for ChartView.
 *
 * Shared between ChartView::Property (public-api) and ChartViewImpl
 * (integration-api, for DALI_PROPERTY_REGISTRATION_EXTERNAL).
 */
struct ChartViewPropertyIndex
{
  enum PropertyRange
  {
    // +2001: intentional gap to avoid index collisions with other dali-ui-components controls
    PROPERTY_START_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 2001,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,
  };

  enum
  {
    SHOW_GRID = PROPERTY_START_INDEX,
    SHOW_LEGEND,
    SHOW_TOOLTIP,
    BACKGROUND_COLOR,
    GRID_COLOR,
    ANIMATION_DURATION,
    Y_AXIS_AUTO_RANGE,
    LEGEND_POSITION,
    AXIS_LABEL_SIZE,
    TITLE_SIZE,
    LINE_WIDTH,
    SHOW_MARKERS,
    MARKER_RADIUS,
    HOVER_ENABLED,
    TOUCH_ENABLED,
  };
};

} // namespace Ui
} // namespace Dali
