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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/public-api/text/style/bevel.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>

namespace Dali
{

namespace Ui
{

namespace Text
{

/**
 * @brief Applies underline style properties to the controller.
 *
 * @param[in] controller The text controller.
 * @param[in] underline The underline style to apply.
 * @return True if any underline property was changed.
 */
bool ApplyUnderlineStyle(ControllerPtr controller, const Underline& underline);

/**
 * @brief Applies shadow style properties to the controller.
 *
 * @param[in] controller The text controller.
 * @param[in] shadow The shadow style to apply.
 * @return True if any shadow property was changed.
 */
bool ApplyShadowStyle(ControllerPtr controller, const Shadow& shadow);

/**
 * @brief Applies outline style properties to the controller.
 *
 * @param[in] controller The text controller.
 * @param[in] outline The outline style to apply.
 * @return True if any outline property was changed.
 */
bool ApplyOutlineStyle(ControllerPtr controller, const Outline& outline);

/**
 * @brief Applies line-through style properties to the controller.
 *
 * @param[in] controller The text controller.
 * @param[in] lineThrough The line-through style to apply.
 * @return True if any line-through property was changed.
 */
bool ApplyLineThroughStyle(ControllerPtr controller, const LineThrough& lineThrough);

/**
 * @brief Applies bevel style properties to the controller.
 *
 * @param[in] controller The text controller.
 * @param[in] bevel The bevel style to apply.
 * @return True if any bevel property was changed.
 */
bool ApplyBevelStyle(ControllerPtr controller, const Bevel& bevel);

} // namespace Text

} // namespace Ui

} // namespace Dali
