/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#ifndef DALI_UI_SAMPLES_DRAG_SESSION_CONTROLLER_H
#define DALI_UI_SAMPLES_DRAG_SESSION_CONTROLLER_H

#include <dali-ui-foundation/public-api/drag-and-drop/drag-and-drop-detector.h>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Samples
{
/**
 * Caller-provided text policy. A product can replace these callbacks with
 * localized resource lookups without changing drag traversal or focus logic.
 */
struct DragSessionMessageProvider
{
  using ItemMessage = std::function<std::string(const std::string&, uint32_t, uint32_t)>;
  using FeedbackMessage =
    std::function<std::string(const std::string&, uint32_t, uint32_t, bool)>;

  ItemMessage                  pickedUp;
  FeedbackMessage              targetFeedback;
  ItemMessage                  dropped;
  ItemMessage                  cancelled;
  std::function<std::string()> endedWithoutDrop;
};

inline DragSessionMessageProvider CreateEnglishDragSessionMessages()
{
  DragSessionMessageProvider messages;
  messages.pickedUp =
    [](const std::string& label, uint32_t position, uint32_t count)
  {
    return "Picked up " + label + ", position " + std::to_string(position + 1u) +
           " of " + std::to_string(count) +
           ". Use increment and decrement to choose a target, then activate to drop.";
  };
  messages.targetFeedback =
    [](const std::string& label, uint32_t position, uint32_t count, bool accepted)
  {
    return label + ", position " + std::to_string(position + 1u) +
           " of " + std::to_string(count) +
           (accepted ? ". Drop allowed." : ". Drop not allowed.");
  };
  messages.dropped =
    [](const std::string& label, uint32_t position, uint32_t count)
  {
    return "Dropped " + label + " at position " + std::to_string(position + 1u) +
           " of " + std::to_string(count) + ".";
  };
  messages.cancelled =
    [](const std::string& label, uint32_t position, uint32_t)
  {
    return "Drag cancelled. Focus returned to " + label + ", position " +
           std::to_string(position + 1u) + ".";
  };
  messages.endedWithoutDrop =
    []
  {
    return std::string("Drag ended without a drop.");
  };
  return messages;
}

struct DragSessionMoveResult
{
  bool     handled{false};
  bool     accepted{false};
  uint32_t position{0u};
};

/**
 * Reusable app-side controller layered on DragAndDropDetector.
 *
 * TargetResolver receives a logical collection position. It may return an
 * already-realized View or synchronously realize/scroll a virtualized item.
 */
class DragSessionController
{
public:
  using TargetCountProvider  = std::function<uint32_t()>;
  using TargetResolver       = std::function<View(uint32_t)>;
  using ViewPositionResolver = std::function<uint32_t(View)>;
  using ItemLabelProvider    = std::function<std::string(uint32_t)>;
  using FocusHandler         = std::function<bool(View)>;
  using AnnouncementSink     = std::function<void(const std::string&, bool)>;

  void SetDetector(DragAndDropDetector detector)
  {
    mDetector = detector;
  }

  void SetTraversalCallbacks(TargetCountProvider  countProvider,
                             TargetResolver       targetResolver,
                             ViewPositionResolver positionResolver = {})
  {
    mTargetCountProvider = std::move(countProvider);
    mTargetResolver      = std::move(targetResolver);
    mPositionResolver    = std::move(positionResolver);
    SynchronizeSelection();
  }

  void SetItemLabelProvider(ItemLabelProvider labelProvider)
  {
    mItemLabelProvider = std::move(labelProvider);
  }

  void SetFocusHandler(FocusHandler focusHandler)
  {
    mFocusHandler = std::move(focusHandler);
  }

  void SetAnnouncementPolicy(DragSessionMessageProvider messageProvider,
                             AnnouncementSink           announcementSink)
  {
    mMessageProvider  = std::move(messageProvider);
    mAnnouncementSink = std::move(announcementSink);
  }

  void SetFocusOnDropEnabled(bool enabled)
  {
    mFocusOnDropEnabled = enabled;
  }

  bool IsFocusOnDropEnabled() const
  {
    return mFocusOnDropEnabled;
  }

  void SetRestoreFocusOnCancelEnabled(bool enabled)
  {
    mRestoreFocusOnCancelEnabled = enabled;
  }

  bool IsRestoreFocusOnCancelEnabled() const
  {
    return mRestoreFocusOnCancelEnabled;
  }

  bool Select(View view)
  {
    if(!SetSelectedView(view))
    {
      return false;
    }
    return Focus(view);
  }

  bool SetSelectedView(View view)
  {
    const uint32_t position = FindPosition(view);
    if(position >= GetTargetCount())
    {
      return false;
    }
    mSelectedPosition = position;
    mTargetPosition   = position;
    return true;
  }

  bool Start(View source, Device::Class::Type deviceClass)
  {
    if(!mDetector || mDetector.IsDragging() || mDetector.IsDragActivationPending())
    {
      return false;
    }

    const uint32_t position = FindPosition(source);
    if(position >= GetTargetCount())
    {
      return false;
    }

    mSelectedPosition = position;
    mSourcePosition   = position;
    mTargetPosition   = position;
    mDeviceClass      = deviceClass;
    Focus(source);
    if(!mDetector.StartDrag(source, deviceClass))
    {
      ResetSession();
      return false;
    }
    return true;
  }

  DragSessionMoveResult MoveBy(int32_t direction)
  {
    DragSessionMoveResult result;
    const uint32_t        count = GetTargetCount();
    if(direction == 0 || count == 0u || !mTargetResolver)
    {
      return result;
    }

    const bool explicitDrag =
      mDetector &&
      mDetector.IsDragging() &&
      mDetector.GetDragSessionOrigin() == DragSessionOrigin::EXPLICIT;
    if(mDetector &&
       (mDetector.IsDragActivationPending() ||
        (mDetector.IsDragging() && !explicitDrag)))
    {
      return result;
    }

    uint32_t next = explicitDrag ? mTargetPosition : mSelectedPosition;
    for(uint32_t attempt = 0u; attempt < count; ++attempt)
    {
      next           = WrapPosition(next, direction, count);
      View candidate = mTargetResolver(next);
      if(!candidate ||
         (explicitDrag && candidate == mDetector.GetDragSource()))
      {
        continue;
      }

      mTargetPosition = next;
      if(!explicitDrag)
      {
        mSelectedPosition = next;
      }
      Focus(candidate);
      result.handled  = true;
      result.position = next;
      result.accepted = explicitDrag ? mDetector.MoveDragTo(candidate) : false;
      return result;
    }
    return result;
  }

  bool Drop()
  {
    return mDetector &&
           mDetector.IsDragging() &&
           mDetector.GetDragSessionOrigin() == DragSessionOrigin::EXPLICIT &&
           mDetector.Drop();
  }

  bool Cancel()
  {
    if(!mDetector ||
       !mDetector.IsDragging() ||
       mDetector.GetDragSessionOrigin() != DragSessionOrigin::EXPLICIT)
    {
      return false;
    }
    mDetector.CancelDrag();
    return true;
  }

  bool IsAccessibilityDrag() const
  {
    return mDetector &&
           mDetector.IsDragging() &&
           mDetector.GetDragSessionOrigin() == DragSessionOrigin::EXPLICIT &&
           mDeviceClass == Device::Class::USER;
  }

  uint32_t GetSelectedPosition() const
  {
    return mSelectedPosition;
  }

  uint32_t GetTargetPosition() const
  {
    return mTargetPosition;
  }

  Device::Class::Type GetDeviceClass() const
  {
    return mDeviceClass;
  }

  void SynchronizeSelection()
  {
    const uint32_t count = GetTargetCount();
    if(count == 0u)
    {
      mSelectedPosition = 0u;
      mSourcePosition   = 0u;
      mTargetPosition   = 0u;
      return;
    }
    mSelectedPosition = std::min(mSelectedPosition, count - 1u);
    mSourcePosition   = std::min(mSourcePosition, count - 1u);
    mTargetPosition   = std::min(mTargetPosition, count - 1u);
  }

  void HandleStarted(const DragAndDropEvent& event)
  {
    if(event.GetSessionOrigin() != DragSessionOrigin::EXPLICIT)
    {
      return;
    }

    const uint32_t position = FindPosition(event.GetSource());
    if(position < GetTargetCount())
    {
      mSourcePosition = position;
    }
    if(mDeviceClass == Device::Class::USER && mMessageProvider.pickedUp)
    {
      BeginAnnouncement(
        mMessageProvider.pickedUp(LabelAt(mSourcePosition),
                                  mSourcePosition,
                                  GetTargetCount()));
    }
  }

  void HandleTargetFeedback(const DragAndDropEvent& event)
  {
    if(event.GetSessionOrigin() != DragSessionOrigin::EXPLICIT ||
       event.GetDeviceClass() != Device::Class::USER ||
       !event.GetCandidateTarget() ||
       !mMessageProvider.targetFeedback)
    {
      return;
    }

    const uint32_t position = FindPosition(event.GetCandidateTarget());
    if(position >= GetTargetCount())
    {
      return;
    }

    if(event.GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED ||
       event.GetTargetStatus() == DragAndDropTargetStatus::REJECTED)
    {
      Announce(
        mMessageProvider.targetFeedback(
          LabelAt(position),
          position,
          GetTargetCount(),
          event.GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED),
        true);
    }
  }

  void HandleDropped(uint32_t finalPosition)
  {
    const uint32_t count = GetTargetCount();
    if(count == 0u)
    {
      return;
    }

    mSelectedPosition = std::min(finalPosition, count - 1u);
    mSourcePosition   = mSelectedPosition;
    mTargetPosition   = mSelectedPosition;
    if(mFocusOnDropEnabled && mTargetResolver)
    {
      Focus(mTargetResolver(mSelectedPosition));
    }
    EndAnnouncement(
      mMessageProvider.dropped
        ? mMessageProvider.dropped(LabelAt(mSelectedPosition),
                                   mSelectedPosition,
                                   count)
        : std::string());
  }

  void HandleCancelled(const DragAndDropEvent& event)
  {
    if(event.GetSessionOrigin() != DragSessionOrigin::EXPLICIT)
    {
      return;
    }

    if(mRestoreFocusOnCancelEnabled && mTargetResolver)
    {
      Focus(mTargetResolver(mSourcePosition));
    }
    mSelectedPosition = mSourcePosition;
    mTargetPosition   = mSourcePosition;
    if(event.GetDeviceClass() == Device::Class::USER)
    {
      EndAnnouncement(
        mMessageProvider.cancelled
          ? mMessageProvider.cancelled(LabelAt(mSourcePosition),
                                       mSourcePosition,
                                       GetTargetCount())
          : std::string());
    }
  }

  void HandleEnded()
  {
    EndAnnouncement(
      mMessageProvider.endedWithoutDrop
        ? mMessageProvider.endedWithoutDrop()
        : std::string());
    ResetSession();
  }

private:
  uint32_t GetTargetCount() const
  {
    return mTargetCountProvider ? mTargetCountProvider() : 0u;
  }

  uint32_t FindPosition(View view) const
  {
    const uint32_t count = GetTargetCount();
    if(!view)
    {
      return count;
    }
    if(mPositionResolver)
    {
      return std::min(mPositionResolver(view), count);
    }
    if(!mTargetResolver)
    {
      return count;
    }
    for(uint32_t position = 0u; position < count; ++position)
    {
      if(mTargetResolver(position) == view)
      {
        return position;
      }
    }
    return count;
  }

  std::string LabelAt(uint32_t position) const
  {
    return mItemLabelProvider
             ? mItemLabelProvider(position)
             : "item " + std::to_string(position + 1u);
  }

  bool Focus(View view)
  {
    return view && (!mFocusHandler || mFocusHandler(view));
  }

  static uint32_t WrapPosition(uint32_t position,
                               int32_t  direction,
                               uint32_t count)
  {
    const int64_t signedCount = static_cast<int64_t>(count);
    int64_t       next =
      (static_cast<int64_t>(position) + static_cast<int64_t>(direction)) %
      signedCount;
    if(next < 0)
    {
      next += signedCount;
    }
    return static_cast<uint32_t>(next);
  }

  void BeginAnnouncement(const std::string& message)
  {
    mAnnouncementSessionActive = true;
    mLastAnnouncement.clear();
    Announce(message, false);
  }

  void EndAnnouncement(const std::string& message)
  {
    if(mAnnouncementSessionActive)
    {
      Announce(message, false);
      mAnnouncementSessionActive = false;
      mLastAnnouncement.clear();
    }
  }

  void Announce(const std::string& message, bool discardable)
  {
    if(!mAnnouncementSessionActive ||
       message.empty() ||
       message == mLastAnnouncement)
    {
      return;
    }
    mLastAnnouncement = message;
    if(mAnnouncementSink)
    {
      mAnnouncementSink(message, discardable);
    }
  }

  void ResetSession()
  {
    mDeviceClass = Device::Class::NONE;
  }

  DragAndDropDetector        mDetector;
  TargetCountProvider        mTargetCountProvider;
  TargetResolver             mTargetResolver;
  ViewPositionResolver       mPositionResolver;
  ItemLabelProvider          mItemLabelProvider;
  FocusHandler               mFocusHandler;
  DragSessionMessageProvider mMessageProvider;
  AnnouncementSink           mAnnouncementSink;
  std::string                mLastAnnouncement;
  uint32_t                   mSelectedPosition{0u};
  uint32_t                   mSourcePosition{0u};
  uint32_t                   mTargetPosition{0u};
  Device::Class::Type        mDeviceClass{Device::Class::NONE};
  bool                       mAnnouncementSessionActive{false};
  bool                       mFocusOnDropEnabled{true};
  bool                       mRestoreFocusOnCancelEnabled{true};
};

} // namespace Samples
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_SAMPLES_DRAG_SESSION_CONTROLLER_H
