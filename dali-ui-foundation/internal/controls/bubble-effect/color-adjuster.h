#ifndef DALI_UI_INTERNAL_COLOR_ADJUSTER_H
#define DALI_UI_INTERNAL_COLOR_ADJUSTER_H

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
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
inline void SetColorAdjusterProperties(Actor& actor, const Vector3& hsvDelta, bool ignoreAlpha = false)
{
  actor.RegisterProperty("uHSVDelta", hsvDelta);
  actor.RegisterProperty("uIgnoreAlpha", ignoreAlpha ? 1.f : 0.f);
}

/**
 * Creates a new ColorAdjuster effect.
 * ColorAdjuster is a custom shader effect to adjust the image color in HSV space.
 * @param[in] hsvDelta The color difference to apply to the HSV channel.
 * @param[in] ignoreAlpha If true, the result color will be opaque even though source has alpha value
 * @return A handle to a newly allocated Dali resource.
 */
inline Property::Map CreateColorAdjuster()
{
  Property::Map customShader;
  customShader[UI::Visual::Shader::Property::FRAGMENT_SHADER] = SHADER_BUBBLE_EFFECT_COLOR_ADJUSTER_FRAG.data();

  Property::Map map;
  map[UI::Visual::Property::SHADER] = customShader;

  return map;
}

} // namespace Internal

} // namespace UI

} // namespace Dali

#endif // DALI_UI_INTERNAL_COLOR_ADJUSTER_H
