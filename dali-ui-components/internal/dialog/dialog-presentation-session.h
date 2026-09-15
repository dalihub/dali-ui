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
 */

#include <dali-ui-components/public-api/dialog/dialog.h>
#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-components/public-api/navigator/navigator.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <memory>
#include <vector>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
class NavigatorImpl;
}
namespace Internal
{
class DialogPresentationSession;

struct DialogPresentationData
{
  DialogDismissPolicy policy{DialogDismissPolicy::BACK_AND_SCRIM};
  Dialog::DismissRequestedSignalType requested;
  Dialog::ShownSignalType shown;
  Dialog::HiddenSignalType hidden;
  std::shared_ptr<DialogPresentationSession> current;
  uint64_t nextId{0u};
};

class DialogPresentationSession : public ConnectionTracker,
                                  public std::enable_shared_from_this<DialogPresentationSession>
{
public:
  enum class State { PREPARING, REGISTERED, CLOSING, ENDED };
  DialogPresentationSession(uint64_t id, Dialog dialog, DialogContainer container, Navigator navigator);
  bool Register(const DialogPostOptions& options);
  bool BeginClose(DialogDismissReason reason);
  void Dismiss(bool animated, DialogDismissReason reason);
  bool RequestDismiss(DialogDismissReason reason);
  void TryNotifyShown();
  void Complete(DialogDismissReason reason, bool hostDestroyed = false, bool containerDestroyed = false);
  void OnSceneVisibilityChanged(Actor actor, bool visible);

  uint64_t id;
  State state{State::PREPARING};
  DialogDismissReason reason{DialogDismissReason::PROGRAMMATIC};
  bool registeredOnce{false};
  bool enterCompleted{false};
  bool shownSent{false};
  bool completing{false};
  WeakHandle<Dialog> dialog;
  WeakHandle<DialogContainer> container;
  WeakHandle<Navigator> navigator;
};

struct NavigatorModalData
{
  struct Removal
  {
    const RefObject* target;
    const DialogPresentationSession* session;
  };
  std::vector<std::shared_ptr<DialogPresentationSession>> sessions;
  uint64_t revision{0u};
  uint64_t transitionId{0u};
  bool destroying{false};
  std::vector<Removal> removing;
};

struct NavigatorModalBridge
{
  static bool Register(Integration::NavigatorImpl& host, std::shared_ptr<DialogPresentationSession> session, const DialogPostOptions& options);
  static bool Dismiss(Integration::NavigatorImpl& host, std::shared_ptr<DialogPresentationSession> session, bool animated, DialogDismissReason reason);
  static std::shared_ptr<DialogPresentationSession> Find(Integration::NavigatorImpl& host, View container);
  static void Detach(Integration::NavigatorImpl& host, uint64_t id, View container);
};
} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE
