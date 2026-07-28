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
#include <dali-ui-foundation/internal/drag-drop-detector/drag-and-drop-detector-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <iterator>
#include <utility>

#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/integration-api/adaptor-framework/scene-holder-impl.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
struct TargetHitTestContext
{
  const std::vector<Dali::Ui::View>* targets{nullptr};
  const std::vector<Dali::Ui::View>* excludedTargets{nullptr};
  Dali::Ui::View                     source;
};

thread_local const TargetHitTestContext* gTargetHitTestContext = nullptr;

bool IsActorEligibleForTargetHitTest(Dali::Actor actor)
{
  return actor &&
         actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) &&
         actor.GetCurrentProperty<bool>(Actor::Property::SENSITIVE) &&
         actor.GetCurrentProperty<bool>(Actor::Property::ENABLED) &&
         !actor.GetCurrentProperty<bool>(Actor::Property::IGNORED);
}

bool IsRegisteredTargetHitTestActor(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  if(!gTargetHitTestContext || !IsActorEligibleForTargetHitTest(actor))
  {
    return false;
  }

  if(type == Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE)
  {
    return true;
  }

  const Dali::Ui::View target = Dali::Ui::View::DownCast(actor);
  if(!target || target == gTargetHitTestContext->source)
  {
    return false;
  }

  if(std::find(gTargetHitTestContext->targets->begin(),
               gTargetHitTestContext->targets->end(),
               target) == gTargetHitTestContext->targets->end())
  {
    return false;
  }

  return std::find(gTargetHitTestContext->excludedTargets->begin(),
                   gTargetHitTestContext->excludedTargets->end(),
                   target) == gTargetHitTestContext->excludedTargets->end();
}

class ScopedTargetHitTestContext
{
public:
  explicit ScopedTargetHitTestContext(const TargetHitTestContext& context)
  : mPrevious(gTargetHitTestContext)
  {
    gTargetHitTestContext = &context;
  }

  ~ScopedTargetHitTestContext()
  {
    gTargetHitTestContext = mPrevious;
  }

private:
  const TargetHitTestContext* mPrevious;
};

class ScopedTrueFlag
{
public:
  explicit ScopedTrueFlag(bool& flag)
  : mFlag(flag),
    mPrevious(flag)
  {
    mFlag = true;
  }

  ~ScopedTrueFlag()
  {
    mFlag = mPrevious;
  }

  ScopedTrueFlag(const ScopedTrueFlag&)            = delete;
  ScopedTrueFlag& operator=(const ScopedTrueFlag&) = delete;

private:
  bool& mFlag;
  bool  mPrevious;
};

bool IsActorHierarchyEligible(Dali::Actor actor)
{
  while(actor)
  {
    if(!actor.GetProperty<bool>(Actor::Property::VISIBLE) ||
       !actor.GetProperty<bool>(Actor::Property::SENSITIVE) ||
       !actor.GetProperty<bool>(Actor::Property::ENABLED) ||
       actor.GetProperty<bool>(Actor::Property::IGNORED))
    {
      return false;
    }
    actor = actor.GetParent();
  }
  return true;
}

bool IsValidDragActivationMode(Dali::Ui::DragActivationMode mode)
{
  return mode == Dali::Ui::DragActivationMode::PAN ||
         mode == Dali::Ui::DragActivationMode::LONG_PRESS;
}

bool IsValidDeviceClass(Device::Class::Type deviceClass)
{
  return deviceClass >= Device::Class::NONE &&
         deviceClass <= Device::Class::GAMEPAD;
}

} // unnamed namespace

Dali::Ui::DragAndDropDetector DragAndDropDetector::New()
{
  Dali::Ui::DragAndDropDetector detector = Dali::Ui::DragAndDropDetector(new DragAndDropDetector());

  return detector;
}

void DragAndDropDetector::AssertRegistrationChangeAllowed() const
{
  DALI_ASSERT_ALWAYS(!mEvaluatingDropProposal &&
                     "Registration cannot change during drop proposal evaluation");
}

void DragAndDropDetector::AssertActivationConfigurationChangeAllowed() const
{
  DALI_ASSERT_ALWAYS(!mActivationPending && !mPointDown && !mFinishing &&
                     "Configuration cannot be changed while activation or a drag is in progress");
}

void DragAndDropDetector::AssertDragConfigurationChangeAllowed() const
{
  DALI_ASSERT_ALWAYS(!mPointDown && !mFinishing &&
                     "Configuration cannot be changed during a drag");
}

void DragAndDropDetector::AssertSourceAttached(const Dali::Ui::View& source) const
{
  DALI_ASSERT_ALWAYS(std::find(mSources.begin(), mSources.end(), source) != mSources.end() &&
                     "Source must be attached before this operation");
}

void DragAndDropDetector::AssertTargetAttached(const Dali::Ui::View& target) const
{
  DALI_ASSERT_ALWAYS(std::find(mTargets.begin(), mTargets.end(), target) != mTargets.end() &&
                     "Target must be attached before this operation");
}

void DragAndDropDetector::AttachSource(Dali::Ui::View& view)
{
  AssertRegistrationChangeAllowed();
  if(view)
  {
    if(std::find(mSources.begin(), mSources.end(), view) == mSources.end())
    {
      mSources.push_back(view);
      mPanGestureDetector.Attach(view);
      if(RequiresLongPressDetector())
      {
        mLongPressGestureDetector.Attach(view);
      }
    }
  }
}

void DragAndDropDetector::SetSourcePayload(Dali::Ui::View& source, Dali::Ui::DragPayload payload)
{
  AssertSourceAttached(source);
  auto match = std::find_if(mSourcePayloads.begin(), mSourcePayloads.end(), [&source](const auto& entry)
  {
    return entry.first == source;
  });
  if(match == mSourcePayloads.end())
  {
    mSourcePayloads.emplace_back(source, std::move(payload));
  }
  else
  {
    match->second = std::move(payload);
  }
}

void DragAndDropDetector::SetSourcePayloadProvider(Dali::Ui::View&       source,
                                                   SourcePayloadProvider provider)
{
  AssertActivationConfigurationChangeAllowed();
  AssertSourceAttached(source);
  DALI_ASSERT_ALWAYS(provider && "A source payload provider is required");
  auto match = std::find_if(
    mSourcePayloadProviders.begin(),
    mSourcePayloadProviders.end(),
    [&source](const auto& entry)
  {
    return entry.first == source;
  });
  if(match == mSourcePayloadProviders.end())
  {
    mSourcePayloadProviders.emplace_back(source, std::move(provider));
  }
  else
  {
    match->second = std::move(provider);
  }
}

void DragAndDropDetector::ClearSourcePayloadProvider(Dali::Ui::View& source)
{
  AssertActivationConfigurationChangeAllowed();
  AssertSourceAttached(source);
  auto match = std::find_if(
    mSourcePayloadProviders.begin(),
    mSourcePayloadProviders.end(),
    [&source](const auto& entry)
  {
    return entry.first == source;
  });
  if(match != mSourcePayloadProviders.end())
  {
    mSourcePayloadProviders.erase(match);
  }
}

void DragAndDropDetector::ClearSourcePayload(Dali::Ui::View& source)
{
  AssertSourceAttached(source);
  auto match = std::find_if(mSourcePayloads.begin(), mSourcePayloads.end(), [&source](const auto& entry)
  {
    return entry.first == source;
  });
  if(match != mSourcePayloads.end())
  {
    mSourcePayloads.erase(match);
  }
}

void DragAndDropDetector::AttachTarget(Dali::Ui::View& view)
{
  AssertRegistrationChangeAllowed();
  if(view && std::find(mTargets.begin(), mTargets.end(), view) == mTargets.end())
  {
    mTargets.push_back(view);
    mDropProposalCallbacks.emplace_back();
    view.SceneDisconnectedSignal().Connect(this, &DragAndDropDetector::OnTargetSceneDisconnected);
  }
}

void DragAndDropDetector::SetDropProposalCallback(Dali::Ui::View&      target,
                                                  DropProposalCallback callback)
{
  AssertDragConfigurationChangeAllowed();
  auto match = std::find(mTargets.begin(), mTargets.end(), target);
  AssertTargetAttached(target);
  DALI_ASSERT_ALWAYS(callback && "A drop proposal callback is required");
  const auto index              = static_cast<uint32_t>(std::distance(mTargets.begin(), match));
  mDropProposalCallbacks[index] = std::move(callback);
}

void DragAndDropDetector::ClearDropProposalCallback(Dali::Ui::View& target)
{
  AssertDragConfigurationChangeAllowed();
  auto match = std::find(mTargets.begin(), mTargets.end(), target);
  AssertTargetAttached(target);
  const auto index              = static_cast<uint32_t>(std::distance(mTargets.begin(), match));
  mDropProposalCallbacks[index] = {};
}

