#ifndef DALI_UI_NPATCH_HELPER_H
#define DALI_UI_NPATCH_HELPER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/public-api/math/int-pair.h>
#include <dali/public-api/rendering/geometry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/utility/npatch-utilities.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class NPatchData;
}

namespace Integration
{
namespace NPatchHelper
{
/**
 * @brief Creates a Npatch Geometry object
 *
 * @param[in] gridSize The gridSize for creating a geometry
 * @return The Geometry for NPatch
 */
DALI_UI_API Geometry CreateGridGeometry(Uint16Pair gridSize);

/**
 * @brief Creates a geometry with the border only for the grid size to be used by this visuals' shaders
 * e.g. a 5x4 grid would create a geometry that would look like:
 *
 *   ---------------------
 *   |  /|  /|  /|  /|  /|
 *   |/  |/  |/  |/  |/  |
 *   ---------------------
 *   |  /|           |  /|
 *   |/  |           |/  |
 *   -----           -----
 *   |  /|           |  /|
 *   |/  |           |/  |
 *   ---------------------
 *   |  /|  /|  /|  /|  /|
 *   |/  |/  |/  |/  |/  |
 *   ---------------------
 *
 * @param[in] gridSize The grid size of the solid geometry to create
 * @return Returns the created geometry for the grid size
 */
DALI_UI_API Geometry CreateBorderGeometry(Uint16Pair gridSize);

/**
 * @brief Registers a properties for Stretch Ranges
 *
 * @param[in,out] renderer The renderer for broken image
 * @param[in] uniformName The name of the uniform
 * @param[in] stretchPixels The stretchable pixels in the cropped image space
 * @param[in] imageExtent The imageExtent
 */
void RegisterStretchProperties(Renderer& renderer, const char* uniformName,
                               const Dali::Ui::Integration::NPatchUtility::StretchRanges& stretchPixels, uint16_t imageExtent);

/**
 * @brief Apply a texture and uniforms
 *
 * @param[in,out] renderer The renderer for broken image
 * @param[in] data The pointer of npatch-data
 */
void ApplyTextureAndUniforms(Renderer& renderer, const Dali::Ui::Internal::NPatchData* data);

} // namespace NPatchHelper

} // namespace Integration
} // namespace Ui

} // namespace Dali

#endif // DALI_UI_NPATCH_HELPER_H
