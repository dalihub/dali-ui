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
#include <dali-ui-foundation/integration-api/layouts/stack-layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/common/unique-ptr.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{

BaseHandle Create()
{
  return StackLayout::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::StackLayout, Ui::Integration::StackLayoutImpl, Ui::Layout, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

StackLayoutImplPtr StackLayoutImpl::New(StackOrientation orientation)
{
  return StackLayoutImplPtr(new StackLayoutImpl(orientation));
}

StackLayoutImpl::StackLayoutImpl(StackOrientation orientation)
: LayoutImpl(),
  mInitOrientation(orientation)
{
}

StackLayoutImpl::~StackLayoutImpl()
{
}

void StackLayoutImpl::OnInitialize()
{
  LayoutImpl::OnInitialize();
  AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(mInitOrientation, 0.0f));
}

void StackLayoutImpl::SetOrientation(StackOrientation orientation)
{
  auto* manager = static_cast<StackLayoutManager*>(GetLayoutManager());
  if(manager->GetOrientation() != orientation)
  {
    manager->SetOrientation(orientation);
    InvalidateMeasure();
  }
}

StackOrientation StackLayoutImpl::GetOrientation() const
{
  return static_cast<StackLayoutManager*>(GetLayoutManager())->GetOrientation();
}

void StackLayoutImpl::SetSpacing(float spacing)
{
  auto* manager = static_cast<StackLayoutManager*>(GetLayoutManager());
  if(manager->GetSpacing() != spacing)
  {
    manager->SetSpacing(spacing);
    InvalidateMeasure();
  }
}

float StackLayoutImpl::GetSpacing() const
{
  return static_cast<StackLayoutManager*>(GetLayoutManager())->GetSpacing();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
