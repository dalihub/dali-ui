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
 */

// CLASS HEADER
#include <dali-ui-components/internal/toast-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <algorithm>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{

BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(ToastImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

constexpr float POST_INITIAL_SECONDS = 0.15f;
constexpr float POST_SETTLE_SECONDS  = 0.40f;
constexpr float CONTENT_FADE_SECONDS = 0.15f;
constexpr float EXPAND_SECONDS       = 0.40f;
constexpr float EXPAND_FADE_DELAY    = 0.21f;
constexpr float DISMISS_SECONDS      = 0.15f;
constexpr float EPSILON              = 0.001f;

const Vector2 POST_INITIAL_CP1(0.33f, 0.0f);
const Vector2 POST_INITIAL_CP2(0.20f, 1.0f);
const Vector2 POST_SETTLE_CP1(0.21f, 1.50f);
const Vector2 POST_SETTLE_CP2(0.54f, 1.0f);
const Vector2 EXPAND_CP1(0.22f, 0.25f);
const Vector2 EXPAND_CP2(0.0f, 1.0f);

float Clamp(float value, float minimum, float maximum)
{
  return std::max(minimum, std::min(maximum, value));
}

} // namespace

Ui::Toast ToastImpl::New(ToastStyle style)
{
  DALI_ASSERT_ALWAYS(style && "ToastStyle must be initialized");

  IntrusivePtr<ToastImpl> impl(new ToastImpl());
  Ui::Toast               handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}

ToastImpl::ToastImpl() = default;

ToastImpl::~ToastImpl()
{
  StopTimeout();
  StopTransition(false);
  if(mWindow)
  {
    mWindow.ResizedSignal().Disconnect(this, &ToastImpl::OnWindowResized);
  }
}

void ToastImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  Ui::View self = Ui::View::DownCast(Self());
  self.SetLayoutMode(LayoutMode::STANDALONE);
  self.SetAccessibilityRole(Accessibility::Role::NOTIFICATION);
  self.SetAccessibilityHighlightable(false);
  self.SetAccessibilityHidden(true);

  mContent = Ui::StackLayout::New(StackOrientation::HORIZONTAL);
  mContent.SetRequestedWidth(MATCH_PARENT);
  mContent.SetRequestedHeight(MATCH_PARENT);
  mContent.SetAccessibilityHidden(false);

  mMessage = Ui::Label::New();
  mMessage.SetMultiLine(true);
  mMessage.SetLineWrapMode(Text::LineWrapMode::WORD);
  mMessage.SetVerticalTextAlignment(Text::Alignment::CENTER);
  mMessage.SetAccessibilityHidden(true);

  mContent.Add(mMessage);
  self.Add(mContent);
}

void ToastImpl::ApplyInitialStyle(ToastStyle style)
{
  mStyle                 = style;
  mTextColor             = style.GetTextColor();
  mFontSize              = style.GetFontSize();
  mFontFamily            = style.GetFontFamily();
  mItemSpacing           = style.GetItemSpacing();
  mActionButtonTextColor = style.GetActionButtonStyle().GetTextColor();

  Ui::View self = Ui::View::DownCast(Self());
  self.SetBackgroundColor(style.GetBackgroundColor());
  self.SetCornerRadius(style.GetCornerRadius());
  self.SetCornerRadiusPolicy(style.GetCornerRadiusPolicy());
  self.SetShadow(style.GetShadow());
  self.SetBorderlineWidth(style.GetBorderlineWidth());
  self.SetBorderlineColor(style.GetBorderlineColor());
  self.SetBorderlineOffset(style.GetBorderlineOffset());

  mActionButton = Ui::TextButton::New(style.GetActionButtonStyle());
  mActionButton.SetAccessibilityHidden(false);
  mActionButton.ConnectClickedSignal(this, &ToastImpl::OnActionButtonClicked);
  mContent.Add(mActionButton);

  ApplyContentStyle();
  ConfigureContent(false);
}

