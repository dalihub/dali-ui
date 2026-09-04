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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/styles/toast-style.h>
#include <dali-ui-components/public-api/text-button.h>
#include <dali-ui-components/public-api/toast.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class ToastImpl : public ViewImpl
{
public:
  static Ui::Toast New(ToastStyle style);

  void         SetText(const Dali::String& text);
  Dali::String GetText() const;
  void         SetDuration(uint32_t duration);
  uint32_t     GetDuration() const;
  void         SetTextColor(const UiColor& color);
  UiColor      GetTextColor() const;
  void         SetFontSize(float fontSize);
  float        GetFontSize() const;
  void         SetFontFamily(const Dali::String& fontFamily);
  Dali::String GetFontFamily() const;
  void         SetActionButtonText(const Dali::String& text);
  Dali::String GetActionButtonText() const;
  void         SetActionButtonTextColor(const UiColor& color);
  UiColor      GetActionButtonTextColor() const;
  void         SetItemSpacing(float spacing);
  float        GetItemSpacing() const;

  void Post(Dali::Window window);
  void Dismiss();

  Ui::Toast::ShownSignalType& ShownSignal()
  {
    return mShownSignal;
  }

  Ui::Toast::HiddenSignalType& HiddenSignal()
  {
    return mHiddenSignal;
  }

  Ui::Toast::ActionButtonClickedSignalType& ActionButtonClickedSignal()
  {
    return mActionButtonClickedSignal;
  }

protected:
  void OnInitialize() override;
  void OnSceneDisconnection() override;
  bool OnAccessibilityRequestDefaultName(Dali::String& value) override;

  ToastImpl();
  ~ToastImpl() override;

private:
  enum class PresentationState
  {
    DETACHED,
    POSTING,
    SHOWN_DEFAULT,
    EXPANDING,
    SHOWN_EXPANDED,
    DISMISSING
  };

  void      ApplyInitialStyle(ToastStyle style);
  void      ApplyContentStyle();
  void      ConfigureContent(bool expanded);
  void      UpdatePresentationGeometry(bool expanded);
  void      RelayoutForCurrentState();
  void      StartPostTransition();
  void      StartExpandTransition();
  void      StartDismissTransition();
  void      FinishPostTransition(Animation animation);
  void      FinishExpandTransition(Animation animation);
  void      FinishDismissTransition(Animation animation);
  void      OnActionButtonClicked(Ui::View view, Ui::InputEvent event);
  void      OnWindowResized(Dali::Window window, Dali::Window::WindowSize size);
  bool      OnTimeout(uint64_t generation);
  void      StartTimeout();
  void      StopTimeout();
  void      StopTransition(bool preserveCurrentValues);
  void      Detach(bool emitHidden);
  void      EmitShownOnce();
  Ui::Toast GetHandle();

private:
  Ui::StackLayout mContent;
  Ui::Label       mMessage;
  Ui::TextButton  mActionButton;

  ToastStyle   mStyle;
  Dali::Window mWindow;
  Dali::Layer  mOverlayLayer;
  Timer        mTimer;
  Animation    mTransition;

  Dali::String mText;
  Dali::String mActionButtonText{"Button"};
  UiColor      mTextColor;
  UiColor      mActionButtonTextColor;
  Dali::String mFontFamily;

  uint32_t mDuration{Ui::Toast::Duration::INDEFINITE};
  uint64_t mGeneration{0u};
  uint64_t mTransitionGeneration{0u};

  float mFontSize{0.0f};
  float mItemSpacing{0.0f};
  float mDefaultWidth{0.0f};
  float mDefaultHeight{0.0f};
  float mExpandedWidth{0.0f};
  float mExpandedHeight{0.0f};
  float mDefaultX{0.0f};
  float mDefaultY{0.0f};
  float mExpandedX{0.0f};
  float mExpandedY{0.0f};

  PresentationState mState{PresentationState::DETACHED};
  bool              mShownEmitted{false};
  bool              mInternalDetach{false};
  bool              mItemSpacingOverridden{false};

  Ui::Toast::ShownSignalType               mShownSignal;
  Ui::Toast::HiddenSignalType              mHiddenSignal;
  Ui::Toast::ActionButtonClickedSignalType mActionButtonClickedSignal;
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
} // namespace Dali
