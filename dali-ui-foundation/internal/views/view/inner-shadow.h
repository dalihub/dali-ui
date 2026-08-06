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
#include <dali-ui-foundation/public-api/types/inner-shadow.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace InnerShadow
{

/**
 * @brief Creates a ColorVisual configured with an inner shadow value.
 *
 * @param[in] innerShadow The inner shadow value
 * @return A color visual that can be registered as an inner shadow visual
 */
ColorVisual CreateVisual(const Ui::InnerShadow& innerShadow);

/**
 * @brief Creates a property map for View::Property::INNER_SHADOW.
 *
 * @param[in] innerShadow The inner shadow value
 * @return A color visual property map describing the inner shadow
 */
Property::Map CreatePropertyMap(const Ui::InnerShadow& innerShadow);

} // namespace InnerShadow
} // namespace Internal
} // namespace Ui
} // namespace Dali
