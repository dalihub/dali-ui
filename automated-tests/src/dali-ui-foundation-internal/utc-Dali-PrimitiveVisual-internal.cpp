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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/visuals/primitive/primitive-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#undef protected
#undef private

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

namespace
{

VisualFactoryCache& GetFactoryCache()
{
  auto visualFactory = Ui::Integration::VisualFactory::Get();
  return GetImplementation(visualFactory).GetFactoryCache();
}

PrimitiveVisualPtr CreatePrimitiveVisual()
{
  return PrimitiveVisualPtr(new PrimitiveVisual(GetFactoryCache()));
}

} // namespace

void utc_dali_primitive_visual_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_primitive_visual_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPrimitiveVisualPropertyDefaultsAndClampingP(void)
{
  UiTestApplication application;
  auto              visual = CreatePrimitiveVisual();

  DALI_TEST_EQUALS(visual->mPrimitiveType, Ui::Integration::PrimitiveVisual::Shape::SPHERE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mSlices, 128, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mStacks, 128, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mScaleDimensions, Vector3::ONE, 0.001f, TEST_LOCATION);

  Property::Map properties;
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SHAPE, "BEVELLED_CUBE");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::MIX_COLOR, Vector3(0.1f, 0.2f, 0.3f));
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SLICES, 1);
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::STACKS, 300);
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_TOP_RADIUS, 0.25f);
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, 0.75f);
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_HEIGHT, 2.0f);
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_RADIUS, 0.5f);
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_DIMENSIONS, Vector3(-1.0f, 0.0f, 3.0f));
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::BEVEL_PERCENTAGE, 2.0f);
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::BEVEL_SMOOTHNESS, -1.0f);
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::LIGHT_POSITION, Vector3(1.0f, 2.0f, 3.0f));
  visual->DoSetProperties(properties);

  DALI_TEST_EQUALS(visual->mPrimitiveType, Ui::Integration::PrimitiveVisual::Shape::BEVELLED_CUBE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mSlices, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mStacks, 255, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mScaleTopRadius, 0.25f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mScaleBottomRadius, 0.75f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mScaleHeight, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mScaleRadius, 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mScaleDimensions, Vector3(1.0f, 1.0f, 3.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mBevelPercentage, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mBevelSmoothness, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mLightPosition, Vector3(1.0f, 2.0f, 3.0f), 0.001f, TEST_LOCATION);

  Property::Map result;
  visual->DoCreatePropertyMap(result);
  DALI_TEST_EQUALS(result.Count(), 13u, TEST_LOCATION);
  DALI_TEST_CHECK(result.Find(Ui::Integration::PrimitiveVisual::Property::SHAPE));

  Property::Map instanceProperties;
  instanceProperties.Insert(1, 1);
  visual->DoCreateInstancePropertyMap(instanceProperties);
  DALI_TEST_EQUALS(instanceProperties.Count(), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPrimitiveVisualInvalidPropertiesP(void)
{
  UiTestApplication application;
  auto              visual = CreatePrimitiveVisual();

  Property::Map properties;
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::MIX_COLOR, Vector4(0.2f, 0.3f, 0.4f, 0.5f));
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SLICES, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::STACKS, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_TOP_RADIUS, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_HEIGHT, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_RADIUS, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::SCALE_DIMENSIONS, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::BEVEL_PERCENTAGE, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::BEVEL_SMOOTHNESS, "invalid");
  properties.Insert(Ui::Integration::PrimitiveVisual::Property::LIGHT_POSITION, "invalid");
  visual->DoSetProperties(properties);

  DALI_TEST_EQUALS(visual->mSlices, 128, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mStacks, 128, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mLightPosition, Vector3::ZERO, 0.001f, TEST_LOCATION);

  Property::Map upperClamp;
  upperClamp.Insert(Ui::Integration::PrimitiveVisual::Property::SLICES, 500);
  upperClamp.Insert(Ui::Integration::PrimitiveVisual::Property::STACKS, 1);
  upperClamp.Insert(Ui::Integration::PrimitiveVisual::Property::BEVEL_PERCENTAGE, -1.0f);
  upperClamp.Insert(Ui::Integration::PrimitiveVisual::Property::BEVEL_SMOOTHNESS, 2.0f);
  visual->DoSetProperties(upperClamp);

  DALI_TEST_EQUALS(visual->mSlices, 255, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mStacks, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mBevelPercentage, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mBevelSmoothness, 1.0f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPrimitiveVisualCircleAndSphereGeometryP(void)
{
  UiTestApplication application;
  auto              visual = CreatePrimitiveVisual();

  Vector<float> sine;
  Vector<float> cosine;
  visual->ComputeCircleTables(sine, cosine, -1, false);
  DALI_TEST_EQUALS(sine.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(cosine.Count(), 0u, TEST_LOCATION);

  visual->ComputeCircleTables(sine, cosine, 4, false);
  DALI_TEST_EQUALS(sine.Count(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(sine[1], 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(cosine[2], -1.0f, 0.001f, TEST_LOCATION);

  visual->ComputeCircleTables(sine, cosine, 4, true);
  DALI_TEST_EQUALS(sine[2], 1.0f, 0.001f, TEST_LOCATION);

  Vector<PrimitiveVisual::Vertex> vertices;
  Vector<unsigned short>          indices;
  visual->CreateSphere(vertices, indices, 4, 3);
  DALI_TEST_EQUALS(vertices.Count(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(indices.Count(), 48u, TEST_LOCATION);
  DALI_TEST_EQUALS(vertices[0].position, Vector3(0.0f, 0.5f, 0.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vertices[9].normal, Vector3(0.0f, -1.0f, 0.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mObjectDimensions, Vector3::ONE, 0.001f, TEST_LOCATION);

  indices.Clear();
  visual->FormSphereTriangles(indices, 4, 1);
  DALI_TEST_EQUALS(indices.Count(), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPrimitiveVisualConicGeometryVariantsP(void)
{
  UiTestApplication application;
  auto              visual = CreatePrimitiveVisual();

  Vector<PrimitiveVisual::Vertex> vertices;
  Vector<unsigned short>          indices;
  visual->CreateConic(vertices, indices, 1.0f, 0.5f, 2.0f, 4);
  DALI_TEST_EQUALS(vertices.Count(), 18u, TEST_LOCATION);
  DALI_TEST_EQUALS(indices.Count(), 48u, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mObjectDimensions, Vector3::ONE, 0.001f, TEST_LOCATION);

  vertices.Clear();
  indices.Clear();
  visual->CreateConic(vertices, indices, 0.0f, 1.0f, 4.0f, 4);
  DALI_TEST_EQUALS(vertices.Count(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(indices.Count(), 24u, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mObjectDimensions, Vector3(0.5f, 1.0f, 0.5f), 0.001f, TEST_LOCATION);

  vertices.Clear();
  indices.Clear();
  visual->CreateConic(vertices, indices, 1.0f, 0.0f, 2.0f, 4);
  DALI_TEST_EQUALS(vertices.Count(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(indices.Count(), 24u, TEST_LOCATION);

  indices.Clear();
  visual->FormConicTriangles(indices, 0.0f, 0.0f, 4);
  DALI_TEST_EQUALS(indices.Count(), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPrimitiveVisualCubeGeometryVariantsP(void)
{
  UiTestApplication application;
  auto              visual = CreatePrimitiveVisual();

  Vector<PrimitiveVisual::Vertex> vertices;
  Vector<unsigned short>          indices;
  visual->CreateBevelledCube(vertices, indices, Vector3(2.0f, 4.0f, 1.0f), 0.0f, 0.0f);
  DALI_TEST_EQUALS(vertices.Count(), 24u, TEST_LOCATION);
  DALI_TEST_EQUALS(indices.Count(), 36u, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mObjectDimensions, Vector3(0.5f, 1.0f, 0.25f), 0.001f, TEST_LOCATION);

  vertices.Clear();
  indices.Clear();
  visual->CreateBevelledCube(vertices, indices, Vector3::ONE, 1.0f, 1.0f);
  DALI_TEST_EQUALS(vertices.Count(), 24u, TEST_LOCATION);
  DALI_TEST_EQUALS(indices.Count(), 24u, TEST_LOCATION);
  DALI_TEST_EQUALS(vertices[0].normal, Vector3::YAXIS, 0.001f, TEST_LOCATION);

  vertices.Clear();
  indices.Clear();
  visual->CreateBevelledCube(vertices, indices, Vector3::ONE, 0.25f, 0.5f);
  DALI_TEST_EQUALS(vertices.Count(), 96u, TEST_LOCATION);
  DALI_TEST_EQUALS(indices.Count(), 132u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPrimitiveVisualCreatesEveryShapeP(void)
{
  UiTestApplication application;
  auto              visual = CreatePrimitiveVisual();

  for(int shape = Ui::Integration::PrimitiveVisual::Shape::SPHERE;
      shape <= Ui::Integration::PrimitiveVisual::Shape::BEVELLED_CUBE;
      ++shape)
  {
    visual->mPrimitiveType = static_cast<Ui::Integration::PrimitiveVisual::Shape::Type>(shape);
    visual->mSlices        = 4;
    visual->mStacks        = 3;
    visual->mGeometry.Reset();
    visual->CreateGeometry();
    DALI_TEST_CHECK(visual->mGeometry);
    DALI_TEST_EQUALS(visual->mGeometry.GetNumberOfVertexBuffers(), 1u, TEST_LOCATION);
  }

  Vector2 naturalSize;
  visual->mGeometry.Reset();
  visual->GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(1.0f, 1.0f), 0.001f, TEST_LOCATION);

  END_TEST;
}
