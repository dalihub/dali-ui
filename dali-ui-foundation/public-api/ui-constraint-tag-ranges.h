#ifndef DALI_UI_CONSTRAINT_TAG_RANGES_H
#define DALI_UI_CONSTRAINT_TAG_RANGES_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/constraint-tag-ranges.h>

namespace Dali::Ui
{
/**
 * @brief Enumeration for the start and end property ranges.
 */
enum ConstraintTagRanges
{
  UI_CONSTRAINT_TAG_START = Dali::ConstraintTagRanges::CORE_CONSTRAINT_TAG_MAX + 1u +
                            Dali::ConstraintTagRanges::INTERNAL_TAG_MAX_COUNT_PER_DERIVATION,
  UI_CONSTRAINT_TAG_END = UI_CONSTRAINT_TAG_START + Dali::ConstraintTagRanges::INTERNAL_TAG_MAX_COUNT_PER_DERIVATION,
};

} // namespace Dali::Ui

#endif // DALI_UI_CONSTRAINT_TAG_RANGES_H