void ToastImpl::ApplyContentStyle()
{
  mMessage.SetText(mText);
  mMessage.SetTextColor(mTextColor);
  mMessage.SetFontSize(mFontSize);
  mMessage.SetFontFamily(mFontFamily);

  mActionButton.SetText(mActionButtonText);
  mActionButton.SetTextColor(mActionButtonTextColor);
  mActionButton.SetVisible(!mActionButtonText.Empty());
  mActionButton.SetAccessibilityHidden(mActionButtonText.Empty());
  mActionButton.SetLayoutMode(mActionButtonText.Empty() ? LayoutMode::STANDALONE : LayoutMode::DEFAULT);
}

void ToastImpl::ConfigureContent(bool expanded)
{
  mContent.SetOrientation(expanded ? StackOrientation::VERTICAL : StackOrientation::HORIZONTAL);
  mContent.SetPadding(expanded ? mStyle.GetExpandedPadding() : mStyle.GetPadding());
  mContent.SetSpacing(mItemSpacingOverridden ? mItemSpacing : (expanded ? mStyle.GetExpandedItemSpacing() : mStyle.GetItemSpacing()));
  mMessage.SetRequestedHeight(expanded ? WRAP_CONTENT : MATCH_PARENT);
  mMessage.SetLayoutParams(StackLayoutParams::New().SetWeight(expanded ? 0.0f : 1.0f).SetAlignment(LayoutAlignment::CENTER));
  mActionButton.SetLayoutParams(StackLayoutParams::New().SetAlignment(expanded ? LayoutAlignment::END : LayoutAlignment::CENTER));
}

void ToastImpl::SetText(const Dali::String& text)
{
  mText = text;
  if(mMessage)
  {
    mMessage.SetText(text);
  }
  if(mState != PresentationState::DETACHED)
  {
    RelayoutForCurrentState();
  }
}

Dali::String ToastImpl::GetText() const
{
  return mText;
}

void ToastImpl::SetDuration(uint32_t duration)
{
  mDuration = duration;
  if(mState != PresentationState::DETACHED && mState != PresentationState::DISMISSING)
  {
    StartTimeout();
  }
}

uint32_t ToastImpl::GetDuration() const
{
  return mDuration;
}

void ToastImpl::SetTextColor(const UiColor& color)
{
  mTextColor = color;
  mMessage.SetTextColor(color);
}

UiColor ToastImpl::GetTextColor() const
{
  return mTextColor;
}

void ToastImpl::SetFontSize(float fontSize)
{
  DALI_ASSERT_ALWAYS(std::isfinite(fontSize) && fontSize >= 0.0f && "Toast font size must be finite and non-negative");
  mFontSize = fontSize;
  mMessage.SetFontSize(fontSize);
  if(mState != PresentationState::DETACHED)
  {
    RelayoutForCurrentState();
  }
}

float ToastImpl::GetFontSize() const
{
  return mFontSize;
}

void ToastImpl::SetFontFamily(const Dali::String& fontFamily)
{
  mFontFamily = fontFamily;
  mMessage.SetFontFamily(fontFamily);
  if(mState != PresentationState::DETACHED)
  {
    RelayoutForCurrentState();
  }
}

Dali::String ToastImpl::GetFontFamily() const
{
  return mFontFamily;
}

void ToastImpl::SetActionButtonText(const Dali::String& text)
{
  mActionButtonText = text;
  mActionButton.SetText(text);
  mActionButton.SetVisible(!text.Empty());
  mActionButton.SetAccessibilityHidden(text.Empty());
  mActionButton.SetLayoutMode(text.Empty() ? LayoutMode::STANDALONE : LayoutMode::DEFAULT);
  if(mState != PresentationState::DETACHED)
  {
    RelayoutForCurrentState();
  }
}

Dali::String ToastImpl::GetActionButtonText() const
{
  return mActionButtonText;
}

