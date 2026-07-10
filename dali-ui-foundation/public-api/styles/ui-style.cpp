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
#include <dali-ui-foundation/public-api/styles/ui-style.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace Dali
{
namespace Ui
{

UiStyle UiStyle::DownCast(BaseHandle handle)
{
  return UiStyle(dynamic_cast<Extension::UiStyleImpl*>(handle.GetObjectPtr()));
}

UiStyle::UiStyle(Extension::UiStyleImpl* impl)
: BaseHandle(impl)
{
}

} // namespace Ui
} // namespace Dali
