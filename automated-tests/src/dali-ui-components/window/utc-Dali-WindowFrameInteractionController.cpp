/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-ui-components/internal/window/window-frame-interaction-controller.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

int UtcDaliWindowFrameInteractionMoveBlocksConcurrentResize(void)
{
  WindowFrameInteractionController controller;

  const auto started = controller.Begin(WindowFrameInteractionType::MOVE);
  DALI_TEST_CHECK(started && started->GetState() == WindowFrameInteractionState::STARTED);
  DALI_TEST_CHECK(!controller.CanBegin());

  const auto released = controller.PointerEnded(WindowFrameInteractionType::MOVE, false);
  DALI_TEST_CHECK(released && released->GetState() == WindowFrameInteractionState::POINTER_RELEASED);
  DALI_TEST_CHECK(!controller.CanBegin());

  const auto completed = controller.CompleteMove();
  DALI_TEST_CHECK(completed && completed->GetState() == WindowFrameInteractionState::COMPLETED);
  DALI_TEST_CHECK(controller.CanBegin());
  END_TEST;
}

int UtcDaliWindowFrameInteractionResizeDirectionAndCompletion(void)
{
  WindowFrameInteractionController controller;
  controller.Begin(WindowFrameInteractionType::RESIZE_BOTTOM_RIGHT);

  DALI_TEST_CHECK(!controller.CompleteMove());

  const auto completion = controller.CompleteResize();
  DALI_TEST_CHECK(completion);
  DALI_TEST_CHECK(completion->GetType() == WindowFrameInteractionType::RESIZE_BOTTOM_RIGHT);
  END_TEST;
}

int UtcDaliWindowFrameInteractionInterruptedAndInvalidBeginN(void)
{
  WindowFrameInteractionController controller;

  DALI_TEST_CHECK(!controller.Begin(WindowFrameInteractionType::NONE));
  DALI_TEST_CHECK(controller.CanBegin());

  controller.Begin(WindowFrameInteractionType::RESIZE_BOTTOM_LEFT);
  const auto interrupted = controller.PointerEnded(WindowFrameInteractionType::RESIZE_BOTTOM_LEFT, true);
  DALI_TEST_CHECK(interrupted && interrupted->GetState() == WindowFrameInteractionState::CANCELLED);
  DALI_TEST_CHECK(controller.CanBegin());
  END_TEST;
}

int UtcDaliWindowFrameInteractionPointerReleaseTracking(void)
{
  WindowFrameInteractionController controller;
  controller.Begin(WindowFrameInteractionType::MOVE);

  // A compositor grab can take window focus while the pointer is still down.
  // Focus handling relies on this flag to leave that interaction running.
  DALI_TEST_CHECK(!controller.IsPointerReleased());

  controller.PointerEnded(WindowFrameInteractionType::MOVE, false);
  DALI_TEST_CHECK(controller.IsPointerReleased());

  controller.CompleteMove();
  DALI_TEST_CHECK(!controller.IsPointerReleased());

  WindowFrameInteractionController interrupted;
  interrupted.Begin(WindowFrameInteractionType::RESIZE_BOTTOM_LEFT);
  interrupted.PointerEnded(WindowFrameInteractionType::RESIZE_BOTTOM_LEFT, true);
  DALI_TEST_CHECK(!interrupted.IsPointerReleased());
  END_TEST;
}

int UtcDaliWindowFrameInteractionExternalCancellation(void)
{
  WindowFrameInteractionController controller;
  controller.Begin(WindowFrameInteractionType::MOVE);

  const auto cancelled = controller.Cancel();
  DALI_TEST_CHECK(cancelled && cancelled->GetState() == WindowFrameInteractionState::CANCELLED);
  DALI_TEST_CHECK(!controller.Cancel());
  END_TEST;
}
