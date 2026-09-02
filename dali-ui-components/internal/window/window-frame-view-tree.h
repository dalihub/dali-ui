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
#include <dali-ui-components/public-api/window/window-frame-style.h>

#include <dali-ui-foundation/dali-ui-foundation.h>

namespace Dali::Ui::Internal
{

// Owns the WindowFrame visual tree. Native state, input routing and command
// policy intentionally stay outside this class.
class WindowFrameViewTree
{
public:
  void Create(const WindowFrameStyle& style);
  void Unparent();
  void Destroy();

  void SetDecoration(const WindowFrameDecoration& decoration);
  void SetFrameStyle(const WindowFrameStyle& style);
  void ApplyLayout(const Dali::PositionSize& positionSize,
                   const WindowFrameLayout&  frameLayout,
                   bool                      maximized,
                   bool                      decorationOverlay,
                   const WindowFrameStyle&   style);
  void SetDecorationVisible(bool visible);

  Dali::Ui::AbsoluteLayout GetSurfaceRoot() const;
  Dali::Ui::View           GetContentRoot() const;

  /**
   * @brief Returns the parent that holds the currently installed slot roots.
   *
   * Used by decoration validation to accept a resubmission of the installed
   * decoration while still rejecting slots owned by anything else.
   */
  Dali::Ui::AbsoluteLayout GetDecorationRoot() const;

  WindowFrameDecoration&       GetDecoration();
  const WindowFrameDecoration& GetDecoration() const;

private:
  void AddDecorationViews();
  void RemoveDecorationViews();

private:
  Dali::Ui::AbsoluteLayout mSurfaceRoot;
  Dali::Ui::AbsoluteLayout mFrameEffectVisual;
  Dali::Ui::View           mFrameEffectContent;
  Dali::Ui::AbsoluteLayout mFrameRoot;
  Dali::Ui::View           mFrameBackground;
  Dali::Ui::AbsoluteLayout mContentRoot;
  Dali::Ui::AbsoluteLayout mDecorationRoot;
  WindowFrameDecoration    mDecoration;
};

} // namespace Dali::Ui::Internal
