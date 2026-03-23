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
#include <dali-ui-foundation/public-api/controls/control.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/math/vector2.h>

#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/scroll-view-layout-manager.h>
#include <dali-ui-foundation/integration-api/scroll-view-impl.h>
#include <dali-ui-foundation/public-api/controls/control.h>
//#include <dali-ui-elements/public-api/scroll-view.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{

BaseHandle Create()
{
  return BaseHandle();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(ScrollViewImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

ScrollViewImplPtr ScrollViewImpl::New()
{
  return ScrollViewImplPtr(new ScrollViewImpl());
}

ScrollViewImpl::ScrollViewImpl()
: ViewImpl(new ScrollViewLayoutManager()),
  mContent(),
  mScrollPosition(0.0f, 0.0f),
  mCurrentPosition(0.0f, 0.0f),
  mScrollableWidth(0.0f),
  mScrollableHeight(0.0f),
  mScrollDirection(ScrollDirection::Vertical),
  mMaxFlingDistance(10000.0f),
  mFlingSensitivity(1.0f),
  mDecelerationRate(0.998f),
  mOverScrollMode(OverScrollMode::ContentScrolls),
  mIsScrolling(false),
  mViewportWidth(0.0f),
  mViewportHeight(0.0f),
  mMaximumStartY(0.0f),
  mMaximumStartX(0.0f),
  mMinimumStartY(0.0f),
  mMinimumStartX(0.0f),
  mHasScrollableArea(false),
  mVerticalScrollBarVisibility(ScrollBarVisibility::Auto),
  mHorizontalScrollBarVisibility(ScrollBarVisibility::Auto),
  mPanGestureDetector(PanGestureDetector::New()),
  mTotalDisplacement(0.0f, 0.0f),
  mPanThreshold(5.0f),
  mIsThresholdMet(false),
  mStartPanPosition(0.0f, 0.0f),
  mLastPanPosition(0.0f, 0.0f)
{
  // Set initial pan gesture directions for vertical scrolling
  mPanGestureDetector.AddDirection(PanGestureDetector::DIRECTION_VERTICAL);
}

ScrollViewImpl::~ScrollViewImpl()
{
}

void ScrollViewImpl::OnInitialize()
{
  // Initialize base class
  ViewImpl::OnInitialize();

  // Enable clipping to bounds for scrollable content
  Self().SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  // Configure pan gesture detector
  mPanGestureDetector.DetectedSignal().Connect(this, &ScrollViewImpl::OnPanGesture);
  // Attach the pan gesture detector to this actor
  // TODO: we need to subsribe the touch event and send gesture event!
  mPanGestureDetector.Attach(Self());
}

void ScrollViewImpl::SetContent(View content)
{
  // Remove old content if exists
  if(mContent)
  {
    RemoveView(mContent);
  }

  mContent = content;

  // Add new content
  if(mContent)
  {
    AddView(mContent);
    mContent.LowerToBottom();
    UpdateScrollingProperties();
  }
}

View ScrollViewImpl::GetContent() const
{
  return mContent;
}

Vector2 ScrollViewImpl::GetScrollPosition() const
{
  return mScrollPosition;
}

void ScrollViewImpl::SetScrollPosition(const Vector2& position)
{
  ScrollTo(position, false);
}

bool ScrollViewImpl::IsScrolling() const
{
  return mIsScrolling;
}

float ScrollViewImpl::GetScrollableWidth() const
{
  return mScrollableWidth;
}

void ScrollViewImpl::SetScrollableWidth(float width)
{
  mScrollableWidth = width;
  UpdateScrollingProperties();
}

float ScrollViewImpl::GetScrollableHeight() const
{
  return mScrollableHeight;
}

void ScrollViewImpl::SetScrollableHeight(float height)
{
  mScrollableHeight = height;
  UpdateScrollingProperties();
}

ScrollDirection ScrollViewImpl::GetScrollDirection() const
{
  return mScrollDirection;
}

void ScrollViewImpl::SetScrollDirection(ScrollDirection direction)
{
  if(mScrollDirection == direction)
  {
    return;
  }

  mScrollDirection = direction;

  // Update pan gesture detector directions if initialized
  if(mPanGestureDetector)
  {
    mPanGestureDetector.RemoveDirection(PanGestureDetector::DIRECTION_VERTICAL);
    mPanGestureDetector.RemoveDirection(PanGestureDetector::DIRECTION_HORIZONTAL);

    switch(mScrollDirection)
    {
      case ScrollDirection::Vertical:
        mPanGestureDetector.AddDirection(PanGestureDetector::DIRECTION_VERTICAL);
        break;
      case ScrollDirection::Horizontal:
        mPanGestureDetector.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
        break;
      case ScrollDirection::Both:
        mPanGestureDetector.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
        mPanGestureDetector.AddDirection(PanGestureDetector::DIRECTION_VERTICAL);
        break;
    }
  }
}

float ScrollViewImpl::GetMaxFlingDistance() const
{
  return mMaxFlingDistance;
}

void ScrollViewImpl::SetMaxFlingDistance(float distance)
{
  mMaxFlingDistance = distance;
}

float ScrollViewImpl::GetFlingSensitivity() const
{
  return mFlingSensitivity;
}

void ScrollViewImpl::SetFlingSensitivity(float sensitivity)
{
  mFlingSensitivity = sensitivity;
}

float ScrollViewImpl::GetDecelerationRate() const
{
  return mDecelerationRate;
}

void ScrollViewImpl::SetDecelerationRate(float rate)
{
  mDecelerationRate = rate;
}

OverScrollMode ScrollViewImpl::GetOverScrollMode() const
{
  return mOverScrollMode;
}

void ScrollViewImpl::SetOverScrollMode(OverScrollMode mode)
{
  mOverScrollMode = mode;
}

void ScrollViewImpl::ScrollTo(const Vector2& position, bool animation)
{
  CancelScrollAnimation();

  // Force immediate scroll if not ready for animation
  bool isVisible = Self().GetProperty<bool>(Actor::Property::VISIBLE);
  if(!isVisible)
  {
    animation = false;
  }

  Vector2 adjustedPosition = AdjustScrollPosition(position);

  if(!animation)
  {
    SendScrollStarted();
    ApplyScrollPosition(adjustedPosition);
    mCurrentPosition = Vector2(std::round(mContent.GetProperty<float>(Actor::Property::POSITION_X)),
                               std::round(mContent.GetProperty<float>(Actor::Property::POSITION_Y)));
    mScrollPosition  = ContentPositionToScrollPosition(mCurrentPosition);
    SendScrolling();
    SendScrollFinished();
  }
  else
  {
    Vector2 delta = DeltaFromScrollPosition(adjustedPosition);
    if(delta.LengthSquared() > 0.0001f)
    {
      SendScrollStarted();
      ApplyScrollPosition(adjustedPosition);
      SendScrollFinished();
    }
  }
}

void ScrollViewImpl::ScrollTo(View child, bool animation, ScrollToPosition scrollToPosition)
{
  if(!mContent)
  {
    return;
  }

  Vector2 childScrollPos = GetScrollPositionForChild(child, Vector2::ZERO);

  if(scrollToPosition == ScrollToPosition::MakeVisible)
  {
    // Check if already visible
    Vector2 scrollPos    = GetScrollPosition();
    float   scrollRight  = scrollPos.x + mViewportWidth;
    float   scrollBottom = scrollPos.y + mViewportHeight;
    float   childRight   = childScrollPos.x + child.GetProperty<float>(Actor::Property::SIZE_WIDTH);
    float   childBottom  = childScrollPos.y + child.GetProperty<float>(Actor::Property::SIZE_HEIGHT);

    if(childScrollPos.x >= scrollPos.x && childRight <= scrollRight && childScrollPos.y >= scrollPos.y &&
       childBottom <= scrollBottom)
    {
      return;
    }

    // Determine scroll direction
    switch(mScrollDirection)
    {
      case ScrollDirection::Vertical:
        scrollToPosition = (childScrollPos.y > scrollPos.y) ? ScrollToPosition::End : ScrollToPosition::Start;
        break;
      case ScrollDirection::Horizontal:
        scrollToPosition = (childScrollPos.x > scrollPos.x) ? ScrollToPosition::End : ScrollToPosition::Start;
        break;
      case ScrollDirection::Both:
      {
        float posX = scrollPos.x;
        float posY = scrollPos.y;
        if(scrollRight < childRight)
        {
          posX = scrollPos.x + childRight - scrollRight;
        }
        else if(scrollPos.x > childScrollPos.x)
        {
          posX = scrollPos.x - (scrollPos.x - childScrollPos.x);
        }
        if(scrollPos.y > childScrollPos.y)
        {
          posY = scrollPos.y - (scrollPos.y - childScrollPos.y);
        }
        else if(scrollBottom < childBottom)
        {
          posY = scrollPos.y + (childBottom - scrollBottom);
        }
        ScrollTo(Vector2(posX, posY), animation);
        return;
      }
    }
  }

  // Adjust position based on scrollToPosition
  switch(scrollToPosition)
  {
    case ScrollToPosition::Center:
      childScrollPos.y -= mViewportHeight / 2.0f - child.GetProperty<float>(Actor::Property::SIZE_HEIGHT) / 2.0f;
      childScrollPos.x -= mViewportWidth / 2.0f - child.GetProperty<float>(Actor::Property::SIZE_WIDTH) / 2.0f;
      break;
    case ScrollToPosition::End:
      childScrollPos.y -= mViewportHeight - child.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
      childScrollPos.x -= mViewportWidth - child.GetProperty<float>(Actor::Property::SIZE_WIDTH);
      break;
    case ScrollToPosition::Start:
    default:
      break;
  }

  ScrollTo(childScrollPos, animation);
}

void ScrollViewImpl::ScrollToX(float position, bool animation)
{
  ScrollTo(Vector2(position, mScrollPosition.y), animation);
}

void ScrollViewImpl::ScrollToY(float position, bool animation)
{
  ScrollTo(Vector2(mScrollPosition.x, position), animation);
}

ScrollBarVisibility ScrollViewImpl::GetVerticalScrollBarVisibility() const
{
  return mVerticalScrollBarVisibility;
}

void ScrollViewImpl::SetVerticalScrollBarVisibility(ScrollBarVisibility visibility)
{
  mVerticalScrollBarVisibility = visibility;
}

ScrollBarVisibility ScrollViewImpl::GetHorizontalScrollBarVisibility() const
{
  return mHorizontalScrollBarVisibility;
}

void ScrollViewImpl::SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility)
{
  mHorizontalScrollBarVisibility = visibility;
}

Ui::ScrollView::ScrollStartedSignalType& ScrollViewImpl::ScrollStartedSignal()
{
  return mScrollStartedSignal;
}

Ui::ScrollView::ScrollingSignalType& ScrollViewImpl::ScrollingSignal()
{
  return mScrollingSignal;
}

Ui::ScrollView::ScrollFinishedSignalType& ScrollViewImpl::ScrollFinishedSignal()
{
  return mScrollFinishedSignal;
}

Ui::ScrollView::DragStartedSignalType& ScrollViewImpl::DragStartedSignal()
{
  return mDragStartedSignal;
}

Ui::ScrollView::DraggingSignalType& ScrollViewImpl::DraggingSignal()
{
  return mDraggingSignal;
}

Ui::ScrollView::DragFinishedSignalType& ScrollViewImpl::DragFinishedSignal()
{
  return mDragFinishedSignal;
}

Vector2 ScrollViewImpl::VelocityToMovement(const Vector2& velocity) const
{
  float decelerationFactor = std::log(mDecelerationRate);

  Vector2 screenSize = Stage::GetCurrent().GetSize();

  float movementX = -1.0f * mFlingSensitivity * (velocity.x > 0.0f ? 1.0f : -1.0f) * velocity.x * velocity.x *
                    screenSize.width / (2000.0f * decelerationFactor);
  float movementY = -1.0f * mFlingSensitivity * (velocity.y > 0.0f ? 1.0f : -1.0f) * velocity.y * velocity.y *
                    screenSize.height / (2000.0f * decelerationFactor);

  if(std::abs(movementX) > mMaxFlingDistance)
  {
    movementX = mMaxFlingDistance * (movementX > 0.0f ? 1.0f : -1.0f);
  }
  if(std::abs(movementY) > mMaxFlingDistance)
  {
    movementY = mMaxFlingDistance * (movementY > 0.0f ? 1.0f : -1.0f);
  }

  return Vector2(movementX, movementY);
}

Vector2 ScrollViewImpl::AdjustMovement(const Vector2& movement) const
{
  float movX = movement.x;
  float movY = movement.y;

  if(mScrollDirection != ScrollDirection::Both)
  {
    // Set dominant movement
    if(std::abs(movX) > std::abs(movY))
    {
      movY = 0.0f;
    }
    else
    {
      movX = 0.0f;
    }
  }

  if(mScrollDirection == ScrollDirection::Vertical)
  {
    movX = 0.0f;
  }
  else if(mScrollDirection == ScrollDirection::Horizontal)
  {
    movY = 0.0f;
  }

  return Vector2(movX, movY);
}

Vector2 ScrollViewImpl::AdjustDelta(const Vector2& movement, const Vector2& currentPosition)
{
  Vector2 adjusted = AdjustMovement(movement);

  float curX = (currentPosition.x != 0.0f) ? currentPosition.x : mCurrentPosition.x;
  float curY = (currentPosition.y != 0.0f) ? currentPosition.y : mCurrentPosition.y;

  float targetY = curY + adjusted.y;
  targetY       = std::max(targetY, mMinimumStartY);
  targetY       = std::min(targetY, mMaximumStartY);

  float targetX = curX + adjusted.x;
  targetX       = std::max(targetX, mMinimumStartX);
  targetX       = std::min(targetX, mMaximumStartX);

  return Vector2(targetX - curX, targetY - curY);
}

Vector2 ScrollViewImpl::AdjustScrollPosition(const Vector2& position) const
{
  float posX = std::max(0.0f, position.x);
  float posY = std::max(0.0f, position.y);

  float maxScrollY = std::max(0.0f, mScrollableHeight - mViewportHeight);
  float maxScrollX = std::max(0.0f, mScrollableWidth - mViewportWidth);

  posX = std::min(posX, maxScrollX);
  posY = std::min(posY, maxScrollY);

  return Vector2(posX, posY);
}

Vector2 ScrollViewImpl::GetScrollPositionForChild(View child, Vector2 current) const
{
  if(child == mContent)
  {
    return current;
  }

  View parent = View::DownCast(child.GetParent());

  if(!parent)
  {
    return current;
  }

  if(parent == mContent)
  {
    return Vector2(current.x + child.GetProperty<float>(Actor::Property::POSITION_X),
                   current.y + child.GetProperty<float>(Actor::Property::POSITION_Y));
  }

  return GetScrollPositionForChild(parent, Vector2(current.x + child.GetProperty<float>(Actor::Property::POSITION_X),
                                                   current.y + child.GetProperty<float>(Actor::Property::POSITION_Y)));
}

Vector2 ScrollViewImpl::ContentPositionToScrollPosition(const Vector2& content) const
{
  float marginStart     = 0.0f;
  float marginStartTop  = 0.0f;
  float paddingStart    = GetViewPadding().start;
  float paddingStartTop = GetViewPadding().top;

  return Vector2(-(content.x - paddingStart - marginStart), -(content.y - paddingStartTop - marginStartTop));
}

Vector2 ScrollViewImpl::DeltaFromScrollPosition(const Vector2& scrollPosition) const
{
  return Vector2(mScrollPosition.x - scrollPosition.x, mScrollPosition.y - scrollPosition.y);
}

void ScrollViewImpl::OnPanGesture(Actor actor, const PanGesture& gesture)
{
  CancelScrollAnimation();

  switch(gesture.GetState())
  {
    case GestureState::STARTED:
      OnDragStarted(gesture);
      break;
    case GestureState::CONTINUING:
      OnDragging(gesture);
      break;
    case GestureState::FINISHED:
      // Fall through
    case GestureState::CANCELLED:
      OnDragFinished(gesture);
      break;
    default:
      break;
  }
}

void ScrollViewImpl::OnDragStarted(const PanGesture& gesture)
{
  mTotalDisplacement = Vector2::ZERO;
  mIsThresholdMet    = false;
  mStartPanPosition  = gesture.GetScreenPosition();
  mLastPanPosition   = mStartPanPosition;
}

void ScrollViewImpl::OnDragging(const PanGesture& gesture)
{
  Vector2 movement = gesture.GetDisplacement();

  if(!mIsThresholdMet)
  {
    mTotalDisplacement += movement;

    switch(mScrollDirection)
    {
      case ScrollDirection::Horizontal:
        mIsThresholdMet = std::abs(mTotalDisplacement.x) > mPanThreshold;
        break;
      case ScrollDirection::Vertical:
        mIsThresholdMet = std::abs(mTotalDisplacement.y) > mPanThreshold;
        break;
      case ScrollDirection::Both:
        mIsThresholdMet =
          (std::abs(mTotalDisplacement.x) > mPanThreshold) || (std::abs(mTotalDisplacement.y) > mPanThreshold);
        break;
    }

    if(!mIsThresholdMet)
    {
      return;
    }

    Vector2 delta = AdjustDelta(mTotalDisplacement, Vector2(mContent.GetProperty<float>(Actor::Property::POSITION_X),
                                                            mContent.GetProperty<float>(Actor::Property::POSITION_Y)));

    if(delta.LengthSquared() > 0.0001f)
    {
      SendScrollStarted();
      SendDragStarted();

      float newX = mContent.GetProperty<float>(Actor::Property::POSITION_X) + delta.x;
      float newY = mContent.GetProperty<float>(Actor::Property::POSITION_Y) + delta.y;
      mContent.SetPositionX(newX);
      mContent.SetPositionY(newY);

      SendDragging(delta.x, delta.y);
    }

    mTotalDisplacement = Vector2::ZERO;
  }
  else
  {
    Vector2 delta = AdjustDelta(movement, Vector2(mContent.GetProperty<float>(Actor::Property::POSITION_X),
                                                  mContent.GetProperty<float>(Actor::Property::POSITION_Y)));

    if(delta.LengthSquared() < 0.0001f)
    {
      return;
    }

    float newX = mContent.GetProperty<float>(Actor::Property::POSITION_X) + delta.x;
    float newY = mContent.GetProperty<float>(Actor::Property::POSITION_Y) + delta.y;
    mContent.SetPositionX(newX);
    mContent.SetPositionY(newY);

    if(!mIsScrolling)
    {
      SendDragStarted();
      SendScrollStarted();
    }

    SendDragging(delta.x, delta.y);
    mLastPanPosition = gesture.GetScreenPosition();
  }
}

void ScrollViewImpl::OnDragFinished(const PanGesture& gesture)
{
  SendDragFinished();

  Vector2 velocity = gesture.GetVelocity();
  Vector2 movement = VelocityToMovement(velocity);
  Vector2 delta    = AdjustDelta(movement);

  if(delta.LengthSquared() < 0.0001f)
  {
    SendScrollFinished();
    return;
  }

  // Apply fling (simplified - would use proper animation in full implementation)
  Vector2 newPos = mScrollPosition + delta;
  ScrollTo(newPos, true);
}

void ScrollViewImpl::SendScrollStarted()
{
  if(!mIsScrolling)
  {
    mIsScrolling              = true;
    Ui::ScrollView scrollView = Ui::ScrollView::DownCast(Self());
    mScrollStartedSignal.Emit(scrollView);
  }
}

void ScrollViewImpl::SendScrolling()
{
  if(mIsScrolling)
  {
    Ui::ScrollView scrollView = Ui::ScrollView::DownCast(Self());
    mScrollingSignal.Emit(scrollView);
  }
}

void ScrollViewImpl::SendScrollFinished()
{
  if(mIsScrolling)
  {
    mIsScrolling              = false;
    Ui::ScrollView scrollView = Ui::ScrollView::DownCast(Self());
    mScrollFinishedSignal.Emit(scrollView);
  }
}

void ScrollViewImpl::SendDragStarted()
{
  Ui::ScrollView scrollView = Ui::ScrollView::DownCast(Self());
  mDragStartedSignal.Emit(scrollView);
}

void ScrollViewImpl::SendDragging(float deltaX, float deltaY)
{
  Ui::ScrollView scrollView = Ui::ScrollView::DownCast(Self());
  mDraggingSignal.Emit(scrollView, deltaX, deltaY);
}

void ScrollViewImpl::SendDragFinished()
{
  Ui::ScrollView scrollView = Ui::ScrollView::DownCast(Self());
  mDragFinishedSignal.Emit(scrollView);
}

void ScrollViewImpl::UpdateScrollingProperties()
{
  if(!mContent)
  {
    return;
  }

  Extents padding = GetViewPadding();

  mMaximumStartX = padding.start;
  mMaximumStartY = padding.top;

  mViewportWidth  = Self().GetProperty<float>(Actor::Property::SIZE_WIDTH);
  mViewportHeight = Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT);

  mScrollableWidth  = mContent.GetProperty<float>(Actor::Property::SIZE_WIDTH) + padding.start + padding.end;
  mScrollableHeight = mContent.GetProperty<float>(Actor::Property::SIZE_HEIGHT) + padding.top + padding.bottom;

  mMinimumStartY = -std::max(0.0f, mScrollableHeight - mViewportHeight);
  mMinimumStartX = -std::max(0.0f, mScrollableWidth - mViewportWidth);

  mMinimumStartY += mMaximumStartY;
  mMinimumStartX += mMaximumStartX;

  mHasScrollableArea = (CanScrollHorizontally(mScrollDirection) && mScrollableWidth > mViewportWidth) ||
                       (CanScrollVertically(mScrollDirection) && mScrollableHeight > mViewportHeight);

  if(mScrollPosition.x < -0.5f)
  {
    float posX      = 0.0f;
    mScrollPosition = Vector2(posX, mScrollPosition.y);
  }

  ApplyScrollPosition(mScrollPosition);
}

void ScrollViewImpl::ApplyScrollPosition(const Vector2& position)
{
  if(mContent)
  {
    float posX = mMaximumStartX - position.x;
    float posY = mMaximumStartY - position.y;
    mContent.SetPositionX(posX);
    mContent.SetPositionY(posY);
  }
}

void ScrollViewImpl::CancelScrollAnimation()
{
  if(mIsScrolling)
  {
    SendScrollFinished();
  }
}

bool ScrollViewImpl::CanScrollHorizontally(ScrollDirection direction)
{
  return direction == ScrollDirection::Horizontal || direction == ScrollDirection::Both;
}

bool ScrollViewImpl::CanScrollVertically(ScrollDirection direction)
{
  return direction == ScrollDirection::Vertical || direction == ScrollDirection::Both;
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
