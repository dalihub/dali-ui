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

// Mock implementation of Dali::VectorAnimationRenderer used by the dali-ui automated-tests.
//
// The real implementation renders Lottie/vector animations via thorvg, which may not be
// installed on the test machine. Symbol interposition (this file is compiled directly into the
// test binary, ahead of libdali2-adaptor.so on the link line) lets LottieAnimationVisual and
// SelectableLottieAnimationView tests run without invoking ThorVG, matching the approach used by
// dali-toolkit's
// dali-toolkit-test-utils/toolkit-vector-animation-renderer.cpp.

#include <dali-ui/ui-event-thread-callback.h>
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/object/base-object.h>
#include <atomic>
#include <memory>
#include <mutex>

namespace
{
std::atomic<uint32_t>                             gLastVectorAnimationWidth{0u};
std::atomic<uint32_t>                             gLastVectorAnimationHeight{0u};
std::mutex                                        gDynamicPropertyProbeMutex;
Dali::Internal::Adaptor::VectorAnimationRenderer* gLatestVectorAnimationRenderer{nullptr};
} // unnamed namespace

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class VectorAnimationRenderer : public Dali::BaseObject
{
public:
  struct DynamicPropertyRecord
  {
    std::string                                   keyPath;
    Dali::VectorAnimationRenderer::VectorProperty property;
    int32_t                                       id;
    std::unique_ptr<CallbackBase>                 callback;
    Dali::Property::Value                         lastValue;
  };

  VectorAnimationRenderer()
  : mEventThreadCallback(new EventThreadCallback(MakeCallback(this, &VectorAnimationRenderer::OnTriggered)))
  {
    std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
    gLatestVectorAnimationRenderer = this;
  }

  ~VectorAnimationRenderer() override
  {
    std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
    if(gLatestVectorAnimationRenderer == this)
    {
      gLatestVectorAnimationRenderer = nullptr;
    }
  }

  bool Load(const std::string& url)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mLoadFailed = (url == "invalid.json");
    if(!mLoadFailed)
    {
      mDefaultWidth  = 100;
      mDefaultHeight = 100;
    }
    return !mLoadFailed;
  }

  bool Load(const Dali::Vector<uint8_t>& data)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mDefaultWidth  = 100;
    mDefaultHeight = 100;
    return true;
  }

  void SetRenderer(Dali::Renderer renderer)
  {
    mRenderer = renderer;
  }

  void SetSize(uint32_t width, uint32_t height)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mWidth  = width;
    mHeight = height;
    gLastVectorAnimationWidth.store(width);
    gLastVectorAnimationHeight.store(height);

    if(!mLoadFailed && mWidth > 0 && mHeight > 0)
    {
      mNeedTrigger   = true;
      mResourceReady = false;
    }
  }

  bool Render(uint32_t frameNumber)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    if(mWidth == 0 || mHeight == 0)
    {
      return false;
    }

    const bool frameChanged = !mHasRenderedFrame || mPreviousFrame != frameNumber;
    mPreviousFrame          = frameNumber;
    mHasRenderedFrame       = true;

    // Match ThorVG: a request for the same frame does not re-evaluate dynamic properties.
    if(frameChanged)
    {
      for(auto&& record : mDynamicPropertyRecords)
      {
        record.lastValue = CallbackBase::ExecuteReturn<Property::Value>(
          *record.callback, record.id, record.property, frameNumber);
      }
    }

    if(mNeedTrigger)
    {
      mEventThreadCallback->Trigger();
      mNeedTrigger = false;
    }

    return true;
  }

  uint32_t GetTotalFrameNumber() const
  {
    return mTotalFrameNumber;
  }

  float GetFrameRate() const
  {
    return mFrameRate;
  }

  void GetDefaultSize(uint32_t& width, uint32_t& height) const
  {
    width  = mDefaultWidth;
    height = mDefaultHeight;
  }

  bool GetMarkerInfo(const std::string& marker, uint32_t& startFrame, uint32_t& endFrame) const
  {
    return false;
  }

  void InvalidateBuffer()
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    if(mResourceReady)
    {
      mNeedTrigger   = true;
      mResourceReady = false;
    }
  }

  void AddPropertyValueCallback(const std::string& keyPath, Dali::VectorAnimationRenderer::VectorProperty property, CallbackBase* callback, int32_t id)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    for(auto&& record : mDynamicPropertyRecords)
    {
      if(record.keyPath == keyPath && record.property == property)
      {
        record.id       = id;
        record.callback = std::unique_ptr<CallbackBase>(callback);
        return;
      }
    }
    mDynamicPropertyRecords.push_back(
      DynamicPropertyRecord{keyPath, property, id, std::unique_ptr<CallbackBase>(callback), Dali::Property::Value()});
  }

  void RefreshDynamicProperty()
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    ++mDynamicPropertyRefreshCount;
    for(auto&& record : mDynamicPropertyRecords)
    {
      record.lastValue = CallbackBase::ExecuteReturn<Property::Value>(
        *record.callback, record.id, record.property, mPreviousFrame);
    }
    mEventThreadCallback->Trigger();
  }

  void KeepRasterizedBuffer()
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mEnableFixedCache = true;
  }

  void SetEnableAspectFit(bool enable)
  {
    mEnableAspectFit = enable;
  }

  bool IsEnableAspectFit() const
  {
    return mEnableAspectFit;
  }

  Dali::VectorAnimationRenderer::UploadCompletedSignalType& UploadCompletedSignal()
  {
    return mUploadCompletedSignal;
  }

  uint32_t GetDynamicPropertyCount() const
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    return static_cast<uint32_t>(mDynamicPropertyRecords.size());
  }

  uint32_t GetLastRenderedFrame() const
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    return mPreviousFrame;
  }

  uint32_t GetDynamicPropertyCount(const std::string&                            keyPath,
                                   Dali::VectorAnimationRenderer::VectorProperty property) const
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    uint32_t                count = 0u;
    for(const auto& record : mDynamicPropertyRecords)
    {
      if(record.keyPath == keyPath && record.property == property)
      {
        ++count;
      }
    }
    return count;
  }

  Dali::Property::Value EvaluateDynamicProperty(const std::string&                            keyPath,
                                                Dali::VectorAnimationRenderer::VectorProperty property,
                                                uint32_t                                      frameNumber)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    for(auto& record : mDynamicPropertyRecords)
    {
      if(record.keyPath == keyPath && record.property == property)
      {
        return CallbackBase::ExecuteReturn<Property::Value>(*record.callback, record.id, record.property, frameNumber);
      }
    }
    DALI_ASSERT_ALWAYS(false && "Dynamic property was not registered");
    return Dali::Property::Value();
  }

  Dali::Property::Value GetLastEvaluatedDynamicProperty(const std::string&                            keyPath,
                                                        Dali::VectorAnimationRenderer::VectorProperty property) const
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    for(const auto& record : mDynamicPropertyRecords)
    {
      if(record.keyPath == keyPath && record.property == property)
      {
        return record.lastValue;
      }
    }
    DALI_ASSERT_ALWAYS(false && "Dynamic property was not registered");
    return Dali::Property::Value();
  }

  uint32_t GetDynamicPropertyRefreshCount() const
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    return mDynamicPropertyRefreshCount;
  }

  void OnTriggered()
  {
    if(mResourceReady)
    {
      return;
    }
    mResourceReady = true;

    Dali::TextureSet textureSet = mRenderer.GetTextures();
    if(textureSet)
    {
      Dali::Texture texture = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, mWidth, mHeight);
      textureSet.SetTexture(0, texture);

      Dali::PixelBuffer pixelBuffer = Dali::PixelBuffer::New(mWidth, mHeight, Pixel::RGBA8888);
      Dali::PixelData   pixelData   = Dali::PixelBuffer::Convert(pixelBuffer);
      texture.Upload(pixelData);
    }

    mUploadCompletedSignal.Emit();
  }

public:
  Dali::Renderer                     mRenderer;
  mutable Dali::Mutex                mMutex;
  std::vector<DynamicPropertyRecord> mDynamicPropertyRecords;

  uint32_t mWidth{0};
  uint32_t mHeight{0};
  uint32_t mDefaultWidth{0};
  uint32_t mDefaultHeight{0};
  uint32_t mTotalFrameNumber{5};
  uint32_t mPreviousFrame{0};
  uint32_t mDynamicPropertyRefreshCount{0};
  float    mFrameRate{60.0f};
  bool     mLoadFailed{false};
  bool     mResourceReady{false};
  bool     mNeedTrigger{true};
  bool     mEnableFixedCache{false};
  bool     mEnableAspectFit{true};
  bool     mHasRenderedFrame{false};

  Dali::VectorAnimationRenderer::UploadCompletedSignalType mUploadCompletedSignal;
  std::unique_ptr<EventThreadCallback>                     mEventThreadCallback;
};

inline VectorAnimationRenderer& GetImplementation(Dali::VectorAnimationRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "VectorAnimationRenderer handle is empty.");
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast<Internal::Adaptor::VectorAnimationRenderer&>(handle);
}

inline const VectorAnimationRenderer& GetImplementation(const Dali::VectorAnimationRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "VectorAnimationRenderer handle is empty.");
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast<const Internal::Adaptor::VectorAnimationRenderer&>(handle);
}

} // namespace Adaptor
} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