void ToastImpl::SetActionButtonTextColor(const UiColor& color)
{
  mActionButtonTextColor = color;
  mActionButton.SetTextColor(color);
}

UiColor ToastImpl::GetActionButtonTextColor() const
{
  return mActionButtonTextColor;
}

void ToastImpl::SetItemSpacing(float spacing)
{
  DALI_ASSERT_ALWAYS(std::isfinite(spacing) && spacing >= 0.0f && "Toast item spacing must be finite and non-negative");
  mItemSpacing           = spacing;
  mItemSpacingOverridden = true;
  mContent.SetSpacing(spacing);
  if(mState != PresentationState::DETACHED)
  {
    RelayoutForCurrentState();
  }
}

float ToastImpl::GetItemSpacing() const
{
  return mItemSpacing;
}

void ToastImpl::Post(Dali::Window window)
{
  DALI_ASSERT_ALWAYS(window && "Toast::Post requires an initialized Window");

  ++mGeneration;
  StopTimeout();
  StopTransition(false);
  if(mState != PresentationState::DETACHED)
  {
    Detach(false);
  }

  mWindow       = window;
  mOverlayLayer = window.GetOverlayLayer();
  DALI_ASSERT_ALWAYS(mOverlayLayer && "Toast::Post requires a Window overlay layer");
  mWindow.ResizedSignal().Connect(this, &ToastImpl::OnWindowResized);

  ConfigureContent(false);
  ApplyContentStyle();
  mShownEmitted = false;
  mState        = PresentationState::POSTING;
  UpdatePresentationGeometry(false);

  Ui::View self = Ui::View::DownCast(Self());
  self.SetAccessibilityHidden(true);
  mOverlayLayer.Add(self);
  self.RaiseToTop();
  StartPostTransition();
  StartTimeout();
}

void ToastImpl::Dismiss()
{
  if(mState == PresentationState::DETACHED || mState == PresentationState::DISMISSING)
  {
    return;
  }
  StartDismissTransition();
}

