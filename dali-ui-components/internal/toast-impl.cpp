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

#include <dali-ui-components/internal/toast-impl.h>

#include <algorithm>

#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>

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

constexpr float   POST_SECONDS    = 0.3f;
constexpr float   DISMISS_SECONDS = 0.3f;
constexpr Vector2 MOTION_CONTROL_POINT_1(0.17f, 0.17f);
constexpr Vector2 MOTION_CONTROL_POINT_2(0.2f, 1.0f);
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

ToastImpl::~ToastImpl()
{
  StopTimeout();
  CancelScaleRelayoutIdle();
  StopTransition(false);
  if(mWindow)
  {
    mWindow.ResizedSignal().Disconnect(this, &ToastImpl::OnWindowResized);
  }
}

void ToastImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(StackOrientation::HORIZONTAL, 0.0f));
  Ui::View self                = Ui::View::DownCast(Self());
  mEffectiveScalePropertyIndex = self.GetPropertyIndex("viewEffectiveScale");
  self.PropertySetSignal().Connect(this, &ToastImpl::OnEffectiveScalePropertySet);
  self.SetLayoutMode(LayoutMode::STANDALONE);
  self.SetAccessibilityRole(Accessibility::Role::NOTIFICATION);
  self.SetAccessibilityHighlightable(false);
  self.SetAccessibilityHidden(true);

  mMessage = Ui::Label::New();
  mMessage.SetMultiLine(true);
  mMessage.SetLineWrapMode(Text::LineWrapMode::WORD);
  mMessage.SetVerticalTextAlignment(Text::Alignment::CENTER);
  mMessage.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
  mMessage.SetAccessibilityHidden(true);
  self.Add(mMessage);
}

void ToastImpl::ApplyInitialStyle(ToastStyle style)
{
  mStyle        = style;
  mIconColor    = style.GetIconColor();
  Ui::View self = Ui::View::DownCast(Self());
  self.SetBackgroundColor(style.GetBackgroundColor());
  self.SetCornerRadius(style.GetCornerRadius());
  self.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
  self.SetShadow(style.GetShadow());
  self.SetBorderlineWidth(style.GetBorderlineWidth());
  self.SetBorderlineOffset(style.GetBorderlineOffset());
  self.SetBorderlineColor(style.GetBorderlineColor());
  self.SetPadding(style.GetPadding());
  static_cast<StackLayoutManager*>(GetLayoutManager())->SetSpacing(style.GetItemSpacing());
  mMessage.SetTextColor(style.GetTextColor());
  mMessage.SetFontSize(style.GetFontSize());
  mMessage.SetFontFamily(style.GetFontFamily());
}

void ToastImpl::SetText(const Dali::String& text)
{
  mText = text;
  mMessage.SetText(text);
  RelayoutActivePresentation();
}

Dali::String ToastImpl::GetText() const
{
  return mText;
}

void ToastImpl::SetDuration(uint32_t duration)
{
  DALI_ASSERT_ALWAYS(duration > 0u && "Toast duration must be greater than zero");
  mDuration = duration;
  if(mState == PresentationState::POSTING || mState == PresentationState::SHOWN)
  {
    StartTimeout();
  }
}

uint32_t ToastImpl::GetDuration() const
{
  return mDuration;
}

void ToastImpl::EnsureIcon()
{
  if(mIcon)
  {
    return;
  }

  mIcon = Ui::ImageView::New();
  mIcon.SetRequestedWidth(mStyle.GetIconWidth());
  mIcon.SetRequestedHeight(mStyle.GetIconHeight());
  mIcon.SetCornerRadius(mStyle.GetIconCornerRadius());
  mIcon.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
  mIcon.SetImageColor(mIconColor);
  mIcon.SetSynchronousLoading(mIconSynchronousLoading);
  mIcon.SetVisible(!mIconResourceUrl.Empty());
  mIcon.SetLayoutMode(mIconResourceUrl.Empty() ? LayoutMode::STANDALONE : LayoutMode::DEFAULT);
  mIcon.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
  mIcon.SetAccessibilityHidden(true);

  Ui::View self = Ui::View::DownCast(Self());
  self.Remove(mMessage);
  self.Add(mIcon);
  self.Add(mMessage);
}

void ToastImpl::SetIconResourceUrl(const Dali::String& url)
{
  EnsureIcon();
  mIconResourceUrl = url;
  mIcon.SetResourceUrl(url);
  mIcon.SetVisible(!url.Empty());
  mIcon.SetLayoutMode(url.Empty() ? LayoutMode::STANDALONE : LayoutMode::DEFAULT);
  RelayoutActivePresentation();
}

