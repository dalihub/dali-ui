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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/styles/switch-style.h>
#include <dali-ui-components/public-api/switch.h>
#include <dali-ui-foundation/extension-api/selectable-view-impl.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class SwitchImpl : public Extension::SelectableViewImpl
{
public:
  static Ui::Switch New(SwitchStyle style);

  void                   SetSelectionAnimationMode(SelectionAnimationMode mode);
  SelectionAnimationMode GetSelectionAnimationMode() const;

protected:
  void         OnInitialize() override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;
  LayoutRect   OnArrange(const LayoutRect& bounds) override;

  SwitchImpl();
  ~SwitchImpl() override;

private:
  void ApplyInitialStyle(SwitchStyle style);
  void OnSelectionChanged(Ui::View view, bool selected, Ui::InputEvent event);
  void OnViewStateChanged(Ui::View view, StateEvent event);
  void OnThemeChanged();
  void OnTransitionFinished(Animation animation);

  bool OnInterceptTouch(Actor actor, TouchEvent event);
  bool OnTouch(Actor actor, TouchEvent event);

  bool  CanUserToggle() const;
  bool  IsSelectionAnimationRequired(const InputEvent& event) const;
  void  CancelTransition();
  void  StopTransitionAtCurrentPosition();
  void  AnimateToSelection(bool selected);
  void  SnapToSelection(bool selected);
  void  SetDragPosition(float position);
  void  ApplyTrackColorForPosition(float position);
  float GetTravelDistance() const;
  float GetCurrentLogicalPosition() const;
  float LogicalPositionToActorX(float position) const;
  bool  IsRightToLeft() const;

private:
  Ui::View mTrack;
  Ui::View mThumbEffectTarget;
  Ui::View mThumb;

  SwitchStyle mStyle;
  UiColor     mTrackColor;
  UiColor     mSelectedTrackColor;
  UiColor     mThumbColor;

  Animation mTransition;

  float mThumbPosition{0.0f};
  float mThumbEffectTargetBaseX{0.0f};
  float mThumbEffectTargetWidth{0.0f};
  float mArrangedWidth{0.0f};
  float mLastTouchX{0.0f};
  float mInitialTouchX{0.0f};
  float mInitialThumbPosition{0.0f};

  SelectionAnimationMode mSelectionAnimationMode{SelectionAnimationMode::AUTO};

  bool mTouchActive{false};
  bool mDragging{false};
};

} // namespace Internal

inline Internal::SwitchImpl& GetImpl(Ui::Switch& control)
{
  DALI_ASSERT_ALWAYS(control);
  return static_cast<Internal::SwitchImpl&>(control.GetImplementation());
}

inline const Internal::SwitchImpl& GetImpl(const Ui::Switch& control)
{
  DALI_ASSERT_ALWAYS(control);
  return static_cast<const Internal::SwitchImpl&>(control.GetImplementation());
}

} // namespace Ui
} // namespace Dali
