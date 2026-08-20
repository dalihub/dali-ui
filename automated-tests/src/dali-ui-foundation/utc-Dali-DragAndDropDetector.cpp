/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include <dali-ui-foundation/public-api/drag-and-drop/drag-and-drop-detector.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-adaptor-impl.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <test-gesture-generator.h>
#include <algorithm>
#include <string>
#include <vector>
#include "../../../samples/in-scene-drag-and-drop/drag-session-controller.h"

using namespace Dali;
using namespace Dali::Ui;

namespace Test
{
void EmitGlobalTimerSignal();
}

namespace
{
static_assert(sizeof(DragPayload) == sizeof(void*), "DragPayload must remain a one-pointer value type");
static_assert(sizeof(DropProposal) == sizeof(void*), "DropProposal must remain a one-pointer value type");
static_assert(sizeof(DragActivationConfiguration) == sizeof(void*), "DragActivationConfiguration must remain a one-pointer value type");
static_assert(sizeof(DragActivationEvent) == sizeof(void*), "DragActivationEvent must remain a one-pointer value type");
static_assert(sizeof(DragAndDropEvent) == sizeof(void*), "DragAndDropEvent must remain a one-pointer value type");
static_assert(sizeof(DragAutoScrollConfiguration) == sizeof(void*), "DragAutoScrollConfiguration must remain a one-pointer value type");
static_assert(sizeof(DragAutoScrollEvent) == sizeof(void*), "DragAutoScrollEvent must remain a one-pointer value type");

struct SignalLog : public ConnectionTracker
{
  void OnStarted(const DragAndDropEvent& event, DragAndDropDetector)
  {
    ++started;
    events.push_back(event);
  }
  void OnEntered(const DragAndDropEvent& event, DragAndDropDetector)
  {
    ++entered;
    events.push_back(event);
  }
  void OnMoved(const DragAndDropEvent& event, DragAndDropDetector)
  {
    ++moved;
    events.push_back(event);
  }
  void OnExited(const DragAndDropEvent& event, DragAndDropDetector)
  {
    ++exited;
    events.push_back(event);
  }
  void OnDropped(const DragAndDropEvent& event, DragAndDropDetector)
  {
    ++dropped;
    events.push_back(event);
  }
  void OnCancelled(const DragAndDropEvent& event, DragAndDropDetector)
  {
    ++cancelled;
    cancelReasons.push_back(event.GetCancelReason());
    events.push_back(event);
  }
  void OnEnded(const DragAndDropEvent& event, DragAndDropDetector)
  {
    ++ended;
    terminalEvents.push_back(event);
  }

  int                           started{0};
  int                           entered{0};
  int                           moved{0};
  int                           exited{0};
  int                           dropped{0};
  int                           cancelled{0};
  int                           ended{0};
  std::vector<DragCancelReason> cancelReasons;
  std::vector<DragAndDropEvent> events;
  std::vector<DragAndDropEvent> terminalEvents;
};

struct DetachOnExit : public ConnectionTracker
{
  void OnExited(const DragAndDropEvent& event, DragAndDropDetector detector)
  {
    ++exited;
    detector.DetachTarget(event.GetTarget());
  }

  int exited{0};
};

struct ResetDetectorCallbacks : public ConnectionTracker
{
  explicit ResetDetectorCallbacks(DragAndDropDetector& owner)
  : detector(owner)
  {
  }

  void OnStarted(const DragAndDropEvent&, DragAndDropDetector)
  {
    ++started;
    detector.Reset();
  }

  void OnCancelled(const DragAndDropEvent&, DragAndDropDetector)
  {
    ++cancelled;
    detector.Reset();
  }

  DragAndDropDetector& detector;
  int                  started{0};
  int                  cancelled{0};
};

struct ActivationGate : public ConnectionTracker
{
  bool Approve(const DragActivationEvent& event)
  {
    ++calls;
    mode         = event.GetMode();
    deviceClass  = event.GetDeviceClass();
    displacement = event.GetScreenDisplacement();
    if(event.GetPayload().GetRepresentationCount() > 0u)
    {
      event.GetPayload().GetRepresentationData(0u).Get(payload);
    }
    source = event.GetSource();
    if(cancelPending)
    {
      detector.CancelDrag();
    }
    return allow;
  }

  bool                allow{false};
  bool                cancelPending{false};
  int                 calls{0};
  int32_t             payload{-1};
  DragActivationMode  mode{DragActivationMode::PAN};
  Device::Class::Type deviceClass{Device::Class::NONE};
  Vector2             displacement;
  View                source;
  DragAndDropDetector detector;
};

struct DisconnectTargetOnAcceptance : public ConnectionTracker
{
  DropProposal Accept(const DragAndDropEvent& event)
  {
    ++calls;
    View target = event.GetCandidateTarget();
    target.Unparent();
    return DropProposal::Accept();
  }

  int calls{0};
};

struct MutateRegistrationOnAcceptance : public ConnectionTracker
{
  DropProposal Accept(const DragAndDropEvent&)
  {
    detector.DetachTarget(target);
    return DropProposal::Accept();
  }

  DragAndDropDetector detector;
  View                target;
};

struct ReplaceVisualOnDrop : public ConnectionTracker
{
  explicit ReplaceVisualOnDrop(View replacement)
  : nextVisual(replacement)
  {
  }

  void OnDropped(const DragAndDropEvent&, DragAndDropDetector detector)
  {
    detector.SetDragPreview(nextVisual);
  }

  View nextVisual;
};

struct TargetLog : public ConnectionTracker
{
  void OnEntered(const DragAndDropEvent& event, DragAndDropDetector)
  {
    entered = event.GetTarget();
    ++enteredCount;
  }

  void OnExited(const DragAndDropEvent& event, DragAndDropDetector)
  {
    exited = event.GetTarget();
    ++exitedCount;
  }

  void OnDropped(const DragAndDropEvent& event, DragAndDropDetector)
  {
    dropped = event.GetTarget();
  }

  View entered;
  View exited;
  View dropped;
  int  enteredCount{0};
  int  exitedCount{0};
};

struct LifecycleOrderLog : public ConnectionTracker
{
  enum class Event
  {
    STARTED,
    ENTERED,
    EXITED,
    DROPPED,
    CANCELLED,
    ENDED
  };

  void OnStarted(const DragAndDropEvent&, DragAndDropDetector)
  {
    events.push_back(Event::STARTED);
  }
  void OnEntered(const DragAndDropEvent&, DragAndDropDetector)
  {
    events.push_back(Event::ENTERED);
  }
  void OnExited(const DragAndDropEvent&, DragAndDropDetector)
  {
    events.push_back(Event::EXITED);
  }
  void OnDropped(const DragAndDropEvent&, DragAndDropDetector)
  {
    events.push_back(Event::DROPPED);
  }
  void OnCancelled(const DragAndDropEvent&, DragAndDropDetector)
  {
    events.push_back(Event::CANCELLED);
  }
  void OnEnded(const DragAndDropEvent&, DragAndDropDetector)
  {
    events.push_back(Event::ENDED);
  }

  std::vector<Event> events;
};

struct SessionVisualCallbacks
{
  View Create(const DragAndDropEvent& event)
  {
    View visual = View::New();
    visual.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    visual.SetProperty(Actor::Property::SENSITIVE, false);
    if(factoryParent)
    {
      factoryParent.Add(visual);
    }
    factoryEvents.push_back(event);
    visuals.push_back(visual);
    return visual;
  }

  void Update(View visual, const DragAndDropEvent& event)
  {
    updateEvents.push_back(event);
    visual.SetLayoutMode(LayoutMode::STANDALONE);
    visual.SetUiScalePolicy(UiScalePolicy::DISABLED);
    visual.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    visual.SetProperty(Actor::Property::POSITION_USES_PIVOT, true);
    visual.SetProperty(Actor::Property::PIVOT,
                       Vector3(event.GetSourceAnchor().x, event.GetSourceAnchor().y, 0.5f));
    visual.SetRequestedX(event.GetPreviewLocalPosition().x);
    visual.SetRequestedY(event.GetPreviewLocalPosition().y);
  }

  void Finalize(View visual, const DragAndDropEvent& event)
  {
    finalizerEvents.push_back(event);
    finalizerVisuals.push_back(visual);
    visualsWereDetached = visualsWereDetached && !visual.GetParent();
  }

  std::vector<DragAndDropEvent> factoryEvents;
  std::vector<DragAndDropEvent> updateEvents;
  std::vector<DragAndDropEvent> finalizerEvents;
  std::vector<View>             visuals;
  std::vector<View>             finalizerVisuals;
  View                          factoryParent;
  bool                          visualsWereDetached{true};
};

struct PayloadAcceptanceCallbacks : public ConnectionTracker
{
  DropProposal AcceptLower(const DragAndDropEvent& event)
  {
    lowerEvents.push_back(event);
    int32_t payload = -1;
    event.GetPayload().GetRepresentationData(0u).Get(payload);
    return payload == 1 ? DropProposal::Accept() : DropProposal::Reject();
  }

  DropProposal AcceptUpper(const DragAndDropEvent& event)
  {
    upperEvents.push_back(event);
    int32_t payload = -1;
    event.GetPayload().GetRepresentationData(0u).Get(payload);
    return payload == 2 ? DropProposal::Accept() : DropProposal::Reject();
  }

  void OnStarted(const DragAndDropEvent& event, DragAndDropDetector detector)
  {
    int32_t payload = -1;
    detector.GetDragPayload().GetRepresentationData(0u).Get(payload);
    startedPayloads.push_back(payload);
    if(startedPayloads.size() == 1u)
    {
      // The active session keeps payload 1. This update applies to the next drag.
      detector.SetSourcePayload(
        event.GetSource(),
        {"application/x-test", Property::Value(2), DragAndDropOperation::MOVE});
    }
  }

  void OnDropped(const DragAndDropEvent& event, DragAndDropDetector detector)
  {
    int32_t payload = -1;
    detector.GetDragPayload().GetRepresentationData(0u).Get(payload);
    droppedPayloads.push_back(payload);
    droppedTargets.push_back(event.GetTarget());
  }

  std::vector<DragAndDropEvent> lowerEvents;
  std::vector<DragAndDropEvent> upperEvents;
  std::vector<int32_t>          startedPayloads;
  std::vector<int32_t>          droppedPayloads;
  std::vector<View>             droppedTargets;
};

struct DynamicPayloadProvider
{
  DragPayload Provide(const DragActivationEvent& event)
  {
    ++calls;
    activationEvents.push_back(event);
    return {"application/x-dynamic-test",
            Property::Value(100 + calls),
            DragAndDropOperation::COPY};
  }

  int                              calls{0};
  std::vector<DragActivationEvent> activationEvents;
};

struct AttemptRestartOnEnded : public ConnectionTracker
{
  void OnEnded(const DragAndDropEvent& event, DragAndDropDetector detector)
  {
    events.push_back(event);
    if(events.size() == 1u)
    {
      attempted = true;
      restarted = detector.StartDrag(
        event.GetSource(),
        {"application/x-restarted-test",
         Property::Value(88),
         DragAndDropOperation::COPY},
        Device::Class::KEYBOARD);
    }
  }

  bool                          attempted{false};
  bool                          restarted{false};
  std::vector<DragAndDropEvent> events;
};

struct TargetFeedbackCallbacks : public ConnectionTracker
{
  DropProposal Accept(const DragAndDropEvent&)
  {
    return accepted ? DropProposal::Accept() : DropProposal::Reject();
  }

  DropProposal Reject(const DragAndDropEvent&)
  {
    return DropProposal::Reject();
  }

  void OnFeedbackChanged(const DragAndDropEvent& event, DragAndDropDetector)
  {
    events.push_back(event);
  }

  bool                          accepted{false};
  std::vector<DragAndDropEvent> events;
};

struct RepresentationProposalCallbacks : public ConnectionTracker
{
  enum class Mode
  {
    SELECT_ALTERNATE,
    INVALID_TYPE,
    INVALID_OPERATION
  };

  DropProposal Propose(const DragAndDropEvent& event)
  {
    candidateEvents.push_back(event);
    switch(mode)
    {
      case Mode::SELECT_ALTERNATE:
        return DropProposal::Accept("text/plain", DragAndDropOperation::COPY);
      case Mode::INVALID_TYPE:
        return DropProposal::Accept("application/x-missing", DragAndDropOperation::COPY);
      case Mode::INVALID_OPERATION:
        return DropProposal::Accept("text/plain", DragAndDropOperation::MOVE);
    }
    return DropProposal::Reject();
  }

  Mode                          mode{Mode::SELECT_ALTERNATE};
  std::vector<DragAndDropEvent> candidateEvents;
};

struct AutoScrollCallbacks
{
  bool Apply(const DragAutoScrollEvent& event)
  {
    events.push_back(event);
    return contentChanged;
  }

  std::vector<DragAutoScrollEvent> events;
  bool                             contentChanged{true};
};

struct SessionQueryLog : public ConnectionTracker
{
  void OnDropped(const DragAndDropEvent& event, DragAndDropDetector detector)
  {
    dropEvent      = event;
    draggingInDrop = detector.IsDragging();
    sourceInDrop   = detector.GetDragSource();
    targetInDrop   = detector.GetDragTarget();
  }

  void OnCancelled(const DragAndDropEvent& event, DragAndDropDetector detector)
  {
    cancellationEvent      = event;
    draggingInCancellation = detector.IsDragging();
    sourceInCancellation   = detector.GetDragSource();
    targetInCancellation   = detector.GetDragTarget();
  }

  void OnEnded(const DragAndDropEvent& event, DragAndDropDetector detector)
  {
    endedEvent      = event;
    draggingInEnded = detector.IsDragging();
    sourceInEnded   = detector.GetDragSource();
    targetInEnded   = detector.GetDragTarget();
  }

  DragAndDropEvent cancellationEvent;
  DragAndDropEvent dropEvent;
  DragAndDropEvent endedEvent;
  View             sourceInCancellation;
  View             targetInCancellation;
  View             sourceInDrop;
  View             targetInDrop;
  View             sourceInEnded;
  View             targetInEnded;
  bool             draggingInCancellation{false};
  bool             draggingInDrop{false};
  bool             draggingInEnded{true};
};

struct CancelOnEntered : public ConnectionTracker
{
  void OnEntered(const DragAndDropEvent& event, DragAndDropDetector detector)
  {
    enteredTarget        = event.GetTarget();
    source               = detector.GetDragSource();
    draggingBeforeCancel = detector.IsDragging();
    detector.CancelDrag();
    draggingAfterCancel = detector.IsDragging();
  }

  View enteredTarget;
  View source;
  bool draggingBeforeCancel{false};
  bool draggingAfterCancel{true};
};

class TestAccessibilityDragActionHandler
{
public:
  virtual bool Activate(View actionView)                 = 0;
  virtual bool Adjust(View actionView, bool isIncreased) = 0;
  virtual bool Escape(View actionView)                   = 0;

protected:
  virtual ~TestAccessibilityDragActionHandler() = default;
};

class TestAccessibilityDragViewImpl : public ViewImpl
{
public:
  using Ptr = IntrusivePtr<TestAccessibilityDragViewImpl>;

  static Ptr New(TestAccessibilityDragActionHandler& handler)
  {
    return Ptr(new TestAccessibilityDragViewImpl(&handler));
  }

  static Ptr New()
  {
    return Ptr(new TestAccessibilityDragViewImpl(nullptr));
  }

  bool OnAccessibilityActivate() override
  {
    return mHandler && mHandler->Activate(View::DownCast(Self()));
  }

  bool OnAccessibilityValueChange(bool isIncreased) override
  {
    return mHandler && mHandler->Adjust(View::DownCast(Self()), isIncreased);
  }

  bool OnAccessibilityEscape() override
  {
    return mHandler && mHandler->Escape(View::DownCast(Self()));
  }

protected:
  ~TestAccessibilityDragViewImpl() override = default;

private:
  explicit TestAccessibilityDragViewImpl(TestAccessibilityDragActionHandler* handler)
  : mHandler(handler)
  {
  }

