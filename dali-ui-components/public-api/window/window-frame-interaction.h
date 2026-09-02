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
#include <dali/public-api/common/unique-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui
{

/**
 * @brief Type of move or resize interaction initiated by decoration controls.
 *
 * Resize is initiated from explicit handles only; there are no implicit edge or
 * corner hit regions, because those need pointer hover feedback that touch and
 * remote-control input cannot provide. Any further resize direction must be
 * appended after the existing values.
 */
enum class WindowFrameInteractionType
{
  NONE                = 0, ///< No move or resize interaction
  MOVE                = 1, ///< Interactive window move
  RESIZE_BOTTOM_LEFT  = 2, ///< Resize from the bottom-left handle
  RESIZE_BOTTOM_RIGHT = 3  ///< Resize from the bottom-right handle
};

/**
 * @brief Current lifecycle state of a decoration interaction.
 */
enum class WindowFrameInteractionState
{
  STARTED          = 0, ///< Native interaction request was dispatched
  POINTER_RELEASED = 1, ///< Initiating pointer ended before native completion
  COMPLETED        = 2, ///< Native move or resize completion was reported
  CANCELLED        = 3  ///< Interaction tracking ended without completion
};

/**
 * @brief Snapshot of a move or resize interaction emitted by WindowFrame.
 */
class DALI_UI_COMPONENTS_API WindowFrameInteraction
{
public:
  /**
   * @brief Creates an empty cancelled interaction.
   */
  WindowFrameInteraction();

  WindowFrameInteraction(const WindowFrameInteraction& rhs);
  WindowFrameInteraction(WindowFrameInteraction&& rhs) noexcept;
  WindowFrameInteraction& operator=(const WindowFrameInteraction& rhs);
  WindowFrameInteraction& operator=(WindowFrameInteraction&& rhs) noexcept;
  ~WindowFrameInteraction();

  /**
   * @brief Returns the interaction type.
   *
   * @return Interaction type
   */
  WindowFrameInteractionType GetType() const;

  /**
   * @brief Returns the interaction lifecycle state.
   *
   * @return Interaction state
   */
  WindowFrameInteractionState GetState() const;

public: // Not intended for application developers
  /**
   * @brief Creates an interaction snapshot.
   *
   * @param[in] type Interaction type
   * @param[in] state Interaction lifecycle state
   */
  DALI_INTERNAL WindowFrameInteraction(WindowFrameInteractionType type, WindowFrameInteractionState state);

private:
  struct DALI_INTERNAL  Impl;
  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Dali::Ui
