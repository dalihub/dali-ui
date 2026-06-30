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
#include <string>
#include <vector>

#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/drag-drop-detector/drag-and-drop-detector.h>
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
  using DragAndDropSignal = Dali::Ui::Integration::DragAndDropDetector::DragAndDropSignal;

  // Creation

  /**
   * @copydoc Ui::Integration::DragAndDropDetector::New()
   */
  static Dali::Ui::Integration::DragAndDropDetector New();

  // Public API

  /**
   * @copydoc Dali::DragAndDropDetector::GetContent() const
   */
  const std::string& GetContent() const;

  /**
   * @copydoc Dali::DragAndDropDetector::GetCurrentScreenPosition() const
   */
  const Vector2& GetCurrentScreenPosition() const;

  /**
   * Attaches view to DragAndDropDetector.
   * @param[in] view  view that will be attached to DragAndDropDetector.
   */
  void Attach(Dali::Ui::View& view);

  /**
   * Detaches view to DragAndDropDetector.
   * @param[in] view  view that will be Detached from DragAndDropDetector.
   */
  void Detach(Dali::Ui::View& view);

  /**
   * Detaches all views attached to DragAndDropDetector.
   */
  void DetachAll();

  /**
   * Returns the number of views attached to the DragAndDropDetector.
   */
  uint32_t GetAttachedViewCount() const;

  /**
   * Returns a view by index. An empty handle if the index is not valid.
   */
  Dali::Ui::View GetAttachedView(uint32_t index) const;

  /**
   * Sets the dragged content.
   * @param[in] content  A string that represents the content that has been dropped.
   */
  void SetContent(const std::string& content);

  /**
   * Clears the stored content.
   */
  void ClearContent();

  /**
   * Sets the position the drop occurred.
   */
  void SetPosition(const Vector2& screenPosition);

  /**
   * Called when a draggable object start drag.
   */
  void EmitStartedSignal(Dali::Ui::View& view);

  /**
   * Called when a draggable object enters other object.
   */
  void EmitEnteredSignal(Dali::Ui::View& view);

  /**
   * Called when a draggable object leaves other object.
   */
  void EmitExitedSignal(Dali::Ui::View& view);

  /**
   * Called when a draggable object leaves other object.
   */
  void EmitMovedSignal(Dali::Ui::View& view);

  /**
   * Is called when a drop actually occurs.
   */
  void EmitDroppedSignal(Dali::Ui::View& view);

  /**
   * Called when a draggable object drag ended.
   */
  void EmitEndedSignal(Dali::Ui::View& view);

public: // Signals
  /**
   * @copydoc Dali::Ui::Integration::DragAndDropDetector::StartedSignal
   */
  DragAndDropSignal& StartedSignal()
  {
    return mStartedSignal;
  }

  /**
   * @copydoc Dali::Ui::Integration::DragAndDropDetector::EnteredSignal
   */
  DragAndDropSignal& EnteredSignal()
  {
    return mEnteredSignal;
  }

  /**
   * @copydoc Dali::Ui::Integration::DragAndDropDetector::ExitedSignal
   */
  DragAndDropSignal& ExitedSignal()
  {
    return mExitedSignal;
  }

  /**
   * @copydoc Dali::Ui::Integration::DragAndDropDetector::MovedSignal
   */
  DragAndDropSignal& MovedSignal()
  {
    return mMovedSignal;
  }

  /**
   * @copydoc Dali::Ui::Integration::DragAndDropDetector::DroppedSignal
   */
  DragAndDropSignal& DroppedSignal()
  {
    return mDroppedSignal;
  }

  /**
   * @copydoc Dali::Ui::Integration::DragAndDropDetector::DroppedSignal
   */
  DragAndDropSignal& EndedSignal()
  {
    return mEndedSignal;
  }

public:
  bool OnDrag(Dali::Actor actor, Dali::TouchEvent data);
  void OnPan(Dali::Actor actor, PanGesture gesture);

private:
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

private:
  std::string mContent; ///< The current Drag & drop content.

  DragAndDropSignal mStartedSignal;
  DragAndDropSignal mEnteredSignal;
  DragAndDropSignal mExitedSignal;
  DragAndDropSignal mMovedSignal;
  DragAndDropSignal mDroppedSignal;
  DragAndDropSignal mEndedSignal;

  std::vector<Dali::Ui::View> mViews;              // views attached by Attach interface for drag&drop
  Dali::Ui::View              mDragView;           // the current drag view
  Dali::Ui::View              mShadowView;         // a shadow view for indicating where the view is, same size as the dragged view
  std::vector<uint32_t>       mFirstEnter;         // view id indicating if the cursor is enter
  Dali::PanGestureDetector    mPanGestureDetector; // pangesture for calculating the shadow actor position

  Vector2 mLocalPosition;
  Vector2 mDragLocalPosition;
  Vector2 mScreenPosition; ///< The screen position of the drop location.

  bool mPointDown; // bool flag to indicate if PointState::DOWN have been processed
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::DragAndDropDetector& GetImplementation(Dali::Ui::Integration::DragAndDropDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "DragAndDropDetector handle is empty");

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::DragAndDropDetector&>(handle);
}

inline const Internal::DragAndDropDetector& GetImplementation(const Dali::Ui::Integration::DragAndDropDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "DragAndDropDetector handle is empty");

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::DragAndDropDetector&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_INTERNAL_DRAG_AND_DROP_DETECTOR_H
