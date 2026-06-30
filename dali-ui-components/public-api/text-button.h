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
#include <dali-ui-components/public-api/styles/text-button-style.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali/public-api/common/dali-string.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class TextButtonImpl;
}

/**
 * @brief TextButton is a label-only interactive button.
 */
class DALI_UI_API TextButton : public InteractiveView
{
public:
  TextButton();
  ~TextButton();

  static TextButton New();
  static TextButton New(TextButtonStyle style);
  static TextButton New(const Dali::String& text);
  static TextButton New(const Dali::String& text, TextButtonStyle style);
  static TextButton DownCast(BaseHandle handle);

  TextButton(const TextButton& handle);
  TextButton(TextButton&& rhs) noexcept;
  TextButton& operator=(const TextButton& handle);
  TextButton& operator=(TextButton&& rhs) noexcept;

  DALI_UI_VIEW_WITH(TextButton)

  void         SetText(const Dali::String& text);
  Dali::String GetText() const;

  void            SetHorizontalAlignment(LayoutAlignment alignment);
  LayoutAlignment GetHorizontalAlignment() const;

  void            SetVerticalAlignment(LayoutAlignment alignment);
  LayoutAlignment GetVerticalAlignment() const;

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
  explicit DALI_INTERNAL TextButton(Internal::TextButtonImpl& implementation);
  explicit DALI_INTERNAL TextButton(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
