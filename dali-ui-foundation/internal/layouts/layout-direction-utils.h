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

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Mirrors a child's logical (left-to-right) x within its parent's width.
 *
 * This is THE definition of the right-to-left mirror used by the layout system.
 * It has exactly two consumers and they must agree, because one places the actor
 * and the other computes the visual bounds a layout transition animates to/from:
 *
 *  - @c ViewDataImpl::ApplyLayoutDirection -- writes the mirrored value to the
 *    child actor's POSITION_X at the end of every arrange pass.
 *  - @c LayoutTransitionDispatcher::VisualBoundsOf -- reports the same child's
 *    on-screen bounds under an RTL parent.
 *
 * Both feed it the SAME inputs: the parent's arranged width and the child's
 * LOGICAL arranged bounds (@c ViewImpl::GetArrangedBounds, which is never
 * mirrored). Sharing one formula is what keeps a transition's endpoints equal to
 * the geometry the arrange pass actually applies.
 *
 * @param[in] parentWidth The arranged width of the parent
 * @param[in] logicalX The child's logical (unmirrored) x within the parent
 * @param[in] childWidth The child's arranged width
 * @return The mirrored x
 */
inline float MirrorX(float parentWidth, float logicalX, float childWidth)
{
  return parentWidth - logicalX - childWidth;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