void DragAndDropDetector::SetAutoScrollCallback(Dali::Ui::DragAutoScrollConfiguration configuration,
                                                AutoScrollCallback                    callback)
{
  AssertDragConfigurationChangeAllowed();
  DALI_ASSERT_ALWAYS(configuration.GetViewport() && "An auto-scroll viewport is required");
  DALI_ASSERT_ALWAYS(configuration.GetEdgeThreshold().x >= 0.0f &&
                     configuration.GetEdgeThreshold().y >= 0.0f &&
                     "Auto-scroll edge thresholds must be non-negative");
  DALI_ASSERT_ALWAYS(configuration.GetMaxSpeed().x >= 0.0f &&
                     configuration.GetMaxSpeed().y >= 0.0f &&
                     "Auto-scroll maximum speeds must be non-negative");
  DALI_ASSERT_ALWAYS(configuration.GetTickIntervalMilliseconds() > 0u &&
                     "Auto-scroll tick interval must be greater than zero");
  DALI_ASSERT_ALWAYS(callback && "An auto-scroll callback is required");

  if(mAutoScrollConfiguration.GetViewport())
  {
    mAutoScrollConfiguration.GetViewport().SceneDisconnectedSignal().Disconnect(
      this,
      &DragAndDropDetector::OnAutoScrollViewportSceneDisconnected);
  }
  StopAutoScrollTimer();
  mAutoScrollConfiguration = std::move(configuration);
  mAutoScrollCallback      = std::move(callback);
  mAutoScrollConfiguration.GetViewport().SceneDisconnectedSignal().Connect(
    this,
    &DragAndDropDetector::OnAutoScrollViewportSceneDisconnected);
  if(mAutoScrollTimer &&
     mAutoScrollTimer.GetInterval() != mAutoScrollConfiguration.GetTickIntervalMilliseconds())
  {
    mAutoScrollTimer.SetInterval(mAutoScrollConfiguration.GetTickIntervalMilliseconds());
    mAutoScrollTimer.Stop();
  }
}

void DragAndDropDetector::ClearAutoScrollCallback()
{
  AssertDragConfigurationChangeAllowed();
  if(mAutoScrollConfiguration.GetViewport())
  {
    mAutoScrollConfiguration.GetViewport().SceneDisconnectedSignal().Disconnect(
      this,
      &DragAndDropDetector::OnAutoScrollViewportSceneDisconnected);
  }
  StopAutoScrollTimer();
  mAutoScrollCallback      = {};
  mAutoScrollConfiguration = {};
}

void DragAndDropDetector::SetDragActivationMode(Dali::Ui::DragActivationMode mode)
{
  AssertActivationConfigurationChangeAllowed();
  DALI_ASSERT_ALWAYS(IsValidDragActivationMode(mode) && "Invalid drag activation mode");
  if(mode == mDragActivationMode)
  {
    return;
  }

  const bool wasRequired = RequiresLongPressDetector();
  mDragActivationMode    = mode;
  UpdateLongPressDetectorAttachments(wasRequired);
}

Dali::Ui::DragActivationMode DragAndDropDetector::GetDragActivationMode() const
{
  return mDragActivationMode;
}

void DragAndDropDetector::SetDragActivationConfiguration(
  Device::Class::Type                   deviceClass,
  Dali::Ui::DragActivationConfiguration configuration)
{
  AssertActivationConfigurationChangeAllowed();
  DALI_ASSERT_ALWAYS(IsValidDeviceClass(deviceClass) && "Invalid input device class");
  DALI_ASSERT_ALWAYS(IsValidDragActivationMode(configuration.GetMode()) &&
                     "Invalid drag activation mode");
  DALI_ASSERT_ALWAYS(configuration.GetMovementThreshold() >= 0.0f &&
                     "Drag activation movement threshold must be non-negative");

  const bool wasRequired = RequiresLongPressDetector();
  auto       match       = std::find_if(
    mDragActivationConfigurations.begin(),
    mDragActivationConfigurations.end(),
    [deviceClass](const auto& entry)
  {
    return entry.first == deviceClass;
  });
  if(match == mDragActivationConfigurations.end())
  {
    mDragActivationConfigurations.emplace_back(deviceClass, configuration);
  }
  else
  {
    match->second = configuration;
  }
  UpdateLongPressDetectorAttachments(wasRequired);
}

void DragAndDropDetector::ClearDragActivationConfiguration(Device::Class::Type deviceClass)
{
  AssertActivationConfigurationChangeAllowed();
  DALI_ASSERT_ALWAYS(IsValidDeviceClass(deviceClass) && "Invalid input device class");

  const bool wasRequired = RequiresLongPressDetector();
  auto       match       = std::find_if(
    mDragActivationConfigurations.begin(),
    mDragActivationConfigurations.end(),
    [deviceClass](const auto& entry)
  {
    return entry.first == deviceClass;
  });
  if(match != mDragActivationConfigurations.end())
  {
    mDragActivationConfigurations.erase(match);
    UpdateLongPressDetectorAttachments(wasRequired);
  }
}

bool DragAndDropDetector::HasDragActivationConfiguration(Device::Class::Type deviceClass) const
{
  DALI_ASSERT_ALWAYS(IsValidDeviceClass(deviceClass) && "Invalid input device class");
  return std::find_if(
           mDragActivationConfigurations.begin(),
           mDragActivationConfigurations.end(),
           [deviceClass](const auto& entry)
  {
    return entry.first == deviceClass;
  }) != mDragActivationConfigurations.end();
}

Dali::Ui::DragActivationConfiguration DragAndDropDetector::GetDragActivationConfiguration(
  Device::Class::Type deviceClass) const
{
  DALI_ASSERT_ALWAYS(IsValidDeviceClass(deviceClass) && "Invalid input device class");
  auto match = std::find_if(
    mDragActivationConfigurations.begin(),
    mDragActivationConfigurations.end(),
    [deviceClass](const auto& entry)
  {
    return entry.first == deviceClass;
  });
  if(match != mDragActivationConfigurations.end())
  {
    return match->second;
  }
  return {mDragActivationMode, mDragStartThreshold};
}

bool DragAndDropDetector::RequiresLongPressDetector() const
{
  if(mDragActivationMode == Dali::Ui::DragActivationMode::LONG_PRESS)
  {
    return true;
  }
  return std::any_of(
    mDragActivationConfigurations.begin(),
    mDragActivationConfigurations.end(),
    [](const auto& entry)
  {
    return entry.second.GetMode() == Dali::Ui::DragActivationMode::LONG_PRESS;
  });
}

void DragAndDropDetector::UpdateLongPressDetectorAttachments(bool wasRequired)
{
  const bool isRequired = RequiresLongPressDetector();
  if(wasRequired == isRequired)
  {
    return;
  }
  for(auto& source : mSources)
  {
    if(isRequired)
    {
      mLongPressGestureDetector.Attach(source);
    }
    else
    {
      mLongPressGestureDetector.Detach(source);
    }
  }
}

void DragAndDropDetector::SetCanStartDragCallback(CanStartDragCallback callback)
{
  AssertActivationConfigurationChangeAllowed();
  DALI_ASSERT_ALWAYS(callback && "A drag activation callback is required");
  mCanStartDragCallback = std::move(callback);
}

void DragAndDropDetector::ClearCanStartDragCallback()
{
  AssertActivationConfigurationChangeAllowed();
  mCanStartDragCallback = {};
}

void DragAndDropDetector::SetDragStartThreshold(float threshold)
{
  AssertActivationConfigurationChangeAllowed();
  DALI_ASSERT_ALWAYS(threshold >= 0.0f && "Drag start threshold must be non-negative");
  mDragStartThreshold = threshold;
}

float DragAndDropDetector::GetDragStartThreshold() const
{
  return mDragStartThreshold;
}

bool DragAndDropDetector::IsDragActivationPending() const
{
  return mActivationPending;
}

