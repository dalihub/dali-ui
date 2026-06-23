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
#include <dali-ui-foundation/public-api/group-selectable-trait.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/group-selectable-trait-impl.h>

namespace Dali
{

namespace Ui
{

GroupSelectableTrait::GroupSelectableTrait()
{
}

GroupSelectableTrait::GroupSelectableTrait(const GroupSelectableTrait& trait)
: SelectableTrait(trait)
{
}

GroupSelectableTrait::~GroupSelectableTrait()
{
}

GroupSelectableTrait GroupSelectableTrait::New()
{
  IntrusivePtr<Internal::CoreInteractionObject> traitObject = new Internal::CoreInteractionObject();
  return New(traitObject.Get());
}

GroupSelectableTrait GroupSelectableTrait::New(Internal::CoreInteractionObject* container)
{
  DALI_ASSERT_ALWAYS(container && "GroupSelectableTrait::New requires CoreInteractionObject");
  container->EnsureGroupSelectableTraitImpl();
  return GroupSelectableTrait(container);
}

GroupSelectableTrait::GroupSelectableTrait(Internal::CoreInteractionObject* container)
: SelectableTrait(container)
{
}

GroupSelectableTrait GroupSelectableTrait::DownCast(BaseHandle handle)
{
  if(auto* traitObject = dynamic_cast<Internal::CoreInteractionObject*>(handle.GetObjectPtr()))
  {
    return traitObject->GetGroupSelectableTraitImpl() ? GroupSelectableTrait(traitObject) : GroupSelectableTrait();
  }

  return GroupSelectableTrait();
}

Internal::GroupSelectableTraitImpl& GetImpl(GroupSelectableTrait& obj)
{
  BaseObject& baseObject  = obj.GetBaseObject();
  auto&       traitObject = static_cast<Internal::CoreInteractionObject&>(baseObject);
  auto*       traitImpl   = traitObject.GetGroupSelectableTraitImpl();
  DALI_ASSERT_ALWAYS(traitImpl && "GroupSelectableTrait handle does not contain a GroupSelectableTraitImpl");
  return *traitImpl;
}

const Internal::GroupSelectableTraitImpl& GetImpl(const GroupSelectableTrait& obj)
{
  const BaseObject& baseObject  = obj.GetBaseObject();
  auto&             traitObject = static_cast<const Internal::CoreInteractionObject&>(baseObject);
  auto*             traitImpl   = traitObject.GetGroupSelectableTraitImpl();
  DALI_ASSERT_ALWAYS(traitImpl && "GroupSelectableTrait handle does not contain a GroupSelectableTraitImpl");
  return *traitImpl;
}

void GroupSelectableTrait::SetGroupName(const Dali::String& name)
{
  GetImpl(*this).SetGroupName(name.CStr());
}

Dali::String GroupSelectableTrait::GetGroupName() const
{
  return GetImpl(*this).GetGroupName().c_str();
}

SelectionGroup GroupSelectableTrait::GetGroup() const
{
  return GetImpl(*this).GetGroup();
}

} // namespace Ui

} // namespace Dali
