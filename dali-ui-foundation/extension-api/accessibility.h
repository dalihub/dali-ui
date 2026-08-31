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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/types/callback.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
namespace Accessibility
{
/**
 * @brief Callback invoked when the direct-reading status changes.
 *
 * The status string reference is valid only for the duration of the callback.
 */
using ReadingStatusCallback = Callback<void(const Dali::String&)>;

/**
 * @brief Sets whether screen-reader output is suppressed.
 *
 * @param[in] suppressed True to suppress screen-reader output, false to allow it
 * @return True if the request was forwarded to an accessibility bridge
 */
DALI_UI_API bool SetScreenReaderSuppressed(bool suppressed);

/**
 * @brief Requests direct reading of the supplied text.
 *
 * @param[in] text The text to read
 * @param[in] discardable True if a later request may discard this reading
 * @param[in] callback Optional callback that receives direct-reading status strings
 */
DALI_UI_API void SayAccessibilityText(
  const Dali::String&   text,
  bool                  discardable,
  ReadingStatusCallback callback = {});

} // namespace Accessibility
} // namespace Extension
} // namespace Ui
} // namespace Dali
