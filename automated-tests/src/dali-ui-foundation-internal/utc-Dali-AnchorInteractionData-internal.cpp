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

#include <dali-ui-foundation/internal/text/anchor/anchor-interaction-data.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Internal::Text;

void utc_dali_anchor_interaction_data_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_anchor_interaction_data_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
Dali::Ui::Text::AsyncAnchorHitRegion MakeRegion(uint32_t start, uint32_t count, const char* href,
                                                 std::initializer_list<Rect<float>> rectangles)
{
  Dali::Ui::Text::AsyncAnchorHitRegion region;
  region.characterIndex = start;
  region.numberOfCharacters = count;
  region.href = href;
  region.rectangles = rectangles;
  return region;
}
} // unnamed namespace

int UtcDaliAnchorInteractionTouchAndHitP(void)
{
  UiTestApplication application(UiConfig::New());
  View owner = View::New();
  AnchorInteractionData& data = GetOrCreateAnchorInteractionData(owner);
  DALI_TEST_CHECK(GetAnchorInteractionData(owner) == &data);
  DALI_TEST_CHECK(!data.IsTouchDown());
  data.StartTouch(Vector2(12.0f, 18.0f));
  DALI_TEST_CHECK(data.IsTouchDown());
  DALI_TEST_EQUALS(data.GetTouchPosition(), Vector2(12.0f, 18.0f), TEST_LOCATION);
  data.EndTouch();

  std::vector<Dali::Ui::Text::AsyncAnchorHitRegion> regions;
  regions.push_back(MakeRegion(0u, 4u, "first", {Rect<float>(0.0f, 0.0f, 20.0f, 10.0f)}));
  regions.push_back(MakeRegion(5u, 3u, "second", {Rect<float>(10.0f, 0.0f, 20.0f, 10.0f)}));
  DALI_TEST_CHECK(SetAnchorHitRegions(owner, std::move(regions)));
  DALI_TEST_CHECK(data.HasHitRegions());

  AnchorHitResult overlap = HitTestAnchor(owner, Vector2(15.0f, 5.0f));
  DALI_TEST_CHECK(overlap.hit);
  DALI_TEST_CHECK(overlap.newlyClicked);
  DALI_TEST_EQUALS(overlap.href, std::string("second"), TEST_LOCATION);
  DALI_TEST_CHECK(!HitTestAnchor(owner, Vector2(15.0f, 5.0f)).newlyClicked);
  DALI_TEST_CHECK(ActivateAnchor(owner, 1u).hit);
  DALI_TEST_CHECK(!ActivateAnchor(owner, 99u).hit);
  DALI_TEST_CHECK(!HitTestAnchor(owner, Vector2(100.0f, 100.0f)).hit);
  DALI_TEST_EQUALS(GetAnchorClickedStates(owner).size(), 2u, TEST_LOCATION);

  std::vector<Dali::Ui::Text::AsyncAnchorHitRegion> replacement;
  replacement.push_back(MakeRegion(5u, 3u, "second", {Rect<float>(1.0f, 1.0f, 2.0f, 2.0f)}));
  SetAnchorHitRegions(owner, std::move(replacement));
  DALI_TEST_EQUALS(GetAnchorClickedStates(owner).size(), 1u, TEST_LOCATION);
  ClearAnchorInteractionData(owner);
  DALI_TEST_CHECK(!data.HasHitRegions());
  END_TEST;
}

int UtcDaliAnchorInteractionAccessibilityP(void)
{
  UiTestApplication application(UiConfig::New());
  View owner = View::New();
  std::vector<Dali::Ui::Text::AsyncAnchorHitRegion> regions;
  regions.push_back(MakeRegion(2u, 5u, "https://example.com", {
    Rect<float>(10.0f, 20.0f, 30.0f, 10.0f), Rect<float>(5.0f, 35.0f, 45.0f, 15.0f)}));
  regions.push_back(MakeRegion(8u, 2u, "empty", {}));
  regions.push_back(MakeRegion(10u, 2u, "zero", {Rect<float>(0.0f, 0.0f, 0.0f, 3.0f)}));
  DALI_TEST_CHECK(SetAnchorHitRegions(owner, std::move(regions)));
  DALI_TEST_CHECK(UpdateA11yAnchorsFromHitRegions(owner, Vector2(3.0f, 4.0f)));
  DALI_TEST_EQUALS(GetA11yAnchors(owner).size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(owner.GetChildCount(), 1u, TEST_LOCATION);

  ClearA11yAnchors(owner);
  DALI_TEST_CHECK(GetA11yAnchors(owner).empty());
  std::vector<TextAnchor> anchors;
  anchors.push_back(TextAnchor());
  anchors.push_back(TextAnchor::New());
  DALI_TEST_CHECK(SetA11yAnchors(owner, std::move(anchors)));
  DALI_TEST_EQUALS(GetA11yAnchors(owner).size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!SetA11yAnchors(owner, {}));
  END_TEST;
}

int UtcDaliAnchorInteractionInvalidOwnerP(void)
{
  View owner;
  DALI_TEST_CHECK(!GetAnchorInteractionData(owner));
  DALI_TEST_CHECK(GetA11yAnchors(owner).empty());
  DALI_TEST_CHECK(GetAnchorClickedStates(owner).empty());
  DALI_TEST_CHECK(!HitTestAnchor(owner, Vector2::ZERO).hit);
  DALI_TEST_CHECK(!ActivateAnchor(owner, 0u).hit);
  DALI_TEST_CHECK(!SetAnchorHitRegions(owner, {}));
  DALI_TEST_CHECK(!SetA11yAnchors(owner, {}));
  DALI_TEST_CHECK(!UpdateA11yAnchorsFromHitRegions(owner, Vector2::ZERO));
  ClearAnchorInteractionData(owner);
  ClearA11yAnchors(owner);

  UiTestApplication application(UiConfig::New());
  View valid = View::New();
  DALI_TEST_CHECK(!SetAnchorHitRegions(valid, {}));
  DALI_TEST_CHECK(!UpdateA11yAnchorsFromHitRegions(valid, Vector2::ZERO));
  END_TEST;
}
