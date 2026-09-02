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
#include "manual-feed-scroller.h"

// EXTERNAL INCLUDES
#include <algorithm>
#include <sstream>

#include <dali/devel-api/actors/actor-devel.h>

namespace GeometryTouchGestureSample
{
ManualFeedScroller::ManualFeedScroller(EventLog&          eventLog,
                                       const std::string& name,
                                       Direction          direction,
                                       float              width,
                                       float              height,
                                       float              contentLength,
                                       bool               interceptOnPanStarted,
                                       bool               propagateAtBoundary)
: mEventLog(eventLog),
  mName(name),
  mDirection(direction),
  mViewport(Dali::Ui::View::New()),
  mContent(Dali::Ui::View::New()),
  mPanDetector(Dali::PanGestureDetector::New()),
  mViewportLength(direction == Direction::HORIZONTAL ? width : height),
  mContentLength(contentLength),
  mOffset(0.0f),
  mInterceptOnPanStarted(interceptOnPanStarted),
  mPropagateAtBoundary(propagateAtBoundary),
  mInterceptConnected(false)
{
  mViewport.SetProperty(Dali::Actor::Property::NAME, mName.c_str());
  mViewport.SetProperty(Dali::Actor::Property::CLIPPING_MODE, static_cast<int>(Dali::ClippingMode::CLIP_TO_BOUNDING_BOX));
  mViewport.SetBackgroundColor(Dali::Vector4(0.16f, 0.18f, 0.22f, 1.0f));
  mViewport.SetRequestedWidth(width);
  mViewport.SetRequestedHeight(height);

  const std::string contentName = mName + ".content";
  mContent.SetProperty(Dali::Actor::Property::NAME, contentName.c_str());
  mContent.SetBackgroundColor(Dali::Vector4(0.24f, 0.27f, 0.32f, 1.0f));
  mContent.SetRequestedWidth(direction == Direction::HORIZONTAL ? contentLength : width);
  mContent.SetRequestedHeight(direction == Direction::VERTICAL ? contentLength : height);
  mViewport.Add(mContent);

  mPanDetector.ClearAngles();
  mPanDetector.AddDirection(direction == Direction::HORIZONTAL ? Dali::PanGestureDetector::DIRECTION_HORIZONTAL : Dali::PanGestureDetector::DIRECTION_VERTICAL);
  mPanDetector.DetectedSignal().Connect(this, &ManualFeedScroller::OnPan);
  mViewport.TouchEventSignal().Connect(this, &ManualFeedScroller::OnTouch);
}

ManualFeedScroller::~ManualFeedScroller()
{
  DisconnectIntercept();
}

Dali::Ui::View ManualFeedScroller::GetViewport() const
{
  return mViewport;
}

Dali::Ui::View ManualFeedScroller::GetContent() const
{
  return mContent;
}

void ManualFeedScroller::Add(Dali::Actor actor)
{
  mContent.Add(actor);
}

void ManualFeedScroller::SetPosition(float x, float y)
{
  mViewport.SetRequestedX(x);
  mViewport.SetRequestedY(y);
}

void ManualFeedScroller::SetOffset(float offset)
{
  mOffset = std::max(GetMinimumOffset(), std::min(0.0f, offset));
  if(mDirection == Direction::HORIZONTAL)
  {
    mContent.SetRequestedX(mOffset);
  }
  else
  {
    mContent.SetRequestedY(mOffset);
  }

  if(mOffsetChangedCallback)
  {
    mOffsetChangedCallback(mOffset);
  }
}

void ManualFeedScroller::SetMaximumTouchesRequired(uint32_t maximumTouches)
{
  mPanDetector.SetMaximumTouchesRequired(maximumTouches);
}

void ManualFeedScroller::SetOffsetChangedCallback(const std::function<void(float)>& callback)
{
  mOffsetChangedCallback = callback;
}

float ManualFeedScroller::GetOffset() const
{
  return mOffset;
}

float ManualFeedScroller::GetMinimumOffset() const
{
  return std::min(0.0f, mViewportLength - mContentLength);
}

bool ManualFeedScroller::OnTouch(Dali::Actor actor, Dali::TouchEvent touch)
{
  const bool consumed = mPanDetector.HandleEvent(actor, touch);
  mEventLog.AddTouch("TOUCH", actor, touch, std::string("pan.HandleEvent=") + (consumed ? "true" : "false"));
  return consumed;
}

bool ManualFeedScroller::OnInterceptTouch(Dali::Actor actor, Dali::TouchEvent touch)
{
  mEventLog.AddTouch("INTERCEPT", actor, touch, "return=true (pan already selected)");
  return true;
}

void ManualFeedScroller::OnPan(Dali::Actor actor, Dali::PanGesture pan)
{
  const float displacement = mDirection == Direction::HORIZONTAL ? pan.GetDisplacement().x : pan.GetDisplacement().y;
  const bool  canScroll    = CanScroll(displacement);

  std::ostringstream stream;
  stream << mName << " state=" << GestureStateToString(pan.GetState())
         << " displacement=" << displacement << " offset=" << mOffset;

  if(pan.GetState() == Dali::GestureState::STARTED)
  {
    if(mPropagateAtBoundary && !canScroll)
    {
      Dali::DevelActor::SetNeedGesturePropagation(actor, true);
      stream << " propagate-to-parent=true";
      mEventLog.Add("PAN", stream.str());
      return;
    }

    if(mInterceptOnPanStarted)
    {
      ConnectIntercept();
      stream << " intercept-connected=true";
    }
  }

  if((pan.GetState() == Dali::GestureState::STARTED || pan.GetState() == Dali::GestureState::CONTINUING) && canScroll)
  {
    ScrollBy(displacement);
    stream << " new-offset=" << mOffset;
  }
  else if(pan.GetState() == Dali::GestureState::FINISHED || pan.GetState() == Dali::GestureState::CANCELLED)
  {
    DisconnectIntercept();
  }

  mEventLog.Add("PAN", stream.str());
}

bool ManualFeedScroller::CanScroll(float displacement) const
{
  if(displacement > 0.0f)
  {
    return mOffset < 0.0f;
  }
  if(displacement < 0.0f)
  {
    return mOffset > GetMinimumOffset();
  }
  return false;
}

void ManualFeedScroller::ScrollBy(float displacement)
{
  SetOffset(mOffset + displacement);
}

void ManualFeedScroller::ConnectIntercept()
{
  if(!mInterceptConnected)
  {
    mViewport.InterceptTouchEventSignal().Connect(this, &ManualFeedScroller::OnInterceptTouch);
    mInterceptConnected = true;
  }
}

void ManualFeedScroller::DisconnectIntercept()
{
  if(mInterceptConnected && mViewport)
  {
    mViewport.InterceptTouchEventSignal().Disconnect(this, &ManualFeedScroller::OnInterceptTouch);
    mInterceptConnected = false;
  }
}

} // namespace GeometryTouchGestureSample
