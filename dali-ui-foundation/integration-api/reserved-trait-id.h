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

#include <dali-ui-foundation/public-api/trait-id.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Framework-reserved TraitId constants.
 *
 * These are allocated once at static initialisation time (in reserved-trait-id.cpp)
 * in a fixed order. All framework code references these by name, never by
 * raw numeric value, so the actual numbers are irrelevant.
 */
namespace ReservedTraitId
{

DALI_UI_API extern const TraitId CORE_INTERACTION_TRAITS;
DALI_UI_API extern const TraitId STATE_HANDLER_TRAIT;

DALI_UI_API extern const TraitId ABSOLUTE_LAYOUT_PARAMS;
DALI_UI_API extern const TraitId STACK_LAYOUT_PARAMS;
DALI_UI_API extern const TraitId GRID_LAYOUT_PARAMS;
DALI_UI_API extern const TraitId FLEX_LAYOUT_PARAMS;

DALI_UI_API extern const TraitId LAYOUT_MANAGER;
DALI_UI_API extern const TraitId LAYOUT_SIGNALS;

DALI_UI_API extern const TraitId STATE_EFFECT;
DALI_UI_API extern const TraitId STATE_EFFECT_TARGET;

} // namespace ReservedTraitId

} // namespace Integration
} // namespace Ui
} // namespace Dali