void DragAndDropDetector::DetachSource(Dali::Ui::View& view)
{
  AssertRegistrationChangeAllowed();
  auto match = std::find(mSources.begin(), mSources.end(), view);
  if(match != mSources.end())
  {
    if(mActivationPending && mDragView == view)
    {
      ResetPendingActivation();
    }
    if(mPointDown && mDragView == view)
    {
      FinishDrag(false, Dali::Ui::DragCancelReason::REGISTRATION_REMOVED);
    }
    match = std::find(mSources.begin(), mSources.end(), view);
    if(match != mSources.end())
    {
      mPanGestureDetector.Detach(*match);
      if(RequiresLongPressDetector())
      {
        mLongPressGestureDetector.Detach(*match);
      }
      mSources.erase(match);
      auto payload = std::find_if(mSourcePayloads.begin(), mSourcePayloads.end(), [&view](const auto& entry)
      {
        return entry.first == view;
      });
      if(payload != mSourcePayloads.end())
      {
        mSourcePayloads.erase(payload);
      }
      auto provider = std::find_if(
        mSourcePayloadProviders.begin(),
        mSourcePayloadProviders.end(),
        [&view](const auto& entry)
      {
        return entry.first == view;
      });
      if(provider != mSourcePayloadProviders.end())
      {
        mSourcePayloadProviders.erase(provider);
      }
    }
  }
}
void DragAndDropDetector::DetachTarget(Dali::Ui::View& view)
{
  AssertRegistrationChangeAllowed();
  auto match = std::find(mTargets.begin(), mTargets.end(), view);
  if(match != mTargets.end())
  {
    const bool resolveTarget = mPointDown &&
                               (mCurrentTarget == view || mFeedbackTarget == view);
    const auto targetIndex = std::distance(mTargets.begin(), match);
    match->SceneDisconnectedSignal().Disconnect(this, &DragAndDropDetector::OnTargetSceneDisconnected);
    mTargets.erase(match);
    mDropProposalCallbacks.erase(mDropProposalCallbacks.begin() + targetIndex);
    if(resolveTarget)
    {
      if(mDragSessionOrigin == Dali::Ui::DragSessionOrigin::EXPLICIT)
      {
        ClearExplicitTarget(view);
      }
      else
      {
        ResolveTarget(mScreenPosition, false);
      }
    }
  }
}

void DragAndDropDetector::DetachAll()
{
  AssertRegistrationChangeAllowed();
  ResetPendingActivation();
  FinishDrag(false, Dali::Ui::DragCancelReason::REGISTRATION_REMOVED);
  for(auto& view : mSources)
  {
    mPanGestureDetector.Detach(view);
    if(RequiresLongPressDetector())
    {
      mLongPressGestureDetector.Detach(view);
    }
  }
  for(auto& view : mTargets)
  {
    view.SceneDisconnectedSignal().Disconnect(this, &DragAndDropDetector::OnTargetSceneDisconnected);
  }
  mSources.clear();
  mTargets.clear();
  mSourcePayloads.clear();
  mSourcePayloadProviders.clear();
  mDropProposalCallbacks.clear();
}

bool DragAndDropDetector::StartDrag(Dali::Ui::View& source, Device::Class::Type deviceClass)
{
  return StartDrag(source, FindSourcePayload(source), deviceClass);
}

bool DragAndDropDetector::StartDrag(Dali::Ui::View&       source,
                                    Dali::Ui::DragPayload payload,
                                    Device::Class::Type   deviceClass)
{
  DALI_ASSERT_ALWAYS(IsValidDeviceClass(deviceClass) && "Invalid drag device class");

  if(mActivationPending || mPointDown || mFinishing ||
     std::find(mSources.begin(), mSources.end(), source) == mSources.end() ||
     !source.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) ||
     !IsActorHierarchyEligible(source))
  {
    return false;
  }

  const Vector2 screenPosition = GetViewScreenCenter(source);
  const Vector3 sourceSize     = source.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  Vector2       localPosition(sourceSize.x * 0.5f, sourceSize.y * 0.5f);
  Vector2       convertedPosition;
  if(source.ScreenToLocal(convertedPosition.x,
                          convertedPosition.y,
                          screenPosition.x,
                          screenPosition.y))
  {
    localPosition = convertedPosition;
  }

  mDragView          = source;
  mDragDeviceClass   = deviceClass;
  mDragSessionOrigin = Dali::Ui::DragSessionOrigin::EXPLICIT;
  StartInterruptionMonitoring();
  ActivateDrag(source, localPosition, screenPosition, std::move(payload));
  return mPointDown &&
         mDragSessionOrigin == Dali::Ui::DragSessionOrigin::EXPLICIT;
}

bool DragAndDropDetector::MoveDragTo(Dali::Ui::View& target)
{
  return UpdateExplicitDrag(target);
}

bool DragAndDropDetector::Drop()
{
  if(!mPointDown ||
     mDragSessionOrigin != Dali::Ui::DragSessionOrigin::EXPLICIT)
  {
    return false;
  }

  const bool dropped = static_cast<bool>(mCurrentTarget);
  FinishDrag(true, Dali::Ui::DragCancelReason::NONE);
  return dropped;
}

void DragAndDropDetector::CancelDrag()
{
  if(mActivationPending)
  {
    ResetPendingActivation();
  }
  else
  {
    FinishDrag(false, Dali::Ui::DragCancelReason::REQUESTED);
  }
}

bool DragAndDropDetector::IsDragging() const
{
  return (mPointDown || mFinishing) && !mEmittingEnded;
}

Dali::Ui::DragSessionOrigin DragAndDropDetector::GetDragSessionOrigin() const
{
  return IsDragging() ? mDragSessionOrigin
                      : Dali::Ui::DragSessionOrigin::NONE;
}

Dali::Ui::View DragAndDropDetector::GetDragSource() const
{
  return IsDragging() ? mDragView : Dali::Ui::View();
}

Dali::Ui::View DragAndDropDetector::GetDragTarget() const
{
  return IsDragging() ? mCurrentTarget : Dali::Ui::View();
}

uint32_t DragAndDropDetector::GetAttachedSourceCount() const
{
  return mSources.size();
}
uint32_t DragAndDropDetector::GetAttachedTargetCount() const
{
  return mTargets.size();
}

const Dali::Ui::DragPayload& DragAndDropDetector::GetDragPayload() const
{
  return mSessionPayload;
}

Dali::Ui::View DragAndDropDetector::GetAttachedSource(uint32_t index) const
{
  return index < mSources.size() ? mSources[index] : Dali::Ui::View();
}

Dali::Ui::View DragAndDropDetector::GetAttachedTarget(uint32_t index) const
{
  return index < mTargets.size() ? mTargets[index] : Dali::Ui::View();
}

void DragAndDropDetector::SetDragPreview(Dali::Ui::View preview)
{
  DALI_ASSERT_ALWAYS(!mPointDown && "Drag preview cannot be changed during an active drag");
  DALI_ASSERT_ALWAYS((!preview || !preview.GetParent()) && "Drag preview must not have a parent");
  mConfiguredDragPreview = preview;
}

void DragAndDropDetector::SetDragPreviewContainer(Dali::Ui::View container)
{
  AssertDragConfigurationChangeAllowed();
  mDragPreviewContainer = container;
}

void DragAndDropDetector::SetDragPreviewCallbacks(DragPreviewFactory   factory,
                                                  DragPreviewUpdater   updater,
                                                  DragPreviewFinalizer finalizer)
{
  AssertDragConfigurationChangeAllowed();
  DALI_ASSERT_ALWAYS(factory && "A drag preview factory is required");
  mDragPreviewFactory   = std::move(factory);
  mDragPreviewUpdater   = std::move(updater);
  mDragPreviewFinalizer = std::move(finalizer);
}

void DragAndDropDetector::ClearDragPreviewCallbacks()
{
  AssertDragConfigurationChangeAllowed();
  mDragPreviewFactory   = {};
  mDragPreviewUpdater   = {};
  mDragPreviewFinalizer = {};
}

Dali::Ui::View DragAndDropDetector::HitTestTarget(
  const Vector2&                     screenPosition,
  const std::vector<Dali::Ui::View>& excludedTargets) const
{
  if(!mDragView)
  {
    return {};
  }

  Dali::Integration::SceneHolder sceneHolder = Dali::Integration::SceneHolder::Get(mDragView);
  if(!sceneHolder)
  {
    return {};
  }

  const TargetHitTestContext context{&mTargets, &excludedTargets, mDragView};
  ScopedTargetHitTestContext contextScope(context);

  Dali::HitTestAlgorithm::Results results;
  Dali::Integration::Scene        scene = Dali::GetImplementation(sceneHolder).GetScene();
  if(Dali::HitTestAlgorithm::HitTest(
       scene,
       screenPosition,
       results,
       IsRegisteredTargetHitTestActor))
  {
    return Dali::Ui::View::DownCast(results.actor);
  }
  return {};
}

bool DragAndDropDetector::IsTargetEligible(const Dali::Ui::View& target) const
{
  if(!target || !mDragView ||
     !target.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) ||
     Dali::Integration::SceneHolder::Get(target) !=
       Dali::Integration::SceneHolder::Get(mDragView))
  {
    return false;
  }
  return IsActorHierarchyEligible(target);
}

Vector2 DragAndDropDetector::GetViewScreenCenter(const Dali::Ui::View& view) const
{
  const Bounds screenExtents = view.CalculateScreenExtents();
  return Vector2(screenExtents.x + screenExtents.width * 0.5f,
                 screenExtents.y + screenExtents.height * 0.5f);
}

Dali::Ui::DragPayload DragAndDropDetector::FindSourcePayload(Dali::Ui::View source) const
{
  auto match = std::find_if(mSourcePayloads.begin(), mSourcePayloads.end(), [&source](const auto& entry)
  {
    return entry.first == source;
  });
  return match == mSourcePayloads.end() ? Dali::Ui::DragPayload() : match->second;
}

