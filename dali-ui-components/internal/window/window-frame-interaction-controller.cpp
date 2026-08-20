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

#include "window-frame-interaction-controller.h"

namespace Dali::Ui::Internal
{

bool WindowFrameInteractionController::CanBegin() const
{
  return mType == WindowFrameInteractionType::NONE;
}

bool WindowFrameInteractionController::IsMoveActive() const
{
  return mType == WindowFrameInteractionType::MOVE;
}

bool WindowFrameInteractionController::IsResizeActive() const
{
  return mType == WindowFrameInteractionType::RESIZE_BOTTOM_LEFT || mType == WindowFrameInteractionType::RESIZE_BOTTOM_RIGHT;
}

std::optional<WindowFrameInteraction> WindowFrameInteractionController::Begin(WindowFrameInteractionType type)
{
  if(!CanBegin() || type == WindowFrameInteractionType::NONE)
  {
    return std::nullopt;
  }

  mType            = type;
  mPointerReleased = false;
  return MakeInteraction(WindowFrameInteractionState::STARTED);
}

bool WindowFrameInteractionController::IsPointerReleased() const
{
  return mPointerReleased;
}

std::optional<WindowFrameInteraction> WindowFrameInteractionController::PointerEnded(WindowFrameInteractionType type, bool interrupted)
{
  if(mType != type)
  {
    return std::nullopt;
  }

  const WindowFrameInteraction interaction = MakeInteraction(interrupted ? WindowFrameInteractionState::CANCELLED : WindowFrameInteractionState::POINTER_RELEASED);
  if(interrupted)
  {
    Reset();
  }
  else
  {
    mPointerReleased = true;
  }
  return interaction;
}

std::optional<WindowFrameInteraction> WindowFrameInteractionController::CompleteMove()
{
  return Complete(true);
}

std::optional<WindowFrameInteraction> WindowFrameInteractionController::CompleteResize()
{
  return Complete(false);
}

std::optional<WindowFrameInteraction> WindowFrameInteractionController::Cancel()
{
  if(CanBegin())
  {
    return std::nullopt;
  }

  const WindowFrameInteraction interaction = MakeInteraction(WindowFrameInteractionState::CANCELLED);
  Reset();
  return interaction;
}

std::optional<WindowFrameInteraction> WindowFrameInteractionController::Complete(bool move)
{
  if((move && !IsMoveActive()) || (!move && !IsResizeActive()))
  {
    return std::nullopt;
  }

  const WindowFrameInteraction interaction = MakeInteraction(WindowFrameInteractionState::COMPLETED);
  Reset();
  return interaction;
}

WindowFrameInteraction WindowFrameInteractionController::MakeInteraction(WindowFrameInteractionState state) const
{
  return WindowFrameInteraction{mType, state};
}

void WindowFrameInteractionController::Reset()
{
  mType            = WindowFrameInteractionType::NONE;
  mPointerReleased = false;
}

const char* WindowFrameInteractionTypeName(WindowFrameInteractionType type)
{
  switch(type)
  {
    case WindowFrameInteractionType::NONE:
      return "none";
    case WindowFrameInteractionType::MOVE:
      return "move";
    case WindowFrameInteractionType::RESIZE_BOTTOM_LEFT:
      return "resize-bottom-left";
    case WindowFrameInteractionType::RESIZE_BOTTOM_RIGHT:
      return "resize-bottom-right";
  }
  return "unknown";
}

const char* WindowFrameInteractionStateName(WindowFrameInteractionState state)
{
  switch(state)
  {
    case WindowFrameInteractionState::STARTED:
      return "started";
    case WindowFrameInteractionState::POINTER_RELEASED:
      return "pointer-released";
    case WindowFrameInteractionState::COMPLETED:
      return "completed";
    case WindowFrameInteractionState::CANCELLED:
      return "cancelled";
  }
  return "unknown";
}

} // namespace Dali::Ui::Internal
