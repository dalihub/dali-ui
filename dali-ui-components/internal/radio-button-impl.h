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

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/radio-button.h>
#include <dali-ui-components/public-api/styles/radio-button-style.h>
#include <dali-ui-foundation/extension-api/group-selectable-view-impl.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class RadioButtonImpl : public Extension::GroupSelectableViewImpl
{
public:
  static Ui::RadioButton New(RadioButtonStyle style);

  void                   SetSelectionAnimationMode(SelectionAnimationMode mode);
  SelectionAnimationMode GetSelectionAnimationMode() const;

  void  SetIconWidth(float width);
  float GetIconWidth() const;
  void  SetIconHeight(float height);
  float GetIconHeight() const;

protected:
  void         OnInitialize() override;
  void         OnSceneConnection(int depth) override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;
  LayoutRect   OnArrange(const LayoutRect& bounds) override;

  RadioButtonImpl();
  ~RadioButtonImpl() override;

private:
  void ApplyInitialStyle(RadioButtonStyle style);
  void OnSelectionChanged(View view, bool selected, InputEvent event);
  void OnViewStateChanged(Ui::View view, StateEvent event);
  void OnThemeChanged();
  void OnAnimationFinished(View view);

  bool IsSelectionAnimationRequired(const InputEvent& event) const;
  void RefreshRestingFrame();
  void PushStateColors();

private:
  Ui::SelectableImageInterface mIcon;
  float                        mIconWidth{0.0f};
  float                        mIconHeight{0.0f};
  UiColor                      mDeselectedIconColor;
  UiColor                      mSelectedIconColor;
  SelectionAnimationMode       mSelectionAnimationMode{SelectionAnimationMode::AUTO};
  bool                         mThemeRefreshPending{false};
};

} // namespace Internal

inline Internal::RadioButtonImpl& GetImpl(Ui::RadioButton& radioButton)
{
  DALI_ASSERT_ALWAYS(radioButton);
  return static_cast<Internal::RadioButtonImpl&>(radioButton.GetImplementation());
}

inline const Internal::RadioButtonImpl& GetImpl(const Ui::RadioButton& radioButton)
{
  DALI_ASSERT_ALWAYS(radioButton);
  return static_cast<const Internal::RadioButtonImpl&>(radioButton.GetImplementation());
}

} // namespace Ui
} // namespace Dali
