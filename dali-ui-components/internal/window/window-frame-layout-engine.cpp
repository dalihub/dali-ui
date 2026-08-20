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

#include <dali-ui-components/internal/window/window-frame-layout-engine.h>

#include <algorithm>

namespace Dali::Ui::Internal
{
namespace
{
float NonNegative(float value)
{
  return std::max(0.0f, value);
}

Dali::Ui::Insets NonNegative(const Dali::Ui::Insets& insets)
{
  return Dali::Ui::Insets(NonNegative(insets.start), NonNegative(insets.end), NonNegative(insets.top), NonNegative(insets.bottom));
}

Dali::Vector2 NonNegative(const Dali::Vector2& size)
{
  return Dali::Vector2(NonNegative(size.width), NonNegative(size.height));
}

Dali::Ui::LayoutRect Inset(const Dali::Ui::LayoutRect& bounds, const Dali::Ui::Insets& requestedInsets)
{
  const Dali::Ui::Insets insets = NonNegative(requestedInsets);
  const float            start  = std::min(insets.start, bounds.width);
  const float            end    = std::min(insets.end, NonNegative(bounds.width - start));
  const float            top    = std::min(insets.top, bounds.height);
  const float            bottom = std::min(insets.bottom, NonNegative(bounds.height - top));

  return Dali::Ui::LayoutRect(bounds.x + start,
                              bounds.y + top,
                              NonNegative(bounds.width - start - end),
                              NonNegative(bounds.height - top - bottom));
}

Dali::Vector2 AddInsets(const Dali::Vector2& size, const Dali::Ui::Insets& requestedInsets)
{
  const Dali::Vector2    clampedSize = NonNegative(size);
  const Dali::Ui::Insets insets      = NonNegative(requestedInsets);
  return Dali::Vector2(clampedSize.width + insets.start + insets.end,
                       clampedSize.height + insets.top + insets.bottom);
}

Dali::Vector2 RemoveInsets(const Dali::Vector2& size, const Dali::Ui::Insets& requestedInsets)
{
  const Dali::Vector2    clampedSize = NonNegative(size);
  const Dali::Ui::Insets insets      = NonNegative(requestedInsets);
  return Dali::Vector2(NonNegative(clampedSize.width - insets.start - insets.end),
                       NonNegative(clampedSize.height - insets.top - insets.bottom));
}

void ClampHandleWidths(float availableWidth, float& leftWidth, float& rightWidth)
{
  leftWidth         = NonNegative(leftWidth);
  rightWidth        = NonNegative(rightWidth);
  const float total = leftWidth + rightWidth;
  if(total > availableWidth && total > 0.0f)
  {
    const float scale = availableWidth / total;
    leftWidth *= scale;
    rightWidth *= scale;
  }
}

bool IsSameRect(const Dali::Ui::LayoutRect& lhs, const Dali::Ui::LayoutRect& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height;
}

} // unnamed namespace

WindowFrameLayoutResult WindowFrameLayoutEngine::Calculate(float surfaceWidth, float surfaceHeight, const WindowFrameLayout& layout, const WindowFrameLayoutOptions& options)
{
  const Dali::Ui::LayoutRect surface(0.0f, 0.0f, NonNegative(surfaceWidth), NonNegative(surfaceHeight));
  const Dali::Ui::LayoutRect effect = surface;
  const Dali::Ui::LayoutRect frame  = Inset(surface, GetEffectiveShadowOutsets(options));

  const Dali::Ui::Insets decorationInsets = NonNegative(layout.GetDecorationInsets());
  const float            start            = std::min(decorationInsets.start, frame.width);
  const float            end              = std::min(decorationInsets.end, NonNegative(frame.width - start));
  const float            topHeight        = std::min(decorationInsets.top, frame.height);
  const float            bottomHeight     = std::min(decorationInsets.bottom, NonNegative(frame.height - topHeight));

  const Dali::Ui::LayoutRect top(frame.x, frame.y, frame.width, topHeight);
  const Dali::Ui::LayoutRect bottom(frame.x, frame.y + frame.height - bottomHeight, frame.width, bottomHeight);
  const Dali::Ui::LayoutRect left(frame.x, frame.y + topHeight, start, NonNegative(frame.height - topHeight - bottomHeight));
  const Dali::Ui::LayoutRect right(frame.x + frame.width - end, frame.y + topHeight, end, NonNegative(frame.height - topHeight - bottomHeight));
  const Dali::Ui::LayoutRect content = options.decorationOverlay ? frame : Inset(frame, decorationInsets);

  const Dali::Vector2 leftHandleSize   = layout.GetBottomLeftResizeHandleSize();
  const Dali::Vector2 rightHandleSize  = layout.GetBottomRightResizeHandleSize();
  float               leftHandleWidth  = leftHandleSize.width;
  float               rightHandleWidth = rightHandleSize.width;
  ClampHandleWidths(frame.width, leftHandleWidth, rightHandleWidth);
  const float leftHandleHeight  = std::min(NonNegative(leftHandleSize.height), frame.height);
  const float rightHandleHeight = std::min(NonNegative(rightHandleSize.height), frame.height);

  const Dali::Ui::LayoutRect bottomLeftResizeHandle(frame.x,
                                                    frame.y + frame.height - leftHandleHeight,
                                                    leftHandleWidth,
                                                    leftHandleHeight);
  const Dali::Ui::LayoutRect bottomRightResizeHandle(frame.x + frame.width - rightHandleWidth,
                                                     frame.y + frame.height - rightHandleHeight,
                                                     rightHandleWidth,
                                                     rightHandleHeight);

  return WindowFrameLayoutResult(surface, effect, frame, content, top, bottom, left, right, bottomLeftResizeHandle, bottomRightResizeHandle);
}

Dali::Ui::Insets WindowFrameLayoutEngine::GetEffectiveShadowOutsets(const WindowFrameLayoutOptions& options)
{
  return options.maximized ? Dali::Ui::Insets() : NonNegative(options.shadowOutsets);
}

Dali::Ui::LayoutRect WindowFrameLayoutEngine::SurfaceToFrameLocal(const Dali::Ui::LayoutRect& surfaceRect, const Dali::Ui::LayoutRect& frameBounds)
{
  return Dali::Ui::LayoutRect(surfaceRect.x - frameBounds.x,
                              surfaceRect.y - frameBounds.y,
                              NonNegative(surfaceRect.width),
                              NonNegative(surfaceRect.height));
}

Dali::Vector2 WindowFrameLayoutEngine::ContentToFrameSize(const Dali::Vector2& contentSize, const WindowFrameLayout& layout)
{
  return AddInsets(contentSize, layout.GetDecorationInsets());
}

Dali::Vector2 WindowFrameLayoutEngine::FrameToContentSize(const Dali::Vector2& frameSize, const WindowFrameLayout& layout, bool decorationOverlay)
{
  return decorationOverlay ? NonNegative(frameSize) : RemoveInsets(frameSize, layout.GetDecorationInsets());
}

Dali::Vector2 WindowFrameLayoutEngine::FrameToSurfaceSize(const Dali::Vector2& frameSize, const Dali::Ui::Insets& shadowOutsets, bool maximized)
{
  return AddInsets(frameSize, GetEffectiveShadowOutsets(WindowFrameLayoutOptions{maximized, false, shadowOutsets}));
}

Dali::Vector2 WindowFrameLayoutEngine::SurfaceToFrameSize(const Dali::Vector2& surfaceSize, const Dali::Ui::Insets& shadowOutsets, bool maximized)
{
  return RemoveInsets(surfaceSize, GetEffectiveShadowOutsets(WindowFrameLayoutOptions{maximized, false, shadowOutsets}));
}

Dali::Vector2 WindowFrameLayoutEngine::ContentToSurfaceSize(const Dali::Vector2& contentSize, const WindowFrameLayout& layout, const Dali::Ui::Insets& shadowOutsets, bool maximized)
{
  return FrameToSurfaceSize(ContentToFrameSize(contentSize, layout), shadowOutsets, maximized);
}

Dali::Vector2 WindowFrameLayoutEngine::SurfaceToContentSize(const Dali::Vector2& surfaceSize, const WindowFrameLayout& layout, const WindowFrameLayoutOptions& options)
{
  return FrameToContentSize(SurfaceToFrameSize(surfaceSize, options.shadowOutsets, options.maximized), layout, options.decorationOverlay);
}

bool IsSameWindowFrameGeometry(const WindowFrameGeometry& lhs, const WindowFrameGeometry& rhs)
{
  const Dali::Ui::Insets leftInsets  = lhs.GetDecorationOverlayInsets();
  const Dali::Ui::Insets rightInsets = rhs.GetDecorationOverlayInsets();
  return IsSameRect(lhs.GetSurfaceBounds(), rhs.GetSurfaceBounds()) &&
         IsSameRect(lhs.GetFrameBounds(), rhs.GetFrameBounds()) &&
         IsSameRect(lhs.GetContentBounds(), rhs.GetContentBounds()) &&
         leftInsets.start == rightInsets.start && leftInsets.end == rightInsets.end &&
         leftInsets.top == rightInsets.top && leftInsets.bottom == rightInsets.bottom;
}

} // namespace Dali::Ui::Internal
