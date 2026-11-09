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

#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{

class ProgressBarStyleImpl : public Extension::UiStyleImpl
{
public:
  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL ProgressBarStyleImpl* NewForBaseHandle()
  {
    return new ProgressBarStyleImpl();
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<ProgressBarStyleImpl> New()
  {
    return IntrusivePtr<ProgressBarStyleImpl>(new ProgressBarStyleImpl());
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<ProgressBarStyleImpl> New(const ProgressBarStyleImpl& rhs)
  {
    return IntrusivePtr<ProgressBarStyleImpl>(new ProgressBarStyleImpl(rhs));
  }

  void    SetPadding(const Insets& padding);
  Insets  GetPadding() const;
  void    SetTrackThickness(float thickness);
  float   GetTrackThickness() const;
  void    SetTrackColor(const UiColor& color);
  UiColor GetTrackColor() const;
  void    SetTrailColor(const UiColor& color);
  UiColor GetTrailColor() const;
  void    SetDividerColor(const UiColor& color);
  UiColor GetDividerColor() const;

protected:
  ProgressBarStyleImpl();
  ProgressBarStyleImpl(const ProgressBarStyleImpl& rhs);
  ~ProgressBarStyleImpl() override;

private:
  Insets  mPadding;
  float   mTrackThickness{10.0f};
  UiColor mTrackColor;
  UiColor mTrailColor;
  UiColor mDividerColor;
};

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
