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
#include <dali-ui-foundation/public-api/views/view-registration.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/string-utils.h>
#include <string_view>

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace
{
// The type registry keys types by unqualified class names, while DALI_TYPE_ID
// stringifies its call-site spelling, which may carry namespace qualifiers.
Dali::String UnqualifiedName(Dali::TypeInfoId id)
{
  std::string_view name(id.GetName());
  const size_t     lastSeparator = name.rfind("::");
  if(lastSeparator != std::string_view::npos)
  {
    name.remove_prefix(lastSeparator + 2);
  }
  return Dali::Integration::ToDaliString(name);
}
} // namespace

ViewRegistration::ViewRegistration(Dali::TypeInfoId viewImplType,
                                   Dali::TypeInfoId baseViewImplType,
                                   CreateFunction   createFunction)
{
  Dali::TypeRegistration registration(UnqualifiedName(viewImplType), UnqualifiedName(baseViewImplType), createFunction);
  mName = registration.RegisteredName();
}

ViewRegistration::ViewRegistration(Dali::TypeInfoId viewType,
                                   Dali::TypeInfoId viewImplType,
                                   Dali::TypeInfoId baseViewImplType,
                                   CreateFunction   createFunction)
{
  Dali::TypeRegistration registration(UnqualifiedName(viewType), UnqualifiedName(viewImplType), UnqualifiedName(baseViewImplType), createFunction);
  mName = registration.RegisteredName();
}

const Dali::String& ViewRegistration::RegisteredName() const
{
  return mName;
}

} // namespace Ui

} //namespace DALI_NAMESPACE