Dali::String ToastImpl::GetIconResourceUrl() const
{
  return mIconResourceUrl;
}

void ToastImpl::SetIconColor(const UiColor& color)
{
  mIconColor = color;
  EnsureIcon();
  mIcon.SetImageColor(color);
}

UiColor ToastImpl::GetIconColor() const
{
  return mIconColor;
}

void ToastImpl::SetIconSynchronousLoading(bool synchronous)
{
  mIconSynchronousLoading = synchronous;
  EnsureIcon();
  mIcon.SetSynchronousLoading(synchronous);
}

bool ToastImpl::IsIconSynchronousLoading() const
{
  return mIconSynchronousLoading;
}

void ToastImpl::Post(Dali::Window window)
{
  DALI_ASSERT_ALWAYS(window && "Toast::Post requires an initialized Window");
  Dali::Layer overlay = window.GetOverlayLayer();
  DALI_ASSERT_ALWAYS(overlay && "Toast::Post requires a Window overlay layer");

  ++mGeneration;
  StopTimeout();
  CancelScaleRelayoutIdle();
  StopTransition(false);
  if(mState != PresentationState::DETACHED)
  {
    Detach(false);
  }

  mWindow       = window;
  mOverlayLayer = overlay;
  mWindow.ResizedSignal().Connect(this, &ToastImpl::OnWindowResized);
  mInputPanelOverlap = ResolveInputPanelOverlap();
  mShownEmitted      = false;
  mState             = PresentationState::POSTING;

  Ui::View self = Ui::View::DownCast(Self());
  self.SetAccessibilityHidden(true);
  mOverlayLayer.Add(self);
  self.RaiseToTop();
  UpdatePresentationGeometry();
  StartPostTransition();
  StartTimeout();
}

void ToastImpl::Dismiss()
{
  if(mState != PresentationState::DETACHED && mState != PresentationState::DISMISSING)
  {
    StartDismissTransition();
  }
}

float ToastImpl::ResolveInputPanelOverlap() const
{
  Ui::View           focused = Ui::FocusManager::Get().GetCurrentFocusView();
  InputMethodContext context;
  if(Ui::InputField field = Ui::InputField::DownCast(focused))
  {
    context = field.GetInputMethodContext();
  }
  else if(Ui::InputEditor editor = Ui::InputEditor::DownCast(focused))
  {
    context = editor.GetInputMethodContext();
  }

  if(!context)
  {
    return 0.0f;
  }
  const InputMethodContext::State panelState = context.GetInputPanelState();
  if(panelState != InputMethodContext::State::SHOW && panelState != InputMethodContext::State::WILL_SHOW)
  {
    return 0.0f;
  }
  return CalculateToastInputPanelOverlap(mWindow.GetPositionSize(), context.GetInputPanelArea(), panelState);
}

void ToastImpl::RelayoutActivePresentation()
{
  if(mState == PresentationState::DETACHED || mState == PresentationState::DISMISSING)
  {
    return;
  }

  const bool restartPostTransition = mState == PresentationState::POSTING;
  if(restartPostTransition)
  {
    StopTransition(false);
  }
  UpdatePresentationGeometry();
  if(restartPostTransition)
  {
    StartPostTransition();
  }
}

void ToastImpl::ArrangePresentationAt(float y)
{
  Ui::View    self  = Ui::View::DownCast(Self());
  const float scale = std::max(0.001f, GetEffectiveScale());
  self.SetRequestedX(mPresentationX / scale);
  self.SetRequestedY(y / scale);
  self.SetRequestedWidth(mPresentationWidth / scale);
  self.SetRequestedHeight(mPresentationHeight / scale);
  self.Measure(mPresentationWidth, mPresentationHeight);
  self.Arrange(LayoutRect(mPresentationX, y, mPresentationWidth, mPresentationHeight));
}