DragAndDropDetector::SourcePayloadProvider* DragAndDropDetector::FindSourcePayloadProvider(
  Dali::Ui::View source)
{
  auto match = std::find_if(
    mSourcePayloadProviders.begin(),
    mSourcePayloadProviders.end(),
    [&source](const auto& entry)
  {
    return entry.first == source;
  });
  return match == mSourcePayloadProviders.end() ? nullptr : &match->second;
}

Dali::Ui::DropProposal DragAndDropDetector::NormalizeDropProposal(
  Dali::Ui::DropProposal proposal) const
{
  if(!proposal.IsAccepted())
  {
    return Dali::Ui::DropProposal::Reject();
  }

  Dali::String selectedType = proposal.GetSelectedRepresentationType();
  if(selectedType.Empty() && mSessionPayload.GetRepresentationCount() > 0u)
  {
    selectedType = mSessionPayload.GetRepresentationType(0u);
  }
  if(!selectedType.Empty() && !mSessionPayload.HasRepresentation(selectedType))
  {
    DALI_LOG_ERROR("Drop proposal selected an unavailable representation; rejecting target.\n");
    return Dali::Ui::DropProposal::Reject();
  }

  Dali::Ui::DragAndDropOperation operation = proposal.GetOperation();
  if(operation == Dali::Ui::DragAndDropOperation::NONE &&
     mSessionPayload.GetAllowedOperationCount() > 0u)
  {
    const Dali::Ui::DragAndDropOperation preferred =
      mSessionPayload.GetPreferredOperation();
    operation = mSessionPayload.IsOperationAllowed(preferred)
                  ? preferred
                  : mSessionPayload.GetAllowedOperation(0u);
  }
  if(operation != Dali::Ui::DragAndDropOperation::NONE &&
     !mSessionPayload.IsOperationAllowed(operation))
  {
    DALI_LOG_ERROR("Drop proposal selected an operation not allowed by the source; rejecting target.\n");
    return Dali::Ui::DropProposal::Reject();
  }

  return Dali::Ui::DropProposal::Accept(std::move(selectedType), operation);
}

Dali::Ui::DropProposal DragAndDropDetector::ResolveDropProposal(uint32_t targetIndex)
{
  DropProposalCallback& callback = mDropProposalCallbacks[targetIndex];
  if(!callback)
  {
    return NormalizeDropProposal(Dali::Ui::DropProposal::Accept());
  }

  const Dali::Ui::View             target = mTargets[targetIndex];
  const Dali::Ui::DragAndDropEvent event =
    CreateEvent(Dali::Ui::DragAndDropResult::NONE,
                {},
                Dali::Ui::DragCancelReason::NONE,
                target,
                Dali::Ui::DragAndDropTargetStatus::NONE,
                Dali::Ui::DropProposal::Reject());
  ScopedTrueFlag         evaluatingDropProposal(mEvaluatingDropProposal);
  Dali::Ui::DropProposal proposal = callback.Invoke(event);
  return mPointDown ? NormalizeDropProposal(std::move(proposal))
                    : Dali::Ui::DropProposal::Reject();
}

DragAndDropDetector::TargetResolution DragAndDropDetector::FindTarget(const Vector2& screenPosition)
{
  TargetResolution            resolution;
  Dali::Ui::View              rejectedTarget;
  std::vector<Dali::Ui::View> excludedTargets;
  const uint32_t              maximumAttempts = static_cast<uint32_t>(mTargets.size()) * 2u + 1u;
  for(uint32_t attempt = 0u; mPointDown && attempt < maximumAttempts; ++attempt)
  {
    const Dali::Ui::View target = HitTestTarget(screenPosition, excludedTargets);
    if(!target)
    {
      break;
    }

    const auto match = std::find(mTargets.begin(), mTargets.end(), target);
    if(match == mTargets.end())
    {
      excludedTargets.push_back(target);
      continue;
    }

    const uint32_t         targetIndex = static_cast<uint32_t>(std::distance(mTargets.begin(), match));
    Dali::Ui::DropProposal proposal    = ResolveDropProposal(targetIndex);
    if(!mPointDown)
    {
      return {};
    }

    if(proposal.IsAccepted())
    {
      if(IsTargetEligible(target) &&
         HitTestTarget(screenPosition, excludedTargets) == target)
      {
        resolution.acceptedTarget = target;
        resolution.proposal       = std::move(proposal);
        break;
      }
    }
    else if(IsTargetEligible(target) && !rejectedTarget)
    {
      rejectedTarget = target;
    }

    excludedTargets.push_back(target);
  }

  if(resolution.acceptedTarget)
  {
    resolution.feedbackTarget = resolution.acceptedTarget;
    resolution.status         = Dali::Ui::DragAndDropTargetStatus::ACCEPTED;
  }
  else if(rejectedTarget)
  {
    resolution.feedbackTarget = rejectedTarget;
    resolution.status         = Dali::Ui::DragAndDropTargetStatus::REJECTED;
  }
  return resolution;
}

Dali::Ui::DragAndDropEvent DragAndDropDetector::CreateEvent(
  Dali::Ui::DragAndDropResult result,
  Dali::Ui::View              target,
  Dali::Ui::DragCancelReason  cancelReason) const
{
  return CreateEvent(result,
                     target,
                     cancelReason,
                     mFeedbackTarget,
                     mTargetStatus,
                     mDropProposal);
}

Dali::Ui::DragAndDropEvent DragAndDropDetector::CreateEvent(
  Dali::Ui::DragAndDropResult       result,
  Dali::Ui::View                    target,
  Dali::Ui::DragCancelReason        cancelReason,
  Dali::Ui::View                    candidateTarget,
  Dali::Ui::DragAndDropTargetStatus targetStatus,
  Dali::Ui::DropProposal            proposal) const
{
  Dali::Ui::View       resolvedTarget = target ? target : mCurrentTarget;
  Vector2              targetLocalPosition;
  const Dali::Ui::View coordinateTarget =
    candidateTarget ? candidateTarget : resolvedTarget;
  if(coordinateTarget)
  {
    Vector2 convertedPosition;
    if(coordinateTarget.ScreenToLocal(convertedPosition.x,
                                      convertedPosition.y,
                                      mScreenPosition.x,
                                      mScreenPosition.y))
    {
      targetLocalPosition = convertedPosition;
    }
  }
  return Dali::Ui::DragAndDropEvent(mDragView,
                                    std::move(resolvedTarget),
                                    std::move(candidateTarget),
                                    mDragDeviceClass,
                                    mDragSessionOrigin,
                                    mScreenPosition,
                                    mSourceGrabPosition,
                                    mSourceParentPosition,
                                    targetLocalPosition,
                                    mPreviewLocalPosition,
                                    mSourceAnchor,
                                    mSessionPayload,
                                    std::move(proposal),
                                    targetStatus,
                                    result,
                                    cancelReason);
}

void DragAndDropDetector::CreateDragPreview()
{
  Dali::Ui::View visual;
  if(mDragPreviewFactory)
  {
    const Dali::Ui::DragAndDropEvent event = CreateEvent(Dali::Ui::DragAndDropResult::NONE);
    visual                                 = mDragPreviewFactory.Invoke(event);
    if(!mPointDown)
    {
      return;
    }
  }
  else
  {
    visual = mConfiguredDragPreview;
  }

  if(visual && visual.GetParent())
  {
    DALI_LOG_ERROR("Drag preview must be detached; continuing without a preview.\n");
    visual.Reset();
  }
  mSessionDragPreview = visual;
  if(mSessionDragPreview)
  {
    Dali::Actor parent = mDragPreviewContainer ? Dali::Actor(mDragPreviewContainer)
                                               : mDragView.GetParent();
    if(parent)
    {
      parent.Add(mSessionDragPreview);
    }
  }
}

void DragAndDropDetector::UpdateDragPreview()
{
  if(!mSessionDragPreview)
  {
    return;
  }

  const Dali::Ui::DragAndDropEvent event = CreateEvent(Dali::Ui::DragAndDropResult::NONE);
  if(mDragPreviewUpdater)
  {
    mDragPreviewUpdater.Invoke(mSessionDragPreview, event);
  }
  else
  {
    if(!mDragPreviewPositionSignal.Empty())
    {
      mDragPreviewPositionSignal.Emit(mSessionDragPreview, event);
    }
    else
    {
      ApplyDefaultDragPreviewPosition();
    }
  }
}

