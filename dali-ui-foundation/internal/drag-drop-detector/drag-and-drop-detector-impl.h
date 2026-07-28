#ifndef DALI_INTERNAL_DRAG_AND_DROP_DETECTOR_H
#define DALI_INTERNAL_DRAG_AND_DROP_DETECTOR_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <algorithm>
#include <utility>
#include <vector>

#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/drag-and-drop/drag-and-drop-detector.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/pan-gesture.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
using DragAndDropDetectorPtr = IntrusivePtr<DragAndDropDetector>;

/**
 * This class listens to Drag & Drop events.
 */
class DragAndDropDetector : public Dali::BaseObject, public ConnectionTracker
{
public:
  using DragEventSignalType             = Dali::Ui::DragAndDropDetector::DragEventSignalType;
  using TargetFeedbackChangedSignalType = Dali::Ui::DragAndDropDetector::TargetFeedbackChangedSignalType;
  using DragPreviewPositionSignalType   = Dali::Ui::DragAndDropDetector::DragPreviewPositionSignalType;
  using DragPreviewFactory              = Dali::Ui::DragAndDropDetector::DragPreviewFactory;
  using DragPreviewUpdater              = Dali::Ui::DragAndDropDetector::DragPreviewUpdater;
  using DragPreviewFinalizer            = Dali::Ui::DragAndDropDetector::DragPreviewFinalizer;
  using DropProposalCallback            = Dali::Ui::DragAndDropDetector::DropProposalCallback;
  using AutoScrollCallback              = Dali::Ui::DragAndDropDetector::AutoScrollCallback;
  using CanStartDragCallback            = Dali::Ui::DragAndDropDetector::CanStartDragCallback;
  using SourcePayloadProvider           = Dali::Ui::DragAndDropDetector::SourcePayloadProvider;

  // Creation

  /**
   * @copydoc Dali::Ui::DragAndDropDetector::New()
   */
  static Dali::Ui::DragAndDropDetector New();

  // Public API

  /**
   * @copydoc Dali::Ui::DragAndDropDetector::GetCurrentScreenPosition() const
   */
  const Vector2& GetCurrentScreenPosition() const;

  void                         AttachSource(Dali::Ui::View& view);
  void                         SetSourcePayload(Dali::Ui::View& source, Dali::Ui::DragPayload payload);
  void                         ClearSourcePayload(Dali::Ui::View& source);
  void                         SetSourcePayloadProvider(Dali::Ui::View& source, SourcePayloadProvider provider);
  void                         ClearSourcePayloadProvider(Dali::Ui::View& source);
  void                         AttachTarget(Dali::Ui::View& view);
  void                         SetDropProposalCallback(Dali::Ui::View& target, DropProposalCallback callback);
  void                         ClearDropProposalCallback(Dali::Ui::View& target);
  void                         SetAutoScrollCallback(Dali::Ui::DragAutoScrollConfiguration configuration,
                                                     AutoScrollCallback                    callback);
  void                         ClearAutoScrollCallback();
  void                         SetDragActivationMode(Dali::Ui::DragActivationMode mode);
  Dali::Ui::DragActivationMode GetDragActivationMode() const;
  void                         SetDragActivationConfiguration(
                            Device::Class::Type                   deviceClass,
                            Dali::Ui::DragActivationConfiguration configuration);
  void                                  ClearDragActivationConfiguration(Device::Class::Type deviceClass);
  bool                                  HasDragActivationConfiguration(Device::Class::Type deviceClass) const;
  Dali::Ui::DragActivationConfiguration GetDragActivationConfiguration(
    Device::Class::Type deviceClass) const;
  void  SetCanStartDragCallback(CanStartDragCallback callback);
  void  ClearCanStartDragCallback();
  void  SetDragStartThreshold(float threshold);
  float GetDragStartThreshold() const;
  bool  IsDragActivationPending() const;

  void DetachSource(Dali::Ui::View& view);
  void DetachTarget(Dali::Ui::View& view);

  /**
   * Detaches all views attached to DragAndDropDetector.
   */
  void                        DetachAll();
  bool                        StartDrag(Dali::Ui::View& source, Device::Class::Type deviceClass);
  bool                        StartDrag(Dali::Ui::View&       source,
                                        Dali::Ui::DragPayload payload,
                                        Device::Class::Type   deviceClass);
  bool                        MoveDragTo(Dali::Ui::View& target);
  bool                        Drop();
  void                        CancelDrag();
  bool                        IsDragging() const;
  Dali::Ui::DragSessionOrigin GetDragSessionOrigin() const;
  Dali::Ui::View              GetDragSource() const;
  Dali::Ui::View              GetDragTarget() const;