void ToastImpl::UpdatePresentationGeometry()
{
  if(!mWindow)
  {
    return;
  }

  const PositionSize bounds       = mWindow.GetPositionSize();
  const float        windowWidth  = std::max(0.0f, static_cast<float>(bounds.width));
  const float        windowHeight = std::max(0.0f, static_cast<float>(bounds.height));
  const float        scale        = std::max(0.001f, GetEffectiveScale());
  mAppliedScale                   = scale;
  const Insets padding            = mStyle.GetPadding();
  const bool   hasIcon            = mIcon && !mIconResourceUrl.Empty();
  const float  bottomOffset       = windowHeight * mStyle.GetBottomOffsetRatio();
  const float  availableHeight    = std::max(0.0f, windowHeight - mInputPanelOverlap - bottomOffset);
  const float  maximumWidth       = windowWidth * mStyle.GetMaximumWidthRatio();
  const float  maximumHeight      = std::min(availableHeight, mStyle.GetMaximumHeight() * scale);

  const float horizontalPadding = (padding.start + padding.end) * scale;
  const float verticalPadding   = (padding.top + padding.bottom) * scale;
  const float horizontalScale   = horizontalPadding > maximumWidth && horizontalPadding > 0.0f ? maximumWidth / horizontalPadding : 1.0f;
  const float verticalScale     = verticalPadding > maximumHeight && verticalPadding > 0.0f ? maximumHeight / verticalPadding : 1.0f;
  const float startPadding      = padding.start * scale * horizontalScale;
  const float endPadding        = padding.end * scale * horizontalScale;
  const float topPadding        = padding.top * scale * verticalScale;
  const float bottomPadding     = padding.bottom * scale * verticalScale;
  const float contentWidth      = std::max(0.0f, maximumWidth - startPadding - endPadding);
  const float contentHeight     = std::max(0.0f, maximumHeight - topPadding - bottomPadding);
  const float iconWidth         = hasIcon ? std::min(mStyle.GetIconWidth() * scale, contentWidth) : 0.0f;
  const float iconHeight        = hasIcon ? std::min(mStyle.GetIconHeight() * scale, contentHeight) : 0.0f;
  const float spacing           = hasIcon ? std::min(mStyle.GetItemSpacing() * scale, std::max(0.0f, contentWidth - iconWidth)) : 0.0f;
  const float textWidth         = std::max(0.0f, contentWidth - iconWidth - spacing);

  Ui::View self = Ui::View::DownCast(Self());
  self.SetPadding(Insets(padding.start * horizontalScale,
                         padding.end * horizontalScale,
                         padding.top * verticalScale,
                         padding.bottom * verticalScale));
  static_cast<StackLayoutManager*>(GetLayoutManager())->SetSpacing(spacing / scale);
  mMessage.SetMaximumWidth(textWidth / scale);
  mMessage.SetMaximumHeight(contentHeight / scale);
  if(mIcon)
  {
    mIcon.SetMaximumWidth(iconWidth / scale);
    mIcon.SetMaximumHeight(iconHeight / scale);
  }

  self.SetMaximumWidth(maximumWidth / scale);
  self.SetMaximumHeight(maximumHeight / scale);
  self.SetRequestedWidth(WRAP_CONTENT);
  self.SetRequestedHeight(WRAP_CONTENT);
  const MeasuredSize measured = self.Measure(maximumWidth, maximumHeight);
  const float        width    = std::min(maximumWidth, measured.width);
  const float        height   = std::min(maximumHeight, measured.height);
  const float        x        = (windowWidth - width) * 0.5f;
  const float        y        = windowHeight - mInputPanelOverlap - bottomOffset - height;

  mPresentationX      = x;
  mPresentationWidth  = width;
  mPresentationHeight = height;
  mRestingY           = y;
  mEdgeY              = windowHeight - mInputPanelOverlap - height;
  ArrangePresentationAt(mRestingY);
}

void ToastImpl::StartPostTransition()
{
  StopTransition(false);
  mTransitionGeneration = mGeneration;
  Ui::View self         = Ui::View::DownCast(Self());
  ArrangePresentationAt(mEdgeY);
  self.SetOpacity(0.0f);
  const AlphaFunction easing(MOTION_CONTROL_POINT_1, MOTION_CONTROL_POINT_2);
  mTransition = Animation::New(POST_SECONDS);
  self.Animate(mTransition)
    .Opacity(1.0f, Ui::Duration(POST_SECONDS), easing)
    .PositionY(mRestingY, Ui::Duration(POST_SECONDS), easing);
  mTransition.FinishedSignal().Connect(this, &ToastImpl::FinishPostTransition);
  mTransition.Play();
}

