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

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/absolute-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/object/property.h>
#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Integration
{
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
: ViewImpl()
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
  if(mInvokingTransitionCallback || !page || InAnyStack(page))
  {
    return;
  }
  SettlePendingTransition();

  Ui::Navigator handle = GetHandle();
  Ui::View      prev   = NavTop();

  mNavStack.push_back(page);
  AddChildFill(page);
  RestackModals();
  UpdateVisibility();

  if(prev && handle)
  {
    mPageWillDisappearSignal.Emit(handle, prev, false);
  }
  if(handle)
  {
    mPageWillAppearSignal.Emit(handle, page, false);
  }

  mTxIncoming       = page;
  mTxOutgoing       = prev;
  mTxByPop          = false;
  mTxRemoveOutgoing = false;
  mTxByModal        = false;
  RunTransition(animated && mPageTransitionAnimationEnabled, true);
}

Ui::View NavigatorImpl::Pop(bool animated)
{
  // The last page may be popped, leaving the stack empty; only an already-empty
  // stack returns an empty handle.
  if(mInvokingTransitionCallback || mNavStack.empty())
  {
    return Ui::View();
  }
  SettlePendingTransition();

  Ui::Navigator handle = GetHandle();
  Ui::View      top    = mNavStack.back();
  mNavStack.pop_back();
  Ui::View newTop = NavTop();

  UpdateVisibility();
  RestackModals();

  if(handle)
  {
    mPageWillDisappearSignal.Emit(handle, top, true);
  }
  if(newTop && handle)
  {
    mPageWillAppearSignal.Emit(handle, newTop, true);
  }

  mTxIncoming       = newTop;
  mTxOutgoing       = top;
  mTxByPop          = true;
  mTxRemoveOutgoing = true;
  mTxByModal        = false;
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
    if(page.GetParent() == Self())
    {
      Self().Remove(page);
    }
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
    mModalStack.erase(mit);
    if(page.GetParent() == Self())
    {
      Self().Remove(page);
    }
    RemovePageSpec(page);
    UpdateVisibility();
  }
}

void NavigatorImpl::Clear()
{
  if(mInvokingTransitionCallback)
  {
    return;
  }

  SettlePendingTransition();
  for(auto& v : mModalStack)
  {
    if(v.GetParent() == Self())
    {
      Self().Remove(v);
    }
  }
  for(auto& v : mNavStack)
  {
    if(v.GetParent() == Self())
    {
      Self().Remove(v);
    }
  }
  mModalStack.clear();
  mNavStack.clear();
  mPageSpecs.clear();
  mModalPageSpecs.clear();
}

// =============================================================================
// Modal stack
// =============================================================================

void NavigatorImpl::PushModal(Ui::View modal, bool animated)
{
  if(mInvokingTransitionCallback || !modal || InAnyStack(modal))
  {
    return;
  }
  SettlePendingTransition();

  Ui::Navigator handle    = GetHandle();
  Ui::View      prevModal = ModalTop();

  mModalStack.push_back(modal);
  AddChildFill(modal);
  RestackModals();

  // Wire tap-to-dismiss when the modal is a DialogContainer.
  Ui::DialogContainer dialogContainer = Ui::DialogContainer::DownCast(modal);
  if(dialogContainer)
  {
    dialogContainer.ScrimClickedSignal().Connect(this, &NavigatorImpl::OnScrimClicked);
  }

  UpdateVisibility();

  // The view being covered is the previous modal, or (for the first modal) the
  // current navigation-stack top.
  Ui::View disappearing = prevModal ? prevModal : NavTop();
  if(disappearing && handle)
  {
    mPageWillDisappearSignal.Emit(handle, disappearing, false);
  }
  if(handle)
  {
    mPageWillAppearSignal.Emit(handle, modal, false);
  }

  mTxIncoming       = modal;
  mTxOutgoing       = disappearing;
  mTxByPop          = false;
  mTxRemoveOutgoing = false;
  mTxByModal        = true;
  RunTransition(animated && mModalTransitionAnimationEnabled, true);
}

