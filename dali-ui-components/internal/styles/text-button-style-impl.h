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
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>
#include <dali/public-api/common/constants.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class TextButtonStyleImpl : public Extension::UiStyleImpl
{
public:
  TextButtonStyleImpl();
  TextButtonStyleImpl(const TextButtonStyleImpl& rhs);

  void  SetMinimumWidth(float width);
  float GetMinimumWidth() const;

  void  SetMinimumHeight(float height);
  float GetMinimumHeight() const;

  void  SetMaximumWidth(float width);
  float GetMaximumWidth() const;

  void  SetMaximumHeight(float height);
  float GetMaximumHeight() const;

  void SetMinimumSize(const Vector2& size);
  void SetMaximumSize(const Vector2& size);

  void    SetCornerRadius(const Vector4& radius);
  Vector4 GetCornerRadius() const;

  void               SetCornerRadiusPolicy(CornerRadiusPolicy policy);
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  void    SetPadding(const Extents& padding);
  Extents GetPadding() const;

  void    SetBackgroundColor(const UiColor& color);
  UiColor GetBackgroundColor() const;

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

  void        SetStateEffect(StateEffect effect);
  StateEffect GetStateEffect() const;

  void            SetTextUnderline(const Text::Underline& underline);
  Text::Underline GetTextUnderline() const;

protected:
  ~TextButtonStyleImpl() override;

private:
  float              mMinimumWidth{0.0f};
  float              mMinimumHeight{0.0f};
  float              mMaximumWidth{0.0f};
  float              mMaximumHeight{0.0f};
  Vector4            mCornerRadius;
  CornerRadiusPolicy mCornerRadiusPolicy{CornerRadiusPolicy::ABSOLUTE};
  Extents            mPadding;
  UiColor            mBackgroundColor;
  LayoutAlignment    mHorizontalAlignment{LayoutAlignment::CENTER};
  LayoutAlignment    mVerticalAlignment{LayoutAlignment::CENTER};
  UiColor            mTextColor;
  float              mFontSize{0.0f};
  Dali::String       mFontFamily;
  StateEffect        mStateEffect;
  Text::Underline    mUnderline{Text::Underline::None()};
};

} // namespace Internal

inline Internal::TextButtonStyleImpl& GetImpl(Ui::TextButtonStyle& style)
{
  BaseObject& handle = style.GetBaseObject();
  return static_cast<Internal::TextButtonStyleImpl&>(handle);
}

inline const Internal::TextButtonStyleImpl& GetImpl(const Ui::TextButtonStyle& style)
{
  const BaseObject& handle = style.GetBaseObject();
  return static_cast<const Internal::TextButtonStyleImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
