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

namespace Dali
{
namespace Ui
{
/**
 * @addtogroup dali_ui_view
 * @{
 */

/**
 * @brief Enumeration for the start and end property ranges.
 */
enum PropertyRanges
{
  /**
   * @brief Enumeration for the start and end property ranges for View.
   */
  VIEW_PROPERTY_START_INDEX = Dali::PropertyRanges::PROPERTY_REGISTRATION_START_INDEX,                                     ///< Start index of View properties.
  VIEW_PROPERTY_END_INDEX   = VIEW_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION, ///< Reserving 1000 property indices.

  VIEW_ANIMATABLE_PROPERTY_START_INDEX = Dali::PropertyRanges::ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,                                     ///< Start index of View animatable properties without uniforms.
  VIEW_ANIMATABLE_PROPERTY_END_INDEX   = VIEW_ANIMATABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION, ///< Reserving 1000 property indices.

  VIEW_ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_START_INDEX = Dali::PropertyRanges::ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX,                                     ///< Start index of View animatable properties without uniforms.
  VIEW_ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_END_INDEX   = VIEW_ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION, ///< Reserving 1000 property indices.

  /**
   * @brief Enumeration for the start and end property ranges for VisualBase.
   */
  VISUAL_MUTABLE_PROPERTY_START_INDEX = Dali::PropertyRanges::CORE_PROPERTY_MAX_INDEX + 1,                                                          ///< Mutable Visual Property Start Index. (Could be changed without create new visual)
  VISUAL_MUTABLE_PROPERTY_END_INDEX   = VISUAL_MUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION * 10, ///< Mutable Visual Property End Index.

  VISUAL_IMMUTABLE_PROPERTY_START_INDEX = VISUAL_MUTABLE_PROPERTY_END_INDEX + 1,                                                                        ///< Immutable Visual Property Start Index. (Must create new visual)
  VISUAL_IMMUTABLE_PROPERTY_END_INDEX   = VISUAL_IMMUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION * 10, ///< Immutable Visual Property End Index.

  VISUAL_READ_ONLY_PROPERTY_START_INDEX = VISUAL_IMMUTABLE_PROPERTY_END_INDEX + 1,                                                                      ///< Read-only Visual Property Start Index.
  VISUAL_READ_ONLY_PROPERTY_END_INDEX   = VISUAL_READ_ONLY_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION * 10, ///< Read-only Visual Property End Index.

  UI_FOUNDATION_PROPERTY_MAX = Dali::PropertyRanges::CORE_PROPERTY_MAX_INDEX + 10000000, ///< The maximum index that ui-foundation properties can go up to
};

/**
 * @}
 */
} // namespace Ui

} // namespace Dali

#endif // DALI_UI_PROPERTY_INDEX_RANGES_H
