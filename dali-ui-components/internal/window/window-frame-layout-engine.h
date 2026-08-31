#pragma once

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

#include <dali-ui-components/public-api/window/window-frame-decoration.h>
#include <dali-ui-components/public-api/window/window-frame-geometry.h>

namespace Dali::Ui::Internal
{
class WindowFrameLayoutResult
{
public:
  WindowFrameLayoutResult() = default;
  WindowFrameLayoutResult(const Dali::Ui::LayoutRect& surface,
                          const Dali::Ui::LayoutRect& effect,
                          const Dali::Ui::LayoutRect& frame,
                          const Dali::Ui::LayoutRect& content,
                          const Dali::Ui::LayoutRect& top,
                          const Dali::Ui::LayoutRect& bottom,
                          const Dali::Ui::LayoutRect& left,
                          const Dali::Ui::LayoutRect& right,
                          const Dali::Ui::LayoutRect& bottomLeftResizeHandle,
                          const Dali::Ui::LayoutRect& bottomRightResizeHandle)
  : mSurface(surface),
    mEffect(effect),
    mFrame(frame),
    mContent(content),
    mTop(top),
    mBottom(bottom),
    mLeft(left),
    mRight(right),
    mBottomLeftResizeHandle(bottomLeftResizeHandle),
    mBottomRightResizeHandle(bottomRightResizeHandle)
  {
  }

  Dali::Ui::LayoutRect GetSurface() const
  {
    return mSurface;
  }

  Dali::Ui::LayoutRect GetEffect() const
  {
    return mEffect;
  }

  Dali::Ui::LayoutRect GetFrame() const
  {
    return mFrame;
  }

  Dali::Ui::LayoutRect GetContent() const
  {
    return mContent;
  }

  Dali::Ui::LayoutRect GetTop() const
  {
    return mTop;
  }

  Dali::Ui::LayoutRect GetBottom() const
  {
    return mBottom;
  }

  Dali::Ui::LayoutRect GetLeft() const
  {
    return mLeft;
  }

  Dali::Ui::LayoutRect GetRight() const
  {
    return mRight;
  }

  Dali::Ui::LayoutRect GetBottomLeftResizeHandle() const
  {
    return mBottomLeftResizeHandle;
  }

  Dali::Ui::LayoutRect GetBottomRightResizeHandle() const
  {
    return mBottomRightResizeHandle;
  }

private:
  Dali::Ui::LayoutRect mSurface;
  Dali::Ui::LayoutRect mEffect;
  Dali::Ui::LayoutRect mFrame;
  Dali::Ui::LayoutRect mContent;
  Dali::Ui::LayoutRect mTop;
  Dali::Ui::LayoutRect mBottom;
  Dali::Ui::LayoutRect mLeft;
  Dali::Ui::LayoutRect mRight;
  Dali::Ui::LayoutRect mBottomLeftResizeHandle;
  Dali::Ui::LayoutRect mBottomRightResizeHandle;
};

struct WindowFrameLayoutOptions
{
  bool             maximized{false};
  bool             decorationOverlay{false};
  Dali::Ui::Insets shadowOutsets;
};

class WindowFrameLayoutEngine
{
public:
  static WindowFrameLayoutResult Calculate(float surfaceWidth, float surfaceHeight, const WindowFrameLayout& layout, const WindowFrameLayoutOptions& options = {});

  static Dali::Ui::Insets GetEffectiveShadowOutsets(const WindowFrameLayoutOptions& options);

  static Dali::Ui::LayoutRect SurfaceToFrameLocal(const Dali::Ui::LayoutRect& surfaceRect, const Dali::Ui::LayoutRect& frameBounds);

  static Dali::Vector2 ContentToFrameSize(const Dali::Vector2& contentSize, const WindowFrameLayout& layout);
  static Dali::Vector2 FrameToContentSize(const Dali::Vector2& frameSize, const WindowFrameLayout& layout, bool decorationOverlay = false);
  static Dali::Vector2 FrameToSurfaceSize(const Dali::Vector2& frameSize, const Dali::Ui::Insets& shadowOutsets, bool maximized = false);
  static Dali::Vector2 SurfaceToFrameSize(const Dali::Vector2& surfaceSize, const Dali::Ui::Insets& shadowOutsets, bool maximized = false);
  static Dali::Vector2 ContentToSurfaceSize(const Dali::Vector2& contentSize, const WindowFrameLayout& layout, const Dali::Ui::Insets& shadowOutsets, bool maximized = false);
  static Dali::Vector2 SurfaceToContentSize(const Dali::Vector2& surfaceSize, const WindowFrameLayout& layout, const WindowFrameLayoutOptions& options = {});
};

bool IsSameWindowFrameGeometry(const WindowFrameGeometry& lhs, const WindowFrameGeometry& rhs);

} // namespace Dali::Ui::Internal
