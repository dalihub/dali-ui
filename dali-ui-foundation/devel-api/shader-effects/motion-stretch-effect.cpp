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
#include <dali-ui-foundation/devel-api/shader-effects/motion-stretch-effect.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::ToPropertyValue;

namespace Dali
{
namespace Ui
{
void SetMotionStretchProperties(Actor& actor)
{
  actor.RegisterProperty(Dali::StringView("uGeometryStretchFactor"), 0.5f);
  actor.RegisterProperty(Dali::StringView("uSpeedScalingFactor"), 0.5f);
  actor.RegisterProperty(Dali::StringView("uObjectFadeStart"), Vector2(0.25f, 0.25f));
  actor.RegisterProperty(Dali::StringView("uObjectFadeEnd"), Vector2(0.5f, 0.5f));
  actor.RegisterProperty(Dali::StringView("uAlphaScale"), 0.75f);
  Property::Index uModelProperty = actor.RegisterProperty(Dali::StringView("uModelLastFrame"), Matrix::IDENTITY);

  Constraint constraint = Constraint::New<Matrix>(actor, uModelProperty, EqualToConstraint());
  constraint.AddSource(Source(actor, Actor::Property::WORLD_MATRIX));
  constraint.Apply();
}

Property::Map CreateMotionStretchEffect()
{
  Property::Map map;

  Property::Map customShader;
  customShader[Visual::Shader::Property::VERTEX_SHADER]   = ToPropertyValue(SHADER_MOTION_STRETCH_EFFECT_VERT.data());
  customShader[Visual::Shader::Property::FRAGMENT_SHADER] = ToPropertyValue(SHADER_MOTION_STRETCH_EFFECT_FRAG.data());

  customShader[Visual::Shader::Property::SUBDIVIDE_GRID_X] = 10;
  customShader[Visual::Shader::Property::SUBDIVIDE_GRID_Y] = 10;

  customShader[Visual::Shader::Property::HINTS] = Shader::Hint::OUTPUT_IS_TRANSPARENT;

  map[Ui::Visual::Property::SHADER] = customShader;
  return map;
}

} // namespace Ui
} // namespace Dali
