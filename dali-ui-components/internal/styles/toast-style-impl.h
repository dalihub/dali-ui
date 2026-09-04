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

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/styles/toast-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class ToastStyleImpl : public Extension::UiStyleImpl
{
public:
  ToastStyleImpl();
  ToastStyleImpl(const ToastStyleImpl& rhs);

#define DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(Name) \
  void  Set##Name(float value);                           \
  float Get##Name() const;

  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(InitialWidth)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(InitialHeight)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(MinimumWidth)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(MinimumHeight)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(ExpandedHeight)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(MaximumHeight)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(MaximumWidthRatio)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(MaximumWidth)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(BottomOffsetRatio)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(BottomOffset)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(ItemSpacing)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(ExpandedItemSpacing)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(FontSize)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(BorderlineWidth)
  DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION(BorderlineOffset)

#undef DALI_TOAST_STYLE_FLOAT_ACCESSOR_DECLARATION

  void               SetCornerRadius(const Vector4& radius);
  Vector4            GetCornerRadius() const;
  void               SetCornerRadiusPolicy(CornerRadiusPolicy policy);
  CornerRadiusPolicy GetCornerRadiusPolicy() const;
  void               SetPadding(const Insets& padding);
  Insets             GetPadding() const;
  void               SetExpandedPadding(const Insets& padding);
  Insets             GetExpandedPadding() const;

  void         SetBackgroundColor(const UiColor& color);
  UiColor      GetBackgroundColor() const;
  void         SetTextColor(const UiColor& color);
  UiColor      GetTextColor() const;
  void         SetFontFamily(const Dali::String& fontFamily);
  Dali::String GetFontFamily() const;
  void         SetShadow(const ShadowStack& shadow);
  ShadowStack  GetShadow() const;

  void            SetBorderlineColor(const UiColor& color);
  UiColor         GetBorderlineColor() const;
  void            SetActionButtonStyle(TextButtonStyle style);
  TextButtonStyle GetActionButtonStyle() const;

protected:
  ~ToastStyleImpl() override;

private:
  float mInitialWidth{0.0f};
  float mInitialHeight{0.0f};
  float mMinimumWidth{0.0f};
  float mMinimumHeight{0.0f};
  float mExpandedHeight{0.0f};
  float mMaximumHeight{0.0f};
  float mMaximumWidthRatio{0.0f};
  float mMaximumWidth{0.0f};
  float mBottomOffsetRatio{0.0f};
  float mBottomOffset{0.0f};

  Vector4            mCornerRadius;
  CornerRadiusPolicy mCornerRadiusPolicy{CornerRadiusPolicy::ABSOLUTE};
  Insets             mPadding;
  Insets             mExpandedPadding;
  float              mItemSpacing{0.0f};
  float              mExpandedItemSpacing{0.0f};

  UiColor      mBackgroundColor;
  UiColor      mTextColor;
  float        mFontSize{0.0f};
  Dali::String mFontFamily;
  ShadowStack  mShadow;

  float   mBorderlineWidth{0.0f};
  UiColor mBorderlineColor;
  float   mBorderlineOffset{0.0f};

  TextButtonStyle mActionButtonStyle;
};

} // namespace Internal

inline Internal::ToastStyleImpl& GetImpl(Ui::ToastStyle& style)
{
  BaseObject& handle = style.GetBaseObject();
  return static_cast<Internal::ToastStyleImpl&>(handle);
}

inline const Internal::ToastStyleImpl& GetImpl(const Ui::ToastStyle& style)
{
  const BaseObject& handle = style.GetBaseObject();
  return static_cast<const Internal::ToastStyleImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