void ToastImpl::UpdatePresentationGeometry(bool expanded)
{
  if(!mWindow)
  {
    return;
  }

  const PositionSize bounds          = mWindow.GetPositionSize();
  const float        windowWidth     = std::max(0.0f, static_cast<float>(bounds.width));
  const float        windowHeight    = std::max(0.0f, static_cast<float>(bounds.height));
  const float        ratioMaximum    = windowWidth * mStyle.GetMaximumWidthRatio();
  const float        absoluteMaximum = mStyle.GetMaximumWidth();
  const float        maximumWidth    = std::max(0.0f, absoluteMaximum > 0.0f ? std::min(ratioMaximum, absoluteMaximum) : ratioMaximum);
  const Insets       padding         = expanded ? mStyle.GetExpandedPadding() : mStyle.GetPadding();

  ConfigureContent(expanded);
  const MeasuredSize actionSize       = mActionButtonText.Empty() ? MeasuredSize(0.0f, 0.0f) : mActionButton.Measure(maximumWidth, mStyle.GetMaximumHeight());
  const Vector3      naturalText      = mMessage.GetNaturalSize();
  const float        effectiveSpacing = mItemSpacingOverridden ? mItemSpacing : (expanded ? mStyle.GetExpandedItemSpacing() : mStyle.GetItemSpacing());
  const float        spacing          = mActionButtonText.Empty() ? 0.0f : effectiveSpacing;
  float              contentWidth;
  float              contentHeight;

  if(expanded)
  {
    contentWidth                   = std::max(naturalText.x, actionSize.width);
    const float availableTextWidth = std::max(1.0f, maximumWidth - padding.start - padding.end);
    contentHeight                  = mMessage.GetHeightForWidth(availableTextWidth) + actionSize.height + spacing;
  }
  else
  {
    contentWidth  = naturalText.x + actionSize.width + spacing;
    contentHeight = std::max(naturalText.y, actionSize.height);
  }

  const float naturalWidth    = Clamp(contentWidth + padding.start + padding.end,
                                      std::min(mStyle.GetMinimumWidth(), maximumWidth),
                                      maximumWidth);
  const float width           = expanded ? maximumWidth : naturalWidth;
  const float naturalHeight   = contentHeight + padding.top + padding.bottom;
  const float height          = expanded
                                  ? std::min(windowHeight, Clamp(std::max(mStyle.GetExpandedHeight(), naturalHeight), mStyle.GetExpandedHeight(), mStyle.GetMaximumHeight()))
                                  : std::min(windowHeight, std::max(mStyle.GetMinimumHeight(), naturalHeight));
  const float requestedOffset = windowHeight * mStyle.GetBottomOffsetRatio() + mStyle.GetBottomOffset();
  const float offset          = std::min(requestedOffset, std::max(0.0f, windowHeight - height));
  const float x               = (windowWidth - width) * 0.5f;
  const float y               = std::max(0.0f, windowHeight - offset - height);

  if(expanded)
  {
    mExpandedWidth  = width;
    mExpandedHeight = height;
    mExpandedX      = x;
    mExpandedY      = y;
  }
  else
  {
    mDefaultWidth  = width;
    mDefaultHeight = height;
    mDefaultX      = x;
    mDefaultY      = y;

    // Expansion uses the available maximum width and recomputes wrapped height.
    ConfigureContent(true);
    const Insets expandedPadding = mStyle.GetExpandedPadding();
    const float  expandedWidth   = maximumWidth;
    const float  textWidth       = std::max(1.0f, expandedWidth - expandedPadding.start - expandedPadding.end);
    const float  expandedNatural = mMessage.GetHeightForWidth(textWidth) + actionSize.height +
                                  (mActionButtonText.Empty() ? 0.0f : (mItemSpacingOverridden ? mItemSpacing : mStyle.GetExpandedItemSpacing())) +
                                  expandedPadding.top + expandedPadding.bottom;
    mExpandedWidth             = expandedWidth;
    mExpandedHeight            = std::min(windowHeight, Clamp(std::max(mStyle.GetExpandedHeight(), expandedNatural), mStyle.GetExpandedHeight(), mStyle.GetMaximumHeight()));
    mExpandedX                 = (windowWidth - expandedWidth) * 0.5f;
    const float expandedOffset = std::min(requestedOffset, std::max(0.0f, windowHeight - mExpandedHeight));
    mExpandedY                 = std::max(0.0f, windowHeight - expandedOffset - mExpandedHeight);
    ConfigureContent(false);
  }

  const float targetX      = expanded ? mExpandedX : mDefaultX;
  const float targetY      = expanded ? mExpandedY : mDefaultY;
  const float targetWidth  = expanded ? mExpandedWidth : mDefaultWidth;
  const float targetHeight = expanded ? mExpandedHeight : mDefaultHeight;

  Ui::View self = Ui::View::DownCast(Self());
  self.SetRequestedX(targetX);
  self.SetRequestedY(targetY);
  self.SetRequestedWidth(targetWidth);
  self.SetRequestedHeight(targetHeight);
  self.Arrange(LayoutRect(targetX, targetY, targetWidth, targetHeight));
  mContent.Measure(targetWidth, targetHeight);
  mContent.Arrange(LayoutRect(0.0f, 0.0f, targetWidth, targetHeight));
}

