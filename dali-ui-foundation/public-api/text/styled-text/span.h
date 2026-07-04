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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

namespace Text
{

/**
 * @brief Base handle for styled text span payload objects.
 *
 * Span only represents the payload. Text range attachment is managed by
 * StyledTextBuilder and StyledText.
 */
class DALI_UI_API Span : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized Span handle.
   */
  Span() = default;

  /**
   * @brief Downcasts a handle to Span.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized Span on success, otherwise empty
   */
  static Span DownCast(BaseHandle handle);

protected:
  /**
   * @brief Creates a Span handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL Span(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
