/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-ui-components/internal/window/window-frame-style-resolver.h>

#include <algorithm>

namespace Dali::Ui::Internal
{
namespace
{
Dali::Ui::Insets NonNegative(const Dali::Ui::Insets& insets)
{
  return Dali::Ui::Insets(std::max(0.0f, insets.start),
                          std::max(0.0f, insets.end),
                          std::max(0.0f, insets.top),
                          std::max(0.0f, insets.bottom));
}

bool HasUsableShadow(const WindowFrameStyle& style)
{
  return style.GetShadowSource() == WindowFrameShadowSource::COLOR ||
         (style.GetShadowSource() == WindowFrameShadowSource::IMAGE && !style.GetShadowImageUrl().Empty());
}
} // unnamed namespace

ResolvedWindowFrameStyle ResolveWindowFrameStyle(const WindowFrameStyle& style, bool maximized)
{
  ResolvedWindowFrameStyle resolved;

  if(!HasUsableShadow(style))
  {
    return resolved;
  }

  resolved.shadowSource  = style.GetShadowSource();
  resolved.shadowOutsets = maximized ? Dali::Ui::Insets() : NonNegative(style.GetShadowOutsets());
  resolved.shadowVisible = !maximized;
  return resolved;
}

Dali::Vector4 ToNPatchBorder(const Dali::Ui::Insets& border)
{
  return Dali::Vector4(border.start, border.top, border.end, border.bottom);
}

} // namespace Dali::Ui::Internal
