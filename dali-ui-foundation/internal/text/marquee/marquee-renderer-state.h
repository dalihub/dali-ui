#ifndef DALI_UI_TEXT_MARQUEE_RENDERER_STATE_H
#define DALI_UI_TEXT_MARQUEE_RENDERER_STATE_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief Returns whether the Label is waiting for a requested marquee loop to finish.
 *
 * Reads the current scroller state so TextVisual can defer renderer updates
 * without duplicating state or changing the control and async interface vtables.
 *
 * @param[in] control The actor that owns the text visual.
 * @return True if the owner is a Label with a pending FINISH_LOOP stop, false otherwise.
 */
bool IsMarqueeStopRequested(Actor control);
} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_TEXT_MARQUEE_RENDERER_STATE_H
