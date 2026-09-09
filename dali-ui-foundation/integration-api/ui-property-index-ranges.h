#ifndef DALI_INTEGRATION_UI_PROPERTY_INDEX_RANGES_H
#define DALI_INTEGRATION_UI_PROPERTY_INDEX_RANGES_H

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
namespace Integration
{
/**
 * @brief Enumeration for the start and end property ranges.
 */
enum PropertyRanges
{
  LAYOUT_GROUP_CHILD_PROPERTY_START_INDEX =
    CHILD_PROPERTY_REGISTRATION_START_INDEX + 1000, ///< Layout Group Property Start Index.
  LAYOUT_GROUP_CHILD_PROPERTY_END_INDEX =
    LAYOUT_GROUP_CHILD_PROPERTY_START_INDEX + 999, ///< Layout Group Property End Index.
  LINEAR_LAYOUT_CHILD_PROPERTY_START_INDEX =
    LAYOUT_GROUP_CHILD_PROPERTY_END_INDEX + 1, ///< Linear Layout Property Start Index.
  LINEAR_LAYOUT_CHILD_PROPERTY_END_INDEX =
    LINEAR_LAYOUT_CHILD_PROPERTY_START_INDEX + 999, ///< Linear Layout Property End Index.
  GRID_LAYOUT_CHILD_PROPERTY_START_INDEX =
    LINEAR_LAYOUT_CHILD_PROPERTY_END_INDEX + 1, ///< Grid Layout Property Start Index.
  GRID_LAYOUT_CHILD_PROPERTY_END_INDEX =
    GRID_LAYOUT_CHILD_PROPERTY_START_INDEX + 999, ///< Grid Layout Property End Index.
  FLEX_LAYOUT_CHILD_PROPERTY_START_INDEX =
    GRID_LAYOUT_CHILD_PROPERTY_END_INDEX + 1, ///< Flex Layout Property Start Index.
  FLEX_LAYOUT_CHILD_PROPERTY_END_INDEX =
    FLEX_LAYOUT_CHILD_PROPERTY_START_INDEX + 999, ///< Flex Layout Property End Index.
  ABSOLUTE_LAYOUT_CHILD_PROPERTY_START_INDEX =
    FLEX_LAYOUT_CHILD_PROPERTY_END_INDEX + 1, ///< Absolute Layout Property Start Index.
  ABSOLUTE_LAYOUT_CHILD_PROPERTY_END_INDEX =
    ABSOLUTE_LAYOUT_CHILD_PROPERTY_START_INDEX + 999, ///< Absolute Layout Property End Index.

  /**
   * @brief Enumeration for the start and end property ranges for VisualBase.
   */
  VISUAL_MUTABLE_PROPERTY_START_INDEX = Dali::PropertyRanges::CORE_PROPERTY_MAX_INDEX + 1,                                                          ///< Mutable Visual Property Start Index. (Could be changed without create new visual)
  VISUAL_MUTABLE_PROPERTY_END_INDEX   = VISUAL_MUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION * 10, ///< Mutable Visual Property End Index.

  VISUAL_IMMUTABLE_PROPERTY_START_INDEX = VISUAL_MUTABLE_PROPERTY_END_INDEX + 1,                                                                        ///< Immutable Visual Property Start Index. (Must create new visual)
  VISUAL_IMMUTABLE_PROPERTY_END_INDEX   = VISUAL_IMMUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION * 10, ///< Immutable Visual Property End Index.

  VISUAL_READ_ONLY_PROPERTY_START_INDEX = VISUAL_IMMUTABLE_PROPERTY_END_INDEX + 1,                                                                      ///< Read-only Visual Property Start Index.
  VISUAL_READ_ONLY_PROPERTY_END_INDEX   = VISUAL_READ_ONLY_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION * 10, ///< Read-only Visual Property End Index.
};

} // namespace Integration
} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_INTEGRATION_UI_PROPERTY_INDEX_RANGES_H
