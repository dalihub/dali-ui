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
 */

// CLASS HEADER
#include <dali-ui-foundation/internal/visuals/animated-image/fixed-image-cache.h>

// EXTERNAL HEADERS
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

#include <locale>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimImgLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ANIMATED_IMAGE");

#define LOG_CACHE                                                                                 \
  if(gAnimImgLogFilter->IsEnabledFor(Debug::Concise))                                             \
  {                                                                                               \
    std::ostringstream oss;                                                                       \
    oss.imbue(std::locale::classic());                                                            \
    if(DALI_UNLIKELY(mBatchCancelled))                                                            \
    {                                                                                             \
      oss << "Batch cancelled." << std::endl;                                                     \
    }                                                                                             \
    else                                                                                          \
    {                                                                                             \
      oss << "Size:" << mImageUrls.size() << " / Batch: " << mReadyFlags.size() << " [ ";         \
      for(std::size_t _i = 0; _i < mImageUrls.size(); ++_i)                                       \
      {                                                                                           \
        oss << _i << "={ tex:" << mTextureIds[_i];                                                \
        oss << " observer:" << (_i < mLoadObservers.size() ? mLoadObservers[_i].get() : nullptr); \
        oss << " rdy:" << ((_i < mReadyFlags.size() && mReadyFlags[_i]) ? "T" : "F") << "}, ";    \
      }                                                                                           \
      oss << " ]" << std::endl;                                                                   \
    }                                                                                             \
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "%s", oss.str().c_str());                    \
  }

#else
#define LOG_CACHE
#endif

constexpr bool     ENABLE_ORIENTATION_CORRECTION(true);
constexpr uint32_t FIRST_FRAME_INDEX = 0u;
} // namespace

FixedImageCache::FixedImageCache(TextureManager&                    textureManager,
                                 ImageDimensions                    size,
                                 Dali::SamplingMode::Type           samplingMode,
                                 const ImageCache::UrlList&         urlList,
                                 TextureManager::MaskingDataPointer maskingData,
                                 ImageCache::FrameReadyObserver&    observer,
                                 uint32_t                           batchSize,
                                 uint32_t                           interval,
                                 bool                               preMultiplyOnLoad,
                                 TextureManager::ReloadPolicy       reloadPolicy)
: ImageCache(textureManager, size, samplingMode, std::move(maskingData), observer, batchSize, interval, preMultiplyOnLoad),
  mImageUrls(urlList),
  mTextureIds(urlList.size(), TextureManager::INVALID_TEXTURE_ID),
  mCurrentFrameIndex(FIRST_FRAME_INDEX),
  mBatchCancelled(false),
  mReloadPolicies(reloadPolicy == TextureManager::ReloadPolicy::CACHED ? nullptr : std::make_unique<std::vector<TextureManager::ReloadPolicy>>(urlList.size(), reloadPolicy))
{
  mReadyFlags.reserve(mImageUrls.size());
  mLoadObservers.reserve(mImageUrls.size());
}

FixedImageCache::~FixedImageCache()
{
  ClearCache();
}

TextureSet FixedImageCache::Frame(uint32_t frameIndex)
{
  TextureSet textureSet;
  if(static_cast<std::size_t>(frameIndex) >= mImageUrls.size())
  {
    DALI_LOG_ERROR("Wrong frameIndex requested.\n");
    return textureSet;
  }

  mBatchCancelled = false;

  mCurrentFrameIndex = frameIndex;

  bool batchRequested = false;

  // Make ensure that current frameIndex load requested.
  while(DALI_LIKELY(!mBatchCancelled) && mReadyFlags.size() <= static_cast<std::size_t>(frameIndex))
  {
    batchRequested = true;
    LoadBatch();
  }

  // Request batch only 1 times for this function.
  if(DALI_LIKELY(!mBatchCancelled) && !batchRequested && mReadyFlags.size() < mImageUrls.size())
  {
    LoadBatch();
  }

  if(IsFrameReady(mCurrentFrameIndex) && mLoadState != TextureManager::LoadState::LOAD_FAILED)
  {
    textureSet = GetTextureSet(mCurrentFrameIndex);
  }

  return textureSet;
}

TextureSet FixedImageCache::FirstFrame()
{
  TextureSet textureSet = Frame(FIRST_FRAME_INDEX);

  return textureSet;
}

uint32_t FixedImageCache::GetFrameInterval(uint32_t frameIndex) const
{
  return mInterval;
}

int32_t FixedImageCache::GetCurrentFrameIndex() const
{
  return static_cast<int32_t>(mCurrentFrameIndex);
}

int32_t FixedImageCache::GetTotalFrameCount() const
{
  return static_cast<int32_t>(static_cast<uint32_t>(mImageUrls.size()));
}

bool FixedImageCache::IsFrameReady(uint32_t frameIndex) const
{
  return ((static_cast<std::size_t>(frameIndex) < mReadyFlags.size()) && (mReadyFlags[frameIndex] == true));
}

