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

#include <dali-ui-components/public-api/window/window-frame.h>

#include <dali/public-api/adaptor-framework/window.h>

#include <cstdint>
#include <functional>

namespace Dali::Ui::Internal
{
// Routes Window::AddFramePresentedCallback() back to the owning WindowFrame.
//
// The native API takes a free function plus an integer id, so a registry maps
// the id back to its controller.  Everything here runs on the event thread;
// the registry only exists so that a destroyed controller cannot be called.
class WindowFramePresentationController
{
public:
  using PresentedCallback = std::function<void(WindowFrame::WindowState)>;

  WindowFramePresentationController();
  ~WindowFramePresentationController();

  WindowFramePresentationController(const WindowFramePresentationController&)            = delete;
  WindowFramePresentationController(WindowFramePresentationController&&)                 = delete;
  WindowFramePresentationController& operator=(const WindowFramePresentationController&) = delete;
  WindowFramePresentationController& operator=(WindowFramePresentationController&&)      = delete;

  void SetPresentedCallback(PresentedCallback callback);
  bool RequestPresentation(Dali::Window window, WindowFrame::WindowState state);
  void CancelPending();

private:
  static void DispatchFramePresented(int32_t frameId);

private:
  PresentedCallback        mPresentedCallback;
  WindowFrame::WindowState mPendingState{WindowFrame::WindowState::NORMAL};
  int32_t                  mPendingFrameId{0};
};

} // namespace Dali::Ui::Internal
