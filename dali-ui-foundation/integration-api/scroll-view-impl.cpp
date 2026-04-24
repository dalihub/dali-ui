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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/math/vector2.h>

#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/scroll-view-layout-manager.h>
#include <dali-ui-foundation/integration-api/scroll-view-impl.h>
//#include <dali-ui-elements/public-api/scroll-view.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SCROLLVIEW");
#endif

BaseHandle Create()
{
  return BaseHandle();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(ScrollViewImpl, LayoutImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

ScrollViewImplPtr ScrollViewImpl::New()
{
  return ScrollViewImplPtr(new ScrollViewImpl());
}

ScrollViewImpl::ScrollViewImpl()
: LayoutImpl(),
  mContent(),
  mScrollPosition(0.0f, 0.0f),
  mCurrentPosition(0.0f, 0.0f),
  mScrollableWidth(0.0f),
  mScrollableHeight(0.0f),
  mScrollDirection(ScrollDirection::Vertical),
  mMaxFlingDistance(6000.0f),
  mMinimumFlingDuration(1000),
  mMaximumFlingDuration(2000),
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
  mPanThreshold(5.0f),
  mLastPanPosition(0.0f, 0.0f),
  mIntercepting(false),
  mJustIntercepted(false),
  mPanRecognized(false),
  mPanStartPosition(0.0f, 0.0f),
  mScrollBar(ScrollBar::New())
{
  // Set initial pan gesture directions for vertical scrolling
  mPanGestureDetector.AddDirection(PanGestureDetector::DIRECTION_VERTICAL);
}

ScrollViewImpl::~ScrollViewImpl()
{
}

void ScrollViewImpl::OnInitialize()
{
  LayoutImpl::OnInitialize();
  SetLayoutManager(new ScrollViewLayoutManager());

  // Enable clipping to bounds for scrollable content
  Self().SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  // Add scroll bar to the view.
  // ScrollBar is set STANDALONE internally (in ScrollBarImpl::OnInitialize)
  // so that position updates of its internal thumb actor do not propagate
  // InvalidateMeasure up to the ScrollView and interfere with scroll
  // animations on the content.  Size tracking (MATCH_PARENT) is handled
  // by ArrangeStandaloneChildren instead.
  Self().Add(mScrollBar);
  // ScrollBar should always be on top of content
  mScrollBar.RaiseToTop();

  // Initialize scroll bar visibility
  mScrollBar.SetVerticalScrollBarVisibility(mVerticalScrollBarVisibility);
  mScrollBar.SetHorizontalScrollBarVisibility(mHorizontalScrollBarVisibility);

  // STANDALONE children are excluded from the automatic ApplyLayoutDirection
  // traversal, so propagate the current direction immediately and then track
  // future changes via signal.
  mScrollBar.SetLayoutDirection(
    static_cast<LayoutDirection::Type>(Self().GetProperty<int>(Actor::Property::LAYOUT_DIRECTION)));
  Self().LayoutDirectionChangedSignal().Connect(this, &ScrollViewImpl::OnLayoutDirectionChanged);

  // Connect to relayout signal to update scroll bar when sizes change

  // Configure pan gesture detector – DetectedSignal drives all scroll logic.
  // We do NOT call Attach() here. Instead, raw touch events are fed manually
  // via HandleEvent() from OnInterceptTouch / OnTouch so that the gesture
  // detector sees every touch even when a child view consumes it.
  mPanGestureDetector.DetectedSignal().Connect(this, &ScrollViewImpl::OnPanGesture);

  // Intercept touch before children: lets us decide per-frame whether to steal
  // the touch sequence (once the pan threshold is exceeded).
  DevelActor::InterceptTouchedSignal(Self()).Connect(this, &ScrollViewImpl::OnInterceptTouch);

  // After interception begins, subsequent events arrive here instead of children.
  Self().TouchedSignal().Connect(this, &ScrollViewImpl::OnTouch);
}

void ScrollViewImpl::SetContent(View content)
{
  // Remove old content if exists
  if(mContent)
  {
    CancelScrollAnimation();
    mContent.OnRelayoutSignal().Disconnect(this, &ScrollViewImpl::OnChildRelayout);
    Self().Remove(mContent);
    mContentPositionNotification.Reset();
  }

  mContent = content;

  // Add new content
  if(mContent)
  {
    Self().Add(mContent);
    mContent.LowerToBottom();

    // Ensure ScrollBar stays on top of content
    mScrollBar.RaiseToTop();

    // Connect to content's relayout signal to update scroll bar when content size changes
    mContent.OnRelayoutSignal().Connect(this, &ScrollViewImpl::OnChildRelayout);

    // Set up property notification to track content position changes during animation.
    // In DALi C++, POSITION property is updated every frame during animation,
    // so PropertyNotification fires as the content scrolls.
    mContentPositionNotification = mContent.AddPropertyNotification(Actor::Property::POSITION,
                                                                    StepCondition(1.0f, 0.0f));
    mContentPositionNotification.NotifySignal().Connect(this, &ScrollViewImpl::OnContentPositionChanged);

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
  if(std::abs(mScrollableWidth - width) < 0.01f)
  {
    return;
  }
  mScrollableWidth = width;
  UpdateScrollingProperties();
}

float ScrollViewImpl::GetScrollableHeight() const
{
  return mScrollableHeight;
}

void ScrollViewImpl::SetScrollableHeight(float height)
{
  if(std::abs(mScrollableHeight - height) < 0.01f)
  {
    return;
  }
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

int ScrollViewImpl::GetMinimumFlingDuration() const
{
  return mMinimumFlingDuration;
}

void ScrollViewImpl::SetMinimumFlingDuration(int duration)
{
  mMinimumFlingDuration = duration;
}

int ScrollViewImpl::GetMaximumFlingDuration() const
{
  return mMaximumFlingDuration;
}

void ScrollViewImpl::SetMaximumFlingDuration(int duration)
{
  mMaximumFlingDuration = duration;
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

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::ScrollTo] requested=[%.2f, %.2f], adjusted=[%.2f, %.2f], animation=%d, scrollableH=%.0f, viewportH=%.0f\n",
                position.x, position.y, adjustedPosition.x, adjustedPosition.y, animation, mScrollableHeight, mViewportHeight);

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
    if(!mContent)
    {
      return;
    }

    float targetPosX = mMaximumStartX - adjustedPosition.x;
    float targetPosY = mMaximumStartY - adjustedPosition.y;

    float currentPosX = mContent.GetProperty<float>(Actor::Property::POSITION_X);
    float currentPosY = mContent.GetProperty<float>(Actor::Property::POSITION_Y);

    float distX = std::abs(targetPosX - currentPosX);
    float distY = std::abs(targetPosY - currentPosY);
    float dist  = std::sqrt(distX * distX + distY * distY);
#ifdef DEBUG_SCROLL
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::ScrollTo] animate from contentPos=[%.2f, %.2f] to [%.2f, %.2f], dist=%.2f\n",
                  currentPosX, currentPosY, targetPosX, targetPosY, dist);
#endif
    if(dist < 0.5f)
    {
      SendScrollFinished();
      return;
    }

    // Calculate duration proportional to distance, clamped between min/max fling duration
    float maxScrollDist = std::max(1.0f, mScrollableHeight - mViewportHeight);
    float ratio         = std::min(1.0f, dist / maxScrollDist);
    float durationMs    = mMinimumFlingDuration + ratio * (mMaximumFlingDuration - mMinimumFlingDuration);
    float durationSec   = durationMs / 1000.0f;
#ifdef DEBUG_SCROLL
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::ScrollTo] animation duration=%.3f sec (ratio=%.3f)\n", durationSec, ratio);
#endif
    SendScrollStarted();

    mScrollAnimation = Animation::New(durationSec);
    mScrollAnimation.AnimateTo(Property(mContent, Actor::Property::POSITION_X), targetPosX, AlphaFunction::EASE_OUT);
    mScrollAnimation.AnimateTo(Property(mContent, Actor::Property::POSITION_Y), targetPosY, AlphaFunction::EASE_OUT);
    mScrollAnimation.FinishedSignal().Connect(this, &ScrollViewImpl::OnScrollAnimationFinished);
    mScrollAnimation.Play();

    mScrollPosition = adjustedPosition;
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
  mScrollBar.SetVerticalScrollBarVisibility(visibility);
}

