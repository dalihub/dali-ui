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

#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_image_url_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_url_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageUrlGenerateFromPath(void)
{
  UiTestApplication  application;
  const Dali::String sourceUrl{"/tmp/image.png"};

  ImageUrl imageUrl = ImageUrlUtils::GenerateUrl(sourceUrl);
  DALI_TEST_CHECK(imageUrl);
  DALI_TEST_EQUALS(imageUrl.GetUrl(), sourceUrl, TEST_LOCATION);

  ImageUrl copy = imageUrl;
  imageUrl.Reset();
  DALI_TEST_CHECK(copy);
  DALI_TEST_EQUALS(copy.GetUrl(), sourceUrl, TEST_LOCATION);

  copy.Reset();

  END_TEST;
}

int UtcDaliImageUrlMultipleImageUrls(void)
{
  UiTestApplication  application;
  const Dali::String sourceUrl{"/tmp/image.png"};

  ImageUrl first  = ImageUrlUtils::GenerateUrl(sourceUrl);
  ImageUrl second = ImageUrlUtils::GenerateUrl(sourceUrl);

  first.Reset();
  DALI_TEST_CHECK(second);
  DALI_TEST_EQUALS(second.GetUrl(), sourceUrl, TEST_LOCATION);

  second.Reset();

  END_TEST;
}

int UtcDaliImageUrlNewFromStringLiteral(void)
{
  UiTestApplication application;

  ImageUrl imageUrl = ImageUrl::New("/tmp/button.9.png");
  DALI_TEST_CHECK(imageUrl);
  DALI_TEST_EQUALS(imageUrl.GetUrl(), Dali::String("/tmp/button.9.png"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageUrlRemoteUrlRemainsUnchanged(void)
{
  UiTestApplication  application;
  const Dali::String sourceUrl{"https://example.com/image.png"};

  ImageUrl imageUrl = ImageUrl::New(sourceUrl);
  DALI_TEST_CHECK(imageUrl);
  DALI_TEST_EQUALS(imageUrl.GetUrl(), sourceUrl, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageUrlStringRejectsManagedResourceProtocols(void)
{
  UiTestApplication application;

  ImageUrl textureUrl = ImageUrl::New("dali://123");
  ImageUrl bufferUrl  = ImageUrl::New("enbuf://456.png");

  DALI_TEST_CHECK(textureUrl);
  DALI_TEST_CHECK(bufferUrl);
  DALI_TEST_EQUALS(textureUrl.GetUrl().Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(bufferUrl.GetUrl().Size(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageUrlAnimatedUrlRemainsUsable(void)
{
  UiTestApplication application;

  ImageUrl gif  = ImageUrl::New("/tmp/animated.gif");
  ImageUrl webp = ImageUrl::New("/tmp/animated.webp");
  ImageUrl json = ImageUrl::New("/tmp/animated.json");

  DALI_TEST_EQUALS(gif.GetUrl(), Dali::String("/tmp/animated.gif"), TEST_LOCATION);
  DALI_TEST_EQUALS(webp.GetUrl(), Dali::String("/tmp/animated.webp"), TEST_LOCATION);
  DALI_TEST_EQUALS(json.GetUrl(), Dali::String("/tmp/animated.json"), TEST_LOCATION);

  gif.Reset();
  webp.Reset();
  json.Reset();

  END_TEST;
}

int UtcDaliImageUrlSurvivesImageViewRecreation(void)
{
  UiTestApplication application;
  ImageUrl          imageUrl = ImageUrl::New("test.png");

  ImageView first = ImageView::New(imageUrl.GetUrl());
  application.GetScene().Add(first);
  application.SendNotification();
  application.Render();
  application.GetScene().Remove(first);
  first.Reset();

  ImageView second = ImageView::New(imageUrl.GetUrl());
  application.GetScene().Add(second);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(second.GetResourceUrl(), imageUrl.GetUrl(), TEST_LOCATION);

  application.GetScene().Remove(second);
  second.Reset();
  imageUrl.Reset();
  application.SendNotification();
  application.Render();

  END_TEST;
}
