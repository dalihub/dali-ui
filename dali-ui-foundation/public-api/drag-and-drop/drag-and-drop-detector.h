#ifndef DALI_DRAG_AND_DROP_DETECTOR_H
#define DALI_DRAG_AND_DROP_DETECTOR_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/drag-and-drop/drag-and-drop-types.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class DragAndDropDetector;
}

/**
 * @brief Manages in-scene drag-and-drop sessions between Views.
 *
 * Sources and targets are registered separately. It provides signals for drag
 * start, target enter, move, exit, drop, cancellation, and drag end.
 * Each active session is owned by one pointer. Adding another touch cancels
 * the session instead of converting it into a multi-touch drag.
 *
 * The basic usage is shown below:
 *
 * @code
 *
 *  void Example()
 *  {
 *    DragAndDropDetector detector = DragAndDropDetector::New();
 *    View source = View::New();
 *    View target = View::New();
 *
 *    detector.AttachSource(source);
 *    detector.SetSourcePayload(
 *      source,
 *      {"application/x-example-item", Property::Value(7), DragAndDropOperation::MOVE});
 *    detector.AttachTarget(target);
 *
 *    detector.SetDropProposalCallback(
 *      target,
 *      DragAndDropDetector::DropProposalCallback::New(&ProposeDrop));
 *    detector.StartedSignal().Connect(&OnStarted);
 *    detector.DroppedSignal().Connect(&OnDropped);
 *    detector.EndedSignal().Connect(&OnEnded);
 *  }
 *
 *  DropProposal ProposeDrop(const DragAndDropEvent& event)
 *  {
 *    return event.GetPayload().HasRepresentation("application/x-example-item")
 *             ? DropProposal::Accept("application/x-example-item",
 *                                    DragAndDropOperation::MOVE)
 *             : DropProposal::Reject();
 *  }
 *
 *  void OnStarted(const DragAndDropEvent& event, DragAndDropDetector)
 *  {
 *    std::cout << "Position = " << event.GetScreenPosition() << std::endl;
 *  }
 *
 *  void OnDropped(const DragAndDropEvent& event, DragAndDropDetector)
 *  {
 *    Property::Value data;
 *    if(event.GetSelectedRepresentationData(data))
 *    {
 *      // Apply data using event.GetDropProposal().GetOperation().
 *    }
 *  }
 *
 *  void OnEnded(const DragAndDropEvent& event, DragAndDropDetector)
 *  {
 *    // Restore UI for DROPPED, CANCELLED, or NO_TARGET.
 *  }
 *
 * @endcode
 */
class DALI_UI_API DragAndDropDetector : public BaseHandle
{
public:
  // Typedefs

  using DragEventSignalType             = Signal<void(const DragAndDropEvent&, DragAndDropDetector)>;
  using TargetFeedbackChangedSignalType = DragEventSignalType;
  using DragPreviewPositionSignalType   = Signal<void(Dali::Ui::View, const DragAndDropEvent&)>;
  using DragPreviewFactory              = Callback<Dali::Ui::View(const DragAndDropEvent&)>;
  using DragPreviewUpdater              = Callback<void(Dali::Ui::View, const DragAndDropEvent&)>;
  using DragPreviewFinalizer            = Callback<void(Dali::Ui::View, const DragAndDropEvent&)>;
  using DropProposalCallback            = Callback<DropProposal(const DragAndDropEvent&)>;
  using AutoScrollCallback              = Callback<bool(const DragAutoScrollEvent&)>;
  using CanStartDragCallback            = Callback<bool(const DragActivationEvent&)>;
  using SourcePayloadProvider           = Callback<DragPayload(const DragActivationEvent&)>;

  /**
   * @brief Create an initialized DragAndDropDetector.
   *
   * @return A handle to a newly allocated Dali DragAndDropDetector
   */
  static DragAndDropDetector New();

