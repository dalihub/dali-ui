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
#include <dali-ui-foundation/public-api/layouts/layout-params.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-params-impl.h>

namespace Dali
{
namespace Ui
{

LayoutParams::LayoutParams()
{
}

LayoutParams::LayoutParams(const LayoutParams& handle)
: Trait(handle)
{
}

LayoutParams::~LayoutParams()
{
}

LayoutParams::LayoutParams(Internal::LayoutParamsImpl* implementation)
: Trait(implementation)
{
}

LayoutParams LayoutParams::DownCast(BaseHandle handle)
{
  return LayoutParams(dynamic_cast<Internal::LayoutParamsImpl*>(handle.GetObjectPtr()));
}

} // namespace Ui
} // namespace Dali