  uint32_t GetAttachedSourceCount() const;
  uint32_t GetAttachedTargetCount() const;

  Dali::Ui::View GetAttachedSource(uint32_t index) const;
  Dali::Ui::View GetAttachedTarget(uint32_t index) const;

  void                         SetDragPreview(Dali::Ui::View preview);
  void                         SetDragPreviewContainer(Dali::Ui::View container);
  void                         SetDragPreviewCallbacks(DragPreviewFactory   factory,
                                                       DragPreviewUpdater   updater,
                                                       DragPreviewFinalizer finalizer);
  void                         ClearDragPreviewCallbacks();
  const Dali::Ui::DragPayload& GetDragPayload() const;

  DragPreviewPositionSignalType& DragPreviewPositionSignal()
  {
    return mDragPreviewPositionSignal;
  }

  /**
   * Sets the position the drop occurred.
   */
  void SetPosition(const Vector2& screenPosition);

  /**
   * Called when a draggable object start drag.
   */
  void EmitStartedSignal();

  /**
   * Called when a draggable object enters other object.
   */
  void EmitEnteredSignal(Dali::Ui::View target);

  /**
   * Called when a draggable object leaves other object.
   */
  void EmitExitedSignal(Dali::Ui::View              target,
                        Dali::Ui::DragAndDropResult result = Dali::Ui::DragAndDropResult::NONE,
                        Dali::Ui::DragCancelReason  cancelReason =
                          Dali::Ui::DragCancelReason::NONE);

  /**
   * Called when a draggable object leaves other object.
   */
  void EmitMovedSignal(Dali::Ui::View target);

  /**
   * Is called when a drop actually occurs.
   */
  void EmitDroppedSignal(const Dali::Ui::DragAndDropEvent& event);

  /**
   * Called when a draggable object drag ended.
   */
  void EmitEndedSignal(const Dali::Ui::DragAndDropEvent& event);

public: // Signals
  /**
   * @copydoc Dali::Ui::DragAndDropDetector::StartedSignal
   */
  DragEventSignalType& StartedSignal()
  {
    return mStartedSignal;
  }

  /**
   * @copydoc Dali::Ui::DragAndDropDetector::EnteredSignal
   */
  DragEventSignalType& EnteredSignal()
  {
    return mEnteredSignal;
  }

  /**
   * @copydoc Dali::Ui::DragAndDropDetector::ExitedSignal
   */
  DragEventSignalType& ExitedSignal()
  {
    return mExitedSignal;
  }

  /**
   * @copydoc Dali::Ui::DragAndDropDetector::MovedSignal
   */
  DragEventSignalType& MovedSignal()
  {
    return mMovedSignal;
  }

  /**
   * @copydoc Dali::Ui::DragAndDropDetector::DroppedSignal
   */
  DragEventSignalType& DroppedSignal()
  {
    return mDroppedSignal;
  }

  DragEventSignalType& CancelledSignal()
  {
    return mCancelledSignal;
  }

  TargetFeedbackChangedSignalType& TargetFeedbackChangedSignal()
  {
    return mTargetFeedbackChangedSignal;
  }

  /**
   * @copydoc Dali::Ui::DragAndDropDetector::EndedSignal
   */
  DragEventSignalType& EndedSignal()
  {
    return mEndedSignal;
  }

public:
  void OnPan(Dali::Actor actor, PanGesture gesture);
  void OnLongPress(Dali::Actor actor, LongPressGesture gesture);
  bool OnSourceTouch(Dali::Actor actor, Dali::TouchEvent touch);
  void OnSourceSceneDisconnected(Dali::Actor actor);
  void OnPreviewContainerSceneDisconnected(Dali::Actor actor);
  void OnTargetSceneDisconnected(Dali::Actor actor);
  void OnAutoScrollViewportSceneDisconnected(Dali::Actor actor);
  void OnWindowFocusChanged(Dali::Window window, bool focusIn);

private:
  struct TargetResolution
  {
    Dali::Ui::View                    acceptedTarget;
    Dali::Ui::View                    feedbackTarget;
    Dali::Ui::DropProposal            proposal;
    Dali::Ui::DragAndDropTargetStatus status{Dali::Ui::DragAndDropTargetStatus::NONE};
  };

  // Construction & Destruction

  /**
   * Constructor.
   */
  DragAndDropDetector();

