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
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali-ui-foundation/internal/visuals/color/color-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-debug.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-shader-factory.h>
#include <dali-ui-test-suite-utils.h>

namespace Dali::EnvironmentVariable
{
void SetTestEnvironmentVariable(const char* variable, const char* value);
}


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
Dali::Ui::Integration::PrecompileShaderOption MakeVisualOption(const char* visualType, const char* flag)
{
  Property::Map options;
  options.Insert(flag, true);
  Property::Map map;
  map.Insert("shaderType", visualType);
  map.Insert("shaderOption", options);
  return Dali::Ui::Integration::PrecompileShaderOption(map);
}

int UtcDaliImageShaderFactoryFeatureMatrixP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::VisualFactoryCache cache(true);
  Dali::Ui::Internal::ImageVisualShaderFactory factory;
  DALI_TEST_CHECK(!factory.GetVertexShaderSource().empty());
  DALI_TEST_CHECK(!factory.GetFragmentShaderSource().empty());

  for(int corner = 0; corner < 3; ++corner)
  {
    for(int borderline = 0; borderline < 2; ++borderline)
    {
      for(int variant = 0; variant < 5; ++variant)
      {
        Dali::Ui::Internal::ImageVisualShaderFeature::FeatureBuilder feature;
        feature.EnableRoundedCorner(corner != 0, corner == 2);
        feature.EnableBorderline(borderline != 0);
        feature.EnableAlphaMaskingOnRendering(variant == 1);
        feature.EnableYuvToRgb(variant >= 2, variant == 3, variant == 4);
        Shader shader = factory.GetShader(cache, feature);
        DALI_TEST_CHECK(shader);
        DALI_TEST_CHECK(factory.GetShader(cache, feature) == shader);
      }
    }
  }

  auto standard = MakeVisualOption("image", "ROUNDED_CORNER");
  auto masked = MakeVisualOption("image", "MASKING");
  DALI_TEST_CHECK(!factory.AddPrecompiledShader(standard));
  DALI_TEST_CHECK(factory.AddPrecompiledShader(masked));
  DALI_TEST_CHECK(!factory.AddPrecompiledShader(masked));
  ShaderPreCompiler::RawShaderData shaders;
  factory.GetPreCompiledShader(shaders);
  DALI_TEST_EQUALS(shaders.shaderCount, 7u, TEST_LOCATION);
  DALI_TEST_CHECK(!shaders.vertexShader.empty());
  DALI_TEST_CHECK(!shaders.fragmentShader.empty());
  END_TEST;
}

int UtcDaliColorShaderFactoryFeatureMatrixP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::VisualFactoryCache cache(true);
  Dali::Ui::Internal::ColorVisualShaderFactory factory;
  DALI_TEST_CHECK(!factory.GetVertexShaderSource().empty());
  DALI_TEST_CHECK(!factory.GetFragmentShaderSource().empty());

  for(int corner = 0; corner < 3; ++corner)
  {
    for(int flags = 0; flags < 8; ++flags)
    {
      Dali::Ui::Internal::ColorVisualShaderFeature::FeatureBuilder feature;
      feature.EnableRoundCorner(corner != 0, corner == 2);
      feature.EnableBorderLine((flags & 1) != 0);
      feature.EnableBlur((flags & 2) != 0);
      feature.EnableCutout((flags & 4) != 0);
      Shader shader = factory.GetShader(cache, feature);
      DALI_TEST_CHECK(shader);
      DALI_TEST_CHECK(factory.GetShader(cache, feature) == shader);
    }
  }

  auto standard = MakeVisualOption("color", "ROUNDED_CORNER");
  auto squircle = MakeVisualOption("color", "SQUIRCLE_CORNER");
  DALI_TEST_CHECK(!factory.AddPrecompiledShader(standard));
  DALI_TEST_CHECK(factory.AddPrecompiledShader(squircle));
  DALI_TEST_CHECK(!factory.AddPrecompiledShader(squircle));
  ShaderPreCompiler::RawShaderData shaders;
  factory.GetPreCompiledShader(shaders);
  DALI_TEST_EQUALS(shaders.shaderCount, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(!shaders.vertexShader.empty());
  DALI_TEST_CHECK(!shaders.fragmentShader.empty());
  END_TEST;
}

int UtcDaliImageShaderDebugScriptFallbackP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::ImageVisualShaderFactory factory;
  std::string vertex(factory.GetVertexShaderSource());
  std::string fragment(factory.GetFragmentShaderSource());
  const std::string originalVertex = vertex;
  const std::string originalFragment = fragment;
  Dali::Ui::Internal::ImageVisualShaderDebug::ApplyImageVisualShaderDebugScriptCode(vertex, fragment);
  DALI_TEST_CHECK(vertex != originalVertex);
  DALI_TEST_CHECK(fragment != originalFragment);
  DALI_TEST_CHECK(vertex.find("DEBUG_APPLY_VARYING_CODE") != std::string::npos);
  DALI_TEST_CHECK(fragment.find("DEBUG_TRIGGER_RED_CODE") != std::string::npos);
  END_TEST;
}

int UtcDaliImageShaderDebugScriptFromJsonP(void)
{
  UiTestApplication application;
  std::string scriptPath(__FILE__);
  scriptPath = scriptPath.substr(0u, scriptPath.find_last_of('/')) + "/resources/debug-image-visual-shader-script.json";
  Dali::EnvironmentVariable::SetTestEnvironmentVariable("DALI_DEBUG_IMAGE_VISUAL_SHADER_SCRIPT_FILE_NAME", scriptPath.c_str());

  Dali::Ui::Internal::ImageVisualShaderFactory factory;
  std::string vertex(factory.GetVertexShaderSource());
  std::string fragment(factory.GetFragmentShaderSource());
  Dali::Ui::Internal::ImageVisualShaderDebug::ApplyImageVisualShaderDebugScriptCode(vertex, fragment);
  DALI_TEST_CHECK(vertex.find("vDebug = gl_Position.xyz") != std::string::npos);
  DALI_TEST_CHECK(vertex.find("mediump vec3 vDebug") != std::string::npos);
  DALI_TEST_CHECK(fragment.find("mediump float uDebug") != std::string::npos);
  DALI_TEST_CHECK(fragment.find("0.5") != std::string::npos);
  DALI_TEST_CHECK(fragment.find("return false") != std::string::npos);
  END_TEST;
}

int UtcDaliImageShaderDebugScriptInvalidJsonP(void)
{
  UiTestApplication application;
  Dali::EnvironmentVariable::SetTestEnvironmentVariable("DALI_DEBUG_IMAGE_VISUAL_SHADER_SCRIPT_FILE_NAME", __FILE__);

  Dali::Ui::Internal::ImageVisualShaderFactory factory;
  std::string vertex(factory.GetVertexShaderSource());
  std::string fragment(factory.GetFragmentShaderSource());
  Dali::Ui::Internal::ImageVisualShaderDebug::ApplyImageVisualShaderDebugScriptCode(vertex, fragment);
  DALI_TEST_CHECK(vertex.find("DEBUG_APPLY_VARYING_CODE") != std::string::npos);
  DALI_TEST_CHECK(fragment.find("return false;") != std::string::npos);
  END_TEST;
}
