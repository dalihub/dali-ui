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

#include <dali-ui-foundation/integration-api/image-loader/async-image-loader-integ.h>
#include <dali-ui-foundation/internal/image-loader/async-image-loader-impl.h>
#include <dali-ui-foundation/public-api/image-loader/async-image-loader.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace Test
{
bool WaitForEventThreadTrigger(int triggerCount, int timeoutInSeconds, int executeCallbacks);
}

namespace
{
struct LoadObserver : public ConnectionTracker
{
  void OnImageLoaded(uint32_t taskId, PixelData pixels)
  {
    ++callCount;
    lastTaskId = taskId;
    loaded = static_cast<bool>(pixels);
  }

  void OnPixelBufferLoaded(uint32_t taskId, std::vector<PixelBuffer>& buffers)
  {
    ++callCount;
    lastTaskId = taskId;
    loaded = !buffers.empty() && static_cast<bool>(buffers.front());
  }

  uint32_t lastTaskId{0u};
  int callCount{0};
  bool loaded{false};
};
} // namespace

void utc_dali_async_image_loader_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_async_image_loader_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAsyncImageLoaderFileSuccessFailureAndCancelP(void)
{
  UiTestApplication application;
  AsyncImageLoader loader = AsyncImageLoader::New();
  DALI_TEST_CHECK(loader);
  DALI_TEST_CHECK(AsyncImageLoader::DownCast(loader));
  LoadObserver observer;
  loader.ImageLoadedSignal().Connect(&observer, &LoadObserver::OnImageLoaded);

  const uint32_t validId = loader.Load("../samples/image-view/res/sample.jpg");
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(observer.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastTaskId, validId, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);
  DALI_TEST_CHECK(!loader.Cancel(validId));

  const uint32_t missingId = loader.Load("../samples/image-view/res/does-not-exist.jpg", ImageDimensions(24u, 24u));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(observer.callCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastTaskId, missingId, TEST_LOCATION);
  DALI_TEST_CHECK(!observer.loaded);

  const uint32_t resizedId = loader.Load("../samples/image-view/res/sample.jpg", ImageDimensions(32u, 20u), SamplingMode::BOX_THEN_LINEAR, false);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(observer.callCount, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastTaskId, resizedId, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);
  DALI_TEST_CHECK(!loader.Cancel(999999u));
  loader.CancelAll();
  END_TEST;
}

int UtcDaliAsyncImageLoaderAnimatedFramesAndBuffersP(void)
{
  UiTestApplication application;
  AsyncImageLoader loader = AsyncImageLoader::New();
  LoadObserver observer;
  Dali::Ui::Integration::PixelBufferLoadedSignal(loader).Connect(&observer, &LoadObserver::OnPixelBufferLoaded);

  auto animation = AnimatedImageLoading::New("../samples/image-view/res/dali-logo-anim.gif", true);
  DALI_TEST_CHECK(animation);
  const uint32_t firstId = Dali::Ui::Integration::LoadAnimatedImage(loader, animation, 0u, Dali::Ui::Integration::PreMultiplyOnLoad::OFF);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(observer.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastTaskId, firstId, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);

  auto& impl = Ui::GetImplementation(loader);
  const uint32_t secondId = impl.LoadAnimatedImage(animation, 1u, ImageDimensions(32u, 32u), SamplingMode::BOX_THEN_LINEAR, Dali::Ui::Integration::PreMultiplyOnLoad::ON, false);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(observer.callCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastTaskId, secondId, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);
  END_TEST;
}

int UtcDaliAsyncImageLoaderEncodedAndMaskTasksP(void)
{
  UiTestApplication application;
  AsyncImageLoader loader = AsyncImageLoader::New();
  LoadObserver observer;
  Dali::Ui::Integration::PixelBufferLoadedSignal(loader).Connect(&observer, &LoadObserver::OnPixelBufferLoaded);
  auto& impl = Ui::GetImplementation(loader);

  EncodedImageBuffer::RawBufferType invalidBytes;
  invalidBytes.PushBack(0x22u);
  auto invalidBuffer = EncodedImageBuffer::New(std::move(invalidBytes));
  const uint32_t encodedId = impl.LoadEncodedImageBuffer(invalidBuffer, ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, true, Dali::Ui::Integration::PreMultiplyOnLoad::OFF);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(observer.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastTaskId, encodedId, TEST_LOCATION);
  DALI_TEST_CHECK(!observer.loaded);

  PixelBuffer image = PixelBuffer::New(2u, 2u, Pixel::RGBA8888);
  PixelBuffer mask = PixelBuffer::New(2u, 2u, Pixel::RGBA8888);
  const uint32_t maskedId = impl.ApplyMask(image, mask, 1.0f, false, Dali::Ui::Integration::PreMultiplyOnLoad::OFF);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(observer.callCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastTaskId, maskedId, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);
  END_TEST;
}
