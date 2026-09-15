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

// CLASS HEADER
#include <dali-ui-components/integration-api/navigator/navigator-impl.h>
#include <dali-ui-components/internal/dialog/dialog-presentation-session.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/absolute-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/object/property.h>
#include <algorithm>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
using Internal::DialogPresentationSession;
using Internal::NavigatorModalBridge;
namespace
{
constexpr float TRANSITION_DURATION = 0.25f; // seconds

NavigationTransitionSpec::AnimatorSignalType& SelectAnimatorSignal(NavigationTransitionSpec& spec, bool byPop, bool isIncoming)
{
  if(isIncoming)
  {
    return byPop ? spec.PopEnterSignal() : spec.EnterSignal();
  }
  return byPop ? spec.PopExitSignal() : spec.ExitSignal();
}

NavigationTransitionSpec::SnapSignalType& SelectSnapSignal(NavigationTransitionSpec& spec, bool isIncoming)
{
  return isIncoming ? spec.SnapIncomingSignal() : spec.SnapOutgoingSignal();
}

class ScopedTransitionCallback
{
public:
  explicit ScopedTransitionCallback(bool& active)
  : mActive(active)
  {
    mActive = true;
  }

  ~ScopedTransitionCallback()
  {
    mActive = false;
  }

private:
  bool& mActive;
};

// Register the type with ViewImpl as the base so instances inherit View's
// (animatable) properties such as viewEffectiveScale, which ViewImpl::Measure
// reads for every view. Without this, measuring a Navigator throws.
BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(NavigatorImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()
} // anonymous namespace

Ui::Navigator NavigatorImpl::New()
{
  IntrusivePtr<NavigatorImpl> impl = new NavigatorImpl();

  Ui::Navigator handle = Ui::Navigator(*impl);

  impl->Initialize();

  return handle;
}

NavigatorImpl::NavigatorImpl()
: ViewImpl(),
  mModalData(new Internal::NavigatorModalData())
{
}

NavigatorImpl::~NavigatorImpl()
{
  if(mTransition)
  {
    mTransition.Stop();
  }
}

void NavigatorImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  // Absolute layout so pushed pages (MATCH_PARENT) fill the navigator.
  AttachLayoutManager(Dali::MakeUnique<AbsoluteLayoutManager>());
}

// =============================================================================
// Navigation stack
// =============================================================================

void NavigatorImpl::Push(Ui::View page, bool animated)
{
  if(mInvokingTransitionCallback || mModalData->destroying || !page || InAnyStack(page))
  {
    return;
  }
  SettlePendingTransition();

  if(InAnyStack(page)) return;

  Ui::Navigator handle = GetHandle();
  Ui::View      prev   = NavTop();

  mNavStack.push_back(page);
  ++mModalData->revision;
  const auto transitionId = ++mModalData->transitionId;
  mTxIncoming             = page;
  mTxOutgoing             = prev;
  mTxByPop                = false;
  mTxRemoveOutgoing       = false;
  mTxByModal              = false;
  AddChildFill(page);
  if(transitionId != mModalData->transitionId) return;
  RestackModals();
  UpdateVisibility();
  if(transitionId != mModalData->transitionId) return;

  if(prev && handle)
  {
    mPageWillDisappearSignal.Emit(handle, prev, false);
  }
  if(transitionId != mModalData->transitionId) return;
  if(handle)
  {
    mPageWillAppearSignal.Emit(handle, page, false);
  }
  if(transitionId != mModalData->transitionId) return;
  RunTransition(animated && mPageTransitionAnimationEnabled, true);
}

