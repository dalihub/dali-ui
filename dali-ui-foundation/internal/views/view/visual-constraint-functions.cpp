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

// CLASS HEADER
#include "visual-constraint-functions.h"

// EXTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali::Ui::Internal
{

void BorderlineCornerRadiusConstraint(Vector4& current, const PropertyInputContainer& inputs)
{
  // We just assume below state are applied.
  // - Transform::ORIGIN is CENTER
  // - Transform::ANCHOR_POINT is CENTER
  // - Transform::OFFSET_POLICY are ABSOLUTE
  // - Transform::SIZE_POLICY are RELATIVE
  // - Transform::SIZE is Vector2::ONE
  // - Transform::EXTRA_SIZE is Vector2::ZERO

  Vector4 viewCornerRadius = inputs[0]->GetVector4();

  const int     viewCornerRadiusPolicy = inputs[1]->GetInteger();
  const Vector3 visualSize             = inputs[2]->GetVector3(); // We use VisualSize as ViewSize.

  const float borderlineWidth  = inputs[3]->GetFloat();
  const float borderlineOffset = inputs[4]->GetFloat();

  if(viewCornerRadiusPolicy == Ui::Visual::Transform::Policy::RELATIVE)
  {
    const float minViewSize = std::min(visualSize.x, visualSize.y);
    viewCornerRadius *= minViewSize;
  }

  // Corner Radius for Borderline is expand about borderlineWidth.
  const float expendedRadius = borderlineWidth * (1.0f + borderlineOffset) * 0.5f;

  // Calculate on pixel scale.
  current.x = viewCornerRadius.x < Dali::Math::MACHINE_EPSILON_100 ? 0.0f : viewCornerRadius.x + expendedRadius;
  current.y = viewCornerRadius.y < Dali::Math::MACHINE_EPSILON_100 ? 0.0f : viewCornerRadius.y + expendedRadius;
  current.z = viewCornerRadius.z < Dali::Math::MACHINE_EPSILON_100 ? 0.0f : viewCornerRadius.z + expendedRadius;
  current.w = viewCornerRadius.w < Dali::Math::MACHINE_EPSILON_100 ? 0.0f : viewCornerRadius.w + expendedRadius;

  if(viewCornerRadiusPolicy == Ui::Visual::Transform::Policy::RELATIVE)
  {
    const float minVisualSize = std::min(visualSize.x + expendedRadius, visualSize.y + expendedRadius);
    if(DALI_LIKELY(minVisualSize > Math::MACHINE_EPSILON_100))
    {
      current /= minVisualSize;
    }
  }
}

void InnerShadowCornerRadiusConstraint(Vector4& current, const PropertyInputContainer& inputs)
{
  // We just assume below state are applied.
  // - Transform::ORIGIN is CENTER
  // - Transform::ANCHOR_POINT is CENTER
  // - Transform::OFFSET_POLICY are ABSOLUTE
  // - Transform::SIZE_POLICY are RELATIVE
  // - Transform::SIZE is Vector2::ONE
  // - Visual::BORDERLINE_OFFSET is -1.0f

  Vector4 viewCornerRadius = inputs[0]->GetVector4();

  const int     viewCornerRadiusPolicy = inputs[1]->GetInteger();
  const Vector3 visualSize             = inputs[2]->GetVector3(); // We use VisualSize as ViewSize.

  Vector2     extraSize       = inputs[3]->GetVector2();
  const float borderlineWidth = inputs[4]->GetFloat(); // inner shadow's own borderline

  if(viewCornerRadiusPolicy == Ui::Visual::Transform::Policy::RELATIVE)
  {
    const float minViewSize = std::min(visualSize.x, visualSize.y);
    viewCornerRadius *= minViewSize;
  }

  // Corner Radius for InnerShadow is expanded about the inner shadow's own borderline width.

  // Calculate on pixel scale.
  current.x = viewCornerRadius.x + borderlineWidth;
  current.y = viewCornerRadius.y + borderlineWidth;
  current.z = viewCornerRadius.z + borderlineWidth;
  current.w = viewCornerRadius.w + borderlineWidth;

  if(viewCornerRadiusPolicy == Ui::Visual::Transform::Policy::RELATIVE)
  {
    const float minInnerShadowSize = std::min(visualSize.x + extraSize.x, visualSize.y + extraSize.y);
    if(DALI_LIKELY(minInnerShadowSize > Math::MACHINE_EPSILON_100))
    {
      current /= minInnerShadowSize;
    }
  }
}

} // namespace Dali::Ui::Internal
