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

#include <dali-ui-components/internal/dialog/dialog-presentation-session.h>
#include <dali-ui-components/integration-api/dialog/dialog-impl.h>
#include <dali-ui-components/integration-api/dialog/dialog-container-impl.h>
#include <dali-ui-components/integration-api/navigator/navigator-impl.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <algorithm>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
DialogPresentationSession::DialogPresentationSession(uint64_t sessionId, Dialog content, DialogContainer hostContainer, Navigator host)
: id(sessionId), dialog(content), container(hostContainer), navigator(host)
{
}

bool DialogPresentationSession::Register(const DialogPostOptions& options)
{
  auto keepAlive = shared_from_this();
  auto content = dialog.GetHandle();
  auto modal = container.GetHandle();
  auto host = navigator.GetHandle();
  if(!content || !modal || !host || state != State::PREPARING || content.GetParent()) return false;
  GetImpl(modal).mPresentationSession = keepAlive;
  DevelActor::OnSceneVisibilityChangedSignal(content).Connect(this, &DialogPresentationSession::OnSceneVisibilityChanged);
  modal.SetModalContent(content);
  if(state != State::PREPARING || content.GetParent() != modal || modal.GetModalContent() != content) return false;
  return NavigatorModalBridge::Register(GetImpl(host), keepAlive, options);
}

bool DialogPresentationSession::BeginClose(DialogDismissReason value)
{
  if(state != State::REGISTERED) return false;
  state = State::CLOSING;
  reason = value;
  return true;
}

void DialogPresentationSession::Dismiss(bool animated, DialogDismissReason value)
{
  auto keepAlive = shared_from_this();
  if(state != State::REGISTERED) return;
  auto host = navigator.GetHandle();
  if(host) NavigatorModalBridge::Dismiss(GetImpl(host), keepAlive, animated, value);
}

bool DialogPresentationSession::RequestDismiss(DialogDismissReason value)
{
  auto keepAlive = shared_from_this();
  auto host = navigator.GetHandle();
  auto modal = container.GetHandle();
  auto content = dialog.GetHandle();
  if(!host || !modal || !content || state != State::REGISTERED || host.GetCurrentView() != modal) return true;
  const uint32_t bit = value == DialogDismissReason::BACK ? 1u : 2u;
  if((static_cast<uint32_t>(content.GetDismissPolicy()) & bit) == 0u) return true;
  if(content.DismissRequestedSignal().EmitOr(content, value)) return true;
  if(state == State::REGISTERED && host.GetCurrentView() == modal &&
     NavigatorModalBridge::Find(GetImpl(host), modal) == keepAlive)
  {
    Dismiss(true, value);
  }
  return true;
}

void DialogPresentationSession::TryNotifyShown()
{
  auto keepAlive = shared_from_this();
  if(state != State::REGISTERED || !enterCompleted || shownSent) return;
  auto content = dialog.GetHandle();
  auto host = navigator.GetHandle();
  auto modal = container.GetHandle();
  if(!content || !host || !modal || host.GetCurrentView() != modal ||
     !content.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) ||
     !DevelActor::IsOnSceneVisible(content)) return;
  shownSent = true;
  content.ShownSignal().Emit(content);
}

void DialogPresentationSession::OnSceneVisibilityChanged(Actor, bool visible)
{
  if(visible) TryNotifyShown();
}

void DialogPresentationSession::Complete(DialogDismissReason value, bool hostDestroyed, bool containerDestroyed)
{
  auto keepAlive = shared_from_this();
  if(state == State::ENDED || completing) return;
  completing = true;
  if(value == DialogDismissReason::HOST_REMOVED || state != State::CLOSING) reason = value;
  // Never acquire a strong handle to an object inside its OnDestroy hook.
  if(hostDestroyed) navigator.Reset();
  if(containerDestroyed) container.Reset();
  auto content = dialog.GetHandle();
  auto modal = container.GetHandle();
  auto host = navigator.GetHandle();
  DisconnectAll();
  if(host) NavigatorModalBridge::Detach(GetImpl(host), id, modal);
  if(modal)
  {
    auto& impl = GetImpl(modal);
    if(impl.mPresentationSession.lock() == keepAlive)
    {
      if(content && modal.GetModalContent() == content) impl.ClearPresentationContent(*this);
      impl.mPresentationSession.reset();
    }
  }
  if(content && GetImpl(content).mPresentation->current == keepAlive)
  {
    GetImpl(content).mPresentation->current.reset();
  }
  state = State::ENDED;
  navigator.Reset();
  container.Reset();
  dialog.Reset();
  completing = false;
  if(registeredOnce && content) content.HiddenSignal().Emit(content, reason);
}

bool NavigatorModalBridge::Register(Integration::NavigatorImpl& host, std::shared_ptr<DialogPresentationSession> session, const DialogPostOptions& options)
{
  auto modal = session->container.GetHandle();
  if(host.mModalData->destroying || host.mInvokingTransitionCallback || !modal) return false;
  if(options.transitionSpec) host.SetPageModalTransitionSpec(modal, options.transitionSpec);
  bool accepted = host.PushModalInternal(modal, options.animated, session);
  if(!accepted) host.RemovePageSpec(modal);
  return accepted;
}

bool NavigatorModalBridge::Dismiss(Integration::NavigatorImpl& host, std::shared_ptr<DialogPresentationSession> session, bool animated, DialogDismissReason reason)
{
  auto modal = session->container.GetHandle();
  if(host.mModalData->destroying || host.mInvokingTransitionCallback || !modal ||
     Find(host, modal) != session || !host.InStack(host.mModalStack, modal)) return false;
  if(!session->BeginClose(reason)) return false;
  host.SettlePendingTransition();
  if(session->state == DialogPresentationSession::State::ENDED) return true;
  if(host.ModalTop() == modal) host.PopModal(animated);
  else host.Remove(modal);
  return true;
}

std::shared_ptr<DialogPresentationSession> NavigatorModalBridge::Find(Integration::NavigatorImpl& host, View modal)
{
  for(auto& session : host.mModalData->sessions)
  {
    if(session->container.GetHandle() == modal) return session;
  }
  return {};
}

void NavigatorModalBridge::Detach(Integration::NavigatorImpl& host, uint64_t id, View modal)
{
  auto& sessions = host.mModalData->sessions;
  sessions.erase(std::remove_if(sessions.begin(), sessions.end(), [&](const auto& session) {
    return session->id == id && session->container.GetHandle() == modal;
  }), sessions.end());
  // Only unregister here. The owning navigation operation performs removal.
}
} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE
