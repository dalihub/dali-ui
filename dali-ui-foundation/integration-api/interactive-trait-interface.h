#pragma once

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
 *
 */

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/public-api/events/key-event.h>

namespace Dali
{

namespace Ui
{

class View;

/**
 * @brief Interface for interaction traits attached to a View.
 *
 * An interaction trait defines how a View reacts to input focus and key events
 * (e.g., clickable, selectable, group-selectable traits used by button-like controls).
 * A single interaction trait is attached to a View via the reserved
 * @c ReservedTraitId::INTERACTION_TRAIT identifier.
 */
class DALI_UI_API InteractiveTraitInterface
{
public:
  /**
   * @brief Called after a key-event is received by the actor that has had its focus set.
   *
   * @param[in] view The sender view
   * @param[in] event The key event
   * @return True if the event should be consumed
   */
  virtual bool OnKeyEvent(View view, const KeyEvent& event) = 0;

  /**
   * @brief Called when the view gains or loses key input focus.
   *
   * @param[in] view The sender view
   * @param[in] focused True if it gained focus
   */
  virtual void OnFocusedChanged(View view, bool focused) = 0;

  /**
   * @brief Called when the view becomes enabled or disabled.
   *
   * Allows the trait to clean up interaction state (e.g. releasing pressed
   * state when the view becomes disabled).
   *
   * @param[in] view The sender view
   * @param[in] enabled True if enabled, false if disabled
   */
  virtual void OnEnabledChanged(View view, bool enabled) = 0;

  /**
   * @brief Called when the view is connected to a scene.
   *
   * @param[in] view The sender view
   */
  virtual void OnSceneConnection(View view) = 0;

  /**
   * @brief Called when the view is disconnected from a scene.
   *
   * Allows the trait to clean up pending interaction state when input release
   * events can no longer be delivered to the view.
   *
   * @param[in] view The sender view
   */
  virtual void OnSceneDisconnection(View view) = 0;
};

} // namespace Ui

} // namespace Dali
