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

#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_view_accessibility_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_accessibility_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliViewAccessibilityReadingInfoInternalP(void)
{
  UiTestApplication application;

  View view       = View::New();
  auto accessible = Dali::Accessibility::Accessible::Get(view);
  DALI_TEST_CHECK(accessible);

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));

  auto defaultTypes = viewData.GetAccessibilityReadingInfoType();
  DALI_TEST_EQUALS(static_cast<bool>(defaultTypes[Dali::Integration::Accessibility::ReadingInfoType::NAME]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(defaultTypes[Dali::Integration::Accessibility::ReadingInfoType::ROLE]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(defaultTypes[Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(defaultTypes[Dali::Integration::Accessibility::ReadingInfoType::STATE]), true, TEST_LOCATION);

  Dali::Integration::Accessibility::ReadingInfoTypes internalTypes;
  internalTypes[Dali::Integration::Accessibility::ReadingInfoType::NAME]        = true;
  internalTypes[Dali::Integration::Accessibility::ReadingInfoType::ROLE]        = true;
  internalTypes[Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION] = true;
  internalTypes[Dali::Integration::Accessibility::ReadingInfoType::STATE]       = true;
  viewData.SetAccessibilityReadingInfoType(internalTypes);

  auto storedTypes = viewData.GetAccessibilityReadingInfoType();
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::NAME]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::ROLE]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::STATE]), true, TEST_LOCATION);

  viewData.SetAccessibilityReadingInfoType({});
  storedTypes = viewData.GetAccessibilityReadingInfoType();
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::NAME]), false, TEST_LOCATION);

  Property::Map attributes;
  attributes.Insert("reading_info_type", "name|description");
  view.SetProperty(View::Property::ACCESSIBILITY_ATTRIBUTES, attributes);

  auto exportedAttributes = accessible->GetAttributes();
  DALI_TEST_EQUALS(exportedAttributes["reading_info_type"], "name|description", TEST_LOCATION);

  attributes["reading_info_type"] = "name|role|description|state";
  view.SetProperty(View::Property::ACCESSIBILITY_ATTRIBUTES, attributes);

  exportedAttributes = accessible->GetAttributes();
  DALI_TEST_EQUALS(exportedAttributes["reading_info_type"], "name|role|description|state", TEST_LOCATION);

  view.SetProperty(View::Property::ACCESSIBILITY_ATTRIBUTES, Property::Map());

  exportedAttributes = accessible->GetAttributes();
  DALI_TEST_CHECK(exportedAttributes.find("reading_info_type") == exportedAttributes.end());

  END_TEST;
}
