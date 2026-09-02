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

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/selection-animation-mode.h>
#include <dali-ui-components/public-api/styles/switch-style.h>
#include <dali-ui-foundation/public-api/views/selectable-view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class SwitchImpl;
}

/**
 * @brief A binary selectable control rendered as a track and movable handle.
 *
 * Selection state, click-to-toggle, SelectionChangedSignal, and enabled state
 * are inherited from SelectableView. Horizontal dragging is provided by the
 * component, while focused execution keys use the inherited selectable path.
 */
class DALI_UI_COMPONENTS_API Switch : public SelectableView
{
public:
  Switch();
  ~Switch();

  static Switch New();
  static Switch New(SwitchStyle style);
  static Switch DownCast(BaseHandle handle);

  Switch(const Switch& handle);
  Switch(Switch&& rhs) noexcept;
  Switch& operator=(const Switch& handle);
  Switch& operator=(Switch&& rhs) noexcept;

  DALI_UI_VIEW_WITH(Switch)

  /**
   * @brief Sets/gets how a selection-state change animates (default AUTO).
   */
  void                   SetSelectionAnimationMode(SelectionAnimationMode mode);
  SelectionAnimationMode GetSelectionAnimationMode() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL Switch(Internal::SwitchImpl& implementation);
  explicit DALI_INTERNAL Switch(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
