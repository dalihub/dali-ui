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
#include <dali-ui-foundation/integration-api/view-integ.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
{

namespace
{

Internal::ViewDataImpl& GetViewImplData(ViewImpl& viewImpl)
{
  return Internal::ViewDataImpl::Get(viewImpl);
}

const Internal::ViewDataImpl& GetViewImplData(const ViewImpl& viewImpl)
{
  return Internal::ViewDataImpl::Get(viewImpl);
}

} // namespace

namespace Integration
{
namespace View
{

void SetTrait(ViewImpl& viewImpl, TraitId id, IntrusivePtr<TraitObject> object)
{
  GetViewImplData(viewImpl).SetTrait(id, std::move(object));
}

IntrusivePtr<TraitObject> GetTrait(const ViewImpl& viewImpl, TraitId id)
{
  return GetViewImplData(viewImpl).GetTrait(id);
}

bool RemoveTrait(ViewImpl& viewImpl, TraitId id)
{
  return GetViewImplData(viewImpl).RemoveTrait(id);
}

// State management

void SetState(ViewImpl& viewImpl, ViewState state, bool on, InputEvent cause)
{
  GetViewImplData(viewImpl).SetState(state, on, cause);
}

void SetNamedStateHandler(ViewImpl& viewImpl, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  GetViewImplData(viewImpl).SetNamedStateHandler(id, tracker, callback);
}

bool UnsetStateHandler(ViewImpl& viewImpl, const Dali::String& id)
{
  return GetViewImplData(viewImpl).UnsetStateHandler(id);
}

bool UnsetStateHandlerWhenNotProcessing(ViewImpl& viewImpl, const Dali::String& id)
{
  return GetViewImplData(viewImpl).UnsetStateHandlerWhenNotProcessing(id);
}

// View-handle overloads

void SetState(Ui::View view, ViewState state, bool on, InputEvent cause)
{
  GetViewImplData(GetImpl(view)).SetState(state, on, cause);
}

void SetNamedStateHandler(Ui::View view, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  GetViewImplData(GetImpl(view)).SetNamedStateHandler(id, tracker, callback);
}

bool UnsetStateHandler(Ui::View view, const Dali::String& id)
{
  return GetViewImplData(GetImpl(view)).UnsetStateHandler(id);
}

bool UnsetStateHandlerWhenNotProcessing(Ui::View view, const Dali::String& id)
{
  return GetViewImplData(GetImpl(view)).UnsetStateHandlerWhenNotProcessing(id);
}

// Visual property helpers

Dali::Property GetVisualProperty(Ui::View view, Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  return GetViewImplData(GetImpl(view)).GetVisualProperty(index, visualPropertyKey);
}

// Layout helpers

bool IsLayoutModeStandalone(const ViewImpl& viewImpl)
{
  return viewImpl.GetLayoutMode() == Ui::LayoutMode::STANDALONE;
}

ChildContainer& GetChildren(ViewImpl& viewImpl)
{
  return GetViewImplData(viewImpl).mChildren;
}

const ChildContainer& GetChildren(const ViewImpl& viewImpl)
{
  return GetViewImplData(viewImpl).mChildren;
}

bool IsLayout(ViewImpl& viewImpl)
{
  return !!Ui::Layout::DownCast(viewImpl.Self());
}

bool HasLayoutCapability(ViewImpl& viewImpl)
{
  return IsLayout(viewImpl) || viewImpl.HasLayoutManager() || viewImpl.HasLayoutCallback();
}

} // namespace View
} // namespace Integration
} // namespace Ui
} // namespace Dali
