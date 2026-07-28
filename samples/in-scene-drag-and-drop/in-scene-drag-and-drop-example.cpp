/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float    SCROLL_VIEW_HEIGHT    = 420.0f;
constexpr float    SCROLL_CONTENT_HEIGHT = 960.0f;
constexpr float    DRAG_START_THRESHOLD  = 48.0f;
constexpr float    CARD_TOP              = 20.0f;
constexpr float    CARD_WIDTH            = 240.0f;
constexpr float    CARD_HEIGHT           = 72.0f;
constexpr float    CARD_SPACING          = 88.0f;
constexpr uint32_t CARD_COUNT            = 8u;
constexpr auto     CARD_PAYLOAD_TYPE     = "application/x-dali-card-index";
constexpr uint32_t CARD_COLORS[CARD_COUNT] =
  {0x1565C0u, 0x00796Bu, 0x2E7D32u, 0x558B2Fu, 0x8D6E00u, 0xEF6C00u, 0xC62828u, 0x6A1B9Au};
constexpr auto DEVICE_ACTIVATION_HINT =
  "Mouse/Touch: hold until preview appears, then drag | K: keyboard | M: mode";
constexpr auto PAN_ACTIVATION_HINT =
  "All devices: move 48 px | K: keyboard | M: mode";

UiColor GetCardColor(uint32_t identity)
{
  return UiColor(CARD_COLORS[identity % CARD_COUNT]);
}

Label CreateCardLabel(uint32_t identity)
{
  const std::string text  = "Card " + std::to_string(identity + 1u);
  Label             label = Label::New(Dali::String(text.c_str()));
  label.SetRequestedWidth(CARD_WIDTH);
  label.SetRequestedHeight(CARD_HEIGHT);
  label.SetParentOrigin(ParentOrigin::TOP_LEFT);
  label.SetPivot(Pivot::TOP_LEFT);
  label.SetSensitive(false);
  label.SetFontSize(22.0f);
  label.SetTextColor(Color::WHITE);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}
} // unnamed namespace

