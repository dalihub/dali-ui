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

#pragma once
#include "image-check.h"

#include <dali/public-api/adaptor-framework/capture.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <chrono>

namespace TextManualTest
{
// One request at a time. Poll Take() from a TC timer; no user callback is
// invoked inside Capture's signal (in particular, no reentrant Start()).
// The TC owns render-readiness, a timeout, and the expected screen geometry.
class ImageCapture : public Dali::ConnectionTracker
{
public:
  using Clock = std::chrono::steady_clock;
  struct Result
  {
    ImageCheck::Image image;
    std::string       error;
    Clock::time_point requested, completed;
    bool              Ok() const
    {
      return error.empty() && image.IsValid();
    }
  };
  ~ImageCapture() override;
  bool Request(Dali::Actor source, Dali::Vector2 screenTopLeft, Dali::Vector2 size, std::string& error);
  bool Take(Result& result);
  bool IsPending() const
  {
    return mPending;
  }
  void Cancel();

private:
  void          OnFinished(Dali::Capture capture, Dali::Capture::FinishState state);
  Dali::Capture mCapture;
  std::string   mScratchDirectory, mScratchFile;
  Result        mResult;
  uint32_t      mWidth{0}, mHeight{0};
  bool          mPending{false}, mReady{false};
};
} // namespace TextManualTest
