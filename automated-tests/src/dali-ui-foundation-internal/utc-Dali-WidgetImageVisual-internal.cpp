/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <dali-ui-test-suite-utils.h>
#include <test-native-image.h>
#include <vector>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#define private public
#define protected public
#include <dali-ui-foundation/internal/visuals/image/image-visual.h>
#undef protected
#undef private

using namespace Dali;

void utc_dali_widget_image_visual_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_widget_image_visual_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
Ui::Integration::Visual::Base CreateWidgetVisual(Texture texture)
{
  auto url = Ui::ImageUrl::New(texture, true);
  Property::Map map;
  map.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  map.Insert(Ui::Integration::ImageVisual::Property::URL, url.GetUrl());
  map.Insert(Ui::Integration::ImageVisual::Property::RELEASE_POLICY, Ui::Image::ReleasePolicy::DESTROYED);
  return Ui::Integration::VisualFactory::Get().CreateVisual(map);
}

Ui::Internal::ImageVisual& ImageImpl(Ui::Integration::Visual::Base visual)
{
  return static_cast<Ui::Internal::ImageVisual&>(Ui::GetImplementation(visual));
}

void ReplaceWidgetVisual(Ui::Integration::Visual::Base& visual, Actor actor, Texture texture)
{
  auto next = CreateWidgetVisual(texture);
  ImageImpl(visual).SetOffScene(actor);
  visual = next;
  visual.SetOnScene(actor);
}
}