Ui::View NavigatorImpl::Pop(bool animated)
{
  // The last page may be popped, leaving the stack empty; only an already-empty
  // stack returns an empty handle.
  if(mInvokingTransitionCallback || mModalData->destroying || mNavStack.empty())
  {
    return Ui::View();
  }
  Ui::Navigator handle = GetHandle();
  Ui::View      top    = mNavStack.back();
  SettlePendingTransition();
  if(!InStack(mNavStack, top)) return top;
  if(NavTop() != top)
  {
    Remove(top);
    return top;
  }
  mNavStack.pop_back();
  ++mModalData->revision;
  Ui::View   newTop       = NavTop();
  const auto transitionId = ++mModalData->transitionId;
  mTxIncoming             = newTop;
  mTxOutgoing             = top;
  mTxByPop                = true;
  mTxRemoveOutgoing       = true;
  mTxByModal              = false;

  UpdateVisibility();
  RestackModals();
  if(transitionId != mModalData->transitionId) return top;

  if(handle)
  {
    mPageWillDisappearSignal.Emit(handle, top, true);
  }
  if(transitionId != mModalData->transitionId) return top;
  if(newTop && handle)
  {
    mPageWillAppearSignal.Emit(handle, newTop, true);
  }
  if(transitionId != mModalData->transitionId) return top;
  RunTransition(animated && mPageTransitionAnimationEnabled, false);
  return top;
}

void NavigatorImpl::InsertBefore(Ui::View page, Ui::View before)
{
  if(mInvokingTransitionCallback || !page || InAnyStack(page))
  {
    return;
  }
  auto it = std::find(mNavStack.begin(), mNavStack.end(), before);
  if(it == mNavStack.end())
  {
    return;
  }
  mNavStack.insert(it, page);
  AddChildFill(page);
  RestackModals();
  UpdateVisibility(); // inserted below the top -> hidden
}

void NavigatorImpl::Remove(Ui::View page)
{
  if(mInvokingTransitionCallback || !page)
  {
    return;
  }

  auto it = std::find(mNavStack.begin(), mNavStack.end(), page);
  if(it != mNavStack.end())
  {
    if(page == NavTop())
    {
      Pop(false);
      return;
    }
    mNavStack.erase(it);
    DetachChild(page);
    RemovePageSpec(page);
    UpdateVisibility();
    return;
  }

  auto mit = std::find(mModalStack.begin(), mModalStack.end(), page);
  if(mit != mModalStack.end())
  {
    if(page == ModalTop())
    {
      PopModal(false);
      return;
    }
    // Lifecycle callbacks below may release the caller's last Navigator handle.
    auto handle  = GetHandle();
    auto session = NavigatorModalBridge::Find(*this, page);
    if(session) session->BeginClose(DialogDismissReason::PROGRAMMATIC);
    if(mTxByModal && mTxOutgoing == page)
    {
      // An Exit callback may be animating this covered content. Stop it before
      // Hidden can detach/repost the same Dialog. Settling emits callbacks, so
      // neither an iterator nor stack membership can be kept across this call.
      const auto visual = mTxOutgoingVisual;
      do
      {
        SettlePendingTransition();
        mit = std::find(mModalStack.begin(), mModalStack.end(), page);
        if(mit == mModalStack.end()) return;
        // Completion callbacks may start another transition on this content.
      } while(mTxByModal && mTxOutgoing == page);
      RestoreModalContent(visual);
      if(page == ModalTop())
      {
        PopModal(false);
        return;
      }
    }
    mModalStack.erase(mit);
    ++mModalData->revision;
    DetachChild(page);
    RemovePageSpec(page);
    UpdateVisibility();
    if(session) session->Complete(DialogDismissReason::PROGRAMMATIC);
  }
}

void NavigatorImpl::Clear()
{
  if(mInvokingTransitionCallback || mModalData->destroying)
  {
    return;
  }

  auto handle   = GetHandle();
  auto modals   = mModalStack;
  auto pages    = mNavStack;
  auto sessions = mModalData->sessions;
  for(auto& session : sessions) session->BeginClose(DialogDismissReason::PROGRAMMATIC);
  SettlePendingTransition();
  for(auto& v : modals)
  {
    mModalStack.erase(std::remove(mModalStack.begin(), mModalStack.end(), v), mModalStack.end());
  }
  for(auto& v : pages)
  {
    mNavStack.erase(std::remove(mNavStack.begin(), mNavStack.end(), v), mNavStack.end());
  }
  ++mModalData->revision;
  for(auto& v : modals)
  {
    DetachChild(v);
    RemovePageSpec(v);
  }
  for(auto& v : pages)
  {
    DetachChild(v);
    RemovePageSpec(v);
  }
  UpdateVisibility();
  for(auto& session : sessions) session->Complete(DialogDismissReason::PROGRAMMATIC);
}

// =============================================================================
// Modal stack
// =============================================================================

