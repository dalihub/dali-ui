#ifndef DALI_UI_INTEGRATION_API_VISUALS_COLOR_VISUAL_PROPERTIES_INTEG_H
#define DALI_UI_INTEGRATION_API_VISUALS_COLOR_VISUAL_PROPERTIES_INTEG_H

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
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
namespace ColorVisual
{
/**
 * @brief ColorVisual Properties.
 */
namespace Property
{
/**
 * @brief Enumeration for the start and end property ranges for this visual.
 */
enum
{
  MUTABLE_PROPERTY_START_INDEX = Ui::Integration::Visual::Property::MUTABLE_PROPERTY_END_INDEX + 1,
  MUTABLE_PROPERTY_END_INDEX   = MUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

  IMMUTABLE_PROPERTY_START_INDEX = Ui::Integration::Visual::Property::IMMUTABLE_PROPERTY_END_INDEX + 1,
  IMMUTABLE_PROPERTY_END_INDEX   = IMMUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

  READ_ONLY_PROPERTY_START_INDEX = Ui::Integration::Visual::Property::READ_ONLY_PROPERTY_END_INDEX + 1,
  READ_ONLY_PROPERTY_END_INDEX   = READ_ONLY_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

  /**
   * @brief Enumeration for the instance of properties belonging to the ColorVisual.
   */
  /**
   * @brief The blur radius of the visual.
   * @details Name "blurRadius", type Property::FLOAT, animatable.
   *          If the value is 0, the edge is sharp. Otherwise, the larger the value, the more the edge is blurred.
   * @note Optional.
   * @note The default is 0.
   * @note The visual size increases by the blur radius.
   * @note If squareness is not zero, the width of blur radius might not equal with it's real value.
   */
  BLUR_RADIUS = MUTABLE_PROPERTY_START_INDEX,

  /**
   * @brief Policy of cutout the color render result.
   * @details Name "cutoutPolicy", type Property::INTEGER.
   * @note Optional.
   * @note The default is CutoutPolicy::NONE.
   */
  CUTOUT_POLICY = IMMUTABLE_PROPERTY_START_INDEX,
};
} // namespace Property

} // namespace ColorVisual

} // namespace Integration
} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_INTEGRATION_API_VISUALS_COLOR_VISUAL_PROPERTIES_INTEG_H
