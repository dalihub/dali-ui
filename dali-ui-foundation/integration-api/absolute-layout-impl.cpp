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
#include <dali-ui-foundation/integration-api/absolute-layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

Ui::AbsoluteLayout AbsoluteLayoutImpl::New()
{
  IntrusivePtr<Integration::AbsoluteLayoutImpl> impl = new Integration::AbsoluteLayoutImpl();
  Ui::AbsoluteLayout handle = Ui::AbsoluteLayout(*impl);
  impl->Initialize();
  return handle;
}

AbsoluteLayoutImpl::AbsoluteLayoutImpl()
  : LayoutImpl()
{
}

AbsoluteLayoutImpl::~AbsoluteLayoutImpl()
{
}

LayoutManager* AbsoluteLayoutImpl::CreateLayoutManager()
{
  return new AbsoluteLayoutManager();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
