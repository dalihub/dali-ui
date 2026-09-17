/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include "image-capture.h"
#include <unistd.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <utility>

namespace TextManualTest
{
ImageCapture::~ImageCapture()
{
  Cancel();
}

bool ImageCapture::Request(Dali::Actor source, Dali::Vector2 position, Dali::Vector2 size, std::string& error)
{
  error.clear();
  if(mPending || mReady)
  {
    error = "capture result still pending or unconsumed";
    return false;
  }
  if(!source || !source.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE) ||
     !std::isfinite(position.x) || !std::isfinite(position.y) ||
     !std::isfinite(size.x) || !std::isfinite(size.y) || size.x < 1 || size.y < 1 ||
     size.x > 4096 || size.y > 4096 || size.x != std::floor(size.x) || size.y != std::floor(size.y))
  {
    error = "capture needs an on-scene actor and finite, integral pixel dimensions (1..4096)";
    return false;
  }
  Cancel();
  mResult                 = {};
  char        directory[] = "/tmp/dali-text-capture-XXXXXX";
  const char* created     = mkdtemp(directory);
  if(!created)
  {
    error = "cannot create private capture scratch directory";
    return false;
  }
  mScratchDirectory = created;
  mScratchFile      = mScratchDirectory + "/capture.png";
  mResult.requested = Clock::now();
  mWidth            = static_cast<uint32_t>(size.x);
  mHeight           = static_cast<uint32_t>(size.y);
  try
  {
    mCapture = Dali::Capture::New();
    mCapture.SetExclusive(false);
    mCapture.FinishedSignal().Connect(this, &ImageCapture::OnFinished);
    mPending = true;
    // The public PNG export works without NativeImage::SetPixels/GetPixels
    // support, which is absent on some supported desktop/target backends.
    mCapture.Start(source, position, size, mScratchFile.c_str(), Dali::Vector4(1, 1, 1, 1));
  }
  catch(const Dali::DaliException& exception)
  {
    error = std::string("Capture unavailable: ") + exception.condition;
    Cancel();
    return false;
  }
  return true;
}

void ImageCapture::OnFinished(Dali::Capture capture, Dali::Capture::FinishState state)
{
  if(!mPending || capture != mCapture) return;
  mResult.completed = Clock::now();
  if(state != Dali::Capture::FinishState::SUCCEEDED)
    mResult.error = "Capture failed or timed out";
  else
  {
    if(ImageCheck::LoadPng(mScratchFile, mResult.image, mResult.error) &&
       (mResult.image.width != mWidth || mResult.image.height != mHeight))
      mResult.error = "readback dimensions differ from requested capture";
  }
  mPending = false;
  mReady   = true;
}

bool ImageCapture::Take(Result& result)
{
  if(!mReady) return false;
  result = std::move(mResult);
  Cancel();
  return true;
}

void ImageCapture::Cancel()
{
  DisconnectAll();
  mCapture.Reset(); // Capture may finish internally; detached callbacks cannot access the TC.
  mPending = mReady = false;
  mResult           = {};
  if(!mScratchFile.empty()) std::remove(mScratchFile.c_str());
  if(!mScratchDirectory.empty()) rmdir(mScratchDirectory.c_str());
  mScratchFile.clear();
  mScratchDirectory.clear();
}
} // namespace TextManualTest