  /**
   * @brief Create an uninitialized handle.
   *
   * This can be initialized with DragAndDropDetector::New().
   */
  DragAndDropDetector();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DragAndDropDetector();

  /**
   * @brief Registers a View as both a drag source and a drop target.
   *
   * @note You can attach several views to a DragAndDropDetector.
   * DragAndDropDetector will keep a handle to the view and keep it alive as long as
   * DragAndDropDetector is deleted or DetachSourceAndTarget() is called.
   * @pre Must not be called from DropProposalCallback.
   */
  void AttachSourceAndTarget(Dali::Ui::View view);

  /**
   * @brief Registers a View that can start a drag gesture.
   *
   * @pre Must not be called from DropProposalCallback.
   */
  void AttachSource(Dali::Ui::View view);

  /**
   * @brief Sets the payload captured when a registered source starts a drag.
   *
   * Property::Value supports scalar, string, array, and map payloads. Setting
   * the payload during an active drag affects only subsequent sessions.
   * The value is copied when an lvalue is passed. Applications may pass
   * std::move(payload) when the payload is no longer needed, or pass a
   * temporary value, to transfer its hidden implementation without a deep
   * copy.
   *
   * @param[in] source Registered source to configure.
   * @param[in] payload Static payload copied or moved into the detector.
   * @pre The source View must already be registered.
   */
  void SetSourcePayload(Dali::Ui::View source, DragPayload payload);

  /**
   * @brief Clears the payload configured for a registered source.
   *
   * @pre The source View must already be registered.
   */
  void ClearSourcePayload(Dali::Ui::View source);

  /**
   * @brief Sets a callback that creates source payload at gesture activation.
   *
   * The provider is invoked once after the configured gesture and movement
   * threshold are satisfied, and before CanStartDragCallback. Its result is
   * retained while application approval is pending and takes precedence over
   * the static source payload for that gesture session.
   *
   * @pre The source View must already be registered.
   * @pre A drag must not be active or finishing.
   */
  void SetSourcePayloadProvider(Dali::Ui::View source, SourcePayloadProvider provider);

  /**
   * @brief Clears a dynamic source payload provider.
   *
   * Future gesture sessions use the static source payload again.
   *
   * @pre The source View must already be registered.
   * @pre A drag must not be active or finishing.
   */
  void ClearSourcePayloadProvider(Dali::Ui::View source);

  /**
   * @brief Registers a View that can receive drag target signals.
   *
   * When registered targets overlap, the detector follows the source View's
   * scene hit-test order, including render-task, layer, hierarchy, transform,
   * and clipping rules. Targets in another Window are not eligible.
   *
   * A target remains registered while disconnected from the scene. If the
   * active target disconnects, ExitedSignal is emitted immediately and the
   * next eligible connected target under the pointer is entered.
   *
   * @pre Must not be called from DropProposalCallback.
   */
  void AttachTarget(Dali::Ui::View view);

  /**
   * @brief Sets a callback that proposes how a registered target accepts a drag.
   *
   * The callback is invoked only after the pointer geometrically intersects
   * the target. Its event contains the candidate target and the source payload
   * captured at drag start. Use event.GetCandidateTarget() for the target being
   * evaluated; event.GetTarget() remains the previously accepted target, if
   * any. The callback returns a representation type and source-allowed
   * operation. Rejecting or returning an invalid proposal causes target
   * resolution to continue with other intersected targets.
   * Empty values in an accepted proposal select payload defaults. Callbacks
   * are evaluated in scene hit-test order and stop after the first valid
   * accepted target, so callbacks behind it are not invoked.
   *
   * @note An object bound to the callback must remain alive until the callback
   * is cleared, the target is detached, or the detector is destroyed.
   * @note Source or target registration must not be changed from inside the
   * callback. Keep the predicate side-effect free.
   *
   * @pre The target View must already be registered.
   * @pre A drag must not be active or finishing when setting the callback.
   */
  void SetDropProposalCallback(Dali::Ui::View target, DropProposalCallback callback);

