#ifndef DALI_UI_SAMPLE_GEOMETRY_TOUCH_GESTURE_MANUAL_FEED_SCROLLER_H
#define DALI_UI_SAMPLE_GEOMETRY_TOUCH_GESTURE_MANUAL_FEED_SCROLLER_H

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
#include <functional>
#include <string>

// INTERNAL INCLUDES
#include "sample-common.h"

namespace GeometryTouchGestureSample
{
/**
 * @brief Feeds touch events to a directional pan detector and applies a bounded content offset.
 */
class ManualFeedScroller : public Dali::ConnectionTracker
{
public:
  enum class Direction
  {
    HORIZONTAL,
    VERTICAL
  };

  /** @brief Creates a bounded scroller backed by a manually fed pan detector. */
  ManualFeedScroller(EventLog&          eventLog,
                     const std::string& name,
                     Direction          direction,
                     float              width,
                     float              height,
                     float              contentLength,
                     bool               interceptOnPanStarted,
                     bool               propagateAtBoundary);

  /** @brief Disconnects optional intercept handling and destroys the helper. */
  ~ManualFeedScroller() override;

  ManualFeedScroller(const ManualFeedScroller&)            = delete;
  ManualFeedScroller& operator=(const ManualFeedScroller&) = delete;

  /** @brief Returns the clipped touch target. */
  Dali::Ui::View GetViewport() const;

  /** @brief Returns the translated content container. */
  Dali::Ui::View GetContent() const;

  /** @brief Adds an Actor to the content container. */
  void Add(Dali::Actor actor);

  /** @brief Positions the viewport in its parent. */
  void SetPosition(float x, float y);

  /** @brief Clamps and applies the content offset. */
  void SetOffset(float offset);

  /** @brief Sets the maximum number of touch points accepted by the pan detector. */
  void SetMaximumTouchesRequired(uint32_t maximumTouches);

  /** @brief Sets a callback invoked whenever the clamped offset changes. */
  void SetOffsetChangedCallback(const std::function<void(float)>& callback);

  /** @brief Returns the current content offset. */
  float GetOffset() const;

  /** @brief Returns the most negative valid content offset. */
  float GetMinimumOffset() const;

private:
  /** @brief Feeds a touch event to the pan detector. */
  bool OnTouch(Dali::Actor actor, Dali::TouchEvent touch);

  /** @brief Consumes interception after this scroller's pan has been selected. */
  bool OnInterceptTouch(Dali::Actor actor, Dali::TouchEvent touch);

  /** @brief Applies pan displacement or requests parent propagation at a boundary. */
  void OnPan(Dali::Actor actor, Dali::PanGesture pan);

  /** @brief Reports whether displacement can move the content within its bounds. */
  bool CanScroll(float displacement) const;

  /** @brief Applies a relative displacement through the clamping setter. */
  void ScrollBy(float displacement);

  /** @brief Connects the optional intercept callback once. */
  void ConnectIntercept();

  /** @brief Disconnects the optional intercept callback when connected. */
  void DisconnectIntercept();

private:
  EventLog&                  mEventLog;
  std::string                mName;
  Direction                  mDirection;
  Dali::Ui::View             mViewport;
  Dali::Ui::View             mContent;
  Dali::PanGestureDetector   mPanDetector;
  std::function<void(float)> mOffsetChangedCallback;
  float                      mViewportLength;
  float                      mContentLength;
  float                      mOffset;
  bool                       mInterceptOnPanStarted;
  bool                       mPropagateAtBoundary;
  bool                       mInterceptConnected;
};

} // namespace GeometryTouchGestureSample

#endif // DALI_UI_SAMPLE_GEOMETRY_TOUCH_GESTURE_MANUAL_FEED_SCROLLER_H