ScrollBarVisibility ScrollViewImpl::GetHorizontalScrollBarVisibility() const
{
  return mHorizontalScrollBarVisibility;
}

void ScrollViewImpl::SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility)
{
  mHorizontalScrollBarVisibility = visibility;
  mScrollBar.SetHorizontalScrollBarVisibility(visibility);
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

  // Updated formula based on OneUIComponents commit
  float movementX = -1.0f * mFlingSensitivity * velocity.x * screenSize.width / (mMaximumFlingDuration * decelerationFactor);
  float movementY = -1.0f * mFlingSensitivity * velocity.y * screenSize.height / (mMaximumFlingDuration * decelerationFactor);
#ifdef DBUG_SCROLL
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::VelocityToMovement] velocity=[%.2f, %.2f], screenSize=[%.0f, %.0f], decelerationFactor=%.6f, maxFlingDuration=%d, raw movement=[%.2f, %.2f]\n",
                velocity.x, velocity.y, screenSize.width, screenSize.height, decelerationFactor, mMaximumFlingDuration, movementX, movementY);
#endif
  if(std::abs(movementX) > mMaxFlingDistance)
  {
    movementX = mMaxFlingDistance * (movementX > 0.0f ? 1.0f : -1.0f);
  }
  if(std::abs(movementY) > mMaxFlingDistance)
  {
    movementY = mMaxFlingDistance * (movementY > 0.0f ? 1.0f : -1.0f);
  }
