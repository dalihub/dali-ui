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
#include <dali-ui-foundation/extension-api/view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
bool GrabAccessibilityHighlight(View view)
{
  auto accessible = Internal::ViewDataImpl::Get(Ui::GetImpl(view)).GetAccessibleObject();
  return DALI_LIKELY(accessible) && accessible->GrabHighlight();
}

bool ClearAccessibilityHighlight(View view)
{
  auto accessible = Internal::ViewDataImpl::Get(Ui::GetImpl(view)).GetAccessibleObject();
  return DALI_LIKELY(accessible) && accessible->ClearHighlight();
}

void SetPositionX(View view, float x)
{
  static_cast<Dali::Actor&>(view).SetPositionX(x);
}

void SetPositionY(View view, float y)
{
  static_cast<Dali::Actor&>(view).SetPositionY(y);
}

void SetSizeWidth(View view, float width)
{
  static_cast<Dali::Actor&>(view).SetWidth(width);
}

void SetSizeHeight(View view, float height)
{
  static_cast<Dali::Actor&>(view).SetHeight(height);
}

} // namespace Extension
} // namespace Ui
} // namespace Dali