  /**
   * @brief Clears a target proposal callback.
   *
   * The target accepts every geometrically eligible drag afterward.
   *
   * @pre The target View must already be registered.
   * @pre A drag must not be active or finishing when clearing the callback.
   */
  void ClearDropProposalCallback(Dali::Ui::View target);

  /**
   * @brief Enables callback-driven auto-scroll near viewport edges.
   *
   * While an active pointer remains inside an edge zone, the callback is
   * invoked periodically even without pointer movement. It should apply
   * event.GetSuggestedDelta() to its ScrollView, RecyclerView, or custom content.
   *
   * Return true when content geometry changed. The detector then resolves the
   * target and updates the drag preview again at the unchanged pointer position.
   * Return false when no scroll was applied, for example at a content boundary;
   * ticking pauses until the next pointer update.
   *
   * A zero threshold or maximum speed disables that axis.
   * If the viewport leaves the scene, ticking stops immediately without
   * cancelling the drag. The configuration is retained; ticking can resume
   * after the viewport reconnects and a new pointer update enters an edge.
   *
   * Clamp the suggested delta to the application's content boundary:
   *
   * @code
   * bool ApplyAutoScroll( const DragAutoScrollEvent& event )
   * {
   *   Vector2 before = scrollView.GetScrollPosition();
   *   Vector2 next(
   *     std::clamp( before.x + event.GetSuggestedDelta().x, 0.0f, maximumScroll.x ),
   *     std::clamp( before.y + event.GetSuggestedDelta().y, 0.0f, maximumScroll.y ) );
   *   if( next == before )
   *   {
   *     return false;
   *   }
   *   scrollView.ScrollTo( next, false );
   *   return true;
   * }
   * @endcode
   *
   * @note An object bound to the callback must remain alive until auto-scroll
   * is cleared or the detector is destroyed.
   *
   * @pre The viewport must be initialized.
   * @pre Thresholds and speeds must be non-negative.
   * @pre The tick interval must be greater than zero.
   * @pre A drag must not be active or finishing.
   */
  void SetAutoScrollCallback(DragAutoScrollConfiguration configuration, AutoScrollCallback callback);

  /**
   * @brief Disables callback-driven drag auto-scroll.
   *
   * @pre A drag must not be active or finishing.
   */
  void ClearAutoScrollCallback();

  /**
   * @brief Sets the built-in gesture prerequisite for drag activation.
   *
   * PAN preserves the default behavior. LONG_PRESS waits for DALi's
   * platform-configured long-press holding time before activation begins.
   * The movement threshold and optional application activation callback are
   * evaluated after this prerequisite.
   *
   * @pre No drag activation may be pending and no drag may be active or finishing.
   */
  void SetDragActivationMode(DragActivationMode mode);

  /**
   * @brief Returns the built-in drag activation mode.
   *
   * @return PAN by default, or the mode most recently configured.
   */
  DragActivationMode GetDragActivationMode() const;

  /**
   * @brief Sets activation settings for one input device class.
   *
   * The device-specific settings override SetDragActivationMode() and
   * SetDragStartThreshold() for future gestures from @p deviceClass. The
   * selected settings are frozen when activation begins and remain unchanged
   * for the pending or active session.
   *
   * @param[in] deviceClass DALi input device class to override.
   * @param[in] configuration Mode and non-negative movement threshold.
   * @pre The device class and activation mode must be valid.
   * @pre No drag activation may be pending and no drag may be active or finishing.
   */
  void SetDragActivationConfiguration(Device::Class::Type         deviceClass,
                                      DragActivationConfiguration configuration);

  /**
   * @brief Removes activation settings for one input device class.
   *
   * Future gestures from the device use the detector's default activation
   * mode and movement threshold.
   *
   * @pre The device class must be valid.
   * @pre No drag activation may be pending and no drag may be active or finishing.
   */
  void ClearDragActivationConfiguration(Device::Class::Type deviceClass);