#ifdef DBUG_SCROLL
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::VelocityToMovement] clamped movement=[%.2f, %.2f], maxFlingDistance=%.0f\n",
                movementX, movementY, mMaxFlingDistance);
#endif
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

  float curX = currentPosition.x;
  float curY = currentPosition.y;

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

  // If layout hasn't run yet, fall back to requested sizes so that
  // SetScrollPosition / ScrollTo work correctly before the first layout pass.
  float scrollableW = (mScrollableWidth > 0.0f) ? mScrollableWidth
                                                : (mContent ? mContent.GetRequestedWidth() : 0.0f);
  float scrollableH = (mScrollableHeight > 0.0f) ? mScrollableHeight
                                                 : (mContent ? mContent.GetRequestedHeight() : 0.0f);
  float viewportW   = (mViewportWidth > 0.0f) ? mViewportWidth : std::max(0.0f, GetRequestedWidth());
  float viewportH   = (mViewportHeight > 0.0f) ? mViewportHeight : std::max(0.0f, GetRequestedHeight());

  float maxScrollY = std::max(0.0f, scrollableH - viewportH);
  float maxScrollX = std::max(0.0f, scrollableW - viewportW);

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
  float paddingStart    = GetPadding().start;
  float paddingStartTop = GetPadding().top;

  return Vector2(-(content.x - paddingStart - marginStart), -(content.y - paddingStartTop - marginStartTop));
}

Vector2 ScrollViewImpl::DeltaFromScrollPosition(const Vector2& scrollPosition) const
{
  return Vector2(mScrollPosition.x - scrollPosition.x, mScrollPosition.y - scrollPosition.y);
}

bool ScrollViewImpl::OnInterceptTouch(Actor actor, TouchEvent touch)
{
  PointState::Type state = touch.GetState(0);

  if(state == PointState::DOWN)
  {
    // Reset all state on every new touch sequence.
    mIntercepting    = false;
    mJustIntercepted = false;
    mPanRecognized   = false;
  }

  // Always feed to the gesture detector so it can build velocity/displacement
  // history from the very beginning of the touch sequence.
  // If Pan is recognized, OnPanGesture(STARTED) fires synchronously here,
  // setting mPanRecognized = true.  Once the post-recognition threshold is
  // exceeded in a subsequent CONTINUING event, mIntercepting is set to true
  // and the touch sequence is stolen from children.
  TouchEvent& nonConstTouch = touch;
  mPanGestureDetector.HandleEvent(actor, nonConstTouch);

  return mIntercepting;
}

