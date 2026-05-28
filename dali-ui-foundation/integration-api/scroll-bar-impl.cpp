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
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/scroll-bar-impl.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>

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
DALI_TYPE_REGISTRATION_BEGIN(ScrollBarImpl, AbsoluteLayoutImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

ScrollBarImplPtr ScrollBarImpl::New()
{
  return ScrollBarImplPtr(new ScrollBarImpl());
}

ScrollBarImpl::ScrollBarImpl()
: AbsoluteLayoutImpl(),
  mVerticalBar(),
  mHorizontalBar(),
  mBarThickness(4.0f),
  mBarColor(0.8f, 0.8f, 0.8f, 1.0f),
  mBarMinSize(1.0f),
  mScrollPosition(0.0f, 0.0f),
  mBarOffset(2.0f),
  mBarCornerRadius(1.0f, 1.0f, 1.0f, 1.0f),
  mVerticalVisibility(ScrollBarVisibility::Auto),
  mHorizontalVisibility(ScrollBarVisibility::Auto),
  mScrollableWidth(0.0f),
  mScrollableHeight(0.0f),
  mViewPortWidth(0.0f),
  mViewPortHeight(0.0f),
  mLastVBarBounds(0.0f, 0.0f, 0.0f, 0.0f),
  mLastHBarBounds(0.0f, 0.0f, 0.0f, 0.0f),
  mFadeInAnimation(),
  mFadeOutAnimation(),
  mIsBarVisible(false)
{
}

ScrollBarImpl::~ScrollBarImpl()
{
}

void ScrollBarImpl::OnInitialize()
{
  AbsoluteLayoutImpl::OnInitialize();

  // Use MATCH_PARENT so ArrangeStandaloneChildren sizes this to fill the
  // parent viewport.
  SetRequestedWidth(MATCH_PARENT);
  SetRequestedHeight(MATCH_PARENT);

  // Mark as STANDALONE so that InvalidateMeasure triggered by thumb position
  // updates does not propagate up to the ScrollView parent and interfere with
  // scroll animations on the content actor.
  SetLayoutMode(LayoutMode::STANDALONE);
}

void ScrollBarImpl::Initialize()
{
  // Call parent Initialize() which triggers OnInitialize()
  AbsoluteLayoutImpl::Initialize();

  // Do not override mBarMinSize set in constructor
}

View ScrollBarImpl::GetTarget() const
{
  return View::DownCast(Self());
}

ScrollBarVisibility ScrollBarImpl::GetVerticalScrollBarVisibility() const
{
  return mVerticalVisibility;
}

void ScrollBarImpl::SetVerticalScrollBarVisibility(ScrollBarVisibility visibility)
{
  ScrollBarVisibility old = mVerticalVisibility;
  mVerticalVisibility     = visibility;

  if(visibility == ScrollBarVisibility::Never)
  {
    RemoveVBar();
  }
  else
  {
    if(old == ScrollBarVisibility::Never)
    {
      CreateVBar();
      UpdateBarSize();
    }
    // Auto: bar appears only during scroll (via UpdatePosition → ShowBar).
    // Always: show immediately since the bar must always be visible.
    if(visibility == ScrollBarVisibility::Always)
    {
      ShowBar();
    }
  }
}

ScrollBarVisibility ScrollBarImpl::GetHorizontalScrollBarVisibility() const
{
  return mHorizontalVisibility;
}

void ScrollBarImpl::SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility)
{
  ScrollBarVisibility old = mHorizontalVisibility;
  mHorizontalVisibility   = visibility;

  if(visibility == ScrollBarVisibility::Never)
  {
    RemoveHBar();
  }
  else
  {
    if(old == ScrollBarVisibility::Never)
    {
      CreateHBar();
      UpdateBarSize();
    }
    // Auto: bar appears only during scroll (via UpdatePosition → ShowBar).
    // Always: show immediately since the bar must always be visible.
    if(visibility == ScrollBarVisibility::Always)
    {
      ShowBar();
    }
  }
}

void ScrollBarImpl::UpdateBarSize(float scrollWidth, float scrollHeight, float viewportWidth, float viewportHeight)
{
  mScrollableWidth  = scrollWidth;
  mScrollableHeight = scrollHeight;
  mViewPortWidth    = viewportWidth;
  mViewPortHeight   = viewportHeight;
  UpdateBarSize();
}

