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
#include <dali-ui-components/public-api/selection-animation-mode.h>
#include <dali-ui-components/public-api/styles/check-box-style.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/selectable-view.h>
#include <dali/public-api/common/dali-string.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class CheckBoxImpl;
}

/**
 * @brief CheckBox is a binary (checked/unchecked) selectable control with an
 *        optional trailing text label. Its glyph is a single Lottie animation whose
 *        check/uncheck transition plays a frame range and recolours its inner fill.
 *
 * Selection state is inherited from SelectableView
 * (IsSelected/SetSelected/SelectionChangedSignal/IsToggleByClickEnabled/
 * SetToggleByClickEnabled) and is intentionally NOT redeclared here.
 */
class DALI_UI_COMPONENTS_API CheckBox : public SelectableView
{
public:
  CheckBox();
  ~CheckBox();

  static CheckBox New();
  static CheckBox New(CheckBoxStyle style);
  static CheckBox New(const Dali::String& text);
  static CheckBox New(const Dali::String& text, CheckBoxStyle style);
  static CheckBox DownCast(BaseHandle handle);

  CheckBox(const CheckBox& handle);
  CheckBox(CheckBox&& rhs) noexcept;
  CheckBox& operator=(const CheckBox& handle);
  CheckBox& operator=(CheckBox&& rhs) noexcept;

  DALI_UI_VIEW_WITH(CheckBox)

  /**
   * @brief Sets the trailing label text. An empty string yields a box-only checkbox.
   */
  void         SetText(const Dali::String& text);
  Dali::String GetText() const;

  /**
   * @brief Sets/gets how a selection-state change animates (default AUTO).
   */
  void                   SetSelectionAnimationMode(SelectionAnimationMode mode);
  SelectionAnimationMode GetSelectionAnimationMode() const;

  /**
   * @brief Sets/gets the icon (glyph) width/height in logical pixels.
   *
   * These mirror CheckBoxStyle and can be changed at runtime. A non-positive value is "unset":
   * the width then follows the resolved icon height and the height follows the content height
   * (square glyph). The getters return the requested value (0 when unset), not the laid-out size.
   */
  void  SetIconWidth(float width);
  float GetIconWidth() const;
  void  SetIconHeight(float height);
  float GetIconHeight() const;

  /**
   * @brief Sets/gets the trailing label's text style — color, font size, font family, and
   * underline — mirroring CheckBoxStyle. An underline of Text::Underline::None() clears it.
   */
  void    SetTextColor(const UiColor& color);
  UiColor GetTextColor() const;

  void  SetFontSize(float fontSize);
  float GetFontSize() const;

  void         SetFontFamily(const Dali::String& fontFamily);
  Dali::String GetFontFamily() const;

  void            SetTextUnderline(const Text::Underline& underline);
  Text::Underline GetTextUnderline() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL CheckBox(Internal::CheckBoxImpl& implementation);
  explicit DALI_INTERNAL CheckBox(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
