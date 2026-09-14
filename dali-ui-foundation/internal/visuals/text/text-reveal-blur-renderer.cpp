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

#include "text-reveal-blur-renderer.h"

#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/render-effects/gaussian-blur-algorithm.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/rendering/vertex-buffer.h>
#include <locale>
#include <sstream>
#include <string>

namespace DALI_NAMESPACE::Ui::Internal::TextRevealBlurRenderer
{
namespace
{
// Match the existing Gaussian shader range: up to 200 pixels / 100 samples.
constexpr uint32_t MAXIMUM_NUMBER_OF_SAMPLES = 100u;

/**
 * @brief Gets the cached quad for non-batched Reveal blur passes.
 */
inline static Dali::Geometry& GetCachedGeometry()
{
  thread_local static Dali::Geometry gPredefinedGeometry;
  if(!gPredefinedGeometry)
  {
    gPredefinedGeometry = Dali::Geometry::New();

    struct VertexPosition
    {
      Dali::Vector2 position;
    };

    VertexPosition positionArray[]  = {{Dali::Vector2(-0.5f, -0.5f)},
                                       {Dali::Vector2(0.5f, -0.5f)},
                                       {Dali::Vector2(-0.5f, 0.5f)},
                                       {Dali::Vector2(0.5f, 0.5f)}};
    uint32_t       numberOfVertices = sizeof(positionArray) / sizeof(VertexPosition);

    Dali::Property::Map positionVertexFormat;
    positionVertexFormat["aPosition"]   = Dali::Property::VECTOR2;
    Dali::VertexBuffer positionVertices = Dali::VertexBuffer::New(positionVertexFormat);
    positionVertices.SetData(positionArray, numberOfVertices);
    gPredefinedGeometry.AddVertexBuffer(positionVertices);

    const uint16_t indices[] = {0, 3, 1, 0, 2, 3};
    gPredefinedGeometry.SetIndexBuffer(&indices[0], sizeof(indices) / sizeof(indices[0]));
  }
  return gPredefinedGeometry;
}

Shader& GetShader(uint32_t blurRadius, bool batch)
{
  if(!GaussianBlurAlgorithm::IsSupportedRadius(blurRadius))
  {
    thread_local Shader invalid;
    invalid.Reset();
    return invalid;
  }
  // Effect shaders and all Reveal-specific bindings belong to separate caches.
  // Only the immutable Gaussian sample block is shared with ordinary effects.
  thread_local Shader shaders[2u][MAXIMUM_NUMBER_OF_SAMPLES + 1u];
  const uint32_t      numSamples = blurRadius >> 1;
  DALI_ASSERT_DEBUG(numSamples <= MAXIMUM_NUMBER_OF_SAMPLES && "numSamples too big!");
  auto& shader = shaders[batch ? 1u : 0u][numSamples];
  if(!shader)
  {
    std::ostringstream shaderName;
    shaderName.imbue(std::locale::classic());
    shaderName << "GaussianBlurShader_" << numSamples << "_TextReveal";
    if(batch)
    {
      shaderName << "_Batch";
    }
    std::ostringstream fragment;
    fragment.imbue(std::locale::classic());
    if(batch)
    {
      fragment << "#define TEXT_REVEAL_DRAW_BATCH\n";
    }
    fragment << SHADER_TEXT_REVEAL_BLUR_FRAG;
    const std::string batchVertex = batch ? "#define NUM_REVEAL_LINES " + std::to_string(MAX_LINES_PER_DRAW) + "\n" + std::string(SHADER_TEXT_REVEAL_BLUR_VERT)
                                          : std::string();
    shader                        = GaussianBlurAlgorithm::CreateShader(
      blurRadius,
      batch ? Dali::Integration::ToDaliStringView(batchVertex) : Dali::Integration::ToDaliStringView(SHADER_CONTROL_RENDERERS_VERT),
      Dali::Integration::ToDaliStringView(fragment.str()), Shader::Hint::FILE_CACHE_SUPPORT,
      Dali::Integration::ToDaliStringView(shaderName.str()));
    shader.ReserveCustomProperties(1);
    shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
  }
  return shader;
}

Renderer CreateRenderer(uint32_t blurRadius, Geometry geometry, bool batch)
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  auto shader = GetShader(blurRadius, batch);
  if(!Dali::Adaptor::IsAvailable() || !shader)
  {
    return {};
  }
  Renderer renderer = Renderer::New(geometry, shader);
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  // Initialize scalar passes too; shared shaders must not retain another
  // renderer's atlas binding. Texture/sampler ownership stays with the caller.
  if(!batch)
  {
    renderer.RegisterProperty("uRevealBatchRect", Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  }
  renderer.RegisterProperty("uRevealBatchInvSize", Vector2::ZERO);
  renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
  return renderer;
}
} // unnamed namespace

Renderer Create(uint32_t blurRadius)
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return {};
  }
  return CreateRenderer(blurRadius, GetCachedGeometry(), false);
}

Renderer CreateBatch(uint32_t blurRadius, Geometry geometry)
{
  return CreateRenderer(blurRadius, geometry, true);
}
} //namespace DALI_NAMESPACE::Ui::Internal::TextRevealBlurRenderer
