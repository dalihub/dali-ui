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
#include <dali-ui-test-suite-utils.h>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/visuals/mesh/mesh-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#undef protected
#undef private

#include <cstdio>
#include <fstream>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

void utc_dali_mesh_visual_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_mesh_visual_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* OBJECT_PATH = "/tmp/dali-ui-mesh-visual-utc.obj";
const char* MATERIAL_PATH = "/tmp/dali-ui-mesh-visual-utc.mtl";

VisualFactoryCache& GetMeshFactoryCache()
{
  auto visualFactory = Ui::Integration::VisualFactory::Get();
  return GetImplementation(visualFactory).GetFactoryCache();
}

MeshVisualPtr CreateMeshVisual()
{
  return MeshVisualPtr(new MeshVisual(GetMeshFactoryCache()));
}

void WriteFile(const char* path, const char* contents)
{
  std::ofstream stream(path);
  stream << contents;
}
} // namespace

int UtcDaliMeshVisualPropertiesP(void)
{
  UiTestApplication application;
  auto visual = CreateMeshVisual();
  Property::Map properties;

  properties.Insert(Ui::Integration::MeshVisual::Property::OBJECT_URL, Dali::String("model.obj"));
  properties.Insert(Ui::Integration::MeshVisual::Property::MATERIAL_URL, Dali::String("model.mtl"));
  properties.Insert(Ui::Integration::MeshVisual::Property::TEXTURES_PATH, Dali::String("textures/"));
  properties.Insert(Ui::Integration::MeshVisual::Property::SHADING_MODE, "TEXTURED_WITH_SPECULAR_LIGHTING");
  properties.Insert(Ui::Integration::MeshVisual::Property::USE_MIPMAPPING, false);
  properties.Insert(Ui::Integration::MeshVisual::Property::USE_SOFT_NORMALS, false);
  properties.Insert(Ui::Integration::MeshVisual::Property::LIGHT_POSITION, Vector3(1.0f, 2.0f, 3.0f));
  visual->DoSetProperties(properties);

  DALI_TEST_EQUALS(visual->mObjectUrl, std::string("model.obj"), TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mMaterialUrl, std::string("model.mtl"), TEST_LOCATION);
  DALI_TEST_CHECK(!visual->mUseMipmapping);
  DALI_TEST_CHECK(!visual->mUseSoftNormals);
  DALI_TEST_EQUALS(visual->mLightPosition, Vector3(1.0f, 2.0f, 3.0f), TEST_LOCATION);

  Property::Map result;
  visual->DoCreatePropertyMap(result);
  DALI_TEST_EQUALS(result.Count(), 8u, TEST_LOCATION);
  Property::Map instance;
  instance.Insert(1, 2);
  visual->DoCreateInstancePropertyMap(instance);
  DALI_TEST_EQUALS(instance.Count(), 1u, TEST_LOCATION);

  Property::Map named;
  named.Insert("objectUrl", Dali::String("named.obj"));
  named.Insert("materialUrl", Dali::String(""));
  named.Insert("texturesPath", Dali::String("named/"));
  named.Insert("shadingMode", 0);
  named.Insert("useMipmapping", true);
  named.Insert("useSoftNormals", true);
  named.Insert("lightPosition", Vector3::ZERO);
  visual->DoSetProperties(named);
  DALI_TEST_EQUALS(visual->mObjectUrl, std::string("named.obj"), TEST_LOCATION);
  DALI_TEST_CHECK(!visual->mUseTexture);
  END_TEST;
}

