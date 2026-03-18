#ifndef DALI_UI_INTERNAL_CONTROL_DEBUG_H
#define DALI_UI_INTERNAL_CONTROL_DEBUG_H
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

#if defined(DEBUG_ENABLED)

#include <dali-ui-foundation/public-api/controls/control-impl.h>
#include <dali/public-api/object/handle.h>
#include <iostream>
#include <string>

namespace Dali
{
namespace Ui
{
namespace Internal
{
// Note: Debug functions have been moved to view-debug.h during the Control to View merge.
// Control now inherits from View, so we use the View debug implementation.
// Use the functions from <dali-ui-foundation/internal/views/view/view-debug.h>
} // namespace Internal

} // namespace Ui

} // namespace Dali

#endif

#endif // DALI_UI_INTERNAL_CONTROL_DEBUG_H