void NavigatorImpl::PushModal(Ui::View modal, bool animated)
{
  PushModalInternal(modal, animated, {});
}

bool NavigatorImpl::PushModalInternal(Ui::View modal, bool animated, std::shared_ptr<DialogPresentationSession> session)
{
  if(mInvokingTransitionCallback || mModalData->destroying || !modal || InAnyStack(modal))
  {
    return false;
  }
  Ui::Navigator handle = GetHandle();
  SettlePendingTransition();
  if(InAnyStack(modal) || (session && session->state != DialogPresentationSession::State::PREPARING)) return false;

  Ui::View prevModal = ModalTop();

  mModalStack.push_back(modal);
  ++mModalData->revision;
  if(session)
  {
    mModalData->sessions.push_back(session);
    session->state          = DialogPresentationSession::State::REGISTERED;
    session->registeredOnce = true;
  }
  const auto transitionId = ++mModalData->transitionId;
  Ui::View   disappearing = prevModal ? prevModal : NavTop();
  mTxIncoming             = modal;
  mTxOutgoing             = disappearing;
  mTxByPop                = false;
  mTxRemoveOutgoing       = false;
  mTxByModal              = true;
  AddChildFill(modal);
  RestackModals();
  if(transitionId != mModalData->transitionId) return true;

  // Wire tap-to-dismiss when the modal is a DialogContainer.
  Ui::DialogContainer dialogContainer = Ui::DialogContainer::DownCast(modal);
  if(dialogContainer)
  {
    dialogContainer.ScrimClickedSignal().Connect(this, &NavigatorImpl::OnScrimClicked);
  }

  UpdateVisibility();
  if(transitionId != mModalData->transitionId) return true;

  // The view being covered is the previous modal, or (for the first modal) the
  // current navigation-stack top.
  if(disappearing && handle)
  {
    mPageWillDisappearSignal.Emit(handle, disappearing, false);
  }
  if(transitionId != mModalData->transitionId) return true;
  if(handle)
  {
    mPageWillAppearSignal.Emit(handle, modal, false);
  }
  if(transitionId != mModalData->transitionId) return true;
  RunTransition(animated && mModalTransitionAnimationEnabled, true);
  return true;
}

Ui::View NavigatorImpl::PopModal(bool animated)
{
  if(mInvokingTransitionCallback || mModalData->destroying || mModalStack.empty())
  {
    return Ui::View();
  }
  Ui::Navigator handle  = GetHandle();
  Ui::View      top     = mModalStack.back();
  auto          session = NavigatorModalBridge::Find(*this, top);
  if(session) session->BeginClose(DialogDismissReason::PROGRAMMATIC);
  SettlePendingTransition();
  if(!InStack(mModalStack, top)) return top;
  if(ModalTop() != top)
  {
    Remove(top);
    return top;
  }
  mModalStack.pop_back();
  ++mModalData->revision;
  Ui::View   newModalTop  = ModalTop();
  Ui::View   appearing    = newModalTop ? newModalTop : NavTop();
  const auto transitionId = ++mModalData->transitionId;
  mTxIncoming             = appearing;
  mTxOutgoing             = top;
  mTxByPop                = true;
  mTxRemoveOutgoing       = true;
  mTxByModal              = true;

  UpdateVisibility();
  RestackModals();
  if(transitionId != mModalData->transitionId) return top;

  // The view revealed is the new modal top, or (when the modal stack is now
  // empty) the navigation-stack top.
  if(handle)
  {
    mPageWillDisappearSignal.Emit(handle, top, true);
  }
  if(transitionId != mModalData->transitionId) return top;
  if(appearing && handle)
  {
    mPageWillAppearSignal.Emit(handle, appearing, true);
  }
  if(transitionId != mModalData->transitionId) return top;
  RunTransition(animated && mModalTransitionAnimationEnabled, false);
  return top;
}

// =============================================================================
// Queries
// =============================================================================

Ui::View NavigatorImpl::GetCurrentView() const
{
  if(!mModalStack.empty())
  {
    return mModalStack.back();
  }
  if(!mNavStack.empty())
  {
    return mNavStack.back();
  }
  return Ui::View();
}

uint32_t NavigatorImpl::GetNavigationStackCount() const
{
  return static_cast<uint32_t>(mNavStack.size());
}

