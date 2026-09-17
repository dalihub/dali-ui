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

#include <dali-ui-foundation/integration-api/shader-effects/dissolve-effect.h>
#include <dali-ui-foundation/integration-api/shader-effects/motion-blur-effect.h>
#include <dali-ui-foundation/integration-api/shader-effects/motion-stretch-effect.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visual-factory/precompile-shader-option.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_shader_effects_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_shader_effects_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliShaderEffectsPropertyMapsP(void)
{
  UiTestApplication application;
  Property::Map high = Ui::Integration::CreateDissolveEffect(true);
  Property::Map medium = Ui::Integration::CreateDissolveEffect(false);
  Property::Map blur = Ui::Integration::CreateMotionBlurEffect();
  Property::Map stretch = Ui::Integration::CreateMotionStretchEffect();

  DALI_TEST_CHECK(high.Find(Ui::Integration::Visual::Property::SHADER));
  DALI_TEST_CHECK(medium.Find(Ui::Integration::Visual::Property::SHADER));
  DALI_TEST_CHECK(blur.Find(Ui::Integration::Visual::Property::SHADER));
  DALI_TEST_CHECK(stretch.Find(Ui::Integration::Visual::Property::SHADER));
  END_TEST;
}

int UtcDaliShaderEffectsUniformsP(void)
{
  UiTestApplication application;
  Actor actor = Actor::New();
  Ui::Integration::DissolveEffectSetCentralLine(actor, Vector2(0.5f, 0.5f), Vector2(1.0f, -1.0f), 0.25f);
  DALI_TEST_CHECK(actor.GetPropertyIndex("uSaddleParam") != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(actor.GetProperty<float>(actor.GetPropertyIndex("uPercentage")), 0.25f, 0.001f, TEST_LOCATION);

  Actor second = Actor::New();
  Ui::Integration::DissolveEffectSetCentralLine(second, Vector2(0.25f, 0.75f), Vector2(-1.0f, -1.0f), 0.5f);
  Actor third = Actor::New();
  Ui::Integration::DissolveEffectSetCentralLine(third, Vector2(0.5f, 0.5f), Vector2(0.0f, 1.0f), 0.75f);

  Ui::Integration::SetMotionBlurProperties(actor, 4u);
  DALI_TEST_EQUALS(actor.GetProperty<float>(actor.GetPropertyIndex("uNumSamples")), 4.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty<float>(actor.GetPropertyIndex("uRecipNumSamples")), 0.25f, 0.001f, TEST_LOCATION);
  Ui::Integration::SetMotionStretchProperties(second);
  DALI_TEST_EQUALS(second.GetProperty<float>(second.GetPropertyIndex("uAlphaScale")), 0.75f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPrecompileShaderOptionP(void)
{
  UiTestApplication application;
  Property::Map flags;
  flags.Insert("ROUNDED_CORNER", true);
  flags.Insert("BORDERLINE", false);
  flags.Insert("EMOJI", true);
  flags.Insert("UNKNOWN_FLAG", true);
  flags.Insert(3, true);
  Property::Map map;
  map.Insert("shaderType", "custom");
  map.Insert("shaderOption", flags);
  map.Insert("vertexShader", "vertex");
  map.Insert("fragmentShader", "fragment");
  map.Insert("shaderName", "named");
  map.Insert("xStretchCount", 3);
  map.Insert("yStretchCount", 4);
  map.Insert(5, "ignored");

  Ui::Integration::PrecompileShaderOption option(map);
  DALI_TEST_EQUALS(static_cast<int>(option.GetShaderType()), static_cast<int>(Ui::Integration::PrecompileShaderOption::ShaderType::CUSTOM), TEST_LOCATION);
  DALI_TEST_EQUALS(option.GetShaderOptions().size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(option.GetVertexShader(), std::string("vertex"), TEST_LOCATION);
  DALI_TEST_EQUALS(option.GetFragmentShader(), std::string("fragment"), TEST_LOCATION);
  DALI_TEST_EQUALS(option.GetShaderName(), std::string("named"), TEST_LOCATION);
  DALI_TEST_EQUALS(option.GetNpatchXStretchCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(option.GetNpatchYStretchCount(), 4u, TEST_LOCATION);

  Property::Map invalid;
  invalid.Insert("shaderType", "invalid");
  invalid.Insert("vertexShader", 1);
  invalid.Insert("fragmentShader", 2);
  invalid.Insert("shaderName", 3);
  invalid.Insert("xStretchCount", "bad");
  invalid.Insert("yStretchCount", "bad");
  option.ConvertShaderMap(invalid);
  DALI_TEST_EQUALS(option.GetShaderName(), std::string("named"), TEST_LOCATION);
  END_TEST;
}
