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
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/types/shadow.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
namespace Shadow
{

/**
 * @brief Creates a ColorVisual configured with a shadow value.
 *
 * @param[in] shadow The shadow value
 * @return A color visual that can be added as a shadow visual
 */
DALI_UI_API ColorVisual CreateVisual(const Ui::Shadow& shadow);

/**
 * @brief Creates a shadow from a property map.
 *
 * @param[in] map The color visual property map describing the shadow
 * @return A Shadow value, or Shadow::None() if the map is empty
 */
DALI_UI_API Ui::Shadow CreateShadow(const Property::Map& map);

/**
 * @brief Creates a property map for View::Property::SHADOW.
 *
 * @param[in] shadow The shadow value
 * @return A color visual property map describing the shadow
 */
DALI_UI_API Property::Map CreatePropertyMap(const Ui::Shadow& shadow);

} // namespace Shadow
} // namespace Extension
} // namespace Ui
} // namespace Dali
