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
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/math/vector2.h>

#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/unique-ptr.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/scroll-view-impl.h>
#include <dali-ui-foundation/internal/scroll-state-observer.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/layouts/scroll-view-layout-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

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

constexpr float WHEEL_SCROLL_STEP   = 120.0f; ///< Pixels scrolled per wheel notch
constexpr float WHEEL_ANIM_SEC      = 0.2f;   ///< Duration of wheel scroll animation (seconds)
constexpr float KEY_SCROLL_ANIM_SEC = 0.25f;  ///< Duration of key step-scroll animation (seconds)

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
  mMinimumFlingDuration(200),
  mMaximumFlingDuration(1200),
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
  mScrollOnFocus(true),
  mFocusScrollToPosition(ScrollToPosition::MakeVisible),
  mFocusScrollPeek(0.0f),
  mKeyScrollEnabled(false),
  mKeyScrollStep(200.0f),
  mKeyScrollLastChild(),
  mKeyScrollLastDir(FocusDirection::DOWN),
  mVerticalScrollBarVisibility(ScrollBarVisibility::Auto),
  mHorizontalScrollBarVisibility(ScrollBarVisibility::Auto),
  mPanGestureDetector(PanGestureDetector::New()),
  mPanThreshold(5.0f),
  mLastPanPosition(0.0f, 0.0f),
  mIntercepting(false),
  mJustIntercepted(false),
  mPanRecognized(false),
  mIsDragging(false),
  mDisambiguating(false),
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
  AttachLayoutManager(Dali::MakeUnique<ScrollViewLayoutManager>());

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
  Self().InterceptTouchEventSignal().Connect(this, &ScrollViewImpl::OnInterceptTouch);

  // After interception begins, subsequent events arrive here instead of children.
  Self().TouchEventSignal().Connect(this, &ScrollViewImpl::OnTouch);

  // Handle mouse wheel events for desktop / emulator scrolling.
  Self().WheelEventSignal().Connect(this, &ScrollViewImpl::OnWheelEvent);

  // Auto-scroll to focused child: subscribe to global focus changes.
  // Disconnects automatically when this object is destroyed (ConnectionTracker).
  FocusManager::Get().FocusChangedSignal().Connect(this, &ScrollViewImpl::OnFocusManagerChanged);
}

