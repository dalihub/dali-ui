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

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class DragAndDropDetector;
}

/**
 * @brief Result associated with a drag-and-drop event snapshot.
 */
enum class DragAndDropResult : uint8_t
{
  NONE,      ///< The drag session is still active.
  DROPPED,   ///< The drag ended on an eligible target.
  CANCELLED, ///< The drag was interrupted or cancelled by registration removal.
  NO_TARGET  ///< The drag finished without an eligible target.
};

/**
 * @brief Origin that created a drag-and-drop session.
 */
enum class DragSessionOrigin : uint8_t
{
  NONE,    ///< There is no drag session.
  GESTURE, ///< A pointer gesture created the session.
  EXPLICIT ///< StartDrag() explicitly created the session.
};

/**
 * @brief Reason why an active drag-and-drop session was cancelled.
 */
enum class DragCancelReason : uint8_t
{
  NONE,                           ///< The event does not represent a cancellation.
  GESTURE_INTERRUPTED,            ///< The owning one-pointer pan was interrupted, including by another touch.
  SOURCE_DISCONNECTED,            ///< The active source left its scene.
  PREVIEW_CONTAINER_DISCONNECTED, ///< The configured drag preview container left its scene.
  WINDOW_FOCUS_LOST,              ///< The source window lost input focus.
  REGISTRATION_REMOVED,           ///< The active source or all registrations were removed.
  REQUESTED                       ///< CancelDrag() was explicitly requested.
};

/**
 * @brief Acceptance status of the target represented by a drag feedback event.
 */
enum class DragAndDropTargetStatus : uint8_t
{
  NONE,     ///< There is no active target feedback.
  ACCEPTED, ///< The candidate is eligible and is the current drop target.
  REJECTED  ///< Registered targets are intersected, but none accepts the drag.
};

/**
 * @brief Built-in gesture prerequisite used to begin drag activation.
 */
enum class DragActivationMode : uint8_t
{
  PAN,       ///< A recognized pan begins activation.
  LONG_PRESS ///< A platform-configured long press begins activation.
};

/**
 * @brief Semantic operation supported or selected by a drag-and-drop session.
 */
enum class DragAndDropOperation : uint8_t
{
  NONE, ///< The application did not specify a transfer operation.
  COPY, ///< Copy the represented data to the target.
  MOVE, ///< Move the represented data to the target.
  LINK  ///< Create a reference to the represented data.
};

/**
 * @brief Typed application data captured for one drag session.
 *
 * A payload can expose the same logical item through several representations.
 * Representation order expresses source preference. Type strings are
 * application-defined format identifiers; MIME-style strings are recommended,
 * but this same-process API does not serialize Property::Value.
 */
class DALI_UI_API DragPayload
{
public:
  /**
   * @brief Creates an empty payload.
   */
  DragPayload();

  /**
   * @brief Creates a payload with one representation and a preferred operation.
   *
   * A non-NONE preferred operation is also added to the allowed operation list.
   */
  DragPayload(Dali::String         type,
              Property::Value      data,
              DragAndDropOperation operation = DragAndDropOperation::NONE);

  /**
   * @brief Creates an independent copy of a payload.
   */
  DragPayload(const DragPayload& rhs);

  /**
   * @brief Moves a payload.
   */
  DragPayload(DragPayload&& rhs) noexcept;

  /**
   * @brief Replaces this payload with an independent copy.
   */
  DragPayload& operator=(const DragPayload& rhs);