  TestAccessibilityDragActionHandler* mHandler;
};

View CreateAccessibilityDragView(TestAccessibilityDragActionHandler& handler,
                                 const Vector2&                      position)
{
  TestAccessibilityDragViewImpl::Ptr impl = TestAccessibilityDragViewImpl::New(handler);
  View                               view(*impl);
  impl->Initialize();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetRequestedX(position.x);
  view.SetRequestedY(position.y);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetFocusable(true);
  return view;
}

Dali::BaseHandle CreateRegisteredTestAccessibilityDragView()
{
  TestAccessibilityDragViewImpl::Ptr impl = TestAccessibilityDragViewImpl::New();
  View                               view(*impl);
  impl->Initialize();
  return view;
}

DALI_TYPE_REGISTRATION_BEGIN(
  TestAccessibilityDragViewImpl,
  Dali::Ui::View,
  CreateRegisteredTestAccessibilityDragView)
DALI_TYPE_REGISTRATION_END()

struct AccessibilityDragActionController : public TestAccessibilityDragActionHandler
{
  bool Activate(View actionView) override
  {
    ++activateCount;
    if(!detector.IsDragging())
    {
      return detector.StartDrag(actionView, Device::Class::USER);
    }

    detector.Drop();
    return true;
  }

  bool Adjust(View actionView, bool isIncreased) override
  {
    ++adjustCount;
    lastAdjustmentIncreased = isIncreased;
    if(!detector.IsDragging() ||
       detector.GetDragSessionOrigin() != DragSessionOrigin::EXPLICIT)
    {
      return false;
    }

    FocusManager::Get().SetCurrentFocusView(actionView);
    detector.MoveDragTo(actionView);
    return true;
  }

  bool Escape(View) override
  {
    ++escapeCount;
    if(!detector.IsDragging())
    {
      return false;
    }
    detector.CancelDrag();
    return true;
  }

  DragAndDropDetector detector;
  int                 activateCount{0};
  int                 adjustCount{0};
  int                 escapeCount{0};
  bool                lastAdjustmentIncreased{false};
};

struct DragSessionControllerSignalBridge : public ConnectionTracker
{
  DragSessionControllerSignalBridge(Samples::DragSessionController& dragController,
                                    const std::vector<View>&        dragTargets)
  : controller(dragController),
    targets(dragTargets)
  {
  }

  void OnStarted(const DragAndDropEvent& event, DragAndDropDetector)
  {
    controller.HandleStarted(event);
  }

  void OnFeedback(const DragAndDropEvent& event, DragAndDropDetector)
  {
    controller.HandleTargetFeedback(event);
  }

  void OnDropped(const DragAndDropEvent& event, DragAndDropDetector)
  {
    for(uint32_t position = 0u; position < targets.size(); ++position)
    {
      if(targets[position] == event.GetTarget())
      {
        controller.HandleDropped(position);
        return;
      }
    }
  }

  void OnCancelled(const DragAndDropEvent& event, DragAndDropDetector)
  {
    controller.HandleCancelled(event);
  }

  void OnEnded(const DragAndDropEvent&, DragAndDropDetector)
  {
    controller.HandleEnded();
  }

  Samples::DragSessionController& controller;
  const std::vector<View>&        targets;
};

View CreateView(const Vector2& position)
{
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetRequestedX(position.x);
  view.SetRequestedY(position.y);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  return view;
}

Dali::Integration::TouchEvent MakeDeviceTouch(PointState::Type    state,
                                              const Vector2&      position,
                                              uint32_t            time,
                                              Device::Class::Type deviceClass)
{
  Dali::Integration::TouchEvent event;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(position);
  point.SetDeviceClass(deviceClass);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  point.SetMouseButton(MouseButton::PRIMARY);
  event.points.push_back(point);
  event.time = time;
  return event;
}

void StartPanForDevice(TestApplication&    application,
                       const Vector2&      start,
                       const Vector2&      end,
                       uint32_t&           time,
                       Device::Class::Type deviceClass)
{
  application.ProcessEvent(MakeDeviceTouch(PointState::DOWN, start, time, deviceClass));
  time += TestGetFrameInterval();
  application.ProcessEvent(MakeDeviceTouch(PointState::MOTION, end, time, deviceClass));
  time += TestGetFrameInterval();
  application.ProcessEvent(MakeDeviceTouch(PointState::MOTION, end, time, deviceClass));
  time += TestGetFrameInterval();
}

void MovePanForDevice(TestApplication&    application,
                      const Vector2&      position,
                      uint32_t            time,
                      Device::Class::Type deviceClass)
{
  application.ProcessEvent(MakeDeviceTouch(PointState::MOTION, position, time, deviceClass));
}

void EndGestureForDevice(TestApplication&    application,
                         const Vector2&      position,
                         uint32_t            time,
                         Device::Class::Type deviceClass)
{
  application.ProcessEvent(MakeDeviceTouch(PointState::UP, position, time, deviceClass));
}

void ConnectSignals(DragAndDropDetector detector, SignalLog& log)
{
  detector.StartedSignal().Connect(&log, &SignalLog::OnStarted);
  detector.EnteredSignal().Connect(&log, &SignalLog::OnEntered);
  detector.MovedSignal().Connect(&log, &SignalLog::OnMoved);
  detector.ExitedSignal().Connect(&log, &SignalLog::OnExited);
  detector.DroppedSignal().Connect(&log, &SignalLog::OnDropped);
  detector.CancelledSignal().Connect(&log, &SignalLog::OnCancelled);
  detector.EndedSignal().Connect(&log, &SignalLog::OnEnded);
}

void ConnectLifecycleOrderSignals(DragAndDropDetector detector, LifecycleOrderLog& log)
{
  detector.StartedSignal().Connect(&log, &LifecycleOrderLog::OnStarted);
  detector.EnteredSignal().Connect(&log, &LifecycleOrderLog::OnEntered);
  detector.ExitedSignal().Connect(&log, &LifecycleOrderLog::OnExited);
  detector.DroppedSignal().Connect(&log, &LifecycleOrderLog::OnDropped);
  detector.CancelledSignal().Connect(&log, &LifecycleOrderLog::OnCancelled);
  detector.EndedSignal().Connect(&log, &LifecycleOrderLog::OnEnded);
}
} // namespace

