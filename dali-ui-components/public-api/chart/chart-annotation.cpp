
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

// CLASS HEADER
#include <dali-ui-components/public-api/chart/chart-annotation.h>

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-annotation-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
ChartAnnotation::ChartAnnotation()                                      = default;
ChartAnnotation::~ChartAnnotation()                                     = default;
ChartAnnotation::ChartAnnotation(const ChartAnnotation&)                = default;
ChartAnnotation::ChartAnnotation(ChartAnnotation&&) noexcept            = default;
ChartAnnotation& ChartAnnotation::operator=(const ChartAnnotation&)     = default;
ChartAnnotation& ChartAnnotation::operator=(ChartAnnotation&&) noexcept = default;

ChartAnnotation ChartAnnotation::New(Type type)
{
  return ChartAnnotation(new Integration::ChartAnnotation(type));
}

ChartAnnotation ChartAnnotation::DownCast(BaseHandle handle)
{
  return ChartAnnotation(dynamic_cast<Integration::ChartAnnotation*>(handle.GetObjectPtr()));
}

void ChartAnnotation::SetPosition(const Vector2& position)
{
  auto& impl = GetImplementation(*this);
  DALI_ASSERT_ALWAYS(std::isfinite(position.x) && std::isfinite(position.y));
  impl.mPosition = position;
  impl.mSeries.Reset();
  impl.mChanged.Emit();
}

void ChartAnnotation::SetDataPoint(ChartSeries series, uint32_t pointIndex)
{
  auto& impl       = GetImplementation(*this);
  impl.mSeries     = series;
  impl.mPointIndex = pointIndex;
  impl.mChanged.Emit();
}

void ChartAnnotation::SetText(Dali::StringView text)
{
  auto& impl = GetImplementation(*this);
  impl.mText = Dali::String(text);
  impl.mChanged.Emit();
}

void ChartAnnotation::SetImageUrl(Dali::StringView url)
{
  auto& impl     = GetImplementation(*this);
  impl.mImageUrl = Dali::String(url);
  impl.mChanged.Emit();
}

void ChartAnnotation::SetColor(const UiColor& color)
{
  auto& impl  = GetImplementation(*this);
  impl.mColor = color;
  impl.mChanged.Emit();
}

void ChartAnnotation::SetSize(const Vector2& size)
{
  auto& impl = GetImplementation(*this);
  DALI_ASSERT_ALWAYS(std::isfinite(size.x) && std::isfinite(size.y) && size.x > 0.0f && size.y > 0.0f);
  impl.mSize = size;
  impl.mChanged.Emit();
}

void ChartAnnotation::SetOffset(const Vector2& offset)
{
  auto& impl = GetImplementation(*this);
  DALI_ASSERT_ALWAYS(std::isfinite(offset.x) && std::isfinite(offset.y));
  impl.mOffset = offset;
  impl.mChanged.Emit();
}

ChartAnnotation::Type ChartAnnotation::GetType() const
{
  return GetImplementation(*this).mType;
}

Dali::String ChartAnnotation::GetText() const
{
  return GetImplementation(*this).mText;
}

void ChartAnnotation::SetTextMaximumWidth(float width)
{
  DALI_ASSERT_ALWAYS(std::isfinite(width) && width >= 0.0f);
  auto& impl             = GetImplementation(*this);
  impl.mTextMaximumWidth = width;
  impl.mChanged.Emit();
}

float ChartAnnotation::GetTextMaximumWidth() const
{
  return GetImplementation(*this).mTextMaximumWidth;
}

void ChartAnnotation::SetTextOverflowMode(Text::OverflowMode mode)
{
  DALI_ASSERT_ALWAYS(mode == Text::OverflowMode::CLIP || mode == Text::OverflowMode::ELLIPSIS);
  auto& impl             = GetImplementation(*this);
  impl.mTextOverflowMode = mode;
  impl.mChanged.Emit();
}

Text::OverflowMode ChartAnnotation::GetTextOverflowMode() const
{
  return GetImplementation(*this).mTextOverflowMode;
}

ChartAnnotation::ChartAnnotation(Integration::ChartAnnotation* impl)
: BaseHandle(impl)
{
}

} // namespace Ui
} // namespace DALI_NAMESPACE