  /**
   * @brief Returns whether one input device has an explicit override.
   */
  bool HasDragActivationConfiguration(Device::Class::Type deviceClass) const;

  /**
   * @brief Returns the effective activation settings for one input device.
   *
   * The explicit override is returned when present; otherwise the current
   * default activation mode and movement threshold are returned.
   */
  DragActivationConfiguration GetDragActivationConfiguration(Device::Class::Type deviceClass) const;

  /**
   * @brief Sets an optional application approval callback for drag activation.
   *
   * The callback runs only after the configured mode and movement threshold
   * have been satisfied. Returning true creates the drag session with the
   * payload in the event. Returning false keeps activation pending and the
   * callback is evaluated again on a subsequent pan update.
   *
   * The callback may call CancelDrag() or unregister its candidate source.
   * In that case activation is discarded even if the callback returns true.
   *
   * @note An object bound to the callback must remain alive until the callback
   * is cleared or the detector is destroyed.
   * @pre The callback must be valid.
   * @pre No drag activation may be pending and no drag may be active or finishing.
   */
  void SetCanStartDragCallback(CanStartDragCallback callback);

  /**
   * @brief Removes the application approval callback.
   *
   * Built-in activation conditions approve future sessions automatically.
   *
   * @pre No drag activation may be pending and no drag may be active or finishing.
   */
  void ClearCanStartDragCallback();

  /**
   * @brief Sets the additional movement required before activation creates a drag.
   *
   * The distance is measured in screen coordinates from the pointer position
   * where the configured activation mode began. PAN uses the original touch
   * position reconstructed from its first displacement. LONG_PRESS uses the
   * recognized hold position. The detector remains inactive until the
   * cumulative Euclidean distance reaches the threshold.
   *
   * A value of zero applies no additional movement requirement. The underlying
   * DALi recognizers can still apply their platform gesture conditions.
   *
   * @param[in] threshold Non-negative distance in screen pixels.
   * @pre No drag activation may be pending and no drag may be active or finishing.
   */
  void SetDragStartThreshold(float threshold);

  /**
   * @brief Returns the additional movement required to activate a drag.
   *
   * @return The non-negative threshold in screen pixels.
   */
  float GetDragStartThreshold() const;

  /**
   * @brief Returns whether a recognized gesture is waiting for activation.
   *
   * This includes waiting for the movement threshold or application callback
   * approval. It is false before the configured gesture prerequisite and
   * after the drag session has been created.
   */
  bool IsDragActivationPending() const;

  /**
   * @brief Detaches the attached view from the detector.
   *
   * @pre The specified view has been attached as both roles.
   * @pre Must not be called from DropProposalCallback.
   */
  void DetachSourceAndTarget(Dali::Ui::View view);

  /**
   * @brief Unregisters a drag source View.
   *
   * @pre Must not be called from DropProposalCallback.
   */
  void DetachSource(Dali::Ui::View view);

  /**
   * @brief Unregisters a drop target View.
   *
   * If this is the current target of an active drag, ExitedSignal is emitted.
   * A gesture session then resolves the next eligible target at the current
   * pointer position. An explicit session clears the target
   * until the application calls MoveDragTo() again.
   *
   * @pre Must not be called from DropProposalCallback.
   */
  void DetachTarget(Dali::Ui::View view);

  /**
   * @brief Detaches all attached views from the detector.
   *
   * An active drag is cancelled before the views are detached.
   *
   * @pre Must not be called from DropProposalCallback.
   */
  void DetachAll();

