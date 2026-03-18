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
#include <dali-ui-foundation/integration-api/layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layout-manager.h>

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
  return Layout::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(Ui::Integration::LayoutImpl, Ui::Integration::ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

LayoutImplPtr LayoutImpl::New()
{
  return LayoutImplPtr(new LayoutImpl());
}

LayoutImpl::LayoutImpl()
: ViewImpl()
{
}

LayoutImpl::~LayoutImpl()
{
}

void LayoutImpl::OnInitialize()
{
  // Call base class initialization
  ViewImpl::OnInitialize();

  // Ensure the layout manager is created
  EnsureLayoutManager();
}

// =============================================================================
// Layout Manager Factory
// =============================================================================

LayoutManager* LayoutImpl::CreateLayoutManager()
{
  // Base Layout class returns nullptr - derived classes should override
  // to provide their specific layout algorithm
  return nullptr;
}

void LayoutImpl::EnsureLayoutManager()
{
  if(!HasLayoutManager())
  {
    LayoutManager* manager = CreateLayoutManager();
    if(manager)
    {
      SetLayoutManager(manager);
    }
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
