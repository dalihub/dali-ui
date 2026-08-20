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

#include <dali-ui-components/public-api/window/window-frame-interaction.h>

#include <optional>

namespace Dali::Ui::Internal
{

const char* WindowFrameInteractionTypeName(WindowFrameInteractionType type);
const char* WindowFrameInteractionStateName(WindowFrameInteractionState state);

class WindowFrameInteractionController
{
public:
  bool CanBegin() const;
  bool IsMoveActive() const;
  bool IsResizeActive() const;

  /**
   * @brief Returns whether the pointer that started the interaction was released.
   *
   * A compositor grab can take window focus while the pointer is still down.
   * Focus handling uses this to tell an in-progress grab apart from an
   * interaction whose completion event never arrived.
   */
  bool IsPointerReleased() const;

  std::optional<WindowFrameInteraction> Begin(WindowFrameInteractionType type);
  std::optional<WindowFrameInteraction> PointerEnded(WindowFrameInteractionType type, bool interrupted);
  std::optional<WindowFrameInteraction> CompleteMove();
  std::optional<WindowFrameInteraction> CompleteResize();
  std::optional<WindowFrameInteraction> Cancel();

private:
  std::optional<WindowFrameInteraction> Complete(bool move);
  WindowFrameInteraction                MakeInteraction(WindowFrameInteractionState state) const;
  void                                  Reset();

private:
  WindowFrameInteractionType mType{WindowFrameInteractionType::NONE};
  bool                       mPointerReleased{false};
};

} // namespace Dali::Ui::Internal