bool ScrollViewImpl::OnTouch(Actor actor, TouchEvent touch)
{
  if(!mIntercepting)
  {
    return false;
  }

  if(mJustIntercepted)
  {
    // This is the same event that was already fed to HandleEvent inside
    // OnInterceptTouch. Skip to avoid double-feeding the gesture detector.
    mJustIntercepted = false;
    return true;
  }

  TouchEvent& nonConstTouch = touch;
  mPanGestureDetector.HandleEvent(actor, nonConstTouch);

  PointState::Type state = touch.GetState(0);
  if(state == PointState::UP || state == PointState::INTERRUPTED)
  {
    mIntercepting = false;
  }

  return true;
}

void ScrollViewImpl::OnPanGesture(Actor actor, PanGesture gesture)
{
  switch(gesture.GetState())
  {
    case GestureState::STARTED:
    {
      // Pan is recognized by the detector. Record the position so we can
      // measure additional displacement before committing to scroll.
      mPanRecognized    = true;
      mPanStartPosition = gesture.GetScreenPosition();
      break;
    }
    case GestureState::CONTINUING:
    {
      if(!mIntercepting)
      {
        // Pan is recognized but we have not yet stolen the touch from children.
        // Wait until the finger has moved mPanThreshold px further from the
        // point where Pan was first detected before committing to scroll.
        Vector2 displacement = gesture.GetScreenPosition() - mPanStartPosition;
        bool    thresholdMet = false;
        switch(mScrollDirection)
        {
          case ScrollDirection::Vertical:
            thresholdMet = std::abs(displacement.y) > mPanThreshold;
            break;
          case ScrollDirection::Horizontal:
            thresholdMet = std::abs(displacement.x) > mPanThreshold;
            break;
          case ScrollDirection::Both:
            thresholdMet = displacement.Length() > mPanThreshold;
            break;
        }

        if(!thresholdMet)
        {
          break;
        }

        // Threshold met: steal the touch sequence and start scrolling.
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::OnPanGesture] post-recognition threshold met – intercepting\n");
        CancelScrollAnimation();
        mIntercepting    = true;
        mJustIntercepted = true;
        OnDragStarted(gesture);
        OnDragging(gesture);
      }
      else
      {
        OnDragging(gesture);
      }
      break;
    }
    case GestureState::FINISHED:
      // Fall through
    case GestureState::CANCELLED:
    {
      if(mIntercepting)
      {
        CancelScrollAnimation();
        OnDragFinished(gesture);
      }
      mPanRecognized = false;
      break;
    }
    default:
      break;
  }
}

void ScrollViewImpl::OnDragStarted(const PanGesture& gesture)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::OnDragStarted] screenPos=[%.2f, %.2f], scrollPos=[%.2f, %.2f]\n",
                gesture.GetScreenPosition().x, gesture.GetScreenPosition().y,
                mScrollPosition.x, mScrollPosition.y);

  mLastPanPosition = gesture.GetScreenPosition();
}

void ScrollViewImpl::OnDragging(const PanGesture& gesture)
{
  // Threshold check is done in OnPanGesture before OnDragging is ever called,
  // so here we unconditionally apply the displacement to the content position.
  Vector2 delta = AdjustDelta(gesture.GetDisplacement(),
                              Vector2(mContent.GetProperty<float>(Actor::Property::POSITION_X),
                                      mContent.GetProperty<float>(Actor::Property::POSITION_Y)));

  if(delta.LengthSquared() < 0.0001f)
  {
    return;
  }

  float newX = mContent.GetProperty<float>(Actor::Property::POSITION_X) + delta.x;
  float newY = mContent.GetProperty<float>(Actor::Property::POSITION_Y) + delta.y;
  // Scroll offset drives the rendered position directly. Setting the
  // Actor property bypasses layout so this does not feed back into the
  // requested layout position.
  mContent.SetProperty(Actor::Property::POSITION_X, newX);
  mContent.SetProperty(Actor::Property::POSITION_Y, newY);

  // Update scroll position from the actual content position
  mCurrentPosition = Vector2(newX, newY);
  mScrollPosition  = ContentPositionToScrollPosition(mCurrentPosition);

  if(!mIsScrolling)
  {
    SendScrollStarted();
    SendDragStarted();
  }

  SendDragging(delta.x, delta.y);
  mLastPanPosition = gesture.GetScreenPosition();

  // Update scroll bar position with the updated scroll position
  mScrollBar.UpdateScrollPosition(mScrollPosition);
}

