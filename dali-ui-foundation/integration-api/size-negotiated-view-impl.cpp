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
#include <dali-ui-foundation/integration-api/size-negotiated-view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>

namespace Dali::Ui::Integration
{

SizeNegotiatedViewImpl::~SizeNegotiatedViewImpl() = default;

void SizeNegotiatedViewImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Internal::ViewDataImpl::Get(*this).RelayoutDefault(size, container);
}

void SizeNegotiatedViewImpl::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 SizeNegotiatedViewImpl::GetNaturalSize()
{
  return Internal::ViewDataImpl::Get(*this).GetBackgroundVisualNaturalSize();
}

float SizeNegotiatedViewImpl::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return CalculateChildSizeBase(child, dimension);
}

float SizeNegotiatedViewImpl::GetHeightForWidth(float width)
{
  return GetHeightForWidthBase(width);
}

float SizeNegotiatedViewImpl::GetWidthForHeight(float height)
{
  return GetWidthForHeightBase(height);
}

bool SizeNegotiatedViewImpl::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return RelayoutDependentOnChildrenBase(dimension);
}

void SizeNegotiatedViewImpl::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void SizeNegotiatedViewImpl::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
}

SizeNegotiatedViewImpl::SizeNegotiatedViewImpl() = default;

} // namespace Dali::Ui::Integration
