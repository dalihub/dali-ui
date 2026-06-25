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
#include <dali-ui-components/public-api/dialog/dialog-properties.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class DialogContainerImpl;
}

/**
 * @brief DialogContainer presents modal content over a tap-to-dismiss scrim.
 *
 * It owns a scrim (a dim, clickable overlay that fills the container) and a modal
 * content slot placed above the scrim. The content's position and size follow the
 * content's own layout parameters (the container lays its children out with an
 * AbsoluteLayout); when none are set, the content is placed at the top-left at its
 * natural size. To center the content, give it AbsoluteLayoutParams with
 * POSITION_PROPORTIONAL bounds. Tapping the scrim emits ScrimClickedSignal;
 * Navigator connects this to PopModal so the dialog is dismissed when the
 * background is tapped.
 */
class DALI_UI_API DialogContainer : public View
{
public:
  DialogContainer();
  static DialogContainer New();
  DialogContainer(const DialogContainer& dialogContainer);
  DialogContainer(DialogContainer&& rhs) noexcept;
  ~DialogContainer();
  DialogContainer& operator=(const DialogContainer& handle);
  DialogContainer& operator=(DialogContainer&& rhs) noexcept;

  DALI_UI_VIEW_WITH(DialogContainer)

  static DialogContainer DownCast(BaseHandle handle);

public: // Content
  /**
   * @brief Sets the modal content (e.g. a Dialog), placed above the scrim.
   *
   * The content's position and size follow its own layout parameters; set
   * AbsoluteLayoutParams with POSITION_PROPORTIONAL to center it.
   * @param[in] modalContent The content view, or an empty handle to clear it
   */
  void SetModalContent(View modalContent);

  /**
   * @brief Gets the modal content.
   * @return The modal content, or an empty handle if none is set
   */
  View GetModalContent() const;

  /**
   * @brief Replaces the built-in scrim with a custom view.
   * @param[in] scrim The scrim view (kept below the modal content)
   */
  void SetScrim(View scrim);

  /**
   * @brief Gets the scrim view.
   * @return The scrim view
   */
  View GetScrim() const;

public: // Signals
  using ScrimClickedSignalType = Signal<void(DialogContainer)>;

  /**
   * @brief Emitted when the scrim is tapped (used for tap-to-dismiss).
   * @return The signal to connect to
   */
  ScrimClickedSignalType& ScrimClickedSignal();

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL          DialogContainer(Integration::DialogContainerImpl& implementation);
  explicit DALI_INTERNAL DialogContainer(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
