#pragma once

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

#include <dali-ui-components/public-api/styles/toast-style.h>
#include <dali-ui-components/public-api/toast.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>

#include <algorithm>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
inline float CalculateToastInputPanelOverlap(const PositionSize&             windowBounds,
                                             const BoundsInteger&            panelBounds,
                                             Dali::InputMethodContext::State panelState)
{
  if((panelState != Dali::InputMethodContext::State::SHOW && panelState != Dali::InputMethodContext::State::WILL_SHOW) ||
     windowBounds.width <= 0 || windowBounds.height <= 0 || panelBounds.width <= 0 || panelBounds.height <= 0)
  {
    return 0.0f;
  }

  const float windowLeft   = static_cast<float>(windowBounds.x);
  const float windowTop    = static_cast<float>(windowBounds.y);
  const float windowRight  = windowLeft + static_cast<float>(windowBounds.width);
  const float windowBottom = windowTop + static_cast<float>(windowBounds.height);
  const float panelLeft    = static_cast<float>(panelBounds.x);
  const float panelTop     = static_cast<float>(panelBounds.y);
  const float panelRight   = panelLeft + static_cast<float>(panelBounds.width);
  const float panelBottom  = panelTop + static_cast<float>(panelBounds.height);

  if(panelRight <= windowLeft || panelLeft >= windowRight || panelBottom <= windowTop || panelTop >= windowBottom)
  {
    return 0.0f;
  }
  return std::clamp(windowBottom - std::max(panelTop, windowTop), 0.0f, static_cast<float>(windowBounds.height));
}

class ToastImpl : public ViewImpl
{
public:
  static Ui::Toast New(ToastStyle style);

  void         SetText(const Dali::String& text);
  Dali::String GetText() const;
  void         SetDuration(uint32_t duration);
  uint32_t     GetDuration() const;
  void         SetIconResourceUrl(const Dali::String& url);
  Dali::String GetIconResourceUrl() const;
  void         SetIconColor(const UiColor& color);
  UiColor      GetIconColor() const;
  void         SetIconSynchronousLoading(bool synchronous);
  bool         IsIconSynchronousLoading() const;
  void         Post(Dali::Window window);
  void         Dismiss();

  Ui::Toast::ShownSignalType& ShownSignal()
  {
    return mShownSignal;
  }
  Ui::Toast::HiddenSignalType& HiddenSignal()
  {
    return mHiddenSignal;
  }

protected:
  void OnInitialize() override;
  void OnSceneDisconnection() override;
  bool OnAccessibilityRequestDefaultName(Dali::String& value) override;
  ToastImpl() = default;
  ~ToastImpl() override;

private:
  enum class PresentationState
  {
    DETACHED,
    POSTING,
    SHOWN,
    DISMISSING
  };

  void      ApplyInitialStyle(ToastStyle style);
  void      EnsureIcon();
  float     ResolveInputPanelOverlap() const;
  void      RelayoutActivePresentation();
  void      ArrangePresentationAt(float y);
  void      UpdatePresentationGeometry();
  void      StartPostTransition();
  void      StartDismissTransition();
  void      FinishPostTransition(Animation animation);
  void      FinishDismissTransition(Animation animation);
  void      OnWindowResized(Dali::Window window, Dali::Window::WindowSize size);
  bool      OnTimeout(uint64_t generation);
  void      StartTimeout();
  void      StopTimeout();
  void      StopTransition(bool preserveOpacity);
  void      OnEffectiveScalePropertySet(Dali::Handle handle, Dali::Property::Index index, const Dali::Property::Value& value);
  void      OnScaleRelayoutIdle();
  void      CancelScaleRelayoutIdle();
  void      Detach(bool emitHidden);
  Ui::Toast GetHandle();

  Ui::Label                   mMessage;
  Ui::ImageView               mIcon;
  ToastStyle                  mStyle;
  Dali::Window                mWindow;
  Dali::Layer                 mOverlayLayer;
  Timer                       mTimer;
  Animation                   mTransition;
  Dali::String                mText;
  Dali::String                mIconResourceUrl;
  UiColor                     mIconColor;
  uint32_t                    mDuration{Ui::Toast::Duration::SHORT};
  uint64_t                    mGeneration{0u};
  uint64_t                    mTransitionGeneration{0u};
  float                       mPresentationX{0.0f};
  float                       mPresentationWidth{0.0f};
  float                       mPresentationHeight{0.0f};
  float                       mRestingY{0.0f};
  float                       mEdgeY{0.0f};
  float                       mInputPanelOverlap{0.0f};
  float                       mAppliedScale{1.0f};
  Dali::Property::Index       mEffectiveScalePropertyIndex{Dali::Property::INVALID_INDEX};
  PresentationState           mState{PresentationState::DETACHED};
  bool                        mShownEmitted{false};
  bool                        mInternalDetach{false};
  bool                        mIconSynchronousLoading{false};
  Dali::CallbackBase*         mScaleRelayoutIdle{nullptr};
  Ui::Toast::ShownSignalType  mShownSignal;
  Ui::Toast::HiddenSignalType mHiddenSignal;
};
} // namespace Internal

inline Internal::ToastImpl& GetImpl(Ui::Toast& toast)
{
  DALI_ASSERT_ALWAYS(toast);
  return static_cast<Internal::ToastImpl&>(toast.GetImplementation());
}

inline const Internal::ToastImpl& GetImpl(const Ui::Toast& toast)
{
  DALI_ASSERT_ALWAYS(toast);
  return static_cast<const Internal::ToastImpl&>(toast.GetImplementation());
}
} // namespace Ui
} //namespace DALI_NAMESPACE
