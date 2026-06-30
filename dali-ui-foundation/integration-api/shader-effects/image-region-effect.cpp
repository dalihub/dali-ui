/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/shader-effects/image-region-effect.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::ToPropertyValue;

namespace Dali
{
namespace Ui
{
namespace Integration
{
Property::Map CreateImageRegionEffect()
{
  Property::Map map;

  Property::Map customShader;
  customShader[Visual::Shader::Property::VERTEX_SHADER] = ToPropertyValue(SHADER_IMAGE_REGION_EFFECT_VERT.data());

  map[Ui::VisualBasePropertyIndex::SHADER] = customShader;
  return map;
}

} // namespace Integration
} // namespace Ui

} // namespace Dali