void utc_dali_drag_and_drop_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_drag_and_drop_detector_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDragPayloadMultipleRepresentationsAndOperations(void)
{
  DragPayload payload{
    "application/x-item-metadata",
    Property::Value(7),
    DragAndDropOperation::LINK};
  payload.AddRepresentation("text/plain", Property::Value("seven"));
  payload.AddRepresentation("text/plain", Property::Value("updated"));
  payload.AddAllowedOperation(DragAndDropOperation::COPY);
  payload.AddAllowedOperation(DragAndDropOperation::COPY);

  DALI_TEST_EQUALS(payload.GetRepresentationCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(payload.GetRepresentationType(0u) == "application/x-item-metadata");
  DALI_TEST_CHECK(payload.GetRepresentationType(1u) == "text/plain");
  DALI_TEST_EQUALS(payload.GetRepresentationData(0u).Get<int32_t>(), 7, TEST_LOCATION);
  DALI_TEST_CHECK(payload.GetRepresentationData(1u).Get<Dali::String>() == "updated");
  DALI_TEST_CHECK(payload.HasRepresentation("text/plain"));
  DALI_TEST_CHECK(!payload.HasRepresentation("image/png"));

  Property::Value selectedData;
  DALI_TEST_CHECK(payload.GetRepresentationData("text/plain", selectedData));
  DALI_TEST_CHECK(selectedData.Get<Dali::String>() == "updated");
  DALI_TEST_CHECK(!payload.GetRepresentationData("image/png", selectedData));
  DALI_TEST_CHECK(selectedData.Get<Dali::String>() == "updated");

  DALI_TEST_EQUALS(payload.GetAllowedOperationCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(payload.GetAllowedOperation(0u) == DragAndDropOperation::LINK);
  DALI_TEST_CHECK(payload.GetAllowedOperation(1u) == DragAndDropOperation::COPY);
  DALI_TEST_CHECK(payload.GetPreferredOperation() == DragAndDropOperation::LINK);
  DALI_TEST_CHECK(payload.IsOperationAllowed(DragAndDropOperation::COPY));
  DALI_TEST_CHECK(!payload.IsOperationAllowed(DragAndDropOperation::MOVE));

  DragPayload copy = payload;
  DALI_TEST_CHECK(payload.RemoveRepresentation("application/x-item-metadata"));
  DALI_TEST_CHECK(!payload.RemoveRepresentation("application/x-item-metadata"));
  DALI_TEST_CHECK(payload.RemoveAllowedOperation(DragAndDropOperation::LINK));
  DALI_TEST_CHECK(payload.GetPreferredOperation() == DragAndDropOperation::COPY);
  payload.SetPreferredOperation(DragAndDropOperation::MOVE);
  DALI_TEST_CHECK(payload.IsOperationAllowed(DragAndDropOperation::MOVE));
  DALI_TEST_CHECK(payload.GetPreferredOperation() == DragAndDropOperation::MOVE);
  const DragAndDropOperation invalidOperation =
    static_cast<DragAndDropOperation>(255u);
  const uint32_t operationCount = payload.GetAllowedOperationCount();
  payload.AddAllowedOperation(DragAndDropOperation::NONE);
  payload.AddAllowedOperation(invalidOperation);
  payload.SetPreferredOperation(invalidOperation);
  DALI_TEST_EQUALS(payload.GetAllowedOperationCount(), operationCount, TEST_LOCATION);
  DALI_TEST_CHECK(payload.GetPreferredOperation() == DragAndDropOperation::MOVE);
  DALI_TEST_CHECK(!payload.IsOperationAllowed(invalidOperation));
  DALI_TEST_CHECK(!payload.RemoveAllowedOperation(invalidOperation));
  payload.ClearRepresentations();
  payload.ClearAllowedOperations();
  DALI_TEST_EQUALS(payload.GetRepresentationCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(payload.GetAllowedOperationCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(payload.GetPreferredOperation() == DragAndDropOperation::NONE);

  DALI_TEST_EQUALS(copy.GetRepresentationCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetAllowedOperationCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(copy.GetPreferredOperation() == DragAndDropOperation::LINK);

  DropProposal accepted =
    DropProposal::Accept("text/plain", DragAndDropOperation::COPY);
  DropProposal acceptedCopy = accepted;
  DALI_TEST_CHECK(accepted.IsAccepted());
  DALI_TEST_CHECK(accepted.GetSelectedRepresentationType() == "text/plain");
  DALI_TEST_CHECK(accepted.GetOperation() == DragAndDropOperation::COPY);
  DALI_TEST_CHECK(acceptedCopy.IsAccepted());
  DALI_TEST_CHECK(!DropProposal::Reject().IsAccepted());
  END_TEST;
}

int UtcDaliDragAndDropDetectorNegotiatesRepresentationAndOperation(void)
{
  UiTestApplication               application;
  DragAndDropDetector             detector = DragAndDropDetector::New();
  RepresentationProposalCallbacks proposals;
  SignalLog                       signals;
  View                            source = CreateView(Vector2::ZERO);
  View                            target = CreateView(Vector2(180.0f, 0.0f));
  DragPayload                     payload{
    "application/x-item-metadata",
    Property::Value(7),
    DragAndDropOperation::LINK};
  payload.AddRepresentation("text/plain", Property::Value("seven"));
  payload.AddAllowedOperation(DragAndDropOperation::COPY);

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDropProposalCallback(
    target,
    DragAndDropDetector::DropProposalCallback::New(
      &proposals,
      &RepresentationProposalCallbacks::Propose));
  ConnectSignals(detector, signals);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(detector.StartDrag(source, payload, Device::Class::KEYBOARD));
  proposals.mode = RepresentationProposalCallbacks::Mode::INVALID_TYPE;
  DALI_TEST_CHECK(!detector.MoveDragTo(target));
  proposals.mode = RepresentationProposalCallbacks::Mode::INVALID_OPERATION;
  DALI_TEST_CHECK(!detector.MoveDragTo(target));
  proposals.mode = RepresentationProposalCallbacks::Mode::SELECT_ALTERNATE;
  DALI_TEST_CHECK(detector.MoveDragTo(target));
  DALI_TEST_CHECK(detector.GetDragTarget() == target);
  DALI_TEST_CHECK(detector.Drop());

  DALI_TEST_EQUALS(proposals.candidateEvents.size(), 3u, TEST_LOCATION);
  for(const DragAndDropEvent& event : proposals.candidateEvents)
  {
    DALI_TEST_CHECK(event.GetCandidateTarget() == target);
    DALI_TEST_CHECK(!event.GetTarget());
    DALI_TEST_CHECK(!event.GetDropProposal().IsAccepted());
    Property::Value rejectedData(99);
    DALI_TEST_CHECK(!event.GetSelectedRepresentationData(rejectedData));
    DALI_TEST_EQUALS(rejectedData.Get<int32_t>(), 99, TEST_LOCATION);
  }
  uint32_t concreteProposalEventCount = 0u;
  for(const DragAndDropEvent& event : signals.events)
  {
    if(event.GetTarget() == target && event.GetDropProposal().IsAccepted())
    {
      ++concreteProposalEventCount;
      DALI_TEST_CHECK(
        event.GetDropProposal().GetSelectedRepresentationType() == "text/plain");
      DALI_TEST_CHECK(
        event.GetDropProposal().GetOperation() == DragAndDropOperation::COPY);
    }
  }
  DALI_TEST_EQUALS(concreteProposalEventCount, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.terminalEvents.size(), 1u, TEST_LOCATION);
  const DropProposal& selected = signals.terminalEvents[0].GetDropProposal();
  DALI_TEST_CHECK(selected.IsAccepted());
  DALI_TEST_CHECK(selected.GetSelectedRepresentationType() == "text/plain");
  DALI_TEST_CHECK(selected.GetOperation() == DragAndDropOperation::COPY);
  Property::Value selectedData;
  DALI_TEST_CHECK(
    signals.terminalEvents[0].GetSelectedRepresentationData(selectedData));
  DALI_TEST_CHECK(selectedData.Get<Dali::String>() == "seven");

  detector.ClearDropProposalCallback(target);
  DALI_TEST_CHECK(detector.StartDrag(source, payload, Device::Class::KEYBOARD));
  DALI_TEST_CHECK(detector.MoveDragTo(target));
  DALI_TEST_CHECK(detector.Drop());
  DALI_TEST_EQUALS(signals.terminalEvents.size(), 2u, TEST_LOCATION);
  const DropProposal& defaultProposal = signals.terminalEvents[1].GetDropProposal();
  DALI_TEST_CHECK(defaultProposal.IsAccepted());
  DALI_TEST_CHECK(
    defaultProposal.GetSelectedRepresentationType() == "application/x-item-metadata");
  DALI_TEST_CHECK(defaultProposal.GetOperation() == DragAndDropOperation::LINK);
  DALI_TEST_CHECK(
    signals.terminalEvents[1].GetSelectedRepresentationData(selectedData));
  DALI_TEST_EQUALS(selectedData.Get<int32_t>(), 7, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorRegistration(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = View::New();
  View                target   = View::New();

  detector.AttachSource(source);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.AttachTarget(target);

  DALI_TEST_EQUALS(detector.GetAttachedSourceCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(detector.GetAttachedSource(0u) == source);
  DALI_TEST_CHECK(detector.GetAttachedTarget(0u) == target);
  DALI_TEST_CHECK(!detector.GetAttachedSource(1u));
  DALI_TEST_CHECK(!detector.GetAttachedTarget(1u));

  detector.DetachSource(source);
  detector.DetachTarget(target);
  DALI_TEST_EQUALS(detector.GetAttachedSourceCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!detector.GetAttachedSource(0u));
  DALI_TEST_CHECK(!detector.GetAttachedTarget(0u));
  DALI_TEST_EQUALS(detector.GetDragStartThreshold(), 0.0f, TEST_LOCATION);
  DALI_TEST_CHECK(detector.GetDragActivationMode() == DragActivationMode::PAN);
  DALI_TEST_CHECK(!detector.IsDragActivationPending());
  detector.SetDragStartThreshold(48.0f);
  DALI_TEST_EQUALS(detector.GetDragStartThreshold(), 48.0f, TEST_LOCATION);
  DALI_TEST_ASSERTION(detector.SetDragStartThreshold(-1.0f), "must be non-negative");
  detector.SetDragActivationMode(DragActivationMode::LONG_PRESS);
  DALI_TEST_CHECK(detector.GetDragActivationMode() == DragActivationMode::LONG_PRESS);
  DALI_TEST_ASSERTION(
    detector.SetDragActivationMode(static_cast<DragActivationMode>(255u)),
    "Invalid drag activation mode");
  END_TEST;
}

int UtcDaliDragAndDropDetectorDeviceActivationConfiguration(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();

  DALI_TEST_CHECK(!detector.HasDragActivationConfiguration(Device::Class::MOUSE));
  DragActivationConfiguration configuration =
    detector.GetDragActivationConfiguration(Device::Class::MOUSE);
  DALI_TEST_CHECK(configuration.GetMode() == DragActivationMode::PAN);
  DALI_TEST_EQUALS(configuration.GetMovementThreshold(), 0.0f, TEST_LOCATION);

  detector.SetDragActivationConfiguration(
    Device::Class::MOUSE,
    {DragActivationMode::PAN, 64.0f});
  detector.SetDragActivationConfiguration(
    Device::Class::TOUCH,
    {DragActivationMode::LONG_PRESS, 0.0f});
  DALI_TEST_CHECK(detector.HasDragActivationConfiguration(Device::Class::MOUSE));
  DALI_TEST_CHECK(detector.HasDragActivationConfiguration(Device::Class::TOUCH));
  configuration = detector.GetDragActivationConfiguration(Device::Class::MOUSE);
  DALI_TEST_CHECK(configuration.GetMode() == DragActivationMode::PAN);
  DALI_TEST_EQUALS(configuration.GetMovementThreshold(), 64.0f, TEST_LOCATION);
  configuration = detector.GetDragActivationConfiguration(Device::Class::TOUCH);
  DALI_TEST_CHECK(configuration.GetMode() == DragActivationMode::LONG_PRESS);
  DALI_TEST_EQUALS(configuration.GetMovementThreshold(), 0.0f, TEST_LOCATION);

  detector.SetDragActivationMode(DragActivationMode::LONG_PRESS);
  detector.SetDragStartThreshold(12.0f);
  configuration = detector.GetDragActivationConfiguration(Device::Class::PEN);
  DALI_TEST_CHECK(configuration.GetMode() == DragActivationMode::LONG_PRESS);
  DALI_TEST_EQUALS(configuration.GetMovementThreshold(), 12.0f, TEST_LOCATION);

  detector.ClearDragActivationConfiguration(Device::Class::MOUSE);
  DALI_TEST_CHECK(!detector.HasDragActivationConfiguration(Device::Class::MOUSE));
  configuration = detector.GetDragActivationConfiguration(Device::Class::MOUSE);
  DALI_TEST_CHECK(configuration.GetMode() == DragActivationMode::LONG_PRESS);
  DALI_TEST_EQUALS(configuration.GetMovementThreshold(), 12.0f, TEST_LOCATION);
  detector.ClearDragActivationConfiguration(Device::Class::MOUSE);

  DALI_TEST_ASSERTION(
    detector.SetDragActivationConfiguration(
      Device::Class::TOUCH,
      {DragActivationMode::PAN, -1.0f}),
    "must be non-negative");
  DALI_TEST_ASSERTION(
    detector.SetDragActivationConfiguration(
      Device::Class::TOUCH,
      {static_cast<DragActivationMode>(255u), 0.0f}),
    "Invalid drag activation mode");
  DALI_TEST_ASSERTION(
    detector.GetDragActivationConfiguration(static_cast<Device::Class::Type>(255)),
    "Invalid input device class");
  END_TEST;
}

int UtcDaliDragAndDropDetectorSelectsDeviceActivationConfiguration(void)
{
  UiTestApplication      application;
  DragAndDropDetector    detector = DragAndDropDetector::New();
  View                   source   = CreateView(Vector2::ZERO);
  ActivationGate         gate;
  SessionVisualCallbacks visuals;

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragActivationConfiguration(
    Device::Class::MOUSE,
    {DragActivationMode::PAN, 64.0f});
  detector.SetDragActivationConfiguration(
    Device::Class::TOUCH,
    {DragActivationMode::LONG_PRESS, 0.0f});
  gate.allow = true;
  detector.SetCanStartDragCallback(
    DragAndDropDetector::CanStartDragCallback::New(&gate, &ActivationGate::Approve));
  detector.SetDragPreviewCallbacks(
    DragAndDropDetector::DragPreviewFactory::New(&visuals, &SessionVisualCallbacks::Create),
    DragAndDropDetector::DragPreviewUpdater::New(&visuals, &SessionVisualCallbacks::Update),
    DragAndDropDetector::DragPreviewFinalizer::New(&visuals, &SessionVisualCallbacks::Finalize));

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  StartPanForDevice(
    application,
    Vector2(10.0f, 10.0f),
    Vector2(30.0f, 10.0f),
    time,
    Device::Class::MOUSE);
  DALI_TEST_CHECK(detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 0, TEST_LOCATION);
  DALI_TEST_ASSERTION(
    detector.SetDragActivationConfiguration(
      Device::Class::MOUSE,
      {DragActivationMode::PAN, 0.0f}),
    "cannot be changed");

  MovePanForDevice(application, Vector2(80.0f, 10.0f), time, Device::Class::MOUSE);
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 1, TEST_LOCATION);
  DALI_TEST_CHECK(gate.mode == DragActivationMode::PAN);
  DALI_TEST_CHECK(gate.deviceClass == Device::Class::MOUSE);
  DALI_TEST_EQUALS(visuals.factoryEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(visuals.factoryEvents.back().GetDeviceClass() == Device::Class::MOUSE);
  EndGestureForDevice(
    application,
    Vector2(80.0f, 10.0f),
    time + TestGetFrameInterval(),
    Device::Class::MOUSE);
  DALI_TEST_CHECK(!detector.IsDragging());

  time += 100u;
  StartPanForDevice(
    application,
    Vector2(10.0f, 10.0f),
    Vector2(30.0f, 10.0f),
    time,
    Device::Class::TOUCH);
  MovePanForDevice(application, Vector2(80.0f, 10.0f), time, Device::Class::TOUCH);
  EndGestureForDevice(
    application,
    Vector2(80.0f, 10.0f),
    time + TestGetFrameInterval(),
    Device::Class::TOUCH);
  DALI_TEST_CHECK(!detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 1, TEST_LOCATION);

  time += 100u;
  application.ProcessEvent(
    MakeDeviceTouch(PointState::DOWN, Vector2(10.0f, 10.0f), time, Device::Class::TOUCH));
  const std::size_t updatesBeforeLongPress = visuals.updateEvents.size();
  TestTriggerLongPress(application);
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 2, TEST_LOCATION);
  DALI_TEST_CHECK(gate.mode == DragActivationMode::LONG_PRESS);
  DALI_TEST_CHECK(gate.deviceClass == Device::Class::TOUCH);
  DALI_TEST_EQUALS(visuals.factoryEvents.size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(visuals.factoryEvents.back().GetDeviceClass() == Device::Class::TOUCH);
  DALI_TEST_CHECK(visuals.updateEvents.size() > updatesBeforeLongPress);
  DALI_TEST_CHECK(visuals.visuals.back().GetParent() == source.GetParent());
  EndGestureForDevice(
    application,
    Vector2(10.0f, 10.0f),
    time + 500u,
    Device::Class::TOUCH);

  DALI_TEST_CHECK(!detector.IsDragging());

  detector.ClearDragActivationConfiguration(Device::Class::TOUCH);
  time += 700u;
  StartPanForDevice(
    application,
    Vector2(10.0f, 10.0f),
    Vector2(30.0f, 10.0f),
    time,
    Device::Class::TOUCH);
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 3, TEST_LOCATION);
  DALI_TEST_CHECK(gate.mode == DragActivationMode::PAN);
  DALI_TEST_CHECK(gate.deviceClass == Device::Class::TOUCH);
  EndGestureForDevice(
    application,
    Vector2(30.0f, 10.0f),
    time + TestGetFrameInterval(),
    Device::Class::TOUCH);

  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.started, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 3, TEST_LOCATION);
  detector.ClearCanStartDragCallback();
  detector.ClearDragPreviewCallbacks();
  END_TEST;
}

int UtcDaliDragAndDropDetectorDragStartThreshold(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2(0.0f, 0.0f));
  View                target   = CreateView(Vector2(180.0f, 0.0f));
  View                visual   = CreateView(Vector2::ZERO);
  visual.SetProperty(Actor::Property::SENSITIVE, false);

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDragPreview(visual);
  detector.SetDragStartThreshold(80.0f);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!visual.GetParent());

  TestMovePan(application, Vector2(60.0f, 10.0f), time);
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);

  TestMovePan(application, Vector2(95.0f, 10.0f), time + TestGetFrameInterval());
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(log.started, 1, TEST_LOCATION);
  DALI_TEST_CHECK(visual.GetParent());

  TestMovePan(application, Vector2(210.0f, 10.0f), time + 2u * TestGetFrameInterval());
  TestEndPan(application, Vector2(210.0f, 10.0f), time + 3u * TestGetFrameInterval());

  DALI_TEST_EQUALS(log.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.dropped, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_CHECK(!visual.GetParent());
  END_TEST;
}

int UtcDaliDragAndDropDetectorThresholdReleaseDoesNotStartSession(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2(0.0f, 0.0f));
  View                visual   = CreateView(Vector2::ZERO);
  visual.SetProperty(Actor::Property::SENSITIVE, false);

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragPreview(visual);
  detector.SetDragStartThreshold(200.0f);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestEndPan(application, Vector2(30.0f, 10.0f), time);

  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_CHECK(!visual.GetParent());

  detector.SetDragStartThreshold(0.0f);
  time += 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(detector.IsDragging());
  TestEndPan(application, Vector2(30.0f, 10.0f), time);
  DALI_TEST_EQUALS(log.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorPendingActivationInterruptions(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2(0.0f, 0.0f));

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragStartThreshold(200.0f);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestAddSecondTouchToPan(application, Vector2(30.0f, 10.0f), Vector2(40.0f, 30.0f), time);

  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!detector.IsDragging());
  detector.SetDragStartThreshold(32.0f);
  TestCancelPan(application, Vector2(30.0f, 10.0f), time + TestGetFrameInterval());

  detector.SetDragStartThreshold(200.0f);
  time += 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  source.Unparent();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!detector.IsDragging());
  detector.SetDragStartThreshold(0.0f);
  END_TEST;
}

int UtcDaliDragAndDropDetectorLongPressActivation(void)
{
  UiTestApplication      application;
  DragAndDropDetector    detector = DragAndDropDetector::New();
  View                   source   = CreateView(Vector2(0.0f, 0.0f));
  SessionVisualCallbacks visuals;

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetSourcePayload(
    source,
    {"application/x-test", Property::Value(7), DragAndDropOperation::MOVE});
  detector.SetDragActivationConfiguration(
    Device::Class::MOUSE,
    {DragActivationMode::LONG_PRESS, 0.0f});
  detector.SetDragPreviewCallbacks(
    DragAndDropDetector::DragPreviewFactory::New(&visuals, &SessionVisualCallbacks::Create),
    DragAndDropDetector::DragPreviewUpdater::New(&visuals, &SessionVisualCallbacks::Update),
    DragAndDropDetector::DragPreviewFinalizer::New(&visuals, &SessionVisualCallbacks::Finalize));

  ActivationGate gate;
  gate.allow = true;
  detector.SetCanStartDragCallback(
    DragAndDropDetector::CanStartDragCallback::New(&gate, &ActivationGate::Approve));

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(
    MakeDeviceTouch(PointState::DOWN, Vector2(10.0f, 10.0f), 100u, Device::Class::MOUSE));
  DALI_TEST_CHECK(!detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 0, TEST_LOCATION);

  // The long-press timer alone must create, attach, and position the preview.
  // No mouse motion is sent before these assertions.
  TestTriggerLongPress(application);
  DALI_TEST_CHECK(!detector.IsDragActivationPending());
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 1, TEST_LOCATION);
  DALI_TEST_CHECK(gate.mode == DragActivationMode::LONG_PRESS);
  DALI_TEST_EQUALS(gate.payload, 7, TEST_LOCATION);
  DALI_TEST_CHECK(gate.source == source);
  DALI_TEST_EQUALS(log.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(visuals.factoryEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!visuals.updateEvents.empty());
  DALI_TEST_EQUALS(visuals.visuals.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(visuals.visuals.back().GetParent() == source.GetParent());
  DALI_TEST_EQUALS(visuals.updateEvents.front().GetScreenPosition(), Vector2(10.0f, 10.0f), TEST_LOCATION);

  EndGestureForDevice(
    application,
    Vector2(10.0f, 10.0f),
    600u,
    Device::Class::MOUSE);
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  detector.ClearCanStartDragCallback();
  detector.ClearDragPreviewCallbacks();
  END_TEST;
}

int UtcDaliDragAndDropDetectorLongPressModeRejectsPanWithoutHold(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2(0.0f, 0.0f));

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragActivationMode(DragActivationMode::LONG_PRESS);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(80.0f, 10.0f), time);
  TestEndPan(application, Vector2(80.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_CHECK(!detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorLongPressPanHandoff(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2(0.0f, 0.0f));
  View                target   = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDragActivationMode(DragActivationMode::LONG_PRESS);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  TestStartLongPress(application, 10.0f, 10.0f, 100u);
  TestTriggerLongPress(application);
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(log.started, 1, TEST_LOCATION);

  TestMovePan(application, Vector2(30.0f, 10.0f), 200u);
  TestMovePan(application, Vector2(30.0f, 10.0f), 200u + TestGetFrameInterval());
  TestMovePan(application, Vector2(210.0f, 10.0f), 200u + 2u * TestGetFrameInterval());
  TestEndPan(application, Vector2(210.0f, 10.0f), 200u + 3u * TestGetFrameInterval());

  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.dropped, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorCustomActivationCallback(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2(0.0f, 0.0f));

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetSourcePayload(
    source,
    {"application/x-test", Property::Value(42), DragAndDropOperation::MOVE});
  detector.SetDragStartThreshold(40.0f);

  ActivationGate gate;
  detector.SetCanStartDragCallback(
    DragAndDropDetector::CanStartDragCallback::New(&gate, &ActivationGate::Approve));

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 0, TEST_LOCATION);

  TestMovePan(application, Vector2(55.0f, 10.0f), time);
  DALI_TEST_CHECK(detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 1, TEST_LOCATION);
  DALI_TEST_CHECK(gate.mode == DragActivationMode::PAN);
  DALI_TEST_EQUALS(gate.payload, 42, TEST_LOCATION);
  DALI_TEST_CHECK(gate.displacement.Length() >= 40.0f);

  gate.allow = true;
  TestMovePan(application, Vector2(60.0f, 10.0f), time + TestGetFrameInterval());
  DALI_TEST_CHECK(!detector.IsDragActivationPending());
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(gate.calls, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(log.started, 1, TEST_LOCATION);

  TestEndPan(application, Vector2(60.0f, 10.0f), time + 2u * TestGetFrameInterval());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  detector.ClearCanStartDragCallback();
  END_TEST;
}

int UtcDaliDragAndDropDetectorPendingActivationCanBeCancelled(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2(0.0f, 0.0f));

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragStartThreshold(200.0f);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(detector.IsDragActivationPending());
  detector.CancelDrag();

  DALI_TEST_CHECK(!detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 0, TEST_LOCATION);

  detector.SetDragStartThreshold(0.0f);
  TestMovePan(application, Vector2(90.0f, 10.0f), time);
  TestEndPan(application, Vector2(90.0f, 10.0f), time + TestGetFrameInterval());
  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorActivationCallbackCanCancelPending(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2(0.0f, 0.0f));

  application.GetScene().Add(source);
  detector.AttachSource(source);

  ActivationGate gate;
  gate.allow         = true;
  gate.cancelPending = true;
  gate.detector      = detector;
  detector.SetCanStartDragCallback(
    DragAndDropDetector::CanStartDragCallback::New(&gate, &ActivationGate::Approve));

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);

  DALI_TEST_EQUALS(gate.calls, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(log.started, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 0, TEST_LOCATION);

  TestEndPan(application, Vector2(30.0f, 10.0f), time);
  detector.ClearCanStartDragCallback();
  END_TEST;
}

int UtcDaliDragAndDropDetectorDropLifecycle(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector   = DragAndDropDetector::New();
  View                source     = CreateView(Vector2(0.0f, 0.0f));
  View                target     = CreateView(Vector2(180.0f, 0.0f));
  View                visual     = CreateView(Vector2::ZERO);
  View                nextVisual = CreateView(Vector2::ZERO);
  visual.SetProperty(Actor::Property::SENSITIVE, false);
  nextVisual.SetProperty(Actor::Property::SENSITIVE, false);

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDragPreview(visual);

  SignalLog log;
  ConnectSignals(detector, log);
  ReplaceVisualOnDrop replaceVisual(nextVisual);
  detector.DroppedSignal().Connect(&replaceVisual, &ReplaceVisualOnDrop::OnDropped);
  SessionQueryLog queryLog;
  detector.DroppedSignal().Connect(&queryLog, &SessionQueryLog::OnDropped);
  detector.EndedSignal().Connect(&queryLog, &SessionQueryLog::OnEnded);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(log.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.entered, 1, TEST_LOCATION);
  DALI_TEST_CHECK(log.moved >= 1);
  DALI_TEST_EQUALS(log.dropped, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.terminalEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(log.terminalEvents[0].GetResult() == DragAndDropResult::DROPPED);
  DALI_TEST_CHECK(log.terminalEvents[0].GetSource() == source);
  DALI_TEST_CHECK(log.terminalEvents[0].GetTarget() == target);
  DALI_TEST_CHECK(queryLog.draggingInDrop);
  DALI_TEST_CHECK(queryLog.sourceInDrop == source);
  DALI_TEST_CHECK(queryLog.targetInDrop == target);
  DALI_TEST_CHECK(!queryLog.draggingInEnded);
  DALI_TEST_CHECK(!queryLog.sourceInEnded);
  DALI_TEST_CHECK(!queryLog.targetInEnded);
  DALI_TEST_CHECK(!visual.GetParent());
  DALI_TEST_CHECK(!nextVisual.GetParent());
  END_TEST;
}

int UtcDaliDragAndDropDetectorDragPreviewContainer(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector     = DragAndDropDetector::New();
  View                sourceParent = CreateView(Vector2::ZERO);
  View                source       = CreateView(Vector2::ZERO);
  View                overlay      = CreateView(Vector2(400.0f, 100.0f));
  View                visual       = View::New();

  sourceParent.Add(source);
  overlay.SetProperty(Actor::Property::SENSITIVE, false);
  visual.SetProperty(Actor::Property::SENSITIVE, false);
  application.GetScene().Add(sourceParent);
  application.GetScene().Add(overlay);
  detector.AttachSource(source);
  detector.SetDragPreview(visual);
  detector.SetDragPreviewContainer(overlay);

  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(visual.GetParent() == overlay);
  TestEndPan(application, Vector2(40.0f, 10.0f), time + TestGetFrameInterval());
  DALI_TEST_CHECK(!visual.GetParent());

  detector.SetDragPreviewContainer(View());
  time = 300u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(visual.GetParent() == sourceParent);
  TestEndPan(application, Vector2(40.0f, 10.0f), time + TestGetFrameInterval());
  DALI_TEST_CHECK(!visual.GetParent());
  END_TEST;
}

int UtcDaliDragAndDropDetectorTransformSafeVisualAnchor(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector     = DragAndDropDetector::New();
  View                sourceParent = CreateView(Vector2(70.0f, 90.0f));
  View                source       = CreateView(Vector2(30.0f, 20.0f));
  View                overlay      = CreateView(Vector2(260.0f, 140.0f));
  View                visual       = View::New();

  sourceParent.SetRequestedWidth(240.0f);
  sourceParent.SetRequestedHeight(220.0f);
  sourceParent.SetProperty(Actor::Property::SCALE, Vector3(1.15f, 0.9f, 1.0f));
  sourceParent.SetProperty(
    Actor::Property::ORIENTATION,
    Quaternion(Radian(Degree(12.0f)), Vector3::ZAXIS));
  source.SetRequestedWidth(120.0f);
  source.SetRequestedHeight(80.0f);
  source.SetProperty(Actor::Property::PIVOT, Vector3(0.3f, 0.65f, 0.5f));

  overlay.SetRequestedWidth(320.0f);
  overlay.SetRequestedHeight(280.0f);
  overlay.SetProperty(Actor::Property::SENSITIVE, false);
  overlay.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  overlay.SetProperty(Actor::Property::SCALE, Vector3(0.8f, 1.2f, 1.0f));
  overlay.SetProperty(
    Actor::Property::ORIENTATION,
    Quaternion(Radian(Degree(-18.0f)), Vector3::ZAXIS));

  const Vector3       originalPivot(0.8f, 0.2f, 0.5f);
  const Vector3       originalParentOrigin  = ParentOrigin::CENTER;
  const LayoutMode    originalLayoutMode    = visual.GetLayoutMode();
  const UiScalePolicy originalUiScalePolicy = visual.GetUiScalePolicy();
  const float         originalRequestedX    = 17.0f;
  const float         originalRequestedY    = 23.0f;
  const bool          originalPositionUsesPivot =
    visual.GetProperty<bool>(Actor::Property::POSITION_USES_PIVOT);
  visual.SetRequestedWidth(150.0f);
  visual.SetRequestedHeight(60.0f);
  visual.SetRequestedX(originalRequestedX);
  visual.SetRequestedY(originalRequestedY);
  visual.SetProperty(Actor::Property::SENSITIVE, false);
  visual.SetProperty(Actor::Property::PIVOT, originalPivot);
  visual.SetProperty(Actor::Property::PARENT_ORIGIN, originalParentOrigin);
  visual.SetProperty(Actor::Property::SCALE, Vector3(1.1f, 0.75f, 1.0f));
  visual.SetProperty(
    Actor::Property::ORIENTATION,
    Quaternion(Radian(Degree(24.0f)), Vector3::ZAXIS));

  sourceParent.Add(source);
  application.GetScene().Add(sourceParent);
  application.GetScene().Add(overlay);
  detector.AttachSource(source);
  detector.SetDragPreview(visual);
  detector.SetDragPreviewContainer(overlay);

  application.SendNotification();
  application.Render();

  const Vector2 start        = source.CalculateScreenPosition();
  const Vector2 firstPointer = start + Vector2(18.0f, 12.0f);
  uint32_t      time         = 100u;
  TestStartPan(application, start, firstPointer, time);
  application.SendNotification();
  application.Render();

  float sourceLocalX = 0.0f;
  float sourceLocalY = 0.0f;
  DALI_TEST_CHECK(source.ScreenToLocal(
    sourceLocalX, sourceLocalY, firstPointer.x, firstPointer.y));
  const Vector3 sourceSize = source.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  const Vector2 expectedAnchor(
    std::clamp(sourceLocalX / sourceSize.x, 0.0f, 1.0f),
    std::clamp(sourceLocalY / sourceSize.y, 0.0f, 1.0f));

  DALI_TEST_CHECK(visual.GetParent() == overlay);
  const Vector3 appliedPivot = visual.GetProperty<Vector3>(Actor::Property::PIVOT);
  DALI_TEST_EQUALS(appliedPivot.x, expectedAnchor.x, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(appliedPivot.y, expectedAnchor.y, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(
    visual.GetProperty<Vector3>(Actor::Property::PARENT_ORIGIN),
    ParentOrigin::TOP_LEFT,
    0.01f,
    TEST_LOCATION);
  float visualLocalX = 0.0f;
  float visualLocalY = 0.0f;
  DALI_TEST_CHECK(visual.ScreenToLocal(
    visualLocalX, visualLocalY, firstPointer.x, firstPointer.y));
  const Vector3 visualSize = visual.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(
    visualLocalX, expectedAnchor.x * visualSize.x, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(
    visualLocalY, expectedAnchor.y * visualSize.y, 0.1f, TEST_LOCATION);

  const Vector2 movedPointer = firstPointer + Vector2(95.0f, 65.0f);
  TestMovePan(application, movedPointer, time);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(visual.ScreenToLocal(
    visualLocalX, visualLocalY, movedPointer.x, movedPointer.y));
  DALI_TEST_EQUALS(
    visualLocalX, expectedAnchor.x * visualSize.x, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(
    visualLocalY, expectedAnchor.y * visualSize.y, 0.1f, TEST_LOCATION);

  TestEndPan(application, movedPointer, time + TestGetFrameInterval());
  DALI_TEST_CHECK(!visual.GetParent());
  DALI_TEST_EQUALS(
    visual.GetProperty<Vector3>(Actor::Property::PIVOT),
    originalPivot,
    0.01f,
    TEST_LOCATION);
  DALI_TEST_EQUALS(
    visual.GetProperty<Vector3>(Actor::Property::PARENT_ORIGIN),
    originalParentOrigin,
    0.01f,
    TEST_LOCATION);
  DALI_TEST_CHECK(visual.GetLayoutMode() == originalLayoutMode);
  DALI_TEST_CHECK(visual.GetUiScalePolicy() == originalUiScalePolicy);
  DALI_TEST_EQUALS(visual.GetRequestedX(), originalRequestedX, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetRequestedY(), originalRequestedY, 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(
    visual.GetProperty<bool>(Actor::Property::POSITION_USES_PIVOT) ==
    originalPositionUsesPivot);
  END_TEST;
}

int UtcDaliDragAndDropDetectorDetachCancels(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  application.GetScene().Add(source);
  detector.AttachSource(source);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  detector.DetachSource(source);

  DALI_TEST_EQUALS(log.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
  DALI_TEST_CHECK(log.cancelReasons.back() ==
                  DragCancelReason::REGISTRATION_REMOVED);
  DALI_TEST_EQUALS(log.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorExitCallbackCanDetachTarget(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                target   = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);

  SignalLog signalLog;
  ConnectSignals(detector, signalLog);
  DetachOnExit detachOnExit;
  detector.ExitedSignal().Connect(&detachOnExit, &DetachOnExit::OnExited);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestMovePan(application, Vector2(350.0f, 10.0f), time);
  TestEndPan(application, Vector2(350.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(signalLog.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.exited, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(detachOnExit.exited, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.ended, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorOverlappingTargetsFollowSceneOrder(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector    = DragAndDropDetector::New();
  View                source      = CreateView(Vector2::ZERO);
  View                lowerTarget = CreateView(Vector2(180.0f, 0.0f));
  View                upperTarget = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(lowerTarget);
  application.GetScene().Add(upperTarget);
  detector.AttachSource(source);
  // Register in the opposite order to prove scene order wins.
  detector.AttachTarget(upperTarget);
  detector.AttachTarget(lowerTarget);

  TargetLog targetLog;
  detector.EnteredSignal().Connect(&targetLog, &TargetLog::OnEntered);
  detector.ExitedSignal().Connect(&targetLog, &TargetLog::OnExited);
  detector.DroppedSignal().Connect(&targetLog, &TargetLog::OnDropped);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  DALI_TEST_CHECK(targetLog.entered == upperTarget);

  lowerTarget.RaiseToTop();
  TestMovePan(application, Vector2(211.0f, 10.0f), time);
  TestEndPan(application, Vector2(211.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_CHECK(targetLog.exited == upperTarget);
  DALI_TEST_CHECK(targetLog.entered == lowerTarget);
  DALI_TEST_CHECK(targetLog.dropped == lowerTarget);
  DALI_TEST_EQUALS(targetLog.enteredCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(targetLog.exitedCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorNestedTargetsPreferDescendant(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector     = DragAndDropDetector::New();
  View                source       = CreateView(Vector2::ZERO);
  View                parentTarget = CreateView(Vector2(180.0f, 0.0f));
  View                childTarget  = CreateView(Vector2(10.0f, 10.0f));

  parentTarget.SetRequestedWidth(140.0f);
  parentTarget.SetRequestedHeight(140.0f);
  parentTarget.Add(childTarget);
  application.GetScene().Add(source);
  application.GetScene().Add(parentTarget);
  detector.AttachSource(source);
  // Register the parent last to prove hierarchy depth wins.
  detector.AttachTarget(childTarget);
  detector.AttachTarget(parentTarget);

  TargetLog targetLog;
  detector.EnteredSignal().Connect(&targetLog, &TargetLog::OnEntered);
  detector.DroppedSignal().Connect(&targetLog, &TargetLog::OnDropped);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 20.0f), time);
  TestEndPan(application, Vector2(210.0f, 20.0f), time + TestGetFrameInterval());

  DALI_TEST_CHECK(targetLog.entered == childTarget);
  DALI_TEST_CHECK(targetLog.dropped == childTarget);
  END_TEST;
}

int UtcDaliDragAndDropDetectorTargetRequiresEligibleAncestors(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector    = DragAndDropDetector::New();
  View                source      = CreateView(Vector2::ZERO);
  View                parent      = CreateView(Vector2(180.0f, 0.0f));
  View                childTarget = CreateView(Vector2(10.0f, 10.0f));

  parent.SetRequestedWidth(140.0f);
  parent.SetRequestedHeight(140.0f);
  parent.SetProperty(Actor::Property::SENSITIVE, false);
  parent.Add(childTarget);
  application.GetScene().Add(source);
  application.GetScene().Add(parent);
  detector.AttachSource(source);
  detector.AttachTarget(childTarget);

  SignalLog signalLog;
  ConnectSignals(detector, signalLog);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 20.0f), time);
  TestEndPan(application, Vector2(210.0f, 20.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(signalLog.entered, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.ended, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorGestureCancellationLifecycle(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                target   = CreateView(Vector2(180.0f, 0.0f));
  View                visual   = CreateView(Vector2::ZERO);

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDragPreview(visual);

  SignalLog signalLog;
  ConnectSignals(detector, signalLog);
  LifecycleOrderLog orderLog;
  ConnectLifecycleOrderSignals(detector, orderLog);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestCancelPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(signalLog.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.exited, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.cancelled, 1, TEST_LOCATION);
  DALI_TEST_CHECK(signalLog.cancelReasons.back() ==
                  DragCancelReason::GESTURE_INTERRUPTED);
  DALI_TEST_EQUALS(signalLog.ended, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.terminalEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(signalLog.terminalEvents[0].GetResult() == DragAndDropResult::CANCELLED);
  DALI_TEST_CHECK(
    signalLog.terminalEvents[0].GetCancelReason() ==
    DragCancelReason::GESTURE_INTERRUPTED);
  DALI_TEST_CHECK(!visual.GetParent());
  const std::vector<LifecycleOrderLog::Event> expectedOrder{
    LifecycleOrderLog::Event::STARTED,
    LifecycleOrderLog::Event::ENTERED,
    LifecycleOrderLog::Event::EXITED,
    LifecycleOrderLog::Event::CANCELLED,
    LifecycleOrderLog::Event::ENDED};
  DALI_TEST_CHECK(orderLog.events == expectedOrder);
  END_TEST;
}

int UtcDaliDragAndDropDetectorSecondTouchCancelsOwningPan(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                visual   = CreateView(Vector2::ZERO);

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragPreview(visual);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestAddSecondTouchToPan(
    application,
    Vector2(40.0f, 10.0f),
    Vector2(40.0f, 50.0f),
    time + TestGetFrameInterval());

  DALI_TEST_EQUALS(log.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(log.cancelReasons.back() ==
                  DragCancelReason::GESTURE_INTERRUPTED);
  DALI_TEST_CHECK(!visual.GetParent());
  END_TEST;
}

int UtcDaliDragAndDropDetectorSourceSceneDisconnectionCancels(void)
{
  UiTestApplication      application;
  SessionVisualCallbacks callbacks;
  DragAndDropDetector    detector = DragAndDropDetector::New();
  View                   source   = CreateView(Vector2::ZERO);

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragPreviewCallbacks(
    DragAndDropDetector::DragPreviewFactory::New(&callbacks, &SessionVisualCallbacks::Create),
    DragAndDropDetector::DragPreviewUpdater::New(&callbacks, &SessionVisualCallbacks::Update),
    DragAndDropDetector::DragPreviewFinalizer::New(&callbacks, &SessionVisualCallbacks::Finalize));

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  application.GetScene().Remove(source);

  DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(log.cancelReasons.back() ==
                  DragCancelReason::SOURCE_DISCONNECTED);
  DALI_TEST_EQUALS(callbacks.finalizerEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(callbacks.finalizerEvents[0].GetCancelReason() ==
                  DragCancelReason::SOURCE_DISCONNECTED);
  DALI_TEST_CHECK(callbacks.visualsWereDetached);
  END_TEST;
}

int UtcDaliDragAndDropDetectorVisualContainerSceneDisconnectionCancels(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                overlay  = CreateView(Vector2(200.0f, 100.0f));
  View                visual   = CreateView(Vector2::ZERO);

  overlay.SetProperty(Actor::Property::SENSITIVE, false);
  visual.SetProperty(Actor::Property::SENSITIVE, false);
  application.GetScene().Add(source);
  application.GetScene().Add(overlay);
  detector.AttachSource(source);
  detector.SetDragPreview(visual);
  detector.SetDragPreviewContainer(overlay);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(visual.GetParent() == overlay);
  application.GetScene().Remove(overlay);

  DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(log.cancelReasons.back() ==
                  DragCancelReason::PREVIEW_CONTAINER_DISCONNECTED);
  DALI_TEST_CHECK(!visual.GetParent());
  END_TEST;
}

int UtcDaliDragAndDropDetectorWindowFocusLossCancels(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                visual   = CreateView(Vector2::ZERO);

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragPreview(visual);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  application.GetWindow().Lower();

  DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(log.cancelReasons.back() ==
                  DragCancelReason::WINDOW_FOCUS_LOST);
  DALI_TEST_CHECK(!visual.GetParent());
  END_TEST;
}

int UtcDaliDragAndDropDetectorDetachAllCancelsActiveDrag(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                target   = CreateView(Vector2(180.0f, 0.0f));
  View                visual   = CreateView(Vector2::ZERO);

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDragPreview(visual);

  SignalLog signalLog;
  ConnectSignals(detector, signalLog);
  LifecycleOrderLog orderLog;
  ConnectLifecycleOrderSignals(detector, orderLog);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  detector.DetachAll();

  DALI_TEST_EQUALS(signalLog.exited, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.cancelled, 1, TEST_LOCATION);
  DALI_TEST_CHECK(signalLog.cancelReasons.back() ==
                  DragCancelReason::REGISTRATION_REMOVED);
  DALI_TEST_EQUALS(signalLog.ended, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAttachedSourceCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!visual.GetParent());
  const std::vector<LifecycleOrderLog::Event> expectedOrder{
    LifecycleOrderLog::Event::STARTED,
    LifecycleOrderLog::Event::ENTERED,
    LifecycleOrderLog::Event::EXITED,
    LifecycleOrderLog::Event::CANCELLED,
    LifecycleOrderLog::Event::ENDED};
  DALI_TEST_CHECK(orderLog.events == expectedOrder);
  END_TEST;
}

int UtcDaliDragAndDropDetectorSessionControl(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                target   = CreateView(Vector2(180.0f, 0.0f));
  View                visual   = CreateView(Vector2::ZERO);

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDragPreview(visual);

  SignalLog log;
  ConnectSignals(detector, log);
  SessionQueryLog queryLog;
  detector.CancelledSignal().Connect(&queryLog, &SessionQueryLog::OnCancelled);
  detector.EndedSignal().Connect(&queryLog, &SessionQueryLog::OnEnded);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_CHECK(!detector.GetDragSource());
  DALI_TEST_CHECK(!detector.GetDragTarget());

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);

  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_CHECK(detector.GetDragSource() == source);
  DALI_TEST_CHECK(detector.GetDragTarget() == target);
  detector.CancelDrag();

  DALI_TEST_EQUALS(log.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.exited, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(log.cancelReasons.back() ==
                  DragCancelReason::REQUESTED);
  DALI_TEST_CHECK(queryLog.cancellationEvent.GetSource() == source);
  DALI_TEST_CHECK(queryLog.cancellationEvent.GetTarget() == target);
  DALI_TEST_CHECK(queryLog.cancellationEvent.GetCancelReason() ==
                  DragCancelReason::REQUESTED);
  DALI_TEST_CHECK(queryLog.draggingInCancellation);
  DALI_TEST_CHECK(queryLog.sourceInCancellation == source);
  DALI_TEST_CHECK(queryLog.targetInCancellation == target);
  DALI_TEST_CHECK(!queryLog.draggingInEnded);
  DALI_TEST_CHECK(!queryLog.sourceInEnded);
  DALI_TEST_CHECK(!queryLog.targetInEnded);
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_CHECK(!detector.GetDragSource());
  DALI_TEST_CHECK(!detector.GetDragTarget());
  DALI_TEST_CHECK(!visual.GetParent());

  detector.CancelDrag();
  DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorCallbacksCanReleaseLastHandle(void)
{
  UiTestApplication application;
  View              source = CreateView(Vector2::ZERO);
  application.GetScene().Add(source);
  application.SendNotification();
  application.Render();

  {
    DragAndDropDetector detector = DragAndDropDetector::New();
    View                visual   = CreateView(Vector2::ZERO);
    detector.AttachSource(source);
    detector.SetDragPreview(visual);

    ResetDetectorCallbacks reset(detector);
    detector.StartedSignal().Connect(&reset, &ResetDetectorCallbacks::OnStarted);

    uint32_t time = 100u;
    TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);

    DALI_TEST_EQUALS(reset.started, 1, TEST_LOCATION);
    DALI_TEST_CHECK(!detector);
    DALI_TEST_CHECK(!visual.GetParent());
    TestEndPan(application, Vector2(30.0f, 10.0f), time + TestGetFrameInterval());
  }

  {
    DragAndDropDetector detector = DragAndDropDetector::New();
    detector.AttachSource(source);

    SignalLog log;
    ConnectSignals(detector, log);
    ResetDetectorCallbacks reset(detector);
    detector.CancelledSignal().Connect(&reset, &ResetDetectorCallbacks::OnCancelled);

    uint32_t time = 300u;
    TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
    detector.CancelDrag();

    DALI_TEST_EQUALS(reset.cancelled, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
    DALI_TEST_CHECK(!detector);
    TestEndPan(application, Vector2(30.0f, 10.0f), time + TestGetFrameInterval());
  }
  END_TEST;
}

int UtcDaliDragAndDropDetectorSceneClippingExcludesTarget(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                clipper  = CreateView(Vector2(180.0f, 0.0f));
  View                target   = CreateView(Vector2(0.0f, 80.0f));

  clipper.SetRequestedHeight(60.0f);
  clipper.SetProperty(Actor::Property::CLIPPING_MODE,
                      ClippingMode::CLIP_TO_BOUNDING_BOX);
  clipper.Add(target);
  application.GetScene().Add(source);
  application.GetScene().Add(clipper);
  detector.AttachSource(source);
  detector.AttachTarget(target);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 90.0f), time);
  TestEndPan(application, Vector2(210.0f, 90.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(log.entered, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.dropped, 0, TEST_LOCATION);

  clipper.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::DISABLED);
  application.SendNotification();
  application.Render();

  time = 300u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 90.0f), time);
  TestEndPan(application, Vector2(210.0f, 90.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(log.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.dropped, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorAcceptanceRevalidatesDisconnectedTarget(void)
{
  UiTestApplication            application;
  DragAndDropDetector          detector    = DragAndDropDetector::New();
  View                         source      = CreateView(Vector2::ZERO);
  View                         lowerTarget = CreateView(Vector2(180.0f, 0.0f));
  View                         upperTarget = CreateView(Vector2(180.0f, 0.0f));
  DisconnectTargetOnAcceptance disconnect;

  application.GetScene().Add(source);
  application.GetScene().Add(lowerTarget);
  application.GetScene().Add(upperTarget);
  detector.AttachSource(source);
  detector.AttachTarget(lowerTarget);
  detector.AttachTarget(upperTarget);
  detector.SetDropProposalCallback(
    upperTarget,
    DragAndDropDetector::DropProposalCallback::New(
      &disconnect,
      &DisconnectTargetOnAcceptance::Accept));

  TargetLog targetLog;
  detector.EnteredSignal().Connect(&targetLog, &TargetLog::OnEntered);
  detector.DroppedSignal().Connect(&targetLog, &TargetLog::OnDropped);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(disconnect.calls, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!upperTarget.GetParent());
  DALI_TEST_CHECK(targetLog.entered == lowerTarget);
  DALI_TEST_CHECK(targetLog.dropped == lowerTarget);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorAcceptanceGuardRestoresAfterException(void)
{
  UiTestApplication              application;
  DragAndDropDetector            detector = DragAndDropDetector::New();
  View                           source   = CreateView(Vector2::ZERO);
  View                           target   = CreateView(Vector2(180.0f, 0.0f));
  MutateRegistrationOnAcceptance mutation;

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  mutation.detector = detector;
  mutation.target   = target;
  detector.SetDropProposalCallback(
    target,
    DragAndDropDetector::DropProposalCallback::New(
      &mutation,
      &MutateRegistrationOnAcceptance::Accept));
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_ASSERTION(
    TestMovePan(application, Vector2(210.0f, 10.0f), time),
    "Registration cannot change during drop proposal evaluation");

  // The callback exception must not leave registration permanently locked.
  detector.DetachTarget(target);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 0u, TEST_LOCATION);
  detector.CancelDrag();
  DALI_TEST_CHECK(!detector.IsDragging());
  END_TEST;
}

int UtcDaliDragAndDropDetectorTargetMustShareSourceWindow(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                target   = CreateView(Vector2(180.0f, 0.0f));
  Window              otherWindow =
    Window::New(PositionSize(0, 0, 480, 800), "drag-and-drop-other-window");

  application.GetScene().Add(source);
  otherWindow.Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);

  Dali::Integration::Scene otherScene =
    Dali::Internal::Adaptor::Adaptor::GetScene(otherWindow);
  application.AddScene(otherScene);

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(log.entered, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(log.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 1u, TEST_LOCATION);

  application.RemoveScene(otherScene);
  END_TEST;
}

int UtcDaliDragAndDropDetectorEnteredCallbackCanCancel(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                target   = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);

  SignalLog log;
  ConnectSignals(detector, log);
  CancelOnEntered cancelOnEntered;
  detector.EnteredSignal().Connect(&cancelOnEntered, &CancelOnEntered::OnEntered);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);

  DALI_TEST_CHECK(cancelOnEntered.enteredTarget == target);
  DALI_TEST_CHECK(cancelOnEntered.source == source);
  DALI_TEST_CHECK(cancelOnEntered.draggingBeforeCancel);
  DALI_TEST_CHECK(!cancelOnEntered.draggingAfterCancel);
  DALI_TEST_EQUALS(log.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.exited, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(log.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(log.cancelReasons.back() ==
                  DragCancelReason::REQUESTED);
  DALI_TEST_CHECK(!detector.IsDragging());
  END_TEST;
}

int UtcDaliDragAndDropDetectorDetachCurrentTargetEntersFallback(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector    = DragAndDropDetector::New();
  View                source      = CreateView(Vector2::ZERO);
  View                lowerTarget = CreateView(Vector2(180.0f, 0.0f));
  View                upperTarget = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(lowerTarget);
  application.GetScene().Add(upperTarget);
  detector.AttachSource(source);
  detector.AttachTarget(lowerTarget);
  detector.AttachTarget(upperTarget);

  TargetLog targetLog;
  detector.EnteredSignal().Connect(&targetLog, &TargetLog::OnEntered);
  detector.ExitedSignal().Connect(&targetLog, &TargetLog::OnExited);
  detector.DroppedSignal().Connect(&targetLog, &TargetLog::OnDropped);
  LifecycleOrderLog orderLog;
  ConnectLifecycleOrderSignals(detector, orderLog);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  DALI_TEST_CHECK(targetLog.entered == upperTarget);

  detector.DetachTarget(upperTarget);
  DALI_TEST_CHECK(targetLog.exited == upperTarget);
  DALI_TEST_CHECK(targetLog.entered == lowerTarget);
  DALI_TEST_EQUALS(targetLog.enteredCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(targetLog.exitedCount, 1, TEST_LOCATION);

  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());
  DALI_TEST_CHECK(targetLog.dropped == lowerTarget);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 1u, TEST_LOCATION);
  const std::vector<LifecycleOrderLog::Event> expectedOrder{
    LifecycleOrderLog::Event::STARTED,
    LifecycleOrderLog::Event::ENTERED,
    LifecycleOrderLog::Event::EXITED,
    LifecycleOrderLog::Event::ENTERED,
    LifecycleOrderLog::Event::DROPPED,
    LifecycleOrderLog::Event::ENDED};
  DALI_TEST_CHECK(orderLog.events == expectedOrder);
  END_TEST;
}

int UtcDaliDragAndDropDetectorDisconnectedTargetEntersFallback(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector    = DragAndDropDetector::New();
  View                source      = CreateView(Vector2::ZERO);
  View                lowerTarget = CreateView(Vector2(180.0f, 0.0f));
  View                upperTarget = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(lowerTarget);
  application.GetScene().Add(upperTarget);
  detector.AttachSource(source);
  detector.AttachTarget(lowerTarget);
  detector.AttachTarget(upperTarget);

  TargetLog targetLog;
  detector.EnteredSignal().Connect(&targetLog, &TargetLog::OnEntered);
  detector.ExitedSignal().Connect(&targetLog, &TargetLog::OnExited);
  detector.DroppedSignal().Connect(&targetLog, &TargetLog::OnDropped);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  DALI_TEST_CHECK(targetLog.entered == upperTarget);

  application.GetScene().Remove(upperTarget);
  DALI_TEST_CHECK(targetLog.exited == upperTarget);
  DALI_TEST_CHECK(targetLog.entered == lowerTarget);
  DALI_TEST_EQUALS(targetLog.enteredCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(targetLog.exitedCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_CHECK(detector.GetDragTarget() == lowerTarget);

  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());
  DALI_TEST_CHECK(targetLog.dropped == lowerTarget);
  DALI_TEST_EQUALS(detector.GetAttachedTargetCount(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorSessionVisualCallbacks(void)
{
  UiTestApplication      application;
  SessionVisualCallbacks callbacks;
  DragAndDropDetector    detector = DragAndDropDetector::New();
  View                   sourceA  = CreateView(Vector2::ZERO);
  View                   sourceB  = CreateView(Vector2(0.0f, 140.0f));
  View                   target   = CreateView(Vector2(180.0f, 0.0f));
  target.SetRequestedHeight(280.0f);

  application.GetScene().Add(sourceA);
  application.GetScene().Add(sourceB);
  application.GetScene().Add(target);
  detector.AttachSource(sourceA);
  detector.AttachSource(sourceB);
  detector.AttachTarget(target);
  detector.SetDragPreviewCallbacks(
    DragAndDropDetector::DragPreviewFactory::New(&callbacks, &SessionVisualCallbacks::Create),
    DragAndDropDetector::DragPreviewUpdater::New(&callbacks, &SessionVisualCallbacks::Update),
    DragAndDropDetector::DragPreviewFinalizer::New(&callbacks, &SessionVisualCallbacks::Finalize));

  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  time = 300u;
  TestStartPan(application, Vector2(10.0f, 150.0f), Vector2(30.0f, 150.0f), time);
  TestMovePan(application, Vector2(210.0f, 150.0f), time);
  TestCancelPan(application, Vector2(210.0f, 150.0f), time + TestGetFrameInterval());

  time = 500u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(350.0f, 10.0f), time);
  TestEndPan(application, Vector2(350.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(callbacks.factoryEvents.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.finalizerEvents.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.visuals.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.finalizerVisuals.size(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(callbacks.updateEvents.size() >= 3u);
  DALI_TEST_CHECK(callbacks.factoryEvents[0].GetSource() == sourceA);
  DALI_TEST_CHECK(callbacks.factoryEvents[1].GetSource() == sourceB);
  DALI_TEST_CHECK(callbacks.factoryEvents[2].GetSource() == sourceA);
  DALI_TEST_CHECK(callbacks.visuals[0] != callbacks.visuals[1]);
  DALI_TEST_CHECK(callbacks.visuals[0] != callbacks.visuals[2]);
  DALI_TEST_CHECK(callbacks.finalizerVisuals[0] == callbacks.visuals[0]);
  DALI_TEST_CHECK(callbacks.finalizerVisuals[1] == callbacks.visuals[1]);
  DALI_TEST_CHECK(callbacks.finalizerVisuals[2] == callbacks.visuals[2]);
  DALI_TEST_CHECK(callbacks.finalizerEvents[0].GetTarget() == target);
  DALI_TEST_CHECK(callbacks.finalizerEvents[1].GetTarget() == target);
  DALI_TEST_CHECK(!callbacks.finalizerEvents[2].GetTarget());
  DALI_TEST_CHECK(callbacks.finalizerEvents[0].GetResult() == DragAndDropResult::DROPPED);
  DALI_TEST_CHECK(callbacks.finalizerEvents[1].GetResult() == DragAndDropResult::CANCELLED);
  DALI_TEST_CHECK(callbacks.finalizerEvents[2].GetResult() == DragAndDropResult::NO_TARGET);
  const DragAndDropEvent& lastUpdateEvent = callbacks.updateEvents.back();
  DALI_TEST_CHECK(lastUpdateEvent.GetSource() == sourceA);
  DALI_TEST_CHECK(!lastUpdateEvent.GetTarget());
  DALI_TEST_EQUALS(lastUpdateEvent.GetScreenPosition(), Vector2(350.0f, 10.0f), 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(callbacks.visualsWereDetached);
  DALI_TEST_CHECK(!callbacks.visuals[0].GetParent());
  DALI_TEST_CHECK(!callbacks.visuals[1].GetParent());
  DALI_TEST_CHECK(!callbacks.visuals[2].GetParent());

  detector.ClearDragPreviewCallbacks();
  END_TEST;
}

int UtcDaliDragAndDropDetectorParentedFactoryPreviewIsIgnored(void)
{
  UiTestApplication      application;
  SessionVisualCallbacks callbacks;
  SignalLog              signals;
  DragAndDropDetector    detector = DragAndDropDetector::New();
  View                   source   = CreateView(Vector2::ZERO);

  callbacks.factoryParent = View::New();
  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetDragPreviewCallbacks(
    DragAndDropDetector::DragPreviewFactory::New(&callbacks, &SessionVisualCallbacks::Create),
    DragAndDropDetector::DragPreviewUpdater::New(&callbacks, &SessionVisualCallbacks::Update),
    DragAndDropDetector::DragPreviewFinalizer::New(&callbacks, &SessionVisualCallbacks::Finalize));
  ConnectSignals(detector, signals);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(detector.StartDrag(source, Device::Class::KEYBOARD));
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(callbacks.visuals.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(callbacks.visuals[0].GetParent() == callbacks.factoryParent);
  DALI_TEST_CHECK(callbacks.updateEvents.empty());
  DALI_TEST_CHECK(callbacks.finalizerEvents.empty());
  DALI_TEST_CHECK(!detector.Drop());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(signals.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(callbacks.finalizerEvents.empty());
  END_TEST;
}

int UtcDaliDragAndDropDetectorPayloadAndTargetAcceptance(void)
{
  UiTestApplication          application;
  PayloadAcceptanceCallbacks callbacks;
  DragAndDropDetector        detector    = DragAndDropDetector::New();
  View                       source      = CreateView(Vector2::ZERO);
  View                       lowerTarget = CreateView(Vector2(180.0f, 0.0f));
  View                       upperTarget = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(lowerTarget);
  application.GetScene().Add(upperTarget);
  detector.AttachSource(source);
  detector.SetSourcePayload(
    source,
    {"application/x-test", Property::Value(1), DragAndDropOperation::MOVE});
  detector.AttachTarget(lowerTarget);
  detector.AttachTarget(upperTarget);
  detector.SetDropProposalCallback(
    lowerTarget,
    DragAndDropDetector::DropProposalCallback::New(&callbacks, &PayloadAcceptanceCallbacks::AcceptLower));
  detector.SetDropProposalCallback(
    upperTarget,
    DragAndDropDetector::DropProposalCallback::New(&callbacks, &PayloadAcceptanceCallbacks::AcceptUpper));
  detector.StartedSignal().Connect(&callbacks, &PayloadAcceptanceCallbacks::OnStarted);
  detector.DroppedSignal().Connect(&callbacks, &PayloadAcceptanceCallbacks::OnDropped);

  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  time = 300u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  const size_t lowerEventCount = callbacks.lowerEvents.size();
  const size_t upperEventCount = callbacks.upperEvents.size();
  detector.ClearDropProposalCallback(lowerTarget);
  detector.ClearDropProposalCallback(upperTarget);
  detector.SetSourcePayload(
    source,
    {"application/x-test", Property::Value(3), DragAndDropOperation::MOVE});

  time = 500u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(callbacks.startedPayloads.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.droppedPayloads.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.droppedTargets.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.startedPayloads[0], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.startedPayloads[1], 2, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.startedPayloads[2], 3, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.droppedPayloads[0], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.droppedPayloads[1], 2, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.droppedPayloads[2], 3, TEST_LOCATION);
  DALI_TEST_CHECK(callbacks.droppedTargets[0] == lowerTarget);
  DALI_TEST_CHECK(callbacks.droppedTargets[1] == upperTarget);
  DALI_TEST_CHECK(callbacks.droppedTargets[2] == upperTarget);
  DALI_TEST_CHECK(!callbacks.lowerEvents.empty());
  DALI_TEST_CHECK(!callbacks.upperEvents.empty());
  bool lowerSawPayload1 = false;
  bool lowerSawPayload2 = false;
  for(const auto& event : callbacks.lowerEvents)
  {
    DALI_TEST_CHECK(event.GetSource() == source);
    DALI_TEST_CHECK(event.GetCandidateTarget() == lowerTarget);
    DALI_TEST_EQUALS(event.GetTargetLocalPosition(), Vector2(30.0f, 10.0f), 0.01f, TEST_LOCATION);
    int32_t payload = -1;
    event.GetPayload().GetRepresentationData(0u).Get(payload);
    lowerSawPayload1 = lowerSawPayload1 || payload == 1;
    lowerSawPayload2 = lowerSawPayload2 || payload == 2;
  }
  bool upperSawPayload1 = false;
  bool upperSawPayload2 = false;
  for(const auto& event : callbacks.upperEvents)
  {
    DALI_TEST_CHECK(event.GetSource() == source);
    DALI_TEST_CHECK(event.GetCandidateTarget() == upperTarget);
    DALI_TEST_EQUALS(event.GetTargetLocalPosition(), Vector2(30.0f, 10.0f), 0.01f, TEST_LOCATION);
    int32_t payload = -1;
    event.GetPayload().GetRepresentationData(0u).Get(payload);
    upperSawPayload1 = upperSawPayload1 || payload == 1;
    upperSawPayload2 = upperSawPayload2 || payload == 2;
  }
  DALI_TEST_CHECK(lowerSawPayload1);
  DALI_TEST_CHECK(!lowerSawPayload2);
  DALI_TEST_CHECK(upperSawPayload1 && upperSawPayload2);
  DALI_TEST_EQUALS(callbacks.lowerEvents.size(), lowerEventCount, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.upperEvents.size(), upperEventCount, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetDragPayload().GetRepresentationCount(), 0u, TEST_LOCATION);

  detector.ClearSourcePayload(source);
  END_TEST;
}

int UtcDaliDragAndDropDetectorDynamicAndExplicitPayload(void)
{
  UiTestApplication      application;
  DynamicPayloadProvider provider;
  ActivationGate         gate;
  SignalLog              signals;
  DragAndDropDetector    detector = DragAndDropDetector::New();
  View                   source   = CreateView(Vector2::ZERO);

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.SetSourcePayload(
    source,
    {"application/x-static-test", Property::Value(7), DragAndDropOperation::MOVE});
  detector.SetSourcePayloadProvider(
    source,
    DragAndDropDetector::SourcePayloadProvider::New(
      &provider,
      &DynamicPayloadProvider::Provide));
  gate.detector = detector;
  detector.SetCanStartDragCallback(
    DragAndDropDetector::CanStartDragCallback::New(
      &gate,
      &ActivationGate::Approve));
  ConnectSignals(detector, signals);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(detector.IsDragActivationPending());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(provider.calls, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetDragPayload().GetRepresentationCount(), 0u, TEST_LOCATION);
  gate.allow = true;
  TestMovePan(application, Vector2(31.0f, 10.0f), time);
  TestEndPan(application, Vector2(30.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(provider.calls, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(provider.activationEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(provider.activationEvents[0].GetPayload().GetRepresentationType(0u) == "application/x-static-test");
  DALI_TEST_EQUALS(
    provider.activationEvents[0].GetPayload().GetRepresentationData(0u).Get<int32_t>(),
    7,
    TEST_LOCATION);
  DALI_TEST_CHECK(provider.activationEvents[0].GetPayload().GetPreferredOperation() == DragAndDropOperation::MOVE);
  DALI_TEST_EQUALS(signals.terminalEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(signals.terminalEvents[0].GetResult() == DragAndDropResult::NO_TARGET);
  DALI_TEST_CHECK(signals.terminalEvents[0].GetPayload().GetRepresentationType(0u) == "application/x-dynamic-test");
  DALI_TEST_EQUALS(signals.terminalEvents[0].GetPayload().GetRepresentationData(0u).Get<int32_t>(), 101, TEST_LOCATION);
  DALI_TEST_CHECK(signals.terminalEvents[0].GetPayload().GetPreferredOperation() == DragAndDropOperation::COPY);

  detector.ClearCanStartDragCallback();
  detector.ClearSourcePayloadProvider(source);
  const DragPayload explicitPayload{
    "application/x-explicit-test",
    Property::Value(55),
    DragAndDropOperation::LINK};
  DALI_TEST_CHECK(
    detector.StartDrag(source, explicitPayload, Device::Class::KEYBOARD));
  DALI_TEST_CHECK(detector.GetDragPayload().GetRepresentationType(0u) == "application/x-explicit-test");
  DALI_TEST_EQUALS(detector.GetDragPayload().GetRepresentationData(0u).Get<int32_t>(), 55, TEST_LOCATION);
  DALI_TEST_CHECK(detector.GetDragPayload().GetPreferredOperation() == DragAndDropOperation::LINK);
  DALI_TEST_CHECK(!detector.Drop());

  DALI_TEST_EQUALS(provider.calls, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.started, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.terminalEvents.size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(signals.terminalEvents[1].GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(signals.terminalEvents[1].GetResult() == DragAndDropResult::NO_TARGET);
  DALI_TEST_CHECK(signals.terminalEvents[1].GetPayload().GetRepresentationType(0u) == "application/x-explicit-test");
  END_TEST;
}

int UtcDaliDragAndDropDetectorTargetFeedback(void)
{
  UiTestApplication       application;
  TargetFeedbackCallbacks feedback;
  SessionVisualCallbacks  visual;
  DragAndDropDetector     detector = DragAndDropDetector::New();
  View                    source   = CreateView(Vector2::ZERO);
  View                    target   = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDropProposalCallback(
    target,
    DragAndDropDetector::DropProposalCallback::New(
      &feedback,
      &TargetFeedbackCallbacks::Accept));
  detector.SetDragPreviewCallbacks(
    DragAndDropDetector::DragPreviewFactory::New(&visual, &SessionVisualCallbacks::Create),
    DragAndDropDetector::DragPreviewUpdater::New(&visual, &SessionVisualCallbacks::Update),
    DragAndDropDetector::DragPreviewFinalizer::New(&visual, &SessionVisualCallbacks::Finalize));
  detector.TargetFeedbackChangedSignal().Connect(
    &feedback,
    &TargetFeedbackCallbacks::OnFeedbackChanged);

  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  feedback.accepted = true;
  TestMovePan(application, Vector2(211.0f, 10.0f), time);
  feedback.accepted = false;
  TestMovePan(application, Vector2(212.0f, 10.0f), time);
  TestMovePan(application, Vector2(350.0f, 10.0f), time);
  feedback.accepted = true;
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(feedback.events.size(), 6u, TEST_LOCATION);
  DALI_TEST_CHECK(feedback.events[0].GetTargetStatus() == DragAndDropTargetStatus::REJECTED);
  DALI_TEST_CHECK(!feedback.events[0].GetTarget());
  DALI_TEST_CHECK(feedback.events[0].GetCandidateTarget() == target);
  DALI_TEST_EQUALS(feedback.events[0].GetTargetLocalPosition(), Vector2(30.0f, 10.0f), 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(feedback.events[1].GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED);
  DALI_TEST_CHECK(feedback.events[1].GetTarget() == target);
  DALI_TEST_CHECK(feedback.events[1].GetCandidateTarget() == target);
  DALI_TEST_EQUALS(feedback.events[1].GetTargetLocalPosition(), Vector2(31.0f, 10.0f), 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(feedback.events[2].GetTargetStatus() == DragAndDropTargetStatus::REJECTED);
  DALI_TEST_CHECK(feedback.events[2].GetCandidateTarget() == target);
  DALI_TEST_EQUALS(feedback.events[2].GetTargetLocalPosition(), Vector2(32.0f, 10.0f), 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(feedback.events[3].GetTargetStatus() == DragAndDropTargetStatus::NONE);
  DALI_TEST_CHECK(!feedback.events[3].GetCandidateTarget());
  DALI_TEST_EQUALS(feedback.events[3].GetTargetLocalPosition(), Vector2::ZERO, 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(feedback.events[4].GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED);
  DALI_TEST_CHECK(feedback.events[4].GetTarget() == target);
  DALI_TEST_CHECK(feedback.events[5].GetTargetStatus() == DragAndDropTargetStatus::NONE);
  DALI_TEST_CHECK(!feedback.events[5].GetCandidateTarget());
  DALI_TEST_CHECK(feedback.events[5].GetTarget() == target);
  DALI_TEST_CHECK(feedback.events[5].GetResult() == DragAndDropResult::DROPPED);

  bool visualSawAccepted = false;
  bool visualSawRejected = false;
  for(const auto& event : visual.updateEvents)
  {
    visualSawAccepted = visualSawAccepted ||
                        event.GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED;
    visualSawRejected = visualSawRejected ||
                        event.GetTargetStatus() == DragAndDropTargetStatus::REJECTED;
  }
  DALI_TEST_CHECK(visualSawAccepted);
  DALI_TEST_CHECK(visualSawRejected);
  DALI_TEST_CHECK(visual.finalizerEvents.back().GetTargetStatus() ==
                  DragAndDropTargetStatus::ACCEPTED);

  feedback.events.clear();
  View upperTarget = CreateView(Vector2(180.0f, 0.0f));
  application.GetScene().Add(upperTarget);
  detector.AttachTarget(upperTarget);
  detector.SetDropProposalCallback(
    upperTarget,
    DragAndDropDetector::DropProposalCallback::New(
      &feedback,
      &TargetFeedbackCallbacks::Reject));
  application.SendNotification();
  application.Render();

  time = 300u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_EQUALS(feedback.events.size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(feedback.events[0].GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED);
  DALI_TEST_CHECK(feedback.events[0].GetTarget() == target);
  DALI_TEST_CHECK(feedback.events[0].GetCandidateTarget() == target);
  DALI_TEST_CHECK(feedback.events[1].GetTargetStatus() == DragAndDropTargetStatus::NONE);
  END_TEST;
}

int UtcDaliDragAndDropDetectorAutoScrollLifecycle(void)
{
  UiTestApplication   application;
  AutoScrollCallbacks callbacks;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                viewport = CreateView(Vector2(150.0f, 100.0f));
  viewport.SetRequestedWidth(200.0f);
  viewport.SetRequestedHeight(200.0f);

  application.GetScene().Add(source);
  application.GetScene().Add(viewport);
  detector.AttachSource(source);
  detector.SetSourcePayload(
    source,
    {"application/x-test", Property::Value(42), DragAndDropOperation::MOVE});
  detector.AttachTarget(viewport);
  SignalLog signalLog;
  ConnectSignals(detector, signalLog);

  const DragAutoScrollConfiguration configuration(
    viewport,
    Vector2(40.0f, 40.0f),
    Vector2(100.0f, 200.0f),
    20u);
  detector.SetAutoScrollCallback(
    configuration,
    DragAndDropDetector::AutoScrollCallback::New(&callbacks, &AutoScrollCallbacks::Apply));

  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(200.0f, 290.0f), time);
  const int movedBeforeFirstTick = signalLog.moved;
  Test::EmitGlobalTimerSignal();

  DALI_TEST_EQUALS(callbacks.events.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(signalLog.moved > movedBeforeFirstTick);
  DALI_TEST_CHECK(callbacks.events[0].GetDragEvent().GetSource() == source);
  DALI_TEST_CHECK(callbacks.events[0].GetDragEvent().GetTarget() == viewport);
  DALI_TEST_EQUALS(
    callbacks.events[0].GetDragEvent().GetPayload().GetRepresentationData(0u).Get<int32_t>(),
    42,
    TEST_LOCATION);
  DALI_TEST_CHECK(callbacks.events[0].GetViewport() == viewport);
  DALI_TEST_EQUALS(callbacks.events[0].GetEdgeIntensity(), Vector2(0.0f, 0.75f), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.events[0].GetSuggestedDelta(), Vector2(0.0f, 3.0f), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.events[0].GetDeltaSeconds(), 0.02f, 0.001f, TEST_LOCATION);

  callbacks.contentChanged          = false;
  const int movedBeforeRejectedTick = signalLog.moved;
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(callbacks.events.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(signalLog.moved, movedBeforeRejectedTick, TEST_LOCATION);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(callbacks.events.size(), 2u, TEST_LOCATION);

  callbacks.contentChanged = true;
  TestMovePan(application, Vector2(200.0f, 289.0f), time);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(callbacks.events.size(), 3u, TEST_LOCATION);

  TestMovePan(application, Vector2(250.0f, 200.0f), time);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(callbacks.events.size(), 3u, TEST_LOCATION);

  TestMovePan(application, Vector2(200.0f, 110.0f), time);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(callbacks.events.size(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.events.back().GetEdgeIntensity(), Vector2(0.0f, -0.75f), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(callbacks.events.back().GetSuggestedDelta(), Vector2(0.0f, -3.0f), 0.01f, TEST_LOCATION);

  TestEndPan(application, Vector2(200.0f, 110.0f), time + TestGetFrameInterval());
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(callbacks.events.size(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetDragPayload().GetRepresentationCount(), 0u, TEST_LOCATION);

  detector.ClearAutoScrollCallback();
  END_TEST;
}

int UtcDaliDragAndDropDetectorAutoScrollViewportDisconnection(void)
{
  UiTestApplication   application;
  AutoScrollCallbacks callbacks;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                viewport = CreateView(Vector2(150.0f, 100.0f));
  viewport.SetRequestedWidth(200.0f);
  viewport.SetRequestedHeight(200.0f);

  application.GetScene().Add(source);
  application.GetScene().Add(viewport);
  detector.AttachSource(source);

  const DragAutoScrollConfiguration configuration(
    viewport,
    Vector2(40.0f, 40.0f),
    Vector2(100.0f, 200.0f),
    20u);
  detector.SetAutoScrollCallback(
    configuration,
    DragAndDropDetector::AutoScrollCallback::New(&callbacks, &AutoScrollCallbacks::Apply));

  SignalLog log;
  ConnectSignals(detector, log);
  application.SendNotification();
  application.Render();

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestMovePan(application, Vector2(200.0f, 290.0f), time);
  application.GetScene().Remove(viewport);
  Test::EmitGlobalTimerSignal();

  DALI_TEST_EQUALS(callbacks.events.size(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_EQUALS(log.cancelled, 0, TEST_LOCATION);

  application.GetScene().Add(viewport);
  application.SendNotification();
  application.Render();
  TestMovePan(application, Vector2(200.0f, 289.0f), time);
  Test::EmitGlobalTimerSignal();
  DALI_TEST_EQUALS(callbacks.events.size(), 1u, TEST_LOCATION);

  TestEndPan(application, Vector2(200.0f, 289.0f), time + TestGetFrameInterval());
  DALI_TEST_CHECK(!detector.IsDragging());
  detector.ClearAutoScrollCallback();
  END_TEST;
}

int UtcDaliDragAndDropDetectorExplicitSession(void)
{
  UiTestApplication      application;
  SignalLog              signals;
  SessionVisualCallbacks visual;
  DragAndDropDetector    detector = DragAndDropDetector::New();
  View                   source   = CreateView(Vector2::ZERO);
  View                   target   = CreateView(Vector2(180.0f, 0.0f));
  View                   invalid  = CreateView(Vector2(360.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  application.GetScene().Add(invalid);
  DALI_TEST_CHECK(!detector.StartDrag(source, Device::Class::KEYBOARD));

  detector.AttachSource(source);
  detector.SetSourcePayload(
    source,
    {"application/x-test", Property::Value(17), DragAndDropOperation::MOVE});
  detector.AttachTarget(target);
  detector.SetDragPreviewCallbacks(
    DragAndDropDetector::DragPreviewFactory::New(&visual, &SessionVisualCallbacks::Create),
    DragAndDropDetector::DragPreviewUpdater::New(&visual, &SessionVisualCallbacks::Update),
    DragAndDropDetector::DragPreviewFinalizer::New(&visual, &SessionVisualCallbacks::Finalize));
  ConnectSignals(detector, signals);
  application.SendNotification();
  application.Render();

  const Bounds  sourceBounds = source.CalculateScreenExtents();
  const Vector2 sourceCenter(sourceBounds.x + sourceBounds.width * 0.5f,
                             sourceBounds.y + sourceBounds.height * 0.5f);
  const Bounds  targetBounds = target.CalculateScreenExtents();
  const Vector2 targetCenter(targetBounds.x + targetBounds.width * 0.5f,
                             targetBounds.y + targetBounds.height * 0.5f);

  DALI_TEST_CHECK(detector.StartDrag(source, Device::Class::KEYBOARD));
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_CHECK(detector.GetDragSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(detector.GetDragSource() == source);
  DALI_TEST_EQUALS(signals.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.factoryEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(visual.factoryEvents[0].GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(visual.factoryEvents[0].GetDeviceClass() == Device::Class::KEYBOARD);
  DALI_TEST_EQUALS(visual.factoryEvents[0].GetScreenPosition(),
                   sourceCenter,
                   0.01f,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(visual.factoryEvents[0].GetSourceGrabPosition(),
                   Vector2(50.0f, 50.0f),
                   0.01f,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(visual.factoryEvents[0].GetPayload().GetRepresentationData(0u).Get<int32_t>(), 17, TEST_LOCATION);

  DALI_TEST_CHECK(detector.MoveDragTo(target));
  DALI_TEST_CHECK(detector.GetDragTarget() == target);
  DALI_TEST_EQUALS(signals.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.moved, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.updateEvents.back().GetScreenPosition(),
                   targetCenter,
                   0.01f,
                   TEST_LOCATION);
  DALI_TEST_CHECK(visual.updateEvents.back().GetTarget() == target);
  DALI_TEST_CHECK(visual.updateEvents.back().GetTargetStatus() ==
                  DragAndDropTargetStatus::ACCEPTED);

  DALI_TEST_CHECK(!detector.MoveDragTo(invalid));
  DALI_TEST_CHECK(detector.GetDragTarget() == target);
  DALI_TEST_CHECK(detector.Drop());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_CHECK(detector.GetDragSessionOrigin() == DragSessionOrigin::NONE);
  DALI_TEST_EQUALS(signals.dropped, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.finalizerEvents.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(visual.finalizerEvents[0].GetResult() == DragAndDropResult::DROPPED);
  DALI_TEST_CHECK(visual.finalizerEvents[0].GetTarget() == target);
  DALI_TEST_CHECK(visual.finalizerEvents[0].GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(visual.finalizerEvents[0].GetDeviceClass() == Device::Class::KEYBOARD);
  END_TEST;
}

int UtcDaliDragAndDropDetectorEndedRejectsReentrantSession(void)
{
  UiTestApplication     application;
  AttemptRestartOnEnded restart;
  DragAndDropDetector   detector = DragAndDropDetector::New();
  View                  source   = CreateView(Vector2::ZERO);

  application.GetScene().Add(source);
  detector.AttachSource(source);
  detector.EndedSignal().Connect(&restart, &AttemptRestartOnEnded::OnEnded);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(detector.StartDrag(
    source,
    {"application/x-first-test",
     Property::Value(11),
     DragAndDropOperation::MOVE},
    Device::Class::KEYBOARD));
  DALI_TEST_CHECK(!detector.Drop());

  DALI_TEST_CHECK(restart.attempted);
  DALI_TEST_CHECK(!restart.restarted);
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(restart.events.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(restart.events[0].GetResult() == DragAndDropResult::NO_TARGET);
  DALI_TEST_CHECK(restart.events[0].GetPayload().GetRepresentationType(0u) == "application/x-first-test");

  DALI_TEST_CHECK(detector.StartDrag(
    source,
    {"application/x-restarted-test",
     Property::Value(88),
     DragAndDropOperation::COPY},
    Device::Class::KEYBOARD));
  DALI_TEST_CHECK(detector.GetDragPayload().GetRepresentationType(0u) == "application/x-restarted-test");
  DALI_TEST_EQUALS(detector.GetDragPayload().GetRepresentationData(0u).Get<int32_t>(), 88, TEST_LOCATION);
  DALI_TEST_CHECK(detector.GetDragPayload().GetPreferredOperation() == DragAndDropOperation::COPY);
  DALI_TEST_CHECK(!detector.Drop());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(restart.events.size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(restart.events[1].GetPayload().GetRepresentationType(0u) == "application/x-restarted-test");
  END_TEST;
}

int UtcDaliDragAndDropDetectorExplicitSessionRejectionAndCancel(void)
{
  UiTestApplication       application;
  SignalLog               signals;
  TargetFeedbackCallbacks feedback;
  SessionVisualCallbacks  visual;
  DragAndDropDetector     detector = DragAndDropDetector::New();
  View                    source   = CreateView(Vector2::ZERO);
  View                    target   = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDropProposalCallback(
    target,
    DragAndDropDetector::DropProposalCallback::New(
      &feedback,
      &TargetFeedbackCallbacks::Accept));
  detector.SetDragPreviewCallbacks(
    DragAndDropDetector::DragPreviewFactory::New(&visual, &SessionVisualCallbacks::Create),
    DragAndDropDetector::DragPreviewUpdater::New(&visual, &SessionVisualCallbacks::Update),
    DragAndDropDetector::DragPreviewFinalizer::New(&visual, &SessionVisualCallbacks::Finalize));
  detector.TargetFeedbackChangedSignal().Connect(
    &feedback,
    &TargetFeedbackCallbacks::OnFeedbackChanged);
  ConnectSignals(detector, signals);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(detector.StartDrag(source, Device::Class::USER));
  DALI_TEST_CHECK(!detector.MoveDragTo(target));
  DALI_TEST_CHECK(!detector.GetDragTarget());
  DALI_TEST_EQUALS(feedback.events.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(feedback.events[0].GetCandidateTarget() == target);
  DALI_TEST_CHECK(feedback.events[0].GetTargetStatus() ==
                  DragAndDropTargetStatus::REJECTED);
  DALI_TEST_CHECK(!detector.Drop());
  DALI_TEST_CHECK(visual.finalizerEvents.back().GetResult() ==
                  DragAndDropResult::NO_TARGET);
  DALI_TEST_CHECK(visual.finalizerEvents.back().GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(visual.finalizerEvents.back().GetDeviceClass() == Device::Class::USER);

  feedback.accepted = true;
  DALI_TEST_CHECK(detector.StartDrag(source, Device::Class::USER));
  DALI_TEST_CHECK(detector.MoveDragTo(target));
  detector.CancelDrag();

  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(signals.started, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.terminalEvents.size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(signals.terminalEvents[0].GetResult() == DragAndDropResult::NO_TARGET);
  DALI_TEST_CHECK(signals.terminalEvents[1].GetResult() == DragAndDropResult::CANCELLED);
  DALI_TEST_CHECK(signals.cancelReasons.back() ==
                  DragCancelReason::REQUESTED);
  DALI_TEST_CHECK(visual.finalizerEvents.back().GetResult() ==
                  DragAndDropResult::CANCELLED);
  DALI_TEST_CHECK(visual.finalizerEvents.back().GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(visual.finalizerEvents.back().GetDeviceClass() == Device::Class::USER);
  END_TEST;
}

int UtcDaliDragAndDropDetectorExplicitSessionIgnoresGestures(void)
{
  UiTestApplication   application;
  SignalLog           signals;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                target   = CreateView(Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  ConnectSignals(detector, signals);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(detector.StartDrag(source, Device::Class::KEYBOARD));
  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  TestEndPan(application, Vector2(30.0f, 10.0f), time + TestGetFrameInterval());
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_CHECK(detector.GetDragSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_EQUALS(signals.started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 0, TEST_LOCATION);
  DALI_TEST_CHECK(detector.MoveDragTo(target));
  DALI_TEST_CHECK(detector.Drop());

  time = 300u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(30.0f, 10.0f), time);
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_CHECK(detector.GetDragSessionOrigin() ==
                  DragSessionOrigin::GESTURE);
  DALI_TEST_CHECK(!detector.MoveDragTo(target));
  DALI_TEST_CHECK(!detector.Drop());
  DALI_TEST_CHECK(detector.IsDragging());
  TestMovePan(application, Vector2(210.0f, 10.0f), time);
  TestEndPan(application, Vector2(210.0f, 10.0f), time + TestGetFrameInterval());

  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(signals.started, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.dropped, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorExplicitSessionTargetOwnership(void)
{
  UiTestApplication   application;
  AutoScrollCallbacks autoScroll;
  SignalLog           signals;
  DragAndDropDetector detector     = DragAndDropDetector::New();
  View                source       = CreateView(Vector2::ZERO);
  View                viewport     = CreateView(Vector2(150.0f, 100.0f));
  View                chosenTarget = CreateView(Vector2(180.0f, 230.0f));
  View                fallback     = CreateView(Vector2(180.0f, 230.0f));
  viewport.SetRequestedWidth(200.0f);
  viewport.SetRequestedHeight(200.0f);

  application.GetScene().Add(source);
  application.GetScene().Add(viewport);
  application.GetScene().Add(chosenTarget);
  application.GetScene().Add(fallback);
  detector.AttachSource(source);
  detector.AttachTarget(chosenTarget);
  detector.AttachTarget(fallback);
  ConnectSignals(detector, signals);

  const DragAutoScrollConfiguration configuration(
    viewport,
    Vector2(40.0f, 40.0f),
    Vector2(100.0f, 200.0f),
    20u);
  detector.SetAutoScrollCallback(
    configuration,
    DragAndDropDetector::AutoScrollCallback::New(
      &autoScroll,
      &AutoScrollCallbacks::Apply));
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(detector.StartDrag(source, Device::Class::KEYBOARD));
  DALI_TEST_CHECK(detector.MoveDragTo(chosenTarget));
  DALI_TEST_CHECK(detector.GetDragTarget() == chosenTarget);
  Test::EmitGlobalTimerSignal();

  DALI_TEST_EQUALS(autoScroll.events.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(autoScroll.events[0].GetDragEvent().GetTarget() == chosenTarget);
  DALI_TEST_CHECK(detector.GetDragTarget() == chosenTarget);

  chosenTarget.Unparent();
  DALI_TEST_CHECK(detector.IsDragging());
  DALI_TEST_CHECK(!detector.GetDragTarget());
  DALI_TEST_EQUALS(signals.entered, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.exited, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!detector.Drop());
  DALI_TEST_EQUALS(signals.dropped, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 1, TEST_LOCATION);
  detector.ClearAutoScrollCallback();
  END_TEST;
}

int UtcDaliDragAndDropDetectorAccessibilityActionAndFocusController(void)
{
  UiTestApplication                 application;
  AccessibilityDragActionController controller;
  SignalLog                         signals;
  TargetFeedbackCallbacks           feedback;
  controller.detector = DragAndDropDetector::New();
  View source         = CreateAccessibilityDragView(controller, Vector2::ZERO);
  View target         = CreateAccessibilityDragView(controller, Vector2(180.0f, 0.0f));

  application.GetScene().Add(source);
  application.GetScene().Add(target);
  controller.detector.AttachSource(source);
  controller.detector.AttachTarget(target);
  ConnectSignals(controller.detector, signals);
  controller.detector.TargetFeedbackChangedSignal().Connect(
    &feedback,
    &TargetFeedbackCallbacks::OnFeedbackChanged);
  application.SendNotification();
  application.Render();

  Property::Map attributes;
  DALI_TEST_CHECK(source.DoAction("activate", attributes));
  DALI_TEST_CHECK(controller.detector.IsDragging());
  DALI_TEST_CHECK(controller.detector.GetDragSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(target.DoAction("increment", attributes));
  DALI_TEST_CHECK(controller.detector.GetDragTarget() == target);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == target);
  DALI_TEST_CHECK(target.DoAction("activate", attributes));
  DALI_TEST_CHECK(!controller.detector.IsDragging());

  DALI_TEST_CHECK(source.DoAction("activate", attributes));
  DALI_TEST_CHECK(target.DoAction("decrement", attributes));
  DALI_TEST_CHECK(target.DoAction("escape", attributes));
  DALI_TEST_CHECK(!controller.detector.IsDragging());

  DALI_TEST_EQUALS(controller.activateCount, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(controller.adjustCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(controller.escapeCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!controller.lastAdjustmentIncreased);
  DALI_TEST_EQUALS(signals.started, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.entered, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.moved, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.dropped, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 2, TEST_LOCATION);
  DALI_TEST_CHECK(signals.cancelReasons.back() ==
                  DragCancelReason::REQUESTED);
  DALI_TEST_EQUALS(feedback.events.size(), 4u, TEST_LOCATION);
  DALI_TEST_CHECK(feedback.events[0].GetTargetStatus() ==
                  DragAndDropTargetStatus::ACCEPTED);
  DALI_TEST_CHECK(feedback.events[0].GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(feedback.events[0].GetDeviceClass() == Device::Class::USER);
  DALI_TEST_CHECK(feedback.events[1].GetTargetStatus() ==
                  DragAndDropTargetStatus::NONE);
  DALI_TEST_CHECK(feedback.events[1].GetResult() == DragAndDropResult::DROPPED);
  DALI_TEST_CHECK(feedback.events[1].GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(feedback.events[1].GetDeviceClass() == Device::Class::USER);
  DALI_TEST_CHECK(feedback.events[2].GetTargetStatus() ==
                  DragAndDropTargetStatus::ACCEPTED);
  DALI_TEST_CHECK(feedback.events[2].GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(feedback.events[2].GetDeviceClass() == Device::Class::USER);
  DALI_TEST_CHECK(feedback.events[3].GetTargetStatus() ==
                  DragAndDropTargetStatus::NONE);
  DALI_TEST_CHECK(feedback.events[3].GetResult() == DragAndDropResult::CANCELLED);
  DALI_TEST_CHECK(feedback.events[3].GetSessionOrigin() ==
                  DragSessionOrigin::EXPLICIT);
  DALI_TEST_CHECK(feedback.events[3].GetDeviceClass() == Device::Class::USER);
  END_TEST;
}

int UtcDaliDragAndDropDetectorReusableDragSessionController(void)
{
  UiTestApplication              application;
  Samples::DragSessionController controller;
  TargetFeedbackCallbacks        feedback;
  DragAndDropDetector            detector = DragAndDropDetector::New();
  std::vector<View>              targets;
  std::vector<View>              focusedViews;
  std::vector<uint32_t>          resolvedPositions;
  std::vector<std::string>       announcements;
  std::vector<bool>              discardableAnnouncements;

  for(uint32_t position = 0u; position < 4u; ++position)
  {
    targets.push_back(CreateView(Vector2(180.0f * position, 0.0f)));
    application.GetScene().Add(targets.back());
    detector.AttachSource(targets.back());
    detector.AttachTarget(targets.back());
    detector.SetDropProposalCallback(
      targets.back(),
      DragAndDropDetector::DropProposalCallback::New(
        &feedback,
        &TargetFeedbackCallbacks::Accept));
  }

  controller.SetDetector(detector);
  controller.SetTraversalCallbacks(
    [&targets]
  {
    return static_cast<uint32_t>(targets.size());
  },
    [&targets, &resolvedPositions](uint32_t position)
  {
    resolvedPositions.push_back(position);
    return position < targets.size() ? targets[position] : View();
  },
    [&targets](View view)
  {
    for(uint32_t position = 0u; position < targets.size(); ++position)
    {
      if(targets[position] == view)
      {
        return position;
      }
    }
    return static_cast<uint32_t>(targets.size());
  });
  controller.SetItemLabelProvider(
    [](uint32_t position)
  {
    return "localized-item-" + std::to_string(position);
  });
  controller.SetFocusHandler(
    [&focusedViews](View view)
  {
    focusedViews.push_back(view);
    return true;
  });

  Samples::DragSessionMessageProvider messages;
  messages.pickedUp =
    [](const std::string& label, uint32_t, uint32_t)
  {
    return "start:" + label;
  };
  messages.targetFeedback =
    [](const std::string& label, uint32_t, uint32_t, bool accepted)
  {
    return std::string(accepted ? "accepted:" : "rejected:") + label;
  };
  messages.dropped =
    [](const std::string& label, uint32_t, uint32_t)
  {
    return "dropped:" + label;
  };
  messages.cancelled =
    [](const std::string& label, uint32_t, uint32_t)
  {
    return "cancelled:" + label;
  };
  messages.endedWithoutDrop =
    []
  {
    return std::string("no-target");
  };
  controller.SetAnnouncementPolicy(
    std::move(messages),
    [&announcements, &discardableAnnouncements](const std::string& message,
                                                bool               discardable)
  {
    announcements.push_back(message);
    discardableAnnouncements.push_back(discardable);
  });

  DragSessionControllerSignalBridge bridge(controller, targets);
  detector.StartedSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnStarted);
  detector.TargetFeedbackChangedSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnFeedback);
  detector.DroppedSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnDropped);
  detector.CancelledSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnCancelled);
  detector.EndedSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnEnded);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(controller.Start(targets[0], Device::Class::USER));
  DALI_TEST_EQUALS(announcements.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(announcements.back() == "start:localized-item-0");
  DALI_TEST_CHECK(!discardableAnnouncements.back());

  DALI_TEST_CHECK(!detector.MoveDragTo(targets[1]));
  DALI_TEST_CHECK(!detector.MoveDragTo(targets[1]));
  DALI_TEST_EQUALS(announcements.size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(announcements.back() == "rejected:localized-item-1");
  DALI_TEST_CHECK(discardableAnnouncements.back());

  Samples::DragSessionMoveResult move = controller.MoveBy(1);
  DALI_TEST_CHECK(move.handled);
  DALI_TEST_CHECK(!move.accepted);
  DALI_TEST_EQUALS(move.position, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(announcements.size(), 2u, TEST_LOCATION);

  feedback.accepted = true;
  move              = controller.MoveBy(1);
  DALI_TEST_CHECK(move.handled);
  DALI_TEST_CHECK(move.accepted);
  DALI_TEST_EQUALS(move.position, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(announcements.back() == "accepted:localized-item-2");
  DALI_TEST_CHECK(controller.Drop());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_CHECK(announcements.back() == "dropped:localized-item-2");
  DALI_TEST_CHECK(!discardableAnnouncements.back());
  DALI_TEST_EQUALS(announcements.size(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedPositions.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedPositions[0], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedPositions[1], 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedPositions[2], 2u, TEST_LOCATION);

  DALI_TEST_CHECK(controller.Start(targets[2], Device::Class::KEYBOARD));
  move = controller.MoveBy(1);
  DALI_TEST_CHECK(move.handled);
  DALI_TEST_CHECK(move.accepted);
  DALI_TEST_CHECK(controller.Cancel());
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(announcements.size(), 4u, TEST_LOCATION);
  DALI_TEST_CHECK(focusedViews.back() == targets[2]);
  DALI_TEST_CHECK(controller.GetDeviceClass() == Device::Class::NONE);

  View dynamicallyAdded = CreateView(Vector2(720.0f, 0.0f));
  targets.push_back(dynamicallyAdded);
  application.GetScene().Add(dynamicallyAdded);
  detector.AttachSource(dynamicallyAdded);
  detector.AttachTarget(dynamicallyAdded);
  detector.SetDropProposalCallback(
    dynamicallyAdded,
    DragAndDropDetector::DropProposalCallback::New(
      &feedback,
      &TargetFeedbackCallbacks::Accept));
  controller.SynchronizeSelection();
  DALI_TEST_CHECK(controller.Start(targets[2], Device::Class::KEYBOARD));
  move = controller.MoveBy(2);
  DALI_TEST_CHECK(move.handled);
  DALI_TEST_CHECK(move.accepted);
  DALI_TEST_EQUALS(move.position, 4u, TEST_LOCATION);
  DALI_TEST_CHECK(detector.GetDragTarget() == dynamicallyAdded);
  DALI_TEST_CHECK(controller.Cancel());
  DALI_TEST_EQUALS(announcements.size(), 4u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDragAndDropDetectorAccessibleControllerInterruptionPolicy(void)
{
  UiTestApplication              application;
  Samples::DragSessionController controller;
  DragAndDropDetector            detector = DragAndDropDetector::New();
  SignalLog                      signals;
  std::vector<View>              targets{
    CreateView(Vector2::ZERO),
    CreateView(Vector2(180.0f, 0.0f))};
  std::vector<std::string> announcements;
  std::vector<bool>        discardableAnnouncements;

  for(View target : targets)
  {
    application.GetScene().Add(target);
    detector.AttachSource(target);
    detector.AttachTarget(target);
  }

  controller.SetDetector(detector);
  controller.SetTraversalCallbacks(
    [&targets]
  {
    return static_cast<uint32_t>(targets.size());
  },
    [&targets](uint32_t position)
  {
    return position < targets.size() ? targets[position] : View();
  },
    [&targets](View view)
  {
    for(uint32_t position = 0u; position < targets.size(); ++position)
    {
      if(targets[position] == view)
      {
        return position;
      }
    }
    return static_cast<uint32_t>(targets.size());
  });
  controller.SetItemLabelProvider(
    [](uint32_t position)
  {
    return "card " + std::to_string(position + 1u);
  });
  controller.SetFocusHandler(
    [](View)
  {
    return true;
  });
  controller.SetAnnouncementPolicy(
    Samples::CreateEnglishDragSessionMessages(),
    [&announcements, &discardableAnnouncements](const std::string& message,
                                                bool               discardable)
  {
    announcements.push_back(message);
    discardableAnnouncements.push_back(discardable);
  });

  DragSessionControllerSignalBridge bridge(controller, targets);
  detector.StartedSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnStarted);
  detector.TargetFeedbackChangedSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnFeedback);
  detector.DroppedSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnDropped);
  detector.CancelledSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnCancelled);
  detector.EndedSignal().Connect(
    &bridge,
    &DragSessionControllerSignalBridge::OnEnded);
  ConnectSignals(detector, signals);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(controller.Start(targets[0], Device::Class::USER));
  const Samples::DragSessionMoveResult move = controller.MoveBy(1);
  DALI_TEST_CHECK(move.handled);
  DALI_TEST_CHECK(move.accepted);
  DALI_TEST_EQUALS(announcements.size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(discardableAnnouncements[0] == false);
  DALI_TEST_CHECK(discardableAnnouncements[1] == true);

  targets[0].Unparent();
  DALI_TEST_CHECK(!detector.IsDragging());
  DALI_TEST_EQUALS(signals.cancelled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, 1, TEST_LOCATION);
  DALI_TEST_CHECK(signals.cancelReasons.back() ==
                  DragCancelReason::SOURCE_DISCONNECTED);
  DALI_TEST_EQUALS(announcements.size(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(
    announcements.back() ==
    "Drag cancelled. Focus returned to card 1, position 1.");
  DALI_TEST_CHECK(!discardableAnnouncements.back());
  DALI_TEST_CHECK(controller.GetDeviceClass() == Device::Class::NONE);
  END_TEST;
}

int UtcDaliDragAndDropDetectorRepeatedLifecycleStress(void)
{
  UiTestApplication   application;
  DragAndDropDetector detector = DragAndDropDetector::New();
  View                source   = CreateView(Vector2::ZERO);
  View                target   = CreateView(Vector2(180.0f, 0.0f));
  View                visual   = CreateView(Vector2::ZERO);
  SignalLog           signals;

  visual.SetProperty(Actor::Property::SENSITIVE, false);
  application.GetScene().Add(source);
  application.GetScene().Add(target);
  detector.AttachSource(source);
  detector.AttachTarget(target);
  detector.SetDragPreview(visual);
  ConnectSignals(detector, signals);
  application.SendNotification();
  application.Render();

  constexpr uint32_t ITERATION_COUNT = 100u;
  for(uint32_t iteration = 0u; iteration < ITERATION_COUNT; ++iteration)
  {
    DALI_TEST_CHECK(detector.StartDrag(source, Device::Class::KEYBOARD));
    DALI_TEST_CHECK(detector.MoveDragTo(target));
    DALI_TEST_CHECK(visual.GetParent());

    switch(iteration % 4u)
    {
      case 0u:
      {
        DALI_TEST_CHECK(detector.Drop());
        break;
      }
      case 1u:
      {
        detector.CancelDrag();
        break;
      }
      case 2u:
      {
        detector.DetachTarget(target);
        DALI_TEST_CHECK(detector.IsDragging());
        DALI_TEST_CHECK(!detector.GetDragTarget());
        detector.CancelDrag();
        detector.AttachTarget(target);
        break;
      }
      case 3u:
      {
        detector.DetachSource(source);
        DALI_TEST_CHECK(!detector.IsDragging());
        detector.AttachSource(source);
        break;
      }
    }

    DALI_TEST_CHECK(!detector.IsDragging());
    DALI_TEST_CHECK(!detector.GetDragSource());
    DALI_TEST_CHECK(!detector.GetDragTarget());
    DALI_TEST_CHECK(!visual.GetParent());
  }

  DALI_TEST_EQUALS(signals.started, static_cast<int>(ITERATION_COUNT), TEST_LOCATION);
  DALI_TEST_EQUALS(signals.dropped, 25, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.cancelled, 75, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.ended, static_cast<int>(ITERATION_COUNT), TEST_LOCATION);
  DALI_TEST_EQUALS(signals.cancelReasons.size(), 75u, TEST_LOCATION);
  END_TEST;
}