void ScrollViewImpl::OnDragFinished(const PanGesture& gesture)
{
  SendDragFinished();

  // Update mCurrentPosition from the actual content position after dragging
  if(mContent)
  {
    mCurrentPosition = Vector2(mContent.GetProperty<float>(Actor::Property::POSITION_X),
                               mContent.GetProperty<float>(Actor::Property::POSITION_Y));
    mScrollPosition  = ContentPositionToScrollPosition(mCurrentPosition);
  }

  Vector2 velocity = gesture.GetVelocity();
  Vector2 movement = VelocityToMovement(velocity);

  // Adjust end point incorrect direction balance (from OneUIComponents commit)
  if(mScrollDirection == ScrollDirection::Horizontal)
  {
    movement = Vector2(movement.x, 0.0f);
  }
  else if(mScrollDirection == ScrollDirection::Vertical)
  {
    movement = Vector2(0.0f, movement.y);
  }

  Vector2 delta = AdjustDelta(movement, mCurrentPosition);

#ifdef DEBUG_SCROLL
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::OnDragFinished] velocity=[%.2f, %.2f], movement=[%.2f, %.2f], contentPos=[%.2f, %.2f], scrollPos=[%.2f, %.2f], delta=[%.2f, %.2f], bounds=[minY=%.2f, maxY=%.2f]\n",
                velocity.x, velocity.y, movement.x, movement.y,
                mCurrentPosition.x, mCurrentPosition.y,
                mScrollPosition.x, mScrollPosition.y,
                delta.x, delta.y,
                mMinimumStartY, mMaximumStartY);
#endif

  if(delta.LengthSquared() < 0.0001f)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::OnDragFinished] delta too small, finishing scroll\n");
    SendScrollFinished();
    return;
  }

  // delta is in content-position space; convert to scroll-position space (negate)
  Vector2 scrollDelta = Vector2(-delta.x, -delta.y);
  Vector2 newPos      = mScrollPosition + scrollDelta;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::OnDragFinished] scrollDelta=[%.2f, %.2f], target scrollPos=[%.2f, %.2f]\n",
                scrollDelta.x, scrollDelta.y, newPos.x, newPos.y);

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
    // Auto-visibility bars fade out 2 seconds after the last scroll position
    // update, handled by the hide timer inside ScrollBarImpl::ShowBar().
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

  Extents padding = GetPadding();

  mMaximumStartX = padding.start;
  mMaximumStartY = padding.top;

  mViewportWidth  = Self().GetProperty<float>(Actor::Property::SIZE_WIDTH);
  mViewportHeight = Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT);

  // mScrollableWidth/mScrollableHeight are set by ArrangeChildren via SetScrollableWidth/SetScrollableHeight
  // Do not override them here.

  mMinimumStartY = -std::max(0.0f, mScrollableHeight - mViewportHeight);
  mMinimumStartX = -std::max(0.0f, mScrollableWidth - mViewportWidth);

  mMinimumStartY += mMaximumStartY;
  mMinimumStartX += mMaximumStartX;

  mHasScrollableArea = (CanScrollHorizontally(mScrollDirection) && mScrollableWidth > mViewportWidth) ||
                       (CanScrollVertically(mScrollDirection) && mScrollableHeight > mViewportHeight);

#ifdef DEBUG_SCROLL
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::UpdateScrollingProperties] viewport=[%.0f, %.0f], scrollable=[%.0f, %.0f], bounds=[minY=%.2f, maxY=%.2f, minX=%.2f, maxX=%.2f], hasScrollableArea=%d\n",
                mViewportWidth, mViewportHeight, mScrollableWidth, mScrollableHeight,
                mMinimumStartY, mMaximumStartY, mMinimumStartX, mMaximumStartX, mHasScrollableArea);
