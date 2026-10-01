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

#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
namespace UiIntegration = Dali::Ui::Integration;


void utc_dali_visual_factory_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_visual_factory_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVisualFactoryCreationRoutesP(void)
{
  UiTestApplication application;
  UiIntegration::VisualFactory factory = UiIntegration::VisualFactory::Get();
  DALI_TEST_CHECK(factory);
  DALI_TEST_CHECK(factory.GetDefaultQuadGeometry());

  factory.SetPreMultiplyOnLoad(false);
  DALI_TEST_CHECK(!factory.GetPreMultiplyOnLoad());
  factory.SetPreMultiplyOnLoad(true);
  DALI_TEST_CHECK(factory.GetPreMultiplyOnLoad());

  factory.SetDefaultCreationOptions(UiIntegration::VisualFactory::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY);
  DALI_TEST_EQUALS(factory.GetDefaultCreationOptions(), UiIntegration::VisualFactory::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY, TEST_LOCATION);

  const char* urls[] = {"missing.png", "missing.9.png", "missing.svg", "missing.tvg", "missing.json", "missing.gif", "missing.webp"};
  for(const char* url : urls)
  {
    UiIntegration::Visual::Base visual = factory.CreateVisual(url, ImageDimensions(48u, 48u));
    DALI_TEST_CHECK(visual);
  }
  DALI_TEST_CHECK(!factory.CreateVisual("", ImageDimensions()));

  Property::Map imageMap;
  imageMap.Insert(UiIntegration::Visual::Property::TYPE, UiIntegration::InternalVisualType::IMAGE);
  imageMap.Insert(UiIntegration::ImageVisual::Property::URL, "missing.webp");
  DALI_TEST_CHECK(factory.CreateVisual(imageMap));
  DALI_TEST_CHECK(factory.CreateVisual(imageMap, UiIntegration::VisualFactory::NONE));

  Property::Array frames;
  frames.PushBack("first.png");
  frames.PushBack("second.png");
  imageMap[UiIntegration::ImageVisual::Property::URL] = frames;
  DALI_TEST_CHECK(factory.CreateVisual(imageMap));

  imageMap[UiIntegration::ImageVisual::Property::URL] = "";
  DALI_TEST_CHECK(!factory.CreateVisual(imageMap));
  imageMap[UiIntegration::ImageVisual::Property::URL] = 42;
  DALI_TEST_CHECK(!factory.CreateVisual(imageMap));

  factory.SetDefaultCreationOptions(UiIntegration::VisualFactory::NONE);
  END_TEST;
}

int UtcDaliVisualFactoryTypedRoutesP(void)
{
  UiTestApplication application;
  UiIntegration::VisualFactory factory = UiIntegration::VisualFactory::Get();
  const UiIntegration::InternalVisualType types[] = {
    UiIntegration::InternalVisualType::BORDER,
    UiIntegration::InternalVisualType::COLOR,
    UiIntegration::InternalVisualType::GRADIENT,
    UiIntegration::InternalVisualType::MESH,
    UiIntegration::InternalVisualType::PRIMITIVE,
    UiIntegration::InternalVisualType::WIREFRAME,
    UiIntegration::InternalVisualType::TEXT,
    UiIntegration::InternalVisualType::ARC
  };
  for(auto type : types)
  {
    Property::Map properties;
    properties.Insert(UiIntegration::Visual::Property::TYPE, type);
    DALI_TEST_CHECK(factory.CreateVisual(properties));
  }

  const UiIntegration::InternalVisualType urlTypes[] = {
    UiIntegration::InternalVisualType::N_PATCH,
    UiIntegration::InternalVisualType::SVG,
    UiIntegration::InternalVisualType::LOTTIE_ANIMATION
  };
  for(auto type : urlTypes)
  {
    Property::Map properties;
    properties.Insert(UiIntegration::Visual::Property::TYPE, type);
    DALI_TEST_CHECK(!factory.CreateVisual(properties));
    properties.Insert(UiIntegration::ImageVisual::Property::URL, "missing.png");
    DALI_TEST_CHECK(factory.CreateVisual(properties));
  }

  Property::Map invalid;
  invalid.Insert(UiIntegration::Visual::Property::TYPE, UiIntegration::InternalVisualType::INVALID);
  DALI_TEST_CHECK(!factory.CreateVisual(invalid));
  END_TEST;
}

int UtcDaliVisualFactoryPrecompileRoutesP(void)
{
  UiTestApplication application;
  UiIntegration::VisualFactory factory = UiIntegration::VisualFactory::Get();

  Property::Map invalid;
  invalid.Insert("shaderType", "invalid");
  DALI_TEST_CHECK(!factory.AddPrecompileShader(invalid));

  struct ShaderCase
  {
    const char* type;
    const char* flag;
  };
  const ShaderCase shaders[] = {{"color", "CUTOUT"}, {"image", "YUV_AND_RGB"}, {"text", "MULTI_COLOR"}, {"npatch", "MASKING"}};
  unsigned accepted = 0u;
  for(const auto& shaderCase : shaders)
  {
    Property::Map shader;
    Property::Map flags;
    flags.Insert(shaderCase.flag, true);
    shader.Insert("shaderType", shaderCase.type);
    shader.Insert("shaderOption", flags);
    accepted += factory.AddPrecompileShader(shader) ? 1u : 0u;
  }
  DALI_TEST_CHECK(accepted > 0u);

  Property::Map custom;
  custom.Insert("shaderType", "custom");
  custom.Insert("shaderName", "test-custom");
  custom.Insert("vertexShader", "void main() {}");
  custom.Insert("fragmentShader", "void main() {}");
  DALI_TEST_CHECK(factory.AddPrecompileShader(custom));
  factory.UsePreCompiledShader();
  factory.UsePreCompiledShader();
  END_TEST;
}
