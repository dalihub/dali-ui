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
// test binary, ahead of libdali2-adaptor.so on the link line) lets LottieAnimationVisual /
// SelectableLottieAnimationView tests (and anything that transitively touches them, e.g.
// CheckBox's default icon style) run without depending on thorvg being present at all, matching
// the approach already used by dali-toolkit's
// dali-toolkit-test-utils/toolkit-vector-animation-renderer.cpp.

#include <dali/public-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/object/base-object.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <memory>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class VectorAnimationRenderer : public Dali::BaseObject
{
public:
  VectorAnimationRenderer()
  : mEventThreadCallback(new EventThreadCallback(MakeCallback(this, &VectorAnimationRenderer::OnTriggered)))
  {
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

    if(!mDynamicPropertyCallbacks.empty())
    {
      for(auto&& dynamicPropertyCallbackPair : mDynamicPropertyCallbacks)
      {
        CallbackBase::ExecuteReturn<Property::Value>(*dynamicPropertyCallbackPair.second, dynamicPropertyCallbackPair.first, 0, frameNumber);
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
    mDynamicPropertyCallbacks.emplace_back(id, std::unique_ptr<CallbackBase>(callback));
  }

  void RefreshDynamicProperty()
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    for(auto&& dynamicPropertyCallbackPair : mDynamicPropertyCallbacks)
    {
      CallbackBase::ExecuteReturn<Property::Value>(*dynamicPropertyCallbackPair.second, dynamicPropertyCallbackPair.first, 0, mPreviousFrame);
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
      Dali::PixelData    pixelData   = Dali::PixelBuffer::Convert(pixelBuffer);
      texture.Upload(pixelData);
    }

    mUploadCompletedSignal.Emit();
  }

public:
  Dali::Renderer                                                 mRenderer;
  Dali::Mutex                                                    mMutex;
  std::vector<std::pair<int32_t, std::unique_ptr<CallbackBase>>> mDynamicPropertyCallbacks;

  uint32_t mWidth{0};
  uint32_t mHeight{0};
  uint32_t mDefaultWidth{0};
  uint32_t mDefaultHeight{0};
  uint32_t mTotalFrameNumber{5};
  uint32_t mPreviousFrame{0};
  float    mFrameRate{60.0f};
  bool     mLoadFailed{false};
  bool     mResourceReady{false};
  bool     mNeedTrigger{true};
  bool     mEnableFixedCache{false};
  bool     mEnableAspectFit{true};

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
