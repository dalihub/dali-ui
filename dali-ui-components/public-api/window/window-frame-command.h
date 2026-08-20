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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/unique-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui
{

/**
 * @brief Commands routed through WindowFrame's command policy.
 */
enum class WindowFrameCommand
{
  MOVE     = 0,
  RESIZE   = 1,
  MINIMIZE = 2,
  MAXIMIZE = 3,
  RESTORE  = 4,
  CLOSE    = 5
};

/**
 * @brief Command and optional resize direction passed to a policy callback.
 */
class DALI_UI_COMPONENTS_API WindowFrameCommandRequest
{
public:
  WindowFrameCommandRequest(const WindowFrameCommandRequest& rhs);
  WindowFrameCommandRequest(WindowFrameCommandRequest&& rhs) noexcept;
  WindowFrameCommandRequest& operator=(const WindowFrameCommandRequest& rhs);
  WindowFrameCommandRequest& operator=(WindowFrameCommandRequest&& rhs) noexcept;
  ~WindowFrameCommandRequest();

  /**
   * @brief Returns the command.
   * @return Requested command
   */
  WindowFrameCommand GetCommand() const;

  /**
   * @brief Reads the direction carried by a resize request.
   *
   * This reports request metadata only. It does not report whether resize is
   * enabled, dispatched, or completed. The output is not modified when the request
   * has no direction.
   * @param[out] direction Resize direction supplied with the request
   * @return true when a resize direction is available
   */
  bool GetResizeDirection(Dali::WindowResizeDirection& direction) const;

public: // Not intended for application developers
  /**
   * @brief Creates a request for a command.
   * @param[in] command Command to dispatch
   */
  DALI_INTERNAL explicit WindowFrameCommandRequest(WindowFrameCommand command);

  /**
   * @brief Creates a resize command request with its native direction.
   * @param[in] command Resize command to dispatch
   * @param[in] resizeDirection Native resize direction
   * @pre command is WindowFrameCommand::RESIZE
   */
  DALI_INTERNAL WindowFrameCommandRequest(WindowFrameCommand command, Dali::WindowResizeDirection resizeDirection);

private:
  struct DALI_INTERNAL  Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Determines whether WindowFrame executes its default command.
 *
 * HANDLED means the interceptor took the command over, whether it completed it,
 * rejected it, or started its own operation. WindowFrame performs no default
 * operation and tracks nothing further for it.
 */
enum class WindowFrameCommandDisposition
{
  CONTINUE_DEFAULT = 0,
  HANDLED          = 1
};

/**
 * @brief Result of dispatching a WindowFrame command.
 *
 * DISPATCHED means the underlying request API was called. It does not mean the
 * window system accepted the request, and it does not mean the request
 * finished. The underlying calls return no acceptance result, so no such
 * guarantee can be offered. Where the outcome is observed depends on the
 * command:
 *
 * - MINIMIZE, MAXIMIZE, RESTORE report through WindowStateChangedSignal().
 *   MAXIMIZE and RESTORE additionally reach WindowStatePresentedSignal() once
 *   a frame showing the new state has been drawn. MINIMIZE never does, because
 *   a minimized window presents no frame.
 * - MOVE and RESIZE report through InteractionChangedSignal(), which reaches
 *   WindowFrameInteractionState::COMPLETED, and through
 *   GeometryChangedSignal() with GeometryChangeCompletedSignal().
 * - CLOSE has no signal of its own. It invokes the CloseCallback supplied to
 *   WindowFrame::New(), which owns what closing means.
 *
 * An explicit request is never suppressed for already being in the requested
 * state. The component would have to compare against the state the window
 * system last reported, which would drop a request sent before the previous one
 * was reported and lose the caller's last intent. A MAXIMIZE followed
 * immediately by a RESTORE therefore dispatches both, and the window ends up
 * restored. Repeating a request the window system has already applied is
 * harmless.
 *
 * CommandProcessedSignal() reports this value itself for every command, so it
 * says what was dispatched rather than what completed.
 */
enum class WindowFrameCommandResult
{
  DISPATCHED     = 0, ///< Default operation was dispatched to the underlying window or callback
  NOT_DISPATCHED = 1, ///< Default operation was unavailable or disabled
  HANDLED        = 2  ///< Command interceptor handled the command
};

} // namespace Dali::Ui
