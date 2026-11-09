#ifndef DALI_UI_PROPERTY_INDEX_RANGES_H
#define DALI_UI_PROPERTY_INDEX_RANGES_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @addtogroup dali_ui_view
 * @{
 */

/**
 * @brief Enumeration for the start and end property ranges.
 * @SINCE_3_0.0
 */
enum PropertyRanges
{
  /**
   * @brief Enumeration for the start and end property ranges for View.
   * @SINCE_3_0.0
   */
  VIEW_PROPERTY_START_INDEX = Dali::PropertyRanges::PROPERTY_REGISTRATION_START_INDEX,                                     ///< Start index of View properties. @SINCE_3_0.0
  VIEW_PROPERTY_END_INDEX   = VIEW_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION, ///< Reserving 1000 property indices. @SINCE_3_0.0

  VIEW_ANIMATABLE_PROPERTY_START_INDEX = Dali::PropertyRanges::ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,                                     ///< Start index of View animatable properties without uniforms. @SINCE_3_0.0
  VIEW_ANIMATABLE_PROPERTY_END_INDEX   = VIEW_ANIMATABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION, ///< Reserving 1000 property indices. @SINCE_3_0.0

  VIEW_ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_START_INDEX = Dali::PropertyRanges::ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX,                                     ///< Start index of View animatable properties without uniforms. @SINCE_3_0.0
  VIEW_ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_END_INDEX   = VIEW_ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION, ///< Reserving 1000 property indices. @SINCE_3_0.0

  UI_FOUNDATION_PROPERTY_MAX = Dali::PropertyRanges::CORE_PROPERTY_MAX_INDEX + 10000000, ///< The maximum index that ui-foundation properties can go up to @SINCE_3_0.0
};

/**
 * @}
 */
} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_PROPERTY_INDEX_RANGES_H
