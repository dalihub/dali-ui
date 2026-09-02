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

#include <dali-ui-components/public-api/window/window-frame-command.h>

#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_WINDOW_FRAME_COMMAND(impl, Type) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from " Type " object")

namespace Dali::Ui
{

struct WindowFrameCommandRequest::Impl
{
  WindowFrameCommand          command{WindowFrameCommand::MOVE};
  Dali::WindowResizeDirection resizeDirection{Dali::WindowResizeDirection::BOTTOM_RIGHT};
  bool                        hasResizeDirection{false};
};

WindowFrameCommandRequest::WindowFrameCommandRequest(WindowFrameCommand command)
: mImpl(Dali::MakeUnique<Impl>())
{
  mImpl->command = command;
}

WindowFrameCommandRequest::WindowFrameCommandRequest(WindowFrameCommand command, Dali::WindowResizeDirection resizeDirection)
: mImpl(Dali::MakeUnique<Impl>())
{
  mImpl->command            = command;
  mImpl->resizeDirection    = resizeDirection;
  mImpl->hasResizeDirection = true;
}

WindowFrameCommandRequest::WindowFrameCommandRequest(const WindowFrameCommandRequest& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_COMMAND(rhs.mImpl, "WindowFrameCommandRequest");
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

WindowFrameCommandRequest::WindowFrameCommandRequest(WindowFrameCommandRequest&& rhs) noexcept = default;

WindowFrameCommandRequest& WindowFrameCommandRequest::operator=(const WindowFrameCommandRequest& rhs)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_COMMAND(rhs.mImpl, "WindowFrameCommandRequest");
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  return *this;
}

WindowFrameCommandRequest& WindowFrameCommandRequest::operator=(WindowFrameCommandRequest&& rhs) noexcept = default;
WindowFrameCommandRequest::~WindowFrameCommandRequest()                                                   = default;

WindowFrameCommand WindowFrameCommandRequest::GetCommand() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_COMMAND(mImpl, "WindowFrameCommandRequest");
  return mImpl->command;
}

bool WindowFrameCommandRequest::GetResizeDirection(Dali::WindowResizeDirection& direction) const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_COMMAND(mImpl, "WindowFrameCommandRequest");
  if(!mImpl->hasResizeDirection)
  {
    return false;
  }

  direction = mImpl->resizeDirection;
  return true;
}

} // namespace Dali::Ui

#undef DALI_ASSERT_VALID_WINDOW_FRAME_COMMAND
