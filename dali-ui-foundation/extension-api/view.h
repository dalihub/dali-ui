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
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
/**
 * @brief Requests accessibility highlight for the given View.
 *
 * If another object is highlighted, the accessibility highlight moves to the
 * given View. When the highlight changes, accessibility clients are notified
 * of the highlighted state change.
 *
 * This request is intended for a View in a stable accessibility tree. A
 * concurrent accessibility context change, such as a newly showing page or
 * modal, may cause an accessibility client to choose a different highlight.
 *
 * @param[in] view The View to request accessibility highlight for
 * @return true if the View has the accessibility highlight, false otherwise
 */
DALI_UI_API bool GrabAccessibilityHighlight(View view);

/**
 * @brief Clears accessibility highlight from the given View.
 *
 * @param[in] view The View to clear accessibility highlight from
 * @return true if the accessibility highlight was cleared, false otherwise
 */
DALI_UI_API bool ClearAccessibilityHighlight(View view);

/**
 * @brief Sets the rendered X position of the view.
 *
 * The raw Dali::Actor geometry setters (SetPosition, SetSize, SetPositionX/Y/Z,
 * SetWidth, SetHeight, SetDepth) are deleted on the public View handle because a
 * View's rendered geometry is owned by the layout system. These extension-api
 * free functions give custom-view / component authors a sanctioned way to drive
 * the rendered position and size of a View handle directly.
 *
 * @param[in] view The view to modify
 * @param[in] x The rendered X position
 * @warning Writes the Actor render property POSITION_X directly and bypasses the
 * layout request; for a layout-managed child the next Arrange pass overwrites it.
 * Use View::SetRequestedX/Y and View::SetRequestedWidth/Height for
 * layout-aware placement and sizing.
 */
DALI_UI_API void SetPositionX(View view, float x);

/**
 * @brief Sets the rendered Y position of the view.
 *
 * @param[in] view The view to modify
 * @param[in] y The rendered Y position
 * @warning Writes the Actor render property POSITION_Y directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetPositionY(View view, float y);

/**
 * @brief Sets the rendered width of the view.
 *
 * @param[in] view The view to modify
 * @param[in] width The rendered width
 * @warning Writes the Actor render property SIZE_WIDTH directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetSizeWidth(View view, float width);

/**
 * @brief Sets the rendered height of the view.
 *
 * @param[in] view The view to modify
 * @param[in] height The rendered height
 * @warning Writes the Actor render property SIZE_HEIGHT directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetSizeHeight(View view, float height);

} // namespace Extension
} // namespace Ui
} // namespace Dali
