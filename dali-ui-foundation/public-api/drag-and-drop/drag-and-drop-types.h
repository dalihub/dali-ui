#ifndef DALI_DRAG_AND_DROP_TYPES_H
#define DALI_DRAG_AND_DROP_TYPES_H

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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/events/device.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/property-value.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class DragAndDropDetector;
}

/**
 * @brief Result associated with a drag-and-drop event snapshot.
 * @SINCE_3_0.0
 */
enum class DragAndDropResult : uint8_t
{
  NONE,      ///< The drag session is still active. @SINCE_3_0.0
  DROPPED,   ///< The drag ended on an eligible target. @SINCE_3_0.0
  CANCELLED, ///< The drag was interrupted or cancelled by registration removal. @SINCE_3_0.0
  NO_TARGET  ///< The drag finished without an eligible target. @SINCE_3_0.0
};

/**
 * @brief Origin that created a drag-and-drop session.
 * @SINCE_3_0.0
 */
enum class DragSessionOrigin : uint8_t
{
  NONE,    ///< There is no drag session. @SINCE_3_0.0
  GESTURE, ///< A pointer gesture created the session. @SINCE_3_0.0
  EXPLICIT ///< StartDrag() explicitly created the session. @SINCE_3_0.0
};

/**
 * @brief Reason why an active drag-and-drop session was cancelled.
 * @SINCE_3_0.0
 */
enum class DragCancelReason : uint8_t
{
  NONE,                           ///< The event does not represent a cancellation. @SINCE_3_0.0
  GESTURE_INTERRUPTED,            ///< The owning one-pointer pan was interrupted, including by another touch. @SINCE_3_0.0
  SOURCE_DISCONNECTED,            ///< The active source left its scene. @SINCE_3_0.0
  PREVIEW_CONTAINER_DISCONNECTED, ///< The configured drag preview container left its scene. @SINCE_3_0.0
  WINDOW_FOCUS_LOST,              ///< The source window lost input focus. @SINCE_3_0.0
  REGISTRATION_REMOVED,           ///< The active source or all registrations were removed. @SINCE_3_0.0
  REQUESTED                       ///< CancelDrag() was explicitly requested. @SINCE_3_0.0
};

/**
 * @brief Acceptance status of the target represented by a drag feedback event.
 * @SINCE_3_0.0
 */
enum class DragAndDropTargetStatus : uint8_t
{
  NONE,     ///< There is no active target feedback. @SINCE_3_0.0
  ACCEPTED, ///< The candidate is eligible and is the current drop target. @SINCE_3_0.0
  REJECTED  ///< Registered targets are intersected, but none accepts the drag. @SINCE_3_0.0
};

/**
 * @brief Built-in gesture prerequisite used to begin drag activation.
 * @SINCE_3_0.0
 */
enum class DragActivationMode : uint8_t
{
  PAN,       ///< A recognized pan begins activation. @SINCE_3_0.0
  LONG_PRESS ///< A platform-configured long press begins activation. @SINCE_3_0.0
};

/**
 * @brief Semantic operation supported or selected by a drag-and-drop session.
 * @SINCE_3_0.0
 */
enum class DragAndDropOperation : uint8_t
{
  NONE, ///< The application did not specify a transfer operation. @SINCE_3_0.0
  COPY, ///< Copy the represented data to the target. @SINCE_3_0.0
  MOVE, ///< Move the represented data to the target. @SINCE_3_0.0
  LINK  ///< Create a reference to the represented data. @SINCE_3_0.0
};

/**
 * @brief Typed application data captured for one drag session.
 *
 * A payload can expose the same logical item through several representations.
 * Representation order expresses source preference. Type strings are
 * application-defined format identifiers; MIME-style strings are recommended,
 * but this same-process API does not serialize Property::Value.
 * @SINCE_3_0.0
 */
class DALI_UI_API DragPayload
{
public:
  /**
   * @brief Creates an empty payload.
   * @SINCE_3_0.0
   */
  DragPayload();

  /**
   * @brief Creates a payload with one representation and a preferred operation.
   *
   * A non-NONE preferred operation is also added to the allowed operation list.
   * @SINCE_3_0.0
   */
  DragPayload(Dali::String         type,
              Property::Value      data,
              DragAndDropOperation operation = DragAndDropOperation::NONE);

