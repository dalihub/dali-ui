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
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_inner_shadow_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_inner_shadow_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInnerShadowDefaultConstructorP(void)
{
  UiTestApplication application;

  InnerShadow innerShadow;

  DALI_TEST_EQUALS(innerShadow.GetColor().GetRgba(), UiColor(0x000000, 0.5f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(innerShadow.GetBlurRadius(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(innerShadow.GetInsets(), Insets(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInnerShadowConstructorAndSettersP(void)
{
  UiTestApplication application;

  InnerShadow innerShadow(Insets(1.0f, 2.0f, 3.0f, 4.0f), 12.0f, UiColor(0.1f, 0.2f, 0.3f, 0.4f));

  DALI_TEST_EQUALS(innerShadow.GetInsets(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(innerShadow.GetBlurRadius(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(innerShadow.GetColor().GetRgba(), Vector4(0.1f, 0.2f, 0.3f, 0.4f), TEST_LOCATION);

  innerShadow.SetInsets(Insets(5.0f, 6.0f, 7.0f, 8.0f))
    .SetBlurRadius(-1.0f)
    .SetColor(UiColor(0.5f, 0.6f, 0.7f, 0.8f));

  DALI_TEST_EQUALS(innerShadow.GetInsets(), Insets(5.0f, 6.0f, 7.0f, 8.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(innerShadow.GetBlurRadius(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(innerShadow.GetColor().GetRgba(), Vector4(0.5f, 0.6f, 0.7f, 0.8f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInnerShadowCopyAndMoveP(void)
{
  UiTestApplication application;

  InnerShadow original(Insets(1.0f, 2.0f, 3.0f, 4.0f), 9.0f, UiColor(0.1f, 0.2f, 0.3f, 0.4f));
  InnerShadow copied(original);

  original.SetBlurRadius(20.0f).SetInsets(Insets(10.0f, 20.0f, 30.0f, 40.0f));

  DALI_TEST_EQUALS(copied.GetBlurRadius(), 9.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copied.GetInsets(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);

  InnerShadow assigned;
  assigned = copied;
  copied.SetColor(UiColor(0.8f, 0.7f, 0.6f, 0.5f));

  DALI_TEST_EQUALS(assigned.GetColor().GetRgba(), Vector4(0.1f, 0.2f, 0.3f, 0.4f), TEST_LOCATION);

  InnerShadow moved(std::move(assigned));
  DALI_TEST_EQUALS(moved.GetBlurRadius(), 9.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetInsets(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);

  InnerShadow moveAssigned;
  moveAssigned = std::move(moved);
  DALI_TEST_EQUALS(moveAssigned.GetBlurRadius(), 9.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInnerShadowNoneP(void)
{
  UiTestApplication application;

  InnerShadow innerShadow;
  DALI_TEST_CHECK(innerShadow != InnerShadow::None());

  InnerShadow none = InnerShadow::None();
  DALI_TEST_CHECK(none == InnerShadow::None());

  DALI_TEST_ASSERTION(none.GetColor(), "Cannot access InnerShadow::None() properties.");
  DALI_TEST_ASSERTION(none.SetBlurRadius(1.0f), "Cannot modify InnerShadow::None().");

  END_TEST;
}

int UtcDaliInnerShadowVisualConversionP(void)
{
  UiTestApplication application;

  const UiColor color(0.1f, 0.2f, 0.3f, 0.4f);
  InnerShadow innerShadow(Insets(20.0f, -10.0f, 30.0f, 5.0f), 8.0f, color);
  View view = View::New();
  view.SetInnerShadow(innerShadow);

  // TizenFX conversion:
  // width = 2 * (max(30, 0) + 8 + 1) = 78
  // offset = ((20 - -10) / 2, (30 - 5) / 2) = (15, 12.5)
  // extraSize = (2 * 78 - 20 - -10, 2 * 78 - 30 - 5) = (146, 121)
  const Property::Value innerShadowValue = view.GetProperty(View::Property::INNER_SHADOW);
  const Property::Map* map = innerShadowValue.GetMap();
  DALI_TEST_CHECK(map);

  const Property::Value* visualTypeValue = map->Find(VisualBasePropertyIndex::TYPE);
  const Property::Value* mixColorValue = map->Find(VisualBasePropertyIndex::MIX_COLOR);
  const Property::Value* blurRadiusValue = map->Find(ColorVisualPropertyIndex::BLUR_RADIUS);
  const Property::Value* cutoutPolicyValue = map->Find(ColorVisualPropertyIndex::CUTOUT_POLICY);
  const Property::Value* borderlineColorValue = map->Find(Dali::Ui::Integration::Visual::Property::BORDERLINE_COLOR);
  const Property::Value* borderlineWidthValue = map->Find(Dali::Ui::Integration::Visual::Property::BORDERLINE_WIDTH);
  const Property::Value* borderlineOffsetValue = map->Find(Dali::Ui::Integration::Visual::Property::BORDERLINE_OFFSET);
  DALI_TEST_CHECK(visualTypeValue);
  DALI_TEST_CHECK(mixColorValue);
  DALI_TEST_CHECK(blurRadiusValue);
  DALI_TEST_CHECK(cutoutPolicyValue);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(visualTypeValue->Get<int>(), static_cast<int>(VisualType::COLOR), TEST_LOCATION);
  DALI_TEST_EQUALS(mixColorValue->Get<Vector4>(), Color::TRANSPARENT, TEST_LOCATION);
  DALI_TEST_EQUALS(blurRadiusValue->Get<float>(), 8.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cutoutPolicyValue->Get<int>(), static_cast<int>(CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS), TEST_LOCATION);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), color.GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), 78.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), -1.0f, TEST_LOCATION);

  const Property::Value* transformValue = map->Find(VisualBasePropertyIndex::TRANSFORM);
  const Property::Map* transformMap = transformValue ? transformValue->GetMap() : nullptr;
  DALI_TEST_CHECK(transformMap);

  const Property::Value* offsetValue = transformMap->Find(Visual::Transform::Property::OFFSET);
  const Property::Value* offsetPolicyValue = transformMap->Find(Visual::Transform::Property::OFFSET_POLICY);
  const Property::Value* extraSizeValue = transformMap->Find(Dali::Ui::Integration::Visual::Transform::Property::EXTRA_SIZE);
  const Property::Value* originValue = transformMap->Find(Visual::Transform::Property::ORIGIN);
  const Property::Value* pivotValue = transformMap->Find(Visual::Transform::Property::PIVOT);
  DALI_TEST_CHECK(offsetValue);
  DALI_TEST_CHECK(offsetPolicyValue);
  DALI_TEST_CHECK(extraSizeValue);
  DALI_TEST_CHECK(originValue);
  DALI_TEST_CHECK(pivotValue);
  DALI_TEST_EQUALS(offsetValue->Get<Vector2>(), Vector2(15.0f, 12.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(offsetPolicyValue->Get<Vector2>(),
                   Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(extraSizeValue->Get<Vector2>(), Vector2(146.0f, 121.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(originValue->Get<int>(), static_cast<int>(Align::CENTER), TEST_LOCATION);
  DALI_TEST_EQUALS(pivotValue->Get<int>(), static_cast<int>(Align::CENTER), TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetInnerShadowP(void)
{
  UiTestApplication application;
  View view = View::New();

  view.SetInnerShadow(InnerShadow(Insets(12.0f, 12.0f, 12.0f, 12.0f), 6.0f, UiColor(0.0f, 0.0f, 0.0f, 0.6f)));

  const Property::Value innerShadowValue = view.GetProperty(View::Property::INNER_SHADOW);
  const Property::Map* innerShadowMap = innerShadowValue.GetMap();
  DALI_TEST_CHECK(innerShadowMap && !innerShadowMap->Empty());

  const Property::Value* borderlineWidth = innerShadowMap->Find(Dali::Ui::Integration::Visual::Property::BORDERLINE_WIDTH);
  const Property::Value* borderlineOffset = innerShadowMap->Find(Dali::Ui::Integration::Visual::Property::BORDERLINE_OFFSET);
  DALI_TEST_CHECK(borderlineWidth);
  DALI_TEST_CHECK(borderlineOffset);
  DALI_TEST_EQUALS(borderlineWidth->Get<float>(), 38.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(borderlineOffset->Get<float>(), -1.0f, TEST_LOCATION);

  view.SetInnerShadow(InnerShadow::None());
  const Property::Value clearedValue = view.GetProperty(View::Property::INNER_SHADOW);
  DALI_TEST_CHECK(clearedValue.GetMap() && clearedValue.GetMap()->Empty());

  END_TEST;
}