void ScrollBarImpl::UpdateScrollPosition(const Vector2& position)
{
  SetScrollPosition(position);
}

void ScrollBarImpl::HideBar()
{
  PlayFadeOut();
}

float ScrollBarImpl::GetBarThickness() const
{
  return mBarThickness;
}

void ScrollBarImpl::SetBarThickness(float thickness)
{
  mBarThickness = thickness;
}

Vector4 ScrollBarImpl::GetBarColor() const
{
  return mBarColor;
}

void ScrollBarImpl::SetBarColor(const Vector4& color)
{
  mBarColor = color;
  if(mVerticalBar)
  {
    mVerticalBar.SetBackgroundColor(color);
  }
  if(mHorizontalBar)
  {
    mHorizontalBar.SetBackgroundColor(color);
  }
}

float ScrollBarImpl::GetBarMinSize() const
{
  if(mBarMinSize > 0.0f)
  {
    return mBarMinSize;
  }
  float size = std::max(Self().GetProperty<float>(Actor::Property::SIZE_WIDTH),
                        Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT));
  return std::max(1.0f, size / 20.0f);
}

void ScrollBarImpl::SetBarMinSize(float minSize)
{
  mBarMinSize = minSize;
}

Vector2 ScrollBarImpl::GetScrollPosition() const
{
  return mScrollPosition;
}

void ScrollBarImpl::SetScrollPosition(const Vector2& position)
{
  mScrollPosition = position;
  UpdatePosition(position);
}

float ScrollBarImpl::GetBarOffset() const
{
  return mBarOffset;
}

void ScrollBarImpl::SetBarOffset(float offset)
{
  mBarOffset = offset;
  // Update bars with new offset
  UpdateBarSize();
  UpdatePosition(mScrollPosition);
}

Vector4 ScrollBarImpl::GetBarCornerRadius() const
{
  return mBarCornerRadius;
}

void ScrollBarImpl::SetBarCornerRadius(float radius)
{
  mBarCornerRadius = Vector4(radius, radius, radius, radius);
  ApplyCornerRadiusToBars();
}

void ScrollBarImpl::SetBarCornerRadius(const Vector4& radius)
{
  mBarCornerRadius = radius;
  ApplyCornerRadiusToBars();
}

void ScrollBarImpl::ApplyCornerRadiusToBars()
{
  // Apply corner radius using View's SetCornerRadius method
  if(mVerticalBar)
  {
    // For vertical bar, we need to adjust the corner order since it's rotated
    // View expects: topLeft, topRight, bottomRight, bottomLeft
    mVerticalBar.SetCornerRadius(mBarCornerRadius.x, mBarCornerRadius.y, mBarCornerRadius.w, mBarCornerRadius.z);
  }
  if(mHorizontalBar)
  {
    mHorizontalBar.SetCornerRadius(mBarCornerRadius.x, mBarCornerRadius.y, mBarCornerRadius.w, mBarCornerRadius.z);
  }
}

void ScrollBarImpl::UpdateBarSize()
{
  // Update vertical bar
  if(mScrollableHeight > 0.0f && mViewPortHeight > 0.0f)
  {
    float hRatio = std::min(1.0f, mViewPortHeight / mScrollableHeight);

    if(hRatio >= 1.0f)
    {
      RemoveVBar();
    }
    else
    {
      CreateVBar();
      UpdateVBarSize(hRatio);
    }
  }

  // Update horizontal bar
  if(mScrollableWidth > 0.0f && mViewPortWidth > 0.0f)
  {
    float wRatio = std::min(1.0f, mViewPortWidth / mScrollableWidth);

    if(wRatio >= 1.0f)
    {
      RemoveHBar();
    }
    else
    {
      CreateHBar();
      UpdateHBarSize(wRatio);
    }
  }
}

