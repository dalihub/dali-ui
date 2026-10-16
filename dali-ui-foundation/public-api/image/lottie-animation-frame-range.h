#pragma once

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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace LottieAnimation
{

/**
 * @brief An inclusive integer frame span [startFrame, endFrame].
 *
 * This fixed-size value stores the supplied frame numbers without normalizing or
 * clamping them. The consumer applies the rules appropriate to playback or color
 * binding. The two-int32_t layout is fixed; no virtual functions or data members
 * may be added.
 */
class DALI_UI_API FrameRange
{
public:
  /**
   * @brief Creates a frame range [0, 0].
   */
  constexpr FrameRange() = default;

  /**
   * @brief Creates a frame range with the supplied endpoints.
   * @param[in] start The first frame of the segment
   * @param[in] end The last frame of the segment
   */
  constexpr FrameRange(int32_t start, int32_t end)
  : mStartFrame(start),
    mEndFrame(end)
  {
  }

  /**
   * @brief Returns the first frame of the segment.
   * @return The start frame
   */
  constexpr int32_t GetStartFrame() const
  {
    return mStartFrame;
  }

  /**
   * @brief Returns the last frame of the segment.
   * @return The end frame
   */
  constexpr int32_t GetEndFrame() const
  {
    return mEndFrame;
  }

private:
  int32_t mStartFrame{0};
  int32_t mEndFrame{0};
};

} // namespace LottieAnimation
} // namespace Ui
} //namespace DALI_NAMESPACE
