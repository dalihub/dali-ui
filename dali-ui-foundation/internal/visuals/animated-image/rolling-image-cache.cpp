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
#include <dali-ui-foundation/internal/visuals/animated-image/rolling-image-cache.h>

// EXTERNAL HEADERS
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

#include <locale>
#include <utility>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimImgLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ANIMATED_IMAGE");

#define LOG_CACHE                                                                                                     \
  if(gAnimImgLogFilter->IsEnabledFor(Debug::Concise))                                                                 \
  {                                                                                                                   \
    std::ostringstream oss;                                                                                           \
    oss.imbue(std::locale::classic());                                                                                \
    if(DALI_UNLIKELY(mBatchCancelled))                                                                                \
    {                                                                                                                 \
      oss << "Batch cancelled." << std::endl;                                                                         \
    }                                                                                                                 \
    else                                                                                                              \
    {                                                                                                                 \
      oss << "Size:" << mQueue.Count() << " [ ";                                                                      \
      for(uint32_t _i = 0u; _i < mQueue.Count(); ++_i)                                                                \
      {                                                                                                               \
        oss << _i << "={ tex:" << mTextureIds[mQueue[_i].mUrlIndex] << " urlId:" << mQueue[_i].mUrlIndex;             \
        oss << " observer:" << mQueue[_i].mLoadObserver.get() << " rdy:" << (mQueue[_i].mReady ? "T" : "F") << "}, "; \
      }                                                                                                               \
      oss << " ]" << std::endl;                                                                                       \
    }                                                                                                                 \
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "%s", oss.str().c_str());                                        \
  }

#else
#define LOG_CACHE
#endif

static constexpr bool ENABLE_ORIENTATION_CORRECTION(true);

static constexpr uint32_t FIRST_FRAME_INDEX = 0u;

} // namespace

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
RollingImageCache::RollingImageCache(TextureManager&                    textureManager,
                                     ImageDimensions                    size,
                                     Dali::SamplingMode::Type           samplingMode,
                                     const ImageCache::UrlList&         urlList,
                                     TextureManager::MaskingDataPointer maskingData,
                                     ImageCache::FrameReadyObserver&    observer,
                                     uint16_t                           cacheSize,
                                     uint16_t                           batchSize,
                                     uint32_t                           interval,
                                     bool                               preMultiplyOnLoad,
                                     TextureManager::ReloadPolicy       reloadPolicy)
: ImageCache(textureManager, size, samplingMode, std::move(maskingData), observer, batchSize, interval, preMultiplyOnLoad),
  mImageUrls(urlList),
  mTextureIds(urlList.size(), TextureManager::INVALID_TEXTURE_ID),
  mQueue(cacheSize),
  mBatchCancelled(false),
  mReloadPolicies(reloadPolicy == TextureManager::ReloadPolicy::CACHED ? nullptr : std::make_unique<std::vector<TextureManager::ReloadPolicy>>(urlList.size(), reloadPolicy))
{
}

RollingImageCache::~RollingImageCache()
{
  ClearCache();
}

TextureSet RollingImageCache::Frame(uint32_t frameIndex)
{
  // Pop frames until the frame of frameIndex become front frame.
  bool popExist = false;
  while(!mQueue.IsEmpty() && mQueue.Front().mUrlIndex != frameIndex)
  {
    PopFrontCache();
    popExist = true;
  }

  mBatchCancelled = false;

  // TODO: synchronous loading of first frame.
  if(popExist || mQueue.IsEmpty())
  {
    uint32_t batchFrameIndex = frameIndex;
    // If the frame of frameIndex was already loaded, load batch from the last frame of queue
    if(!mQueue.IsEmpty())
    {
      batchFrameIndex = (mQueue.Back().mUrlIndex + 1) % static_cast<uint32_t>(mImageUrls.size());
    }
    LoadBatch(batchFrameIndex);
  }

  TextureSet textureSet;
  if(IsFrontReady() == true && mLoadState != TextureManager::LoadState::LOAD_FAILED)
  {
    textureSet = GetFrontTextureSet();
  }

  return textureSet;
}

TextureSet RollingImageCache::FirstFrame()
{
  TextureSet textureSet = Frame(FIRST_FRAME_INDEX);
  return textureSet;
}

uint32_t RollingImageCache::GetFrameInterval(uint32_t frameIndex) const
{
  return mInterval;
}

int32_t RollingImageCache::GetCurrentFrameIndex() const
{
  if(mQueue.IsEmpty())
  {
    return -1;
  }
  return mQueue.Front().mUrlIndex;
}

int32_t RollingImageCache::GetTotalFrameCount() const
{
  return static_cast<int32_t>(static_cast<uint32_t>(mImageUrls.size()));
}

bool RollingImageCache::IsFrontReady() const
{
  return (!mQueue.IsEmpty() && mQueue.Front().mReady);
}