uint32_t NavigatorImpl::GetModalStackCount() const
{
  return static_cast<uint32_t>(mModalStack.size());
}

Ui::View NavigatorImpl::GetNavigationStackItem(uint32_t index) const
{
  return (index < mNavStack.size()) ? mNavStack[index] : Ui::View();
}

Ui::View NavigatorImpl::GetModalStackItem(uint32_t index) const
{
  return (index < mModalStack.size()) ? mModalStack[index] : Ui::View();
}

// =============================================================================
// Back navigation
// =============================================================================

bool NavigatorImpl::NavigateBack()
{
  if(mInvokingTransitionCallback)
  {
    return false;
  }

  // Keep the implementation alive if a Back callback resets the caller's last
  // Navigator handle before automatic navigation continues.
  Ui::Navigator navigator = GetHandle();
  if(!navigator)
  {
    return false;
  }

  if(!mModalStack.empty())
  {
    auto top     = ModalTop();
    auto session = NavigatorModalBridge::Find(*this, top);
    if(EmitBackRequested(navigator, top))
    {
      return true;
    }
    if(ModalTop() != top || NavigatorModalBridge::Find(*this, top) != session) return true;
    if(session) return session->RequestDismiss(DialogDismissReason::BACK);
    PopModal(true);
    return true;
  }
  if(mNavStack.size() > 1)
  {
    auto top = NavTop();
    if(EmitBackRequested(navigator, top))
    {
      return true;
    }
    if(NavTop() != top || ModalTop()) return true;
    Pop(true);
    return true;
  }
  return false;
}

void NavigatorImpl::SetPageTransitionAnimationEnabled(bool enabled)
{
  mPageTransitionAnimationEnabled = enabled;
}

bool NavigatorImpl::IsPageTransitionAnimationEnabled() const
{
  return mPageTransitionAnimationEnabled;
}

void NavigatorImpl::SetModalTransitionAnimationEnabled(bool enabled)
{
  mModalTransitionAnimationEnabled = enabled;
}

bool NavigatorImpl::IsModalTransitionAnimationEnabled() const
{
  return mModalTransitionAnimationEnabled;
}

void NavigatorImpl::SetTransitionSpec(NavigationTransitionSpec spec)
{
  mDefaultSpec = std::move(spec);
}

void NavigatorImpl::ClearTransitionSpec()
{
  mDefaultSpec.Reset();
}

void NavigatorImpl::SetPageTransitionSpec(Ui::View page, NavigationTransitionSpec spec)
{
  SetPageSpec(mPageSpecs, page, std::move(spec));
}

void NavigatorImpl::ClearPageTransitionSpec(Ui::View page)
{
  ClearPageSpec(mPageSpecs, page);
}

void NavigatorImpl::SetModalTransitionSpec(NavigationTransitionSpec spec)
{
  mDefaultModalSpec = std::move(spec);
}

void NavigatorImpl::ClearModalTransitionSpec()
{
  mDefaultModalSpec.Reset();
}

void NavigatorImpl::SetPageModalTransitionSpec(Ui::View page, NavigationTransitionSpec spec)
{
  SetPageSpec(mModalPageSpecs, page, std::move(spec));
}

void NavigatorImpl::ClearPageModalTransitionSpec(Ui::View page)
{
  ClearPageSpec(mModalPageSpecs, page);
}

// =============================================================================
// Helpers
// =============================================================================

Ui::Navigator NavigatorImpl::GetHandle()
{
  return Ui::Navigator::DownCast(Self());
}

Ui::View NavigatorImpl::NavTop() const
{
  return mNavStack.empty() ? Ui::View() : mNavStack.back();
}

Ui::View NavigatorImpl::ModalTop() const
{
  return mModalStack.empty() ? Ui::View() : mModalStack.back();
}

void NavigatorImpl::AddChildFill(Ui::View view)
{
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(MATCH_PARENT);
  Self().Add(view);
}

void NavigatorImpl::UpdateVisibility()
{
  auto       pages    = mNavStack;
  auto       modals   = mModalStack;
  const auto revision = mModalData->revision;
  for(auto& page : pages)
  {
    if(revision != mModalData->revision) return;
    page.SetProperty(Dali::Actor::Property::VISIBLE, page == NavTop());
  }
  for(auto& modal : modals)
  {
    if(revision != mModalData->revision) return;
    modal.SetProperty(Dali::Actor::Property::VISIBLE, modal == ModalTop());
  }
}

