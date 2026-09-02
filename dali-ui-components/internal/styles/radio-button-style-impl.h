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
#include <dali-ui-components/public-api/styles/radio-button-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

// EXTERNAL INCLUDES
#include <memory>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class RadioButtonStyleImpl : public Extension::UiStyleImpl
{
public:
  RadioButtonStyleImpl();
  RadioButtonStyleImpl(const RadioButtonStyleImpl& rhs);

  void   SetMinimumWidth(float width);
  float  GetMinimumWidth() const;
  void   SetMinimumHeight(float height);
  float  GetMinimumHeight() const;
  void   SetPadding(const Insets& padding);
  Insets GetPadding() const;

  void  SetIconWidth(float width);
  float GetIconWidth() const;
  void  SetIconHeight(float height);
  float GetIconHeight() const;

  void SetIconGenerator(RadioButtonStyle::IconGenerator&& generator)
  {
    mIconGenerator = std::make_shared<RadioButtonStyle::IconGenerator>(std::move(generator));
  }

  SelectableImageInterface CreateIcon() const
  {
    return (mIconGenerator && *mIconGenerator) ? mIconGenerator->Invoke() : SelectableImageInterface();
  }

  void    SetIconColor(const UiColor& color);
  UiColor GetIconColor() const;
  void    SetSelectedIconColor(const UiColor& color);
  UiColor GetSelectedIconColor() const;

  void        SetStateEffect(StateEffect effect);
  StateEffect GetStateEffect() const;

protected:
  ~RadioButtonStyleImpl() override;

private:
  float                                            mMinimumWidth{0.0f};
  float                                            mMinimumHeight{0.0f};
  Insets                                           mPadding;
  float                                            mIconWidth{0.0f};
  float                                            mIconHeight{0.0f};
  std::shared_ptr<RadioButtonStyle::IconGenerator> mIconGenerator;
  UiColor                                          mDeselectedIconColor;
  UiColor                                          mSelectedIconColor;
  StateEffect                                      mStateEffect;
};

} // namespace Internal

inline Internal::RadioButtonStyleImpl& GetImpl(Ui::RadioButtonStyle& style)
{
  BaseObject& handle = style.GetBaseObject();
  return static_cast<Internal::RadioButtonStyleImpl&>(handle);
}

inline const Internal::RadioButtonStyleImpl& GetImpl(const Ui::RadioButtonStyle& style)
{
  const BaseObject& handle = style.GetBaseObject();
  return static_cast<const Internal::RadioButtonStyleImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
