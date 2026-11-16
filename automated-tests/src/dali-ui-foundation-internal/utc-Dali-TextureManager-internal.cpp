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

// EXTERNAL INCLUDES
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/texture-manager/texture-manager-impl.h>
#include <dali-ui-foundation/internal/texture-manager/texture-upload-observer.h>

using namespace Dali;
using namespace Dali::Ui::Internal;

namespace Test
{
bool WaitForEventThreadTrigger(int triggerCount, int timeoutInSeconds, int executeCallbacks);
}

namespace
{
const char* TEST_IMAGE_FILE_NAME   = "../samples/image-view/res/sample.jpg";
const char* TEST_IMAGE_2_FILE_NAME = "../samples/image-view/res/landscape-sample.jpg";
const char* TEST_IMAGE_3_FILE_NAME = "../samples/image-view/res/placeholder_image.png";

class TestObserver : public Dali::Ui::TextureUploadObserver
{
public:
  void LoadComplete(bool loadSuccess, TextureInformation information) override
  {
    mLoaded         = loadSuccess;
    mObserverCalled = true;
    mTextureSet     = information.textureSet;
  }

  TextureSet mTextureSet;
  bool       mLoaded{false};
  bool       mObserverCalled{false};
};

class TestObserverWithCustomFunction : public TestObserver
{
public:
  void LoadComplete(bool loadSuccess, TextureInformation information) override
  {
    TestObserver::LoadComplete(loadSuccess, information);
    for(auto& signal : mSignals)
    {
      signal(nullptr);
    }
    if(!mKeepSignal)
    {
      mSignals.clear();
    }
  }

  void ConnectFunction(std::function<void(void*)> signal)
  {
    mSignals.push_back(signal);
  }

  std::vector<std::function<void(void*)>> mSignals;
  bool                                    mKeepSignal{false};
};
} // namespace