void ToastImpl::StartDismissTransition()
{
  StopTimeout();
  StopTransition(true);
  mState                   = PresentationState::DISMISSING;
  mTransitionGeneration    = mGeneration;
  Ui::View            self = Ui::View::DownCast(Self());
  const AlphaFunction easing(MOTION_CONTROL_POINT_1, MOTION_CONTROL_POINT_2);
  mTransition = Animation::New(DISMISS_SECONDS);
  self.Animate(mTransition)
    .Opacity(0.0f, Ui::Duration(DISMISS_SECONDS), easing)
    .PositionY(mEdgeY, Ui::Duration(DISMISS_SECONDS), easing);
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
  mState = PresentationState::SHOWN;
  ArrangePresentationAt(mRestingY);
  Ui::View::DownCast(Self()).SetAccessibilityHidden(false);
  if(!mShownEmitted)
  {
    mShownEmitted = true;
    mShownSignal.Emit(GetHandle());
  }
}

void ToastImpl::FinishDismissTransition(Animation animation)
{
  if(animation == mTransition && mTransitionGeneration == mGeneration && mState == PresentationState::DISMISSING)
  {
    mTransition.Reset();
    Detach(true);
  }
}

void ToastImpl::OnWindowResized(Dali::Window window, Dali::Window::WindowSize)
{
  if(window == mWindow && (mState == PresentationState::POSTING || mState == PresentationState::SHOWN))
  {
    mInputPanelOverlap               = ResolveInputPanelOverlap();
    const bool restartPostTransition = mState == PresentationState::POSTING;
    if(restartPostTransition)
    {
      StopTransition(false);
    }
    UpdatePresentationGeometry();
    if(restartPostTransition)
    {
      StartPostTransition();
    }
  }
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
  if(mState == PresentationState::DETACHED || mState == PresentationState::DISMISSING)
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

void ToastImpl::StopTransition(bool preserveOpacity)
{
  if(!mTransition)
  {
    return;
  }
  Ui::View    self    = Ui::View::DownCast(Self());
  const float opacity = preserveOpacity ? self.GetCurrentProperty<float>(Actor::Property::OPACITY) : 0.0f;
  mTransition.FinishedSignal().Disconnect(this, &ToastImpl::FinishPostTransition);
  mTransition.FinishedSignal().Disconnect(this, &ToastImpl::FinishDismissTransition);
  mTransition.Stop();
  mTransition.Reset();
  if(preserveOpacity)
  {
    self.SetOpacity(opacity);
  }
}

void ToastImpl::Detach(bool emitHidden)
{
  Ui::Toast handle = GetHandle();
  StopTimeout();
  CancelScaleRelayoutIdle();
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

void ToastImpl::OnEffectiveScalePropertySet(Dali::Handle, Dali::Property::Index index, const Dali::Property::Value& value)
{
  if(index != mEffectiveScalePropertyIndex)
  {
    return;
  }

  float scale{1.0f};
  if(!value.Get(scale) || scale == mAppliedScale)
  {
    return;
  }

  if(!mScaleRelayoutIdle && Adaptor::IsAvailable())
  {
    mScaleRelayoutIdle = MakeCallback(this, &ToastImpl::OnScaleRelayoutIdle);
    if(!Adaptor::Get().AddIdle(mScaleRelayoutIdle, false))
    {
      mScaleRelayoutIdle = nullptr;
    }
  }
}

void ToastImpl::OnScaleRelayoutIdle()
{
  mScaleRelayoutIdle = nullptr;
  if(GetEffectiveScale() == mAppliedScale)
  {
    return;
  }

  if(mState == PresentationState::DISMISSING)
  {
    Ui::View    self           = Ui::View::DownCast(Self());
    const float currentY       = self.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
    const float travel         = mEdgeY - mRestingY;
    const float motionProgress = travel > 0.0f ? std::clamp((currentY - mRestingY) / travel, 0.0f, 1.0f) : 1.0f;
    StopTransition(true);
    UpdatePresentationGeometry();
    ArrangePresentationAt(mRestingY + (mEdgeY - mRestingY) * motionProgress);
    StartDismissTransition();
    return;
  }
  RelayoutActivePresentation();
}

void ToastImpl::CancelScaleRelayoutIdle()
{
  if(mScaleRelayoutIdle && Adaptor::IsAvailable())
  {
    Adaptor::Get().RemoveIdle(mScaleRelayoutIdle);
    mScaleRelayoutIdle = nullptr;
  }
}

bool ToastImpl::OnAccessibilityRequestDefaultName(Dali::String& value)
{
  value = mText;
  return !value.Empty();
}

Ui::Toast ToastImpl::GetHandle()
{
  return Ui::Toast::DownCast(Self());
}
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
