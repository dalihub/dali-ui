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

#include <algorithm>
#include <cstdint>
#include <string>

#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>

#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/visuals/streamed-image/streamed-image-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>

#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include "../dali-test-suite-utils/test-native-image.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
using Ui::Internal::StreamedImageVisual;

StreamedImageVisual::Frame MakeFrame(Texture texture)
{
  StreamedImageVisual::Frame frame;
  frame.texture     = texture;
  frame.contentSize = Vector2(512.0f, 256.0f);
  return frame;
}

Ui::Internal::StreamedImageVisualPtr NewVisual(const StreamedImageVisual::Frame& frame)
{
  Ui::Integration::VisualFactory factoryHandle = Ui::Integration::VisualFactory::Get();
  Ui::Internal::VisualFactory&   factory       = GetImplementation(factoryHandle);
  return StreamedImageVisual::New(factory.GetFactoryCache(), frame);
}

std::string GetVertexShaderSource(Renderer renderer)
{
  Property::Value value = renderer.GetShader().GetProperty(Shader::Property::PROGRAM);
  Property::Map*  map   = value.GetMap();
  DALI_TEST_CHECK(map);
  Property::Value* vertexValue = map->Find("vertex");
  DALI_TEST_CHECK(vertexValue);
  Dali::String vertexSource;
  DALI_TEST_CHECK(vertexValue->Get(vertexSource));
  return std::string(vertexSource.CStr());
}

Vector2 MapTextureCoordinate(const Vector4& pixelArea, bool swapAxes, Vector2 point)
{
  if(swapAxes)
  {
    std::swap(point.x, point.y);
  }
  return Vector2(pixelArea.x + pixelArea.z * point.x, pixelArea.y + pixelArea.w * point.y);
}

} // namespace

int UtcDaliInternalStreamedImageVisualTransform(void)
{
  UiTestApplication application;
  tet_infoline("Check the pixel area and UV swap for every rotation and flip combination");

  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  auto    frame   = MakeFrame(texture);
  frame.crop      = Rect<float>(0.25f, 0.125f, 0.5f, 0.25f);

  auto visual = NewVisual(frame);

  View                          control = View::New();
  Ui::Integration::Visual::Base handle(visual.Get());
  const Property::Index         index = control.RegisterProperty("streamedImageContent", true);
  Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(control)).RegisterVisual(index, handle, Ui::Integration::DepthIndex::CONTENT);
  control.SetRequestedWidth(200.0f);
  control.SetRequestedHeight(200.0f);
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();

  using Rotation             = StreamedImageVisual::Rotation;
  const Rotation rotations[] = {Rotation::DEGREE_0, Rotation::DEGREE_90, Rotation::DEGREE_180, Rotation::DEGREE_270};
  // Expected unrotated source corners for the display corners TL, TR, BL, BR.
  const Vector2 expectedCorners[][4] = {
    {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f)},
    {Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f)},
    {Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f)},
    {Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f)},
  };
  const Vector2 displayCorners[4] = {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f)};

  for(uint32_t rotationIndex = 0u; rotationIndex < 4u; ++rotationIndex)
  {
    for(uint32_t flips = 0u; flips < 4u; ++flips)
    {
      frame.rotation       = rotations[rotationIndex];
      frame.horizontalFlip = (flips & 1u) != 0u;
      frame.verticalFlip   = (flips & 2u) != 0u;
      visual->SetFrame(frame);
      application.SendNotification();
      application.Render();

      DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
      Renderer renderer = control.GetRendererAt(0u);

      const bool expectedSwap = rotationIndex == 1u || rotationIndex == 3u;
      const bool actualSwap   = GetVertexShaderSource(renderer).find("#define IS_REQUIRED_SWAP_UV") != std::string::npos;
      DALI_TEST_EQUALS(actualSwap, expectedSwap, TEST_LOCATION);

      const Vector4 pixelArea = renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("pixelArea"));
      for(uint32_t corner = 0u; corner < 4u; ++corner)
      {
        // The flips reflect the display coordinate before the rotation table applies.
        Vector2 displayPoint = displayCorners[corner];
        if(frame.horizontalFlip)
        {
          displayPoint.x = 1.0f - displayPoint.x;
        }
        if(frame.verticalFlip)
        {
          displayPoint.y = 1.0f - displayPoint.y;
        }
        uint32_t      lookup   = (displayPoint.x > 0.5f ? 1u : 0u) + (displayPoint.y > 0.5f ? 2u : 0u);
        const Vector2 source   = expectedCorners[rotationIndex][lookup];
        const Vector2 expected = Vector2(frame.crop.x + source.x * frame.crop.width, frame.crop.y + source.y * frame.crop.height);
        DALI_TEST_EQUALS(MapTextureCoordinate(pixelArea, actualSwap, displayCorners[corner]), expected, 0.0001f, TEST_LOCATION);
      }
    }
  }

  END_TEST;
}