void ScrollBarImpl::ShowBar()
{
  PlayFadeIn();

  // Restart the auto-hide timer whenever any Auto-visibility bar is shown.
  // The timer fires 2 seconds after the last ShowBar call (i.e. after the
  // last scroll position update), then fades out the bar.
  const bool hasAutoBar = (mVerticalVisibility == ScrollBarVisibility::Auto && mVerticalBar) ||
                          (mHorizontalVisibility == ScrollBarVisibility::Auto && mHorizontalBar);
  if(hasAutoBar)
  {
    if(mHideTimer)
    {
      mHideTimer.Stop();
    }
    mHideTimer = Timer::New(1000u);
    mHideTimer.TickSignal().Connect(this, &ScrollBarImpl::OnHideTimerFired);
    mHideTimer.Start();
  }
}

bool ScrollBarImpl::OnHideTimerFired()
{
  PlayFadeOut();
  return false; // one-shot: do not repeat
}

void ScrollBarImpl::PlayFadeIn()
{
  if(mFadeOutAnimation)
  {
    mFadeOutAnimation.Stop();
  }

  // Skip if bars are already fully visible or fade-in is already playing
  if(mIsBarVisible)
  {
    return;
  }

  if(mFadeInAnimation && mFadeInAnimation.GetState() == Animation::PLAYING)
  {
    return;
  }

  if(mVerticalBar || mHorizontalBar)
  {
    mFadeInAnimation = Animation::New(0.1f);

    if(mVerticalBar)
    {
      mFadeInAnimation.AnimateTo(Property(mVerticalBar, Actor::Property::OPACITY), 1.0f);
    }
    if(mHorizontalBar)
    {
      mFadeInAnimation.AnimateTo(Property(mHorizontalBar, Actor::Property::OPACITY), 1.0f);
    }

    mFadeInAnimation.FinishedSignal().Connect(this, &ScrollBarImpl::OnFadeInFinished);
    mFadeInAnimation.Play();
  }
}

void ScrollBarImpl::OnFadeInFinished(Animation animation)
{
  mIsBarVisible = true;
}

void ScrollBarImpl::PlayFadeOut()
{
  mIsBarVisible = false;

  if(mHideTimer)
  {
    mHideTimer.Stop();
  }

  if(mFadeInAnimation)
  {
    mFadeInAnimation.Stop();
  }

  if((mVerticalVisibility == ScrollBarVisibility::Auto && mVerticalBar) ||
     (mHorizontalVisibility == ScrollBarVisibility::Auto && mHorizontalBar))
  {
    mFadeOutAnimation = Animation::New(0.255f);

    if(mVerticalVisibility == ScrollBarVisibility::Auto && mVerticalBar)
    {
      mFadeOutAnimation.AnimateTo(Property(mVerticalBar, Actor::Property::OPACITY), 0.0f);
    }
    if(mHorizontalVisibility == ScrollBarVisibility::Auto && mHorizontalBar)
    {
      mFadeOutAnimation.AnimateTo(Property(mHorizontalBar, Actor::Property::OPACITY), 0.0f);
    }

    mFadeOutAnimation.Play();
  }
}

