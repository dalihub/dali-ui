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

#include "window-frame-feature-config.h"

#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/signals/callback.h>

#include <memory>
#include <optional>

namespace Dali::Ui
{
class WindowFrameOptions;
}

namespace Dali::Ui::Internal
{

struct NativeWindowState
{
  bool maximized{false};
  bool minimized{false};
};

class WindowOperationAdapter
{
public:
  using CloseCallback = Dali::Callback<void()>;

  virtual ~WindowOperationAdapter() = default;

  virtual const WindowFrameFeatureConfig&   GetFeatureConfig() const         = 0;
  virtual std::optional<Dali::PositionSize> GetTargetMaximizedBounds() const = 0;
  virtual NativeWindowState                 GetWindowState() const           = 0;

  virtual bool RequestMove()                                                                                             = 0;
  virtual bool RequestResize(Dali::WindowResizeDirection direction)                                                      = 0;
  virtual bool RequestMinimize()                                                                                         = 0;
  virtual bool RequestMaximize(bool maximize, std::optional<Dali::Window::WindowSize> restoreSurfaceSize = std::nullopt) = 0;
  virtual bool RequestClose()                                                                                            = 0;
  virtual bool ApplyResizeAspectRatioHint(bool enabled)                                                                  = 0;
  virtual void RestoreResizeAspectRatioHint()                                                                            = 0;
};

std::unique_ptr<WindowOperationAdapter> CreateDaliWindowOperationAdapter(Dali::Window                          window,
                                                                         WindowOperationAdapter::CloseCallback closeCallback,
                                                                         const WindowFrameOptions&             options);

} // namespace Dali::Ui::Internal
