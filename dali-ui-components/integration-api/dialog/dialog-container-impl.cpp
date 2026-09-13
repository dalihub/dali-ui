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
#include <dali-ui-components/integration-api/dialog/dialog-container-impl.h>

// EXTERNAL INCLUDES
#include <unordered_map>

#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/render-effects/background-blur-effect.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/common/unique-ptr.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
namespace
{
// Register the type with ViewImpl as the base so instances inherit View's
// (animatable) properties such as viewEffectiveScale, which ViewImpl::Measure
// reads for every view. Without this, measuring a DialogContainer throws.
BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(DialogContainerImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

// UI-thread-only, non-owning registry. Actor parent pointers are already clear
// during disconnection callbacks, while modal ownership may still be live.
// Entries exist only for acquired state and are erased by Release/OnDestroy.
std::unordered_map<const RefObject*, DialogContainerImpl*> gModalOwners;

bool IsAccessibilityShowing(Ui::View view)
{
  auto* accessible = Dali::Accessibility::Accessible::Get(view);
  return accessible && accessible->GetStates()[Dali::Integration::Accessibility::State::SHOWING];
}
} // anonymous namespace

Ui::DialogContainer DialogContainerImpl::New()
{
  return New(Ui::DialogContainerStyle::Default());
}

Ui::DialogContainer DialogContainerImpl::New(Ui::DialogContainerStyle style)
{
  DALI_ASSERT_ALWAYS(style && "DialogContainer style must be initialized");
  IntrusivePtr<DialogContainerImpl> impl = new DialogContainerImpl();

  Ui::DialogContainer handle = Ui::DialogContainer(*impl);

  impl->mStyle = style;
  impl->Initialize();

  return handle;
}

DialogContainerImpl::DialogContainerImpl()
: ViewImpl()
{
}

DialogContainerImpl::~DialogContainerImpl()
{
}

void DialogContainerImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  // Derived implementations may initialize a View handle without going
  // through our static factory. Keep that construction path usable.
  if(!mStyle)
  {
    mStyle = Ui::DialogContainerStyle::Default();
  }

  // Absolute layout so the scrim (MATCH_PARENT) fills the container and the
  // modal content can be positioned/sized freely above it.
  AttachLayoutManager(Dali::MakeUnique<AbsoluteLayoutManager>());

  CreateDefaultScrim();
}

void DialogContainerImpl::CreateDefaultScrim()
{
  InteractiveView scrim = InteractiveView::New();
  scrim.SetFocusable(false);
  scrim.SetFocusOnTouchEnabled(false);
  scrim.SetBackgroundColor(mStyle.GetScrimColor());
  if(mStyle.GetScrimBlurRadius() > 0.0f)
  {
    scrim.SetRenderEffect(BackgroundBlurEffect::New(static_cast<uint32_t>(mStyle.GetScrimBlurRadius())));
  }
  scrim.SetRequestedWidth(MATCH_PARENT);
  scrim.SetRequestedHeight(MATCH_PARENT);
  scrim.ConnectClickedSignal(this, &DialogContainerImpl::OnScrimClicked);

  mScrim = scrim;
  Self().Add(mScrim);
}

void DialogContainerImpl::SetModalContent(Ui::View modalContent)
{
  if(mModalContent == modalContent)
  {
    return;
  }
  // Scene/visibility callbacks can synchronously clear or replace the content.
  // Keep this actor alive and let the newest nested setter supersede this one.
  Actor          self       = Self();
  const uint64_t generation = ++mModalChangeGeneration;
  Ui::View       oldContent = mModalContent;
  if(oldContent && oldContent.GetParent() == self)
  {
    // Keep the existing immediate Actor::Remove behavior. Its scene callback
    // reports disappearance while the content still has our modal flag.
    self.Remove(oldContent);
  }
  if(generation != mModalChangeGeneration)
  {
    return;
  }
  ReleaseModalContent();
  if(generation != mModalChangeGeneration)
  {
    return;
  }

  if(modalContent)
  {
    // Release the old owner's snapshot before capturing ours. Otherwise a
    // transfer would preserve the temporary true flag rather than the user's.
    auto previous = gModalOwners.find(modalContent.GetObjectPtr());
    if(previous != gModalOwners.end() && previous->second != this)
    {
      previous->second->SetModalContent({});
      if(generation != mModalChangeGeneration)
      {
        return;
      }
    }
  }

  mModalContent = modalContent;
  if(mModalContent)
  {
    // This is only a tentative association until Add succeeds. An old parent's
    // callback may keep the view or install it into another DialogContainer.
    // Do not make that owner inherit our temporary modal flag.
    self.Add(modalContent);
    if(generation != mModalChangeGeneration || mModalContent != modalContent)
    {
      return;
    }
    if(modalContent.GetParent() != self)
    {
      ReleaseModalContent();
      return;
    }
    mOriginalModal                            = modalContent.IsAccessibilityModal();
    mModalShowing                             = false;
    mModalStateAcquired                       = true;
    gModalOwners[modalContent.GetObjectPtr()] = this;
    modalContent.SetAccessibilityModal(true);
    if(generation != mModalChangeGeneration)
    {
      return;
    }
    ObserveModalContent();
    modalContent.RaiseToTop(); // keep content above the scrim
    if(generation == mModalChangeGeneration && mModalContent == modalContent)
    {
      UpdateModalShowing();
    }
  }
}

