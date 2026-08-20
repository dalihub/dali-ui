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

#include <dali-ui-components/public-api/window/window-frame-geometry.h>

#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_WINDOW_FRAME_VALUE(impl, Type) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from " Type " object")

namespace Dali::Ui
{

struct WindowFrameGeometry::Impl
{
  Dali::Ui::LayoutRect surfaceBounds;
  Dali::Ui::LayoutRect frameBounds;
  Dali::Ui::LayoutRect contentBounds;
  Dali::Ui::Insets     decorationOverlayInsets;
};

WindowFrameGeometry::WindowFrameGeometry()
: mImpl(Dali::MakeUnique<Impl>())
{
}

WindowFrameGeometry::WindowFrameGeometry(const WindowFrameGeometry& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_VALUE(rhs.mImpl, "WindowFrameGeometry");
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

WindowFrameGeometry::WindowFrameGeometry(WindowFrameGeometry&& rhs) noexcept = default;

WindowFrameGeometry& WindowFrameGeometry::operator=(const WindowFrameGeometry& rhs)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_VALUE(rhs.mImpl, "WindowFrameGeometry");
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  return *this;
}

WindowFrameGeometry& WindowFrameGeometry::operator=(WindowFrameGeometry&& rhs) noexcept = default;
WindowFrameGeometry::~WindowFrameGeometry()                                             = default;

WindowFrameGeometry::WindowFrameGeometry(const Dali::Ui::LayoutRect& surfaceBounds,
                                         const Dali::Ui::LayoutRect& frameBounds,
                                         const Dali::Ui::LayoutRect& contentBounds,
                                         const Dali::Ui::Insets&     decorationOverlayInsets)
: mImpl(Dali::MakeUnique<Impl>(Impl{surfaceBounds, frameBounds, contentBounds, decorationOverlayInsets}))
{
}

Dali::Ui::LayoutRect WindowFrameGeometry::GetSurfaceBounds() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_VALUE(mImpl, "WindowFrameGeometry");
  return mImpl->surfaceBounds;
}

Dali::Ui::LayoutRect WindowFrameGeometry::GetFrameBounds() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_VALUE(mImpl, "WindowFrameGeometry");
  return mImpl->frameBounds;
}

Dali::Ui::LayoutRect WindowFrameGeometry::GetContentBounds() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_VALUE(mImpl, "WindowFrameGeometry");
  return mImpl->contentBounds;
}

Dali::Ui::Insets WindowFrameGeometry::GetDecorationOverlayInsets() const
{
  DALI_ASSERT_VALID_WINDOW_FRAME_VALUE(mImpl, "WindowFrameGeometry");
  return mImpl->decorationOverlayInsets;
}

} // namespace Dali::Ui

#undef DALI_ASSERT_VALID_WINDOW_FRAME_VALUE
