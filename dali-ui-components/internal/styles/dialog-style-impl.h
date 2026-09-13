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

#include <dali-ui-components/public-api/styles/dialog-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class DialogStyleImpl : public Extension::UiStyleImpl
{
public:
  struct Values
  {
    float              mRequestedWidth{WRAP_CONTENT};
    float              mRequestedHeight{WRAP_CONTENT};
    UiColor            mBackgroundColor{UiColor(0x000000u, 0.0f)};
    Insets             mPadding{Insets()};
    float              mSpacing{0.0f};
    Vector4            mCornerRadius{Vector4::ZERO};
    CornerRadiusPolicy mCornerRadiusPolicy{CornerRadiusPolicy::ABSOLUTE};
    ShadowStack        mShadow{ShadowStack()};
    float              mBorderlineWidth{0.0f};
    float              mBorderlineOffset{0.0f};
    UiColor            mBorderlineColor{UiColor(0x000000u, 1.0f)};
    float              mBackgroundBlurRadius{0.0f};
  } values;

  DialogStyleImpl() = default;
  DialogStyleImpl(const DialogStyleImpl& rhs)
  : Extension::UiStyleImpl(),
    values(rhs.values)
  {
  }

protected:
  ~DialogStyleImpl() override = default;
};
} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE
