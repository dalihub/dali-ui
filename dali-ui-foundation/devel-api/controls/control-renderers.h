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

#ifndef DALI_UI_FOUNDATION_CONTROL_RENDERERS_H
#define DALI_UI_FOUNDATION_CONTROL_RENDERERS_H

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-toolkit-common.h>

namespace Dali::UI::DevelControl
{

/**
 * Helper method for rendering an image with custom shader.
 * @param[in] vertexSrc The custom vertex shader.
 * @param[in] fragmentSrc The custom fragment shader.
 * @return A newly created renderer.
 */
DALI_UI_API Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc);

/**
 * Helper method for rendering an image with custom shader.
 * @param[in] vertexSrc The custom vertex shader.
 * @param[in] fragmentSrc The custom fragment shader.
 * @param[in] hints Shader hints for optimization.
 * @param[in] shaderName Name of the shader for debugging.
 * @param[in] gridSize The number of grid sub-divisions required.
 * @return A newly created renderer.
 */
DALI_UI_API Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc,
                                          Dali::Shader::Hint::Value hints, const std::string& shaderName,
                                          Dali::Uint16Pair gridSize);

/**
 * Helper method for setting the first texture passed to a renderer.
 * @param[in] renderer The renderer using the texture.
 * @param[in] texture The texture to set.
 */
DALI_UI_API void SetRendererTexture(Dali::Renderer renderer, Dali::Texture texture);

/**
 * Helper method for setting the first texture passed to a renderer.
 * @param[in] renderer The renderer using the texture.
 * @param[in] frameBuffer A frame buffer color texture attached.
 */
DALI_UI_API void SetRendererTexture(Dali::Renderer renderer, Dali::FrameBuffer frameBuffer);

} // namespace Dali::UI::DevelControl

#endif // DALI_UI_FOUNDATION_CONTROL_RENDERERS_H