int UtcDaliInternalStreamedImageVisualNaturalSize(void)
{
  UiTestApplication application;
  tet_infoline("Check the natural size against the crop and the rotation");

  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  auto    frame   = MakeFrame(texture);

  auto                          visual = NewVisual(frame);
  Ui::Integration::Visual::Base handle(visual.Get());

  Vector2 naturalSize;
  handle.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(512.0f, 256.0f), TEST_LOCATION);

  frame.crop = Rect<float>(0.25f, 0.125f, 0.5f, 0.25f);
  visual->SetFrame(frame);
  handle.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(256.0f, 64.0f), TEST_LOCATION);

  frame.rotation = StreamedImageVisual::Rotation::DEGREE_90;
  visual->SetFrame(frame);
  handle.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(64.0f, 256.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalStreamedImageVisualFrameUpdate(void)
{
  UiTestApplication application;
  tet_infoline("Check that frame updates replace the texture but keep the shader when possible");

  Texture textureA = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  Texture textureB = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  auto    frame    = MakeFrame(textureA);

  auto visual = NewVisual(frame);
  DALI_TEST_CHECK(visual->GetFrame().texture == textureA);

  View                          control = View::New();
  Ui::Integration::Visual::Base handle(visual.Get());
  const Property::Index         index = control.RegisterProperty("streamedImageContent", true);
  Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(control)).RegisterVisual(index, handle, Ui::Integration::DepthIndex::CONTENT);
  control.SetRequestedWidth(200.0f);
  control.SetRequestedHeight(200.0f);
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
  Renderer     renderer      = control.GetRendererAt(0u);
  const Shader initialShader = renderer.GetShader();

  // The same texture presented again keeps both the texture and the shader.
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == textureA);
  DALI_TEST_CHECK(renderer.GetShader() == initialShader);

  // Another plain texture replaces the texture but never the shader.
  frame.texture = textureB;
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == textureB);
  DALI_TEST_CHECK(renderer.GetShader() == initialShader);

  // A rotation that swaps the UV axes requires a new shader.
  frame.rotation = StreamedImageVisual::Rotation::DEGREE_270;
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(renderer.GetShader() != initialShader);
  DALI_TEST_CHECK(GetVertexShaderSource(renderer).find("#define IS_REQUIRED_SWAP_UV") != std::string::npos);

  // Leaving the scene removes the renderer.
  control.Unparent();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetRendererCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalStreamedImageVisualFactory(void)
{
  UiTestApplication application;
  auto              factory = Ui::Integration::VisualFactory::Get();
  Property::Map     properties;
  properties.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::STREAMED_IMAGE);
  DALI_TEST_CHECK(!factory.CreateVisual(properties));
  Property::Map named;
  named.Insert("visualType", "STREAMED_IMAGE");
  DALI_TEST_CHECK(!factory.CreateVisual(named));
  END_TEST;
}

