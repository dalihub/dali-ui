#ifndef DALI_UI_ALIGN_ENUMERATIONS_H
#define DALI_UI_ALIGN_ENUMERATIONS_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Ui
{
/**
 * @addtogroup dali_ui_view
 * @{
 */

/**
 * @brief Enumerations for the alignment of Visuals
 */
namespace Align
{
/**
 * @brief Describes anchor point and parent origin of visuals
 */
enum Type
{
  TOP_BEGIN = 0,
  TOP_CENTER,
  TOP_END,
  CENTER_BEGIN,
  CENTER,
  CENTER_END,
  BOTTOM_BEGIN,
  BOTTOM_CENTER,
  BOTTOM_END
};
} // namespace Align

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_ALIGN_ENUMERATIONS_H
