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
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace DALI_NAMESPACE
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

  void SetMaximumWidthRatio(float value);
  void SetMaximumHeight(float value);
  void SetBottomOffsetRatio(float value);
  void SetItemSpacing(float value);
  void SetCornerRadius(const Vector4& value);
  void SetPadding(const Insets& value);
  void SetBackgroundColor(const UiColor& value)
  {
    mBackgroundColor = value;
  }
  void SetTextColor(const UiColor& value)
  {
    mTextColor = value;
  }
  void SetFontSize(float value);
  void SetFontFamily(const Dali::String& value)
  {
    mFontFamily = value;
  }
  void SetIconWidth(float value);
  void SetIconHeight(float value);
  void SetIconCornerRadius(const Vector4& value);
  void SetIconColor(const UiColor& value)
  {
    mIconColor = value;
  }
  void SetShadow(const ShadowStack& value)
  {
    mShadow = value;
  }
  void SetBorderlineWidth(float value);
  void SetBorderlineOffset(float value);
  void SetBorderlineColor(const UiColor& value)
  {
    mBorderlineColor = value;
  }

  float GetMaximumWidthRatio() const
  {
    return mMaximumWidthRatio;
  }
  float GetMaximumHeight() const
  {
    return mMaximumHeight;
  }
  float GetBottomOffsetRatio() const
  {
    return mBottomOffsetRatio;
  }
  float GetItemSpacing() const
  {
    return mItemSpacing;
  }
  Vector4 GetCornerRadius() const
  {
    return mCornerRadius;
  }
  Insets GetPadding() const
  {
    return mPadding;
  }
  UiColor GetBackgroundColor() const
  {
    return mBackgroundColor;
  }
  UiColor GetTextColor() const
  {
    return mTextColor;
  }
  float GetFontSize() const
  {
    return mFontSize;
  }
  Dali::String GetFontFamily() const
  {
    return mFontFamily;
  }
  float GetIconWidth() const
  {
    return mIconWidth;
  }
  float GetIconHeight() const
  {
    return mIconHeight;
  }
  Vector4 GetIconCornerRadius() const
  {
    return mIconCornerRadius;
  }
  UiColor GetIconColor() const
  {
    return mIconColor;
  }
  ShadowStack GetShadow() const
  {
    return mShadow;
  }
  float GetBorderlineWidth() const
  {
    return mBorderlineWidth;
  }
  float GetBorderlineOffset() const
  {
    return mBorderlineOffset;
  }
  UiColor GetBorderlineColor() const
  {
    return mBorderlineColor;
  }

protected:
  ~ToastStyleImpl() override = default;

private:
  float        mMaximumWidthRatio{0.68f};
  float        mMaximumHeight{112.0f};
  float        mBottomOffsetRatio{0.05f};
  float        mItemSpacing{16.0f};
  Vector4      mCornerRadius{36.0f, 36.0f, 36.0f, 36.0f};
  Insets       mPadding{32.0f, 32.0f, 16.0f, 16.0f};
  UiColor      mBackgroundColor{UiColor("SurfaceFixed")};
  UiColor      mTextColor{UiColor("OnSurfaceContainerFixedVariantBright")};
  float        mFontSize{28.0f};
  Dali::String mFontFamily{"SamsungOneUI400"};
  float        mIconWidth{36.0f};
  float        mIconHeight{36.0f};
  Vector4      mIconCornerRadius{Vector4::ZERO};
  UiColor      mIconColor{Color::WHITE};
  ShadowStack  mShadow;
  float        mBorderlineWidth{2.0f};
  float        mBorderlineOffset{-1.0f};
  UiColor      mBorderlineColor{UiColor("OutlineACC")};
};

inline const ToastStyleImpl& GetImpl(const Ui::ToastStyle& style)
{
  return static_cast<const ToastStyleImpl&>(style.GetBaseObject());
}
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