// A custom preview replaces the former detector-owned grey shadow View.
class InSceneDragAndDropExample : public ConnectionTracker
{
public:
  explicit InSceneDragAndDropExample(Application& application)
  : mApplication(application)
  {
    application.InitSignal().Connect(this, &InSceneDragAndDropExample::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    mDetector     = DragAndDropDetector::New();
    ConfigureDeviceAwareActivation();
    mDetector.SetCanStartDragCallback(
      DragAndDropDetector::CanStartDragCallback::New(
        this,
        &InSceneDragAndDropExample::ApproveActivation));
    mRoot           = View::New();
    const auto size = window.GetPositionSize();
    mRoot.SetRequestedWidth(static_cast<float>(size.width));
    mRoot.SetRequestedHeight(static_cast<float>(size.height));
    window.Add(mRoot);

    mScrollView = ScrollView::New();
    mScrollView.SetRequestedWidth(340.0f);
    mScrollView.SetRequestedHeight(SCROLL_VIEW_HEIGHT);
    mScrollView.SetRequestedX(40.0f);
    mScrollView.SetRequestedY(40.0f);
    mScrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mScrollView.SetPivot(Pivot::TOP_LEFT);
    mScrollView.SetScrollDirection(ScrollDirection::Vertical);
    mContent = View::New();
    mContent.SetRequestedWidth(340.0f);
    mContent.SetRequestedHeight(SCROLL_CONTENT_HEIGHT);
    mContent.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mContent.SetPivot(Pivot::TOP_LEFT);
    mScrollView.SetContent(mContent);
    mRoot.Add(mScrollView);

    mCards.reserve(CARD_COUNT);
    mCardIdentity.reserve(CARD_COUNT);
    for(uint32_t i = 0u; i < CARD_COUNT; ++i)
    {
      mCards.push_back(View::New());
      mCardIdentity.push_back(mCards.back());
      mCards[i].SetRequestedWidth(CARD_WIDTH);
      mCards[i].SetRequestedHeight(CARD_HEIGHT);
      mCards[i].SetRequestedX(40.0f);
      mCards[i].SetRequestedY(CARD_TOP + CARD_SPACING * i);
      mCards[i].SetParentOrigin(ParentOrigin::TOP_LEFT);
      mCards[i].SetPivot(Pivot::CENTER);
      mCards[i].SetCornerRadius(10.0f);
      mCards[i].SetBackgroundColor(GetCardColor(i));
      mCards[i].SetFocusable(true);
      mCards[i].SetAutomationId(
        Dali::String(("in-scene-drag-card-" + std::to_string(i + 1u)).c_str()));
      mCards[i].Add(CreateCardLabel(i));
      mContent.Add(mCards[i]);
      mDetector.AttachSource(mCards[i]);
      mDetector.SetSourcePayloadProvider(
        mCards[i],
        DragAndDropDetector::SourcePayloadProvider::New(
          this,
          &InSceneDragAndDropExample::CreateCardPayload));
      mDetector.AttachTarget(mCards[i]);
      mDetector.SetDropProposalCallback(
        mCards[i],
        DragAndDropDetector::DropProposalCallback::New(this, &InSceneDragAndDropExample::AcceptDrop));
    }

    mActivationHint = Label::New();
    mActivationHint.SetRequestedWidth(300.0f);
    mActivationHint.SetRequestedHeight(96.0f);
    mActivationHint.SetRequestedX(20.0f);
    mActivationHint.SetRequestedY(CARD_TOP + CARD_SPACING * CARD_COUNT + 12.0f);
    mActivationHint.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mActivationHint.SetPivot(Pivot::TOP_LEFT);
    mActivationHint.SetSensitive(false);
    mActivationHint.SetText(DEVICE_ACTIVATION_HINT);
    mContent.Add(mActivationHint);

    // Use a transformed scene-level overlay to demonstrate that the preview
    // anchor is independent of the source and container transform hierarchy.
    mDragOverlay = View::New();
    mDragOverlay.SetRequestedWidth(static_cast<float>(size.width));
    mDragOverlay.SetRequestedHeight(static_cast<float>(size.height));
    mDragOverlay.SetRequestedX(0.0f);
    mDragOverlay.SetRequestedY(0.0f);
    mDragOverlay.SetLayoutMode(LayoutMode::STANDALONE);
    mDragOverlay.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mDragOverlay.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mDragOverlay.SetPivot(Pivot::CENTER);
    mDragOverlay.SetSensitive(false);
    mDragOverlay.SetOrientation(
      Quaternion(Radian(Degree(3.0f)), Vector3::ZAXIS));
    mDragOverlay.SetScale(Vector3(0.97f, 1.03f, 1.0f));
    mRoot.Add(mDragOverlay);

    mDetector.SetDragPreviewCallbacks(
      DragAndDropDetector::DragPreviewFactory::New(this, &InSceneDragAndDropExample::CreatePreview),
      DragAndDropDetector::DragPreviewUpdater::New(this, &InSceneDragAndDropExample::PositionPreview),
      DragAndDropDetector::DragPreviewFinalizer::New(this, &InSceneDragAndDropExample::FinalizePreview));
    // Keep the preview above the ScrollView clip. Applications can use any
    // scene-level overlay View as the drag preview container.
    mDetector.SetDragPreviewContainer(mDragOverlay);
    const DragAutoScrollConfiguration autoScroll(
      mScrollView,
      Vector2(0.0f, 56.0f),
      Vector2(0.0f, 480.0f),
      16u);
    mDetector.SetAutoScrollCallback(
      autoScroll,
      DragAndDropDetector::AutoScrollCallback::New(this, &InSceneDragAndDropExample::ApplyAutoScroll));
    mDetector.StartedSignal().Connect(this, &InSceneDragAndDropExample::OnStarted);
    mDetector.EnteredSignal().Connect(this, &InSceneDragAndDropExample::OnEntered);
    mDetector.ExitedSignal().Connect(this, &InSceneDragAndDropExample::OnExited);
    mDetector.DroppedSignal().Connect(this, &InSceneDragAndDropExample::OnDropped);
    mDetector.CancelledSignal().Connect(this, &InSceneDragAndDropExample::OnCancelled);
    mDetector.TargetFeedbackChangedSignal().Connect(
      this,
      &InSceneDragAndDropExample::OnTargetFeedbackChanged);
    mDetector.EndedSignal().Connect(this, &InSceneDragAndDropExample::OnEnded);
    FocusManager::Get().FocusChangedSignal().Connect(
      this,
      &InSceneDragAndDropExample::OnFocusChanged);
    window.KeyEventSignal().Connect(this, &InSceneDragAndDropExample::OnKeyEvent);
    FocusCard(0u);
  }

private:
  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(event.GetKeyName() == "K" || event.GetKeyName() == "k")
    {
      ToggleKeyboardDrag();
    }
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT) ||
            IsKey(event, Dali::DALI_KEY_CURSOR_UP))
    {
      NavigateDrag(-1);
    }
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT) ||
            IsKey(event, Dali::DALI_KEY_CURSOR_DOWN))
    {
      NavigateDrag(1);
    }
    else if(event.GetKeyName() == "M" || event.GetKeyName() == "m")
    {
      if(!mDetector.IsDragging() && !mDetector.IsDragActivationPending())
      {
        mDeviceAwareActivation = !mDeviceAwareActivation;
        if(mDeviceAwareActivation)
        {
          ConfigureDeviceAwareActivation();
          mActivationHint.SetText(DEVICE_ACTIVATION_HINT);
        }
        else
        {
          mDetector.ClearDragActivationConfiguration(Device::Class::MOUSE);
          mDetector.ClearDragActivationConfiguration(Device::Class::TOUCH);
          mDetector.SetDragActivationMode(DragActivationMode::PAN);
          mDetector.SetDragStartThreshold(DRAG_START_THRESHOLD);
          mActivationHint.SetText(PAN_ACTIVATION_HINT);
        }
      }
    }
    else if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      if(mDetector.IsDragging() || mDetector.IsDragActivationPending())
      {
        mDetector.CancelDrag();
      }
      else
      {
        mApplication.Quit();
      }
    }
  }

  void ToggleKeyboardDrag()
  {
    if(mDetector.IsDragging())
    {
      if(mDetector.GetDragSessionOrigin() == DragSessionOrigin::EXPLICIT)
      {
        const bool dropped = mDetector.Drop();
        mActivationHint.SetText(
          dropped ? "Explicit drag dropped"
                  : "Explicit drag ended without an accepted target");
      }
      return;
    }
    if(mDetector.IsDragActivationPending())
    {
      return;
    }

    if(mSelectedPosition >= mCards.size())
    {
      return;
    }

    mKeyboardSourcePosition = mSelectedPosition;
    mKeyboardTargetPosition = mSelectedPosition;
    FocusCard(mSelectedPosition);
    const DragPayload payload{
      CARD_PAYLOAD_TYPE,
      Property::Value(static_cast<int32_t>(mSelectedPosition)),
      DragAndDropOperation::MOVE};
    if(!mDetector.StartDrag(
         mCards[mSelectedPosition],
         payload,
         Device::Class::KEYBOARD))
    {
      mActivationHint.SetText("Could not start keyboard drag");
    }
  }

  void NavigateDrag(int direction)
  {
    const uint32_t count = static_cast<uint32_t>(mCards.size());
    if(direction == 0 || count == 0u)
    {
      return;
    }

    const bool keyboardDrag =
      mDetector.IsDragging() &&
      mDetector.GetDragSessionOrigin() == DragSessionOrigin::EXPLICIT;
    if(mDetector.IsDragActivationPending() ||
       (mDetector.IsDragging() && !keyboardDrag))
    {
      return;
    }

    uint32_t next = keyboardDrag ? mKeyboardTargetPosition : mSelectedPosition;
    for(uint32_t attempt = 0u; attempt < count; ++attempt)
    {
      const int64_t signedCount = static_cast<int64_t>(count);
      int64_t       candidatePosition =
        (static_cast<int64_t>(next) + static_cast<int64_t>(direction)) %
        signedCount;
      if(candidatePosition < 0)
      {
        candidatePosition += signedCount;
      }
      next = static_cast<uint32_t>(candidatePosition);

      View candidate = ResolveExplicitTarget(next);
      if(!candidate ||
         (keyboardDrag && candidate == mDetector.GetDragSource()))
      {
        continue;
      }

      FocusCard(next);
      if(keyboardDrag)
      {
        mKeyboardTargetPosition = next;
        const bool accepted     = mDetector.MoveDragTo(candidate);
        SetHint(
          std::string(accepted ? "Accepted " : "Rejected ") +
          "card " + std::to_string(next + 1u) +
          " | arrows: navigate | K: drop | Esc: cancel");
      }
      else
      {
        mSelectedPosition = next;
        SetHint(
          "Selected card " + std::to_string(next + 1u) +
          " | K: keyboard drag");
      }
      return;
    }
  }

  uint32_t FindCardIndex(View card) const
  {
    for(uint32_t index = 0u; index < mCards.size(); ++index)
    {
      if(mCards[index] == card)
      {
        return index;
      }
    }
    return static_cast<uint32_t>(mCards.size());
  }

  uint32_t FindCardIdentity(View card) const
  {
    for(uint32_t identity = 0u; identity < mCardIdentity.size(); ++identity)
    {
      if(mCardIdentity[identity] == card)
      {
        return identity;
      }
    }
    return 0u;
  }

  bool FocusCard(uint32_t index)
  {
    return index < mCards.size() &&
           FocusManager::Get().SetCurrentFocusView(mCards[index]);
  }

  View ResolveExplicitTarget(uint32_t position)
  {
    if(position >= mCards.size())
    {
      return View();
    }

    // A virtualized collection can realize the requested logical position in
    // this callback. This sample keeps all Views alive, but scrolls the
    // requested item into the viewport before returning it.
    const Vector2 before     = mScrollView.GetScrollPosition();
    const float   cardTop    = CARD_TOP + CARD_SPACING * position;
    const float   cardBottom = cardTop + CARD_HEIGHT;
    float         nextY      = before.y;
    if(cardTop < before.y)
    {
      nextY = cardTop;
    }
    else if(cardBottom > before.y + SCROLL_VIEW_HEIGHT)
    {
      nextY = cardBottom - SCROLL_VIEW_HEIGHT;
    }
    nextY = std::clamp(nextY, 0.0f, SCROLL_CONTENT_HEIGHT - SCROLL_VIEW_HEIGHT);
    if(nextY != before.y)
    {
      mScrollView.ScrollTo(Vector2(before.x, nextY), false);
    }
    return mCards[position];
  }

  void OnFocusChanged(View, View to)
  {
    if(mDetector.IsDragging() || mDetector.IsDragActivationPending())
    {
      return;
    }

    const uint32_t index = FindCardIndex(to);
    if(index < mCards.size())
    {
      mSelectedPosition = index;
      SetHint(
        "Focused card " + std::to_string(index + 1u) +
        " | K: keyboard | Mouse/Touch: gesture");
    }
  }

  void SetHint(const std::string& text)
  {
    mActivationHint.SetText(Dali::String(text.c_str()));
  }

  bool ApproveActivation(const DragActivationEvent& event)
  {
    int32_t sourceIndex = -1;
    event.GetPayload().GetRepresentationData(0u).Get(sourceIndex);
    const bool approved =
      sourceIndex >= 0 &&
      static_cast<uint32_t>(sourceIndex) < mCards.size();
    if(approved)
    {
      const bool longPress = event.GetMode() == DragActivationMode::LONG_PRESS;
      mActivationHint.SetText(
        event.GetDeviceClass() == Device::Class::MOUSE
          ? (longPress ? "App approved mouse long press" : "App approved mouse pan")
        : event.GetDeviceClass() == Device::Class::TOUCH
          ? (longPress ? "App approved touch long press" : "App approved touch pan")
          : (longPress ? "App approved other long press" : "App approved other pan"));
    }
    return approved;
  }

  DragPayload CreateCardPayload(const DragActivationEvent& event)
  {
    return {
      CARD_PAYLOAD_TYPE,
      Property::Value(static_cast<int32_t>(FindCardIndex(event.GetSource()))),
      DragAndDropOperation::MOVE};
  }

  void ConfigureDeviceAwareActivation()
  {
    // Some desktop adaptors report mouse input as Device::Class::NONE.
    // Keep the fallback configuration aligned with the explicit mouse/touch
    // configurations so those events do not silently fall back to PAN.
    mDetector.SetDragActivationMode(DragActivationMode::LONG_PRESS);
    mDetector.SetDragStartThreshold(0.0f);
    mDetector.SetDragActivationConfiguration(
      Device::Class::MOUSE,
      {DragActivationMode::LONG_PRESS, 0.0f});
    mDetector.SetDragActivationConfiguration(
      Device::Class::TOUCH,
      {DragActivationMode::LONG_PRESS, 0.0f});
  }

  View CreatePreview(const DragAndDropEvent& event)
  {
    const uint32_t identity   = FindCardIdentity(event.GetSource());
    View           preview    = View::New();
    const Vector3  sourceSize = event.GetSource().GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    preview.SetRequestedWidth(sourceSize.x);
    preview.SetRequestedHeight(sourceSize.y);
    preview.SetLayoutMode(LayoutMode::STANDALONE);
    preview.SetUiScalePolicy(UiScalePolicy::DISABLED);
    preview.SetParentOrigin(ParentOrigin::TOP_LEFT);
    preview.SetPivot(Pivot::TOP_LEFT);
    preview.SetPositionUsesPivotEnabled(true);
    preview.SetOrientation(
      Quaternion(Radian(Degree(-6.0f)), Vector3::ZAXIS));
    preview.SetScale(Vector3(1.05f, 0.95f, 1.0f));
    preview.SetSensitive(false);
    preview.SetCornerRadius(10.0f);
    preview.SetBackgroundColor(GetCardColor(identity));
    preview.Add(CreateCardLabel(identity));
    return preview;
  }

  DropProposal AcceptDrop(const DragAndDropEvent& event)
  {
    int32_t sourceIndex = -1;
    Property::Value cardData;
    if(event.GetPayload().GetRepresentationData(CARD_PAYLOAD_TYPE, cardData))
    {
      cardData.Get(sourceIndex);
    }

    int32_t targetIndex = -1;
    for(uint32_t index = 0u; index < mCards.size(); ++index)
    {
      if(mCards[index] == event.GetCandidateTarget())
      {
        targetIndex = static_cast<int32_t>(index);
      }
    }

    // This sample accepts only adjacent card moves.
    const bool acceptsMove =
      event.GetPayload().IsOperationAllowed(DragAndDropOperation::MOVE) &&
      sourceIndex >= 0 &&
      targetIndex >= 0 &&
      (sourceIndex + 1 == targetIndex || targetIndex + 1 == sourceIndex);
    return acceptsMove
             ? DropProposal::Accept(CARD_PAYLOAD_TYPE, DragAndDropOperation::MOVE)
             : DropProposal::Reject();
  }

  bool ApplyAutoScroll(const DragAutoScrollEvent& event)
  {
    const Vector2   before         = mScrollView.GetScrollPosition();
    constexpr float maximumScrollY = SCROLL_CONTENT_HEIGHT - SCROLL_VIEW_HEIGHT;
    const Vector2   next(before.x,
                         std::clamp(before.y + event.GetSuggestedDelta().y, 0.0f, maximumScrollY));
    if(next == before)
    {
      return false;
    }
    mScrollView.ScrollTo(next, false);
    return true;
  }

  void PositionPreview(View preview, const DragAndDropEvent& event)
  {
    // Position the normalized source grab point directly under the pointer.
    // This remains correct when the source, root overlay, or preview is scaled
    // or rotated because both anchor and position use their own coordinate
    // spaces instead of subtracting unrelated local coordinates.
    preview.SetPivot(
      Vector3(event.GetSourceAnchor().x, event.GetSourceAnchor().y, 0.5f));
    preview.SetRequestedX(event.GetPreviewLocalPosition().x);
    preview.SetRequestedY(event.GetPreviewLocalPosition().y);
    if(event.GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED)
    {
      preview.SetBackgroundColor(UiColor(0.15f, 0.8f, 0.35f, 0.75f));
    }
    else if(event.GetTargetStatus() == DragAndDropTargetStatus::REJECTED)
    {
      preview.SetBackgroundColor(UiColor(0.95f, 0.3f, 0.1f, 0.75f));
    }
    else
    {
      preview.SetBackgroundColor(UiColor(0.95f, 0.25f, 0.2f, 0.65f));
    }
  }

  void FinalizePreview(View preview, const DragAndDropEvent& event)
  {
    // The detector has already detached the preview. An application could
    // reparent it here for an asynchronous drop/cancel animation.
    if(event.GetResult() == DragAndDropResult::CANCELLED)
    {
      preview.SetOpacity(0.0f);
    }
  }

  void OnStarted(const DragAndDropEvent& event, DragAndDropDetector)
  {
    View source = event.GetSource();
    mDragged    = source;
    // The drag owns pointer motion until it ends. Normal ScrollView pan
    // scrolling is suspended; ApplyAutoScroll() can still use ScrollTo() when
    // the pointer reaches the viewport's top or bottom edge.
    mScrollView.SetPanScrollEnabled(false);
    source.SetOpacity(0.35f);
    if(event.GetSessionOrigin() == DragSessionOrigin::EXPLICIT)
    {
      const uint32_t sourceIndex = FindCardIndex(event.GetSource());
      if(sourceIndex < mCards.size())
      {
        mKeyboardSourcePosition = sourceIndex;
        mKeyboardTargetPosition = sourceIndex;
      }
      SetHint("Keyboard drag | arrows: navigate | K: drop | Esc: cancel");
    }
  }

  void OnEntered(const DragAndDropEvent& event, DragAndDropDetector)
  {
    View target = event.GetTarget();
    target.SetBackgroundColor(UiColor(0.2f, 0.8f, 0.35f, 1.0f));
  }

  void OnExited(const DragAndDropEvent& event, DragAndDropDetector)
  {
    RestoreCardColor(event.GetTarget());
  }

  void OnDropped(const DragAndDropEvent& event, DragAndDropDetector)
  {
    int32_t sourceIndex = -1;
    Property::Value cardData;
    if(!event.GetSelectedRepresentationData(cardData) ||
       !cardData.Get(sourceIndex))
    {
      return;
    }
    const bool explicitSession =
      event.GetSessionOrigin() == DragSessionOrigin::EXPLICIT;
    uint32_t targetIndex = static_cast<uint32_t>(mCards.size());
    for(uint32_t index = 0u; index < mCards.size(); ++index)
    {
      if(mCards[index] == event.GetTarget()) targetIndex = index;
    }
    if(sourceIndex >= 0 &&
       static_cast<uint32_t>(sourceIndex) < mCards.size() &&
       targetIndex < mCards.size())
    {
      std::swap(mCards[static_cast<uint32_t>(sourceIndex)], mCards[targetIndex]);
      for(uint32_t index = 0u; index < mCards.size(); ++index)
      {
        mCards[index].SetRequestedX(40.0f);
        mCards[index].SetRequestedY(CARD_TOP + CARD_SPACING * index);
        // Preserve each card's identity color so the reordered result is
        // immediately visible instead of recoloring by slot.
        RestoreCardColor(mCards[index]);
      }
      if(explicitSession)
      {
        mSelectedPosition       = targetIndex;
        mKeyboardSourcePosition = targetIndex;
        mKeyboardTargetPosition = targetIndex;
        FocusCard(targetIndex);
      }
    }
  }

  void OnCancelled(const DragAndDropEvent& event, DragAndDropDetector)
  {
    // Explicit-session cancellation returns focus to the original source.
    if(event.GetSessionOrigin() == DragSessionOrigin::EXPLICIT)
    {
      mSelectedPosition       = mKeyboardSourcePosition;
      mKeyboardTargetPosition = mKeyboardSourcePosition;
      FocusCard(mKeyboardSourcePosition);
    }
  }

  void OnTargetFeedbackChanged(const DragAndDropEvent& event, DragAndDropDetector)
  {
    if(mFeedbackTarget)
    {
      RestoreCardColor(mFeedbackTarget);
    }

    mFeedbackTarget = event.GetCandidateTarget();
    if(event.GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED && mFeedbackTarget)
    {
      mFeedbackTarget.SetBackgroundColor(UiColor(0.2f, 0.8f, 0.35f, 1.0f));
    }
    else if(event.GetTargetStatus() == DragAndDropTargetStatus::REJECTED && mFeedbackTarget)
    {
      mFeedbackTarget.SetBackgroundColor(UiColor(0.95f, 0.3f, 0.1f, 1.0f));
    }
  }

  void OnEnded(const DragAndDropEvent& event, DragAndDropDetector)
  {
    View source = event.GetSource();
    mScrollView.SetPanScrollEnabled(true);
    source.SetOpacity(1.0f);
    mDragged.Reset();
    const uint32_t focusedIndex =
      FindCardIndex(FocusManager::Get().GetCurrentFocusView());
    if(focusedIndex < mCards.size())
    {
      mSelectedPosition = focusedIndex;
    }
    mActivationHint.SetText(
      mDeviceAwareActivation ? DEVICE_ACTIVATION_HINT : PAN_ACTIVATION_HINT);
  }

  void RestoreCardColor(View card)
  {
    for(uint32_t identity = 0u; identity < mCardIdentity.size(); ++identity)
    {
      if(mCardIdentity[identity] == card)
      {
        card.SetBackgroundColor(GetCardColor(identity));
        return;
      }
    }
  }

  Application&        mApplication;
  DragAndDropDetector mDetector;
  View                mRoot, mContent, mDragOverlay, mDragged, mFeedbackTarget;
  std::vector<View>   mCards;
  std::vector<View>   mCardIdentity;
  Label               mActivationHint;
  ScrollView          mScrollView;
  uint32_t            mSelectedPosition{0u};
  uint32_t            mKeyboardSourcePosition{0u};
  uint32_t            mKeyboardTargetPosition{0u};
  bool                mDeviceAwareActivation{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application               application = Application::New(&argc, &argv);
  InSceneDragAndDropExample example(application);
  application.MainLoop();
  return 0;
}
