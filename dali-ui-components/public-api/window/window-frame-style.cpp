/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-ui-components/public-api/window/window-frame-style.h>

#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from WindowFrameStyle object")

namespace Dali::Ui
{

struct WindowFrameStyle::Impl
{
  WindowFrameShadowSource      shadowSource{WindowFrameShadowSource::NONE};
  Dali::Ui::Insets             shadowOutsets;
  Dali::Ui::Shadow             shadow;
  Dali::String                 shadowImageUrl;
  Dali::Ui::Insets             shadowImageBorder;
  Dali::Ui::UiColor            frameBackgroundColor{0x000000, 0.0f};
  Dali::Vector4                frameCornerRadius;
  Dali::Ui::CornerRadiusPolicy frameCornerRadiusPolicy{Dali::Ui::CornerRadiusPolicy::ABSOLUTE};
  bool                         shadowImageBorderOnly{false};
};

WindowFrameStyle::WindowFrameStyle()
: mImpl(Dali::MakeUnique<Impl>())
{
}

WindowFrameStyle::WindowFrameStyle(const WindowFrameStyle& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

WindowFrameStyle::WindowFrameStyle(WindowFrameStyle&& rhs) noexcept = default;

WindowFrameStyle& WindowFrameStyle::operator=(const WindowFrameStyle& rhs)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  return *this;
}

WindowFrameStyle& WindowFrameStyle::operator=(WindowFrameStyle&& rhs) noexcept = default;
WindowFrameStyle::~WindowFrameStyle()                                          = default;

#define DALI_DEFINE_FRAME_STYLE_VALUE_ACCESSORS(ValueType, Name, member) \
  void WindowFrameStyle::Set##Name(ValueType value)                      \
  {                                                                      \
    DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);                         \
    mImpl->member = value;                                               \
  }                                                                      \
  ValueType WindowFrameStyle::Get##Name() const                          \
  {                                                                      \
    DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);                         \
    return mImpl->member;                                                \
  }

#define DALI_DEFINE_FRAME_STYLE_REFERENCE_ACCESSORS(ValueType, Name, member) \
  void WindowFrameStyle::Set##Name(const ValueType& value)                   \
  {                                                                          \
    DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);                             \
    mImpl->member = value;                                                   \
  }                                                                          \
  ValueType WindowFrameStyle::Get##Name() const                              \
  {                                                                          \
    DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);                             \
    return mImpl->member;                                                    \
  }

WindowFrameShadowSource WindowFrameStyle::GetShadowSource() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);
  return mImpl->shadowSource;
}

void WindowFrameStyle::ClearShadow()
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);
  mImpl->shadowSource = WindowFrameShadowSource::NONE;
}

DALI_DEFINE_FRAME_STYLE_REFERENCE_ACCESSORS(Dali::Ui::Insets, ShadowOutsets, shadowOutsets)

void WindowFrameStyle::SetShadow(const Dali::Ui::Shadow& shadow)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);
  mImpl->shadow       = shadow;
  mImpl->shadowSource = WindowFrameShadowSource::COLOR;
}

Dali::Ui::Shadow WindowFrameStyle::GetShadow() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);
  return mImpl->shadow;
}

void WindowFrameStyle::SetShadowImageUrl(Dali::StringView imageUrl)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);
  mImpl->shadowImageUrl = Dali::String(imageUrl);
  mImpl->shadowSource   = mImpl->shadowImageUrl.Empty() ? WindowFrameShadowSource::NONE : WindowFrameShadowSource::IMAGE;
}

Dali::String WindowFrameStyle::GetShadowImageUrl() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);
  return mImpl->shadowImageUrl;
}

DALI_DEFINE_FRAME_STYLE_REFERENCE_ACCESSORS(Dali::Ui::Insets, ShadowImageBorder, shadowImageBorder)

void WindowFrameStyle::SetShadowImageBorderOnly(bool borderOnly)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);
  mImpl->shadowImageBorderOnly = borderOnly;
}

bool WindowFrameStyle::IsShadowImageBorderOnly() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_STYLE(mImpl);
  return mImpl->shadowImageBorderOnly;
}

DALI_DEFINE_FRAME_STYLE_REFERENCE_ACCESSORS(Dali::Ui::UiColor, FrameBackgroundColor, frameBackgroundColor)
DALI_DEFINE_FRAME_STYLE_REFERENCE_ACCESSORS(Dali::Vector4, FrameCornerRadius, frameCornerRadius)
DALI_DEFINE_FRAME_STYLE_VALUE_ACCESSORS(Dali::Ui::CornerRadiusPolicy, FrameCornerRadiusPolicy, frameCornerRadiusPolicy)

} // namespace Dali::Ui

#undef DALI_DEFINE_FRAME_STYLE_REFERENCE_ACCESSORS
#undef DALI_DEFINE_FRAME_STYLE_VALUE_ACCESSORS
#undef DALI_ASSERT_VALID_WINDOW_FRAME_STYLE