void NavigatorImpl::RestackModals()
{
  // Keep modal content above the navigation stack (later children are higher).
  auto modals = mModalStack;
  for(auto& modal : modals)
  {
    if(InStack(mModalStack, modal)) modal.RaiseToTop();
  }
}

NavigatorImpl::TransitionView NavigatorImpl::CaptureTransitionView(Ui::View view) const
{
  TransitionView state;
  auto           container = mTxByModal ? Ui::DialogContainer::DownCast(view) : Ui::DialogContainer();
  state.modalContent       = static_cast<bool>(container);
  state.view               = container ? container.GetModalContent() : view;
  if(state.view)
  {
    state.position = state.view.GetProperty<Vector3>(Dali::Actor::Property::POSITION);
    state.scale    = state.view.GetProperty<Vector3>(Dali::Actor::Property::SCALE);
    state.opacity  = state.view.GetProperty<float>(Dali::Actor::Property::OPACITY);
  }
  return state;
}

void NavigatorImpl::RestoreModalContent(const TransitionView& state)
{
  if(state.modalContent && state.view)
  {
    auto view = state.view;
    view.SetProperty(Dali::Actor::Property::POSITION, state.position);
    view.SetProperty(Dali::Actor::Property::SCALE, state.scale);
    view.SetProperty(Dali::Actor::Property::OPACITY, state.opacity);
  }
}

void NavigatorImpl::RestoreScrim()
{
  if(mTxScrim)
  {
    mTxScrim.SetProperty(Dali::Actor::Property::OPACITY, mTxScrimOpacity);
    mTxScrim.Reset();
  }
}

void NavigatorImpl::RunTransition(bool animated, bool fadeIncoming)
{
  AbortTransition();
  mTxIncomingVisual = CaptureTransitionView(mTxIncoming);
  mTxOutgoingVisual = CaptureTransitionView(mTxOutgoing);
  mTxIncomingSpec.Reset();
  mTxOutgoingSpec.Reset();
  mTxIncomingSnapSpec.Reset();
  mTxOutgoingSnapSpec.Reset();
  mTxIncomingSnapSpec =
    mTxIncoming ? LookupSnapSpec(mTxIncoming, true, mTxByModal) : NavigationTransitionSpec();
  mTxOutgoingSnapSpec =
    mTxOutgoing ? LookupSnapSpec(mTxOutgoing, false, mTxByModal) : NavigationTransitionSpec();

  if(animated && (mTxIncoming || mTxOutgoing))
  {
    auto container = mTxByModal ? Ui::DialogContainer::DownCast(fadeIncoming ? mTxIncoming : mTxOutgoing)
                                : Ui::DialogContainer();
    mTxScrim       = container ? container.GetScrim() : Ui::View();
    if(mTxScrim)
    {
      mTxScrimOpacity = mTxScrim.GetProperty<float>(Dali::Actor::Property::OPACITY);
    }
    // Keep both selected specifications alive across callback emission. A callback
    // may clear or replace Navigator's stored specifications re-entrantly.
    mTxIncomingSpec =
      mTxIncoming ? LookupAnimatorSpec(mTxIncoming, mTxByPop, true, mTxByModal) : NavigationTransitionSpec();
    mTxOutgoingSpec =
      mTxOutgoing ? LookupAnimatorSpec(mTxOutgoing, mTxByPop, false, mTxByModal) : NavigationTransitionSpec();
    const bool custom = mTxIncomingSpec || mTxOutgoingSpec;
    Ui::View   target = fadeIncoming ? mTxIncomingVisual.view : mTxOutgoingVisual.view;
    if(custom || target || mTxScrim)
    {
      if(custom && mTxIncoming)
      {
        mTxIncoming.SetProperty(Dali::Actor::Property::VISIBLE, true);
      }
      if(custom && mTxOutgoing)
      {
        mTxOutgoing.SetProperty(Dali::Actor::Property::VISIBLE, true);
      }

      const float duration = (custom || container) ? ResolveTransitionDuration(mTxIncoming, mTxOutgoing)
                                                   : TRANSITION_DURATION;
      mTransition          = Dali::Animation::New(duration);
      if(mTxScrim)
      {
        // Only the newly pushed or removed scrim fades. Covered/revealed scrims
        // keep their resting opacity, and the full container remains stationary.
        mTxScrim.SetProperty(Dali::Actor::Property::OPACITY, fadeIncoming ? 0.0f : mTxScrimOpacity);
        mTransition.AnimateTo(Dali::Property(mTxScrim, Dali::Actor::Property::OPACITY),
                              fadeIncoming ? mTxScrimOpacity : 0.0f,
                              Dali::AlphaFunction::LINEAR, Dali::TimePeriod(duration));
      }
      if(custom)
      {
        ScopedTransitionCallback callbackScope(mInvokingTransitionCallback);
        if(mTxIncomingSpec && mTxIncomingVisual.view)
        {
          SelectAnimatorSignal(mTxIncomingSpec, mTxByPop, true).Emit(mTransition, mTxIncomingVisual.view);
        }
        if(mTxOutgoingSpec && mTxOutgoingVisual.view)
        {
          SelectAnimatorSignal(mTxOutgoingSpec, mTxByPop, false).Emit(mTransition, mTxOutgoingVisual.view);
        }
      }
      else if(target)
      {
        const float opacity = 1.0f;
        target.SetProperty(Dali::Actor::Property::OPACITY, fadeIncoming ? 0.0f : opacity);
        mTransition.AnimateTo(Dali::Property(target, Dali::Actor::Property::OPACITY), fadeIncoming ? opacity : 0.0f);
      }
      mTransition.FinishedSignal().Connect(this, &NavigatorImpl::OnTransitionFinished);
      mTransition.Play();
      return;
    }
  }
  FinishTransition();
}

