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

#include <dali-ui-foundation/integration-api/visual-factory/precompile-shader-option.h>
#include <dali-ui-foundation/internal/visuals/custom-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-shader-factory.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Dali::Ui::Integration::PrecompileShaderOption MakeOption(uint32_t xStretchCount, uint32_t yStretchCount, bool masking)
{
  Property::Map flags;
  if(masking)
  {
    flags.Insert("MASKING", true);
  }
  Property::Map map;
  map.Insert("shaderType", "npatch");
  map.Insert("shaderOption", flags);
  map.Insert("vertexShader", "vertex-prefix");
  map.Insert("fragmentShader", "fragment-prefix");
  map.Insert("shaderName", "custom-name");
  map.Insert("xStretchCount", static_cast<int>(xStretchCount));
  map.Insert("yStretchCount", static_cast<int>(yStretchCount));
  return Dali::Ui::Integration::PrecompileShaderOption(map);
}
} // namespace

void utc_dali_shader_factory_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_shader_factory_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCustomShaderFactoryP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::CustomShaderFactory factory;
  auto option = MakeOption(0u, 0u, false);
  DALI_TEST_CHECK(factory.AddPrecompiledShader(option));
  DALI_TEST_CHECK(factory.AddPrecompiledShader(option));

  ShaderPreCompiler::RawShaderData shaders;
  factory.GetPreCompiledShader(shaders);
  DALI_TEST_EQUALS(shaders.shaderCount, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(shaders.shaderName[0], std::string("custom-name"), TEST_LOCATION);
  DALI_TEST_EQUALS(shaders.vertexPrefix[0], std::string("vertex-prefix"), TEST_LOCATION);
  DALI_TEST_EQUALS(shaders.fragmentPrefix[0], std::string("fragment-prefix"), TEST_LOCATION);
  DALI_TEST_CHECK(shaders.vertexShader.empty());
  DALI_TEST_CHECK(shaders.fragmentShader.empty());
  DALI_TEST_CHECK(shaders.custom);

  factory.GetPreCompiledShader(shaders);
  DALI_TEST_EQUALS(shaders.shaderCount, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliNpatchShaderFactoryP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::NpatchShaderFactory factory;
  ShaderPreCompiler::RawShaderData shaders;

  auto standard = MakeOption(1u, 1u, false);
  DALI_TEST_CHECK(factory.AddPrecompiledShader(standard));
  DALI_TEST_CHECK(!factory.AddPrecompiledShader(standard));
  factory.GetPreCompiledShader(shaders);
  DALI_TEST_EQUALS(shaders.shaderCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!shaders.vertexPrefix[0].empty());
  DALI_TEST_CHECK(!shaders.fragmentPrefix[0].empty());

  auto custom = MakeOption(2u, 3u, false);
  DALI_TEST_CHECK(factory.AddPrecompiledShader(custom));
  factory.GetPreCompiledShader(shaders);
  DALI_TEST_EQUALS(shaders.shaderCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(shaders.shaderName[0].find("2x3") != std::string::npos);
  DALI_TEST_CHECK(shaders.vertexPrefix[0].find("FACTOR_SIZE_X 4") != std::string::npos);

  auto masking = MakeOption(0u, 0u, true);
  DALI_TEST_CHECK(factory.AddPrecompiledShader(masking));
  factory.GetPreCompiledShader(shaders);
  DALI_TEST_EQUALS(shaders.shaderCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(shaders.custom);
  END_TEST;
}
