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
#include <dali-ui-foundation/integration-api/view-accessibility.h>
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

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
namespace ViewAccessibility
{

void SetAccessibleObjectCreator(ViewImpl& viewImpl, AccessibleObjectCreator creator)
{
  GetViewImplData(viewImpl).SetAccessibleObjectCreator(creator);
}

void Register()
{
  static bool onceFlag = false;
  if(DALI_UNLIKELY(!onceFlag))
  {
    onceFlag = true;
    Dali::Accessibility::Accessible::RegisterExternalAccessibleGetter(
      [](Dali::Actor actor) -> std::pair<SharedPtr<Dali::Accessibility::Accessible>, bool>
    {
      auto view = Ui::View::DownCast(actor);
      if(!view)
      {
        return {SharedPtr<ViewAccessible>(), true};
      }

      auto& viewImpl = GetImpl(view);
      auto& viewData = GetViewImplData(viewImpl);
      if(viewData.IsCreateAccessibleEnabled())
      {
        return {SharedPtr<ViewAccessible>(viewData.CreateAccessibleObject()), true};
      }
      return {SharedPtr<ViewAccessible>(), false};
    });
  }
}

} // namespace ViewAccessibility

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
  return GetViewImplData(viewImpl).GetChildren();
}

const ChildContainer& GetChildren(const ViewImpl& viewImpl)
{
  return GetViewImplData(viewImpl).GetChildren();
}

bool IsLayout(ViewImpl& viewImpl)
{
  return !!Ui::Layout::DownCast(viewImpl.Self());
}

bool HasLayoutCapability(ViewImpl& viewImpl)
{
  const auto& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);
  return IsLayout(viewImpl) || viewDataImpl.HasLayoutManager() || viewDataImpl.HasLayoutCallback();
}

} // namespace View
} // namespace Integration
} // namespace Ui
} // namespace Dali