  /**
   * @brief Starts an explicit drag session.
   *
   * This input-agnostic command is intended for keyboard, accessibility, and
   * other application controllers. The detector does not move focus or choose
   * a target traversal order. The source must be registered, connected, and
   * eligible. The session starts at the center of the source's transformed
   * screen bounds and bypasses gesture activation settings and callbacks.
   *
   * @param[in] source Registered source used to create the session.
   * @param[in] deviceClass Input class represented by the controller, such as
   * KEYBOARD or USER.
   * @return True if the session remains active after start callbacks.
   * @pre The device class must be valid.
   * @note A new session cannot start from a terminal lifecycle callback.
   * Start it after the callback returns.
   */
  bool StartDrag(Dali::Ui::View      source,
                 Device::Class::Type deviceClass = Device::Class::NONE);

  /**
   * @brief Starts an explicit drag session with an explicit payload.
   *
   * The supplied payload is captured for this session only and does not modify
   * the source's static payload or provider.
   *
   * @param[in] source Registered source used to create the session.
   * @param[in] payload Data and operation captured for this session.
   * @param[in] deviceClass Input class represented by the controller.
   * @return True if the session remains active after start callbacks.
   * @note A new session cannot start from a terminal lifecycle callback.
   * Start it after the callback returns.
   */
  bool StartDrag(Dali::Ui::View      source,
                 DragPayload         payload,
                 Device::Class::Type deviceClass = Device::Class::NONE);

  /**
   * @brief Moves an explicit drag session to a registered target.
   *
   * The application owns target navigation order. The detector positions the
   * session at the center of the target's transformed screen bounds and
   * applies target eligibility, proposal negotiation, enter/exit, feedback,
   * preview, and auto-scroll processing. A rejected target becomes feedback
   * only and is not the current drop target.
   *
   * @return True if @p target is accepted and remains the current target.
   */
  bool MoveDragTo(Dali::Ui::View target);

  /**
   * @brief Finishes an explicit drag session.
   *
   * @return True when the session was dropped on an accepted target. An
   * active session without a target finishes with NO_TARGET and returns false.
   */
  bool Drop();

  /**
   * @brief Cancels pending activation or the active drag session.
   *
   * Pending activation is discarded without emitting drag lifecycle signals,
   * because no session exists yet. An active drag reports
   * REQUESTED through its cancellation event. This is a no-op
   * when neither state exists or terminal callbacks are already running.
   * It is safe to call from Started, Entered, Moved, target feedback, or drag
   * preview callbacks.
   */
  void CancelDrag();

  /**
   * @brief Returns whether a drag session is active or running terminal callbacks.
   *
   * This remains true through Dropped, Cancelled, target feedback cleanup, and
   * the drag preview finalizer. It is false in EndedSignal and outside a session.
   */
  bool IsDragging() const;

  /**
   * @brief Returns how the active or finishing drag session was created.
   *
   * @return GESTURE or EXPLICIT during a session, otherwise NONE.
   */
  DragSessionOrigin GetDragSessionOrigin() const;

  /**
   * @brief Returns the active or finishing drag source.
   *
   * @return The source, or an empty View outside a drag session.
   */
  Dali::Ui::View GetDragSource() const;

  /**
   * @brief Returns the active or finishing accepted target.
   *
   * @return The current or finishing target, or an empty View.
   */
  Dali::Ui::View GetDragTarget() const;

  /** @brief Returns the number of registered drag sources. */
  uint32_t GetAttachedSourceCount() const;

  /** @brief Returns the number of registered drop targets. */
  uint32_t GetAttachedTargetCount() const;

  /**
   * @brief Returns a registered drag source by index.
   *
   * Sources are indexed in registration order. Registering the same source
   * again does not change its index.
   *
   * @return The source, or an empty View when the index is out of range.
   */
  Dali::Ui::View GetAttachedSource(uint32_t index) const;

  /**
   * @brief Returns a registered drop target by index.
   *
   * Targets are indexed in registration order. Registering the same target
   * again does not change its index. Scene disconnection does not remove a
   * target registration.
   *
   * @return The target, or an empty View when the index is out of range.
   */
  Dali::Ui::View GetAttachedTarget(uint32_t index) const;

