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
#include <dali-ui-foundation/public-api/drag-and-drop/drag-and-drop-detector.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/drag-drop-detector/drag-and-drop-detector-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <algorithm>
#include <utility>
#include <vector>

namespace Dali
{
namespace Ui
{
namespace
{
template<typename Impl>
const Impl& GetValueImplementation(const Dali::UniquePtr<Impl>& impl)
{
  DALI_ASSERT_ALWAYS(impl && "Cannot use a moved-from drag-and-drop value object");
  return *impl;
}

bool IsValidDragAndDropOperation(DragAndDropOperation operation)
{
  switch(operation)
  {
    case DragAndDropOperation::NONE:
    case DragAndDropOperation::COPY:
    case DragAndDropOperation::MOVE:
    case DragAndDropOperation::LINK:
      return true;
  }
  return false;
}
} // unnamed namespace

struct DALI_INTERNAL DragPayload::Impl
{
  using Representation = std::pair<Dali::String, Property::Value>;

  std::vector<Representation>       representations;
  std::vector<DragAndDropOperation> allowedOperations;
  DragAndDropOperation              preferredOperation{DragAndDropOperation::NONE};
};

DragPayload::DragPayload()
: mImpl(MakeUnique<Impl>())
{
}

DragPayload::DragPayload(Dali::String type, Property::Value data, DragAndDropOperation operation)
: mImpl(MakeUnique<Impl>())
{
  AddRepresentation(std::move(type), std::move(data));
  SetPreferredOperation(operation);
}

DragPayload::DragPayload(const DragPayload& rhs)
: mImpl(MakeUnique<Impl>(GetValueImplementation(rhs.mImpl)))
{
}

DragPayload::DragPayload(DragPayload&& rhs) noexcept = default;

DragPayload& DragPayload::operator=(const DragPayload& rhs)
{
  if(this != &rhs)
  {
    mImpl = MakeUnique<Impl>(GetValueImplementation(rhs.mImpl));
  }
  return *this;
}

DragPayload& DragPayload::operator=(DragPayload&& rhs) noexcept = default;

DragPayload::~DragPayload() = default;

void DragPayload::AddRepresentation(Dali::String type, Property::Value data)
{
  if(type.Empty())
  {
    DALI_LOG_ERROR("DragPayload representation type must not be empty.\n");
    return;
  }

  Impl& implementation = *mImpl;
  auto  match          = std::find_if(implementation.representations.begin(),
                                      implementation.representations.end(),
                                      [&type](const Impl::Representation& representation)
            {
    return representation.first == type;
  });
  if(match == implementation.representations.end())
  {
    implementation.representations.emplace_back(std::move(type), std::move(data));
  }
  else
  {
    match->second = std::move(data);
  }
}

bool DragPayload::RemoveRepresentation(const Dali::String& type)
{
  Impl& implementation = *mImpl;
  auto  match          = std::find_if(implementation.representations.begin(),
                                      implementation.representations.end(),
                                      [&type](const Impl::Representation& representation)
            {
    return representation.first == type;
  });
  if(match == implementation.representations.end())
  {
    return false;
  }
  implementation.representations.erase(match);
  return true;
}

void DragPayload::ClearRepresentations()
{
  mImpl->representations.clear();
}

uint32_t DragPayload::GetRepresentationCount() const
{
  return static_cast<uint32_t>(GetValueImplementation(mImpl).representations.size());
}

const Dali::String& DragPayload::GetRepresentationType(uint32_t index) const
{
  const auto& representations = GetValueImplementation(mImpl).representations;
  DALI_ASSERT_ALWAYS(index < representations.size() && "DragPayload representation index is out of range");
  return representations[index].first;
}

const Property::Value& DragPayload::GetRepresentationData(uint32_t index) const
{
  const auto& representations = GetValueImplementation(mImpl).representations;
  DALI_ASSERT_ALWAYS(index < representations.size() && "DragPayload representation index is out of range");
  return representations[index].second;
}

bool DragPayload::HasRepresentation(const Dali::String& type) const
{
  const auto& representations = GetValueImplementation(mImpl).representations;
  return std::find_if(representations.begin(),
                      representations.end(),
                      [&type](const Impl::Representation& representation)
  {
    return representation.first == type;
  }) != representations.end();
}

bool DragPayload::GetRepresentationData(const Dali::String& type, Property::Value& data) const
{
  const auto& representations = GetValueImplementation(mImpl).representations;
  auto        match           = std::find_if(representations.begin(),
                                             representations.end(),
                                             [&type](const Impl::Representation& representation)
                   {
    return representation.first == type;
  });
  if(match == representations.end())
  {
    return false;
  }
  data = match->second;
  return true;
}

void DragPayload::AddAllowedOperation(DragAndDropOperation operation)
{
  if(operation == DragAndDropOperation::NONE)
  {
    return;
  }
  if(!IsValidDragAndDropOperation(operation))
  {
    DALI_LOG_ERROR("DragPayload operation is invalid; it was not added.\n");
    return;
  }

  auto& operations = mImpl->allowedOperations;
  if(std::find(operations.begin(), operations.end(), operation) == operations.end())
  {
    operations.push_back(operation);
  }
}

bool DragPayload::RemoveAllowedOperation(DragAndDropOperation operation)
{
  if(!IsValidDragAndDropOperation(operation))
  {
    return false;
  }

  auto& operations = mImpl->allowedOperations;
  auto  match      = std::find(operations.begin(), operations.end(), operation);
  if(match == operations.end())
  {
    return false;
  }

  operations.erase(match);
  if(mImpl->preferredOperation == operation)
  {
    mImpl->preferredOperation =
      operations.empty() ? DragAndDropOperation::NONE : operations.front();
  }
  return true;
}

void DragPayload::ClearAllowedOperations()
{
  mImpl->allowedOperations.clear();
  mImpl->preferredOperation = DragAndDropOperation::NONE;
}

uint32_t DragPayload::GetAllowedOperationCount() const
{
  return static_cast<uint32_t>(GetValueImplementation(mImpl).allowedOperations.size());
}

DragAndDropOperation DragPayload::GetAllowedOperation(uint32_t index) const
{
  const auto& operations = GetValueImplementation(mImpl).allowedOperations;
  DALI_ASSERT_ALWAYS(index < operations.size() && "DragPayload operation index is out of range");
  return operations[index];
}

bool DragPayload::IsOperationAllowed(DragAndDropOperation operation) const
{
  if(!IsValidDragAndDropOperation(operation))
  {
    return false;
  }

  const auto& operations = GetValueImplementation(mImpl).allowedOperations;
  return std::find(operations.begin(), operations.end(), operation) != operations.end();
}

void DragPayload::SetPreferredOperation(DragAndDropOperation operation)
{
  if(!IsValidDragAndDropOperation(operation))
  {
    DALI_LOG_ERROR("DragPayload preferred operation is invalid; preference was not changed.\n");
    return;
  }

  if(operation != DragAndDropOperation::NONE)
  {
    AddAllowedOperation(operation);
  }
  mImpl->preferredOperation = operation;
}

DragAndDropOperation DragPayload::GetPreferredOperation() const
{
  return GetValueImplementation(mImpl).preferredOperation;
}

struct DALI_INTERNAL DropProposal::Impl
{
  bool                 accepted{false};
  Dali::String         selectedRepresentationType;
  DragAndDropOperation operation{DragAndDropOperation::NONE};
};

DropProposal::DropProposal()
: mImpl(MakeUnique<Impl>())
{
}

DropProposal::DropProposal(bool                 accepted,
                           Dali::String         selectedRepresentationType,
                           DragAndDropOperation operation)
: mImpl(MakeUnique<Impl>(Impl{accepted, std::move(selectedRepresentationType), operation}))
{
}

DropProposal::DropProposal(const DropProposal& rhs)
: mImpl(MakeUnique<Impl>(GetValueImplementation(rhs.mImpl)))
{
}

DropProposal::DropProposal(DropProposal&& rhs) noexcept = default;

DropProposal& DropProposal::operator=(const DropProposal& rhs)
{
  if(this != &rhs)
  {
    mImpl = MakeUnique<Impl>(GetValueImplementation(rhs.mImpl));
  }
  return *this;
}

DropProposal& DropProposal::operator=(DropProposal&& rhs) noexcept = default;

DropProposal::~DropProposal() = default;

DropProposal DropProposal::Reject()
{
  return {};
}

DropProposal DropProposal::Accept(Dali::String         selectedRepresentationType,
                                  DragAndDropOperation operation)
{
  return DropProposal(true, std::move(selectedRepresentationType), operation);
}

bool DropProposal::IsAccepted() const
{
  return GetValueImplementation(mImpl).accepted;
}

const Dali::String& DropProposal::GetSelectedRepresentationType() const
{
  return GetValueImplementation(mImpl).selectedRepresentationType;
}

DragAndDropOperation DropProposal::GetOperation() const
{
  return GetValueImplementation(mImpl).operation;
}

struct DALI_INTERNAL DragActivationConfiguration::Impl
{
  DragActivationMode mode{DragActivationMode::PAN};
  float              movementThreshold{0.0f};
};

DragActivationConfiguration::DragActivationConfiguration()
: mImpl(MakeUnique<Impl>())
{
}

DragActivationConfiguration::DragActivationConfiguration(DragActivationMode mode, float movementThreshold)
: mImpl(MakeUnique<Impl>(Impl{mode, movementThreshold}))
{
}

DragActivationConfiguration::DragActivationConfiguration(const DragActivationConfiguration& rhs)
: mImpl(MakeUnique<Impl>(GetValueImplementation(rhs.mImpl)))
{
}

DragActivationConfiguration::DragActivationConfiguration(DragActivationConfiguration&& rhs) noexcept = default;

DragActivationConfiguration& DragActivationConfiguration::operator=(const DragActivationConfiguration& rhs)
{
  if(this != &rhs)
  {
    mImpl = MakeUnique<Impl>(GetValueImplementation(rhs.mImpl));
  }
  return *this;
}

DragActivationConfiguration& DragActivationConfiguration::operator=(DragActivationConfiguration&& rhs) noexcept = default;

DragActivationConfiguration::~DragActivationConfiguration() = default;

DragActivationMode DragActivationConfiguration::GetMode() const
{
  return GetValueImplementation(mImpl).mode;
}

float DragActivationConfiguration::GetMovementThreshold() const
{
  return GetValueImplementation(mImpl).movementThreshold;
}

struct DALI_INTERNAL DragActivationEvent::Impl
{
  Dali::Ui::View      source;
  DragActivationMode  mode{DragActivationMode::PAN};
  Device::Class::Type deviceClass{Device::Class::NONE};
  Vector2             initialScreenPosition;
  Vector2             screenPosition;
  Vector2             sourceLocalPosition;
  Vector2             screenDisplacement;
  DragPayload         payload;
};

DragActivationEvent::DragActivationEvent()
: mImpl(MakeUnique<Impl>())
{
}

DragActivationEvent::DragActivationEvent(Dali::Ui::View      source,
                                         DragActivationMode  mode,
                                         Device::Class::Type deviceClass,
                                         Vector2             initialScreenPosition,
                                         Vector2             screenPosition,
                                         Vector2             sourceLocalPosition,
                                         Vector2             screenDisplacement,
                                         DragPayload         payload)
: mImpl(MakeUnique<Impl>(Impl{std::move(source),
                              mode,
                              deviceClass,
                              initialScreenPosition,
                              screenPosition,
                              sourceLocalPosition,
                              screenDisplacement,
                              std::move(payload)}))
{
}

DragActivationEvent::DragActivationEvent(const DragActivationEvent& rhs)
: mImpl(MakeUnique<Impl>(GetValueImplementation(rhs.mImpl)))
{
}

DragActivationEvent::DragActivationEvent(DragActivationEvent&& rhs) noexcept = default;

DragActivationEvent& DragActivationEvent::operator=(const DragActivationEvent& rhs)
{
  if(this != &rhs)
  {
    mImpl = MakeUnique<Impl>(GetValueImplementation(rhs.mImpl));
  }
  return *this;
}

DragActivationEvent& DragActivationEvent::operator=(DragActivationEvent&& rhs) noexcept = default;

DragActivationEvent::~DragActivationEvent() = default;

Dali::Ui::View DragActivationEvent::GetSource() const
{
  return GetValueImplementation(mImpl).source;
}

DragActivationMode DragActivationEvent::GetMode() const
{
  return GetValueImplementation(mImpl).mode;
}

Device::Class::Type DragActivationEvent::GetDeviceClass() const
{
  return GetValueImplementation(mImpl).deviceClass;
}

const Vector2& DragActivationEvent::GetInitialScreenPosition() const
{
  return GetValueImplementation(mImpl).initialScreenPosition;
}

const Vector2& DragActivationEvent::GetScreenPosition() const
{
  return GetValueImplementation(mImpl).screenPosition;
}

const Vector2& DragActivationEvent::GetSourceLocalPosition() const
{
  return GetValueImplementation(mImpl).sourceLocalPosition;
}

const Vector2& DragActivationEvent::GetScreenDisplacement() const
{
  return GetValueImplementation(mImpl).screenDisplacement;
}

const DragPayload& DragActivationEvent::GetPayload() const
{
  return GetValueImplementation(mImpl).payload;
}

struct DALI_INTERNAL DragAndDropEvent::Impl
{
  Dali::Ui::View          source;
  Dali::Ui::View          target;
  Dali::Ui::View          candidateTarget;
  Device::Class::Type     deviceClass{Device::Class::NONE};
  DragSessionOrigin       sessionOrigin{DragSessionOrigin::NONE};
  Vector2                 screenPosition;
  Vector2                 sourceGrabPosition;
  Vector2                 sourceParentPosition;
  Vector2                 targetLocalPosition;
  Vector2                 previewLocalPosition;
  Vector2                 sourceAnchor;
  DragPayload             payload;
  DropProposal            dropProposal;
  DragAndDropTargetStatus targetStatus{DragAndDropTargetStatus::NONE};
  DragAndDropResult       result{DragAndDropResult::NONE};
  DragCancelReason        cancelReason{DragCancelReason::NONE};
};

DragAndDropEvent::DragAndDropEvent()
: mImpl(MakeUnique<Impl>())
{
}

DragAndDropEvent::DragAndDropEvent(Dali::Ui::View          source,
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
                                   DragCancelReason        cancelReason)
: mImpl(MakeUnique<Impl>(Impl{std::move(source),
                              std::move(target),
                              std::move(candidateTarget),
                              deviceClass,
                              sessionOrigin,
                              screenPosition,
                              sourceGrabPosition,
                              sourceParentPosition,
                              targetLocalPosition,
                              previewLocalPosition,
                              sourceAnchor,
                              std::move(payload),
                              std::move(dropProposal),
                              targetStatus,
                              result,
                              cancelReason}))
{
}

DragAndDropEvent::DragAndDropEvent(const DragAndDropEvent& rhs)
: mImpl(MakeUnique<Impl>(GetValueImplementation(rhs.mImpl)))
{
}

DragAndDropEvent::DragAndDropEvent(DragAndDropEvent&& rhs) noexcept = default;

DragAndDropEvent& DragAndDropEvent::operator=(const DragAndDropEvent& rhs)
{
  if(this != &rhs)
  {
    mImpl = MakeUnique<Impl>(GetValueImplementation(rhs.mImpl));
  }
  return *this;
}

DragAndDropEvent& DragAndDropEvent::operator=(DragAndDropEvent&& rhs) noexcept = default;

DragAndDropEvent::~DragAndDropEvent() = default;

Dali::Ui::View DragAndDropEvent::GetSource() const
{
  return GetValueImplementation(mImpl).source;
}

Dali::Ui::View DragAndDropEvent::GetTarget() const
{
  return GetValueImplementation(mImpl).target;
}

Dali::Ui::View DragAndDropEvent::GetCandidateTarget() const
{
  return GetValueImplementation(mImpl).candidateTarget;
}

Device::Class::Type DragAndDropEvent::GetDeviceClass() const
{
  return GetValueImplementation(mImpl).deviceClass;
}

DragSessionOrigin DragAndDropEvent::GetSessionOrigin() const
{
  return GetValueImplementation(mImpl).sessionOrigin;
}

const Vector2& DragAndDropEvent::GetScreenPosition() const
{
  return GetValueImplementation(mImpl).screenPosition;
}

const Vector2& DragAndDropEvent::GetSourceGrabPosition() const
{
  return GetValueImplementation(mImpl).sourceGrabPosition;
}

const Vector2& DragAndDropEvent::GetSourceParentPosition() const
{
  return GetValueImplementation(mImpl).sourceParentPosition;
}

const Vector2& DragAndDropEvent::GetTargetLocalPosition() const
{
  return GetValueImplementation(mImpl).targetLocalPosition;
}

const Vector2& DragAndDropEvent::GetPreviewLocalPosition() const
{
  return GetValueImplementation(mImpl).previewLocalPosition;
}

const Vector2& DragAndDropEvent::GetSourceAnchor() const
{
  return GetValueImplementation(mImpl).sourceAnchor;
}

const DragPayload& DragAndDropEvent::GetPayload() const
{
  return GetValueImplementation(mImpl).payload;
}

const DropProposal& DragAndDropEvent::GetDropProposal() const
{
  return GetValueImplementation(mImpl).dropProposal;
}

bool DragAndDropEvent::GetSelectedRepresentationData(Property::Value& data) const
{
  const Impl& implementation = GetValueImplementation(mImpl);
  return implementation.dropProposal.IsAccepted() &&
         implementation.payload.GetRepresentationData(
           implementation.dropProposal.GetSelectedRepresentationType(),
           data);
}

DragAndDropTargetStatus DragAndDropEvent::GetTargetStatus() const
{
  return GetValueImplementation(mImpl).targetStatus;
}

DragAndDropResult DragAndDropEvent::GetResult() const
{
  return GetValueImplementation(mImpl).result;
}

DragCancelReason DragAndDropEvent::GetCancelReason() const
{
  return GetValueImplementation(mImpl).cancelReason;
}

struct DALI_INTERNAL DragAutoScrollConfiguration::Impl
{
  Dali::Ui::View viewport;
  Vector2        edgeThreshold{48.0f, 48.0f};
  Vector2        maxSpeed{600.0f, 600.0f};
  uint32_t       tickIntervalMilliseconds{16u};
};

DragAutoScrollConfiguration::DragAutoScrollConfiguration()
: mImpl(MakeUnique<Impl>())
{
}

DragAutoScrollConfiguration::DragAutoScrollConfiguration(Dali::Ui::View viewport,
                                                         Vector2        edgeThreshold,
                                                         Vector2        maxSpeed,
                                                         uint32_t       tickIntervalMilliseconds)
: mImpl(MakeUnique<Impl>(
    Impl{std::move(viewport), edgeThreshold, maxSpeed, tickIntervalMilliseconds}))
{
}

DragAutoScrollConfiguration::DragAutoScrollConfiguration(const DragAutoScrollConfiguration& rhs)
: mImpl(MakeUnique<Impl>(GetValueImplementation(rhs.mImpl)))
{
}

DragAutoScrollConfiguration::DragAutoScrollConfiguration(DragAutoScrollConfiguration&& rhs) noexcept = default;

DragAutoScrollConfiguration& DragAutoScrollConfiguration::operator=(const DragAutoScrollConfiguration& rhs)
{
  if(this != &rhs)
  {
    mImpl = MakeUnique<Impl>(GetValueImplementation(rhs.mImpl));
  }
  return *this;
}

DragAutoScrollConfiguration& DragAutoScrollConfiguration::operator=(DragAutoScrollConfiguration&& rhs) noexcept = default;

DragAutoScrollConfiguration::~DragAutoScrollConfiguration() = default;

Dali::Ui::View DragAutoScrollConfiguration::GetViewport() const
{
  return GetValueImplementation(mImpl).viewport;
}

const Vector2& DragAutoScrollConfiguration::GetEdgeThreshold() const
{
  return GetValueImplementation(mImpl).edgeThreshold;
}

const Vector2& DragAutoScrollConfiguration::GetMaxSpeed() const
{
  return GetValueImplementation(mImpl).maxSpeed;
}

uint32_t DragAutoScrollConfiguration::GetTickIntervalMilliseconds() const
{
  return GetValueImplementation(mImpl).tickIntervalMilliseconds;
}

struct DALI_INTERNAL DragAutoScrollEvent::Impl
{
  DragAndDropEvent dragEvent;
  Dali::Ui::View   viewport;
  Vector2          edgeIntensity;
  Vector2          suggestedDelta;
  float            deltaSeconds{0.0f};
};

DragAutoScrollEvent::DragAutoScrollEvent()
: mImpl(MakeUnique<Impl>())
{
}

DragAutoScrollEvent::DragAutoScrollEvent(DragAndDropEvent dragEvent,
                                         Dali::Ui::View   viewport,
                                         Vector2          edgeIntensity,
                                         Vector2          suggestedDelta,
                                         float            deltaSeconds)
: mImpl(MakeUnique<Impl>(Impl{std::move(dragEvent),
                              std::move(viewport),
                              edgeIntensity,
                              suggestedDelta,
                              deltaSeconds}))
{
}

DragAutoScrollEvent::DragAutoScrollEvent(const DragAutoScrollEvent& rhs)
: mImpl(MakeUnique<Impl>(GetValueImplementation(rhs.mImpl)))
{
}

DragAutoScrollEvent::DragAutoScrollEvent(DragAutoScrollEvent&& rhs) noexcept = default;

DragAutoScrollEvent& DragAutoScrollEvent::operator=(const DragAutoScrollEvent& rhs)
{
  if(this != &rhs)
  {
    mImpl = MakeUnique<Impl>(GetValueImplementation(rhs.mImpl));
  }
  return *this;
}

DragAutoScrollEvent& DragAutoScrollEvent::operator=(DragAutoScrollEvent&& rhs) noexcept = default;

DragAutoScrollEvent::~DragAutoScrollEvent() = default;

const DragAndDropEvent& DragAutoScrollEvent::GetDragEvent() const
{
  return GetValueImplementation(mImpl).dragEvent;
}

Dali::Ui::View DragAutoScrollEvent::GetViewport() const
{
  return GetValueImplementation(mImpl).viewport;
}

const Vector2& DragAutoScrollEvent::GetEdgeIntensity() const
{
  return GetValueImplementation(mImpl).edgeIntensity;
}

const Vector2& DragAutoScrollEvent::GetSuggestedDelta() const
{
  return GetValueImplementation(mImpl).suggestedDelta;
}

float DragAutoScrollEvent::GetDeltaSeconds() const
{
  return GetValueImplementation(mImpl).deltaSeconds;
}

DragAndDropDetector::DragAndDropDetector()
{
}

DragAndDropDetector::~DragAndDropDetector()
{
}

DragAndDropDetector DragAndDropDetector::New()
{
  return Dali::Ui::Internal::DragAndDropDetector::New();
}

void DragAndDropDetector::AttachSourceAndTarget(View view)
{
  AttachSource(view);
  AttachTarget(view);
}
void DragAndDropDetector::AttachSource(View view)
{
  GetImplementation(*this).AttachSource(view);
}
void DragAndDropDetector::SetSourcePayload(View source, DragPayload payload)
{
  GetImplementation(*this).SetSourcePayload(source, std::move(payload));
}
void DragAndDropDetector::ClearSourcePayload(View source)
{
  GetImplementation(*this).ClearSourcePayload(source);
}
void DragAndDropDetector::SetSourcePayloadProvider(View source, SourcePayloadProvider provider)
{
  GetImplementation(*this).SetSourcePayloadProvider(source, std::move(provider));
}
void DragAndDropDetector::ClearSourcePayloadProvider(View source)
{
  GetImplementation(*this).ClearSourcePayloadProvider(source);
}
void DragAndDropDetector::AttachTarget(View view)
{
  GetImplementation(*this).AttachTarget(view);
}
void DragAndDropDetector::SetDropProposalCallback(View target, DropProposalCallback callback)
{
  GetImplementation(*this).SetDropProposalCallback(target, std::move(callback));
}
void DragAndDropDetector::ClearDropProposalCallback(View target)
{
  GetImplementation(*this).ClearDropProposalCallback(target);
}

void DragAndDropDetector::SetAutoScrollCallback(DragAutoScrollConfiguration configuration,
                                                AutoScrollCallback          callback)
{
  GetImplementation(*this).SetAutoScrollCallback(std::move(configuration), std::move(callback));
}

void DragAndDropDetector::ClearAutoScrollCallback()
{
  GetImplementation(*this).ClearAutoScrollCallback();
}

void DragAndDropDetector::SetDragActivationMode(DragActivationMode mode)
{
  GetImplementation(*this).SetDragActivationMode(mode);
}

DragActivationMode DragAndDropDetector::GetDragActivationMode() const
{
  return GetImplementation(*this).GetDragActivationMode();
}

void DragAndDropDetector::SetDragActivationConfiguration(
  Device::Class::Type         deviceClass,
  DragActivationConfiguration configuration)
{
  GetImplementation(*this).SetDragActivationConfiguration(deviceClass, configuration);
}

void DragAndDropDetector::ClearDragActivationConfiguration(Device::Class::Type deviceClass)
{
  GetImplementation(*this).ClearDragActivationConfiguration(deviceClass);
}

bool DragAndDropDetector::HasDragActivationConfiguration(Device::Class::Type deviceClass) const
{
  return GetImplementation(*this).HasDragActivationConfiguration(deviceClass);
}

DragActivationConfiguration DragAndDropDetector::GetDragActivationConfiguration(
  Device::Class::Type deviceClass) const
{
  return GetImplementation(*this).GetDragActivationConfiguration(deviceClass);
}

void DragAndDropDetector::SetCanStartDragCallback(CanStartDragCallback callback)
{
  GetImplementation(*this).SetCanStartDragCallback(std::move(callback));
}

void DragAndDropDetector::ClearCanStartDragCallback()
{
  GetImplementation(*this).ClearCanStartDragCallback();
}

void DragAndDropDetector::SetDragStartThreshold(float threshold)
{
  GetImplementation(*this).SetDragStartThreshold(threshold);
}

float DragAndDropDetector::GetDragStartThreshold() const
{
  return GetImplementation(*this).GetDragStartThreshold();
}

bool DragAndDropDetector::IsDragActivationPending() const
{
  return GetImplementation(*this).IsDragActivationPending();
}

void DragAndDropDetector::DetachSourceAndTarget(View view)
{
  DetachSource(view);
  DetachTarget(view);
}
void DragAndDropDetector::DetachSource(View view)
{
  GetImplementation(*this).DetachSource(view);
}
void DragAndDropDetector::DetachTarget(View view)
{
  GetImplementation(*this).DetachTarget(view);
}

void DragAndDropDetector::DetachAll()
{
  GetImplementation(*this).DetachAll();
}

bool DragAndDropDetector::StartDrag(View source, Device::Class::Type deviceClass)
{
  return GetImplementation(*this).StartDrag(source, deviceClass);
}

bool DragAndDropDetector::StartDrag(View                source,
                                    DragPayload         payload,
                                    Device::Class::Type deviceClass)
{
  return GetImplementation(*this).StartDrag(source, std::move(payload), deviceClass);
}

bool DragAndDropDetector::MoveDragTo(View target)
{
  return GetImplementation(*this).MoveDragTo(target);
}

bool DragAndDropDetector::Drop()
{
  return GetImplementation(*this).Drop();
}

void DragAndDropDetector::CancelDrag()
{
  GetImplementation(*this).CancelDrag();
}

bool DragAndDropDetector::IsDragging() const
{
  return GetImplementation(*this).IsDragging();
}

DragSessionOrigin DragAndDropDetector::GetDragSessionOrigin() const
{
  return GetImplementation(*this).GetDragSessionOrigin();
}

View DragAndDropDetector::GetDragSource() const
{
  return GetImplementation(*this).GetDragSource();
}

View DragAndDropDetector::GetDragTarget() const
{
  return GetImplementation(*this).GetDragTarget();
}

uint32_t DragAndDropDetector::GetAttachedSourceCount() const
{
  return GetImplementation(*this).GetAttachedSourceCount();
}
uint32_t DragAndDropDetector::GetAttachedTargetCount() const
{
  return GetImplementation(*this).GetAttachedTargetCount();
}

View DragAndDropDetector::GetAttachedSource(uint32_t index) const
{
  return GetImplementation(*this).GetAttachedSource(index);
}

View DragAndDropDetector::GetAttachedTarget(uint32_t index) const
{
  return GetImplementation(*this).GetAttachedTarget(index);
}

void DragAndDropDetector::SetDragPreview(View preview)
{
  GetImplementation(*this).SetDragPreview(preview);
}

void DragAndDropDetector::SetDragPreviewContainer(View container)
{
  GetImplementation(*this).SetDragPreviewContainer(container);
}

void DragAndDropDetector::SetDragPreviewCallbacks(DragPreviewFactory   factory,
                                                  DragPreviewUpdater   updater,
                                                  DragPreviewFinalizer finalizer)
{
  GetImplementation(*this).SetDragPreviewCallbacks(std::move(factory), std::move(updater), std::move(finalizer));
}

void DragAndDropDetector::ClearDragPreviewCallbacks()
{
  GetImplementation(*this).ClearDragPreviewCallbacks();
}

DragAndDropDetector::DragPreviewPositionSignalType& DragAndDropDetector::DragPreviewPositionSignal()
{
  return GetImplementation(*this).DragPreviewPositionSignal();
}

const DragPayload& DragAndDropDetector::GetDragPayload() const
{
  return GetImplementation(*this).GetDragPayload();
}

const Vector2& DragAndDropDetector::GetCurrentScreenPosition() const
{
  return GetImplementation(*this).GetCurrentScreenPosition();
}

DragAndDropDetector::DragEventSignalType& DragAndDropDetector::StartedSignal()
{
  return GetImplementation(*this).StartedSignal();
}

DragAndDropDetector::DragEventSignalType& DragAndDropDetector::EnteredSignal()
{
  return GetImplementation(*this).EnteredSignal();
}

DragAndDropDetector::DragEventSignalType& DragAndDropDetector::ExitedSignal()
{
  return GetImplementation(*this).ExitedSignal();
}

DragAndDropDetector::DragEventSignalType& DragAndDropDetector::MovedSignal()
{
  return GetImplementation(*this).MovedSignal();
}

DragAndDropDetector::DragEventSignalType& DragAndDropDetector::DroppedSignal()
{
  return GetImplementation(*this).DroppedSignal();
}

DragAndDropDetector::DragEventSignalType& DragAndDropDetector::CancelledSignal()
{
  return GetImplementation(*this).CancelledSignal();
}

DragAndDropDetector::TargetFeedbackChangedSignalType& DragAndDropDetector::TargetFeedbackChangedSignal()
{
  return GetImplementation(*this).TargetFeedbackChangedSignal();
}

DragAndDropDetector::DragEventSignalType& DragAndDropDetector::EndedSignal()
{
  return GetImplementation(*this).EndedSignal();
}

DragAndDropDetector::DragAndDropDetector(Dali::Ui::Internal::DragAndDropDetector* detector)
: BaseHandle(detector)
{
}

} // namespace Ui

} // namespace Dali
