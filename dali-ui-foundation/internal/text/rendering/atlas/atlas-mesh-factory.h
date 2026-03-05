#ifndef DALI_UI_ATLAS_MESH_FACTORY_H
#define DALI_UI_ATLAS_MESH_FACTORY_H

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
 */

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-manager.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace AtlasMeshFactory
{
typedef uint32_t SizeType;

/**
 * @brief Create a Quad that describes an area in an atlas and a position.
 *
 * @param[in]  width Width of area in pixels.
 * @param[in]  height Height of area in pixels.
 * @param[in]  block Block position in atlas.
 * @param[in]  atlasSize Atlas and block dimensions.
 * @param[in]  position Position to place area in space.
 * @param[out] mesh Mesh object to hold created quad.
 */
void CreateQuad(SizeType width, SizeType height, SizeType block, const UI::AtlasManager::AtlasSize& atlasSize,
                const Vector2& position, UI::AtlasManager::Mesh2D& mesh);

/**
 * @brief Append one mesh to another.
 *
 * @param[in,out] first Mesh to append to.
 * @param[in]     second Mesh to append.
 */
void AppendMesh(UI::AtlasManager::Mesh2D& first, const UI::AtlasManager::Mesh2D& second);

} // namespace AtlasMeshFactory

} // namespace Internal

} // namespace UI

} // namespace Dali

#endif // DALI_UI_ATLAS_MESH_FACTORY_H