void FixedImageCache::LoadBatch()
{
  // Try and load up to mBatchSize images, until the cache is filled, and clear cache called during load batch.
  // Once the cache is filled, no more images are loaded.
  for(unsigned int i = 0; i < mBatchSize && DALI_LIKELY(!mBatchCancelled) && mReadyFlags.size() < mImageUrls.size(); ++i)
  {
    uint32_t         frameIndex = static_cast<uint32_t>(mReadyFlags.size());
    const VisualUrl& url        = mImageUrls[frameIndex];

    mReadyFlags.push_back(false);
    auto requestObserver = std::make_shared<LoadRequestObserver>(*this, weak_from_this(), frameIndex);
    mLoadObservers.push_back(requestObserver);
    mLoadState = TextureManager::LoadState::LOADING;

    bool synchronousLoading = false;
    bool loadingStatus      = false;

    auto preMultiplyOnLoading = mPreMultiplyOnLoad ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                                                   : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    auto reloadPolicy = mReloadPolicies ? (*mReloadPolicies)[frameIndex] : TextureManager::ReloadPolicy::CACHED;
    if(mReloadPolicies)
    {
      (*mReloadPolicies)[frameIndex] = TextureManager::ReloadPolicy::CACHED;
      if(static_cast<std::size_t>(frameIndex) + 1u == mReloadPolicies->size())
      {
        // We dont need to keep force load required frame info anymore.
        mReloadPolicies.reset();
      }
    }

    mTextureManager.LoadTexture(url, mDesiredSize, mSamplingMode, mMaskingData, synchronousLoading, mTextureIds[frameIndex], loadingStatus, requestObserver.get(), ENABLE_ORIENTATION_CORRECTION, reloadPolicy, preMultiplyOnLoading);
  }

  LOG_CACHE;
}

TextureSet FixedImageCache::GetTextureSet(uint32_t frameIndex) const
{
  TextureSet textureSet = mTextureManager.GetTextureSet(mTextureIds[frameIndex]);
  return textureSet;
}

void FixedImageCache::MakeReady(bool wasReady, uint32_t frameIndex, bool preMultiplied)
{
  if(wasReady == false && IsFrameReady(frameIndex))
  {
    mObserver.FrameReady(GetTextureSet(frameIndex), mInterval, preMultiplied);
  }
}

void FixedImageCache::ClearCache(bool keepUnusedTexture)
{
  mBatchCancelled = true;
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    for(std::size_t i = 0u; i < mTextureIds.size(); ++i)
    {
      if(mTextureIds[i] != TextureManager::INVALID_TEXTURE_ID)
      {
        mTextureManager.RequestRemove(mTextureIds[i], i < mLoadObservers.size() ? mLoadObservers[i].get() : nullptr, keepUnusedTexture);
        mTextureIds[i] = TextureManager::INVALID_TEXTURE_ID;
      }
    }
  }
  mReadyFlags.clear();
  mLoadObservers.clear();
  mLoadState = TextureManager::LoadState::NOT_STARTED;
  // Do not clear mReloadPolicies because we need to keep whether we reload requested or not for each frame.
  if(mMaskingData)
  {
    mMaskingData->mAlphaMaskId = TextureManager::INVALID_TEXTURE_ID;
  }
}

void FixedImageCache::OnLoadResult(LoadRequestObserver& requestObserver, uint32_t frameIndex, bool loadSuccess, TextureUploadObserver::TextureInformation textureInformation)
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "FixedImageCache::OnLoadResult(textureId:%d, observer:%p, frameIndex:%d, loadSuccess:%d) start\n", textureInformation.textureId, &requestObserver, frameIndex, loadSuccess);
  LOG_CACHE;

  if(DALI_UNLIKELY(mBatchCancelled))
  {
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "FixedImageCache::OnLoadResult() batch cancelled\n");
    return;
  }

  DALI_ASSERT_ALWAYS(static_cast<std::size_t>(frameIndex) < mLoadObservers.size() && "Invalid frameIndex received in OnLoadResult");

  if(mLoadObservers[frameIndex].get() != &requestObserver)
  {
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "FixedImageCache::OnLoadResult() Observer mismatched (has : %p but caller : %p)\n", mLoadObservers[frameIndex].get(), &requestObserver);
    return;
  }
  mLoadObservers[frameIndex].reset();

  if(loadSuccess)
  {
    mLoadState                = TextureManager::LoadState::LOAD_FINISHED;
    bool wasCurrentFrameReady = IsFrameReady(mCurrentFrameIndex);
    mTextureIds[frameIndex]   = textureInformation.textureId;
    mReadyFlags[frameIndex]   = true;
    MakeReady(wasCurrentFrameReady, mCurrentFrameIndex, TextureManager::IsAlphaPreMultiplied(textureInformation.textureSet));
  }
  else
  {
    mLoadState = TextureManager::LoadState::LOAD_FAILED;
    // preMultiplied should be false because broken image don't premultiply alpha on load
    mObserver.FrameReady(TextureSet(), 0, false);
  }
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
