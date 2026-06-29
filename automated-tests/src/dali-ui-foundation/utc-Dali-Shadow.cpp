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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/provider-api/shadow.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_shadow_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_shadow_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliShadowDefaultConstructorP(void)
{
  UiTestApplication application;

  Shadow shadow;

  DALI_TEST_EQUALS(shadow.GetColor().GetRgba(), UiColor(0x000000, 0.5f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetBlurRadius(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetCutoutPolicy(), CutoutPolicy::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetOffset(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetExtents(), Vector2::ZERO, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShadowConstructorAndSettersP(void)
{
  UiTestApplication application;

  Shadow blurOnly(10.0f);
  DALI_TEST_EQUALS(blurOnly.GetColor().GetRgba(), UiColor(0x000000, 0.5f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(blurOnly.GetBlurRadius(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blurOnly.GetOffset(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(blurOnly.GetExtents(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(blurOnly.GetCutoutPolicy(), CutoutPolicy::NONE, TEST_LOCATION);

  Shadow blurAndOffset(11.0f, Vector2(2.0f, 3.0f));
  DALI_TEST_EQUALS(blurAndOffset.GetColor().GetRgba(), UiColor(0x000000, 0.5f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(blurAndOffset.GetBlurRadius(), 11.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blurAndOffset.GetOffset(), Vector2(2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(blurAndOffset.GetExtents(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(blurAndOffset.GetCutoutPolicy(), CutoutPolicy::NONE, TEST_LOCATION);

  Shadow blurAndColor(12.0f, UiColor(0.7f, 0.6f, 0.5f, 0.4f));
  DALI_TEST_EQUALS(blurAndColor.GetColor().GetRgba(), UiColor(0.7f, 0.6f, 0.5f, 0.4f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(blurAndColor.GetBlurRadius(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blurAndColor.GetOffset(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(blurAndColor.GetExtents(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(blurAndColor.GetCutoutPolicy(), CutoutPolicy::NONE, TEST_LOCATION);

  Shadow shadow(10.0f, Vector2(1.0f, 2.0f), UiColor(0.1f, 0.2f, 0.3f, 0.4f), Vector2(3.0f, 4.0f), CutoutPolicy::CUTOUT_OUTSIDE);

  DALI_TEST_EQUALS(shadow.GetColor().GetRgba(), UiColor(0.1f, 0.2f, 0.3f, 0.4f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetBlurRadius(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetCutoutPolicy(), CutoutPolicy::CUTOUT_OUTSIDE, TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetOffset(), Vector2(1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetExtents(), Vector2(3.0f, 4.0f), TEST_LOCATION);

  shadow.SetColor(UiColor(0.5f, 0.6f, 0.7f, 0.8f))
    .SetBlurRadius(-5.0f)
    .SetCutoutPolicy(CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS)
    .SetOffset(Vector2(5.0f, 6.0f))
    .SetExtents(Vector2(7.0f, 8.0f));

  DALI_TEST_EQUALS(shadow.GetColor().GetRgba(), UiColor(0.5f, 0.6f, 0.7f, 0.8f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetBlurRadius(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetCutoutPolicy(), CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS, TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetOffset(), Vector2(5.0f, 6.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(shadow.GetExtents(), Vector2(7.0f, 8.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliShadowCopyAndMoveP(void)
{
  UiTestApplication application;

  Shadow original(9.0f, Vector2(1.0f, 2.0f), UiColor(0.1f, 0.2f, 0.3f, 0.4f), Vector2(3.0f, 4.0f), CutoutPolicy::CUTOUT_VIEW);
  Shadow copied(original);

  original.SetBlurRadius(20.0f).SetOffset(Vector2(10.0f, 20.0f));

  DALI_TEST_EQUALS(copied.GetBlurRadius(), 9.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copied.GetOffset(), Vector2(1.0f, 2.0f), TEST_LOCATION);

  Shadow assigned;
  assigned = copied;

  copied.SetExtents(Vector2(30.0f, 40.0f));

  DALI_TEST_EQUALS(assigned.GetExtents(), Vector2(3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(assigned.GetCutoutPolicy(), CutoutPolicy::CUTOUT_VIEW, TEST_LOCATION);

  Shadow moved(std::move(assigned));

  DALI_TEST_EQUALS(moved.GetBlurRadius(), 9.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetOffset(), Vector2(1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetExtents(), Vector2(3.0f, 4.0f), TEST_LOCATION);

  Shadow moveAssigned;
  moveAssigned = std::move(moved);

  DALI_TEST_EQUALS(moveAssigned.GetBlurRadius(), 9.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moveAssigned.GetCutoutPolicy(), CutoutPolicy::CUTOUT_VIEW, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShadowNoneP(void)
{
  UiTestApplication application;

  Shadow shadow;
  DALI_TEST_CHECK(shadow != Shadow::None());

  Shadow none = Shadow::None();
  DALI_TEST_CHECK(none == Shadow::None());

  Shadow copiedNone(none);
  DALI_TEST_CHECK(copiedNone == Shadow::None());
  DALI_TEST_CHECK(shadow != copiedNone);

  Shadow sameShadow;
  DALI_TEST_CHECK(shadow == sameShadow);
  sameShadow.SetBlurRadius(1.0f);
  DALI_TEST_CHECK(shadow != sameShadow);

  DALI_TEST_ASSERTION(none.GetColor(), "Cannot access Shadow::None() properties.");
  DALI_TEST_ASSERTION(none.SetBlurRadius(1.0f), "Cannot modify Shadow::None().");

  END_TEST;
}

int UtcDaliShadowPropertyMapRoundTripP(void)
{
  UiTestApplication application;

  Shadow       shadow(12.0f, Vector2(6.0f, 7.0f), UiColor(0.2f, 0.3f, 0.4f, 0.5f), Vector2(8.0f, 9.0f), CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS);
  Property::Map map = Provider::Shadow::CreatePropertyMap(shadow);
  Shadow       roundTrip = Provider::Shadow::CreateShadow(map);

  DALI_TEST_EQUALS(roundTrip.GetColor().GetRgba(), shadow.GetColor().GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(roundTrip.GetBlurRadius(), shadow.GetBlurRadius(), TEST_LOCATION);
  DALI_TEST_EQUALS(roundTrip.GetCutoutPolicy(), shadow.GetCutoutPolicy(), TEST_LOCATION);
  DALI_TEST_EQUALS(roundTrip.GetOffset(), shadow.GetOffset(), TEST_LOCATION);
  DALI_TEST_EQUALS(roundTrip.GetExtents(), shadow.GetExtents(), TEST_LOCATION);

  Shadow none = Provider::Shadow::CreateShadow(Property::Map());
  DALI_TEST_CHECK(none == Shadow::None());
  DALI_TEST_CHECK(Provider::Shadow::CreatePropertyMap(Shadow::None()).Empty());

  END_TEST;
}

int UtcDaliShadowVisualHelpersP(void)
{
  UiTestApplication application;

  Shadow      shadow(14.0f, Vector2(1.5f, 2.5f), UiColor(0.3f, 0.4f, 0.5f, 0.6f), Vector2(3.5f, 4.5f), CutoutPolicy::CUTOUT_VIEW);
  ColorVisual visual = Provider::Shadow::CreateVisual(shadow);

  DALI_TEST_EQUALS(visual.GetColor().GetRgba(), shadow.GetColor().GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBlurRadius(), shadow.GetBlurRadius(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCutoutPolicy(), shadow.GetCutoutPolicy(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOffsetX(), shadow.GetOffset().x, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOffsetY(), shadow.GetOffset().y, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetExtraWidth(), shadow.GetExtents().width, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetExtraHeight(), shadow.GetExtents().height, TEST_LOCATION);

  END_TEST;
}
