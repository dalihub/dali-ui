/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "wireframe-visual.h"

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
const char* const POSITION_ATTRIBUTE_NAME("aPosition");
} // namespace

WireframeVisualPtr WireframeVisual::New(VisualFactoryCache& factoryCache, const Property::Map& properties)
{
  Visual::BasePtr emtptyVisual;

  return New(factoryCache, emtptyVisual, properties);
}

WireframeVisualPtr WireframeVisual::New(VisualFactoryCache& factoryCache, Visual::BasePtr actualVisual)
{
  WireframeVisualPtr wireframeVisual(new WireframeVisual(factoryCache, actualVisual));
  wireframeVisual->Initialize();
  return wireframeVisual;
}

WireframeVisualPtr WireframeVisual::New(VisualFactoryCache& factoryCache, Visual::BasePtr actualVisual,
                                        const Property::Map& properties)
{
  WireframeVisualPtr wireframeVisual(new WireframeVisual(factoryCache, actualVisual));

  // Instead of calling SetProperties, looking for the only valid property 'transform'
  Property::Value* transformValue = properties.Find(Ui::VisualBasePropertyIndex::TRANSFORM, TRANSFORM);
  Property::Map    transformMap;
  if(transformValue && transformValue->Get(transformMap))
  {
    // TODO : It might not works well if effectiveScale is not 1.0f!
    wireframeVisual->SetTransformAndSize(transformMap, Vector2::ZERO, 1.0f);
  }
  wireframeVisual->Initialize();
  return wireframeVisual;
}

WireframeVisual::WireframeVisual(VisualFactoryCache& factoryCache, Visual::BasePtr actualVisual)
: Visual::Base(factoryCache, actualVisual ? actualVisual->GetType() : Ui::InternalVisualType::WIREFRAME),
  mActualVisual(actualVisual)
{
}

WireframeVisual::~WireframeVisual()
{
}

float WireframeVisual::GetHeightForWidth(float width)
{
  if(mActualVisual)
  {
    return mActualVisual->GetHeightForWidth(width);
  }
  else
  {
    return Visual::Base::GetHeightForWidth(width);
  }
}

void WireframeVisual::GetNaturalSize(Vector2& naturalSize)
{
  if(mActualVisual)
  {
    mActualVisual->GetNaturalSize(naturalSize);
  }
  else
  {
    Visual::Base::GetNaturalSize(naturalSize);
  }
}

void WireframeVisual::DoCreatePropertyMap(Property::Map& map) const
{
  if(mActualVisual)
  {
    mActualVisual->CreatePropertyMap(map);
  }
  else
  {
    map.Clear();
    map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::InternalVisualType::WIREFRAME);
  }
}

void WireframeVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void WireframeVisual::DoSetProperties(const Property::Map& propertyMap)
{
  Property::Value* mixValue = propertyMap.Find(Ui::VisualBasePropertyIndex::MIX_COLOR, MIX_COLOR);
  if(mixValue)
  {
    Vector4 mixColor;
    mixValue->Get(mixColor);
    SetMixColor(mixColor);
  }
}

void WireframeVisual::DoSetOnScene(Actor& actor)
{
  actor.AddRenderer(mImpl->mRenderer);

  // Wireframe generated and ready to display
  ResourceReady(Ui::Visual::ResourceStatus::READY);
}

void WireframeVisual::OnInitialize()
{
  Shader shader = mFactoryCache.GetShader(VisualFactoryCache::WIREFRAME_SHADER);
  if(!shader)
  {
    shader = mFactoryCache.GenerateAndSaveShader(
      VisualFactoryCache::WIREFRAME_SHADER, SHADER_WIREFRAME_VISUAL_SHADER_VERT, SHADER_WIREFRAME_VISUAL_SHADER_FRAG);

    shader.ReserveCustomProperties(2);
    shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
    shader.RegisterUniqueProperty("visualTransformUseEffectiveScale", 1.0f);
  }

  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::WIREFRAME_GEOMETRY);
  if(!geometry)
  {
    geometry = CreateQuadWireframeGeometry();
    mFactoryCache.SaveGeometry(VisualFactoryCache::WIREFRAME_GEOMETRY, geometry);
  }

  // Create the renderer
  mImpl->mRenderer = VisualRenderer::New(geometry, shader);

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Geometry WireframeVisual::CreateQuadWireframeGeometry()
{
  const float halfWidth  = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex
  {
    Vector2 position;
  };
  QuadVertex quadVertexData[4] = {{Vector2(-halfWidth, -halfHeight)},
                                  {Vector2(halfWidth, -halfHeight)},
                                  {Vector2(halfWidth, halfHeight)},
                                  {Vector2(-halfWidth, halfHeight)}};

  Property::Map quadVertexFormat;
  quadVertexFormat[POSITION_ATTRIBUTE_NAME] = Property::VECTOR2;
  VertexBuffer quadVertices                 = VertexBuffer::New(quadVertexFormat);
  quadVertices.SetData(quadVertexData, 4);

  // Create indices
  unsigned short indexData[10] = {0, 1, 1, 2, 2, 3, 3, 0};

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(quadVertices);
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));
  geometry.SetType(Geometry::LINES);

  return geometry;
}

void WireframeVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    // Register transform properties
    mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

Visual::Base& WireframeVisual::GetVisualObject()
{
  if(mActualVisual)
  {
    return *mActualVisual.Get();
  }

  return *this;
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
