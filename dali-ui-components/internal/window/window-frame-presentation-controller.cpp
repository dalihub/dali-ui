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

#include "window-frame-presentation-controller.h"

#include <dali/public-api/signals/callback.h>

#include <map>
#include <utility>

namespace Dali::Ui::Internal
{
namespace
{
// Event-thread only. A function-local static keeps the registry alive for as
// long as any controller can reach it, regardless of static destruction order.
std::map<int32_t, WindowFramePresentationController*>& GetPresentationRegistry()
{
  static std::map<int32_t, WindowFramePresentationController*> registry;
  return registry;
}

int32_t NextFrameId()
{
  static int32_t nextFrameId = 0;
  return ++nextFrameId;
}
} // unnamed namespace

WindowFramePresentationController::WindowFramePresentationController() = default;

WindowFramePresentationController::~WindowFramePresentationController()
{
  CancelPending();
}

void WindowFramePresentationController::SetPresentedCallback(PresentedCallback callback)
{
  mPresentedCallback = std::move(callback);
}

bool WindowFramePresentationController::RequestPresentation(Dali::Window window, WindowFrame::WindowState state)
{
  // A minimized window presents no frame to wait for.
  if(!window || state == WindowFrame::WindowState::MINIMIZED)
  {
    return false;
  }

  // Only the newest request is tracked; an older one can no longer describe the
  // state that will actually be presented.
  CancelPending();
  mPendingState   = state;
  mPendingFrameId = NextFrameId();
  GetPresentationRegistry().emplace(mPendingFrameId, this);

  window.AddFramePresentedCallback(Dali::MakeCallback(&WindowFramePresentationController::DispatchFramePresented), mPendingFrameId);
  return true;
}

void WindowFramePresentationController::CancelPending()
{
  if(mPendingFrameId != 0)
  {
    GetPresentationRegistry().erase(mPendingFrameId);
    mPendingFrameId = 0;
  }
}

void WindowFramePresentationController::DispatchFramePresented(int32_t frameId)
{
  auto&      registry = GetPresentationRegistry();
  const auto found    = registry.find(frameId);
  if(found == registry.end())
  {
    return;
  }

  WindowFramePresentationController* controller = found->second;
  registry.erase(found);
  controller->mPendingFrameId = 0;
  if(controller->mPresentedCallback)
  {
    controller->mPresentedCallback(controller->mPendingState);
  }
}

} // namespace Dali::Ui::Internal
