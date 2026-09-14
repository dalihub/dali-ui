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

#include <dali-ui-test-suite-utils.h>
#include <dali.h>

#define private public
#include <dali-ui-foundation/internal/visuals/mesh/obj-loader.h>
#undef private

#include <string>

using namespace Dali;
using namespace Dali::Ui::Internal;

namespace
{

bool LoadObject(ObjLoader& loader, std::string source)
{
  return loader.LoadObject(source.data(), static_cast<std::streampos>(source.size()));
}

void CheckUnitBounds(const ObjLoader& loader)
{
  DALI_TEST_EQUALS(loader.mSceneAABB.pointMin, Vector3(-0.5f, -0.5f, 0.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(loader.mSceneAABB.pointMax, Vector3(0.5f, 0.5f, 0.0f), 0.001f, TEST_LOCATION);
}

} // namespace

void utc_dali_obj_loader_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_obj_loader_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliObjLoaderInitialAndInvalidStateP(void)
{
  ObjLoader loader;

  DALI_TEST_CHECK(!loader.IsSceneLoaded());
  DALI_TEST_CHECK(!loader.IsMaterialLoaded());
  DALI_TEST_CHECK(!loader.IsTexturePresent());
  DALI_TEST_CHECK(!loader.IsDiffuseMapPresent());
  DALI_TEST_CHECK(!loader.IsNormalMapPresent());
  DALI_TEST_CHECK(!loader.IsSpecularMapPresent());

  DALI_TEST_CHECK(!LoadObject(loader, "\n# no geometry\ng empty\nusemtl none\nmtllib none.mtl\ns off\n"));
  DALI_TEST_CHECK(!loader.IsSceneLoaded());

  END_TEST;
}

int UtcDaliObjLoaderPointOnlyTriangleNormalsP(void)
{
  UiTestApplication application;
  ObjLoader         loader;

  DALI_TEST_CHECK(LoadObject(loader,
                             "\n"
                             "v 1 1 1\n"
                             "v 3 1 1\n"
                             "v 1 3 1\n"
                             "g triangle\n"
                             "f 1 2 3\n"));
  DALI_TEST_CHECK(loader.IsSceneLoaded());
  DALI_TEST_CHECK(!loader.IsTexturePresent());
  DALI_TEST_EQUALS(loader.GetCenter(), Vector3::ZERO, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(loader.GetSize(), Vector3(1.0f, 1.0f, 0.0f), 0.001f, TEST_LOCATION);
  CheckUnitBounds(loader);

  Geometry geometry = loader.CreateGeometry(0, false);
  DALI_TEST_CHECK(geometry);
  DALI_TEST_EQUALS(geometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(loader.mNormals.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(loader.mNormals[0u], Vector3::ZAXIS, 0.001f, TEST_LOCATION);

  loader.ClearArrays();
  DALI_TEST_CHECK(!loader.IsSceneLoaded());
  DALI_TEST_EQUALS(loader.mPoints.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(loader.mTriangles.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliObjLoaderQuadWithCompleteVertexFrameP(void)
{
  UiTestApplication application;
  ObjLoader         loader;

  DALI_TEST_CHECK(LoadObject(loader,
                             "\n"
                             "v 0 0 0\n"
                             "v 2 0 0\n"
                             "v 2 2 0\n"
                             "v 0 2 0\n"
                             "vt 0 0\n"
                             "vt 1 0\n"
                             "vt 1 1\n"
                             "vt 0 1\n"
                             "#_#vt1 0.25 0.75\n"
                             "vn 0 0 1\n"
                             "vn 0 0 1\n"
                             "vn 0 0 1\n"
                             "vn 0 0 1\n"
                             "#_#tangent 1 0 0\n"
                             "#_#tangent 1 0 0\n"
                             "#_#tangent 1 0 0\n"
                             "#_#tangent 1 0 0\n"
                             "#_#binormal 0 1 0\n"
                             "#_#binormal 0 1 0\n"
                             "#_#binormal 0 1 0\n"
                             "#_#binormal 0 1 0\n"
                             "mtllib square.mtl\n"
                             "usemtl square\n"
                             "s 1\n"
                             "f 1/1/1 2/2/2 3/3/3 4/4/4\n"));
  DALI_TEST_CHECK(loader.IsTexturePresent());
  DALI_TEST_EQUALS(loader.mTriangles.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(loader.mTextures2.Count(), 1u, TEST_LOCATION);

  std::string material =
    "\n"
    "newmtl square\n"
    "Ka 0.1 0.2 0.3\n"
    "Kd 0.4 0.5 0.6\n"
    "Ks 0.7 0.8 0.9\n"
    "Tf 1 1 1\n"
    "Ni 1.5\n"
    "map_Kd diffuse.png\n"
    "bump normal.png\n"
    "map_Ks gloss.png\n";
  std::string diffuse;
  std::string normal;
  std::string gloss;
  loader.LoadMaterial(material.data(), static_cast<std::streampos>(material.size()), diffuse, normal, gloss);

  DALI_TEST_CHECK(loader.IsMaterialLoaded());
  DALI_TEST_CHECK(loader.IsDiffuseMapPresent());
  DALI_TEST_CHECK(loader.IsNormalMapPresent());
  DALI_TEST_CHECK(loader.IsSpecularMapPresent());
  DALI_TEST_EQUALS(diffuse, "diffuse.png", TEST_LOCATION);
  DALI_TEST_EQUALS(normal, "normal.png", TEST_LOCATION);
  DALI_TEST_EQUALS(gloss, "gloss.png", TEST_LOCATION);

  Geometry geometry = loader.CreateGeometry(ObjLoader::TEXTURE_COORDINATES | ObjLoader::TANGENTS | ObjLoader::BINORMALS, true);
  DALI_TEST_CHECK(geometry);
  DALI_TEST_EQUALS(geometry.GetNumberOfVertexBuffers(), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliObjLoaderPointNormalAndPointTextureFormatsP(void)
{
  UiTestApplication application;

  ObjLoader normalLoader;
  DALI_TEST_CHECK(LoadObject(normalLoader,
                             "\n"
                             "v 0 0 0\n"
                             "v 1 0 0\n"
                             "v 0 1 0\n"
                             "vn 0 0 1\n"
                             "f 1//1 2//1 3//1\n"));
  DALI_TEST_CHECK(!normalLoader.IsTexturePresent());
  DALI_TEST_EQUALS(normalLoader.mNormals.Count(), 1u, TEST_LOCATION);
  Geometry normalGeometry = normalLoader.CreateGeometry(0, true);
  DALI_TEST_EQUALS(normalGeometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION);

  ObjLoader textureLoader;
  DALI_TEST_CHECK(LoadObject(textureLoader,
                             "\n"
                             "v 0 0 0\n"
                             "v 1 0 0\n"
                             "v 0 1 0\n"
                             "vt 0 0\n"
                             "vt 1 0\n"
                             "vt 0 1\n"
                             "f 1/1 2/2 3/3\n"));
  DALI_TEST_CHECK(textureLoader.IsTexturePresent());
  DALI_TEST_EQUALS(textureLoader.mNormals.Count(), 0u, TEST_LOCATION);

  Geometry textureGeometry = textureLoader.CreateGeometry(ObjLoader::TANGENTS | ObjLoader::BINORMALS, false);
  DALI_TEST_EQUALS(textureGeometry.GetNumberOfVertexBuffers(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureLoader.mNormals.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureLoader.mTangents.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureLoader.mBiTangents.Count(), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliObjLoaderExpandsMismatchedAttributeMapsP(void)
{
  ObjLoader loader;

  DALI_TEST_CHECK(LoadObject(loader,
                             "\n"
                             "v 0 0 0\n"
                             "v 1 0 0\n"
                             "v 0 1 0\n"
                             "vt 0 0\n"
                             "f 1/1 2/1 3/1\n"));

  Dali::Vector<ObjLoader::Vertex>    vertices;
  Dali::Vector<Vector2>              textures;
  Dali::Vector<ObjLoader::VertexExt> verticesExt;
  Dali::Vector<unsigned short>       indices;
  loader.CreateGeometryArray(vertices, textures, verticesExt, indices, true);

  DALI_TEST_EQUALS(vertices.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(textures.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(verticesExt.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(indices.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(textures[0u], Vector2(0.0f, 1.0f), 0.001f, TEST_LOCATION);

  END_TEST;
}
