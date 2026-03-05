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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali-ui-foundation/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-ui-elements/internal/dummy-element-impl.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

UI::DummyElement DummyElementImpl::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Internal::DummyElementImpl> impl = new Internal::DummyElementImpl();

  // Pass ownership to CustomActor handle
  UI::DummyElement handle = UI::DummyElement(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

DummyElementImpl::DummyElementImpl()
  : ViewImpl()
{
}

DummyElementImpl::~DummyElementImpl()
{
}

} // namespace Internal

} // namespace UI

} // namespace Dali