void RollingImageCache::LoadBatch(uint32_t frameIndex)
{
  // Try and load up to mBatchSize images, until the cache is filled.
  // Once the cache is filled, as frames progress, the old frame is
  // cleared, but not erased, and another image is loaded
  for(unsigned int i = 0; i < mBatchSize && DALI_LIKELY(!mBatchCancelled) && !mQueue.IsFull(); ++i)
  {
    ImageFrame imageFrame;

    frameIndex = static_cast<uint32_t>(static_cast<std::size_t>(frameIndex) % mImageUrls.size());

    const VisualUrl& url     = mImageUrls[frameIndex];
    imageFrame.mUrlIndex     = frameIndex;
    imageFrame.mReady        = false;
    imageFrame.mLoadObserver = std::make_shared<LoadRequestObserver>(*this, weak_from_this(), frameIndex);

    mQueue.PushBack(imageFrame);

    mLoadState = TextureManager::LoadState::LOADING;

    bool synchronousLoading = false;
    bool loadingStatus      = false;

    auto preMultiplyOnLoading = mPreMultiplyOnLoad ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                                                   : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    auto reloadPolicy = mReloadPolicies ? (*mReloadPolicies)[frameIndex] : TextureManager::ReloadPolicy::CACHED;
    if(mReloadPolicies)
    {
      (*mReloadPolicies)[frameIndex] = TextureManager::ReloadPolicy::CACHED;
      // TODO : How can we remove mReloadPolicies when all frames are loaded?
    }

    TextureSet textureSet = mTextureManager.LoadTexture(
      url, mDesiredSize, mSamplingMode, mMaskingData, synchronousLoading, mTextureIds[imageFrame.mUrlIndex], loadingStatus, imageFrame.mLoadObserver.get(), ENABLE_ORIENTATION_CORRECTION, reloadPolicy, preMultiplyOnLoading);

    ++frameIndex;
  }
}

TextureSet RollingImageCache::GetFrontTextureSet() const
{
  TextureManager::TextureId textureId  = GetCachedTextureId(0);
  TextureSet                textureSet = mTextureManager.GetTextureSet(textureId);
  if(textureSet)
  {
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(Dali::WrapMode::Type::DEFAULT, Dali::WrapMode::Type::DEFAULT);
    textureSet.SetSampler(0u, sampler);
  }
  return textureSet;
}

TextureManager::TextureId RollingImageCache::GetCachedTextureId(uint32_t index) const
{
  return mTextureIds[mQueue[index].mUrlIndex];
}

void RollingImageCache::PopFrontCache(bool keepUnusedTexture)
{
  auto requestObserver = mQueue.Front().mLoadObserver;
  mQueue.Front().mLoadObserver.reset();
  ImageFrame imageFrame = mQueue.PopFront();

  if(mTextureIds[imageFrame.mUrlIndex] != TextureManager::INVALID_TEXTURE_ID)
  {
    mTextureManager.RequestRemove(mTextureIds[imageFrame.mUrlIndex], requestObserver.get(), keepUnusedTexture);
    mTextureIds[imageFrame.mUrlIndex] = TextureManager::INVALID_TEXTURE_ID;
  }

  if(mMaskingData && mMaskingData->mAlphaMaskId != TextureManager::INVALID_TEXTURE_ID)
  {
    if(mQueue.IsEmpty())
    {
      mMaskingData->mAlphaMaskId = TextureManager::INVALID_TEXTURE_ID;
    }
  }
}

void RollingImageCache::ClearCache(bool keepUnusedTexture)
{
  mBatchCancelled = true;
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    while(!mQueue.IsEmpty())
    {
      PopFrontCache(keepUnusedTexture);
    }
  }
  mLoadState = TextureManager::LoadState::NOT_STARTED;
}

void RollingImageCache::OnLoadResult(LoadRequestObserver& requestObserver, uint32_t frameIndex, bool loadSuccess, TextureUploadObserver::TextureInformation textureInformation)
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "RollingImageCache::OnLoadResult(textureId:%d, observer:%p, frameIndex:%d, loadSuccess:%d) start\n", textureInformation.textureId, &requestObserver, frameIndex, loadSuccess);
  LOG_CACHE;

  if(DALI_UNLIKELY(mBatchCancelled))
  {
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "RollingImageCache::OnLoadResult() batch cancelled\n");
    return;
  }

  bool frontFrameReady = IsFrontReady();
  bool requestFound    = false;
  for(uint32_t i = 0u; i < mQueue.Count(); ++i)
  {
    if(mQueue[i].mUrlIndex == frameIndex)
    {
      if(DALI_LIKELY(mQueue[i].mLoadObserver.get() == &requestObserver))
      {
        mQueue[i].mLoadObserver.reset();
        mQueue[i].mReady = loadSuccess;
        requestFound     = true;
        break;
      }
      else
      {
        DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "RollingImageCache::OnLoadResult() Observer mismatched (has : %p but caller : %p)\n", mQueue[i].mLoadObserver.get(), &requestObserver);
      }
    }
  }

  if(DALI_UNLIKELY(!requestFound))
  {
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "RollingAnimatedImageCache::OnLoadResult() Observer mismatched\n");
    return;
  }

  if(loadSuccess)
  {
    mLoadState              = TextureManager::LoadState::LOAD_FINISHED;
    mTextureIds[frameIndex] = textureInformation.textureId;

    if(!frontFrameReady && IsFrontReady())
    {
      mObserver.FrameReady(textureInformation.textureSet, mInterval, TextureManager::IsAlphaPreMultiplied(textureInformation.textureSet));
    }
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