Ui::View NavigatorImpl::PopModal(bool animated)
{
  if(mInvokingTransitionCallback || mModalStack.empty())
  {
    return Ui::View();
  }
  SettlePendingTransition();

  Ui::Navigator handle = GetHandle();
  Ui::View      top    = mModalStack.back();
  mModalStack.pop_back();
  Ui::View newModalTop = ModalTop();

  UpdateVisibility();
  RestackModals();

  // The view revealed is the new modal top, or (when the modal stack is now
  // empty) the navigation-stack top.
  Ui::View appearing = newModalTop ? newModalTop : NavTop();

  if(handle)
  {
    mPageWillDisappearSignal.Emit(handle, top, true);
  }
  if(appearing && handle)
  {
    mPageWillAppearSignal.Emit(handle, appearing, true);
  }

  mTxIncoming       = appearing;
  mTxOutgoing       = top;
  mTxByPop          = true;
  mTxRemoveOutgoing = true;
  mTxByModal        = true;
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
    if(EmitBackRequested(navigator, mModalStack.back()))
    {
      return true;
    }
    PopModal(true);
    return true;
  }
  if(mNavStack.size() > 1)
  {
    if(EmitBackRequested(navigator, mNavStack.back()))
    {
      return true;
    }
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
  for(std::size_t i = 0; i < mNavStack.size(); ++i)
  {
    mNavStack[i].SetProperty(Dali::Actor::Property::VISIBLE, (i + 1 == mNavStack.size()));
  }
  for(std::size_t i = 0; i < mModalStack.size(); ++i)
  {
    mModalStack[i].SetProperty(Dali::Actor::Property::VISIBLE, (i + 1 == mModalStack.size()));
  }
}

void NavigatorImpl::RestackModals()
{
  // Keep modal content above the navigation stack (later children are higher).
  for(auto& modal : mModalStack)
  {
    modal.RaiseToTop();
  }
}

void NavigatorImpl::RunTransition(bool animated, bool fadeIncoming)
{
  AbortTransition();
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
    // Keep both selected specifications alive across callback emission. A callback
    // may clear or replace Navigator's stored specifications re-entrantly.
    mTxIncomingSpec =
      mTxIncoming ? LookupAnimatorSpec(mTxIncoming, mTxByPop, true, mTxByModal) : NavigationTransitionSpec();
    mTxOutgoingSpec =
      mTxOutgoing ? LookupAnimatorSpec(mTxOutgoing, mTxByPop, false, mTxByModal) : NavigationTransitionSpec();
    if(mTxIncomingSpec || mTxOutgoingSpec)
    {
      if(mTxIncoming)
      {
        mTxIncoming.SetProperty(Dali::Actor::Property::VISIBLE, true);
      }
      if(mTxOutgoing)
      {
        mTxOutgoing.SetProperty(Dali::Actor::Property::VISIBLE, true);
      }

      mTransition = Dali::Animation::New(ResolveTransitionDuration(mTxIncoming, mTxOutgoing));
      {
        ScopedTransitionCallback callbackScope(mInvokingTransitionCallback);
        if(mTxIncomingSpec && mTxIncoming)
        {
          SelectAnimatorSignal(mTxIncomingSpec, mTxByPop, true).Emit(mTransition, mTxIncoming);
        }
        if(mTxOutgoingSpec && mTxOutgoing)
        {
          SelectAnimatorSignal(mTxOutgoingSpec, mTxByPop, false).Emit(mTransition, mTxOutgoing);
        }
      }
      mTransition.FinishedSignal().Connect(this, &NavigatorImpl::OnTransitionFinished);
      mTransition.Play();
      return;
    }

    Ui::View target = fadeIncoming ? mTxIncoming : mTxOutgoing;
    if(target)
    {
      const float from = fadeIncoming ? 0.0f : 1.0f;
      const float to   = fadeIncoming ? 1.0f : 0.0f;
      target.SetProperty(Dali::Actor::Property::OPACITY, from);

      mTransition = Dali::Animation::New(TRANSITION_DURATION);
      mTransition.AnimateTo(Dali::Property(target, Dali::Actor::Property::OPACITY), to);
      mTransition.FinishedSignal().Connect(this, &NavigatorImpl::OnTransitionFinished);
      mTransition.Play();
      return;
    }
  }
  FinishTransition();
}

void NavigatorImpl::OnTransitionFinished(Dali::Animation /*animation*/)
{
  FinishTransition();
}

void NavigatorImpl::FinishTransition()
{
  Ui::Navigator handle = GetHandle();

  if(mTxIncoming)
  {
    SnapView(mTxIncoming, true);
  }
  if(mTxOutgoing && !mTxRemoveOutgoing)
  {
    SnapView(mTxOutgoing, false);
  }
  if(mTxRemoveOutgoing && mTxOutgoing)
  {
    if(mTxOutgoing.GetParent() == Self())
    {
      Self().Remove(mTxOutgoing);
    }
    RemovePageSpec(mTxOutgoing);
  }
  UpdateVisibility();

  Ui::View   incoming = mTxIncoming;
  Ui::View   outgoing = mTxOutgoing;
  const bool byPop    = mTxByPop;
  mTxIncoming.Reset();
  mTxOutgoing.Reset();
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

  if(handle)
  {
    if(outgoing)
    {
      mPageDidDisappearSignal.Emit(handle, outgoing, byPop);
    }
    if(incoming)
    {
      mPageDidAppearSignal.Emit(handle, incoming, byPop);
    }
    mTransitionFinishedSignal.Emit(handle);
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

void NavigatorImpl::OnScrimClicked(Ui::DialogContainer /*container*/)
{
  PopModal(true);
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
} // namespace Dali