VectorAnimationRenderer VectorAnimationRenderer::New()
{
  Internal::Adaptor::VectorAnimationRenderer* animationRenderer = new Internal::Adaptor::VectorAnimationRenderer();
  return VectorAnimationRenderer(animationRenderer);
}

VectorAnimationRenderer::VectorAnimationRenderer()
{
}

VectorAnimationRenderer::~VectorAnimationRenderer()
{
}

VectorAnimationRenderer::VectorAnimationRenderer(Internal::Adaptor::VectorAnimationRenderer* internal)
: BaseHandle(internal)
{
}

VectorAnimationRenderer::VectorAnimationRenderer(const VectorAnimationRenderer& handle)
: BaseHandle(handle)
{
}

VectorAnimationRenderer& VectorAnimationRenderer::operator=(const VectorAnimationRenderer& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void VectorAnimationRenderer::Finalize()
{
}

bool VectorAnimationRenderer::Load(const std::string& url)
{
  return Internal::Adaptor::GetImplementation(*this).Load(url);
}

bool VectorAnimationRenderer::Load(const Dali::Vector<uint8_t>& data)
{
  return Internal::Adaptor::GetImplementation(*this).Load(data);
}

void VectorAnimationRenderer::SetRenderer(Renderer renderer)
{
  Internal::Adaptor::GetImplementation(*this).SetRenderer(renderer);
}

void VectorAnimationRenderer::SetSize(uint32_t width, uint32_t height)
{
  Internal::Adaptor::GetImplementation(*this).SetSize(width, height);
}

bool VectorAnimationRenderer::Render(uint32_t frameNumber)
{
  return Internal::Adaptor::GetImplementation(*this).Render(frameNumber);
}

void VectorAnimationRenderer::RenderStopped()
{
}

uint32_t VectorAnimationRenderer::GetTotalFrameNumber() const
{
  return Internal::Adaptor::GetImplementation(*this).GetTotalFrameNumber();
}

float VectorAnimationRenderer::GetFrameRate() const
{
  return Internal::Adaptor::GetImplementation(*this).GetFrameRate();
}

void VectorAnimationRenderer::GetDefaultSize(uint32_t& width, uint32_t& height) const
{
  Internal::Adaptor::GetImplementation(*this).GetDefaultSize(width, height);
}

void VectorAnimationRenderer::GetLayerInfo(Property::Map& map) const
{
}

bool VectorAnimationRenderer::GetMarkerInfo(const std::string& marker, uint32_t& startFrame, uint32_t& endFrame) const
{
  return Internal::Adaptor::GetImplementation(*this).GetMarkerInfo(marker, startFrame, endFrame);
}

void VectorAnimationRenderer::GetMarkerInfo(Property::Map& map) const
{
}

void VectorAnimationRenderer::InvalidateBuffer()
{
  Internal::Adaptor::GetImplementation(*this).InvalidateBuffer();
}

void VectorAnimationRenderer::AddPropertyValueCallback(const std::string& keyPath, VectorProperty property, CallbackBase* callback, int32_t id)
{
  Internal::Adaptor::GetImplementation(*this).AddPropertyValueCallback(keyPath, property, callback, id);
}

void VectorAnimationRenderer::RefreshDynamicProperty()
{
  Internal::Adaptor::GetImplementation(*this).RefreshDynamicProperty();
}

void VectorAnimationRenderer::KeepRasterizedBuffer()
{
  Internal::Adaptor::GetImplementation(*this).KeepRasterizedBuffer();
}

void VectorAnimationRenderer::SetEnableAspectFit(bool enable)
{
  Internal::Adaptor::GetImplementation(*this).SetEnableAspectFit(enable);
}

bool VectorAnimationRenderer::IsEnableAspectFit() const
{
  return Internal::Adaptor::GetImplementation(*this).IsEnableAspectFit();
}

VectorAnimationRenderer::UploadCompletedSignalType& VectorAnimationRenderer::UploadCompletedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).UploadCompletedSignal();
}

} // namespace Dali