int UtcDaliMeshVisualInvalidPropertiesAndGeometryFailureP(void)
{
  UiTestApplication application;
  auto visual = CreateMeshVisual();
  Property::Map invalid;
  invalid.Insert(Ui::Integration::MeshVisual::Property::OBJECT_URL, 1);
  invalid.Insert(Ui::Integration::MeshVisual::Property::MATERIAL_URL, 2);
  invalid.Insert(Ui::Integration::MeshVisual::Property::TEXTURES_PATH, 3);
  invalid.Insert(Ui::Integration::MeshVisual::Property::USE_MIPMAPPING, "bad");
  invalid.Insert(Ui::Integration::MeshVisual::Property::USE_SOFT_NORMALS, "bad");
  invalid.Insert(Ui::Integration::MeshVisual::Property::LIGHT_POSITION, "bad");
  visual->DoSetProperties(invalid);
  DALI_TEST_EQUALS(visual->mLightPosition, Vector3::ZERO, TEST_LOCATION);

  visual->mObjectUrl = "/tmp/does-not-exist-dali-ui.obj";
  visual->OnInitialize();
  DALI_TEST_CHECK(visual->mGeometry);
  DALI_TEST_CHECK(visual->mShader);
  DALI_TEST_CHECK(!visual->LoadGeometry());
  DALI_TEST_CHECK(!visual->LoadMaterial());
  END_TEST;
}

int UtcDaliMeshVisualLoadsTexturelessObjectP(void)
{
  UiTestApplication application;
  WriteFile(OBJECT_PATH,
            "v 0 0 0\n"
            "v 1 0 0\n"
            "v 0 1 0\n"
            "vn 0 0 1\n"
            "f 1//1 2//1 3//1\n");

  auto fileVisual = CreateMeshVisual();
  fileVisual->mObjectUrl = OBJECT_PATH;
  DALI_TEST_CHECK(fileVisual->LoadGeometry());

  auto visual = CreateMeshVisual();
  std::string object =
    "\n"
    "v 0 0 0\n"
    "v 1 0 0\n"
    "v 0 1 0\n"
    "f 1 2 3\n";
  DALI_TEST_CHECK(visual->mObjLoader.LoadObject(object.data(), static_cast<std::streampos>(object.size())));
  visual->mUseTexture = false;
  visual->mUseSoftNormals = false;
  DALI_TEST_CHECK(visual->CreateGeometry());
  visual->CreateShader();
  DALI_TEST_CHECK(visual->LoadTextures());

  DALI_TEST_CHECK(visual->mGeometry);
  DALI_TEST_CHECK(visual->mShader);
  DALI_TEST_CHECK(visual->mTextureSet);
  DALI_TEST_EQUALS(visual->mShadingMode,
                   Ui::Integration::MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING,
                   TEST_LOCATION);

  visual->SupplyEmptyGeometry();
  Actor actor = Actor::New();
  visual->DoSetOnScene(actor);
  visual->OnSetTransform();
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  visual->UpdateShaderUniforms(Vector2(480.0f, 800.0f));
  DALI_TEST_CHECK(visual->mLightPosition != Vector3::ZERO);

  std::remove(OBJECT_PATH);
  END_TEST;
}

int UtcDaliMeshVisualMaterialAndTextureFailuresP(void)
{
  UiTestApplication application;
  WriteFile(MATERIAL_PATH,
            "newmtl material\n"
            "map_Kd missing-diffuse.png\n"
            "bump missing-normal.png\n"
            "map_Ks missing-gloss.png\n");

  auto visual = CreateMeshVisual();
  visual->mMaterialUrl = MATERIAL_PATH;
  DALI_TEST_CHECK(visual->LoadMaterial());
  DALI_TEST_EQUALS(visual->mDiffuseTextureUrl, std::string("missing-diffuse.png"), TEST_LOCATION);

  visual->mShadingMode = Ui::Integration::MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING;
  visual->mTexturesPath = "/tmp/";
  visual->mUseMipmapping = true;
  DALI_TEST_CHECK(!visual->LoadTextures());

  visual->mDiffuseTextureUrl.clear();
  visual->mNormalTextureUrl.clear();
  visual->mGlossTextureUrl.clear();
  DALI_TEST_CHECK(visual->LoadTextures());

  visual->CreateShader();
  visual->mShadingMode = Ui::Integration::MeshVisual::ShadingMode::TEXTURED_WITH_SPECULAR_LIGHTING;
  visual->CreateShader();
  visual->mShadingMode = Ui::Integration::MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING;
  visual->CreateShader();
  std::remove(MATERIAL_PATH);
  END_TEST;
}
