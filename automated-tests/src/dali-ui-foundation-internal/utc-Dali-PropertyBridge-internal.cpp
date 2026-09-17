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
 */

#include <dali-ui-foundation/integration-api/property-bridge/property-bridge.h>
#include <dali-ui-foundation/internal/property-bridge/property-bridge-impl.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;

namespace
{
void GetTestString(void* object, const char* propertyName, std::string* result)
{
  *result = object ? std::string(propertyName) + "-actor" : std::string(propertyName) + "-empty";
}
} // namespace

void utc_dali_property_bridge_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_property_bridge_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPropertyBridgeInternalP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::PropertyBridge directBridge;
  DALI_TEST_CHECK(!directBridge.EnsureStringGetter());
  DALI_TEST_EQUALS(directBridge.GetStringProperty(Actor(), "before"), std::string(), TEST_LOCATION);
  Dali::Ui::Internal::PropertyBridge::RegisterStringGetter(nullptr);
  Dali::Ui::Integration::PropertyBridgeRegisterStringGetter(&GetTestString);
  DALI_TEST_CHECK(directBridge.EnsureStringGetter());
  DALI_TEST_EQUALS(directBridge.GetStringProperty(Actor(), "name"), std::string("name-empty"), TEST_LOCATION);

  Dali::Ui::Integration::PropertyBridge bridge = Dali::Ui::Integration::PropertyBridge::Get();
  DALI_TEST_CHECK(bridge);
  DALI_TEST_EQUALS(Dali::Ui::Integration::PropertyBridge::Get(), bridge, TEST_LOCATION);
  Actor actor = Actor::New();
  DALI_TEST_EQUALS(bridge.GetStringProperty(actor, "title"), std::string("title-actor"), TEST_LOCATION);
  END_TEST;
}
