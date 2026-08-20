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

#include <dali-ui-components/public-api/window/window-frame-interaction.h>

#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_WINDOW_FRAME_INTERACTION(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from WindowFrameInteraction object")

namespace Dali::Ui
{

struct WindowFrameInteraction::Impl
{
  WindowFrameInteractionType  type{WindowFrameInteractionType::NONE};
  WindowFrameInteractionState state{WindowFrameInteractionState::CANCELLED};
};

WindowFrameInteraction::WindowFrameInteraction()
: mImpl(Dali::MakeUnique<Impl>())
{
}

WindowFrameInteraction::WindowFrameInteraction(WindowFrameInteractionType type, WindowFrameInteractionState state)
: mImpl(Dali::MakeUnique<Impl>())
{
  mImpl->type  = type;
  mImpl->state = state;
}

WindowFrameInteraction::WindowFrameInteraction(const WindowFrameInteraction& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_INTERACTION(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

WindowFrameInteraction::WindowFrameInteraction(WindowFrameInteraction&& rhs) noexcept = default;

WindowFrameInteraction& WindowFrameInteraction::operator=(const WindowFrameInteraction& rhs)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_INTERACTION(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  return *this;
}

WindowFrameInteraction& WindowFrameInteraction::operator=(WindowFrameInteraction&& rhs) noexcept = default;
WindowFrameInteraction::~WindowFrameInteraction()                                                = default;

WindowFrameInteractionType WindowFrameInteraction::GetType() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_INTERACTION(mImpl);
  return mImpl->type;
}

WindowFrameInteractionState WindowFrameInteraction::GetState() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_INTERACTION(mImpl);
  return mImpl->state;
}

} // namespace Dali::Ui

#undef DALI_ASSERT_VALID_WINDOW_FRAME_INTERACTION
