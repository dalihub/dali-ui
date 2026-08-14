#ifndef DALI_UI_IMAGE_URL_TRACKER_H
#define DALI_UI_IMAGE_URL_TRACKER_H

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

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Dali
{
namespace Ui
{
namespace Internal
{
/**
 * Tracks active application-owned ImageUrls for file/remote image resources.
 *
 * This object is event-thread confined, consistently with other Dali handles.
 * It deliberately does not know about type-specific cache resource IDs.
 */
class ImageUrlTracker
{
public:
  /**
   * @return True when this is the first active ImageUrl for @p url.
   */
  bool TrackImageUrl(const std::string& url)
  {
    auto&      count           = mImageUrlCounts[url];
    const bool isFirstImageUrl = (count == 0u);
    ++count;
    return isFirstImageUrl;
  }

  /**
   * @return True when this was the last active ImageUrl for @p url.
   */
  bool UntrackImageUrl(const std::string& url)
  {
    auto iter = mImageUrlCounts.find(url);
    if(iter == mImageUrlCounts.end())
    {
      return false;
    }

    if(--iter->second == 0u)
    {
      mImageUrlCounts.erase(iter);
      return true;
    }
    return false;
  }

  bool HasActiveImageUrl(const std::string& url) const
  {
    return mImageUrlCounts.find(url) != mImageUrlCounts.end();
  }

private:
  std::unordered_map<std::string, uint32_t> mImageUrlCounts;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_IMAGE_URL_TRACKER_H
