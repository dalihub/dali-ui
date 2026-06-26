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
#include <dali-ui-components/public-api/text-button.h>
#include <dali-ui-foundation/provider-api/interactive-view-impl.h>
#include <dali-ui-foundation/public-api/label.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class TextButtonImpl : public Provider::InteractiveViewImpl
{
public:
  static Ui::TextButton New(TextButtonStyle style);

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

protected:
  void         OnInitialize() override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

  TextButtonImpl();
  ~TextButtonImpl() override;

private:
  void ApplyInitialStyle(TextButtonStyle style);
  void ApplyAlignment();

private:
  Ui::Label       mLabel;
  Text::Underline mUnderline{Text::Underline::None()};
  LayoutAlignment mHorizontalAlignment{LayoutAlignment::CENTER};
  LayoutAlignment mVerticalAlignment{LayoutAlignment::CENTER};
};

} // namespace Internal

inline Internal::TextButtonImpl& GetImpl(Ui::TextButton& button)
{
  DALI_ASSERT_ALWAYS(button);
  return static_cast<Internal::TextButtonImpl&>(button.GetImplementation());
}

inline const Internal::TextButtonImpl& GetImpl(const Ui::TextButton& button)
{
  DALI_ASSERT_ALWAYS(button);
  return static_cast<const Internal::TextButtonImpl&>(button.GetImplementation());
}

} // namespace Ui
} // namespace Dali