void NavigatorImpl::OnTransitionFinished(Dali::Animation animation)
{
  if(animation == mTransition) FinishTransition();
}

void NavigatorImpl::FinishTransition()
{
  Ui::Navigator handle          = GetHandle();
  const auto    transitionId    = mModalData->transitionId;
  Ui::View      incoming        = mTxIncoming;
  Ui::View      outgoing        = mTxOutgoing;
  const bool    byPop           = mTxByPop;
  const bool    removeOutgoing  = mTxRemoveOutgoing;
  auto          incomingSession = NavigatorModalBridge::Find(*this, incoming);
  auto          outgoingSession = NavigatorModalBridge::Find(*this, outgoing);

  if(incoming)
  {
    SnapView(incoming, true);
  }
  if(transitionId != mModalData->transitionId) return;
  if(outgoing && !removeOutgoing)
  {
    SnapView(outgoing, false);
  }
  if(transitionId != mModalData->transitionId) return;
  RestoreScrim();
  if(removeOutgoing || (outgoingSession && outgoingSession->state == DialogPresentationSession::State::CLOSING))
  {
    // A covered managed Dialog can also be closing while its Exit settles.
    RestoreModalContent(mTxOutgoingVisual);
  }
  if(removeOutgoing && outgoing)
  {
    DetachChild(outgoing);
    RemovePageSpec(outgoing);
  }
  if(transitionId != mModalData->transitionId) return;
  mTxIncoming.Reset();
  mTxOutgoing.Reset();
  mTxIncomingVisual = {};
  mTxOutgoingVisual = {};
  mTxByPop          = false;
  mTxRemoveOutgoing = false;
  mTxByModal        = false;
  mTxIncomingSpec.Reset();
  mTxOutgoingSpec.Reset();
  mTxIncomingSnapSpec.Reset();
  mTxOutgoingSnapSpec.Reset();
  // Drop the finished animation handle so a subsequent SettlePendingTransition()
  // does not treat this (already completed) transition as still pending and emit
  // TransitionFinishedSignal a second time.
  mTransition.Reset();
  ++mModalData->transitionId;
  const auto completedId = mModalData->transitionId;
  UpdateVisibility();

  if(handle)
  {
    if(outgoing)
    {
      mPageDidDisappearSignal.Emit(handle, outgoing, byPop);
    }
    if(incoming && completedId == mModalData->transitionId)
    {
      mPageDidAppearSignal.Emit(handle, incoming, byPop);
    }
    if(completedId == mModalData->transitionId) mTransitionFinishedSignal.Emit(handle);
  }
  if(removeOutgoing && outgoingSession) outgoingSession->Complete(DialogDismissReason::PROGRAMMATIC);
  if(incomingSession)
  {
    incomingSession->enterCompleted = true;
    incomingSession->TryNotifyShown();
  }
}