  /**
   * @brief Moves another payload into this object.
   */
  DragPayload& operator=(DragPayload&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~DragPayload();

  /**
   * @brief Adds or replaces an application data representation.
   *
   * Replacing an existing type preserves its preference order.
   *
   * @param[in] type Non-empty application-defined format identifier
   * @param[in] data Same-process data for the representation
   */
  void AddRepresentation(Dali::String type, Property::Value data);

  /**
   * @brief Removes a representation by type.
   *
   * @return True if a representation was removed
   */
  bool RemoveRepresentation(const Dali::String& type);

  /**
   * @brief Removes every representation.
   */
  void ClearRepresentations();

  /**
   * @brief Returns the number of representations in preference order.
   */
  uint32_t GetRepresentationCount() const;

  /**
   * @brief Returns a representation type by preference index.
   *
   * @pre @p index must be less than GetRepresentationCount().
   */
  const Dali::String& GetRepresentationType(uint32_t index) const;

  /**
   * @brief Returns representation data by preference index.
   *
   * @pre @p index must be less than GetRepresentationCount().
   */
  const Property::Value& GetRepresentationData(uint32_t index) const;

  /**
   * @brief Tests whether a representation type is available.
   */
  bool HasRepresentation(const Dali::String& type) const;

  /**
   * @brief Copies data for a representation type.
   *
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
   */
  void AddAllowedOperation(DragAndDropOperation operation);

  /**
   * @brief Removes a source-supported operation.
   *
   * Removing the preferred operation selects the first remaining operation as
   * the new preference.
   *
   * @return True if an operation was removed
   */
  bool RemoveAllowedOperation(DragAndDropOperation operation);

  /**
   * @brief Clears every allowed operation and resets the preference to NONE.
   */
  void ClearAllowedOperations();

  /**
   * @brief Returns the number of allowed operations.
   */
  uint32_t GetAllowedOperationCount() const;

  /**
   * @brief Returns an allowed operation by insertion index.
   *
   * @pre @p index must be less than GetAllowedOperationCount().
   */
  DragAndDropOperation GetAllowedOperation(uint32_t index) const;

  /**
   * @brief Tests whether the source allows an operation.
   */
  bool IsOperationAllowed(DragAndDropOperation operation) const;

  /**
   * @brief Sets the preferred operation.
   *
   * A non-NONE operation is automatically added to the allowed list.
   * NONE clears only the preference.
   */
  void SetPreferredOperation(DragAndDropOperation operation);

  /**
   * @brief Returns the source-preferred operation, or NONE.
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
 */
class DALI_UI_API DropProposal
{
public:
  /**
   * @brief Creates a rejected proposal.
   */
  DropProposal();

  /**
   * @brief Creates an independent copy of a proposal.
   */
  DropProposal(const DropProposal& rhs);

  /**
   * @brief Moves a proposal.
   */
  DropProposal(DropProposal&& rhs) noexcept;

  /**
   * @brief Replaces this proposal with an independent copy.
   */
  DropProposal& operator=(const DropProposal& rhs);

  /**
   * @brief Moves another proposal into this object.
   */
  DropProposal& operator=(DropProposal&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~DropProposal();

  /**
   * @brief Creates a rejected proposal.
   */
  static DropProposal Reject();

  /**
   * @brief Creates an accepted proposal.
   *
   * An empty type requests the first payload representation. NONE requests the
   * payload's preferred operation, or its first allowed operation.
   */
  static DropProposal Accept(
    Dali::String         selectedRepresentationType = {},
    DragAndDropOperation operation                  = DragAndDropOperation::NONE);

  /**
   * @brief Returns whether the target accepted the candidate.
   */
  bool IsAccepted() const;

  /**
   * @brief Returns the selected representation type.
   *
   * This is concrete in accepted lifecycle events and can be empty only when
   * the payload has no representations.
   */
  const Dali::String& GetSelectedRepresentationType() const;

  /**
   * @brief Returns the selected operation.
   *
   * This can be NONE only when the payload has no allowed operations.
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
 */
class DALI_UI_API DragActivationConfiguration
{
public:
  /**
   * @brief Creates the default PAN configuration with no additional threshold.
   */
  DragActivationConfiguration();

  /**
   * @brief Creates a gesture activation configuration.
   */
  DragActivationConfiguration(DragActivationMode mode, float movementThreshold = 0.0f);

  /**
   * @brief Creates an independent copy of a configuration.
   */
  DragActivationConfiguration(const DragActivationConfiguration& rhs);

  /**
   * @brief Moves a configuration.
   */
  DragActivationConfiguration(DragActivationConfiguration&& rhs) noexcept;

  /**
   * @brief Replaces this configuration with an independent copy.
   */
  DragActivationConfiguration& operator=(const DragActivationConfiguration& rhs);

  /**
   * @brief Moves another configuration into this object.
   */
  DragActivationConfiguration& operator=(DragActivationConfiguration&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~DragActivationConfiguration();

  /**
   * @brief Returns the gesture prerequisite.
   */
  DragActivationMode GetMode() const;

  /**
   * @brief Returns the additional screen-space movement threshold in pixels.
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
 */
class DALI_UI_API DragActivationEvent
{
public:
  /**
   * @brief Creates an empty activation snapshot.
   */
  DragActivationEvent();

  /**
   * @brief Creates an independent copy of an activation snapshot.
   */
  DragActivationEvent(const DragActivationEvent& rhs);

  /**
   * @brief Moves an activation snapshot.
   */
  DragActivationEvent(DragActivationEvent&& rhs) noexcept;

  /**
   * @brief Replaces this snapshot with an independent copy.
   */
  DragActivationEvent& operator=(const DragActivationEvent& rhs);

  /**
   * @brief Moves another snapshot into this object.
   */
  DragActivationEvent& operator=(DragActivationEvent&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~DragActivationEvent();

  /**
   * @brief Returns the candidate drag source.
   */
  Dali::Ui::View GetSource() const;

  /**
   * @brief Returns the built-in activation mode.
   */
  DragActivationMode GetMode() const;

  /**
   * @brief Returns the input device class.
   */
  Device::Class::Type GetDeviceClass() const;

  /**
   * @brief Returns the activation origin in screen coordinates.
   */
  const Vector2& GetInitialScreenPosition() const;

  /**
   * @brief Returns the current pointer position in screen coordinates.
   */
  const Vector2& GetScreenPosition() const;

  /**
   * @brief Returns the current pointer position in source coordinates.
   */
  const Vector2& GetSourceLocalPosition() const;

  /**
   * @brief Returns the current position minus the activation origin.
   */
  const Vector2& GetScreenDisplacement() const;

  /**
   * @brief Returns the payload proposed for the drag session.
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
 */
class DALI_UI_API DragAndDropEvent
{
public:
  /**
   * @brief Creates an empty drag-and-drop snapshot.
   */
  DragAndDropEvent();

  /**
   * @brief Creates an independent copy of a drag-and-drop snapshot.
   */
  DragAndDropEvent(const DragAndDropEvent& rhs);

  /**
   * @brief Moves a drag-and-drop snapshot.
   */
  DragAndDropEvent(DragAndDropEvent&& rhs) noexcept;

  /**
   * @brief Replaces this snapshot with an independent copy.
   */
  DragAndDropEvent& operator=(const DragAndDropEvent& rhs);

  /**
   * @brief Moves another snapshot into this object.
   */
  DragAndDropEvent& operator=(DragAndDropEvent&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~DragAndDropEvent();

  /** @brief Returns the source that started the drag. */
  Dali::Ui::View GetSource() const;

  /**
   * @brief Returns the accepted current or finishing target, or an empty handle.
   *
   * During DropProposalCallback this is the target accepted before the current
   * candidate evaluation, if any. Use GetCandidateTarget() for the target being
   * evaluated.
   */
  Dali::Ui::View GetTarget() const;

  /**
   * @brief Returns the target being evaluated or represented by feedback.
   *
   * DropProposalCallback should use this getter to identify its candidate.
   */
  Dali::Ui::View GetCandidateTarget() const;

  /**
   * @brief Returns the input device class that owns the session.
   */
  Device::Class::Type GetDeviceClass() const;

  /**
   * @brief Returns how the drag session was created.
   */
  DragSessionOrigin GetSessionOrigin() const;

  /**
   * @brief Returns the current pointer position in screen coordinates.
   */
  const Vector2& GetScreenPosition() const;

  /**
   * @brief Returns the drag-start pointer offset in source coordinates.
   */
  const Vector2& GetSourceGrabPosition() const;

  /**
   * @brief Returns the pointer position in source-parent coordinates.
   */
  const Vector2& GetSourceParentPosition() const;

  /**
   * @brief Returns the pointer position in target or candidate coordinates.
   */
  const Vector2& GetTargetLocalPosition() const;

  /**
   * @brief Returns the pointer position in preview-parent coordinates.
   */
  const Vector2& GetPreviewLocalPosition() const;

  /**
   * @brief Returns the normalized drag-start point within the source.
   */
  const Vector2& GetSourceAnchor() const;

  /**
   * @brief Returns the payload captured when the drag started.
   */
  const DragPayload& GetPayload() const;

  /**
   * @brief Returns the target's concrete selection, or a rejected proposal.
   *
   * An accepted proposal describes target eligibility and does not by itself
   * mean a drop occurred. Check GetResult() or handle DroppedSignal before
   * mutating application data.
   */
  const DropProposal& GetDropProposal() const;

  /**
   * @brief Copies the data selected by the accepted drop proposal.
   *
   * @param[out] data Receives the selected representation
   * @return True if the proposal is accepted and its selected representation
   * exists in the payload. The output is unchanged when false is returned.
   */
  bool GetSelectedRepresentationData(Property::Value& data) const;

  /**
   * @brief Returns the current target feedback status.
   */
  DragAndDropTargetStatus GetTargetStatus() const;

  /**
   * @brief Returns the current or terminal session result.
   */
  DragAndDropResult GetResult() const;

  /**
   * @brief Returns the cancellation reason, or NONE.
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
                                 DragCancelReason        cancelReason);

  friend class Internal::DragAndDropDetector;

  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Configuration for callback-driven drag auto-scroll.
 *
 * The detector only calculates edge proximity and a suggested content delta.
 * The application callback owns the actual scrolling operation.
 */
class DALI_UI_API DragAutoScrollConfiguration
{
public:
  /**
   * @brief Creates an empty configuration with default edge and timing values.
   */
  DragAutoScrollConfiguration();

  /**
   * @brief Creates an auto-scroll configuration for a viewport.
   */
  DragAutoScrollConfiguration(Dali::Ui::View viewport,
                              Vector2        edgeThreshold            = Vector2(48.0f, 48.0f),
                              Vector2        maxSpeed                 = Vector2(600.0f, 600.0f),
                              uint32_t       tickIntervalMilliseconds = 16u);

  /**
   * @brief Creates an independent copy of a configuration.
   */
  DragAutoScrollConfiguration(const DragAutoScrollConfiguration& rhs);

  /**
   * @brief Moves a configuration.
   */
  DragAutoScrollConfiguration(DragAutoScrollConfiguration&& rhs) noexcept;

  /**
   * @brief Replaces this configuration with an independent copy.
   */
  DragAutoScrollConfiguration& operator=(const DragAutoScrollConfiguration& rhs);

  /**
   * @brief Moves another configuration into this object.
   */
  DragAutoScrollConfiguration& operator=(DragAutoScrollConfiguration&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~DragAutoScrollConfiguration();

  /**
   * @brief Returns the View whose bounds define the edge zones.
   */
  Dali::Ui::View GetViewport() const;

  /**
   * @brief Returns the horizontal and vertical edge-zone widths.
   */
  const Vector2& GetEdgeThreshold() const;

  /**
   * @brief Returns the maximum horizontal and vertical scroll speed.
   */
  const Vector2& GetMaxSpeed() const;

  /**
   * @brief Returns the callback interval in milliseconds.
   */
  uint32_t GetTickIntervalMilliseconds() const;

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Snapshot passed to the drag auto-scroll callback.
 */
class DALI_UI_API DragAutoScrollEvent
{
public:
  /**
   * @brief Creates an empty auto-scroll snapshot.
   */
  DragAutoScrollEvent();

  /**
   * @brief Creates an independent copy of an auto-scroll snapshot.
   */
  DragAutoScrollEvent(const DragAutoScrollEvent& rhs);

  /**
   * @brief Moves an auto-scroll snapshot.
   */
  DragAutoScrollEvent(DragAutoScrollEvent&& rhs) noexcept;

  /**
   * @brief Replaces this snapshot with an independent copy.
   */
  DragAutoScrollEvent& operator=(const DragAutoScrollEvent& rhs);

  /**
   * @brief Moves another snapshot into this object.
   */
  DragAutoScrollEvent& operator=(DragAutoScrollEvent&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~DragAutoScrollEvent();

  /**
   * @brief Returns the current drag session snapshot.
   */
  const DragAndDropEvent& GetDragEvent() const;

  /**
   * @brief Returns the configured viewport.
   */
  Dali::Ui::View GetViewport() const;

  /**
   * @brief Returns the per-axis edge proximity in the range [-1, 1].
   */
  const Vector2& GetEdgeIntensity() const;

  /**
   * @brief Returns the suggested content delta for this tick.
   */
  const Vector2& GetSuggestedDelta() const;

  /**
   * @brief Returns the nominal tick interval in seconds.
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
} // namespace Dali

#endif // DALI_DRAG_AND_DROP_TYPES_H
