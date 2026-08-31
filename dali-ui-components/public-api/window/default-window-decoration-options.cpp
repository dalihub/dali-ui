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

#include <dali-ui-components/public-api/window/default-window-decoration.h>

#include <dali/public-api/common/dali-common.h>

#include <algorithm>

#define DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from DefaultWindowDecorationOptions object")

namespace Dali::Ui
{
namespace
{
constexpr float DEFAULT_BAR_HEIGHT     = 50.0f;
constexpr float DEFAULT_EDGE_THICKNESS = 5.0f;
constexpr float DEFAULT_CORNER_RADIUS  = 12.0f;
} // unnamed namespace

struct DefaultWindowDecorationOptions::Impl
{
  Dali::Ui::UiColor            backgroundColor;
  DefaultWindowDecorationTheme theme{DefaultWindowDecorationTheme::DARK};
  float                        barHeight{DEFAULT_BAR_HEIGHT};
  float                        edgeThickness{DEFAULT_EDGE_THICKNESS};
  float                        topAreaHeight{0.0f};
  float                        cornerRadius{DEFAULT_CORNER_RADIUS};
  bool                         hasBackgroundColor{false};
  bool                         minimizeControlEnabled{true};
  bool                         maximizeRestoreControlEnabled{true};
  bool                         closeControlEnabled{true};
  bool                         resizeHandlesEnabled{true};
  bool                         shadowEnabled{true};
};

DefaultWindowDecorationOptions::DefaultWindowDecorationOptions()
: mImpl(Dali::MakeUnique<Impl>())
{
}

DefaultWindowDecorationOptions::DefaultWindowDecorationOptions(const DefaultWindowDecorationOptions& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

DefaultWindowDecorationOptions::DefaultWindowDecorationOptions(DefaultWindowDecorationOptions&& rhs) noexcept = default;

DefaultWindowDecorationOptions& DefaultWindowDecorationOptions::operator=(const DefaultWindowDecorationOptions& rhs)
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  return *this;
}

DefaultWindowDecorationOptions& DefaultWindowDecorationOptions::operator=(DefaultWindowDecorationOptions&& rhs) noexcept = default;
DefaultWindowDecorationOptions::~DefaultWindowDecorationOptions()                                                        = default;

void DefaultWindowDecorationOptions::SetTheme(DefaultWindowDecorationTheme theme)
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  mImpl->theme = theme;
}

DefaultWindowDecorationTheme DefaultWindowDecorationOptions::GetTheme() const
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  return mImpl->theme;
}

void DefaultWindowDecorationOptions::SetBarHeight(float height)
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  mImpl->barHeight = std::max(0.0f, height);
}

float DefaultWindowDecorationOptions::GetBarHeight() const
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  return mImpl->barHeight;
}

void DefaultWindowDecorationOptions::SetEdgeThickness(float thickness)
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  mImpl->edgeThickness = std::max(0.0f, thickness);
}

float DefaultWindowDecorationOptions::GetEdgeThickness() const
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  return mImpl->edgeThickness;
}

void DefaultWindowDecorationOptions::SetTopAreaHeight(float height)
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  mImpl->topAreaHeight = std::max(0.0f, height);
}

float DefaultWindowDecorationOptions::GetTopAreaHeight() const
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  return mImpl->topAreaHeight;
}

void DefaultWindowDecorationOptions::SetCornerRadius(float radius)
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  mImpl->cornerRadius = std::max(0.0f, radius);
}

float DefaultWindowDecorationOptions::GetCornerRadius() const
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  return mImpl->cornerRadius;
}

void DefaultWindowDecorationOptions::SetBackgroundColor(const Dali::Ui::UiColor& color)
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  mImpl->backgroundColor    = color;
  mImpl->hasBackgroundColor = true;
}

Dali::Ui::UiColor DefaultWindowDecorationOptions::GetBackgroundColor() const
{
  DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);
  if(mImpl->hasBackgroundColor)
  {
    return mImpl->backgroundColor;
  }
  return mImpl->theme == DefaultWindowDecorationTheme::LIGHT ? Dali::Ui::UiColor(0xF2F2F2, 0.85f)
                                                             : Dali::Ui::UiColor(0xB3B3B3, 0.6f);
}

#define DALI_DEFINE_DEFAULT_WINDOW_DECORATION_FLAG(Name, member)        \
  void DefaultWindowDecorationOptions::Set##Name##Enabled(bool enabled) \
  {                                                                     \
    DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);         \
    mImpl->member = enabled;                                            \
  }                                                                     \
  bool DefaultWindowDecorationOptions::Is##Name##Enabled() const        \
  {                                                                     \
    DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS(mImpl);         \
    return mImpl->member;                                               \
  }

DALI_DEFINE_DEFAULT_WINDOW_DECORATION_FLAG(MinimizeControl, minimizeControlEnabled)
DALI_DEFINE_DEFAULT_WINDOW_DECORATION_FLAG(MaximizeRestoreControl, maximizeRestoreControlEnabled)
DALI_DEFINE_DEFAULT_WINDOW_DECORATION_FLAG(CloseControl, closeControlEnabled)
DALI_DEFINE_DEFAULT_WINDOW_DECORATION_FLAG(Shadow, shadowEnabled)
DALI_DEFINE_DEFAULT_WINDOW_DECORATION_FLAG(ResizeHandles, resizeHandlesEnabled)

#undef DALI_DEFINE_DEFAULT_WINDOW_DECORATION_FLAG

} // namespace Dali::Ui

#undef DALI_ASSERT_VALID_DEFAULT_WINDOW_DECORATION_OPTIONS