void DragAndDropDetector::UpdateCoordinateSnapshots(const Vector2& screenPosition)
{
  SetPosition(screenPosition);

  Actor sourceParent    = mDragView.GetParent();
  mSourceParentPosition = screenPosition;
  if(sourceParent)
  {
    Vector2 convertedPosition;
    if(sourceParent.ScreenToLocal(convertedPosition.x,
                                  convertedPosition.y,
                                  screenPosition.x,
                                  screenPosition.y))
    {
      mSourceParentPosition = convertedPosition;
    }
  }

  Actor visualParent    = mDragPreviewContainer ? Actor(mDragPreviewContainer) : sourceParent;
  mPreviewLocalPosition = screenPosition;
  if(visualParent)
  {
    Vector2 convertedPosition;
    if(visualParent.ScreenToLocal(convertedPosition.x,
                                  convertedPosition.y,
                                  screenPosition.x,
                                  screenPosition.y))
    {
      mPreviewLocalPosition = convertedPosition;
    }
  }
}

void DragAndDropDetector::ApplyDefaultDragPreviewPosition()
{
  if(!mDragPreviewPositioningPropertiesOverridden)
  {
    mOriginalDragPreviewPivot =
      mSessionDragPreview.GetProperty<Vector3>(Actor::Property::PIVOT);
    mOriginalDragPreviewParentOrigin =
      mSessionDragPreview.GetProperty<Vector3>(Actor::Property::PARENT_ORIGIN);
    mOriginalDragPreviewLayoutMode    = mSessionDragPreview.GetLayoutMode();
    mOriginalDragPreviewUiScalePolicy = mSessionDragPreview.GetUiScalePolicy();
    mOriginalDragPreviewRequestedX    = mSessionDragPreview.GetRequestedX();
    mOriginalDragPreviewRequestedY    = mSessionDragPreview.GetRequestedY();
    mOriginalDragPreviewPositionUsesPivot =
      mSessionDragPreview.GetProperty<bool>(Actor::Property::POSITION_USES_PIVOT);

    mSessionDragPreview.SetLayoutMode(LayoutMode::STANDALONE);
    mSessionDragPreview.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mSessionDragPreview.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mSessionDragPreview.SetProperty(Actor::Property::POSITION_USES_PIVOT, true);
    mSessionDragPreview.SetProperty(
      Actor::Property::PIVOT,
      Vector3(mSourceAnchor.x, mSourceAnchor.y, mOriginalDragPreviewPivot.z));
    mDragPreviewPositioningPropertiesOverridden = true;
  }

  mSessionDragPreview.SetRequestedX(mPreviewLocalPosition.x);
  mSessionDragPreview.SetRequestedY(mPreviewLocalPosition.y);
}

void DragAndDropDetector::RestoreDragPreviewPositioningProperties(Dali::Ui::View visual)
{
  if(visual && mDragPreviewPositioningPropertiesOverridden)
  {
    visual.SetProperty(Actor::Property::PIVOT, mOriginalDragPreviewPivot);
    visual.SetProperty(Actor::Property::PARENT_ORIGIN, mOriginalDragPreviewParentOrigin);
    visual.SetLayoutMode(mOriginalDragPreviewLayoutMode);
    visual.SetUiScalePolicy(mOriginalDragPreviewUiScalePolicy);
    visual.SetRequestedX(mOriginalDragPreviewRequestedX);
    visual.SetRequestedY(mOriginalDragPreviewRequestedY);
    visual.SetProperty(Actor::Property::POSITION_USES_PIVOT,
                       mOriginalDragPreviewPositionUsesPivot);
  }
  mDragPreviewPositioningPropertiesOverridden = false;
}

void DragAndDropDetector::FinalizeDragPreview(Dali::Ui::View visual, const Dali::Ui::DragAndDropEvent& event)
{
  RemoveDragPreview(visual);
  RestoreDragPreviewPositioningProperties(visual);
  if(visual && mDragPreviewFinalizer)
  {
    mDragPreviewFinalizer.Invoke(visual, event);
  }
}

void DragAndDropDetector::ResolveTarget(const Vector2& screenPosition, bool emitMoved)
{
  Dali::Ui::DragAndDropDetector keepAlive(this);
  TargetResolution              resolution = FindTarget(screenPosition);
  Dali::Ui::View                nextTarget = resolution.acceptedTarget;
  if(!mPointDown)
  {
    return;
  }
  if(nextTarget != mCurrentTarget)
  {
    if(mCurrentTarget)
    {
      Dali::Ui::View previousTarget = mCurrentTarget;
      mCurrentTarget.Reset();
      EmitExitedSignal(previousTarget);
      if(!mPointDown)
      {
        return;
      }
      // An exit callback may change the registered targets or scene geometry.
      resolution = FindTarget(screenPosition);
      nextTarget = resolution.acceptedTarget;
    }
    mDropProposal  = resolution.proposal;
    mCurrentTarget = nextTarget;
    if(mCurrentTarget)
    {
      EmitEnteredSignal(mCurrentTarget);
      if(!mPointDown || mCurrentTarget != nextTarget)
      {
        return;
      }
    }
  }
  else
  {
    mDropProposal = resolution.proposal;
  }
  UpdateTargetFeedback(resolution.feedbackTarget, resolution.status);
  if(!mPointDown)
  {
    return;
  }
  if(emitMoved && mCurrentTarget)
  {
    EmitMovedSignal(mCurrentTarget);
  }
}

bool DragAndDropDetector::UpdateExplicitDrag(Dali::Ui::View target)
{
  if(!mPointDown ||
     mDragSessionOrigin != Dali::Ui::DragSessionOrigin::EXPLICIT ||
     target == mDragView)
  {
    return false;
  }

  auto targetMatch = std::find(mTargets.begin(), mTargets.end(), target);
  if(targetMatch == mTargets.end() || !IsTargetEligible(target))
  {
    return false;
  }

  UpdateCoordinateSnapshots(GetViewScreenCenter(target));
  const uint32_t targetIndex =
    static_cast<uint32_t>(std::distance(mTargets.begin(), targetMatch));
  Dali::Ui::DropProposal proposal = ResolveDropProposal(targetIndex);
  if(!mPointDown)
  {
    return false;
  }

  const auto targetIsAvailable = [this, &target]()
  {
    return std::find(mTargets.begin(), mTargets.end(), target) != mTargets.end() &&
           IsTargetEligible(target);
  };

  TargetResolution resolution;
  if(targetIsAvailable())
  {
    resolution.feedbackTarget = target;
    resolution.status         = proposal.IsAccepted()
                                  ? Dali::Ui::DragAndDropTargetStatus::ACCEPTED
                                  : Dali::Ui::DragAndDropTargetStatus::REJECTED;
    if(proposal.IsAccepted())
    {
      resolution.acceptedTarget = target;
      resolution.proposal       = std::move(proposal);
    }
  }

  Dali::Ui::View nextTarget = resolution.acceptedTarget;
  if(nextTarget != mCurrentTarget)
  {
    if(mCurrentTarget)
    {
      Dali::Ui::View previousTarget = mCurrentTarget;
      mCurrentTarget.Reset();
      EmitExitedSignal(previousTarget);
      if(!mPointDown)
      {
        return false;
      }
    }

    if(nextTarget && !targetIsAvailable())
    {
      nextTarget.Reset();
      resolution = {};
    }
    mDropProposal  = resolution.proposal;
    mCurrentTarget = nextTarget;
    if(mCurrentTarget)
    {
      EmitEnteredSignal(mCurrentTarget);
      if(!mPointDown || mCurrentTarget != nextTarget)
      {
        return false;
      }
    }
  }
  else
  {
    mDropProposal = resolution.proposal;
  }

  UpdateTargetFeedback(resolution.feedbackTarget, resolution.status);
  if(!mPointDown)
  {
    return false;
  }
  if(mCurrentTarget == target)
  {
    EmitMovedSignal(mCurrentTarget);
    if(!mPointDown || mCurrentTarget != target)
    {
      return false;
    }
  }

  UpdateDragPreview();
  if(mPointDown)
  {
    UpdateAutoScrollState();
  }
  return mPointDown && mCurrentTarget == target;
}

void DragAndDropDetector::ClearExplicitTarget(Dali::Ui::View target)
{
  if(mCurrentTarget == target)
  {
    mCurrentTarget.Reset();
    EmitExitedSignal(target);
    if(!mPointDown)
    {
      return;
    }
    mDropProposal = Dali::Ui::DropProposal::Reject();
  }
  StopAutoScrollTimer();
  if(mFeedbackTarget == target)
  {
    UpdateTargetFeedback({}, Dali::Ui::DragAndDropTargetStatus::NONE);
  }
  if(mPointDown)
  {
    UpdateDragPreview();
  }
}