void ToastImpl::StartPostTransition()
{
  StopTransition(false);
  mTransitionGeneration = mGeneration;

  Ui::View           self            = Ui::View::DownCast(Self());
  const PositionSize windowBounds    = mWindow.GetPositionSize();
  const float        minimumWidth    = std::min(mStyle.GetMinimumWidth(), mDefaultWidth);
  const float        minimumHeight   = std::min(mStyle.GetMinimumHeight(), mDefaultHeight);
  const float        initialWidth    = std::min(mStyle.GetInitialWidth(), mDefaultWidth);
  const float        initialHeight   = std::min(mStyle.GetInitialHeight(), mDefaultHeight);
  const float        requestedOffset = static_cast<float>(windowBounds.height) * mStyle.GetBottomOffsetRatio() + mStyle.GetBottomOffset();
  const float        phaseOneY       = std::max(0.0f, static_cast<float>(windowBounds.height) - requestedOffset * 2.0f - mDefaultHeight);
  self.SetProperty(Actor::Property::SIZE_WIDTH, initialWidth);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, initialHeight);
  self.SetProperty(Actor::Property::POSITION_X, mDefaultX + (mDefaultWidth - initialWidth) * 0.5f);
  self.SetProperty(Actor::Property::POSITION_Y, static_cast<float>(windowBounds.height));
  self.SetOpacity(0.0f);
  mContent.SetOpacity(0.0f);

  const AlphaFunction initialEasing(POST_INITIAL_CP1, POST_INITIAL_CP2);
  const AlphaFunction settleEasing(POST_SETTLE_CP1, POST_SETTLE_CP2);
  const Ui::Duration  initialTiming(POST_INITIAL_SECONDS);
  const Ui::Duration  settleTiming(POST_SETTLE_SECONDS);
  const Ui::Duration  settleDelay(POST_INITIAL_SECONDS);
  const Ui::Duration  contentDelay(POST_INITIAL_SECONDS + POST_SETTLE_SECONDS - CONTENT_FADE_SECONDS);

  mTransition = Animation::New(POST_INITIAL_SECONDS + POST_SETTLE_SECONDS);
  self.Animate(mTransition)
    .SizeWidth(minimumWidth, initialTiming, initialEasing)
    .SizeHeight(minimumHeight, initialTiming, initialEasing)
    .PositionX(mDefaultX + (mDefaultWidth - minimumWidth) * 0.5f, initialTiming, initialEasing)
    .PositionY(phaseOneY, initialTiming, initialEasing)
    .Opacity(1.0f, initialTiming, initialEasing)
    .SizeWidth(mDefaultWidth, settleTiming, settleEasing, settleDelay)
    .SizeHeight(mDefaultHeight, settleTiming, settleEasing, settleDelay)
    .PositionX(mDefaultX, settleTiming, settleEasing, settleDelay)
    .PositionY(mDefaultY, settleTiming, settleEasing, settleDelay);
  mContent.Animate(mTransition).Opacity(1.0f, Ui::Duration(CONTENT_FADE_SECONDS), AlphaFunction::LINEAR, contentDelay);
  mTransition.FinishedSignal().Connect(this, &ToastImpl::FinishPostTransition);
  mTransition.Play();
}

void ToastImpl::StartExpandTransition()
{
  StopTimeout();
  StopTransition(true);

  Ui::View    self           = Ui::View::DownCast(Self());
  const float currentWidth   = self.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float currentHeight  = self.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  const float currentX       = self.GetProperty<float>(Actor::Property::POSITION_X);
  const float currentY       = self.GetProperty<float>(Actor::Property::POSITION_Y);
  const float currentOpacity = self.GetProperty<float>(Actor::Property::OPACITY);

  UpdatePresentationGeometry(true);
  self.SetProperty(Actor::Property::SIZE_WIDTH, currentWidth);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, currentHeight);
  self.SetProperty(Actor::Property::POSITION_X, currentX);
  self.SetProperty(Actor::Property::POSITION_Y, currentY);
  self.SetOpacity(currentOpacity);
  mState                = PresentationState::EXPANDING;
  mTransitionGeneration = mGeneration;

  mContent.SetOpacity(0.0f);
  mTransition = Animation::New(EXPAND_SECONDS);
  const AlphaFunction easing(EXPAND_CP1, EXPAND_CP2);
  const Ui::Duration  timing(EXPAND_SECONDS);
  self.Animate(mTransition)
    .SizeWidth(mExpandedWidth, timing, easing)
    .SizeHeight(mExpandedHeight, timing, easing)
    .PositionX(mExpandedX, timing, easing)
    .PositionY(mExpandedY, timing, easing);
  mContent.Animate(mTransition).Opacity(1.0f, Ui::Duration(CONTENT_FADE_SECONDS), easing, Ui::Duration(EXPAND_FADE_DELAY));
  mTransition.FinishedSignal().Connect(this, &ToastImpl::FinishExpandTransition);
  mTransition.Play();
}