void DialogContainerImpl::ObserveModalContent()
{
  DevelActor::OnSceneVisibilityChangedSignal(mModalContent).Connect(this, &DialogContainerImpl::OnModalVisibilityChanged);

  // Scene visibility excludes transparency and culling. Observe these rendered
  // properties too, then query Foundation's actual accessible SHOWING state.
  mModalAlphaNotification = mModalContent.AddPropertyNotification(Actor::Property::WORLD_COLOR_MULTIPLIER, 3, OutsideCondition(0.0f, 0.0f));
  mModalAlphaNotification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
  mModalAlphaNotification.NotifySignal().Connect(this, &DialogContainerImpl::OnModalPropertyChanged);
  mModalCulledNotification = mModalContent.AddPropertyNotification(DevelActor::Property::CULLED, GreaterThanCondition(0.5f));
  mModalCulledNotification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
  mModalCulledNotification.NotifySignal().Connect(this, &DialogContainerImpl::OnModalPropertyChanged);
}

void DialogContainerImpl::UpdateModalShowing()
{
  // During application shutdown singleton services can disappear before scene
  // actors. GetStates also queries keyboard focus; do not call it (or the
  // bridge's state-dependent emitter) after its FocusManager is gone.
  if(!mModalContent || !mModalStateAcquired || !Ui::FocusManager::Get())
  {
    return;
  }
  const bool showing = IsAccessibilityShowing(mModalContent);
  if(showing != mModalShowing)
  {
    mModalShowing = showing;
    Dali::Ui::Extension::View::NotifyAccessibilityShowingChanged(mModalContent, showing);
  }
}

void DialogContainerImpl::OnModalVisibilityChanged(Actor actor, bool /*visible*/)
{
  if(actor == mModalContent)
  {
    UpdateModalShowing();
  }
}

void DialogContainerImpl::OnModalPropertyChanged(PropertyNotification notification)
{
  if(notification == mModalAlphaNotification || notification == mModalCulledNotification)
  {
    UpdateModalShowing();
  }
}

void DialogContainerImpl::ReleaseModalContent()
{
  if(!mModalContent)
  {
    return;
  }
  Ui::View       content       = mModalContent;
  const bool     originalModal = mOriginalModal;
  const bool     stateAcquired = mModalStateAcquired;
  const uint64_t generation    = mModalChangeGeneration;
  // The bridge filters non-Dialog showing events by the current modal flag;
  // reconcile disappearance before restoring a saved false value. A custom
  // accessible can run application code, so do not release a newer request.
  UpdateModalShowing();
  if(generation != mModalChangeGeneration || mModalContent != content)
  {
    return;
  }
  if(stateAcquired)
  {
    auto owner = gModalOwners.find(content.GetObjectPtr());
    if(owner != gModalOwners.end() && owner->second == this)
    {
      gModalOwners.erase(owner);
    }
    DevelActor::OnSceneVisibilityChangedSignal(content).Disconnect(this, &DialogContainerImpl::OnModalVisibilityChanged);
    content.RemovePropertyNotification(mModalAlphaNotification);
    content.RemovePropertyNotification(mModalCulledNotification);
  }
  mModalAlphaNotification.Reset();
  mModalCulledNotification.Reset();
  mModalContent.Reset();
  mModalShowing       = false;
  mOriginalModal      = false;
  mModalStateAcquired = false;
  if(stateAcquired)
  {
    content.SetAccessibilityModal(originalModal);
  }
}

void DialogContainerImpl::OnChildRemove(Actor& child)
{
  ViewImpl::OnChildRemove(child);
  if(child == mModalContent)
  {
    // Includes external reparent/removal, so this container cannot later
    // restore stale state over a new owner's snapshot.
    ReleaseModalContent();
  }
}

void DialogContainerImpl::OnDestroy()
{
  ReleaseModalContent();
  ViewImpl::OnDestroy();
}

Ui::View DialogContainerImpl::GetModalContent() const
{
  return mModalContent;
}

void DialogContainerImpl::SetScrim(Ui::View scrim)
{
  if(mScrim == scrim)
  {
    return;
  }
  InteractiveView oldInteractive = InteractiveView::DownCast(mScrim);
  if(oldInteractive)
  {
    oldInteractive.ClickedSignal().Disconnect(this, &DialogContainerImpl::OnScrimClicked);
  }
  if(mScrim && mScrim.GetParent() == Self())
  {
    Self().Remove(mScrim);
  }
  mScrim = scrim;
  if(mScrim)
  {
    mScrim.SetRequestedWidth(MATCH_PARENT);
    mScrim.SetRequestedHeight(MATCH_PARENT);
    Self().Add(mScrim);
    mScrim.LowerToBottom(); // keep scrim below the modal content

    // Forward clicks from an interactive custom scrim.
    InteractiveView interactive = InteractiveView::DownCast(mScrim);
    if(interactive)
    {
      interactive.ConnectClickedSignal(this, &DialogContainerImpl::OnScrimClicked);
    }
  }
}

Ui::View DialogContainerImpl::GetScrim() const
{
  return mScrim;
}

void DialogContainerImpl::OnScrimClicked(Ui::View view, Ui::InputEvent /*event*/)
{
  if(view != mScrim || !mScrim || mScrim.GetParent() != Self())
  {
    return;
  }
  Ui::DialogContainer handle = Ui::DialogContainer::DownCast(Self());
  if(handle)
  {
    mScrimClickedSignal.Emit(handle);
  }
}

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
