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
    float              mRequestedWidth{908.0f};
    float              mRequestedHeight{WRAP_CONTENT};
    // Fold the common OneUI section insets into our own content padding.
    UiColor            mBackgroundColor{UiColor(0xFFFFFFu, 0.6f)};
    Insets             mPadding{44.0f, 44.0f, 36.0f, 32.0f};
    float              mSpacing{12.0f};
    Vector4            mCornerRadius{44.0f, 44.0f, 44.0f, 44.0f};
    CornerRadiusPolicy mCornerRadiusPolicy{CornerRadiusPolicy::ABSOLUTE};
    ShadowStack        mShadow{
      Shadow(8.0f, Vector2::ZERO, UiColor(0x000000u, 0.05f), Vector2::ZERO, CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS),
      Shadow(32.0f, Vector2(0.0f, 16.0f), UiColor(0x000000u, 0.1f), Vector2::ZERO, CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS)};
    float              mBorderlineWidth{2.0f};
    float              mBorderlineOffset{-1.0f};
    UiColor            mBorderlineColor{UiColor(0xFCFCFFu, 0.05f)};
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