void ScrollBarImpl::UpdatePosition(const Vector2& position)
{
  // Get padding from View (inherited)
  Extents padding = GetPadding();

  // mViewPortWidth/Height are VISUAL units (set from the ScrollView's actor size).
  // AbsoluteLayoutParams expects NATURAL units, so convert by dividing by the
  // effective scale.  At scale==1.0 this is a no-op.
  float scale             = GetEffectiveScale();
  float naturalSelfWidth  = ((mViewPortWidth > 0.0f) ? mViewPortWidth : Self().GetProperty<float>(Actor::Property::SIZE_WIDTH)) / scale;
  float naturalSelfHeight = ((mViewPortHeight > 0.0f) ? mViewPortHeight : Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT)) / scale;

  if(mVerticalBar)
  {
    float maxScrollY = mScrollableHeight - mViewPortHeight;
    float hRatio     = (maxScrollY > 0.0f) ? std::min(1.0f, position.y / maxScrollY) : 0.0f;

    // Content area height (padding excluded) — all in natural units
    float contentHeight = naturalSelfHeight - padding.top - padding.bottom;

    // Use mLastVBarBounds.height instead of Actor::Property::SIZE_HEIGHT because
    // the layout may not have been applied yet, so SIZE_HEIGHT would return 0.
    float barHeight = mLastVBarBounds.height;
    // Bar moves within content area (no offset on y-axis for VBar)
    float maxH = contentHeight - barHeight;
    float y    = maxH * hRatio;

    // x position: offset from right edge of content area — all in natural units
    float contentWidth = naturalSelfWidth - padding.start - padding.end;
    float xPos         = contentWidth - mBarThickness - mBarOffset;

    SetVBarBounds(xPos, y, mBarThickness, barHeight);
  }

  if(mHorizontalBar)
  {
    float maxScrollX = mScrollableWidth - mViewPortWidth;
    float wRatio     = (maxScrollX > 0.0f) ? std::min(1.0f, position.x / maxScrollX) : 0.0f;

    // Content area width (padding excluded) — all in natural units
    float contentWidth = naturalSelfWidth - padding.start - padding.end;

    // Use mLastHBarBounds.width instead of Actor::Property::SIZE_WIDTH because
    // the layout may not have been applied yet, so SIZE_WIDTH would return 0.
    float barWidth = mLastHBarBounds.width;
    // Bar moves within content area (no offset on x-axis for HBar)
    float maxW = contentWidth - barWidth;
    float x    = maxW * wRatio;

    // y position: offset from bottom edge of content area — all in natural units
    float contentHeight = naturalSelfHeight - padding.top - padding.bottom;
    float yPos          = contentHeight - mBarThickness - mBarOffset;

    SetHBarBounds(x, yPos, barWidth, mBarThickness);
  }

  ShowBar();
}

void ScrollBarImpl::SetVBarBounds(float x, float y, float width, float height)
{
  if(!mVerticalBar)
    return;

  LayoutRect newBounds(x, y, width, height);

  // Only update if bounds have actually changed to avoid infinite relayout loop
  if(std::abs(mLastVBarBounds.x - newBounds.x) > 0.01f ||
     std::abs(mLastVBarBounds.y - newBounds.y) > 0.01f ||
     std::abs(mLastVBarBounds.width - newBounds.width) > 0.01f ||
     std::abs(mLastVBarBounds.height - newBounds.height) > 0.01f)
  {
    mLastVBarBounds = newBounds;
    mVerticalBar.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(newBounds));
  }
}

void ScrollBarImpl::SetHBarBounds(float x, float y, float width, float height)
{
  if(!mHorizontalBar)
    return;

  LayoutRect newBounds(x, y, width, height);

  // Only update if bounds have actually changed to avoid infinite relayout loop
  if(std::abs(mLastHBarBounds.x - newBounds.x) > 0.01f ||
     std::abs(mLastHBarBounds.y - newBounds.y) > 0.01f ||
     std::abs(mLastHBarBounds.width - newBounds.width) > 0.01f ||
     std::abs(mLastHBarBounds.height - newBounds.height) > 0.01f)
  {
    mLastHBarBounds = newBounds;
    mHorizontalBar.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(newBounds));
  }
}

void ScrollBarImpl::RemoveVBar()
{
  if(mVerticalBar)
  {
    Self().Remove(mVerticalBar);
    mVerticalBar.Reset();
    mLastVBarBounds = LayoutRect(0.0f, 0.0f, 0.0f, 0.0f);
  }
}

void ScrollBarImpl::CreateVBar()
{
  if(mVerticalVisibility == ScrollBarVisibility::Never)
    return;

  if(mVerticalBar)
    return;

  // New bar starts at opacity 0; ensure PlayFadeIn runs on the next ShowBar call.
  mIsBarVisible = false;

  mVerticalBar = View::New();
  mVerticalBar.SetBackgroundColor(mBarColor);
  mVerticalBar.SetOpacity(0.0f);

  // Apply corner radius
  mVerticalBar.SetCornerRadius(mBarCornerRadius.x, mBarCornerRadius.y, mBarCornerRadius.w, mBarCornerRadius.z);

  // Place at right edge with offset using AbsoluteLayoutParams (natural units).
  Extents padding          = GetPadding();
  float   scale            = GetEffectiveScale();
  float   naturalSelfWidth = ((mViewPortWidth > 0.0f) ? mViewPortWidth : Self().GetProperty<float>(Actor::Property::SIZE_WIDTH)) / scale;
  float   contentWidth     = naturalSelfWidth - padding.start - padding.end;
  float   xPos             = contentWidth - mBarThickness - mBarOffset;
  mVerticalBar.SetLayoutParams(AbsoluteLayoutParams::New()
                                 .SetBounds(LayoutRect(xPos, 0.0f, mBarThickness, 0.0f)));
  mLastVBarBounds = LayoutRect(xPos, 0.0f, mBarThickness, 0.0f);

  Self().Add(mVerticalBar);
}