void ToastImpl::StartDismissTransition()
{
  StopTimeout();
  StopTransition(true);
  mState                = PresentationState::DISMISSING;
  mTransitionGeneration = mGeneration;

  Ui::View    self    = Ui::View::DownCast(Self());
  const float hiddenY = mWindow ? static_cast<float>(mWindow.GetPositionSize().height) : self.GetProperty<float>(Actor::Property::POSITION_Y);
  mTransition         = Animation::New(DISMISS_SECONDS);
  self.Animate(mTransition)
    .Opacity(0.0f, Ui::Duration(DISMISS_SECONDS), AlphaFunction::LINEAR)
    .PositionY(hiddenY, Ui::Duration(DISMISS_SECONDS), AlphaFunction::LINEAR);
  mContent.Animate(mTransition).Opacity(0.0f, Ui::Duration(DISMISS_SECONDS), AlphaFunction::LINEAR);
  mTransition.FinishedSignal().Connect(this, &ToastImpl::FinishDismissTransition);
  mTransition.Play();
}

void ToastImpl::FinishPostTransition(Animation animation)
{
  if(animation != mTransition || mTransitionGeneration != mGeneration || mState != PresentationState::POSTING)
  {
    return;
  }
  mTransition.Reset();
  mState = PresentationState::SHOWN_DEFAULT;
  Ui::View::DownCast(Self()).SetAccessibilityHidden(false);
  EmitShownOnce();
}

void ToastImpl::FinishExpandTransition(Animation animation)
{
  if(animation != mTransition || mTransitionGeneration != mGeneration || mState != PresentationState::EXPANDING)
  {
    return;
  }
  mTransition.Reset();
  mState = PresentationState::SHOWN_EXPANDED;
  Ui::View::DownCast(Self()).SetAccessibilityHidden(false);
  EmitShownOnce();
}

void ToastImpl::FinishDismissTransition(Animation animation)
{
  if(animation != mTransition || mTransitionGeneration != mGeneration || mState != PresentationState::DISMISSING)
  {
    return;
  }
  mTransition.Reset();
  Detach(true);
}

void ToastImpl::OnActionButtonClicked(Ui::View, Ui::InputEvent)
{
  if(mActionButtonText.Empty() || mState == PresentationState::DETACHED || mState == PresentationState::DISMISSING)
  {
    return;
  }

  const uint64_t generation = mGeneration;
  Ui::Toast      handle     = GetHandle();
  if(mState == PresentationState::POSTING || mState == PresentationState::SHOWN_DEFAULT)
  {
    StartExpandTransition();
    mActionButtonClickedSignal.Emit(handle, Ui::Toast::ActionStage::CONFIRM_IN_DEFAULT_MODE);
  }
  else
  {
    mActionButtonClickedSignal.Emit(handle, Ui::Toast::ActionStage::CONFIRM_IN_EXPANDED_MODE);
    if(generation == mGeneration && (mState == PresentationState::EXPANDING || mState == PresentationState::SHOWN_EXPANDED))
    {
      Dismiss();
    }
  }
}

void ToastImpl::OnWindowResized(Dali::Window window, Dali::Window::WindowSize)
{
  if(window != mWindow || mState == PresentationState::DETACHED || mState == PresentationState::DISMISSING)
  {
    return;
  }

  RelayoutForCurrentState();
}

