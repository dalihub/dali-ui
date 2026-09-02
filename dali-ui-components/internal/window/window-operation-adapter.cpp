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

#include "window-operation-adapter.h"

#include <dali-ui-components/public-api/window/window-frame-options.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/adaptor-framework/window-system-devel.h>
#include <dali/integration-api/debug.h>

#include <utility>

namespace Dali::Ui::Internal
{
namespace
{
const Dali::String RESIZE_ASPECT_RATIO_HINT{"wm.policy.win.resize_aspect_ratio"};
const Dali::String RESIZE_ASPECT_RATIO_ENABLED{"1"};
const Dali::String RESIZE_ASPECT_RATIO_DISABLED{"0"};

WindowFrameFeatureConfig BuildFeatureConfig(const WindowFrameOptions& options)
{
  WindowFrameFeatureConfig config;
  config.moveEnabled            = options.IsFeatureEnabled(WindowFrameFeature::MOVE);
  config.resizeEnabled          = options.IsFeatureEnabled(WindowFrameFeature::RESIZE);
  config.minimizeEnabled        = options.IsFeatureEnabled(WindowFrameFeature::MINIMIZE);
  config.maximizeRestoreEnabled = options.IsFeatureEnabled(WindowFrameFeature::MAXIMIZE_RESTORE);
  return config;
}

class DaliWindowOperationAdapter : public WindowOperationAdapter
{
public:
  DaliWindowOperationAdapter(Dali::Window window, CloseCallback closeCallback, const WindowFrameOptions& options)
  : mWindow(window),
    mCloseCallback(std::move(closeCallback)),
    mFeatureConfig(BuildFeatureConfig(options))
  {
  }

  ~DaliWindowOperationAdapter() override
  {
    RestoreResizeAspectRatioHint();
    if(mCreatedResizeAspectRatioHint && mResizeAspectRatioHintId != 0u && mWindow)
    {
      mWindow.RemoveAuxiliaryHint(mResizeAspectRatioHintId);
    }
  }

  const WindowFrameFeatureConfig& GetFeatureConfig() const override
  {
    return mFeatureConfig;
  }

  std::optional<Dali::PositionSize> GetTargetMaximizedBounds() const override
  {
    int32_t width  = 0;
    int32_t height = 0;
    Dali::DevelWindowSystem::GetScreenSize(width, height);
    if(width <= 0 || height <= 0)
    {
      return std::nullopt;
    }
    return Dali::PositionSize(0, 0, width, height);
  }

  NativeWindowState GetWindowState() const override
  {
    if(!mWindow)
    {
      return {};
    }
    // Feature settings gate requests; they must not hide state reported by the
    // underlying window system.
    return NativeWindowState{mWindow.IsMaximized(), mWindow.IsMinimized()};
  }

  bool RequestMove() override
  {
    if(!CanDispatchRequest(mFeatureConfig.moveEnabled, "move"))
    {
      return false;
    }
    mWindow.RequestMoveToServer();
    return true;
  }

  bool RequestResize(Dali::WindowResizeDirection direction) override
  {
    if(!CanDispatchRequest(mFeatureConfig.resizeEnabled, "resize"))
    {
      return false;
    }
    Dali::DevelWindow::RequestResizeToServer(mWindow, direction);
    return true;
  }

  bool RequestMinimize() override
  {
    if(!CanDispatchRequest(mFeatureConfig.minimizeEnabled, "minimize"))
    {
      return false;
    }
    mWindow.Minimize(true);
    return true;
  }

  bool RequestMaximize(bool maximize, std::optional<Dali::Window::WindowSize> restoreSurfaceSize) override
  {
    if(!CanDispatchRequest(mFeatureConfig.maximizeRestoreEnabled, maximize ? "maximize" : "restore"))
    {
      return false;
    }

    if(restoreSurfaceSize)
    {
      Dali::DevelWindow::MaximizeWithRestoreSize(mWindow, maximize, *restoreSurfaceSize);
    }
    else
    {
      mWindow.Maximize(maximize);
    }
    return true;
  }

  bool RequestClose() override
  {
    if(!mCloseCallback)
    {
      LogRequest("close", false);
      return false;
    }
    LogRequest("close", true);
    mCloseCallback.Invoke();
    return true;
  }

  bool ApplyResizeAspectRatioHint(bool enabled) override
  {
    if(!mWindow)
    {
      return false;
    }

    const Dali::String& value = enabled ? RESIZE_ASPECT_RATIO_ENABLED : RESIZE_ASPECT_RATIO_DISABLED;
    if(mResizeAspectRatioHintId == 0u)
    {
      mResizeAspectRatioHintId = mWindow.GetAuxiliaryHintId(RESIZE_ASPECT_RATIO_HINT);
      if(mResizeAspectRatioHintId != 0u)
      {
        mPreviousResizeAspectRatioHintValue = mWindow.GetAuxiliaryHintValue(mResizeAspectRatioHintId);
      }
      else
      {
        mResizeAspectRatioHintId = mWindow.AddAuxiliaryHint(RESIZE_ASPECT_RATIO_HINT, value);
        if(mResizeAspectRatioHintId == 0u)
        {
          return false;
        }
        mCreatedResizeAspectRatioHint = true;
        mResizeAspectRatioHintApplied = true;
        return true;
      }
    }

    mResizeAspectRatioHintApplied = mWindow.SetAuxiliaryHintValue(mResizeAspectRatioHintId, value);
    return mResizeAspectRatioHintApplied;
  }

  void RestoreResizeAspectRatioHint() override
  {
    if(!mResizeAspectRatioHintApplied || !mWindow || mResizeAspectRatioHintId == 0u)
    {
      return;
    }

    const Dali::String& restoreValue = mCreatedResizeAspectRatioHint ? RESIZE_ASPECT_RATIO_DISABLED : mPreviousResizeAspectRatioHintValue;
    if(mWindow.SetAuxiliaryHintValue(mResizeAspectRatioHintId, restoreValue))
    {
      mResizeAspectRatioHintApplied = false;
    }
  }

private:
  bool CanDispatchRequest(bool enabled, const char* operation) const
  {
    const bool dispatched = enabled && mWindow;
    LogRequest(operation, dispatched);
    return dispatched;
  }

  void LogRequest(const char* operation, bool dispatched) const
  {
    DALI_LOG_DEBUG_INFO("[WindowFrame] operation:%s:%s\n", operation, dispatched ? "dispatched" : "not-dispatched");
  }

private:
  Dali::Window             mWindow;
  CloseCallback            mCloseCallback;
  WindowFrameFeatureConfig mFeatureConfig;
  Dali::String             mPreviousResizeAspectRatioHintValue;
  unsigned int             mResizeAspectRatioHintId{0u};
  bool                     mCreatedResizeAspectRatioHint{false};
  bool                     mResizeAspectRatioHintApplied{false};
};
} // unnamed namespace

std::unique_ptr<WindowOperationAdapter> CreateDaliWindowOperationAdapter(Dali::Window                          window,
                                                                         WindowOperationAdapter::CloseCallback closeCallback,
                                                                         const WindowFrameOptions&             options)
{
  return std::make_unique<DaliWindowOperationAdapter>(window, std::move(closeCallback), options);
}

} // namespace Dali::Ui::Internal
