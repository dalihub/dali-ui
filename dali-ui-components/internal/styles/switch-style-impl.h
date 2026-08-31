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
#include <dali-ui-components/public-api/styles/switch-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class SwitchStyleImpl : public Extension::UiStyleImpl
{
public:
  SwitchStyleImpl();
  SwitchStyleImpl(const SwitchStyleImpl& rhs);

  void   SetMinimumWidth(float width);
  float  GetMinimumWidth() const;
  void   SetMinimumHeight(float height);
  float  GetMinimumHeight() const;
  void   SetPadding(const Insets& padding);
  Insets GetPadding() const;

  void   SetTrackWidth(float width);
  float  GetTrackWidth() const;
  void   SetTrackHeight(float height);
  float  GetTrackHeight() const;
  void   SetThumbPadding(const Insets& padding);
  Insets GetThumbPadding() const;

  void    SetTrackColor(const UiColor& color);
  UiColor GetTrackColor() const;
  void    SetSelectedTrackColor(const UiColor& color);
  UiColor GetSelectedTrackColor() const;
  void    SetThumbColor(const UiColor& color);
  UiColor GetThumbColor() const;

  void          SetOverlayEffect(OverlayEffect effect);
  OverlayEffect GetOverlayEffect() const;

  void  SetDragThreshold(float pixels);
  float GetDragThreshold() const;

protected:
  ~SwitchStyleImpl() override;

private:
  float         mMinimumWidth{0.0f};
  float         mMinimumHeight{0.0f};
  Insets        mPadding;
  float         mTrackWidth{0.0f};
  float         mTrackHeight{0.0f};
  Insets        mThumbPadding;
  UiColor       mTrackColor;
  UiColor       mSelectedTrackColor;
  UiColor       mThumbColor;
  OverlayEffect mOverlayEffect;
  float         mDragThreshold{0.0f};
};

} // namespace Internal

inline Internal::SwitchStyleImpl& GetImpl(Ui::SwitchStyle& style)
{
  BaseObject& handle = style.GetBaseObject();
  return static_cast<Internal::SwitchStyleImpl&>(handle);
}

inline const Internal::SwitchStyleImpl& GetImpl(const Ui::SwitchStyle& style)
{
  const BaseObject& handle = style.GetBaseObject();
  return static_cast<const Internal::SwitchStyleImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