#endif
  // Clamp scroll position to valid bounds based on scroll direction and scrollable area
  mScrollPosition = AdjustScrollPosition(mScrollPosition);

  ApplyScrollPosition(mScrollPosition);

#ifdef DEBUG_SCROLL
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::UpdateScrollingProperties] Updating scroll bar: viewport=[%.0f, %.0f], scrollable=[%.0f, %.0f]\n",
                mViewportWidth, mViewportHeight, mScrollableWidth, mScrollableHeight);
#endif
  // Update scroll bar
  mScrollBar.SetVerticalScrollBarVisibility(mVerticalScrollBarVisibility);
  mScrollBar.SetHorizontalScrollBarVisibility(mHorizontalScrollBarVisibility);
  mScrollBar.UpdateBarSize(mScrollableWidth, mScrollableHeight, mViewportWidth, mViewportHeight);
  mScrollBar.UpdateScrollPosition(mScrollPosition);
}

void ScrollViewImpl::ApplyScrollPosition(const Vector2& position)
{
  if(mContent)
  {
    float posX = mMaximumStartX - position.x;
    float posY = mMaximumStartY - position.y;

    // See note in ApplyScrollPosition delta path: scroll offsets drive the
    // rendered position directly without feeding back into layout.
    mContent.SetProperty(Actor::Property::POSITION_X, posX);
    mContent.SetProperty(Actor::Property::POSITION_Y, posY);

    // Update scroll bar position
    mScrollBar.UpdateScrollPosition(position);
  }
}

void ScrollViewImpl::CancelScrollAnimation()
{
  if(mScrollAnimation)
  {
    mScrollAnimation.Stop();
    mScrollAnimation.Reset();
  }
  if(mIsScrolling)
  {
    // Update positions from actual content state
    if(mContent)
    {
      mCurrentPosition = Vector2(mContent.GetCurrentProperty<float>(Actor::Property::POSITION_X),
                                 mContent.GetCurrentProperty<float>(Actor::Property::POSITION_Y));
      mScrollPosition  = ContentPositionToScrollPosition(mCurrentPosition);
    }
    SendScrollFinished();
  }
}

void ScrollViewImpl::OnScrollAnimationFinished(Animation animation)
{
  mScrollAnimation.Reset();

  if(mContent)
  {
    mCurrentPosition = Vector2(std::round(mContent.GetCurrentProperty<float>(Actor::Property::POSITION_X)),
                               std::round(mContent.GetCurrentProperty<float>(Actor::Property::POSITION_Y)));
    mScrollPosition  = ContentPositionToScrollPosition(mCurrentPosition);

    // Update scroll bar position
    mScrollBar.UpdateScrollPosition(mScrollPosition);
  }

  SendScrollFinished();
}

bool ScrollViewImpl::CanScrollHorizontally(ScrollDirection direction)
{
  return direction == ScrollDirection::Horizontal || direction == ScrollDirection::Both;
}

bool ScrollViewImpl::CanScrollVertically(ScrollDirection direction)
{
  return direction == ScrollDirection::Vertical || direction == ScrollDirection::Both;
}

void ScrollViewImpl::OnChildRelayout(Actor actor)
{
  UpdateScrollingProperties();
}

void ScrollViewImpl::OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type)
{
  mScrollBar.SetLayoutDirection(type);
}

void ScrollViewImpl::OnContentPositionChanged(PropertyNotification source)
{
  if(!mContent)
  {
    return;
  }

  // Read the current (animated) position of the content
  Vector3 currentPos = mContent.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  mCurrentPosition   = Vector2(currentPos.x, currentPos.y);
  mScrollPosition    = ContentPositionToScrollPosition(mCurrentPosition);

#ifdef DEBUG_SCROLL
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Content position changed! [%.1f, %.1f]", mScrollPosition.x, mScrollPosition.y);
#endif

  // Emit scrolling signal during animation
  SendScrolling();

  // Update scroll bar position
  mScrollBar.UpdateScrollPosition(mScrollPosition);
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
