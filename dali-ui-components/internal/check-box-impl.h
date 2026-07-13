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
#include <dali-ui-components/public-api/check-box.h>
#include <dali-ui-components/public-api/styles/check-box-style.h>
#include <dali-ui-foundation/extension-api/selectable-view-impl.h>
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/image/i-selectable-image.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/common/dali-string.h>

// EXTERNAL INCLUDES
#include <cstdint>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class CheckBoxImpl : public Extension::SelectableViewImpl
{
public:
  static Ui::CheckBox New(CheckBoxStyle style);

  void         SetText(const Dali::String& text);
  Dali::String GetText() const;

  void                   SetSelectionAnimationMode(SelectionAnimationMode mode);
  SelectionAnimationMode GetSelectionAnimationMode() const;

protected:
  void         OnInitialize() override;
  void         OnSceneConnection(int depth) override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

  /**
   * @copydoc Ui::Internal::ViewImpl::CreateAccessibleObject()
   */
  ViewAccessible* CreateAccessibleObject() override;

  CheckBoxImpl();
  ~CheckBoxImpl() override;

  /**
   * @brief Accessible node for CheckBox: supplies the localizable default usage-hint
   * description derived from the current selection state.
   */
  class CheckBoxAccessible : public ViewAccessible
  {
  public:
    using ViewAccessible::ViewAccessible;

    /**
     * @copydoc Dali::Ui::ViewAccessible::GetDescriptionRaw()
     */
    std::string GetDescriptionRaw() const override;
  };

private:
  void ApplyInitialStyle(CheckBoxStyle style);
  void OnSelectionChanged(View view, bool selected, InputEvent event);
  void OnThemeChanged();
  void OnAnimationFinished(View view);

  // Decide, from the change's cause + on-scene/visible, whether to animate.
  bool IsSelectionAnimationRequired(const InputEvent& event) const;
  // Render the resting state for the current IsSelected() when the glyph is not playing.
  void RefreshRestingFrame();
  // Resolve the icon tokens against the current theme and push them into the glyph view.
  void PushStateColors();
  // Mirror selected -> a11y CHECKED bit + usage-hint description.
  void UpdateAccessibility(bool selected);

private:
  Ui::ISelectableImage mIcon;  ///< selectable image (drives its own frame-range + recolour)
  Ui::Label            mLabel; ///< optional trailing label

  Dali::String mText;

  // Captured from the style in ApplyInitialStyle():
  float   mBoxSize{0.0f};
  float   mGap{0.0f};
  UiColor mIconColor;         ///< deselected inner-fill token
  UiColor mSelectedIconColor; ///< selected inner-fill token

  SelectionAnimationMode mSelectionAnimationMode{SelectionAnimationMode::AUTO};

  bool mThemeRefreshPending{false};
};

} // namespace Internal

inline Internal::CheckBoxImpl& GetImpl(Ui::CheckBox& checkBox)
{
  DALI_ASSERT_ALWAYS(checkBox);
  return static_cast<Internal::CheckBoxImpl&>(checkBox.GetImplementation());
}

inline const Internal::CheckBoxImpl& GetImpl(const Ui::CheckBox& checkBox)
{
  DALI_ASSERT_ALWAYS(checkBox);
  return static_cast<const Internal::CheckBoxImpl&>(checkBox.GetImplementation());
}

} // namespace Ui
} // namespace Dali
