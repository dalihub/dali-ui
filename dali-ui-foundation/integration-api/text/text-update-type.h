#ifndef DALI_UI_INTEGRATION_TEXT_UPDATE_TYPE_H
#define DALI_UI_INTEGRATION_TEXT_UPDATE_TYPE_H

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

namespace Dali::Ui::Integration::Text
{
/**
 * @brief Enumerates which text-rendering state changed during relayout.
 */
namespace TextUpdate
{
enum Type
{
  NONE_UPDATED      = 0x0, ///< Nothing has been updated.
  MODEL_UPDATED     = 0x1, ///< The text model has been updated.
  DECORATOR_UPDATED = 0x2  ///< The text decoration has been updated.
};
} // namespace TextUpdate

using UpdateTextType = TextUpdate::Type;

} // namespace Dali::Ui::Integration::Text

#endif // DALI_UI_INTEGRATION_TEXT_UPDATE_TYPE_H
