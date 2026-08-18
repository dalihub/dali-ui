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
#include <dali-ui-foundation/internal/texture-manager/texture-manager-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-url.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-test-suite-utils.h>

namespace
{
Dali::Ui::Internal::TextureManager& GetTextureManager()
{
  auto visualFactory = Dali::Ui::Integration::VisualFactory::Get();
  return Dali::Ui::GetImplementation(visualFactory).GetFactoryCache().GetTextureManager();
}

Dali::Ui::ImageUrl CreateEncodedImageUrl()
{
  Dali::EncodedImageBuffer::RawBufferType rawBuffer;
  rawBuffer.PushBack(0x11u);
  rawBuffer.PushBack(0x22u);
  rawBuffer.PushBack(0x33u);

  return Dali::Ui::ImageUrl::New(Dali::EncodedImageBuffer::New(std::move(rawBuffer)));
}

void ProcessRemoveQueue(UiTestApplication& application)
{
  application.SendNotification();
}
} // unnamed namespace

void utc_dali_image_url_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_url_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageViewSetResourceUrlKeepsImageUrlHandle(void)
{
  UiTestApplication application;

  Dali::Ui::ImageUrl                  imageUrl = CreateEncodedImageUrl();
  const Dali::Ui::Internal::VisualUrl visualUrl{imageUrl.GetUrl().CStr()};
  auto&                               textureManager = GetTextureManager();

  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::New();
  imageView.SetResourceUrl(imageUrl);
  imageUrl.Reset();

  // The ImageView must own the ImageUrl before its deferred visual is created.
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(visualUrl));

  // The string overload replaces the resource and releases the stored ImageUrl.
  imageView.SetResourceUrl(Dali::String());
  ProcessRemoveQueue(application);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(visualUrl));

  END_TEST;
}

int UtcDaliImageViewNewKeepsImageUrlHandle(void)
{
  UiTestApplication application;

  Dali::Ui::ImageUrl                  imageUrl = CreateEncodedImageUrl();
  const Dali::Ui::Internal::VisualUrl visualUrl{imageUrl.GetUrl().CStr()};
  auto&                               textureManager = GetTextureManager();

  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::New(imageUrl);
  imageUrl.Reset();

  DALI_TEST_EQUALS(imageView.GetResourceUrl(), Dali::String(visualUrl.GetUrl().c_str()), TEST_LOCATION);
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(visualUrl));

  imageView.SetResourceUrl(Dali::String());
  ProcessRemoveQueue(application);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(visualUrl));

  END_TEST;
}

int UtcDaliImageViewReplacesStoredImageUrlHandle(void)
{
  UiTestApplication application;

  Dali::Ui::ImageUrl                  first  = CreateEncodedImageUrl();
  Dali::Ui::ImageUrl                  second = CreateEncodedImageUrl();
  const Dali::Ui::Internal::VisualUrl firstUrl{first.GetUrl().CStr()};
  const Dali::Ui::Internal::VisualUrl secondUrl{second.GetUrl().CStr()};
  auto&                               textureManager = GetTextureManager();

  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::New(first);
  first.Reset();
  imageView.SetResourceUrl(second);
  second.Reset();
  ProcessRemoveQueue(application);

  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(firstUrl));
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(secondUrl));

  imageView.SetResourceUrl(Dali::String());
  ProcessRemoveQueue(application);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(secondUrl));

  END_TEST;
}
