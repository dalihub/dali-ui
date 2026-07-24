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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
namespace View
{
namespace Internal
{
void SetNamedStateObserver(ViewImpl& viewImpl, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  Ui::Internal::ViewDataImpl::Get(viewImpl).SetNamedStateObserver(id, tracker, callback);
}
} // namespace Internal

bool GrabAccessibilityHighlight(Ui::View view)
{
  auto accessible = Dali::Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view)).GetAccessibleObject();
  return DALI_LIKELY(accessible) && accessible->GrabHighlight();
}

bool ClearAccessibilityHighlight(Ui::View view)
{
  auto accessible = Dali::Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view)).GetAccessibleObject();
  return DALI_LIKELY(accessible) && accessible->ClearHighlight();
}

void SetState(ViewImpl& viewImpl, ViewState stateToChange, bool on, InputEvent cause)
{
  Ui::Internal::ViewDataImpl::Get(viewImpl).SetState(stateToChange, on, cause);
}

void SetState(ViewImpl& viewImpl, ViewState statesToClear, ViewState statesToSet, InputEvent cause)
{
  Ui::Internal::ViewDataImpl::Get(viewImpl).SetState(statesToClear, statesToSet, cause);
}

bool UnsetNamedStateObserver(ViewImpl& viewImpl, const Dali::String& id)
{
  return Ui::Internal::ViewDataImpl::Get(viewImpl).UnsetNamedStateObserver(id);
}

bool UnsetNamedStateObserverIfNotExecuting(ViewImpl& viewImpl, const Dali::String& id)
{
  return Ui::Internal::ViewDataImpl::Get(viewImpl).UnsetNamedStateObserverIfNotExecuting(id);
}

void SetPositionX(Ui::View view, float x)
{
  static_cast<Dali::Actor&>(view).SetPositionX(x);
}

void SetPositionY(Ui::View view, float y)
{
  static_cast<Dali::Actor&>(view).SetPositionY(y);
}

void SetSizeWidth(Ui::View view, float width)
{
  static_cast<Dali::Actor&>(view).SetWidth(width);
}

void SetSizeHeight(Ui::View view, float height)
{
  static_cast<Dali::Actor&>(view).SetHeight(height);
}

} // namespace View
} // namespace Extension
} // namespace Ui
} // namespace Dali