void DragAndDropDetector::UpdateTargetFeedback(Dali::Ui::View                    target,
                                               Dali::Ui::DragAndDropTargetStatus status)
{
  if(mFeedbackTarget == target && mTargetStatus == status)
  {
    return;
  }

  mFeedbackTarget = target;
  mTargetStatus   = status;
  if(!mTargetFeedbackChangedSignal.Empty())
  {
    const Dali::Ui::DragAndDropEvent event = CreateEvent(Dali::Ui::DragAndDropResult::NONE);
    Dali::Ui::DragAndDropDetector    handle(this);
    mTargetFeedbackChangedSignal.Emit(event, handle);
  }
}

void DragAndDropDetector::ClearTargetFeedback(Dali::Ui::DragAndDropResult result,
                                              Dali::Ui::View              finishingTarget,
                                              Dali::Ui::DragCancelReason  cancelReason)
{
  if(!mFeedbackTarget && mTargetStatus == Dali::Ui::DragAndDropTargetStatus::NONE)
  {
    return;
  }

  mFeedbackTarget.Reset();
  mTargetStatus = Dali::Ui::DragAndDropTargetStatus::NONE;
  if(!mTargetFeedbackChangedSignal.Empty())
  {
    const Dali::Ui::DragAndDropEvent event = CreateEvent(result, finishingTarget, cancelReason);
    Dali::Ui::DragAndDropDetector    handle(this);
    mTargetFeedbackChangedSignal.Emit(event, handle);
  }
}

void DragAndDropDetector::UpdateDrag(const Vector2& screenPosition)
{
  UpdateCoordinateSnapshots(screenPosition);

  ResolveTarget(screenPosition, true);
  if(!mPointDown)
  {
    return;
  }
  UpdateDragPreview();
  if(mPointDown)
  {
    UpdateAutoScrollState();
  }
}

void DragAndDropDetector::RemoveDragPreview(Dali::Ui::View visual)
{
  if(visual && visual.GetParent())
  {
    visual.GetParent().Remove(visual);
  }
}

void DragAndDropDetector::FinishDrag(bool dropped, Dali::Ui::DragCancelReason cancelReason)
{
  if(!mPointDown)
  {
    return;
  }

  Dali::Ui::DragAndDropDetector keepAlive(this);
  Dali::Ui::View                target  = mCurrentTarget;
  Dali::Ui::View                preview = mSessionDragPreview;
  const bool                    cancelled =
    cancelReason != Dali::Ui::DragCancelReason::NONE;
  const Dali::Ui::DragAndDropResult result =
    dropped && target ? Dali::Ui::DragAndDropResult::DROPPED
                      : (cancelled ? Dali::Ui::DragAndDropResult::CANCELLED
                                   : Dali::Ui::DragAndDropResult::NO_TARGET);
  const Dali::Ui::DragAndDropEvent event = CreateEvent(result, target, cancelReason);
  mPointDown                             = false;
  mFinishing                             = true;
  StopAutoScrollTimer();
  StopInterruptionMonitoring();

  if(dropped && target)
  {
    EmitDroppedSignal(event);
  }
  else if(target)
  {
    EmitExitedSignal(target, result, cancelReason);
  }

  if(cancelled && !mCancelledSignal.Empty())
  {
    Dali::Ui::DragAndDropDetector handle(this);
    mCancelledSignal.Emit(event, handle);
  }

  ClearTargetFeedback(result, target, cancelReason);

  // A signal callback may configure a different reusable preview for the next
  // drag. Finalize the preview captured for this session.
  FinalizeDragPreview(preview, event);
  mSessionDragPreview.Reset();
  mCurrentTarget.Reset();
  mDragView.Reset();
  mSessionPayload    = {};
  mDropProposal      = Dali::Ui::DropProposal::Reject();
  mPanTracking       = false;
  mDragDeviceClass   = Device::Class::NONE;
  mDragSessionOrigin = Dali::Ui::DragSessionOrigin::NONE;
  mEmittingEnded     = true;
  EmitEndedSignal(event);
  mEmittingEnded = false;
  mFinishing     = false;
}

void DragAndDropDetector::StartInterruptionMonitoring()
{
  mDragView.TouchEventSignal().Connect(this, &DragAndDropDetector::OnSourceTouch);
  mDragView.SceneDisconnectedSignal().Connect(this, &DragAndDropDetector::OnSourceSceneDisconnected);
  if(mDragPreviewContainer && mDragPreviewContainer != mDragView)
  {
    mDragPreviewContainer.SceneDisconnectedSignal().Connect(
      this,
      &DragAndDropDetector::OnPreviewContainerSceneDisconnected);
  }

  mSessionWindow = Dali::Window::Get(mDragView);
  if(mSessionWindow)
  {
    mSessionWindow.FocusChangedSignal().Connect(this, &DragAndDropDetector::OnWindowFocusChanged);
  }
}

void DragAndDropDetector::StopInterruptionMonitoring()
{
  if(mDragView)
  {
    mDragView.TouchEventSignal().Disconnect(this, &DragAndDropDetector::OnSourceTouch);
    mDragView.SceneDisconnectedSignal().Disconnect(this, &DragAndDropDetector::OnSourceSceneDisconnected);
  }
  if(mDragPreviewContainer && mDragPreviewContainer != mDragView)
  {
    mDragPreviewContainer.SceneDisconnectedSignal().Disconnect(
      this,
      &DragAndDropDetector::OnPreviewContainerSceneDisconnected);
  }
  if(mSessionWindow)
  {
    mSessionWindow.FocusChangedSignal().Disconnect(this, &DragAndDropDetector::OnWindowFocusChanged);
    mSessionWindow.Reset();
  }
}

bool DragAndDropDetector::OnSourceTouch(Dali::Actor, Dali::TouchEvent touch)
{
  if(touch.GetPointCount() > 1u)
  {
    if(mActivationPending)
    {
      ResetPendingActivation();
    }
    else if(mPointDown)
    {
      FinishDrag(false, Dali::Ui::DragCancelReason::GESTURE_INTERRUPTED);
    }
  }
  return false;
}

void DragAndDropDetector::OnSourceSceneDisconnected(Dali::Actor)
{
  if(mActivationPending)
  {
    ResetPendingActivation();
  }
  else
  {
    FinishDrag(false, Dali::Ui::DragCancelReason::SOURCE_DISCONNECTED);
  }
}

void DragAndDropDetector::OnPreviewContainerSceneDisconnected(Dali::Actor)
{
  if(mActivationPending)
  {
    ResetPendingActivation();
    return;
  }
  const bool sourceDisconnected =
    !mDragView ||
    !mDragView.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE);
  FinishDrag(
    false,
    sourceDisconnected ? Dali::Ui::DragCancelReason::SOURCE_DISCONNECTED
                       : Dali::Ui::DragCancelReason::PREVIEW_CONTAINER_DISCONNECTED);
}

void DragAndDropDetector::OnTargetSceneDisconnected(Dali::Actor actor)
{
  Dali::Ui::View target = Dali::Ui::View::DownCast(actor);
  if(mPointDown && (mCurrentTarget == target || mFeedbackTarget == target))
  {
    if(mDragSessionOrigin == Dali::Ui::DragSessionOrigin::EXPLICIT)
    {
      ClearExplicitTarget(target);
    }
    else
    {
      ResolveTarget(mScreenPosition, false);
    }
  }
}

void DragAndDropDetector::OnAutoScrollViewportSceneDisconnected(Dali::Actor)
{
  StopAutoScrollTimer();
}

void DragAndDropDetector::OnWindowFocusChanged(Dali::Window, bool focusIn)
{
  if(!focusIn)
  {
    if(mActivationPending)
    {
      ResetPendingActivation();
    }
    else
    {
      FinishDrag(false, Dali::Ui::DragCancelReason::WINDOW_FOCUS_LOST);
    }
  }
}

Vector2 DragAndDropDetector::CalculateAutoScrollEdgeIntensity() const
{
  const Dali::Ui::View viewport = mAutoScrollConfiguration.GetViewport();
  if(!viewport ||
     !viewport.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    return Vector2::ZERO;
  }

  float localX = 0.0f;
  float localY = 0.0f;
  if(!viewport.ScreenToLocal(localX, localY, mScreenPosition.x, mScreenPosition.y))
  {
    return Vector2::ZERO;
  }

  const Vector3 size          = viewport.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  const auto    calculateAxis = [](float position, float extent, float threshold, float maxSpeed)
  {
    if(extent <= 0.0f || threshold <= 0.0f || maxSpeed <= 0.0f)
    {
      return 0.0f;
    }

    const float effectiveThreshold = std::min(threshold, extent * 0.5f);
    if(position < effectiveThreshold)
    {
      return -std::clamp((effectiveThreshold - position) / effectiveThreshold, 0.0f, 1.0f);
    }
    if(position > extent - effectiveThreshold)
    {
      return std::clamp((position - (extent - effectiveThreshold)) / effectiveThreshold, 0.0f, 1.0f);
    }
    return 0.0f;
  };

  return Vector2(
    calculateAxis(localX, size.x, mAutoScrollConfiguration.GetEdgeThreshold().x, mAutoScrollConfiguration.GetMaxSpeed().x),
    calculateAxis(localY, size.y, mAutoScrollConfiguration.GetEdgeThreshold().y, mAutoScrollConfiguration.GetMaxSpeed().y));
}

