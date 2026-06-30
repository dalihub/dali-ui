#ifndef DALI_UI_INTEGRATION_API_VISUALS_VISUAL_ACTIONS_INTEG_H
#define DALI_UI_INTEGRATION_API_VISUALS_VISUAL_ACTIONS_INTEG_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-action-index-ranges.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace Visual
{

/**
 * @brief Actions that the visual can perform. These actions are called through the Visual::Base::DoAction API.
 */
namespace Action
{
/**
 * @brief The available actions for this visual
 */
enum Type
{
  /**
   * @brief Update the properties of the visual.
   */
  UPDATE_PROPERTY = VISUAL_ACTION_BASE_START_INDEX
};

} // namespace Action

} // namespace Visual

} // namespace Integration
} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTEGRATION_API_VISUALS_VISUAL_ACTIONS_INTEG_H