int UtcDaliInternalStreamedImageVisualDecorations(void)
{
  UiTestApplication             application;
  auto                          visual = NewVisual(MakeFrame(Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u)));
  Ui::Integration::Visual::Base handle(visual.Get());
  Property::Map                 properties;
  handle.CreatePropertyMap(properties);
  DALI_TEST_CHECK(properties.Find(Ui::Integration::Visual::Property::TYPE));
  DALI_TEST_EQUALS(properties.Find(Ui::Integration::Visual::Property::TYPE)->Get<int>(), static_cast<int>(Ui::Integration::InternalVisualType::STREAMED_IMAGE), TEST_LOCATION);
  const Vector4 radius(24.0f, 16.0f, 8.0f, 4.0f);
  const Vector4 squareness(0.5f, 0.5f, 0.5f, 0.5f);
  Property::Map decoration;
  decoration.Insert(Ui::Integration::Visual::Property::CORNER_RADIUS, radius);
  decoration.Insert(Ui::Integration::Visual::Property::CORNER_SQUARENESS, squareness);
  decoration.Insert(Ui::Integration::Visual::Property::BORDERLINE_WIDTH, 3.0f);
  decoration.Insert(Ui::Integration::Visual::Property::BORDERLINE_COLOR, Color::RED);
  handle.DoAction(Ui::Integration::Visual::Action::UPDATE_PROPERTY, decoration);
  View control = View::New();
  control.SetRequestedWidth(200.0f);
  control.SetRequestedHeight(200.0f);
  const Property::Index index = control.RegisterProperty("streamedImageContent", true);
  Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(control)).RegisterVisual(index, handle, Ui::Integration::DepthIndex::CONTENT);
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();
  const Renderer    renderer = control.GetRendererAt(0u);
  const auto        program  = renderer.GetShader().GetProperty<Property::Map>(Shader::Property::PROGRAM);
  const std::string fragment(program.Find("fragment")->Get<Dali::String>().CStr());
  DALI_TEST_CHECK(fragment.find("#define IS_REQUIRED_SQUIRCLE_CORNER") != std::string::npos);
  DALI_TEST_CHECK(fragment.find("#define IS_REQUIRED_BORDERLINE") != std::string::npos);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), radius, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), squareness, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Color::RED, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInternalStreamedImageVisualNativeShaderReuse(void)
{
  UiTestApplication             application;
  auto                          nativeA = TestNativeImage::New(512u, 256u);
  auto                          nativeB = TestNativeImage::New(512u, 256u);
  auto                          frame   = MakeFrame(Texture::New(*nativeA));
  auto                          visual  = NewVisual(frame);
  Ui::Integration::Visual::Base handle(visual.Get());
  View                          control = View::New();
  control.SetRequestedWidth(200.0f);
  control.SetRequestedHeight(200.0f);
  const Property::Index index = control.RegisterProperty("streamedImageContent", true);
  Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(control)).RegisterVisual(index, handle, Ui::Integration::DepthIndex::CONTENT);
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();
  const Renderer renderer     = control.GetRendererAt(0u);
  const Shader   nativeShader = renderer.GetShader();
  frame.texture               = Texture::New(*nativeB);
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == frame.texture);
  DALI_TEST_CHECK(renderer.GetShader() == nativeShader);
  frame.texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  const Shader plainShader = renderer.GetShader();
  DALI_TEST_CHECK(plainShader != nativeShader);
  auto program = plainShader.GetProperty<Property::Map>(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(std::string(program.Find("fragment")->Get<Dali::String>().CStr()).find("#extension GL_OES_EGL_image_external:require") == std::string::npos);
  frame.texture = Texture::New(*nativeA);
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(renderer.GetShader() != plainShader);
  program = renderer.GetShader().GetProperty<Property::Map>(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(std::string(program.Find("fragment")->Get<Dali::String>().CStr()).find("#extension GL_OES_EGL_image_external:require") != std::string::npos);
  END_TEST;
}