void DragAndDropDetector::UpdateAutoScrollState()
{
  if(!mPointDown || !mAutoScrollCallback)
  {
    StopAutoScrollTimer();
    return;
  }

  mAutoScrollEdgeIntensity = CalculateAutoScrollEdgeIntensity();
  if(mAutoScrollEdgeIntensity.x == 0.0f && mAutoScrollEdgeIntensity.y == 0.0f)
  {
    StopAutoScrollTimer();
  }
  else
  {
    StartAutoScrollTimer();
  }
}

void DragAndDropDetector::StartAutoScrollTimer()
{
  if(!mAutoScrollTimer)
  {
    mAutoScrollTimer = Dali::Timer::New(mAutoScrollConfiguration.GetTickIntervalMilliseconds());
    mAutoScrollTimer.TickSignal().Connect(this, &DragAndDropDetector::OnAutoScrollTick);
  }
  if(!mAutoScrollTicking)
  {
    mAutoScrollTimer.Start();
    mAutoScrollTicking = true;
  }
}

void DragAndDropDetector::StopAutoScrollTimer()
{
  if(mAutoScrollTicking)
  {
    mAutoScrollTimer.Stop();
    mAutoScrollTicking = false;
  }
  mAutoScrollEdgeIntensity = Vector2::ZERO;
}

bool DragAndDropDetector::OnAutoScrollTick()
{
  if(!mAutoScrollTicking || !mPointDown || !mAutoScrollCallback)
  {
    return false;
  }

  Dali::Ui::DragAndDropDetector keepAlive(this);
  mAutoScrollEdgeIntensity = CalculateAutoScrollEdgeIntensity();
  if(mAutoScrollEdgeIntensity.x == 0.0f && mAutoScrollEdgeIntensity.y == 0.0f)
  {
    StopAutoScrollTimer();
    return false;
  }

  const float deltaSeconds =
    static_cast<float>(mAutoScrollConfiguration.GetTickIntervalMilliseconds()) * 0.001f;
  const Vector2 suggestedDelta(
    mAutoScrollEdgeIntensity.x * mAutoScrollConfiguration.GetMaxSpeed().x * deltaSeconds,
    mAutoScrollEdgeIntensity.y * mAutoScrollConfiguration.GetMaxSpeed().y * deltaSeconds);
  const Dali::Ui::DragAutoScrollEvent event(
    CreateEvent(Dali::Ui::DragAndDropResult::NONE),
    mAutoScrollConfiguration.GetViewport(),
    mAutoScrollEdgeIntensity,
    suggestedDelta,
    deltaSeconds);

  const bool contentChanged = mAutoScrollCallback.Invoke(event);
  if(!mPointDown)
  {
    return false;
  }
  if(!contentChanged)
  {
    StopAutoScrollTimer();
    return false;
  }

  if(mDragSessionOrigin == Dali::Ui::DragSessionOrigin::EXPLICIT)
  {
    Dali::Ui::View target = mFeedbackTarget ? mFeedbackTarget : mCurrentTarget;
    if(target)
    {
      UpdateExplicitDrag(target);
    }
  }
  else
  {
    UpdateDrag(mScreenPosition);
  }
  return mAutoScrollTicking;
}

void DragAndDropDetector::BeginActivation(Dali::Ui::View                        view,
                                          const Vector2&                        localPosition,
                                          const Vector2&                        screenPosition,
                                          const Vector2&                        originScreenPosition,
                                          Device::Class::Type                   deviceClass,
                                          Dali::Ui::DragActivationConfiguration configuration,
                                          bool                                  panTracking)
{
  mActivationPending              = true;
  mPendingPayloadResolved         = false;
  mPendingPayload                 = {};
  mDragView                       = view;
  mActivationOriginScreenPosition = originScreenPosition;
  mPendingActivationMode          = configuration.GetMode();
  mPendingDragStartThreshold      = configuration.GetMovementThreshold();
  mDragDeviceClass                = deviceClass;
  mPanTracking                    = panTracking;
  StartInterruptionMonitoring();

  TryActivateDrag(view, localPosition, screenPosition);
}

bool DragAndDropDetector::HasReachedDragStartThreshold(const Vector2& screenPosition) const
{
  const Vector2 displacement = screenPosition - mActivationOriginScreenPosition;
  return displacement.LengthSquared() >=
         mPendingDragStartThreshold * mPendingDragStartThreshold;
}

void DragAndDropDetector::TryActivateDrag(Dali::Ui::View view,
                                          const Vector2& localPosition,
                                          const Vector2& screenPosition)
{
  if(!mActivationPending ||
     view != mDragView ||
     !HasReachedDragStartThreshold(screenPosition))
  {
    return;
  }

  Dali::Ui::DragPayload payload =
    mPendingPayloadResolved ? mPendingPayload : FindSourcePayload(view);
  Dali::Ui::DragActivationEvent event(view,
                                      mPendingActivationMode,
                                      mDragDeviceClass,
                                      mActivationOriginScreenPosition,
                                      screenPosition,
                                      localPosition,
                                      screenPosition - mActivationOriginScreenPosition,
                                      payload);

  if(!mPendingPayloadResolved)
  {
    if(SourcePayloadProvider* provider = FindSourcePayloadProvider(view))
    {
      payload = provider->Invoke(event);
      if(!mActivationPending ||
         view != mDragView ||
         std::find(mSources.begin(), mSources.end(), view) == mSources.end())
      {
        return;
      }
    }
    mPendingPayload         = payload;
    mPendingPayloadResolved = true;
    event                   = Dali::Ui::DragActivationEvent(view,
                                                            mPendingActivationMode,
                                                            mDragDeviceClass,
                                                            mActivationOriginScreenPosition,
                                                            screenPosition,
                                                            localPosition,
                                                            screenPosition - mActivationOriginScreenPosition,
                                                            payload);
  }

  if(mCanStartDragCallback)
  {
    const bool approved = mCanStartDragCallback.Invoke(event);
    if(!approved)
    {
      return;
    }
  }

  if(!mActivationPending ||
     view != mDragView ||
     std::find(mSources.begin(), mSources.end(), view) == mSources.end() ||
     !view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    return;
  }

  mDragSessionOrigin = Dali::Ui::DragSessionOrigin::GESTURE;
  ActivateDrag(view, localPosition, screenPosition, std::move(payload));
}

void DragAndDropDetector::ActivateDrag(Dali::Ui::View        view,
                                       const Vector2&        localPosition,
                                       const Vector2&        screenPosition,
                                       Dali::Ui::DragPayload payload)
{
  mActivationPending      = false;
  mPendingPayloadResolved = false;
  mPendingPayload         = {};
  mSourceGrabPosition     = localPosition;
  mPointDown              = true;
  mSessionPayload         = std::move(payload);
  mCurrentTarget.Reset();
  mFeedbackTarget.Reset();
  mDropProposal                               = Dali::Ui::DropProposal::Reject();
  mTargetStatus                               = Dali::Ui::DragAndDropTargetStatus::NONE;
  const Vector3 sourceSize                    = view.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  mSourceAnchor.x                             = sourceSize.x > 0.0f
                                                  ? std::clamp(mSourceGrabPosition.x / sourceSize.x, 0.0f, 1.0f)
                                                  : 0.5f;
  mSourceAnchor.y                             = sourceSize.y > 0.0f
                                                  ? std::clamp(mSourceGrabPosition.y / sourceSize.y, 0.0f, 1.0f)
                                                  : 0.5f;
  mDragPreviewPositioningPropertiesOverridden = false;
  UpdateCoordinateSnapshots(screenPosition);
  CreateDragPreview();
  if(!mPointDown)
  {
    return;
  }
  // A long-press is activated by a recognizer timer, so there may be no
  // following pointer motion to drive the first visual update or frame.
  // Present the preview as part of activation itself.
  UpdateDragPreview();
  if(!mPointDown)
  {
    return;
  }
  EmitStartedSignal();
  if(mPointDown)
  {
    UpdateDrag(screenPosition);
    if(mSessionDragPreview && mSessionWindow)
    {
      mSessionWindow.KeepRendering(0.0f);
    }
  }
}

void DragAndDropDetector::ResetPendingActivation()
{
  if(!mActivationPending)
  {
    return;
  }

  mActivationPending      = false;
  mPendingPayloadResolved = false;
  mPendingPayload         = {};
  StopInterruptionMonitoring();
  mDragView.Reset();
  mActivationOriginScreenPosition = Vector2::ZERO;
  mPendingActivationMode          = mDragActivationMode;
  mPendingDragStartThreshold      = mDragStartThreshold;
  mDragDeviceClass                = Device::Class::NONE;
  mDragSessionOrigin              = Dali::Ui::DragSessionOrigin::NONE;
  mPanTracking                    = false;
}