void ToastImpl::RelayoutForCurrentState()
{
  if(!mWindow || mState == PresentationState::DETACHED || mState == PresentationState::DISMISSING)
  {
    return;
  }

  const PresentationState previousState = mState;
  const bool              expanded      = previousState == PresentationState::EXPANDING || previousState == PresentationState::SHOWN_EXPANDED;
  StopTransition(true);

  Ui::View    self                  = Ui::View::DownCast(Self());
  const float currentWidth          = self.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float currentHeight         = self.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  const float currentX              = self.GetProperty<float>(Actor::Property::POSITION_X);
  const float currentY              = self.GetProperty<float>(Actor::Property::POSITION_Y);
  const float currentOpacity        = self.GetProperty<float>(Actor::Property::OPACITY);
  const float currentContentOpacity = mContent.GetProperty<float>(Actor::Property::OPACITY);

  UpdatePresentationGeometry(expanded);

  if(previousState == PresentationState::POSTING || previousState == PresentationState::EXPANDING)
  {
    self.SetProperty(Actor::Property::SIZE_WIDTH, currentWidth);
    self.SetProperty(Actor::Property::SIZE_HEIGHT, currentHeight);
    self.SetProperty(Actor::Property::POSITION_X, currentX);
    self.SetProperty(Actor::Property::POSITION_Y, currentY);
    self.SetOpacity(currentOpacity);
    mContent.SetOpacity(currentContentOpacity);

    const float         targetWidth  = expanded ? mExpandedWidth : mDefaultWidth;
    const float         targetHeight = expanded ? mExpandedHeight : mDefaultHeight;
    const float         targetX      = expanded ? mExpandedX : mDefaultX;
    const float         targetY      = expanded ? mExpandedY : mDefaultY;
    const float         seconds      = expanded ? EXPAND_SECONDS : POST_SETTLE_SECONDS;
    const AlphaFunction easing       = expanded ? AlphaFunction(EXPAND_CP1, EXPAND_CP2) : AlphaFunction(POST_SETTLE_CP1, POST_SETTLE_CP2);

    mState                = previousState;
    mTransitionGeneration = mGeneration;
    mTransition           = Animation::New(seconds);
    self.Animate(mTransition)
      .SizeWidth(targetWidth, Ui::Duration(seconds), easing)
      .SizeHeight(targetHeight, Ui::Duration(seconds), easing)
      .PositionX(targetX, Ui::Duration(seconds), easing)
      .PositionY(targetY, Ui::Duration(seconds), easing)
      .Opacity(1.0f, Ui::Duration(seconds), easing);
    mContent.Animate(mTransition).Opacity(1.0f, Ui::Duration(CONTENT_FADE_SECONDS), easing);
    if(expanded)
    {
      mTransition.FinishedSignal().Connect(this, &ToastImpl::FinishExpandTransition);
    }
    else
    {
      mTransition.FinishedSignal().Connect(this, &ToastImpl::FinishPostTransition);
    }
    mTransition.Play();
    return;
  }

  if(previousState == PresentationState::SHOWN_EXPANDED)
  {
    self.SetProperty(Actor::Property::SIZE_WIDTH, mExpandedWidth);
    self.SetProperty(Actor::Property::SIZE_HEIGHT, mExpandedHeight);
    self.SetProperty(Actor::Property::POSITION_X, mExpandedX);
    self.SetProperty(Actor::Property::POSITION_Y, mExpandedY);
  }
  else
  {
    self.SetProperty(Actor::Property::SIZE_WIDTH, mDefaultWidth);
    self.SetProperty(Actor::Property::SIZE_HEIGHT, mDefaultHeight);
    self.SetProperty(Actor::Property::POSITION_X, mDefaultX);
    self.SetProperty(Actor::Property::POSITION_Y, mDefaultY);
  }
  self.SetOpacity(1.0f);
  mContent.SetOpacity(1.0f);
  self.SetAccessibilityHidden(false);
}

bool ToastImpl::OnTimeout(uint64_t generation)
{
  if(generation == mGeneration)
  {
    Dismiss();
  }
  return false;
}