  /**
   * Destructor.
   */
  virtual ~DragAndDropDetector();

  // Undefined
  DragAndDropDetector(const DragAndDropDetector&) = delete;
  DragAndDropDetector& operator=(DragAndDropDetector&);

  void AssertRegistrationChangeAllowed() const;
  void AssertActivationConfigurationChangeAllowed() const;
  void AssertDragConfigurationChangeAllowed() const;
  void AssertSourceAttached(const Dali::Ui::View& source) const;
  void AssertTargetAttached(const Dali::Ui::View& target) const;

  TargetResolution           FindTarget(const Vector2& screenPosition);
  Dali::Ui::View             HitTestTarget(const Vector2&                     screenPosition,
                                           const std::vector<Dali::Ui::View>& excludedTargets) const;
  Dali::Ui::DropProposal     ResolveDropProposal(uint32_t targetIndex);
  Dali::Ui::DropProposal     NormalizeDropProposal(Dali::Ui::DropProposal proposal) const;
  bool                       IsTargetEligible(const Dali::Ui::View& target) const;
  Vector2                    GetViewScreenCenter(const Dali::Ui::View& view) const;
  Dali::Ui::DragPayload      FindSourcePayload(Dali::Ui::View source) const;
  SourcePayloadProvider*     FindSourcePayloadProvider(Dali::Ui::View source);
  Dali::Ui::DragAndDropEvent CreateEvent(Dali::Ui::DragAndDropResult result,
                                         Dali::Ui::View              target = {},
                                         Dali::Ui::DragCancelReason  cancelReason =
                                           Dali::Ui::DragCancelReason::NONE) const;
  Dali::Ui::DragAndDropEvent CreateEvent(Dali::Ui::DragAndDropResult       result,
                                         Dali::Ui::View                    target,
                                         Dali::Ui::DragCancelReason        cancelReason,
                                         Dali::Ui::View                    candidateTarget,
                                         Dali::Ui::DragAndDropTargetStatus targetStatus,
                                         Dali::Ui::DropProposal            proposal) const;
  void                       CreateDragPreview();
  void                       UpdateDragPreview();
  void                       UpdateCoordinateSnapshots(const Vector2& screenPosition);
  void                       ApplyDefaultDragPreviewPosition();
  void                       RestoreDragPreviewPositioningProperties(Dali::Ui::View visual);
  void                       FinalizeDragPreview(Dali::Ui::View visual, const Dali::Ui::DragAndDropEvent& event);
  void                       ResolveTarget(const Vector2& screenPosition, bool emitMoved);
  bool                       UpdateExplicitDrag(Dali::Ui::View target);
  void                       ClearExplicitTarget(Dali::Ui::View target);
  void                       UpdateTargetFeedback(Dali::Ui::View                    target,
                                                  Dali::Ui::DragAndDropTargetStatus status);
  void                       ClearTargetFeedback(Dali::Ui::DragAndDropResult result,
                                                 Dali::Ui::View              finishingTarget,
                                                 Dali::Ui::DragCancelReason  cancelReason);
  void                       UpdateDrag(const Vector2& screenPosition);
  void                       FinishDrag(bool dropped, Dali::Ui::DragCancelReason cancelReason);
  void                       StartInterruptionMonitoring();
  void                       StopInterruptionMonitoring();
  void                       RemoveDragPreview(Dali::Ui::View visual);
  Vector2                    CalculateAutoScrollEdgeIntensity() const;
  void                       UpdateAutoScrollState();
  void                       StartAutoScrollTimer();
  void                       StopAutoScrollTimer();
  bool                       OnAutoScrollTick();
  void                       BeginActivation(Dali::Ui::View                        view,
                                             const Vector2&                        localPosition,
                                             const Vector2&                        screenPosition,
                                             const Vector2&                        originScreenPosition,
                                             Device::Class::Type                   deviceClass,
                                             Dali::Ui::DragActivationConfiguration configuration,
                                             bool                                  panTracking);
  bool                       RequiresLongPressDetector() const;
  void                       UpdateLongPressDetectorAttachments(bool wasRequired);
  bool                       HasReachedDragStartThreshold(const Vector2& screenPosition) const;
  void                       TryActivateDrag(Dali::Ui::View view,
                                             const Vector2& localPosition,
                                             const Vector2& screenPosition);
  void                       ActivateDrag(Dali::Ui::View        view,
                                          const Vector2&        localPosition,
                                          const Vector2&        screenPosition,
                                          Dali::Ui::DragPayload payload);
  void                       ResetPendingActivation();

private:
  DragEventSignalType                   mStartedSignal;
  DragEventSignalType                   mEnteredSignal;
  DragEventSignalType                   mExitedSignal;
  DragEventSignalType                   mMovedSignal;
  DragEventSignalType                   mDroppedSignal;
  DragEventSignalType                   mCancelledSignal;
  DragEventSignalType                   mEndedSignal;
  TargetFeedbackChangedSignalType       mTargetFeedbackChangedSignal;
  DragPreviewPositionSignalType         mDragPreviewPositionSignal;
  DragPreviewFactory                    mDragPreviewFactory;
  DragPreviewUpdater                    mDragPreviewUpdater;
  DragPreviewFinalizer                  mDragPreviewFinalizer;
  AutoScrollCallback                    mAutoScrollCallback;
  CanStartDragCallback                  mCanStartDragCallback;
  Dali::Ui::DragAutoScrollConfiguration mAutoScrollConfiguration;

