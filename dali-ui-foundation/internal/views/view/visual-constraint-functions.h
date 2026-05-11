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

#pragma once

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>

namespace Dali::Ui::Internal
{

/**
 * @brief Constraint for BoxShadow corner radius accounting for ABSOLUTE policy + EFFECTIVE_SCALE.
 * inputs[0] : View CornerRadius (natural px), [1] : View CornerRadiusPolicy, [2] : View EFFECTIVE_SCALE
 * @param[out] current BoxShadow visual corner radius.
 * @param[in] inputs Input properties.
 */
void BoxShadowCornerRadiusConstraint(Vector4& current, const PropertyInputContainer& inputs);

/**
 * @brief Default corner-radius constraint for background / image / shadow visuals.
 * inputs[0] : View CORNER_RADIUS (natural value — fraction for RELATIVE, natural px for ABSOLUTE)
 * inputs[1] : View CORNER_RADIUS_POLICY
 * inputs[2] : View EFFECTIVE_SCALE
 * @param[out] current Visual corner radius in its own coordinate system.
 * @param[in] inputs Input properties.
 */
void ScaledCornerRadiusConstraint(Vector4& current, const PropertyInputContainer& inputs);

/**
 * @brief Constraint function for Borderline's CornerRadius
 * inputs[0] : View CornerRadius (natural px), [1] : View CornerRadiusPolicy, [2] : View size,
 * [3] : Borderline Width (natural px), [4] : Borderline Offset, [5] : View EFFECTIVE_SCALE
 * @param[out] current Borderline visual corner radius value.
 * @param[in] inputs Input properties.
 */
void BorderlineCornerRadiusConstraint(Vector4& current, const PropertyInputContainer& inputs);

/**
 * @brief Constraint that copies View BORDERLINE_WIDTH scaled by EFFECTIVE_SCALE.
 * inputs[0] : View BORDERLINE_WIDTH (natural px), [1] : View EFFECTIVE_SCALE
 * @param[out] current Visual renderer BORDERLINE_WIDTH (visual px).
 * @param[in] inputs Input properties.
 */
void ScaledBorderlineWidthConstraint(float& current, const PropertyInputContainer& inputs);

/**
 * @brief Constraint function for InnerShadow's CornerRadius
 * inputs[0] : View CornerRadius (natural px), [1] : View CornerRadiusPolicy, [2] : View size,
 * [3] : ExtraSize, [4] : InnerShadow Borderline Width, [5] : View EFFECTIVE_SCALE
 * @param[out] current InnerShadow's corner radius value.
 * @param[in] inputs Input properties.
 */
void InnerShadowCornerRadiusConstraint(Vector4& current, const PropertyInputContainer& inputs);

} // namespace Dali::Ui::Internal