void ScrollViewImpl::SetContent(View content)
{
  // Remove old content if exists
  if(mContent)
  {
    CancelScrollAnimation();
    DevelActor::OnRelayoutSignal(mContent).Disconnect(this, &ScrollViewImpl::OnChildRelayout);
    Self().Remove(mContent);
    mContentPositionNotification.Reset();
  }

  mKeyScrollLastChild.Reset(); // stale child reference is invalid after content swap
  mContent = content;

  // Add new content
  if(mContent)
  {
    Self().Add(mContent);
    mContent.LowerToBottom();

    // Ensure ScrollBar stays on top of content
    mScrollBar.RaiseToTop();

    // Connect to content's relayout signal to update scroll bar when content size changes
    DevelActor::OnRelayoutSignal(mContent).Connect(this, &ScrollViewImpl::OnChildRelayout);

    // Set up property notification to track content position changes during animation.
    // In DALi C++, POSITION property is updated every frame during animation,
    // so PropertyNotification fires as the content scrolls.
    mContentPositionNotification = mContent.AddPropertyNotification(Actor::Property::POSITION,
                                                                    StepCondition(1.0f, 0.0f));
    mContentPositionNotification.NotifySignal().Connect(this, &ScrollViewImpl::OnContentPositionChanged);

    // Keep edge-effect sources in sync with the content view.
    if(mStartEdgeEffect) mStartEdgeEffect.SetSource(mContent);
    if(mEndEdgeEffect) mEndEdgeEffect.SetSource(mContent);

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
  OnScrollableAreaChanged();
}

float ScrollViewImpl::GetScrollableHeight() const
{
  return mScrollableHeight;
}

float ScrollViewImpl::GetViewportWidth() const
{
  return mViewportWidth;
}

float ScrollViewImpl::GetViewportHeight() const
{
  return mViewportHeight;
}

void ScrollViewImpl::SetScrollableHeight(float height)
{
  if(std::abs(mScrollableHeight - height) < 0.01f)
  {
    return;
  }
  mScrollableHeight = height;
  UpdateScrollingProperties();
  OnScrollableAreaChanged();
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

void ScrollViewImpl::ScrollToWithDuration(const Vector2& position, float durationSec)
{
  CancelScrollAnimation();

  if(!mContent)
  {
    return;
  }

  Vector2 adjustedPosition = AdjustScrollPosition(position);
  float   targetPosX       = mMaximumStartX - adjustedPosition.x;
  float   targetPosY       = mMaximumStartY - adjustedPosition.y;

  float distX = std::abs(targetPosX - mCurrentPosition.x);
  float distY = std::abs(targetPosY - mCurrentPosition.y);
  if(std::sqrt(distX * distX + distY * distY) < 0.5f)
  {
    SendScrollFinished();
    return;
  }

  SendScrollStarted();
  mScrollAnimation = Animation::New(durationSec);
  mScrollAnimation.AnimateTo(Property(mContent, Actor::Property::POSITION_X), targetPosX, AlphaFunction::EASE_OUT);
  mScrollAnimation.AnimateTo(Property(mContent, Actor::Property::POSITION_Y), targetPosY, AlphaFunction::EASE_OUT);
  mScrollAnimation.FinishedSignal().Connect(this, &ScrollViewImpl::OnScrollAnimationFinished);
  mScrollAnimation.Play();
  mScrollPosition = adjustedPosition;
}

void ScrollViewImpl::ScrollTo(View child, bool animation, ScrollToPosition scrollToPosition)
{
  if(!mContent)
  {
    return;
  }

  Vector2    childScrollPos = GetScrollPositionForChild(child, Vector2::ZERO);
  const bool wasMakeVisible = (scrollToPosition == ScrollToPosition::MakeVisible);

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
        // Apply peek: scroll a little further past the item edge to hint more content
        if(mFocusScrollPeek > 0.0f)
        {
          if(posX > scrollPos.x)
            posX += mFocusScrollPeek;
          else if(posX < scrollPos.x)
            posX -= mFocusScrollPeek;
          if(posY > scrollPos.y)
            posY += mFocusScrollPeek;
          else if(posY < scrollPos.y)
            posY -= mFocusScrollPeek;
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
      // Apply peek: scroll further past the bottom/right edge
      if(wasMakeVisible && mFocusScrollPeek > 0.0f)
      {
        if(mScrollDirection != ScrollDirection::Horizontal) childScrollPos.y += mFocusScrollPeek;
        if(mScrollDirection != ScrollDirection::Vertical) childScrollPos.x += mFocusScrollPeek;
      }
      break;
    case ScrollToPosition::Start:
    default:
      // Apply peek: scroll further past the top/left edge
      if(wasMakeVisible && mFocusScrollPeek > 0.0f)
      {
        if(mScrollDirection != ScrollDirection::Horizontal) childScrollPos.y -= mFocusScrollPeek;
        if(mScrollDirection != ScrollDirection::Vertical) childScrollPos.x -= mFocusScrollPeek;
      }
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

void ScrollViewImpl::SetScrollOnFocus(bool enable)
{
  mScrollOnFocus = enable;
}

bool ScrollViewImpl::GetScrollOnFocus() const
{
  return mScrollOnFocus;
}

void ScrollViewImpl::SetFocusScrollToPosition(ScrollToPosition position)
{
  mFocusScrollToPosition = position;
}

ScrollToPosition ScrollViewImpl::GetFocusScrollToPosition() const
{
  return mFocusScrollToPosition;
}

void ScrollViewImpl::SetFocusScrollPeek(float peek)
{
  mFocusScrollPeek = std::max(0.0f, peek);
}

float ScrollViewImpl::GetFocusScrollPeek() const
{
  return mFocusScrollPeek;
}

void ScrollViewImpl::SetKeyScrollEnabled(bool enable)
{
  mKeyScrollEnabled = enable;
  if(!enable)
    mKeyScrollLastChild.Reset(); // case-3 state is only valid while key-scroll is active
}

bool ScrollViewImpl::IsKeyScrollEnabled() const
{
  return mKeyScrollEnabled;
}

void ScrollViewImpl::SetKeyScrollStep(float step)
{
  // Clamp to a small positive minimum so the step value is always meaningful.
  // step==0 would make distance<=0 always true (focus always jumps, ignoring key-scroll).
  // step<0 is nonsensical and would prevent focus from ever moving.
  mKeyScrollStep = std::max(1.0f, step);
}

float ScrollViewImpl::GetKeyScrollStep() const
{
  return mKeyScrollStep;
}

namespace
{
// Returns true when a View can receive keyboard focus. Mirrors the conditions
// DALi's global FocusFinder applies so we never propose a candidate that the
// FocusManager would then reject.
// ENABLED (not SENSITIVE) is the correct "enabled/disabled"
// flag for keyboard focus: SENSITIVE controls touch hit-testing only.
bool IsFocusableCandidate(View view)
{
  return view &&
         view.GetProperty<bool>(Actor::Property::FOCUSABLE) &&
         view.GetProperty<bool>(Actor::Property::VISIBLE) &&
         view.GetProperty<bool>(Actor::Property::ENABLED);
}

} // namespace

bool ScrollViewImpl::IsChildInViewport(View child) const
{
  Vector2 pos  = GetScrollPositionForChild(child, Vector2::ZERO);
  Vector2 size = child.GetProperty<Vector2>(Actor::Property::SIZE);
  Vector2 sp   = GetScrollPosition();

  bool visibleY = true;
  bool visibleX = true;

  if(CanScrollVertically(mScrollDirection))
    visibleY = (pos.y + size.y) > sp.y && pos.y < (sp.y + mViewportHeight);
  if(CanScrollHorizontally(mScrollDirection))
    visibleX = (pos.x + size.x) > sp.x && pos.x < (sp.x + mViewportWidth);

  return visibleX && visibleY;
}

View ScrollViewImpl::OnFocusRequested()
{
  View selfView = View::DownCast(Self());

  // When key-scroll is disabled the ScrollView shouldn't absorb focus into
  // Self() — use the base class implementation which delegates to the first
  // focusable child.  return View() would cause FocusManager::RequestFocus()
  // to fail outright rather than falling back to child delegation.
  if(!mKeyScrollEnabled || !mContent) return LayoutImpl::OnFocusRequested();

  // Always focus the content item nearest to the entry edge: the ScrollView
  // should self-focus (for OnKeyEvent step-scroll) only when there are no
  // focusable items at all (nofocus case).  This makes entering from any
  // external direction behave consistently — the item at the approached edge
  // receives focus directly, without the ScrollView acting as an intermediary.
  //
  // Guard: skip entry-edge detection when the previous focus is already one of
  // our content descendants (happens in case 3, where OnFocusNavigationRequested
  // returns Self() and FocusManager calls back into OnFocusRequested).  Content
  // items can have their world-position center outside the visible viewport when
  // they are only partially on-screen, which would otherwise falsely trigger the
  // "approaching from below/above" branch and re-focus the same item.
  View prevFocus = FocusManager::Get().GetCurrentFocusView();

  if(prevFocus && !IsDescendantOfContent(prevFocus))
  {
    Vector3 prevPos  = prevFocus.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector3 selfPos  = selfView.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector3 selfSize = selfView.GetProperty<Vector3>(Actor::Property::SIZE);

    float selfTop    = selfPos.y - selfSize.y * 0.5f;
    float selfBottom = selfPos.y + selfSize.y * 0.5f;
    float selfLeft   = selfPos.x - selfSize.x * 0.5f;
    float selfRight  = selfPos.x + selfSize.x * 0.5f;

    FocusDirection edgeDirection = FocusDirection::UP;
    Vector2        edgePos;
    bool           hasEdge = false;

    // Vertical axis takes priority; horizontal checked only if vertical doesn't fire.
    if(!hasEdge && CanScrollVertically(mScrollDirection))
    {
      if(prevPos.y < selfTop)
      {
        // Approaching from above → item nearest to top of current viewport.
        edgeDirection = FocusDirection::DOWN;
        edgePos       = Vector2(0.0f, GetScrollPosition().y);
        hasEdge       = true;
      }
      else if(prevPos.y > selfBottom)
      {
        // Approaching from below → item nearest to bottom of current viewport.
        edgeDirection = FocusDirection::UP;
        edgePos       = Vector2(0.0f, GetScrollPosition().y + mViewportHeight);
        hasEdge       = true;
      }
    }
    if(!hasEdge && CanScrollHorizontally(mScrollDirection))
    {
      if(prevPos.x < selfLeft)
      {
        edgeDirection = FocusDirection::RIGHT;
        edgePos       = Vector2(GetScrollPosition().x, 0.0f);
        hasEdge       = true;
      }
      else if(prevPos.x > selfRight)
      {
        edgeDirection = FocusDirection::LEFT;
        edgePos       = Vector2(GetScrollPosition().x + mViewportWidth, 0.0f);
        hasEdge       = true;
      }
    }

    if(hasEdge)
    {
      View edgeItem = FindNextFocusableInContent(View(), edgePos, edgeDirection);
      // Only return the edge item if it is already visible in the current viewport.
      // If it is off-screen, fall through to Self() so OnKeyEvent step-scrolls
      // toward it — the same path used when there are no focusable items at all.
      if(edgeItem && IsChildInViewport(edgeItem)) return edgeItem;
      // Entering from outside with no visible item: clear case-3 skip memory so
      // OnKeyEvent treats items freshly (the previous last-child is no longer relevant).
      mKeyScrollLastChild.Reset();
    }
  }

  // No visible item at the entry edge (or no prevFocus context).
  // Return Self() so OnKeyEvent can step-scroll toward the first/last focusable
  // item.  Only valid if ScrollView is keyboard-focusable; otherwise delegate to
  // the base class so FocusManager can use child delegation instead of failing.
  if(!selfView.GetProperty<bool>(Actor::Property::FOCUSABLE))
    return LayoutImpl::OnFocusRequested();
  return selfView;
}

bool ScrollViewImpl::OnKeyEvent(const Dali::KeyEvent& event)
{
  if(!mKeyScrollEnabled || !mContent) return false;
  if(event.GetState() != Dali::KeyEvent::DOWN) return false;

  const Dali::String keyName = event.GetKeyName();

  // HOME / END: intercepted at ScrollView level regardless of which view has focus,
  // mirroring Android ScrollView.onKeyDown().
  // Assumption: DALi dispatches key events to the focused actor and, if not consumed
  // (returns false), propagates them up the parent chain.  If DALi ever changes to
  // not bubble key events, Home/End will only work when ScrollView itself is focused.
  if(keyName == "Home" || keyName == "End")
  {
    bool toEnd = (keyName == "End");
    if(!IsDirectionCompatible(toEnd ? FocusDirection::DOWN : FocusDirection::UP)) return false;
    View result = FullScrollAndFocus(toEnd);
    if(result) FocusManager::Get().SetCurrentFocusView(result);
    return true;
  }

  // All other navigation keys: only act when ScrollView itself is the focused view.
  // When a content descendant is focused, OnFocusNavigationRequested handles key-scroll;
  // acting here too would fire scroll twice per key press.
  if(FocusManager::Get().GetCurrentFocusView() != View::DownCast(Self())) return false;

  FocusDirection direction;
  if(keyName == "Up")
    direction = FocusDirection::UP;
  else if(keyName == "Down")
    direction = FocusDirection::DOWN;
  else if(keyName == "Left")
    direction = FocusDirection::LEFT;
  else if(keyName == "Right")
    direction = FocusDirection::RIGHT;
  else if(keyName == "Prior" || keyName == "Page_Up")
    direction = FocusDirection::PAGE_UP;
  else if(keyName == "Next" || keyName == "Page_Down")
    direction = FocusDirection::PAGE_DOWN;
  else
    return false;

  if(!IsDirectionCompatible(direction)) return false;

  // PAGE_* when ScrollView is focused: FocusManager fires first (via sceneHolder.
  // KeyEventSignal) but skips MoveFocus when the focused view lacks FOCUS_INDICATED
  // (as in case-3 mode — confirmed by arrow keys working correctly in that state).
  // The key then reaches here.  If FocusManager did call MoveFocus and changed focus,
  // it would dispatch the key to the new view instead, so no double-scroll can occur.
  if(direction == FocusDirection::PAGE_UP || direction == FocusDirection::PAGE_DOWN)
  {
    View result = PageScrollAndFocus(direction);
    if(result) FocusManager::Get().SetCurrentFocusView(result);
    return true;
  }

  // Arrow keys: use the TRAILING viewport edge as the search anchor so items that
  // have just entered the viewport from the leading side are still "ahead" of it.
  Vector2 scrollPos = GetScrollPosition();
  Vector2 viewportEdge;
  switch(direction)
  {
    case FocusDirection::DOWN:
      viewportEdge = Vector2(0.0f, scrollPos.y);
      break;
    case FocusDirection::UP:
      viewportEdge = Vector2(0.0f, scrollPos.y + mViewportHeight);
      break;
    case FocusDirection::RIGHT:
      viewportEdge = Vector2(scrollPos.x, 0.0f);
      break;
    default: // LEFT
      viewportEdge = Vector2(scrollPos.x + mViewportWidth, 0.0f);
      break;
  }

  // Case-3 step-scroll mode: a content item transferred focus to ScrollView because
  // it was the last focusable item but the content could still scroll.  While in this
  // mode skip item re-selection to avoid the focus cycle (item → case3 → ScrollView →
  // item → …).  Any direction reversal immediately clears the mode.
  if(mKeyScrollLastChild && direction != mKeyScrollLastDir)
    mKeyScrollLastChild.Reset();

  if(mKeyScrollLastChild) // implies direction == mKeyScrollLastDir
  {
    if(IsAtScrollBoundary(direction))
    {
      mKeyScrollLastChild.Reset();
      TriggerKeyEdgeFeedback(direction);
      return false;
    }
    ScrollByKeyDirection(direction);
    return true;
  }

  // Normal arrow-key mode: focus the nearest visible item, or step-scroll toward it.
  View next = FindNextFocusableInContent(View(), viewportEdge, direction);
  if(next && IsChildInViewport(next))
  {
    mKeyScrollLastChild.Reset();
    FocusManager::Get().SetCurrentFocusView(next);
    return true;
  }

  if(IsAtScrollBoundary(direction))
  {
    TriggerKeyEdgeFeedback(direction);
    return false;
  }

  ScrollByKeyDirection(direction);
  return true;
}

View ScrollViewImpl::OnFocusNavigationRequested(View currentFocusedView, FocusDirection direction)
{
  if(!mKeyScrollEnabled || !mContent) return View();

  // OnFocusNavigationRequested is only called on PARENT layouts (FindNextFocusByParentNavigation
  // traverses GetParent() chain).  When ScrollView itself is the focused view, it is never
  // called here — PAGE_* and HOME/END are handled in OnKeyEvent instead.
  if(!IsDescendantOfContent(currentFocusedView)) return View();

  View selfView = View::DownCast(Self());
  if(!IsDirectionCompatible(direction)) return View();

  // PAGE_*: scroll one full page and focus the topmost / bottommost item in the
  // destination viewport (mirrors Android ScrollView.pageScroll behavior).
  if(direction == FocusDirection::PAGE_UP || direction == FocusDirection::PAGE_DOWN)
    return PageScrollAndFocus(direction);

  // Arrow key navigation below.
  Vector2 currentPos = GetScrollPositionForChild(currentFocusedView, Vector2::ZERO);
  View    next       = FindNextFocusableInContent(currentFocusedView, currentPos, direction);

  if(!next)
  {
    if(IsAtScrollBoundary(direction))
    {
      // At boundary with no next item: let FocusFinder exit to a neighboring view.
      TriggerKeyEdgeFeedback(direction);
      return View();
    }
    // Case 3: past the last focusable item, content can still scroll.
    // Transfer focus to Self() so subsequent arrow presses are handled by OnKeyEvent.
    // Scroll one step immediately so this key press is not wasted.
    ScrollByKeyDirection(direction);
    if(selfView.GetProperty<bool>(Actor::Property::FOCUSABLE))
    {
      mKeyScrollLastChild = currentFocusedView;
      mKeyScrollLastDir   = direction;
      return selfView;
    }
    return currentFocusedView;
  }

  // Case 5: next item is within one step of the viewport edge — focus it directly.
  // ScrollOnFocus will animate it into full view.
  Vector2 scrollPos = GetScrollPosition();
  Vector2 nextPos   = GetScrollPositionForChild(next, Vector2::ZERO);
  float   distance  = 0.0f;
  switch(direction)
  {
    case FocusDirection::DOWN:
      distance = std::max(0.0f, nextPos.y - (scrollPos.y + mViewportHeight));
      break;
    case FocusDirection::UP:
      distance = std::max(0.0f, scrollPos.y - nextPos.y);
      break;
    case FocusDirection::RIGHT:
      distance = std::max(0.0f, nextPos.x - (scrollPos.x + mViewportWidth));
      break;
    case FocusDirection::LEFT:
      distance = std::max(0.0f, scrollPos.x - nextPos.x);
      break;
    default:
      distance = std::numeric_limits<float>::max();
      break;
  }

  if(distance < mKeyScrollStep)
  {
    mKeyScrollLastChild.Reset();
    return next;
  }

  // Case 4b: boundary override — scrolling is a no-op, focus next directly.
  if(IsAtScrollBoundary(direction))
  {
    mKeyScrollLastChild.Reset();
    return next;
  }

  // Case 4: step-scroll toward next item, keep current focus to block FocusFinder.
  ScrollByKeyDirection(direction);
  mKeyScrollLastChild.Reset();
  return currentFocusedView;
}

bool ScrollViewImpl::IsDirectionCompatible(FocusDirection direction) const
{
  switch(direction)
  {
    case FocusDirection::UP:
    case FocusDirection::DOWN:
    case FocusDirection::PAGE_UP:
    case FocusDirection::PAGE_DOWN:
      return mScrollDirection == ScrollDirection::Vertical || mScrollDirection == ScrollDirection::Both;
    case FocusDirection::LEFT:
    case FocusDirection::RIGHT:
      return mScrollDirection == ScrollDirection::Horizontal || mScrollDirection == ScrollDirection::Both;
    default:
      return false;
  }
}

View ScrollViewImpl::FindNextFocusableInContent(View currentFocusedView, const Vector2& currentPos, FocusDirection direction) const
{
  View  best;
  float bestDist = std::numeric_limits<float>::max();
  CollectNextFocusCandidate(mContent, currentFocusedView, currentPos, direction, best, bestDist);
  return best;
}

void ScrollViewImpl::CollectNextFocusCandidate(View container, View excludeView, const Vector2& currentPos,
                                               FocusDirection direction, View& bestView, float& bestDist) const
{
  for(uint32_t i = 0; i < GetImpl(container).GetChildViewCount(); ++i)
  {
    View child = GetImpl(container).GetChildViewAt(i);
    if(!child)
    {
      continue;
    }

    if(child != excludeView && IsFocusableCandidate(child))
    {
      Vector2 childPos = GetScrollPositionForChild(child, Vector2::ZERO);
      if(IsAheadInDirection(currentPos, childPos, direction))
      {
        float dist = FocusAxisDistance(currentPos, childPos, direction);
        if(dist < bestDist)
        {
          bestDist = dist;
          bestView = child;
        }
      }
    }

    // Only recurse into a container if it permits keyboard focus on its children.
    // This mirrors the FocusManager's own traversal rule, so we never propose a
    // candidate that the FocusManager itself would skip.
    const bool childrenFocusable = child.GetProperty<bool>(Actor::Property::ALLOW_DESCENDANT_FOCUS);
    if(childrenFocusable)
    {
      CollectNextFocusCandidate(child, excludeView, currentPos, direction, bestView, bestDist);
    }
  }
}

View ScrollViewImpl::PageScrollAndFocus(FocusDirection direction)
{
  mKeyScrollLastChild.Reset();

  const float maxScrollY = std::max(0.0f, mScrollableHeight - mViewportHeight);

  Vector2 pos = GetScrollPosition();
  // PAGE_UP/PAGE_DOWN always move the Y axis.  In ScrollDirection::Both the X
  // position is kept unchanged because there is no horizontal equivalent of a
  // "page" — horizontal paging would require PAGE_LEFT/PAGE_RIGHT directions.
  float destY = pos.y;
  switch(direction)
  {
    case FocusDirection::PAGE_DOWN:
      destY = std::min(pos.y + mViewportHeight, maxScrollY);
      break;
    case FocusDirection::PAGE_UP:
      destY = std::max(pos.y - mViewportHeight, 0.0f);
      break;
    default:
      break;
  }
  ScrollToWithDuration(Vector2(pos.x, destY), KEY_SCROLL_ANIM_SEC);

  // Find the best item that intersects the destination viewport.
  // Use full rectangle intersection (item top/bottom vs dest top/bottom) so that
  // a large item whose leading edge is outside the viewport but whose body
  // overlaps it is correctly included — matching IsChildInViewport's logic.
  float destTop    = destY;
  float destBottom = destY + mViewportHeight;

  View    bestItem;
  Vector2 anchor;
  if(direction == FocusDirection::PAGE_DOWN)
  {
    // topmost focusable item that intersects destination viewport
    anchor   = Vector2(0.0f, destTop - 1.0f);
    bestItem = FindNextFocusableInContent(View(), anchor, FocusDirection::DOWN);
    if(bestItem)
    {
      Vector2 itemPos  = GetScrollPositionForChild(bestItem, Vector2::ZERO);
      Vector2 itemSize = bestItem.GetProperty<Vector2>(Actor::Property::SIZE);
      if(itemPos.y >= destBottom || (itemPos.y + itemSize.y) <= destTop)
        bestItem = View();
    }
  }
  else // PAGE_UP
  {
    // bottommost focusable item that intersects destination viewport
    anchor   = Vector2(0.0f, destBottom + 1.0f);
    bestItem = FindNextFocusableInContent(View(), anchor, FocusDirection::UP);
    if(bestItem)
    {
      Vector2 itemPos  = GetScrollPositionForChild(bestItem, Vector2::ZERO);
      Vector2 itemSize = bestItem.GetProperty<Vector2>(Actor::Property::SIZE);
      if((itemPos.y + itemSize.y) <= destTop || itemPos.y >= destBottom)
        bestItem = View();
    }
  }

  if(bestItem) return bestItem;

  // No focusable item in destination viewport.
  bool atBoundary = (direction == FocusDirection::PAGE_DOWN)
                      ? (destY >= maxScrollY - 1.0f)
                      : (destY <= 1.0f);
  if(atBoundary) TriggerKeyEdgeFeedback(direction);

  View selfView = View::DownCast(Self());
  return selfView.GetProperty<bool>(Actor::Property::FOCUSABLE) ? selfView : View();
}

View ScrollViewImpl::FullScrollAndFocus(bool toEnd)
{
  mKeyScrollLastChild.Reset();

  const float maxScrollY = std::max(0.0f, mScrollableHeight - mViewportHeight);
  float       destY      = toEnd ? maxScrollY : 0.0f;

  Vector2 pos = GetScrollPosition();
  ScrollToWithDuration(Vector2(pos.x, destY), KEY_SCROLL_ANIM_SEC);

  float destTop    = destY;
  float destBottom = destY + mViewportHeight;

  View    bestItem;
  Vector2 anchor;
  if(!toEnd)
  {
    // Home: topmost focusable item intersecting [destTop, destBottom]
    anchor   = Vector2(0.0f, destTop - 1.0f);
    bestItem = FindNextFocusableInContent(View(), anchor, FocusDirection::DOWN);
    if(bestItem)
    {
      Vector2 itemPos  = GetScrollPositionForChild(bestItem, Vector2::ZERO);
      Vector2 itemSize = bestItem.GetProperty<Vector2>(Actor::Property::SIZE);
      if(itemPos.y >= destBottom || (itemPos.y + itemSize.y) <= destTop)
        bestItem = View();
    }
  }
  else
  {
    // End: bottommost focusable item intersecting [destTop, destBottom]
    anchor   = Vector2(0.0f, destBottom + 1.0f);
    bestItem = FindNextFocusableInContent(View(), anchor, FocusDirection::UP);
    if(bestItem)
    {
      Vector2 itemPos  = GetScrollPositionForChild(bestItem, Vector2::ZERO);
      Vector2 itemSize = bestItem.GetProperty<Vector2>(Actor::Property::SIZE);
      if((itemPos.y + itemSize.y) <= destTop || itemPos.y >= destBottom)
        bestItem = View();
    }
  }

  if(bestItem) return bestItem;

  View selfView = View::DownCast(Self());
  return selfView.GetProperty<bool>(Actor::Property::FOCUSABLE) ? selfView : View();
}

bool ScrollViewImpl::IsAtScrollBoundary(FocusDirection direction) const
{
  // Use the same clamping logic as AdjustScrollPosition so the boundary
  // detection is consistent with what ScrollTo / ScrollByKeyDirection enforce.
  // mMaximumStartY / mMaximumStartX hold the content actor's resting position
  // (= padding offset), NOT the maximum scroll amount, so they must not be
  // used directly here.
  constexpr float kEpsilon = 1.0f; // 1 px tolerance for floating-point drift

  const float maxScrollY = std::max(0.0f, mScrollableHeight - mViewportHeight);
  const float maxScrollX = std::max(0.0f, mScrollableWidth - mViewportWidth);

  switch(direction)
  {
    case FocusDirection::DOWN:
    case FocusDirection::PAGE_DOWN:
      return mScrollPosition.y >= maxScrollY - kEpsilon;
    case FocusDirection::UP:
    case FocusDirection::PAGE_UP:
      return mScrollPosition.y <= kEpsilon;
    case FocusDirection::RIGHT:
      return mScrollPosition.x >= maxScrollX - kEpsilon;
    case FocusDirection::LEFT:
      return mScrollPosition.x <= kEpsilon;
    default:
      return false;
  }
}

void ScrollViewImpl::TriggerKeyEdgeFeedback(FocusDirection direction)
{
  // velocity chosen so |vel| * ABSORB_FACTOR (~0.05) ≈ 30 px displacement.
  constexpr float KEY_ABSORB_VEL = 600.0f;

  switch(direction)
  {
    case FocusDirection::UP:
    case FocusDirection::PAGE_UP:
      if(mStartEdgeEffect)
        mStartEdgeEffect.OnAbsorb(KEY_ABSORB_VEL);
      break;
    case FocusDirection::DOWN:
    case FocusDirection::PAGE_DOWN:
      if(mEndEdgeEffect)
        mEndEdgeEffect.OnAbsorb(-KEY_ABSORB_VEL);
      break;
    case FocusDirection::LEFT:
    {
      if(mStartEdgeEffect) mStartEdgeEffect.OnAbsorb(KEY_ABSORB_VEL);
      break;
    }
    case FocusDirection::RIGHT:
    {
      if(mEndEdgeEffect) mEndEdgeEffect.OnAbsorb(-KEY_ABSORB_VEL);
      break;
    }
    default:
      break;
  }
}

void ScrollViewImpl::ScrollByKeyDirection(FocusDirection direction)
{
  Vector2 pos = GetScrollPosition();
  switch(direction)
  {
    case FocusDirection::UP:
      ScrollToWithDuration(Vector2(pos.x, pos.y - mKeyScrollStep), KEY_SCROLL_ANIM_SEC);
      break;
    case FocusDirection::DOWN:
      ScrollToWithDuration(Vector2(pos.x, pos.y + mKeyScrollStep), KEY_SCROLL_ANIM_SEC);
      break;
    case FocusDirection::LEFT:
      ScrollToWithDuration(Vector2(pos.x - mKeyScrollStep, pos.y), KEY_SCROLL_ANIM_SEC);
      break;
    case FocusDirection::RIGHT:
      ScrollToWithDuration(Vector2(pos.x + mKeyScrollStep, pos.y), KEY_SCROLL_ANIM_SEC);
      break;
    case FocusDirection::PAGE_UP:
      ScrollToWithDuration(Vector2(pos.x, pos.y - mViewportHeight), KEY_SCROLL_ANIM_SEC);
      break;
    case FocusDirection::PAGE_DOWN:
      ScrollToWithDuration(Vector2(pos.x, pos.y + mViewportHeight), KEY_SCROLL_ANIM_SEC);
      break;
    default:
      break;
  }
}

float ScrollViewImpl::FocusAxisDistance(const Vector2& from, const Vector2& to, FocusDirection direction)
{
  switch(direction)
  {
    case FocusDirection::UP:
    case FocusDirection::PAGE_UP:
      return from.y - to.y;
    case FocusDirection::DOWN:
    case FocusDirection::PAGE_DOWN:
      return to.y - from.y;
    case FocusDirection::LEFT:
      return from.x - to.x;
    case FocusDirection::RIGHT:
      return to.x - from.x;
    default:
      return std::numeric_limits<float>::max();
  }
}

bool ScrollViewImpl::IsAheadInDirection(const Vector2& from, const Vector2& to, FocusDirection direction)
{
  return FocusAxisDistance(from, to, direction) > 0.0f;
}

void ScrollViewImpl::OnFocusManagerChanged(View from, View to)
{
  if(!mScrollOnFocus || !mContent || !to)
  {
    return;
  }
  // When key-scroll fires it returns currentFocusedView from OnFocusNavigationRequested,
  // which causes FocusManager to emit FocusChangedSignal(same, same).
  // Scrolling to the same view here would undo the just-completed step scroll,
  // especially when mFocusScrollToPosition is Start / End / Center (no early exit).
  if(from == to)
  {
    return;
  }
  if(IsDescendantOfContent(to))
  {
    ScrollTo(to, true, mFocusScrollToPosition);
  }
}

bool ScrollViewImpl::IsDescendantOfContent(View view) const
{
  Actor current = view.GetParent();
  while(current)
  {
    if(current == mContent)
    {
      return true;
    }
    current = current.GetParent();
  }
  return false;
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

void ScrollViewImpl::SetStartEdgeEffect(Ui::EdgeEffect effect)
{
  mStartEdgeEffect = effect;
  if(mStartEdgeEffect && mContent)
  {
    mStartEdgeEffect.SetSource(mContent);
  }
}

Ui::EdgeEffect ScrollViewImpl::GetStartEdgeEffect() const
{
  return mStartEdgeEffect;
}

void ScrollViewImpl::SetEndEdgeEffect(Ui::EdgeEffect effect)
{
  mEndEdgeEffect = effect;
  if(mEndEdgeEffect && mContent)
  {
    mEndEdgeEffect.SetSource(mContent);
  }
}

Ui::EdgeEffect ScrollViewImpl::GetEndEdgeEffect() const
{
  return mEndEdgeEffect;
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

Vector2 ScrollViewImpl::VelocityTracker::Compute() const
{
  if(count < 2)
    return Vector2::ZERO;

  int      newestIdx  = (writeIdx - 1 + CAPACITY) % CAPACITY;
  uint32_t newestTime = samples[newestIdx].timeMs;
  uint32_t cutoff     = (newestTime > HORIZON_MS) ? newestTime - HORIZON_MS : 0u;

  float sumVX = 0.0f, sumVY = 0.0f, sumW = 0.0f;
  int   prevIdx = newestIdx;

  for(int i = 1; i < count; ++i)
  {
    int curIdx = (newestIdx - i + CAPACITY) % CAPACITY;
    if(samples[curIdx].timeMs < cutoff)
      break;

    float dt = static_cast<float>(samples[prevIdx].timeMs) - static_cast<float>(samples[curIdx].timeMs);
    if(dt > 0.5f)
    {
      float vx  = (samples[prevIdx].pos.x - samples[curIdx].pos.x) / dt;
      float vy  = (samples[prevIdx].pos.y - samples[curIdx].pos.y) / dt;
      float age = static_cast<float>(newestTime - samples[prevIdx].timeMs);
      float w   = std::exp(-age / WEIGHT_TAU);
      sumVX += w * vx;
      sumVY += w * vy;
      sumW += w;
    }
    prevIdx = curIdx;
  }

  if(sumW < 1.0e-6f)
    return Vector2::ZERO;

  return Vector2(sumVX / sumW, sumVY / sumW);
}

Vector2 ScrollViewImpl::VelocityToMovement(const Vector2& velocity) const
{
  float decelerationFactor = std::log(mDecelerationRate);

  // VelocityToMovement is called during gesture handling, so the window is guaranteed to be available.
  Dali::PositionSize windowSize = Window::Get(Self()).GetPositionSize();
  Vector2            screenSize(static_cast<float>(windowSize.width), static_cast<float>(windowSize.height));

  // Updated formula based on OneUIComponents commit
  float movementX = -1.0f * mFlingSensitivity * velocity.x * screenSize.width / (mMaximumFlingDuration * decelerationFactor);
  float movementY = -1.0f * mFlingSensitivity * velocity.y * screenSize.height / (mMaximumFlingDuration * decelerationFactor);
#ifdef DEBUG_SCROLL
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
#ifdef DEBUG_SCROLL
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

bool ScrollViewImpl::OnWheelEvent(Actor /*actor*/, WheelEvent event)
{
  if(!mContent || !mHasScrollableArea)
  {
    return false;
  }

  // GetDelta(): positive = roll down / clockwise, negative = roll up / counter-clockwise.
  const int32_t delta       = event.GetDelta();
  const int32_t hwDirection = event.GetDirection(); // 0 = vertical wheel, 1 = horizontal wheel

  const bool canV = CanScrollVertically(mScrollDirection);
  const bool canH = CanScrollHorizontally(mScrollDirection);

  float stepX = 0.0f;
  float stepY = 0.0f;

  if(hwDirection == 0)
  {
    if(canV)
    {
      stepY = delta * WHEEL_SCROLL_STEP;
    }
    else if(canH)
    {
      // Redirect vertical wheel to horizontal axis when only horizontal scroll is available.
      stepX = delta * WHEEL_SCROLL_STEP;
    }
  }
  else // hwDirection == 1
  {
    if(canH)
    {
      stepX = delta * WHEEL_SCROLL_STEP;
    }
  }

  if(stepX == 0.0f && stepY == 0.0f)
  {
    return false;
  }

  // Cancel any ongoing fling/animation and pick up the live content position.
  CancelScrollAnimation();

  Vector2 newPosition = AdjustScrollPosition(Vector2(mScrollPosition.x + stepX, mScrollPosition.y + stepY));

  const float targetPosX  = mMaximumStartX - newPosition.x;
  const float targetPosY  = mMaximumStartY - newPosition.y;
  const float currentPosX = mContent.GetCurrentProperty<float>(Actor::Property::POSITION_X);
  const float currentPosY = mContent.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
  const float distX       = targetPosX - currentPosX;
  const float distY       = targetPosY - currentPosY;

  if(std::sqrt(distX * distX + distY * distY) < 0.5f)
  {
    return true; // Already at target; consume but do nothing.
  }

  SendScrollStarted();

  mScrollAnimation = Animation::New(WHEEL_ANIM_SEC);
  mScrollAnimation.AnimateTo(Property(mContent, Actor::Property::POSITION_X), targetPosX, AlphaFunction::EASE_OUT);
  mScrollAnimation.AnimateTo(Property(mContent, Actor::Property::POSITION_Y), targetPosY, AlphaFunction::EASE_OUT);
  mScrollAnimation.FinishedSignal().Connect(this, &ScrollViewImpl::OnScrollAnimationFinished);
  mScrollAnimation.Play();

  mScrollPosition = newPosition;
  return true;
}

bool ScrollViewImpl::OnInterceptTouch(Actor actor, TouchEvent touch)
{
  PointState::Type state = touch.GetState(0);

  if(state == PointState::DOWN)
  {
    // mIntercepting stays true only when the previous touch sequence was abandoned
    // (pointer left the window with no UP/INTERRUPTED reaching OnTouch).
    // A normal release always reaches OnTouch(UP) which sets mIntercepting=false,
    // so this block is a no-op for every properly-terminated sequence.
    if(mIntercepting)
    {
      if(mStartEdgeActive)
      {
        if(mStartEdgeEffect) mStartEdgeEffect.Finish();
        mStartEdgeActive  = false;
        mEdgeDisplacement = 0.0f;
      }
      if(mEndEdgeActive)
      {
        if(mEndEdgeEffect) mEndEdgeEffect.Finish();
        mEndEdgeActive    = false;
        mEdgeDisplacement = 0.0f;
      }
      if(mIsDragging)
      {
        CancelScrollAnimation();
        SendDragFinished();
      }
    }

    // Reset all state on every new touch sequence.
    mIntercepting    = false;
    mJustIntercepted = false;
    mPanRecognized   = false;
    // Disambiguation begins immediately on touch: we don't yet know if this
    // will be a tap or a scroll.
    mDisambiguating = true;
    Ui::Internal::ScrollStateObserver::Get().NotifyGestureDisambiguationBegan();
  }

  // Always feed to the gesture detector so it can build velocity/displacement
  // history from the very beginning of the touch sequence.
  // If Pan is recognized, OnPanGesture(STARTED) fires synchronously here,
  // setting mPanRecognized = true.  Once the post-recognition threshold is
  // exceeded in a subsequent CONTINUING event, mIntercepting is set to true
  // and the touch sequence is stolen from children.
  TouchEvent& nonConstTouch = touch;
  mPanGestureDetector.HandleEvent(actor, nonConstTouch);

  if((state == PointState::UP || state == PointState::INTERRUPTED || state == PointState::LEAVE) && mDisambiguating)
  {
    // Touch ended (or left window) without the gesture detector resolving
    // disambiguation (pure tap / pointer exit before threshold).
    mDisambiguating = false;
    Ui::Internal::ScrollStateObserver::Get().NotifyGestureDisambiguationEnded();
  }

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
    // Pan CANCELLED fires synchronously inside HandleEvent above, so OnDragFinished
    // has already cleared mIsDragging/mStartEdgeActive.  This block is therefore a
    // no-op for the common case and acts as a safety net for any path where
    // OnDragFinished was skipped (e.g. gesture not yet recognised at lift-off).
    if(mIsDragging)
    {
      if(mStartEdgeActive)
      {
        if(mStartEdgeEffect) mStartEdgeEffect.OnRelease();
        mStartEdgeActive  = false;
        mEdgeDisplacement = 0.0f;
      }
      if(mEndEdgeActive)
      {
        if(mEndEdgeEffect) mEndEdgeEffect.OnRelease();
        mEndEdgeActive    = false;
        mEdgeDisplacement = 0.0f;
      }
      CancelScrollAnimation();
      SendDragFinished();
    }
    mIntercepting = false;
  }
  else if(state == PointState::LEAVE)
  {
    // Touch left the ScrollView bounds. The pan gesture may fire CANCELLED
    // synchronously inside HandleEvent() above, in which case OnDragFinished
    // has already handled edge-effect cleanup and any fling.  If the gesture
    // detector does not cancel on LEAVE (gesture still live), we treat this
    // the same as an interrupted touch to avoid a dangling drag state.
    if(mIsDragging)
    {
      // Use Finish() (immediate reset, no spring-back animation) so that a
      // subsequent gesture CANCELLED / finger-up fling can animate content
      // without conflicting with a bounce animation.
      if(mStartEdgeActive)
      {
        if(mStartEdgeEffect) mStartEdgeEffect.Finish();
        mStartEdgeActive  = false;
        mEdgeDisplacement = 0.0f;
      }
      if(mEndEdgeActive)
      {
        if(mEndEdgeEffect) mEndEdgeEffect.Finish();
        mEndEdgeActive    = false;
        mEdgeDisplacement = 0.0f;
      }
      CancelScrollAnimation();
      SendDragFinished();
    }
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
      // Seed the velocity tracker with the recognition point.
      mVelocityTracker.Clear();
      mVelocityTracker.Add(gesture.GetTime(), gesture.GetScreenPosition());
      // Disambiguation was already signalled in OnInterceptTouch(DOWN).
      break;
    }
    case GestureState::CONTINUING:
    {
      // Accumulate every touch point for velocity estimation.
      mVelocityTracker.Add(gesture.GetTime(), gesture.GetScreenPosition());
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
        // Disambiguation resolved: this is a scroll, not a tap.
        mDisambiguating = false;
        Ui::Internal::ScrollStateObserver::Get().NotifyGestureDisambiguationEnded();
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
      // Capture the lift-off point before processing so the tracker includes
      // the final position when OnDragFinished queries it.
      mVelocityTracker.Add(gesture.GetTime(), gesture.GetScreenPosition());
      if(mDisambiguating && !mIntercepting)
      {
        // Pan was recognized but threshold was never met: resolved as a tap.
        mDisambiguating = false;
        Ui::Internal::ScrollStateObserver::Get().NotifyGestureDisambiguationEnded();
      }
      // Use mIsDragging as a fallback: mIntercepting may have been cleared by
      // OnTouch(UP) before gesture FINISHED fires asynchronously.
      if(mIntercepting || mIsDragging)
      {
        // Cancel any in-flight snap animation before processing the drag end.
        // Skip entirely when no animation is running: CancelScrollAnimation would
        // call SendScrollFinished with a mid-drag scroll position, corrupting
        // mCurrentPage before OnDragFinished processes the gesture velocity.
        if(mScrollAnimation)
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
  //
  // Use mCurrentPosition (scroll-managed) rather than reading the live actor
  // position.  When an edge effect is active it physically displaces mContent,
  // so GetProperty would return boundary + bounceOffset.  Feeding that offset
  // back into AdjustDelta produces a large negative delta and a runaway
  // over-scroll feedback loop ("bouncing forever").
  Vector2 curPos   = mCurrentPosition;
  Vector2 adjusted = AdjustMovement(gesture.GetDisplacement());      // direction-filtered, NOT clamped
  Vector2 delta    = AdjustDelta(gesture.GetDisplacement(), curPos); // clamped to scroll bounds

  // If the user reverses direction while an edge effect is still active (scroll
  // resumes into the valid range), dismiss the effect immediately so its
  // internal offset does not compound on the next pull.
  if(delta.LengthSquared() > 0.0001f && (mStartEdgeActive || mEndEdgeActive))
  {
    if(mStartEdgeActive)
    {
      if(mStartEdgeEffect) mStartEdgeEffect.Finish();
      mStartEdgeActive  = false;
      mEdgeDisplacement = 0.0f;
    }
    if(mEndEdgeActive)
    {
      if(mEndEdgeEffect) mEndEdgeEffect.Finish();
      mEndEdgeActive    = false;
      mEdgeDisplacement = 0.0f;
    }
  }
  // Compute the over-scroll: the finger displacement that exceeds the scroll boundary.
  // overScroll.y > 0 → dragging content down at top boundary (start edge).
  // overScroll.y < 0 → dragging content up at bottom boundary (end edge).
  Vector2 overScroll = adjusted - delta;

  if(delta.LengthSquared() < 0.0001f && overScroll.LengthSquared() < 0.0001f)
  {
    return;
  }

  // Update content position BEFORE calling OnPull so that CaptureBasePosition()
  // inside BounceEdgeEffectImpl reads the boundary position, not the pre-delta position.
  // Without this ordering, mBasePosition is captured one frame early (e.g. y=-800 instead
  // of y=-900), causing ApplyOffset to never push the content past the visible boundary.
  if(delta.LengthSquared() > 0.0001f)
  {
    float newX = curPos.x + delta.x;
    float newY = curPos.y + delta.y;
    // Scroll offset drives the rendered position directly. Setting the
    // Actor property bypasses layout so this does not feed back into the
    // requested layout position.
    Dali::Ui::Extension::SetPositionX(mContent, newX);
    Dali::Ui::Extension::SetPositionY(mContent, newY);

    // Update scroll position from the actual content position
    mCurrentPosition = Vector2(newX, newY);
    mScrollPosition  = ContentPositionToScrollPosition(mCurrentPosition);
  }

  // Drive edge effects AFTER content position is updated so that
  // CaptureBasePosition() inside BounceEdgeEffectImpl reads the boundary
  // position (just set above), not the previous frame's position.
  if((mStartEdgeEffect || mEndEdgeEffect) && overScroll.LengthSquared() > 0.0f)
  {
    float over = (mScrollDirection == ScrollDirection::Horizontal) ? overScroll.x : overScroll.y;
    if(over > 0.0f && mStartEdgeEffect)
    {
      mEdgeDisplacement += over;
      mStartEdgeEffect.OnPull(over, mEdgeDisplacement);
      mStartEdgeActive = true;
    }
    else if(over < 0.0f && mEndEdgeEffect)
    {
      mEdgeDisplacement += -over;
      mEndEdgeEffect.OnPull(over, -mEdgeDisplacement);
      mEndEdgeActive = true;
    }
  }

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

  // Do NOT read mContent.GetProperty here: if an edge effect displaced mContent
  // for the bounce visual, the live position includes that offset and would
  // contaminate mCurrentPosition / mScrollPosition.  Both were kept accurate
  // throughout the drag by OnDragging (updated on every non-zero delta).

  // Use the history-weighted velocity instead of the raw last-frame velocity.
  // mFlingSensitivity lets callers tune the effective speed.
  Vector2 velocity = mVelocityTracker.Compute() * mFlingSensitivity;

  // Physics model: exponential velocity decay  v(t) = v0 * exp(-k*t)
  //   total distance = v0 / k_dist
  //   animation time = ln(v0 / v_stop) / k_dur
  //
  // Two separate k values let distance and duration be tuned independently:
  //   k_dist=0.002, k_dur=0.003  →  distance ~1.5× longer, duration unchanged
  //   0.5 px/ms → ~250 px / 0.77 s
  //   1.0 px/ms → ~500 px / 0.92 s
  //   2.0 px/ms → ~1000 px / 1.15 s
  constexpr float FLING_K_DIST = 0.002f; // distance deceleration (per ms); smaller = farther
  constexpr float FLING_K_DUR  = 0.003f; // duration deceleration (per ms); controls timing feel
  constexpr float FLING_VMIN   = 0.08f;  // below this speed, skip fling (px/ms)
  constexpr float FLING_VSTOP  = 0.04f;  // velocity at which animation is considered done (px/ms)

  float speed = velocity.Length();

  // Compute fling movement vector (zero if speed is below threshold).
  // Edge-effect release logic below needs movement/delta regardless of whether
  // a fling animation will be started, so compute them before the early-return.
  Vector2 movement = Vector2::ZERO;
  Vector2 delta    = Vector2::ZERO;
  if(speed >= FLING_VMIN)
  {
    float distance = speed / FLING_K_DIST;
    distance       = std::min(distance, mMaxFlingDistance);
    movement       = (velocity / speed) * distance;
    delta          = AdjustDelta(movement, mCurrentPosition);
  }

  // Release any active edge effects (or trigger absorb for a fling into the boundary).
  // Must happen before any early return so that a slow lift always springs back.
  if(mStartEdgeActive || mEndEdgeActive)
  {
    if(mStartEdgeActive)
    {
      // Fling delta is also clamped: if movement != delta, the fling hits the start boundary
      float flingAxis         = (mScrollDirection == ScrollDirection::Horizontal) ? movement.x : movement.y;
      float deltaAxis         = (mScrollDirection == ScrollDirection::Horizontal) ? delta.x : delta.y;
      bool  flingHitsBoundary = std::abs(flingAxis) > 0.001f && std::abs(deltaAxis) < std::abs(flingAxis) * 0.5f;
      if(flingHitsBoundary && mStartEdgeEffect)
      {
        float absVel = (mScrollDirection == ScrollDirection::Horizontal)
                         ? std::abs(velocity.x)
                         : std::abs(velocity.y);
        mStartEdgeEffect.OnAbsorb(absVel);
      }
      else if(mStartEdgeEffect)
      {
        // When a fling is about to start, finish immediately so the fling animation
        // has sole ownership of content position (no spring-back conflict).
        if(speed >= FLING_VMIN)
          mStartEdgeEffect.Finish();
        else
          mStartEdgeEffect.OnRelease();
      }
      mStartEdgeActive  = false;
      mEdgeDisplacement = 0.0f;
    }
    else if(mEndEdgeActive)
    {
      float flingAxis         = (mScrollDirection == ScrollDirection::Horizontal) ? movement.x : movement.y;
      float deltaAxis         = (mScrollDirection == ScrollDirection::Horizontal) ? delta.x : delta.y;
      bool  flingHitsBoundary = std::abs(flingAxis) > 0.001f && std::abs(deltaAxis) < std::abs(flingAxis) * 0.5f;
      if(flingHitsBoundary && mEndEdgeEffect)
      {
        float absVel = (mScrollDirection == ScrollDirection::Horizontal)
                         ? std::abs(velocity.x)
                         : std::abs(velocity.y);
        mEndEdgeEffect.OnAbsorb(-absVel); // negative: moving toward end boundary
      }
      else if(mEndEdgeEffect)
      {
        if(speed >= FLING_VMIN)
          mEndEdgeEffect.Finish();
        else
          mEndEdgeEffect.OnRelease();
      }
      mEndEdgeActive    = false;
      mEdgeDisplacement = 0.0f;
    }
  }

  if(speed < FLING_VMIN)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::OnDragFinished] speed %.3f < %.3f, no fling\n", speed, FLING_VMIN);
    Vector2 snapPos = mScrollPosition;
    float   snapDur = 0.0f;
    OnBeforeScrollAnimation(snapPos, snapDur);
    if(snapDur > 0.0f && (snapPos - mScrollPosition).LengthSquared() > 0.01f)
      ScrollToWithDuration(snapPos, snapDur);
    else
      SendScrollFinished();
    return;
  }

  float effectiveVStop = std::min(FLING_VSTOP, speed * 0.5f);
  float durationMs     = std::log(speed / effectiveVStop) / FLING_K_DUR;
  durationMs           = std::max(static_cast<float>(mMinimumFlingDuration),
                                  std::min(static_cast<float>(mMaximumFlingDuration), durationMs));

#ifdef DEBUG_SCROLL
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::OnDragFinished] trackedVel=[%.3f, %.3f] speed=%.3f dist=%.1f durMs=%.0f movement=[%.1f, %.1f] delta=[%.1f, %.1f]\n",
                velocity.x, velocity.y, speed, movement.Length(), durationMs,
                movement.x, movement.y, delta.x, delta.y);
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

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[ScrollView::OnDragFinished] scrollDelta=[%.1f, %.1f] target=[%.1f, %.1f] dur=%.3fs\n",
                scrollDelta.x, scrollDelta.y, newPos.x, newPos.y, durationMs / 1000.0f);

  float   targetDur = durationMs / 1000.0f;
  Vector2 targetPos = newPos;
  OnBeforeScrollAnimation(targetPos, targetDur);
  ScrollToWithDuration(targetPos, targetDur);
}

void ScrollViewImpl::OnBeforeScrollAnimation(Vector2& /*targetPosition*/, float& /*durationSec*/)
{
  // Default: no-op — subclasses override to redirect to snap points.
}

void ScrollViewImpl::OnScrollableAreaChanged()
{
  // Default: no-op — subclasses override to react to layout-driven dimension changes.
}

void ScrollViewImpl::SendScrollStarted()
{
  if(!mIsScrolling)
  {
    mIsScrolling = true;
    Ui::Internal::ScrollStateObserver::Get().NotifyScrollStarted();
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
    mIsScrolling = false;
    Ui::Internal::ScrollStateObserver::Get().NotifyScrollFinished();
    Ui::ScrollView scrollView = Ui::ScrollView::DownCast(Self());
    mScrollFinishedSignal.Emit(scrollView);
    // Auto-visibility bars fade out 2 seconds after the last scroll position
    // update, handled by the hide timer inside ScrollBarImpl::ShowBar().
  }
}

void ScrollViewImpl::SendDragStarted()
{
  mIsDragging = true;
  Ui::Internal::ScrollStateObserver::Get().NotifyDragStarted();
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
  mIsDragging = false;
  Ui::Internal::ScrollStateObserver::Get().NotifyDragFinished();
  Ui::ScrollView scrollView = Ui::ScrollView::DownCast(Self());
  mDragFinishedSignal.Emit(scrollView);
}

void ScrollViewImpl::UpdateScrollingProperties()
{
  if(!mContent)
  {
    return;
  }

  Insets padding = GetPadding();

  mMaximumStartX = padding.start;
  mMaximumStartY = padding.top;

  mViewportWidth  = Self().GetProperty<float>(Actor::Property::SIZE_WIDTH);
  mViewportHeight = Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT);

  // mScrollableWidth/mScrollableHeight are set by Arrange via SetScrollableWidth/SetScrollableHeight
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

  // Do not call SetProperty while an animation is running — it would override
  // the BAKE'd mid-animation position and cause a visible snap backward.
  if(!mScrollAnimation)
  {
    ApplyScrollPosition(mScrollPosition);
  }

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

    Dali::Ui::Extension::SetPositionX(mContent, posX);
    Dali::Ui::Extension::SetPositionY(mContent, posY);

    // Update scroll bar position
    mScrollBar.UpdateScrollPosition(position);
  }
}

void ScrollViewImpl::CancelScrollAnimation()
{
  if(mScrollAnimation)
  {
    if(mContent)
    {
      mCurrentPosition = Vector2(mContent.GetCurrentProperty<float>(Actor::Property::POSITION_X),
                                 mContent.GetCurrentProperty<float>(Actor::Property::POSITION_Y));
      mScrollPosition  = ContentPositionToScrollPosition(mCurrentPosition);
    }
    mScrollAnimation.Stop();
    mScrollAnimation.Reset();
  }
  if(mIsScrolling)
  {
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

  // Only track position during a DALi scroll animation. When no scroll animation
  // is running the notification fires because a bounce edge effect is physically
  // displacing mContent, and reading GetCurrentProperty would contaminate
  // mCurrentPosition / mScrollPosition with the bounce offset.
  if(!mScrollAnimation)
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