  /**
   * @brief Sets an optional application-provided View used as the drag preview.
   *
   * The View must not have a parent. The detector adds it to the source
   * View's parent for the active drag and removes it when the drag ends.
   * Pass an empty View to disable the drag preview.
   *
   * This may be called from Dropped, Cancelled, or Ended callbacks to configure
   * the reusable preview for a subsequent session.
   *
   * @pre A drag must not be processing pointer or explicit movement updates.
   */
  void SetDragPreview(Dali::Ui::View preview);

  /**
   * @brief Sets the application-owned container used to display drag previews.
   *
   * When set, the detector adds each active drag preview to this container
   * instead of the source View's parent. This is useful for a scene-level
   * overlay that must not be clipped by a ScrollView or another source
   * ancestor. Pass an empty View to restore source-parent placement.
   *
   * The application remains responsible for positioning a custom preview
   * through DragPreviewUpdater or DragPreviewPositionSignal. Their event exposes
   * the pointer in the selected container as previewLocalPosition and the
   * normalized source grab point as sourceAnchor. The default updater applies
   * those values automatically.
   *
   * A custom updater can preserve the original grab point across unrelated
   * source and overlay transforms:
   *
   * @code
   * void PositionPreview( View preview, const DragAndDropEvent& event )
   * {
   *   preview.SetProperty(
   *     Actor::Property::PIVOT,
   *     Vector3( event.GetSourceAnchor().x, event.GetSourceAnchor().y, 0.5f ) );
   *   preview.SetRequestedX( event.GetPreviewLocalPosition().x );
   *   preview.SetRequestedY( event.GetPreviewLocalPosition().y );
   * }
   *
   * detector.SetDragPreviewContainer( sceneOverlay );
   * detector.SetDragPreviewCallbacks( factory, updater, finalizer );
   * @endcode
   *
   * @pre A drag must not be active or finishing.
   */
  void SetDragPreviewContainer(Dali::Ui::View container);

  /**
   * @brief Sets callbacks that create and manage a preview for each drag session.
   *
   * The factory is invoked once at drag start and should return a detached
   * View, or an empty View for a preview-less session. The detector adds a
   * detached View to the configured drag preview container, or to the source
   * parent when no container is configured, and removes it before invoking
   * the finalizer. An empty or already-parented factory result is ignored and
   * does not invoke the updater or finalizer.
   *
   * The optional updater is invoked after target resolution on every pan
   * update. If it is empty, DragPreviewPositionSignal or the default
   * pointer-offset positioning is used.
   *
   * The optional finalizer receives the detached preview and a snapshot whose
   * result is DROPPED, CANCELLED, or NO_TARGET. It may reparent the preview to
   * run an application-owned finish animation.
   *
   * These callbacks take precedence over the reusable View configured by
   * SetDragPreview().
   *
   * @note An object bound to a callback must remain alive until the callbacks
   * are cleared or the detector is destroyed.
   *
   * @pre A drag must not be active or finishing.
   */
  void SetDragPreviewCallbacks(DragPreviewFactory   factory,
                               DragPreviewUpdater   updater   = {},
                               DragPreviewFinalizer finalizer = {});

  /**
   * @brief Clears the per-session drag preview callbacks.
   *
   * The reusable preview configured with SetDragPreview(), if any, becomes
   * active again for subsequent drags.
   *
   * @pre A drag must not be active or finishing.
   */
  void ClearDragPreviewCallbacks();

  /**
   * @brief Emitted while dragging when an application-provided preview is set.
   *
   * Connect this signal to take complete ownership of preview positioning.
   * event.GetPreviewLocalPosition() is the pointer in the coordinate space of the
   * configured drag preview container, or the source View's parent when no
   * container is configured. event.GetSourceAnchor() is the normalized source grab
   * point. A layout-safe implementation can use STANDALONE layout with
   * UiScalePolicy::DISABLED, set PARENT_ORIGIN to TOP_LEFT and PIVOT to
   * sourceAnchor, enable POSITION_USES_PIVOT, then set the requested X/Y
   * position to previewLocalPosition.
   *
   * If no callback is connected, the detector applies that policy itself. It
   * temporarily changes the preview's layout mode, UiScale policy, requested
   * position, PARENT_ORIGIN, PIVOT, and POSITION_USES_PIVOT, then restores
   * them before the finalizer is invoked.
   */
  DragPreviewPositionSignalType& DragPreviewPositionSignal();

