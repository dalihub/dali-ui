#ifndef DALI_UI_IMAGE_CACHE_UTILS_H
#define DALI_UI_IMAGE_CACHE_UTILS_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
/**
 * @brief Utilities for managing image resources cached by visuals.
 */
namespace ImageCacheUtils
{
/**
 * @brief Removes unused image resources retained by Image::ReleasePolicy::NEVER.
 *
 * This operation affects all unused resources retained by the current visual factory, not resources belonging to a
 * specific visual. Resources that are still referenced by visuals are not removed. Removed resources are loaded again
 * when they are requested later.
 *
 * Removal is processed after pending visual resource releases. The underlying graphics allocator may retain released
 * memory, so process or GPU memory usage is not guaranteed to decrease immediately.
 *
 * @note Call this function only when eviction of all unused resources retained by Image::ReleasePolicy::NEVER is
 * intended.
 */
DALI_UI_API void ClearUnusedTextures();

} // namespace ImageCacheUtils
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_IMAGE_CACHE_UTILS_H