namespace Test
{
namespace UiVectorAnimationRenderer
{
void ResetLastSize()
{
  gLastVectorAnimationWidth.store(0u);
  gLastVectorAnimationHeight.store(0u);
}

uint32_t GetLastWidth()
{
  return gLastVectorAnimationWidth.load();
}

uint32_t GetLastHeight()
{
  return gLastVectorAnimationHeight.load();
}

void ResetDynamicPropertyProbe()
{
  std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
  gLatestVectorAnimationRenderer = nullptr;
}

uint32_t GetDynamicPropertyCount()
{
  std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
  DALI_ASSERT_ALWAYS(gLatestVectorAnimationRenderer && "No vector animation renderer is available");
  return gLatestVectorAnimationRenderer->GetDynamicPropertyCount();
}

uint32_t GetLastRenderedFrame()
{
  std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
  DALI_ASSERT_ALWAYS(gLatestVectorAnimationRenderer && "No vector animation renderer is available");
  return gLatestVectorAnimationRenderer->GetLastRenderedFrame();
}

uint32_t GetDynamicPropertyRefreshCount()
{
  std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
  DALI_ASSERT_ALWAYS(gLatestVectorAnimationRenderer && "No vector animation renderer is available");
  return gLatestVectorAnimationRenderer->GetDynamicPropertyRefreshCount();
}

Dali::Property::Value GetLastEvaluatedDynamicProperty(
  const std::string&                            keyPath,
  Dali::VectorAnimationRenderer::VectorProperty property)
{
  std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
  DALI_ASSERT_ALWAYS(gLatestVectorAnimationRenderer && "No vector animation renderer is available");
  return gLatestVectorAnimationRenderer->GetLastEvaluatedDynamicProperty(keyPath, property);
}

uint32_t GetDynamicPropertyCount(const std::string&                            keyPath,
                                 Dali::VectorAnimationRenderer::VectorProperty property)
{
  std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
  DALI_ASSERT_ALWAYS(gLatestVectorAnimationRenderer && "No vector animation renderer is available");
  return gLatestVectorAnimationRenderer->GetDynamicPropertyCount(keyPath, property);
}

Dali::Property::Value EvaluateDynamicProperty(const std::string&                            keyPath,
                                              Dali::VectorAnimationRenderer::VectorProperty property,
                                              uint32_t                                      frameNumber)
{
  std::lock_guard<std::mutex> lock(gDynamicPropertyProbeMutex);
  DALI_ASSERT_ALWAYS(gLatestVectorAnimationRenderer && "No vector animation renderer is available");
  return gLatestVectorAnimationRenderer->EvaluateDynamicProperty(keyPath, property, frameNumber);
}
} // namespace UiVectorAnimationRenderer
} // namespace Test