void NavigatorImpl::AbortTransition()
{
  if(mTransition)
  {
    mTransition.Stop();
    mTransition.Clear();
    mTransition.Reset();
  }
}

void NavigatorImpl::SettlePendingTransition()
{
  if(mTransition || mTxIncoming || mTxOutgoing)
  {
    AbortTransition();
    FinishTransition();
  }
}

void NavigatorImpl::OnScrimClicked(Ui::DialogContainer container)
{
  if(ModalTop() != container) return;
  if(auto session = NavigatorModalBridge::Find(*this, container))
  {
    session->RequestDismiss(DialogDismissReason::SCRIM);
    return;
  }
  PopModal(true);
}

void NavigatorImpl::DetachChild(Ui::View view)
{
  if(view && view.GetParent() == Self())
  {
    auto session = NavigatorModalBridge::Find(*this, view);
    mModalData->removing.push_back({view.GetObjectPtr(), session.get()});
    struct ResetRemovalPermission
    {
      std::vector<Internal::NavigatorModalData::Removal>& removals;
      ~ResetRemovalPermission()
      {
        removals.pop_back();
      }
    } resetRemovalPermission{mModalData->removing};
    Self().Remove(view);
  }
}

void NavigatorImpl::OnChildRemove(Actor& child)
{
  if(!mModalData->destroying)
  {
    auto view     = Ui::View::DownCast(child);
    auto session  = NavigatorModalBridge::Find(*this, view);
    auto expected = std::find_if(mModalData->removing.rbegin(), mModalData->removing.rend(), [&](const auto& removal)
    {
      return removal.target == child.GetObjectPtr() && removal.session == session.get();
    });
    DALI_ASSERT_ALWAYS((!session || expected != mModalData->removing.rend()) &&
                       "Managed DialogContainer must be removed through Navigator or Dialog::Dismiss");
    if(expected != mModalData->removing.rend()) expected->target = nullptr;
  }
  ViewImpl::OnChildRemove(child);
}

void NavigatorImpl::OnDestroy()
{
  mModalData->destroying = true;
  AbortTransition();
  RestoreScrim();
  RestoreModalContent(mTxIncomingVisual);
  RestoreModalContent(mTxOutgoingVisual);
  mTxIncomingVisual = {};
  mTxOutgoingVisual = {};
  auto sessions     = std::move(mModalData->sessions);
  for(auto& session : sessions) session->Complete(DialogDismissReason::HOST_REMOVED, true);
  ViewImpl::OnDestroy();
}

bool NavigatorImpl::EmitBackRequested(Ui::Navigator navigator, Ui::View page)
{
  if(!navigator || !page || mBackRequestedSignal.Empty())
  {
    return false;
  }
  return mBackRequestedSignal.EmitOr(navigator, page);
}

NavigationTransitionSpec NavigatorImpl::LookupAnimatorSpec(Ui::View view, bool byPop, bool isIncoming, bool byModal)
{
  if(!view)
  {
    return NavigationTransitionSpec();
  }

  auto& pageSpecs   = byModal ? mModalPageSpecs : mPageSpecs;
  auto& defaultSpec = byModal ? mDefaultModalSpec : mDefaultSpec;

  for(auto& entry : pageSpecs)
  {
    if(entry.first == view && entry.second)
    {
      if(!SelectAnimatorSignal(entry.second, byPop, isIncoming).Empty())
      {
        return entry.second;
      }
      break;
    }
  }

  if(defaultSpec)
  {
    if(!SelectAnimatorSignal(defaultSpec, byPop, isIncoming).Empty())
    {
      return defaultSpec;
    }
  }

  return NavigationTransitionSpec();
}