  /**
   * @brief Returns the payload captured for the active or finishing drag.
   *
   * The value remains available through Started, target, Dropped, Cancelled,
   * and preview finalizer callbacks. EndedSignal receives its terminal payload
   * in DragAndDropEvent after detector query state has been cleared. This
   * method returns an empty payload outside an active or finishing session.
   */
  const DragPayload& GetDragPayload() const;

  /**
   * @brief Returns the current position of the dragged object.
   *
   * This is the dropped position when an object is dropped.
   * @return The current screen position.
   */
  const Vector2& GetCurrentScreenPosition() const;

  // Signals
  /**
   * @brief Emitted when a drag session starts.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( const DragAndDropEvent& event,
   *                      DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragEventSignalType& StartedSignal();

  /**
   * @brief Emitted when the drag enters an accepted target.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( const DragAndDropEvent& event,
   *                      DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragEventSignalType& EnteredSignal();

  /**
   * @brief Emitted when the drag leaves its accepted target.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( const DragAndDropEvent& event,
   *                      DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragEventSignalType& ExitedSignal();

  /**
   * @brief Emitted when an active drag moves or target geometry changes.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( const DragAndDropEvent& event,
   *                      DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragEventSignalType& MovedSignal();

  /**
   * @brief Emitted when a drag is dropped on an accepted target.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( const DragAndDropEvent& event,
   *                      DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragEventSignalType& DroppedSignal();

  /**
   * @brief Emitted when an active drag is cancelled without a drop.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( const DragAndDropEvent& event,
   *                      DragAndDropDetector detector );
   * @endcode
   *
   * The event identifies the active source, finishing target, payload, and
   * cancellation reason. A second touch interrupts the one-pointer drag and
   * reports GESTURE_INTERRUPTED. Source or preview-container scene
   * disconnection, window focus loss, and registration removal are reported
   * separately.
   *
   * ExitedSignal is emitted first when a target is active, and EndedSignal is
   * emitted after this signal.
   */
  DragEventSignalType& CancelledSignal();

  /**
   * @brief Emitted when the effective drag target feedback changes.
   *
   * The event status is ACCEPTED when candidateTarget is the current eligible
   * drop target, REJECTED when registered targets are intersected but none
   * accepts the drag, and NONE when no registered target is intersected.
   *
   * When several registered targets overlap, ACCEPTED describes the actual
   * target that would receive a drop. REJECTED is reported only when no
   * intersected target accepts the drag. The signal emits a final NONE event
   * when a session finishes so application feedback can be reset.
   *
   * @return The target feedback changed signal.
   */
  TargetFeedbackChangedSignalType& TargetFeedbackChangedSignal();

  /**
   * @brief Emitted once after every active drag session finishes.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( const DragAndDropEvent& event,
   *                      DragAndDropDetector detector );
   * @endcode
   *
   * Detector query state has been cleared and IsDragging() is false while
   * this signal is emitted. Use the event argument for terminal data. Starting
   * another session is rejected until the callback returns.
   * @return The signal to connect to.
   */
  DragEventSignalType& EndedSignal();

public: // Not intended for application developers
  /**
   * @brief This constructor is used by DragAndDropDetector::Get().
   *
   * @param[in] detector A pointer to the drag and drop detector.
   */
  explicit DALI_INTERNAL DragAndDropDetector(Dali::Ui::Internal::DragAndDropDetector* detector);
};

} // namespace Ui

} // namespace Dali

#endif // DALI_DRAG_AND_DROP_DETECTOR_H
