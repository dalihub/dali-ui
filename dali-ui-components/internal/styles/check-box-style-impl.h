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
#include <dali-ui-components/public-api/styles/check-box-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/views/image/i-selectable-image.h>

// EXTERNAL INCLUDES
#include <memory>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class CheckBoxStyleImpl : public Extension::UiStyleImpl
{
public:
  CheckBoxStyleImpl();
  CheckBoxStyleImpl(const CheckBoxStyleImpl& rhs);

  void    SetMinimumWidth(float width);
  float   GetMinimumWidth() const;
  void    SetMinimumHeight(float height);
  float   GetMinimumHeight() const;
  void    SetMinimumSize(const Vector2& size);
  void    SetPadding(const Extents& padding);
  Extents GetPadding() const;

  void  SetBoxSize(float size);
  float GetBoxSize() const;
  void  SetLabelGap(float gap);
  float GetLabelGap() const;

  void SetIconGenerator(CheckBoxStyle::IconGenerator&& generator)
  {
    mIconGenerator = std::make_shared<CheckBoxStyle::IconGenerator>(std::move(generator));
  }
  ISelectableImage CreateIcon() const
  {
    return mIconGenerator ? mIconGenerator->Invoke() : ISelectableImage();
  }

  void    SetIconColor(const UiColor& color);
  UiColor GetIconColor() const;
  void    SetSelectedIconColor(const UiColor& color);
  UiColor GetSelectedIconColor() const;
  void    SetLabelColor(const UiColor& color);
  UiColor GetLabelColor() const;

  void        SetStateEffect(StateEffect effect);
  StateEffect GetStateEffect() const;

protected:
  ~CheckBoxStyleImpl() override;

private:
  float                                         mMinimumWidth{0.0f};
  float                                         mMinimumHeight{0.0f};
  Extents                                       mPadding;
  float                                         mBoxSize{0.0f};
  float                                         mLabelGap{0.0f};
  std::shared_ptr<CheckBoxStyle::IconGenerator> mIconGenerator;
  UiColor                                       mIconColor;
  UiColor                                       mSelectedIconColor;
  UiColor                                       mLabelColor;
  StateEffect                                   mStateEffect;
};

} // namespace Internal

inline Internal::CheckBoxStyleImpl& GetImpl(Ui::CheckBoxStyle& style)
{
  BaseObject& handle = style.GetBaseObject();
  return static_cast<Internal::CheckBoxStyleImpl&>(handle);
}

inline const Internal::CheckBoxStyleImpl& GetImpl(const Ui::CheckBoxStyle& style)
{
  const BaseObject& handle = style.GetBaseObject();
  return static_cast<const Internal::CheckBoxStyleImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