NavigationTransitionSpec NavigatorImpl::LookupSnapSpec(Ui::View view, bool isIncoming, bool byModal)
{
  if(!view)
  {
    return NavigationTransitionSpec();
  }

  auto& pageSpecs   = byModal ? mModalPageSpecs : mPageSpecs;
  auto& defaultSpec = byModal ? mDefaultModalSpec : mDefaultSpec;

  for(auto& entry : pageSpecs)
  {
    if(entry.first == view && entry.second)
    {
      if(!SelectSnapSignal(entry.second, isIncoming).Empty())
      {
        return entry.second;
      }
      break;
    }
  }

  if(defaultSpec)
  {
    if(!SelectSnapSignal(defaultSpec, isIncoming).Empty())
    {
      return defaultSpec;
    }
  }

  return NavigationTransitionSpec();
}

float NavigatorImpl::ResolveTransitionDuration(Ui::View incoming, Ui::View outgoing) const
{
  auto lookupDuration = [this](Ui::View view)
  {
    const auto& pageSpecs   = mTxByModal ? mModalPageSpecs : mPageSpecs;
    const auto& defaultSpec = mTxByModal ? mDefaultModalSpec : mDefaultSpec;

    if(view)
    {
      for(const auto& entry : pageSpecs)
      {
        if(entry.first == view && entry.second && entry.second.GetDuration() > 0.0f)
        {
          return entry.second.GetDuration();
        }
      }
    }
    return (defaultSpec && defaultSpec.GetDuration() > 0.0f) ? defaultSpec.GetDuration() : 0.0f;
  };

  const float incomingDuration = lookupDuration(incoming);
  const float outgoingDuration = lookupDuration(outgoing);
  const float resolvedDuration = std::max(incomingDuration, outgoingDuration);
  return resolvedDuration > 0.0f ? resolvedDuration : TRANSITION_DURATION;
}

void NavigatorImpl::SnapView(Ui::View view, bool isIncoming)
{
  const auto& captured = isIncoming ? mTxIncomingVisual : mTxOutgoingVisual;
  // PageWill callbacks can settle a transition before RunTransition captures
  // its visual targets. An empty modal content is still a valid snapshot.
  const auto state = (captured.view || captured.modalContent) ? captured : CaptureTransitionView(view);
  view             = state.view;
  if(!view)
  {
    return;
  }

  // Use only the specification snapshot selected when the transition started.
  // Replacing Navigator's configured spec must not affect an active transition.
  NavigationTransitionSpec snapSpec = isIncoming ? mTxIncomingSnapSpec : mTxOutgoingSnapSpec;
  if(snapSpec && !SelectSnapSignal(snapSpec, isIncoming).Empty())
  {
    ScopedTransitionCallback callbackScope(mInvokingTransitionCallback);
    SelectSnapSignal(snapSpec, isIncoming).Emit(view);
    return;
  }

  if(isIncoming)
  {
    view.SetProperty(Dali::Actor::Property::OPACITY, 1.0f);
  }
}

void NavigatorImpl::SetPageSpec(std::vector<std::pair<Ui::View, NavigationTransitionSpec>>& specs, Ui::View page, NavigationTransitionSpec spec)
{
  if(!page)
  {
    return;
  }

  if(!spec)
  {
    ClearPageSpec(specs, page);
    return;
  }

  for(auto& entry : specs)
  {
    if(entry.first == page)
    {
      entry.second = std::move(spec);
      return;
    }
  }

  specs.emplace_back(page, std::move(spec));
}

void NavigatorImpl::ClearPageSpec(std::vector<std::pair<Ui::View, NavigationTransitionSpec>>& specs, Ui::View page)
{
  specs.erase(
    std::remove_if(specs.begin(),
                   specs.end(),
                   [&page](const std::pair<Ui::View, NavigationTransitionSpec>& entry)
  {
    return entry.first == page;
  }),
    specs.end());
}

void NavigatorImpl::RemovePageSpec(Ui::View page)
{
  ClearPageSpec(mPageSpecs, page);
  ClearPageSpec(mModalPageSpecs, page);
}

bool NavigatorImpl::InStack(const std::vector<Ui::View>& stack, Ui::View view)
{
  return std::find(stack.begin(), stack.end(), view) != stack.end();
}

bool NavigatorImpl::InAnyStack(Ui::View view) const
{
  return InStack(mNavStack, view) || InStack(mModalStack, view);
}

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