  /**
   * @brief Creates an independent copy of a payload.
   * @SINCE_3_0.0
   */
  DragPayload(const DragPayload& rhs);

  /**
   * @brief Moves a payload.
   * @SINCE_3_0.0
   */
  DragPayload(DragPayload&& rhs) noexcept;

  /**
   * @brief Replaces this payload with an independent copy.
   * @SINCE_3_0.0
   */
  DragPayload& operator=(const DragPayload& rhs);

  /**
   * @brief Moves another payload into this object.
   * @SINCE_3_0.0
   */
  DragPayload& operator=(DragPayload&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~DragPayload();

  /**
   * @brief Adds or replaces an application data representation.
   *
   * Replacing an existing type preserves its preference order.
   *
   * @SINCE_3_0.0
   * @param[in] type Non-empty application-defined format identifier
   * @param[in] data Same-process data for the representation
   */
  void AddRepresentation(Dali::String type, Property::Value data);

  /**
   * @brief Removes a representation by type.
   *
   * @SINCE_3_0.0
   * @return True if a representation was removed
   */
  bool RemoveRepresentation(const Dali::String& type);

  /**
   * @brief Removes every representation.
   * @SINCE_3_0.0
   */
  void ClearRepresentations();

  /**
   * @brief Returns the number of representations in preference order.
   * @SINCE_3_0.0
   */
  uint32_t GetRepresentationCount() const;

  /**
   * @brief Returns a representation type by preference index.
   *
   * @SINCE_3_0.0
   * @pre @p index must be less than GetRepresentationCount().
   */
  const Dali::String& GetRepresentationType(uint32_t index) const;

  /**
   * @brief Returns representation data by preference index.
   *
   * @SINCE_3_0.0
   * @pre @p index must be less than GetRepresentationCount().
   */
  const Property::Value& GetRepresentationData(uint32_t index) const;

  /**
   * @brief Tests whether a representation type is available.
   * @SINCE_3_0.0
   */
  bool HasRepresentation(const Dali::String& type) const;

  /**
   * @brief Copies data for a representation type.
   *
   * @SINCE_3_0.0
   * @param[in] type Application-defined format identifier
   * @param[out] data Receives the representation when found
   * @return True if the representation was found. The output is unchanged
   * when false is returned.
   */
  bool GetRepresentationData(const Dali::String& type, Property::Value& data) const;

  /**
   * @brief Adds a source-supported operation.
   *
   * NONE is not added because it represents the absence of a semantic
   * operation. Repeated additions are ignored, and enumeration APIs preserve
   * insertion order. Use SetPreferredOperation() to select the default; the
   * first enumerated operation is only the fallback when no valid preference
   * is set.
   * @SINCE_3_0.0
   */
  void AddAllowedOperation(DragAndDropOperation operation);

  /**
   * @brief Removes a source-supported operation.
   *
   * Removing the preferred operation selects the first remaining operation as
   * the new preference.
   *
   * @SINCE_3_0.0
   * @return True if an operation was removed
   */
  bool RemoveAllowedOperation(DragAndDropOperation operation);

  /**
   * @brief Clears every allowed operation and resets the preference to NONE.
   * @SINCE_3_0.0
   */
  void ClearAllowedOperations();

  /**
   * @brief Returns the number of allowed operations.
   * @SINCE_3_0.0
   */
  uint32_t GetAllowedOperationCount() const;

  /**
   * @brief Returns an allowed operation by insertion index.
   *
   * @SINCE_3_0.0
   * @pre @p index must be less than GetAllowedOperationCount().
   */
  DragAndDropOperation GetAllowedOperation(uint32_t index) const;

  /**
   * @brief Tests whether the source allows an operation.
   * @SINCE_3_0.0
   */
  bool IsOperationAllowed(DragAndDropOperation operation) const;

  /**
   * @brief Sets the preferred operation.
   *
   * A non-NONE operation is automatically added to the allowed list.
   * NONE clears only the preference.
   * @SINCE_3_0.0
   */
  void SetPreferredOperation(DragAndDropOperation operation);

  /**
   * @brief Returns the source-preferred operation, or NONE.
   * @SINCE_3_0.0
   */
  DragAndDropOperation GetPreferredOperation() const;

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief A target's decision for a candidate drag payload.
 *
 * An accepted proposal selects one payload representation and one source-
 * allowed operation. Empty selection values request detector defaults and are
 * resolved before accepted lifecycle signals are emitted.
 * @SINCE_3_0.0
 */
class DALI_UI_API DropProposal
{
public:
  /**
   * @brief Creates a rejected proposal.
   * @SINCE_3_0.0
   */
  DropProposal();

  /**
   * @brief Creates an independent copy of a proposal.
   * @SINCE_3_0.0
   */
  DropProposal(const DropProposal& rhs);

  /**
   * @brief Moves a proposal.
   * @SINCE_3_0.0
   */
  DropProposal(DropProposal&& rhs) noexcept;

  /**
   * @brief Replaces this proposal with an independent copy.
   * @SINCE_3_0.0
   */
  DropProposal& operator=(const DropProposal& rhs);

  /**
   * @brief Moves another proposal into this object.
   * @SINCE_3_0.0
   */
  DropProposal& operator=(DropProposal&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~DropProposal();

  /**
   * @brief Creates a rejected proposal.
   * @SINCE_3_0.0
   */
  static DropProposal Reject();

  /**
   * @brief Creates an accepted proposal.
   *
   * An empty type requests the first payload representation. NONE requests the
   * payload's preferred operation, or its first allowed operation.
   * @SINCE_3_0.0
   */
  static DropProposal Accept(
    Dali::String         selectedRepresentationType = {},
    DragAndDropOperation operation                  = DragAndDropOperation::NONE);

  /**
   * @brief Returns whether the target accepted the candidate.
   * @SINCE_3_0.0
   */
  bool IsAccepted() const;

  /**
   * @brief Returns the selected representation type.
   *
   * This is concrete in accepted lifecycle events and can be empty only when
   * the payload has no representations.
   * @SINCE_3_0.0
   */
  const Dali::String& GetSelectedRepresentationType() const;

  /**
   * @brief Returns the selected operation.
   *
   * This can be NONE only when the payload has no allowed operations.
   * @SINCE_3_0.0
   */
  DragAndDropOperation GetOperation() const;

private:
  DALI_INTERNAL DropProposal(bool                 accepted,
                             Dali::String         selectedRepresentationType,
                             DragAndDropOperation operation);

  friend class Internal::DragAndDropDetector;

  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Device-specific drag activation settings.
 *
 * A configuration registered for a Device::Class overrides the detector's
 * default activation mode and movement threshold for that input device.
 * @SINCE_3_0.0
 */
class DALI_UI_API DragActivationConfiguration
{
public:
  /**
   * @brief Creates the default PAN configuration with no additional threshold.
   * @SINCE_3_0.0
   */
  DragActivationConfiguration();

  /**
   * @brief Creates a gesture activation configuration.
   * @SINCE_3_0.0
   */
  DragActivationConfiguration(DragActivationMode mode, float movementThreshold = 0.0f);

  /**
   * @brief Creates an independent copy of a configuration.
   * @SINCE_3_0.0
   */
  DragActivationConfiguration(const DragActivationConfiguration& rhs);

  /**
   * @brief Moves a configuration.
   * @SINCE_3_0.0
   */
  DragActivationConfiguration(DragActivationConfiguration&& rhs) noexcept;

  /**
   * @brief Replaces this configuration with an independent copy.
   * @SINCE_3_0.0
   */
  DragActivationConfiguration& operator=(const DragActivationConfiguration& rhs);

  /**
   * @brief Moves another configuration into this object.
   * @SINCE_3_0.0
   */
  DragActivationConfiguration& operator=(DragActivationConfiguration&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~DragActivationConfiguration();

  /**
   * @brief Returns the gesture prerequisite.
   * @SINCE_3_0.0
   */
  DragActivationMode GetMode() const;

  /**
   * @brief Returns the additional screen-space movement threshold in pixels.
   * @SINCE_3_0.0
   */
  float GetMovementThreshold() const;

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Snapshot passed to a drag activation callback.
 *
 * The built-in activation mode and movement threshold have already been
 * satisfied when this event is emitted.
 * @SINCE_3_0.0
 */
class DALI_UI_API DragActivationEvent
{
public:
  /**
   * @brief Creates an empty activation snapshot.
   * @SINCE_3_0.0
   */
  DragActivationEvent();

  /**
   * @brief Creates an independent copy of an activation snapshot.
   * @SINCE_3_0.0
   */
  DragActivationEvent(const DragActivationEvent& rhs);

  /**
   * @brief Moves an activation snapshot.
   * @SINCE_3_0.0
   */
  DragActivationEvent(DragActivationEvent&& rhs) noexcept;

  /**
   * @brief Replaces this snapshot with an independent copy.
   * @SINCE_3_0.0
   */
  DragActivationEvent& operator=(const DragActivationEvent& rhs);

  /**
   * @brief Moves another snapshot into this object.
   * @SINCE_3_0.0
   */
  DragActivationEvent& operator=(DragActivationEvent&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~DragActivationEvent();

  /**
   * @brief Returns the candidate drag source.
   * @SINCE_3_0.0
   */
  Dali::Ui::View GetSource() const;

  /**
   * @brief Returns the built-in activation mode.
   * @SINCE_3_0.0
   */
  DragActivationMode GetMode() const;

  /**
   * @brief Returns the input device class.
   * @SINCE_3_0.0
   */
  Device::Class::Type GetDeviceClass() const;

  /**
   * @brief Returns the activation origin in screen coordinates.
   * @SINCE_3_0.0
   */
  const Vector2& GetInitialScreenPosition() const;

  /**
   * @brief Returns the current pointer position in screen coordinates.
   * @SINCE_3_0.0
   */
  const Vector2& GetScreenPosition() const;

  /**
   * @brief Returns the current pointer position in source coordinates.
   * @SINCE_3_0.0
   */
  const Vector2& GetSourceLocalPosition() const;

  /**
   * @brief Returns the current position minus the activation origin.
   * @SINCE_3_0.0
   */
  const Vector2& GetScreenDisplacement() const;

  /**
   * @brief Returns the payload proposed for the drag session.
   * @SINCE_3_0.0
   */
  const DragPayload& GetPayload() const;

private:
  DALI_INTERNAL DragActivationEvent(Dali::Ui::View      source,
                                    DragActivationMode  mode,
                                    Device::Class::Type deviceClass,
                                    Vector2             initialScreenPosition,
                                    Vector2             screenPosition,
                                    Vector2             sourceLocalPosition,
                                    Vector2             screenDisplacement,
                                    DragPayload         payload);

  friend class Internal::DragAndDropDetector;

  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Immutable snapshot passed to drag lifecycle and preview callbacks.
 *
 * Callback APIs receive this value through a const reference. All coordinates
 * describe the same drag update or finish operation.
 * @SINCE_3_0.0
 */
class DALI_UI_API DragAndDropEvent
{
public:
  /**
   * @brief Creates an empty drag-and-drop snapshot.
   * @SINCE_3_0.0
   */
  DragAndDropEvent();

  /**
   * @brief Creates an independent copy of a drag-and-drop snapshot.
   * @SINCE_3_0.0
   */
  DragAndDropEvent(const DragAndDropEvent& rhs);

  /**
   * @brief Moves a drag-and-drop snapshot.
   * @SINCE_3_0.0
   */
  DragAndDropEvent(DragAndDropEvent&& rhs) noexcept;

  /**
   * @brief Replaces this snapshot with an independent copy.
   * @SINCE_3_0.0
   */
  DragAndDropEvent& operator=(const DragAndDropEvent& rhs);

  /**
   * @brief Moves another snapshot into this object.
   * @SINCE_3_0.0
   */
  DragAndDropEvent& operator=(DragAndDropEvent&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~DragAndDropEvent();

  /**
   * @brief Returns the source that started the drag.
   * @SINCE_3_0.0
   */
  Dali::Ui::View GetSource() const;

  /**
   * @brief Returns the accepted current or finishing target, or an empty handle.
   *
   * During DropProposalCallback this is the target accepted before the current
   * candidate evaluation, if any. Use GetCandidateTarget() for the target being
   * evaluated.
   * @SINCE_3_0.0
   */
  Dali::Ui::View GetTarget() const;

  /**
   * @brief Returns the target being evaluated or represented by feedback.
   *
   * DropProposalCallback should use this getter to identify its candidate.
   * @SINCE_3_0.0
   */
  Dali::Ui::View GetCandidateTarget() const;

  /**
   * @brief Returns the input device class that owns the session.
   * @SINCE_3_0.0
   */
  Device::Class::Type GetDeviceClass() const;

  /**
   * @brief Returns how the drag session was created.
   * @SINCE_3_0.0
   */
  DragSessionOrigin GetSessionOrigin() const;

  /**
   * @brief Returns the current pointer position in screen coordinates.
   * @SINCE_3_0.0
   */
  const Vector2& GetScreenPosition() const;

  /**
   * @brief Returns the drag-start pointer offset in source coordinates.
   * @SINCE_3_0.0
   */
  const Vector2& GetSourceGrabPosition() const;

  /**
   * @brief Returns the pointer position in source-parent coordinates.
   * @SINCE_3_0.0
   */
  const Vector2& GetSourceParentPosition() const;

  /**
   * @brief Returns the pointer position in target or candidate coordinates.
   * @SINCE_3_0.0
   */
  const Vector2& GetTargetLocalPosition() const;

  /**
   * @brief Returns the pointer position in preview-parent coordinates.
   *
   * The position is relative to the top-left corner of the configured drag preview
   * container, or of the source View's parent when no container is configured, in
   * that parent's local actor coordinates -- the units Actor::ScreenToLocal() reports.
   * Laid-out geometry in those units already includes the effective scale, so use
   * GetPreviewRequestedX() and GetPreviewRequestedY() to position the preview
   * with View::SetRequestedX() and View::SetRequestedY().
   * @SINCE_3_0.0
   */
  const Vector2& GetPreviewLocalPosition() const;

  /**
   * @brief Returns the normalized drag-start point within the source.
   * @SINCE_3_0.0
   */
  const Vector2& GetSourceAnchor() const;

  /**
   * @brief Returns the pointer X position in the drag preview's requested units.
   *
   * This is the x of GetPreviewLocalPosition() divided by the effective scale of
   * the session's drag preview -- its window scale times the UI-scale term its
   * UiScalePolicy selects, both 1.0 while Extension::LayoutScaleManager::SetScalable(false) is in
   * force -- read when this snapshot was taken. The layout multiplies the requested
   * position of a STANDALONE View by that View's own effective scale, so a
   * STANDALONE preview whose PARENT_ORIGIN is TOP_LEFT, whose POSITION_USES_PIVOT is
   * enabled and whose PIVOT is GetSourceAnchor() puts its grab point under the
   * pointer when it passes this value and GetPreviewRequestedY() to
   * View::SetRequestedX() and View::SetRequestedY(), whatever UiScalePolicy the
   * preview or its container uses. A start margin on the preview offsets the grab
   * point by the margin times that scale.
   *
   * The detector adds the preview to its container before it creates the first
   * snapshot that has a preview, so the updater and DragPreviewPositionSignal
   * receive the value converted with the scale the next layout pass uses. If the
   * effective scale changes during the drag, the next drag update carries the newly converted
   * value. While the session has no preview -- in the DragPreviewFactory event,
   * which is created before the preview exists, and in every event of a session
   * without one -- there is nothing to convert for, and the value equals the x of
   * GetPreviewLocalPosition().
   *
   * @SINCE_3_0.0
   * @return The pointer X position to pass to View::SetRequestedX() of the drag
   *         preview
   */
  float GetPreviewRequestedX() const;

  /**
   * @brief Returns the pointer Y position in the drag preview's requested units.
   *
   * This is the y of GetPreviewLocalPosition() converted exactly as
   * GetPreviewRequestedX() describes, with the same timing and the same
   * passthrough while the session has no preview. A top margin on the preview
   * offsets the grab point by the margin times the preview's effective scale.
   *
   * @SINCE_3_0.0
   * @return The pointer Y position to pass to View::SetRequestedY() of the drag
   *         preview
   */
  float GetPreviewRequestedY() const;

  /**
   * @brief Returns the source's laid-out width in requested units.
   *
   * This is the source's width when the drag started, as last rendered, divided by
   * the effective scale it was rendered at: the width to pass to
   * View::SetRequestedWidth() of a preview that copies the source. Like any View,
   * such a preview is laid out at its own effective scale, children included, so it
   * appears as large as the source when both resolve the same scale -- for example
   * a preview that inherits its scale from an ordinary container in the source's
   * window. If the source sits in a subtree that opts out of the system scale, give
   * the preview the same Extension::UiScalePolicy to match it
   * (Extension::View::SetUiScalePolicy()).
   *
   * The value is captured once when the drag starts and is the same in every event
   * of the session, the DragPreviewFactory event included. Unlike the source's own
   * requested width it is never WRAP_CONTENT or MATCH_PARENT.
   *
   * @SINCE_3_0.0
   * @return The source's drag-start width in requested units, or zero for an empty
   *         snapshot or a source that has not been rendered yet
   */
  float GetPreviewRequestedWidth() const;

  /**
   * @brief Returns the source's laid-out height in requested units.
   *
   * This is the source's height when the drag started, as last rendered, divided
   * by the effective scale it was rendered at, exactly as
   * GetPreviewRequestedWidth() describes: the height to pass to
   * View::SetRequestedHeight() of a preview that copies the source. It is captured
   * once when the drag starts and is never WRAP_CONTENT or MATCH_PARENT.
   *
   * @SINCE_3_0.0
   * @return The source's drag-start height in requested units, or zero for an empty
   *         snapshot or a source that has not been rendered yet
   */
  float GetPreviewRequestedHeight() const;

  /**
   * @brief Returns the payload captured when the drag started.
   * @SINCE_3_0.0
   */
  const DragPayload& GetPayload() const;

  /**
   * @brief Returns the target's concrete selection, or a rejected proposal.
   *
   * An accepted proposal describes target eligibility and does not by itself
   * mean a drop occurred. Check GetResult() or handle DroppedSignal before
   * mutating application data.
   * @SINCE_3_0.0
   */
  const DropProposal& GetDropProposal() const;

  /**
   * @brief Copies the data selected by the accepted drop proposal.
   *
   * @SINCE_3_0.0
   * @param[out] data Receives the selected representation
   * @return True if the proposal is accepted and its selected representation
   * exists in the payload. The output is unchanged when false is returned.
   */
  bool GetSelectedRepresentationData(Property::Value& data) const;

  /**
   * @brief Returns the current target feedback status.
   * @SINCE_3_0.0
   */
  DragAndDropTargetStatus GetTargetStatus() const;

  /**
   * @brief Returns the current or terminal session result.
   * @SINCE_3_0.0
   */
  DragAndDropResult GetResult() const;

  /**
   * @brief Returns the cancellation reason, or NONE.
   * @SINCE_3_0.0
   */
  DragCancelReason GetCancelReason() const;

private:
  DALI_INTERNAL DragAndDropEvent(Dali::Ui::View          source,
                                 Dali::Ui::View          target,
                                 Dali::Ui::View          candidateTarget,
                                 Device::Class::Type     deviceClass,
                                 DragSessionOrigin       sessionOrigin,
                                 Vector2                 screenPosition,
                                 Vector2                 sourceGrabPosition,
                                 Vector2                 sourceParentPosition,
                                 Vector2                 targetLocalPosition,
                                 Vector2                 previewLocalPosition,
                                 Vector2                 sourceAnchor,
                                 DragPayload             payload,
                                 DropProposal            dropProposal,
                                 DragAndDropTargetStatus targetStatus,
                                 DragAndDropResult       result,
                                 DragCancelReason        cancelReason,
                                 Vector2                 previewRequestedPosition,
                                 Vector2                 previewRequestedSize);

  friend class Internal::DragAndDropDetector;

  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Configuration for callback-driven drag auto-scroll.
 *
 * The detector only calculates edge proximity and a suggested content delta.
 * The application callback owns the actual scrolling operation.
 * @SINCE_3_0.0
 */
class DALI_UI_API DragAutoScrollConfiguration
{
public:
  /**
   * @brief Creates an empty configuration with default edge and timing values.
   * @SINCE_3_0.0
   */
  DragAutoScrollConfiguration();

  /**
   * @brief Creates an auto-scroll configuration for a viewport.
   * @SINCE_3_0.0
   */
  DragAutoScrollConfiguration(Dali::Ui::View viewport,
                              Vector2        edgeThreshold            = Vector2(48.0f, 48.0f),
                              Vector2        maxSpeed                 = Vector2(600.0f, 600.0f),
                              uint32_t       tickIntervalMilliseconds = 16u);

  /**
   * @brief Creates an independent copy of a configuration.
   * @SINCE_3_0.0
   */
  DragAutoScrollConfiguration(const DragAutoScrollConfiguration& rhs);

  /**
   * @brief Moves a configuration.
   * @SINCE_3_0.0
   */
  DragAutoScrollConfiguration(DragAutoScrollConfiguration&& rhs) noexcept;

  /**
   * @brief Replaces this configuration with an independent copy.
   * @SINCE_3_0.0
   */
  DragAutoScrollConfiguration& operator=(const DragAutoScrollConfiguration& rhs);

  /**
   * @brief Moves another configuration into this object.
   * @SINCE_3_0.0
   */
  DragAutoScrollConfiguration& operator=(DragAutoScrollConfiguration&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~DragAutoScrollConfiguration();

  /**
   * @brief Returns the View whose bounds define the edge zones.
   * @SINCE_3_0.0
   */
  Dali::Ui::View GetViewport() const;

  /**
   * @brief Returns the horizontal and vertical edge-zone widths.
   * @SINCE_3_0.0
   */
  const Vector2& GetEdgeThreshold() const;

  /**
   * @brief Returns the maximum horizontal and vertical scroll speed.
   * @SINCE_3_0.0
   */
  const Vector2& GetMaxSpeed() const;

  /**
   * @brief Returns the callback interval in milliseconds.
   * @SINCE_3_0.0
   */
  uint32_t GetTickIntervalMilliseconds() const;

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Snapshot passed to the drag auto-scroll callback.
 * @SINCE_3_0.0
 */
class DALI_UI_API DragAutoScrollEvent
{
public:
  /**
   * @brief Creates an empty auto-scroll snapshot.
   * @SINCE_3_0.0
   */
  DragAutoScrollEvent();

  /**
   * @brief Creates an independent copy of an auto-scroll snapshot.
   * @SINCE_3_0.0
   */
  DragAutoScrollEvent(const DragAutoScrollEvent& rhs);

  /**
   * @brief Moves an auto-scroll snapshot.
   * @SINCE_3_0.0
   */
  DragAutoScrollEvent(DragAutoScrollEvent&& rhs) noexcept;

  /**
   * @brief Replaces this snapshot with an independent copy.
   * @SINCE_3_0.0
   */
  DragAutoScrollEvent& operator=(const DragAutoScrollEvent& rhs);

  /**
   * @brief Moves another snapshot into this object.
   * @SINCE_3_0.0
   */
  DragAutoScrollEvent& operator=(DragAutoScrollEvent&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~DragAutoScrollEvent();

  /**
   * @brief Returns the current drag session snapshot.
   * @SINCE_3_0.0
   */
  const DragAndDropEvent& GetDragEvent() const;

  /**
   * @brief Returns the configured viewport.
   * @SINCE_3_0.0
   */
  Dali::Ui::View GetViewport() const;

  /**
   * @brief Returns the per-axis edge proximity in the range [-1, 1].
   * @SINCE_3_0.0
   */
  const Vector2& GetEdgeIntensity() const;

  /**
   * @brief Returns the suggested content delta for this tick.
   * @SINCE_3_0.0
   */
  const Vector2& GetSuggestedDelta() const;

  /**
   * @brief Returns the nominal tick interval in seconds.
   * @SINCE_3_0.0
   */
  float GetDeltaSeconds() const;

private:
  DALI_INTERNAL DragAutoScrollEvent(DragAndDropEvent dragEvent,
                                    Dali::Ui::View   viewport,
                                    Vector2          edgeIntensity,
                                    Vector2          suggestedDelta,
                                    float            deltaSeconds);

  friend class Internal::DragAndDropDetector;

  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Ui
} //namespace DALI_NAMESPACE

#endif // DALI_DRAG_AND_DROP_TYPES_H