int UtcDaliWidgetImageVisualReattachWithoutNewFrame(void)
{
  UiTestApplication application;
  auto first = TestNativeImage::New(64, 64);
  auto visual = CreateWidgetVisual(Texture::New(*first));
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  visual.SetOnScene(actor);
  application.SendNotification();
  application.Render();
  auto renderer = visual.GetRenderer();
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

  // First frame and later frames must both survive a detach with no delivery.
  Vector2 expectedSize(64.0f, 64.0f);
  for(int frame = 0; frame < 3; ++frame)
  {
    renderer = visual.GetRenderer();
    auto texture = renderer.GetTextures().GetTexture(0);
    ImageImpl(visual).SetOffScene(actor);
    application.SendNotification();
    application.Render();
    visual.SetOnScene(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(visual.GetRenderer() == renderer);
    DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0) == texture);
    Vector2 naturalSize;
    visual.GetNaturalSize(naturalSize);
    DALI_TEST_EQUALS(naturalSize, expectedSize, TEST_LOCATION);

    // Alternate aspect ratios to catch stale size metadata after replacement.
    expectedSize = frame % 2 == 0 ? Vector2(128.0f, 32.0f) : Vector2(32.0f, 128.0f);
    auto next = TestNativeImage::New(static_cast<uint32_t>(expectedSize.x), static_cast<uint32_t>(expectedSize.y));
    ReplaceWidgetVisual(visual, actor, Texture::New(*next));
    DALI_TEST_CHECK(visual.GetRenderer() != renderer);
    visual.GetNaturalSize(naturalSize);
    DALI_TEST_EQUALS(naturalSize, expectedSize, TEST_LOCATION);
    DALI_TEST_EQUALS(visual.GetHeightForWidth(128.0f), frame % 2 == 0 ? 32.0f : 512.0f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliWidgetImageVisualCurrentNativeShader(void)
{
  UiTestApplication application;
  auto first = TestNativeImage::New(64, 64);
  auto visual = CreateWidgetVisual(Texture::New(*first));
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  visual.SetOnScene(actor);
  application.SendNotification();
  application.Render();
  for(int frame = 0; frame < 3; ++frame)
  {
    auto current = TestNativeImage::New(64, 64);
    auto texture = Texture::New(*current);
    ReplaceWidgetVisual(visual, actor, texture);
    DALI_TEST_CHECK(ImageImpl(visual).mNativeTexture == texture);
    current->mCallStack.Reset();
    current->mCallStack.Enable(true);
    // Force a custom shader rebuild, bypassing the standard shader cache.
    Property::Map shader;
    shader.Insert("fragmentShader", "void main() { gl_FragColor = texture2D(sTexture, vTexCoord); }");
    Property::Map properties;
    properties.Insert(Ui::Integration::Visual::Property::SHADER, shader);
    visual.SetProperties(properties);
    ImageImpl(visual).UpdateShader();
    DALI_TEST_CHECK(current->mCallStack.FindMethod("ApplyNativeFragmentShader"));
  }
  END_TEST;
}

int UtcDaliWidgetImageVisualReleasesOriginalTexture(void)
{
  UiTestApplication application;
  auto first = TestNativeImage::New(64, 64);
  auto original = Texture::New(*first);
  auto visual = CreateWidgetVisual(original);
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  visual.SetOnScene(actor);
  application.SendNotification();
  application.Render();
  auto current = TestNativeImage::New(64, 64);
  ReplaceWidgetVisual(visual, actor, Texture::New(*current));
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(original.GetBaseObject().ReferenceCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetImageVisualReplacementBeforeRendering(void)
{
  UiTestApplication application;
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  auto first = TestNativeImage::New(64, 64);
  std::vector<Texture> textures{Texture::New(*first)};
  auto visual = CreateWidgetVisual(textures.back());
  visual.SetOnScene(actor);
  for(int frame = 0; frame < 32; ++frame)
  {
    auto image = TestNativeImage::New(64, 64);
    textures.push_back(Texture::New(*image));
    ReplaceWidgetVisual(visual, actor, textures.back());
  }
  for(int frame = 0; frame < 4; ++frame)
  {
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(visual.GetRenderer().GetTextures().GetTexture(0u) == textures.back());
  for(size_t frame = 0; frame + 1 < textures.size(); ++frame)
  {
    DALI_TEST_EQUALS(textures[frame].GetBaseObject().ReferenceCount(), 1, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliWidgetImageVisualClearEffect(void)
{
  UiTestApplication application;
  auto first = TestNativeImage::New(64, 64);
  auto visual = CreateWidgetVisual(Texture::New(*first));
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  visual.SetOnScene(actor);
  application.SendNotification();
  application.Render();
  auto current = TestNativeImage::New(64, 64);
  auto texture = Texture::New(*current);
  ReplaceWidgetVisual(visual, actor, texture);
  Property::Map shader;
  shader.Insert("fragmentShader", "void main() { gl_FragColor = texture2D(sTexture, vTexCoord); }");
  Property::Map properties;
  properties.Insert(Ui::Integration::Visual::Property::SHADER, shader);
  visual.SetProperties(properties);
  DALI_TEST_CHECK(ImageImpl(visual).IsUsingCustomShader());
  properties.Clear();
  properties.Insert(Ui::Integration::Visual::Property::SHADER, Property::Array());
  visual.SetProperties(properties);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!ImageImpl(visual).IsUsingCustomShader());
  DALI_TEST_CHECK(ImageImpl(visual).mNativeTexture == texture);
  DALI_TEST_CHECK(visual.GetRenderer().GetTextures().GetTexture(0u) == texture);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetImageVisualCreationFailureReleasesTexture(void)
{
  UiTestApplication application;
  for(bool renderBeforeRelease : {false, true})
  {
    // Keep the NativeImage alive, as the engine's buffer queue does. Resource
    // cleanup must not depend on destruction of the NativeImage object itself.
    auto image = TestNativeImage::New(16, 16);
    {
      auto url = Ui::ImageUrlUtils::GenerateUrl(image, true);
      Property::Map properties;
      // Force factory rejection after external texture registration.
      properties.Insert(Ui::Integration::Visual::Property::TYPE, -1);
      properties.Insert(Ui::Integration::ImageVisual::Property::URL, url.GetUrl());
      auto visual = Ui::Integration::VisualFactory::Get().CreateVisual(properties);
      DALI_TEST_CHECK(!visual);
      if(renderBeforeRelease)
      {
        application.SendNotification();
        application.Render();
      }
      // Release the URL without creating a second, fallback Texture.
    }
    for(int frame = 0; frame < 4; ++frame)
    {
      application.SendNotification();
      application.Render();
    }
    DALI_TEST_EQUALS(image->mExtensionDestroyCalls, 1, TEST_LOCATION);
  }
  END_TEST;
}