  std::vector<Dali::Ui::View>                                   mSources;
  std::vector<Dali::Ui::View>                                   mTargets;
  std::vector<std::pair<Dali::Ui::View, Dali::Ui::DragPayload>> mSourcePayloads;
  std::vector<std::pair<Dali::Ui::View, SourcePayloadProvider>> mSourcePayloadProviders;
  std::vector<std::pair<Device::Class::Type, Dali::Ui::DragActivationConfiguration>>
                                    mDragActivationConfigurations;
  std::vector<DropProposalCallback> mDropProposalCallbacks;
  Dali::Ui::View                    mDragView; // the current drag view
  Dali::Ui::View                    mCurrentTarget;
  Dali::Ui::View                    mFeedbackTarget;
  Dali::Ui::View                    mConfiguredDragPreview; // reusable application-provided preview
  Dali::Ui::View                    mDragPreviewContainer;  // optional application-owned preview layer
  Dali::Ui::View                    mSessionDragPreview;    // preview displayed during the active drag
  Dali::PanGestureDetector          mPanGestureDetector;    // pan gesture for drag tracking
  Dali::LongPressGestureDetector    mLongPressGestureDetector;
  Dali::Window                      mSessionWindow; // source window monitored during a drag

  Vector2                      mSourceParentPosition;
  Vector2                      mPreviewLocalPosition;
  Vector2                      mSourceGrabPosition;
  Vector2                      mSourceAnchor;
  Vector2                      mScreenPosition; ///< The screen position of the drop location.
  Vector2                      mActivationOriginScreenPosition;
  Vector3                      mOriginalDragPreviewPivot;
  Vector3                      mOriginalDragPreviewParentOrigin;
  LayoutMode                   mOriginalDragPreviewLayoutMode;
  UiScalePolicy                mOriginalDragPreviewUiScalePolicy;
  float                        mOriginalDragPreviewRequestedX;
  float                        mOriginalDragPreviewRequestedY;
  bool                         mOriginalDragPreviewPositionUsesPivot;
  Dali::Ui::DragPayload        mPendingPayload;
  Dali::Ui::DragPayload        mSessionPayload;
  Dali::Ui::DropProposal       mDropProposal;
  Dali::Timer                  mAutoScrollTimer;
  Vector2                      mAutoScrollEdgeIntensity;
  float                        mDragStartThreshold;
  float                        mPendingDragStartThreshold;
  Dali::Ui::DragActivationMode mDragActivationMode;
  Dali::Ui::DragActivationMode mPendingActivationMode;
  Device::Class::Type          mDragDeviceClass;
  Dali::Ui::DragSessionOrigin  mDragSessionOrigin;

  bool                              mPointDown; // bool flag to indicate if PointState::DOWN have been processed
  bool                              mActivationPending;
  bool                              mPendingPayloadResolved;
  bool                              mPanTracking;
  bool                              mFinishing;
  bool                              mEmittingEnded;
  bool                              mEvaluatingDropProposal;
  bool                              mAutoScrollTicking;
  bool                              mDragPreviewPositioningPropertiesOverridden;
  Dali::Ui::DragAndDropTargetStatus mTargetStatus;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::DragAndDropDetector& GetImplementation(Dali::Ui::DragAndDropDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "DragAndDropDetector handle is empty");

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::DragAndDropDetector&>(handle);
}

inline const Internal::DragAndDropDetector& GetImplementation(const Dali::Ui::DragAndDropDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "DragAndDropDetector handle is empty");

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::DragAndDropDetector&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_INTERNAL_DRAG_AND_DROP_DETECTOR_H