void ToastImpl::StartTimeout()
{
  StopTimeout();
  if(mDuration == Ui::Toast::Duration::INDEFINITE || mState == PresentationState::DETACHED || mState == PresentationState::DISMISSING)
  {
    return;
  }
  const uint64_t generation = mGeneration;
  mTimer                    = Timer::New(mDuration);
  mTimer.TickSignal().Connect(this, [this, generation]()
  { return OnTimeout(generation); });
  mTimer.Start();
}

void ToastImpl::StopTimeout()
{
  if(mTimer)
  {
    mTimer.Stop();
    mTimer.Reset();
  }
}

void ToastImpl::StopTransition(bool preserveCurrentValues)
{
  if(!mTransition)
  {
    return;
  }

  Ui::View self = Ui::View::DownCast(Self());
  float    width{0.0f};
  float    height{0.0f};
  float    x{0.0f};
  float    y{0.0f};
  float    opacity{0.0f};
  float    contentOpacity{0.0f};
  if(preserveCurrentValues)
  {
    width          = self.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH);
    height         = self.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT);
    x              = self.GetCurrentProperty<float>(Actor::Property::POSITION_X);
    y              = self.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
    opacity        = self.GetCurrentProperty<float>(Actor::Property::OPACITY);
    contentOpacity = mContent.GetCurrentProperty<float>(Actor::Property::OPACITY);
  }

  mTransition.FinishedSignal().Disconnect(this, &ToastImpl::FinishPostTransition);
  mTransition.FinishedSignal().Disconnect(this, &ToastImpl::FinishExpandTransition);
  mTransition.FinishedSignal().Disconnect(this, &ToastImpl::FinishDismissTransition);
  mTransition.Stop();
  mTransition.Reset();

  if(preserveCurrentValues)
  {
    self.SetProperty(Actor::Property::SIZE_WIDTH, width);
    self.SetProperty(Actor::Property::SIZE_HEIGHT, height);
    self.SetProperty(Actor::Property::POSITION_X, x);
    self.SetProperty(Actor::Property::POSITION_Y, y);
    self.SetOpacity(opacity);
    mContent.SetOpacity(contentOpacity);
  }
}

void ToastImpl::Detach(bool emitHidden)
{
  // Keep the body alive while removing its final scene-owned reference and
  // while application callbacks run from HiddenSignal().
  Ui::Toast handle = GetHandle();

  StopTimeout();
  StopTransition(false);

  Dali::Window oldWindow = mWindow;
  Dali::Layer  oldLayer  = mOverlayLayer;
  if(oldWindow)
  {
    oldWindow.ResizedSignal().Disconnect(this, &ToastImpl::OnWindowResized);
  }

  mState = PresentationState::DETACHED;
  mWindow.Reset();
  mOverlayLayer.Reset();
  Ui::View self = Ui::View::DownCast(Self());
  self.SetAccessibilityHidden(true);
  self.SetOpacity(0.0f);

  if(oldLayer && self.GetParent() == oldLayer)
  {
    mInternalDetach = true;
    oldLayer.Remove(self);
    mInternalDetach = false;
  }

  if(emitHidden)
  {
    mHiddenSignal.Emit(handle);
  }
}

void ToastImpl::OnSceneDisconnection()
{
  ViewImpl::OnSceneDisconnection();
  if(!mInternalDetach && mState != PresentationState::DETACHED)
  {
    ++mGeneration;
    Detach(true);
  }
}

bool ToastImpl::OnAccessibilityRequestDefaultName(Dali::String& value)
{
  value = mText;
  if(!mActionButtonText.Empty())
  {
    if(!value.Empty())
    {
      value += ' ';
    }
    value += mActionButtonText;
  }
  return !value.Empty();
}

void ToastImpl::EmitShownOnce()
{
  if(mShownEmitted)
  {
    return;
  }
  mShownEmitted = true;
  mShownSignal.Emit(GetHandle());
}

Ui::Toast ToastImpl::GetHandle()
{
  return Ui::Toast::DownCast(Self());
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
