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

#include <dali-ui-foundation/internal/text/text-effects-style.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_text_effects_style_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_effects_style_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextEffectsStyleDefaultMapsP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = Text::Controller::New();
  Property::Value output;

  Property::Map underline;
  underline.Insert("enable", true);
  underline.Insert("color", Vector4(0.2f, 0.3f, 0.4f, 1.0f));
  underline.Insert("height", 3.0f);
  underline.Insert("type", static_cast<int>(Text::Underline::Type::DASHED));
  underline.Insert("dashWidth", 6.0f);
  underline.Insert("dashGap", 2.0f);
  DALI_TEST_CHECK(Text::SetUnderlineProperties(controller, Property::Value(underline), Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetUnderlineProperties(controller, Property::Value(underline), Text::EffectStyle::DEFAULT));
  Text::GetUnderlineProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::MAP, TEST_LOCATION);

  Property::Map shadow;
  shadow.Insert("color", Vector4(0.1f, 0.2f, 0.3f, 0.8f));
  shadow.Insert("offset", Vector2(4.0f, 5.0f));
  shadow.Insert("blurRadius", 7.0f);
  DALI_TEST_CHECK(Text::SetShadowProperties(controller, Property::Value(shadow), Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetShadowProperties(controller, Property::Value(shadow), Text::EffectStyle::DEFAULT));
  Text::GetShadowProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::MAP, TEST_LOCATION);

  Property::Map emboss;
  emboss.Insert("enable", true);
  emboss.Insert("direction", Vector2(1.0f, -1.0f));
  emboss.Insert("strength", 0.75f);
  emboss.Insert("lightColor", Vector4(1.0f, 0.9f, 0.7f, 1.0f));
  emboss.Insert("shadowColor", Vector4(0.1f, 0.1f, 0.2f, 1.0f));
  DALI_TEST_CHECK(Text::SetEmbossProperties(controller, Property::Value(emboss), Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetEmbossProperties(controller, Property::Value(emboss), Text::EffectStyle::DEFAULT));
  Text::GetEmbossProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::MAP, TEST_LOCATION);

  Property::Map outline;
  outline.Insert("color", Vector4(0.8f, 0.2f, 0.4f, 1.0f));
  outline.Insert("width", 4.0f);
  outline.Insert("offset", Vector2(2.0f, 3.0f));
  outline.Insert("blurRadius", 5.0f);
  DALI_TEST_CHECK(Text::SetOutlineProperties(controller, Property::Value(outline), Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetOutlineProperties(controller, Property::Value(outline), Text::EffectStyle::DEFAULT));
  Text::GetOutlineProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::MAP, TEST_LOCATION);

  Property::Map background;
  background.Insert("enable", true);
  background.Insert("color", Vector4(0.9f, 0.8f, 0.2f, 1.0f));
  DALI_TEST_CHECK(Text::SetBackgroundProperties(controller, Property::Value(background), Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetBackgroundProperties(controller, Property::Value(background), Text::EffectStyle::DEFAULT));
  Text::GetBackgroundProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::MAP, TEST_LOCATION);

  Property::Map strike;
  strike.Insert("enable", true);
  strike.Insert("color", Vector4(0.7f, 0.1f, 0.2f, 1.0f));
  strike.Insert("height", 2.5f);
  DALI_TEST_CHECK(Text::SetStrikethroughProperties(controller, Property::Value(strike), Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetStrikethroughProperties(controller, Property::Value(strike), Text::EffectStyle::DEFAULT));
  Text::GetStrikethroughProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::MAP, TEST_LOCATION);

  Property::Map empty;
  Property::Value emptyString("");
  DALI_TEST_CHECK(Text::SetUnderlineProperties(controller, emptyString, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(Text::SetShadowProperties(controller, emptyString, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(Text::SetEmbossProperties(controller, emptyString, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(Text::SetOutlineProperties(controller, emptyString, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(Text::SetBackgroundProperties(controller, Property::Value(empty), Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(Text::SetStrikethroughProperties(controller, emptyString, Text::EffectStyle::DEFAULT));
  END_TEST;
}

int UtcDaliTextEffectsStyleStringsAndInputP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = Text::Controller::New();
  Property::Value output;

  const Property::Value underline(
    R"JSON({"enable":"true","color":"red","height":"2.5","type":"double","dashWidth":"5","dashGap":"3"})JSON");
  DALI_TEST_CHECK(Text::SetUnderlineProperties(controller, underline, Text::EffectStyle::DEFAULT));
  Text::GetUnderlineProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);

  const Property::Value shadow(R"JSON({"color":"blue","offset":"3 4","blurRadius":"6"})JSON");
  DALI_TEST_CHECK(Text::SetShadowProperties(controller, shadow, Text::EffectStyle::DEFAULT));
  Text::GetShadowProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);

  const Property::Value strike(R"JSON({"enable":"true","color":"green","height":"1.5"})JSON");
  DALI_TEST_CHECK(Text::SetStrikethroughProperties(controller, strike, Text::EffectStyle::DEFAULT));
  Text::GetStrikethroughProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);

  const Property::Value legacy("legacy-effect");
  DALI_TEST_CHECK(!Text::SetEmbossProperties(controller, legacy, Text::EffectStyle::DEFAULT));
  Text::GetEmbossProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);
  DALI_TEST_CHECK(!Text::SetOutlineProperties(controller, legacy, Text::EffectStyle::DEFAULT));
  Text::GetOutlineProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);

  DALI_TEST_CHECK(!Text::SetUnderlineProperties(controller, legacy, Text::EffectStyle::INPUT));
  Text::GetUnderlineProperties(controller, output, Text::EffectStyle::INPUT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);
  DALI_TEST_CHECK(!Text::SetShadowProperties(controller, legacy, Text::EffectStyle::INPUT));
  Text::GetShadowProperties(controller, output, Text::EffectStyle::INPUT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);
  DALI_TEST_CHECK(!Text::SetEmbossProperties(controller, legacy, Text::EffectStyle::INPUT));
  Text::GetEmbossProperties(controller, output, Text::EffectStyle::INPUT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);
  DALI_TEST_CHECK(!Text::SetOutlineProperties(controller, legacy, Text::EffectStyle::INPUT));
  Text::GetOutlineProperties(controller, output, Text::EffectStyle::INPUT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);
  DALI_TEST_CHECK(Text::SetStrikethroughProperties(controller, legacy, Text::EffectStyle::INPUT));
  Text::GetStrikethroughProperties(controller, output, Text::EffectStyle::INPUT);
  DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);
  DALI_TEST_CHECK(!Text::SetBackgroundProperties(controller, legacy, Text::EffectStyle::INPUT));
  Text::GetBackgroundProperties(controller, output, Text::EffectStyle::INPUT);

  DALI_TEST_EQUALS(Text::StringToUnderlineType("dashed"), Text::Underline::Type::DASHED, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::StringToUnderlineType("double"), Text::Underline::Type::DOUBLE, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::StringToUnderlineType("unknown"), Text::Underline::Type::SOLID, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextEffectsStyleNullControllerP(void)
{
  Text::ControllerPtr controller;
  Property::Map empty;
  Property::Value input(empty);
  Property::Value output;

  DALI_TEST_CHECK(!Text::SetUnderlineProperties(controller, input, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetShadowProperties(controller, input, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetEmbossProperties(controller, input, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetOutlineProperties(controller, input, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetBackgroundProperties(controller, input, Text::EffectStyle::DEFAULT));
  DALI_TEST_CHECK(!Text::SetStrikethroughProperties(controller, input, Text::EffectStyle::DEFAULT));
  Text::GetUnderlineProperties(controller, output, Text::EffectStyle::DEFAULT);
  Text::GetShadowProperties(controller, output, Text::EffectStyle::DEFAULT);
  Text::GetEmbossProperties(controller, output, Text::EffectStyle::DEFAULT);
  Text::GetOutlineProperties(controller, output, Text::EffectStyle::DEFAULT);
  Text::GetBackgroundProperties(controller, output, Text::EffectStyle::DEFAULT);
  Text::GetStrikethroughProperties(controller, output, Text::EffectStyle::DEFAULT);
  DALI_TEST_EQUALS(output.GetType(), Property::NONE, TEST_LOCATION);
  END_TEST;
}
