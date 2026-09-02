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

#include <dali-ui-components/public-api/window/window-frame-decoration.h>

#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_WINDOW_FRAME_DECORATION(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from WindowFrameDecoration object")
#define DALI_ASSERT_VALID_WINDOW_FRAME_LAYOUT(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from WindowFrameLayout object")

namespace Dali::Ui
{

struct WindowFrameDecoration::Impl
{
  Dali::Ui::View top;
  Dali::Ui::View bottom;
  Dali::Ui::View left;
  Dali::Ui::View right;
  Dali::Ui::View moveRegion;
  Dali::Ui::View minimizeControl;
  Dali::Ui::View maximizeRestoreControl;
  Dali::Ui::View closeControl;
  Dali::Ui::View bottomLeftResizeHandle;
  Dali::Ui::View bottomRightResizeHandle;
};

WindowFrameDecoration::WindowFrameDecoration()
: mImpl(Dali::MakeUnique<Impl>())
{
}

WindowFrameDecoration::WindowFrameDecoration(const WindowFrameDecoration& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_DECORATION(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

WindowFrameDecoration::WindowFrameDecoration(WindowFrameDecoration&& rhs) noexcept = default;

WindowFrameDecoration& WindowFrameDecoration::operator=(const WindowFrameDecoration& rhs)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_DECORATION(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  return *this;
}

WindowFrameDecoration& WindowFrameDecoration::operator=(WindowFrameDecoration&& rhs) noexcept = default;
WindowFrameDecoration::~WindowFrameDecoration()                                               = default;

#define DALI_DEFINE_DECORATION_ACCESSORS(Name, member)              \
  void WindowFrameDecoration::Set##Name(const Dali::Ui::View& view) \
  {                                                                 \
    DALI_ASSERT_VALID_WINDOW_FRAME_DECORATION(mImpl);               \
    mImpl->member = view;                                           \
  }                                                                 \
  Dali::Ui::View WindowFrameDecoration::Get##Name() const           \
  {                                                                 \
    DALI_ASSERT_VALID_WINDOW_FRAME_DECORATION(mImpl);               \
    return mImpl->member;                                           \
  }

DALI_DEFINE_DECORATION_ACCESSORS(TopSlot, top)
DALI_DEFINE_DECORATION_ACCESSORS(BottomSlot, bottom)
DALI_DEFINE_DECORATION_ACCESSORS(LeftSlot, left)
DALI_DEFINE_DECORATION_ACCESSORS(RightSlot, right)
DALI_DEFINE_DECORATION_ACCESSORS(MoveRegion, moveRegion)
DALI_DEFINE_DECORATION_ACCESSORS(MinimizeControl, minimizeControl)
DALI_DEFINE_DECORATION_ACCESSORS(MaximizeRestoreControl, maximizeRestoreControl)
DALI_DEFINE_DECORATION_ACCESSORS(CloseControl, closeControl)
DALI_DEFINE_DECORATION_ACCESSORS(BottomLeftResizeHandle, bottomLeftResizeHandle)
DALI_DEFINE_DECORATION_ACCESSORS(BottomRightResizeHandle, bottomRightResizeHandle)

#undef DALI_DEFINE_DECORATION_ACCESSORS

struct WindowFrameLayout::Impl
{
  Dali::Ui::Insets decorationInsets{8.0f, 8.0f, 56.0f, 30.0f};
  Dali::Vector2    bottomLeftResizeHandleSize{28.0f, 28.0f};
  Dali::Vector2    bottomRightResizeHandleSize{28.0f, 28.0f};
};

WindowFrameLayout::WindowFrameLayout()
: mImpl(Dali::MakeUnique<Impl>())
{
}

WindowFrameLayout::WindowFrameLayout(const WindowFrameLayout& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_LAYOUT(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

WindowFrameLayout::WindowFrameLayout(WindowFrameLayout&& rhs) noexcept = default;

WindowFrameLayout& WindowFrameLayout::operator=(const WindowFrameLayout& rhs)
{
  DALI_ASSERT_VALID_WINDOW_FRAME_LAYOUT(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  return *this;
}

WindowFrameLayout& WindowFrameLayout::operator=(WindowFrameLayout&& rhs) noexcept = default;
WindowFrameLayout::~WindowFrameLayout()                                           = default;

#define DALI_DEFINE_LAYOUT_ACCESSORS(ValueType, Name, member) \
  void WindowFrameLayout::Set##Name(const ValueType& value)   \
  {                                                           \
    DALI_ASSERT_VALID_WINDOW_FRAME_LAYOUT(mImpl);             \
    mImpl->member = value;                                    \
  }                                                           \
  ValueType WindowFrameLayout::Get##Name() const              \
  {                                                           \
    DALI_ASSERT_VALID_WINDOW_FRAME_LAYOUT(mImpl);             \
    return mImpl->member;                                     \
  }

DALI_DEFINE_LAYOUT_ACCESSORS(Dali::Ui::Insets, DecorationInsets, decorationInsets)
DALI_DEFINE_LAYOUT_ACCESSORS(Dali::Vector2, BottomLeftResizeHandleSize, bottomLeftResizeHandleSize)
DALI_DEFINE_LAYOUT_ACCESSORS(Dali::Vector2, BottomRightResizeHandleSize, bottomRightResizeHandleSize)

#undef DALI_DEFINE_LAYOUT_ACCESSORS

} // namespace Dali::Ui

#undef DALI_ASSERT_VALID_WINDOW_FRAME_LAYOUT
#undef DALI_ASSERT_VALID_WINDOW_FRAME_DECORATION