void ScrollBarImpl::UpdateVBarSize(float ratio)
{
  if(!mVerticalBar)
    return;

  Extents padding           = GetPadding();
  float   scale             = GetEffectiveScale();
  float   naturalSelfHeight = ((mViewPortHeight > 0.0f) ? mViewPortHeight : Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT)) / scale;
  float   contentHeight     = naturalSelfHeight - padding.top - padding.bottom;
  float   sizeHeight        = std::max(GetBarMinSize(), std::round(contentHeight * ratio));

  float naturalSelfWidth = ((mViewPortWidth > 0.0f) ? mViewPortWidth : Self().GetProperty<float>(Actor::Property::SIZE_WIDTH)) / scale;
  float contentWidth     = naturalSelfWidth - padding.start - padding.end;
  float xPos             = contentWidth - mBarThickness - mBarOffset;

  SetVBarBounds(xPos, 0.0f, mBarThickness, sizeHeight);
}

void ScrollBarImpl::RemoveHBar()
{
  if(mHorizontalBar)
  {
    Self().Remove(mHorizontalBar);
    mHorizontalBar.Reset();
    mLastHBarBounds = LayoutRect(0.0f, 0.0f, 0.0f, 0.0f);
  }
}

void ScrollBarImpl::CreateHBar()
{
  if(mHorizontalVisibility == ScrollBarVisibility::Never)
    return;

  if(mHorizontalBar)
    return;

  // New bar starts at opacity 0; ensure PlayFadeIn runs on the next ShowBar call.
  mIsBarVisible = false;

  mHorizontalBar = View::New();
  mHorizontalBar.SetBackgroundColor(mBarColor);
  mHorizontalBar.SetOpacity(0.0f);

  // Apply corner radius
  mHorizontalBar.SetCornerRadius(mBarCornerRadius.x, mBarCornerRadius.y, mBarCornerRadius.w, mBarCornerRadius.z);

  // Place at bottom edge with offset using AbsoluteLayoutParams (natural units).
  Extents padding           = GetPadding();
  float   scale             = GetEffectiveScale();
  float   naturalSelfHeight = ((mViewPortHeight > 0.0f) ? mViewPortHeight : Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT)) / scale;
  float   contentHeight     = naturalSelfHeight - padding.top - padding.bottom;
  float   yPos              = contentHeight - mBarThickness - mBarOffset;
  mHorizontalBar.SetLayoutParams(AbsoluteLayoutParams::New()
                                   .SetBounds(LayoutRect(0.0f, yPos, 0.0f, mBarThickness)));
  mLastHBarBounds = LayoutRect(0.0f, yPos, 0.0f, mBarThickness);

  Self().Add(mHorizontalBar);
}

void ScrollBarImpl::UpdateHBarSize(float ratio)
{
  if(!mHorizontalBar)
    return;

  Extents padding          = GetPadding();
  float   scale            = GetEffectiveScale();
  float   naturalSelfWidth = ((mViewPortWidth > 0.0f) ? mViewPortWidth : Self().GetProperty<float>(Actor::Property::SIZE_WIDTH)) / scale;
  float   contentWidth     = naturalSelfWidth - padding.start - padding.end;
  float   sizeWidth        = std::max(GetBarMinSize(), std::round(contentWidth * ratio));

  float naturalSelfHeight = ((mViewPortHeight > 0.0f) ? mViewPortHeight : Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT)) / scale;
  float contentHeight     = naturalSelfHeight - padding.top - padding.bottom;
  float yPos              = contentHeight - mBarThickness - mBarOffset;

  SetHBarBounds(0.0f, yPos, sizeWidth, mBarThickness);
}

} // namespace Integration

} // namespace Ui

} // namespace Dali