void DragAndDropDetector::OnPan(Dali::Actor actor, PanGesture gesture)
{
  Dali::Ui::DragAndDropDetector keepAlive(this);
  Dali::Ui::View                view = Dali::Ui::View::DownCast(actor);

  if(mPointDown &&
     mDragSessionOrigin == Dali::Ui::DragSessionOrigin::EXPLICIT)
  {
    return;
  }

  GestureState state = gesture.GetState();

  if(state == GestureState::STARTED)
  {
    const Dali::Ui::DragActivationConfiguration configuration =
      GetDragActivationConfiguration(gesture.GetDeviceClass());
    if(mPointDown && view == mDragView)
    {
      mPanTracking = true;
      UpdateDrag(gesture.GetScreenPosition());
    }
    else if(mActivationPending && view == mDragView)
    {
      mPanTracking = true;
      TryActivateDrag(view, gesture.GetPosition(), gesture.GetScreenPosition());
    }
    else if(configuration.GetMode() == Dali::Ui::DragActivationMode::PAN)
    {
      BeginActivation(
        view,
        gesture.GetPosition(),
        gesture.GetScreenPosition(),
        gesture.GetScreenPosition() - gesture.GetScreenDisplacement(),
        gesture.GetDeviceClass(),
        configuration,
        true);
    }
  }
  else if(mActivationPending && state == GestureState::CONTINUING)
  {
    if(view == mDragView)
    {
      mPanTracking = true;
      TryActivateDrag(view, gesture.GetPosition(), gesture.GetScreenPosition());
    }
  }
  else if(mPointDown && state == GestureState::CONTINUING)
  {
    UpdateDrag(gesture.GetScreenPosition());
  }
  else if(mPointDown && state == GestureState::FINISHED)
  {
    UpdateDrag(gesture.GetScreenPosition());
    FinishDrag(true, Dali::Ui::DragCancelReason::NONE);
  }
  else if(mPointDown && state == GestureState::CANCELLED)
  {
    FinishDrag(false, Dali::Ui::DragCancelReason::GESTURE_INTERRUPTED);
  }
  else if(mActivationPending &&
          (state == GestureState::FINISHED || state == GestureState::CANCELLED))
  {
    ResetPendingActivation();
  }
}

void DragAndDropDetector::OnLongPress(Dali::Actor actor, LongPressGesture gesture)
{
  Dali::Ui::DragAndDropDetector keepAlive(this);
  Dali::Ui::View                view  = Dali::Ui::View::DownCast(actor);
  const GestureState            state = gesture.GetState();

  if(mPointDown &&
     mDragSessionOrigin == Dali::Ui::DragSessionOrigin::EXPLICIT)
  {
    return;
  }

  const Dali::Ui::DragActivationConfiguration configuration =
    GetDragActivationConfiguration(gesture.GetDeviceClass());

  if(configuration.GetMode() != Dali::Ui::DragActivationMode::LONG_PRESS)
  {
    return;
  }

  if(state == GestureState::STARTED && !mActivationPending && !mPointDown)
  {
    BeginActivation(
      view,
      gesture.GetLocalPoint(),
      gesture.GetScreenPoint(),
      gesture.GetScreenPoint(),
      gesture.GetDeviceClass(),
      configuration,
      false);
  }
  else if(state == GestureState::FINISHED && view == mDragView && !mPanTracking)
  {
    if(mActivationPending)
    {
      ResetPendingActivation();
    }
    else if(mPointDown)
    {
      UpdateDrag(gesture.GetScreenPoint());
      FinishDrag(true, Dali::Ui::DragCancelReason::NONE);
    }
  }
}

const Vector2& DragAndDropDetector::GetCurrentScreenPosition() const
{
  return mScreenPosition;
}

void DragAndDropDetector::SetPosition(const Vector2& screenPosition)
{
  mScreenPosition = screenPosition;
}

void DragAndDropDetector::EmitStartedSignal()
{
  if(!mStartedSignal.Empty())
  {
    const Dali::Ui::DragAndDropEvent event =
      CreateEvent(Dali::Ui::DragAndDropResult::NONE);
    Dali::Ui::DragAndDropDetector handle(this);
    mStartedSignal.Emit(event, handle);
  }
}

void DragAndDropDetector::EmitEnteredSignal(Dali::Ui::View target)
{
  if(!mEnteredSignal.Empty())
  {
    const Dali::Ui::DragAndDropEvent event =
      CreateEvent(Dali::Ui::DragAndDropResult::NONE,
                  target,
                  Dali::Ui::DragCancelReason::NONE,
                  target,
                  Dali::Ui::DragAndDropTargetStatus::ACCEPTED,
                  mDropProposal);
    Dali::Ui::DragAndDropDetector handle(this);
    mEnteredSignal.Emit(event, handle);
  }
}

void DragAndDropDetector::EmitExitedSignal(
  Dali::Ui::View              target,
  Dali::Ui::DragAndDropResult result,
  Dali::Ui::DragCancelReason  cancelReason)
{
  if(!mExitedSignal.Empty())
  {
    const Dali::Ui::DragAndDropEvent event =
      CreateEvent(result,
                  target,
                  cancelReason,
                  target,
                  Dali::Ui::DragAndDropTargetStatus::NONE,
                  mDropProposal);
    Dali::Ui::DragAndDropDetector handle(this);
    mExitedSignal.Emit(event, handle);
  }
}

void DragAndDropDetector::EmitMovedSignal(Dali::Ui::View target)
{
  if(!mMovedSignal.Empty())
  {
    const Dali::Ui::DragAndDropEvent event =
      CreateEvent(Dali::Ui::DragAndDropResult::NONE, target);
    Dali::Ui::DragAndDropDetector handle(this);
    mMovedSignal.Emit(event, handle);
  }
}

void DragAndDropDetector::EmitDroppedSignal(const Dali::Ui::DragAndDropEvent& event)
{
  if(!mDroppedSignal.Empty())
  {
    Dali::Ui::DragAndDropDetector handle(this);
    mDroppedSignal.Emit(event, handle);
  }
}

void DragAndDropDetector::EmitEndedSignal(const Dali::Ui::DragAndDropEvent& event)
{
  if(!mEndedSignal.Empty())
  {
    Dali::Ui::DragAndDropDetector handle(this);
    mEndedSignal.Emit(event, handle);
  }
}

DragAndDropDetector::DragAndDropDetector()
: mScreenPosition()
{
  mPanGestureDetector = Dali::PanGestureDetector::New();
  mPanGestureDetector.SetMinimumTouchesRequired(1u);
  // Keep the recognizer active long enough for OnSourceTouch() to distinguish
  // a second pointer from a normal one-pointer finish.
  mPanGestureDetector.SetMaximumTouchesRequired(2u);
  mPanGestureDetector.DetectedSignal().Connect(this, &DragAndDropDetector::OnPan);
  mLongPressGestureDetector = Dali::LongPressGestureDetector::New();
  mLongPressGestureDetector.SetTouchesRequired(1u);
  mLongPressGestureDetector.DetectedSignal().Connect(this, &DragAndDropDetector::OnLongPress);
  mDragActivationMode                         = Dali::Ui::DragActivationMode::PAN;
  mPendingActivationMode                      = Dali::Ui::DragActivationMode::PAN;
  mDragStartThreshold                         = 0.0f;
  mPendingDragStartThreshold                  = 0.0f;
  mDragDeviceClass                            = Device::Class::NONE;
  mDragSessionOrigin                          = Dali::Ui::DragSessionOrigin::NONE;
  mPointDown                                  = false;
  mActivationPending                          = false;
  mPendingPayloadResolved                     = false;
  mPanTracking                                = false;
  mFinishing                                  = false;
  mEmittingEnded                              = false;
  mEvaluatingDropProposal                     = false;
  mAutoScrollTicking                          = false;
  mDragPreviewPositioningPropertiesOverridden = false;
  mTargetStatus                               = Dali::Ui::DragAndDropTargetStatus::NONE;
}

DragAndDropDetector::~DragAndDropDetector()
{
  StopAutoScrollTimer();
  for(auto& view : mTargets)
  {
    view.SceneDisconnectedSignal().Disconnect(this, &DragAndDropDetector::OnTargetSceneDisconnected);
  }
  if(mAutoScrollConfiguration.GetViewport())
  {
    mAutoScrollConfiguration.GetViewport().SceneDisconnectedSignal().Disconnect(
      this,
      &DragAndDropDetector::OnAutoScrollViewportSceneDisconnected);
  }
  StopInterruptionMonitoring();
  RemoveDragPreview(mSessionDragPreview);
  RestoreDragPreviewPositioningProperties(mSessionDragPreview);
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