int UtcTextureManagerReloadPendingObserverDuringNotify(void)
{
  UiTestApplication                  application;
  TextureManager                     textureManager;
  TestObserverWithCustomFunction     observers[3];
  TextureManager::TextureId          textureIds[3];
  TextureManager::MaskingDataPointer maskInfo;
  auto                               preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  bool                               loading     = false;
  unsigned int                       counts[3]   = {0u, 0u, 0u};
  Texture                            firstTextures[3];
  Texture                            reloadedTexture;
  std::string                        order;

  for(unsigned int index = 0u; index < 3u; ++index)
  {
    textureIds[index]            = TextureManager::INVALID_TEXTURE_ID;
    observers[index].mKeepSignal = true;
    observers[index].ConnectFunction([&, index](void*)
    {
      ++counts[index];
      order += static_cast<char>('A' + index);
      if(index != 1u)
      {
        firstTextures[index] = observers[index].mTextureSet.GetTexture(0u);
      }
      else
      {
        reloadedTexture = observers[index].mTextureSet.GetTexture(0u);
      }
      if(index == 0u && counts[index] == 1u)
      {
        order += '[';
        textureManager.LoadTexture(VisualUrl(TEST_IMAGE_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, maskInfo, false, textureIds[1], loading, &observers[1], true, TextureManager::ReloadPolicy::FORCED, preMultiply);
        // Repeated reloads must not queue duplicate completion callbacks.
        textureManager.LoadTexture(VisualUrl(TEST_IMAGE_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, maskInfo, false, textureIds[1], loading, &observers[1], true, TextureManager::ReloadPolicy::FORCED, preMultiply);
        order += ']';
      }
    });
    textureManager.LoadTexture(VisualUrl(TEST_IMAGE_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, maskInfo, false, textureIds[index], loading, &observers[index], true, TextureManager::ReloadPolicy::CACHED, preMultiply);
  }
  DALI_TEST_EQUALS(textureIds[0], textureIds[1], TEST_LOCATION);
  DALI_TEST_EQUALS(textureIds[0], textureIds[2], TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  tet_printf("Original LoadComplete: %s (%u/%u/%u)\n", order.c_str(), counts[0], counts[1], counts[2]);
  DALI_TEST_EQUALS(order, std::string("A[]C"), TEST_LOCATION);
  for(unsigned int index = 0u; index < 3u; index += 2u)
  {
    DALI_TEST_CHECK(observers[index].mLoaded);
    DALI_TEST_CHECK(firstTextures[index]);
    DALI_TEST_EQUALS(firstTextures[index], firstTextures[0], TEST_LOCATION);
  }
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  tet_printf("Reload LoadComplete: %s (%u/%u/%u)\n", order.c_str(), counts[0], counts[1], counts[2]);
  DALI_TEST_EQUALS(order, std::string("A[]CB"), TEST_LOCATION);
  DALI_TEST_EQUALS(counts[0], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(counts[1], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(counts[2], 1u, TEST_LOCATION);
  DALI_TEST_CHECK(reloadedTexture);
  DALI_TEST_CHECK(reloadedTexture != firstTextures[0]);
  END_TEST;
}

int UtcTextureManagerQueuedObserverCancellation(void)
{
  UiTestApplication              application;
  TextureManager                 textureManager;
  TestObserver                   cachedObserver;
  TestObserverWithCustomFunction sourceObserver;
  TestObserverWithCustomFunction firstQueuedObserver;
  TestObserver                   removedObserver;
  TestObserver                   loadingObservers[2];
  auto                           destroyedObserver = std::make_unique<TestObserver>();
  auto                           preMultiply       = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  TextureManager::TextureId      cachedId          = textureManager.RequestLoad(VisualUrl(TEST_IMAGE_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, &cachedObserver, true, TextureManager::ReloadPolicy::CACHED, preMultiply);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_CHECK(cachedObserver.mLoaded);

  firstQueuedObserver.ConnectFunction([&](void*)
  {
    // These entries are later in the same vector currently being processed.
    // Neither cancellation may erase elements or invalidate that iteration.
    destroyedObserver.reset();
    textureManager.RequestRemove(cachedId, &removedObserver);
  });
  sourceObserver.ConnectFunction([&](void*)
  {
    textureManager.RequestLoad(VisualUrl(TEST_IMAGE_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, &firstQueuedObserver, true, TextureManager::ReloadPolicy::CACHED, preMultiply);
    textureManager.RequestLoad(VisualUrl(TEST_IMAGE_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, destroyedObserver.get(), true, TextureManager::ReloadPolicy::CACHED, preMultiply);
    textureManager.RequestLoad(VisualUrl(TEST_IMAGE_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, &removedObserver, true, TextureManager::ReloadPolicy::CACHED, preMultiply);
    // A different texture gets its own vector, with observers kept in order.
    for(auto& observer : loadingObservers)
    {
      textureManager.RequestLoad(VisualUrl(TEST_IMAGE_3_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, &observer, true, TextureManager::ReloadPolicy::CACHED, preMultiply);
    }
  });
  textureManager.RequestLoad(VisualUrl(TEST_IMAGE_2_FILE_NAME), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, &sourceObserver, true, TextureManager::ReloadPolicy::CACHED, preMultiply);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_CHECK(sourceObserver.mLoaded);
  DALI_TEST_CHECK(firstQueuedObserver.mLoaded);
  DALI_TEST_CHECK(!destroyedObserver);
  DALI_TEST_CHECK(!removedObserver.mObserverCalled);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_CHECK(loadingObservers[0].mLoaded);
  DALI_TEST_CHECK(loadingObservers[1].mLoaded);
  DALI_TEST_EQUALS(loadingObservers[0].mTextureSet.GetTexture(0u), loadingObservers[1].mTextureSet.GetTexture(0u), TEST_LOCATION);
  DALI_TEST_CHECK(!removedObserver.mObserverCalled);
  END_TEST;
}
