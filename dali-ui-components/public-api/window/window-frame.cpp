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

#include <dali-ui-components/internal/window/window-frame-impl.h>
#include <dali-ui-components/public-api/window/window-frame.h>

#include <dali/integration-api/debug.h>

#include <utility>

namespace Dali::Ui
{

WindowFrame::WindowFrame()                                      = default;
WindowFrame::~WindowFrame()                                     = default;
WindowFrame::WindowFrame(const WindowFrame& rhs)                = default;
WindowFrame::WindowFrame(WindowFrame&& rhs) noexcept            = default;
WindowFrame& WindowFrame::operator=(const WindowFrame& rhs)     = default;
WindowFrame& WindowFrame::operator=(WindowFrame&& rhs) noexcept = default;

WindowFrame WindowFrame::New(Dali::Window window, CloseCallback closeCallback)
{
  return New(window, std::move(closeCallback), WindowFrameOptions());
}

WindowFrame WindowFrame::New(Dali::Window window, CloseCallback closeCallback, const WindowFrameOptions& options)
{
  if(!window)
  {
    DALI_LOG_ERROR("[WindowFrame] cannot create a frame for an empty window handle\n");
    return WindowFrame();
  }
  return WindowFrame(new Internal::WindowFrameImpl(window, std::move(closeCallback), options));
}

WindowFrame WindowFrame::DownCast(Dali::BaseHandle handle)
{
  return WindowFrame(dynamic_cast<Internal::WindowFrameImpl*>(handle.GetObjectPtr()));
}

bool WindowFrame::Attach()
{
  WindowFrame self(*this);
  return GetImplementation(self).Attach();
}
void WindowFrame::Detach()
{
  WindowFrame self(*this);
  GetImplementation(self).Detach();
}
bool WindowFrame::IsAttached() const
{
  return GetImplementation(*this).IsAttached();
}
Dali::Ui::View WindowFrame::GetContentRoot() const
{
  return GetImplementation(*this).GetContentRoot();
}
bool WindowFrame::IsFeatureEnabled(WindowFrameFeature feature) const
{
  return GetImplementation(*this).IsFeatureEnabled(feature);
}
WindowFrame::WindowState WindowFrame::GetWindowState() const
{
  return GetImplementation(*this).GetWindowState();
}
bool WindowFrame::IsMaximized() const
{
  return GetImplementation(*this).GetWindowState() == WindowFrame::WindowState::MAXIMIZED;
}
bool WindowFrame::IsMinimized() const
{
  return GetImplementation(*this).GetWindowState() == WindowFrame::WindowState::MINIMIZED;
}
WindowFrameGeometry WindowFrame::GetGeometry() const
{
  return GetImplementation(*this).GetGeometry();
}

WindowFrameDecorationResult WindowFrame::SetDecoration(const WindowFrameDecoration& decoration, const WindowFrameLayout& layout, WindowFrameSizePolicy sizePolicy)
{
  WindowFrame self(*this);
  return GetImplementation(self).SetDecoration(decoration, layout, sizePolicy);
}

WindowFrameDecorationResult WindowFrame::SetDecoration(const WindowFrameDecoration& decoration, const WindowFrameLayout& layout, const WindowFrameStyle& style, WindowFrameSizePolicy sizePolicy)
{
  WindowFrame self(*this);
  return GetImplementation(self).SetDecoration(decoration, layout, style, sizePolicy);
}

void WindowFrame::SetDecorationLayout(const WindowFrameLayout& layout, WindowFrameSizePolicy sizePolicy)
{
  WindowFrame self(*this);
  GetImplementation(self).SetDecorationLayout(layout, sizePolicy);
}

void WindowFrame::ClearDecoration()
{
  WindowFrame self(*this);
  GetImplementation(self).ClearDecoration();
}
void WindowFrame::SetDecorationVisible(bool visible)
{
  WindowFrame self(*this);
  GetImplementation(self).SetDecorationVisible(visible);
}
bool WindowFrame::IsDecorationVisible() const
{
  return GetImplementation(*this).IsDecorationVisible();
}
void WindowFrame::SetFrameStyle(const WindowFrameStyle& style, WindowFrameSizePolicy sizePolicy)
{
  WindowFrame self(*this);
  GetImplementation(self).SetFrameStyle(style, sizePolicy);
}
WindowFrameStyle WindowFrame::GetFrameStyle() const
{
  return GetImplementation(*this).GetFrameStyle();
}
void WindowFrame::SetOverlayEnabled(bool enabled)
{
  WindowFrame self(*this);
  GetImplementation(self).SetOverlayEnabled(enabled);
}
bool WindowFrame::IsOverlayEnabled() const
{
  return GetImplementation(*this).IsOverlayEnabled();
}
void WindowFrame::SetOverlayAutoHideDelay(uint32_t milliseconds)
{
  WindowFrame self(*this);
  GetImplementation(self).SetOverlayAutoHideDelay(milliseconds);
}
uint32_t WindowFrame::GetOverlayAutoHideDelay() const
{
  return GetImplementation(*this).GetOverlayAutoHideDelay();
}
void WindowFrame::ShowOverlayTemporarily()
{
  WindowFrame self(*this);
  GetImplementation(self).ShowOverlayTemporarily();
}
bool WindowFrame::IsOverlayAutoHidden() const
{
  return GetImplementation(*this).IsOverlayAutoHidden();
}
void WindowFrame::SetInteractiveResizePolicy(WindowFrameInteractiveResizePolicy policy)
{
  WindowFrame self(*this);
  GetImplementation(self).SetInteractiveResizePolicy(policy);
}
WindowFrameInteractiveResizePolicy WindowFrame::GetInteractiveResizePolicy() const
{
  return GetImplementation(*this).GetInteractiveResizePolicy();
}
bool WindowFrame::SetMinimumFrameSize(const Dali::Vector2& size)
{
  WindowFrame self(*this);
  return GetImplementation(self).SetMinimumFrameSize(size);
}
bool WindowFrame::SetMaximumFrameSize(const Dali::Vector2& size)
{
  WindowFrame self(*this);
  return GetImplementation(self).SetMaximumFrameSize(size);
}
void WindowFrame::ClearMinimumFrameSize()
{
  WindowFrame self(*this);
  GetImplementation(self).ClearMinimumFrameSize();
}
void WindowFrame::ClearMaximumFrameSize()
{
  WindowFrame self(*this);
  GetImplementation(self).ClearMaximumFrameSize();
}
bool WindowFrame::GetMinimumFrameSize(Dali::Vector2& size) const
{
  return GetImplementation(*this).GetMinimumFrameSize(size);
}
bool WindowFrame::GetMaximumFrameSize(Dali::Vector2& size) const
{
  return GetImplementation(*this).GetMaximumFrameSize(size);
}
bool WindowFrame::RequestFrameResize(const Dali::Vector2& size)
{
  WindowFrame self(*this);
  return GetImplementation(self).RequestFrameResize(size);
}
bool WindowFrame::RequestContentResize(const Dali::Vector2& size)
{
  WindowFrame self(*this);
  return GetImplementation(self).RequestContentResize(size);
}
Dali::Vector2 WindowFrame::GetFrameSize() const
{
  return GetImplementation(*this).GetFrameSize();
}
Dali::Vector2 WindowFrame::GetContentSize() const
{
  return GetImplementation(*this).GetContentSize();
}
WindowFrameCommandResult WindowFrame::RequestMinimize()
{
  WindowFrame self(*this);
  return GetImplementation(self).RequestMinimize();
}
WindowFrameCommandResult WindowFrame::RequestMaximize()
{
  WindowFrame self(*this);
  return GetImplementation(self).RequestMaximizedState(true);
}
WindowFrameCommandResult WindowFrame::RequestRestore()
{
  WindowFrame self(*this);
  return GetImplementation(self).RequestMaximizedState(false);
}
WindowFrameCommandResult WindowFrame::ToggleMaximize()
{
  WindowFrame self(*this);
  return GetImplementation(self).ToggleMaximize();
}
WindowFrameCommandResult WindowFrame::RequestClose()
{
  WindowFrame self(*this);
  return GetImplementation(self).RequestClose();
}
void WindowFrame::SetCommandInterceptor(CommandInterceptor interceptor)
{
  GetImplementation(*this).SetCommandInterceptor(std::move(interceptor));
}

WindowFrame::WindowStateChangedSignalType& WindowFrame::WindowStateChangedSignal()
{
  return GetImplementation(*this).WindowStateChangedSignal();
}
WindowFrame::WindowStatePresentedSignalType& WindowFrame::WindowStatePresentedSignal()
{
  return GetImplementation(*this).WindowStatePresentedSignal();
}
WindowFrame::GeometryChangedSignalType& WindowFrame::GeometryChangedSignal()
{
  return GetImplementation(*this).GeometryChangedSignal();
}
WindowFrame::GeometryChangeCompletedSignalType& WindowFrame::GeometryChangeCompletedSignal()
{
  return GetImplementation(*this).GeometryChangeCompletedSignal();
}
WindowFrame::DecorationVisibilityChangedSignalType& WindowFrame::DecorationVisibilityChangedSignal()
{
  return GetImplementation(*this).DecorationVisibilityChangedSignal();
}
WindowFrame::InteractionChangedSignalType& WindowFrame::InteractionChangedSignal()
{
  return GetImplementation(*this).InteractionChangedSignal();
}
WindowFrame::CommandProcessedSignalType& WindowFrame::CommandProcessedSignal()
{
  return GetImplementation(*this).CommandProcessedSignal();
}

WindowFrame::WindowFrame(Internal::WindowFrameImpl* impl)
: Dali::BaseHandle(impl)
{
}

} // namespace Dali::Ui
